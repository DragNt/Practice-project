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
#include <Poco/Mutex.h>
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
    Poco::ThreadPool threadPool_;
    Poco::FastMutex mutex_;

    vector<string> read_url();
    void processURL(const string &url);
    string get_filename_from_url(const string &url);
    string get_filename_from_content_disposition(const string &content_disposition);
    string sanitize_filename(const string &filename);
    string generate_unique_filename(const string &filename);
    void log(const string &message);
};

class DownloadTask : public Poco::Runnable
{
public:
    DownloadTask(Downloader &downloader, const string &url);
    void run() override;
    Poco::ThreadPool threadPool_;

private:
    Downloader &downloader_;
    string url_;
};

#endif // DOWNLOADER_H
