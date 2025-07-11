#include "downloader.h"
#include <iostream>

using namespace std;

Downloader::Downloader(const string &url_file, const string &output_dir, int max_parallel)
    : url_file_(url_file), output_dir_(output_dir), max_parallel_(max_parallel)
{
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

void Downloader::run()
{
}