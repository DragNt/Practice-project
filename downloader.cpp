#include "downloader.h"
#include <iostream>

using namespace std;

Downloader::Downloader(const string &url_file, const string &output_dir, int max_parallel)
    : url_file_(url_file), output_dir_(output_dir), max_parallel_(max_parallel)
{
}

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