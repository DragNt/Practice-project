#ifndef DOWNLOADER_H
#define DOWNLOADER_H
#include <iostream>

using namespace std;

class Downloader
{
public:
    Downloader(const string &url_file, const string &output_dir, int max_parallel);
    ~Downloader();

    void run();

private:
    string url_file_;
    string output_dir_;
    int max_parallel_;

    string Downloader::get_filename_from_url(const string &url);
};

#endif // DOWNLOADER_H