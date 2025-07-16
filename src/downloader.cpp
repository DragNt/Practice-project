#include "downloader.h"
#include <Poco/NumberFormatter.h>
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

void Downloader::run()
{
}