#include "downloader.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        cerr << "Usage: " << argv[0] << " <url_file> <output_dir> <max_parallel>" << endl;
        return 1;
    }
    try
    {
        string url_file = argv[1];
        string output_dir = argv[2];
        int max_parallel = stoi(argv[3]);

        if (max_parallel < 1 || max_parallel > 999)
        {
            cerr << "max_parallel must be between 1 and 999" << endl;
            return 1;
        }

        Downloader downloader(url_file, output_dir, max_parallel);
        downloader.run();
    }
    catch (Poco::Exception &e)
    {
        cerr << "Error: " + e.displayText() << endl;
    }
    return 0;
}