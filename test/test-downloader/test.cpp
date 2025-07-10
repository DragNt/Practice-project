#include "pch.h"
#include "C:\\Users\\User\\Desktop\\Practice-progect\\Practice-project\\downloader.h"
#include "gtest/gtest.h"

using namespace std;

TEST(DownloaderTest, TestSanitizeFilename) {
    Downloader d("", "", 1);
    EXPECT_EQ(d.sanitize_filename("normal.txt"), "normal.txt");
    EXPECT_EQ(d.sanitize_filename("file$name.jpg"), "file_name.jpg");
    EXPECT_EQ(d.sanitize_filename("UPPER&lower.png"), "UPPER_lower.png");
}

TEST(DownloaderTest, TestGetFilenameFromURL) {
    Downloader d("", "", 1);
    EXPECT_EQ(d.get_filename_from_url("http://example.com/file.txt"), "file.txt");
    EXPECT_EQ(d.get_filename_from_url("http://example.com/path/to/image.jpg"), "image.jpg");
    EXPECT_EQ(d.get_filename_from_url("http://example.com/"), "file");
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}