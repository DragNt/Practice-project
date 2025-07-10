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

    // struct DowloadResult
    // {
    //     string url;
    //     bool success;
    //     string message;
    //     string filename;
    // };

    string url_file_;
    string output_dir_;
    int max_parallel_;

    string get_filename_from_url(const string &url);
    string sanitize_filename(const string &filename);
};

#endif // DOWNLOADER_H