#include "fstreamfileio.hpp"

#include <chrono>
#include <filesystem>
#include <gtest/gtest.h>
#include <thread>

class FStreamFileIOTest : public ::testing::Test {
  protected:
    std::string test_dir_;
    std::string file_path_;
    std::string tmp_file_path_;
    std::string bak_file_path_;
    std::string init_write_ = "test";

    void SetUp() override {
        const std::string test_name = ::testing::UnitTest::GetInstance()->current_test_info()->name();
        test_dir_ = "fstreamfileio_test_" + test_name;

        std::filesystem::create_directory(test_dir_);
        file_path_ = test_dir_ + "/test_file";
        tmp_file_path_ = file_path_ + ".tmp";
        bak_file_path_ = file_path_ + ".bak";
    }

    void TearDown() override {
        int attempts = 3;
        while (attempts-- > 0) {
            try {
                std::filesystem::remove_all(test_dir_);
                break;
            } catch (...) {
                if (attempts == 0) {
                    throw;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }

    auto TestDeleteFile(FStreamFileIO &file_io, const std::string &path) -> bool { return file_io.DeleteFile(path); }

    void ExpectCommitTempNoMain(FStreamFileIO &file_io) {
        EXPECT_EQ(file_io.OpenWriteTemp(), 0);
        file_io.WriteTemp(init_write_.c_str(), init_write_.size());
        file_io.CloseWriteTemp();

        // Only temp file should exist
        EXPECT_FALSE(file_io.GetExists(false));
        EXPECT_TRUE(file_io.GetExists(true));

        EXPECT_EQ(file_io.CommitTemp(), 0);
        EXPECT_TRUE(file_io.GetExists(false));
        EXPECT_FALSE(file_io.GetExists(true));
    }
};

// OpenWriteTemp
TEST_F(FStreamFileIOTest, OpenWriteTemp_ValidPath_Returns0) {
    FStreamFileIO file_io(file_path_);
    EXPECT_EQ(file_io.OpenWriteTemp(), 0);
    file_io.CloseWriteTemp();
}

// WriteTemp
TEST_F(FStreamFileIOTest, WriteTemp_ValidStream_ReturnsTrue) {
    FStreamFileIO file_io(file_path_);
    EXPECT_EQ(file_io.OpenWriteTemp(), 0);
    EXPECT_TRUE(file_io.WriteTemp("test", 4));
    file_io.CloseWriteTemp();
}

TEST_F(FStreamFileIOTest, WriteTemp_NoOpenStream_ReturnsFalse) {
    FStreamFileIO file_io(file_path_);
    EXPECT_FALSE(file_io.WriteTemp("test", 4));
}

// CommitTemp
TEST_F(FStreamFileIOTest, CommitTemp_NoExistingFile_Returns0) {
    FStreamFileIO file_io(file_path_);

    ExpectCommitTempNoMain(file_io);
}

TEST_F(FStreamFileIOTest, CommitTemp_ExistingFile_Returns0) {
    FStreamFileIO file_io(file_path_);

    ExpectCommitTempNoMain(file_io);

    // recreate temp
    EXPECT_EQ(file_io.OpenWriteTemp(), 0);
    file_io.WriteTemp("new", 3);
    file_io.CloseWriteTemp();
    EXPECT_TRUE(file_io.GetExists(false));
    EXPECT_TRUE(file_io.GetExists(true));

    EXPECT_EQ(file_io.CommitTemp(), 0);
    EXPECT_TRUE(file_io.GetExists(false));
    EXPECT_FALSE(file_io.GetExists(true));
}

TEST_F(FStreamFileIOTest, CommitTemp_RenameMainToBakFails_ReturnsNegative1) {
    FStreamFileIO file_io(file_path_);
    ExpectCommitTempNoMain(file_io);

    // make bak a directory so renaming main to bak fails
    std::filesystem::create_directory(bak_file_path_);

    EXPECT_EQ(file_io.OpenWriteTemp(), 0);
    file_io.WriteTemp("new", 3);
    file_io.CloseWriteTemp();
    EXPECT_EQ(file_io.CommitTemp(), -1);

    EXPECT_TRUE(file_io.GetExists(false));
    EXPECT_FALSE(file_io.GetExists(true)); // temp should be deleted
    std::filesystem::remove_all(bak_file_path_);
}

// OpenRead
TEST_F(FStreamFileIOTest, OpenRead_ExistingFile_Returns0) {
    FStreamFileIO file_io(file_path_);
    ExpectCommitTempNoMain(file_io);

    EXPECT_TRUE(file_io.GetExists(false));
    EXPECT_EQ(file_io.OpenRead(), 0);
    file_io.CloseRead();
}

TEST_F(FStreamFileIOTest, OpenRead_NonExistentFile_ReturnsNegative1) {
    FStreamFileIO file_io(file_path_);
    EXPECT_FALSE(file_io.GetExists(false));
    EXPECT_EQ(file_io.OpenRead(), -1);
}

// Read & GetPositionRead
TEST_F(FStreamFileIOTest, Read_ValidStream_ReturnsTrue) {
    FStreamFileIO file_io(file_path_);
    ExpectCommitTempNoMain(file_io);

    EXPECT_EQ(file_io.OpenRead(), 0);
    char buf[init_write_.size() + 1];
    EXPECT_TRUE(file_io.Read(buf, init_write_.size()));
    EXPECT_EQ(file_io.GetPositionRead(), init_write_.size());
    file_io.CloseRead();
    buf[init_write_.size()] = 0;

    EXPECT_EQ(memcmp(buf, init_write_.c_str(), init_write_.size()), 0);
}

TEST_F(FStreamFileIOTest, Read_InvalidStream_ReturnsFalse) {
    FStreamFileIO file_io(file_path_);
    char buf[10];
    EXPECT_FALSE(file_io.Read(buf, 10));
}

// GetPositionWriteTemp
TEST_F(FStreamFileIOTest, GetPositionWriteTemp_AfterWrite_ReturnsCorrectPosition) {
    FStreamFileIO file_io(file_path_);
    file_io.OpenWriteTemp();
    file_io.WriteTemp("test", 4);
    EXPECT_EQ(file_io.GetPositionWriteTemp(), 4);
    file_io.CloseWriteTemp();
}

// GetSize
TEST_F(FStreamFileIOTest, GetSize_ExistingFile_ReturnsCorrectSize) {
    FStreamFileIO file_io(file_path_);
    EXPECT_EQ(file_io.GetSize(false), 0);
    EXPECT_EQ(file_io.GetSize(true), 0);

    EXPECT_EQ(file_io.OpenWriteTemp(), 0);
    file_io.WriteTemp(init_write_.c_str(), init_write_.size());
    file_io.CloseWriteTemp();

    EXPECT_TRUE(file_io.GetExists(true));

    EXPECT_EQ(file_io.GetSize(false), 0);
    EXPECT_EQ(file_io.GetSize(true), init_write_.size());

    EXPECT_EQ(file_io.CommitTemp(), 0);
    EXPECT_TRUE(file_io.GetExists(false));

    EXPECT_EQ(file_io.GetSize(false), init_write_.size());
    EXPECT_EQ(file_io.GetSize(true), 0);
}

// GetExists
TEST_F(FStreamFileIOTest, GetExists_FilePresent_ReturnsTrue) {
    FStreamFileIO file_io(file_path_);
    EXPECT_FALSE(file_io.GetExists(false));
    EXPECT_FALSE(file_io.GetExists(true));

    EXPECT_EQ(file_io.OpenWriteTemp(), 0);
    file_io.WriteTemp(init_write_.c_str(), init_write_.size());
    file_io.CloseWriteTemp();

    EXPECT_FALSE(file_io.GetExists(false));
    EXPECT_TRUE(file_io.GetExists(true));

    EXPECT_EQ(file_io.CommitTemp(), 0);

    EXPECT_TRUE(file_io.GetExists(false));
    EXPECT_FALSE(file_io.GetExists(true));
}

TEST_F(FStreamFileIOTest, GetExists_FileMissing_ReturnsFalse) {
    FStreamFileIO file_io(file_path_);
    EXPECT_FALSE(file_io.GetExists(false));
    EXPECT_FALSE(file_io.GetExists(true));
}

// DeleteFile
TEST_F(FStreamFileIOTest, DeleteFile_ExistingFile_ReturnsTrue) {
    FStreamFileIO file_io(file_path_);
    EXPECT_EQ(file_io.OpenWriteTemp(), 0);
    file_io.WriteTemp(init_write_.c_str(), init_write_.size());
    file_io.CloseWriteTemp();
    EXPECT_TRUE(TestDeleteFile(file_io, tmp_file_path_));
}

TEST_F(FStreamFileIOTest, DeleteFile_NonExistentFile_ReturnsFalse) {
    FStreamFileIO file_io(file_path_);
    EXPECT_FALSE(TestDeleteFile(file_io, file_path_));
}

// Delete
TEST_F(FStreamFileIOTest, Delete_ExistingFile_ReturnsTrue) {
    FStreamFileIO file_io(file_path_);
    EXPECT_FALSE(file_io.Delete(false));
    EXPECT_FALSE(file_io.Delete(true));

    EXPECT_EQ(file_io.OpenWriteTemp(), 0);
    file_io.WriteTemp(init_write_.c_str(), init_write_.size());
    file_io.CloseWriteTemp();
    EXPECT_FALSE(file_io.Delete(false));
    EXPECT_TRUE(file_io.Delete(true));

    ExpectCommitTempNoMain(file_io);
    EXPECT_TRUE(file_io.Delete(false));
    EXPECT_FALSE(file_io.Delete(true));
}
