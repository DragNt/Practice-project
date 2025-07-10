#include "downloader.h"
#include <iostream>

using namespace std;

Downloader::Downloader(const string &url_file, const string &output_dir, int max_parallel)
    : url_file_(url_file), output_dir_(output_dir), max_parallel_(max_parallel)
{
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

    return filename;
}

void Downloader::run()
{
}