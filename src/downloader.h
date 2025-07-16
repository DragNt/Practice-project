#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <Poco/File.h>
#include <Poco/Path.h>

using namespace std;

class Downloader
{
public:
    Downloader(const string &url_file, const string &output_dir, int max_parallel);
    ~Downloader();

    void run();

    string url_file_;
    string output_dir_;
    int max_parallel_;

    vector<string> read_url();
    string get_filename_from_url(const string &url);
    string sanitize_filename(const string &filename);
    string generate_unique_filename(const string &filename);
};

#endif // DOWNLOADER_H
