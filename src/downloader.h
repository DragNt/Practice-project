#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <string>
#include <vector>
#include <algorithm>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/URI.h>
#include <Poco/File.h>
#include <Poco/Path.h>
#include <Poco/Exception.h>
#include <Poco/ThreadPool.h>
#include <Poco/Runnable.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/DateTimeFormat.h>
#include <memory>

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
    string get_filename_from_content_disposition(const string &content_disposition);
    string sanitize_filename(const string &filename);
    string generate_unique_filename(const string &filename);
    void log(const string &message)

        Poco::ThreadPool threadPool_;
};

class DownloadTask : public Poco::Runnable
{
public:
    DownloadTask(Downloader &downloader, const std::string &url);
    void run() override;

private:
    Downloader &downloader_;
    std::string url_;
};

#endif // DOWNLOADER_H
