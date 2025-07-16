#include "downloader.h"
#include <fstream>
#include <Poco/StreamCopier.h>
#include <Poco/NumberFormatter.h>
#include <Poco/DirectoryIterator.h>
#include <Poco/Net/Context.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/SSLManager.h>
#include <Poco/Net/AcceptCertificateHandler.h>
using namespace std;

Downloader::Downloader(const string &url_file, const string &output_dir, int max_parallel)
    : url_file_(url_file), output_dir_(output_dir), max_parallel_(max_parallel), treadPool_(1, max_parallel)
{
    // Инициализация SSL (для HTTPS)
    Poco::Net::initializeSSL();
    Poco::SharedPtr<Poco::Net::InvalidCertificateHandler> certHandler =
        new Poco::Net::AcceptCertificateHandler(false);
    Poco::Net::Context::Ptr context =
        new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "", "", "",
                               Poco::Net::Context::VERIFY_NONE, 9, false,
                               "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
    Poco::Net::SSLManager::instance().initializeClient(0, certHandler, context);
}

Downloader::~Downloader()
{
    Poco::Net::uninitializeSSL();
}

// Чтение URL из файла построчно
vector<string> Downloader::read_url()
{
    ifstream file(url_file_);
    if (!file.is_open())
    {
        throw runtime_error("Failed to open URL file " + url_file_);
    }

    vector<string> urls;
    string line;
    while (getline(file, line))
    {
        // Удаление символа возврата каретки (для кросс-платформенности)
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());

        if (!line.empty())
        {
            urls.push_back(line);
        }
    }

    return urls;
}

void Downloader::run()
{
    log("Program started");
    log("Input parameters:");
    log("  URL file: " + url_file_);
    log("  Output directory: " + output_dir_);
    log("  Concurrency: " + Poco::NumberFormatter::format(max_parallel));

    // Создаёт выходной каталог, если он не существует
    Poco::File outputDir(output_dir_);
    if (!outputDir.exists())
    {
        outputDir.createDirectories();
    }

    // Чтение URL-адресов из файла
    vector<string> urls = read_url();

    for (const auto &url : urls)
    {
        threadPool_.start(new DownloadTask(*this, url));
    }

    threadPool_.joinAll();
    log("Program finished");
}

void Downloader::processURL(const std::string &url)
{
    log("Starting download: " + url);

    try
    {
        Poco::URI uri(url);
        string path(uri.getPathAndQuery());
        if (path.empty())
            path = "/";

        Poco::Net::HTTPClientSession *session = nullptr;
        if (uri.getScheme() == "https")
        {
            session = new Poco::Net::HTTPSClientSession(uri.getHost(), uri.getPort());
        }
        else
        {
            session = new Poco::Net::HTTPClientSession(uri.getHost(), uri.getPort());
        }

        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, path,
                                       Poco::Net::HTTPMessage::HTTP_1_1);
        Poco::Net::HTTPResponse response;

        session->sendRequest(request);
        istream &rs = session->receiveResponse(response);

        if (response.getStatus() != Poco::Net::HTTPResponse::HTTP_OK)
        {
            log("Error: Server returned status code: " +
                Poco::NumberFormatter::format(response.getStatus()) +
                " for URL: " + url);
            delete session;
            return;
        }

        // Определяет имя файла
        string filename;
        if (response.has("Content-Disposition"))
        {
            filename = get_filename_from_content_disposition(response.get("Content-Disposition"));
        }
        if (filename.empty())
        {
            filename = get_filename_from_url(uri);
        }
        filename = sanitize_filename(filename);
        filename = generate_unique_filename(filename);

        // Сохранение файла
        string fullPath = Poco::Path(output_dir_).append(filename).toString();
        ofstream file(fullPath, ios::binary);
        if (file.is_open())
        {
            Poco::StreamCopier::copyStream(rs, file);
            file.close();
            log("Finished download: " + url + " -> " + fullPath);
        }
        else
        {
            log("Error: Could not open file for writing: " + fullPath);
        }

        delete session;
    }
    catch (Poco::Exception &e)
    {
        log("Error: " + e.displayText() + " for URL: " + url);
    }
}

// Очищает имя файла от от недопустимых символов
string Downloader::sanitize_filename(const string &filename)
{
    string result;
    for (char c : filename)
    {
        if (isalnum(c) || c == '.' || c == '-' || c == '_')
        {
            result += c;
        }
        else
        {
            result += '_';
        }
    }
    return result;
}

// Функция для извлечения имени файла из HTTP заголовка
string Downloader::get_filename_from_content_disposition(const string &content_disposition)
{
    const string filename_prefix = "filename=";
    size_t pos = content_disposition.find(filename_prefix);
    if (pos != string::npos)
    {
        pos += filename_prefix.length();
        // Обработка имени в кавычках
        if (content_disposition[pos] == '"')
        {
            pos++;
            size_t end_pos = content_disposition.find('"', pos);
            if (end_pos != string::npos)
                return content_disposition.substr(pos, end_pos);
        }
        // Обработка имени без кавычек
        else
        {
            size_t end_pos = content_disposition.find(';', pos);
            if (end_pos == string::npos)
                end_pos = content_disposition.length();

            return content_disposition.substr(pos, end_pos - pos);
        }
    }
    return "";
}

// Извлекает имя файла из URL
string Downloader::get_filename_from_url(const string &url)
{
    size_t last_slash = url.find_last_of('/');
    if (last_slash == string::npos)
    {
        return "file";
    }

    string filename = url.substr(last_slash + 1);
    if (filename.empty())
    {
        return "file";
    }

    return sanitize_filename(filename);
}

string Downloader::generate_unique_filename(const string &filename)
{
    string base_name;
    string extension;
    size_t dot_pos = filename.find_last_of('.');
    if (dot_pos != string::npos)
    {
        base_name = filename.substr(0, dot_pos);
        extension = filename.substr(dot_pos);
    }
    else
    {
        base_name = filename;
    }

    string result = filename;
    int counter = 1;

    while (Poco::File(Poco::Path(output_dir_).append(result)).exists())
    {
        result = base_name + " (" + Poco::NumberFormatter::format(counter) + ") " + extension;
        counter++;
    }

    return result;
}

void Downloader::log(const string &message)
{
    cout << Poco::DateTimeFormatter::format(
                Poco::Timestamp(),
                "%Y-%m-%d %H:%M:%S.%i")
         << " - " << message << endl;
}

DownloadTask::DownloadTask(Downloader &downloader, const std::string &url)
    : downloader_(downloader), url_(url) {}

void DownloadTask::run()
{
    downloader_.processURL(url_);
}