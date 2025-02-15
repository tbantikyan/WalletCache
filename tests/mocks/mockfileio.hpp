#ifndef MOCKFILEIO_HPP
#define MOCKFILEIO_HPP

#include "ifileio.hpp"

#include <gmock/gmock.h>

class MockFileIO : public IFileIO {
  public:
    ~MockFileIO() override = default;

    MOCK_METHOD(bool, Read, (char *buf, int64_t stream_size), (override));
    MOCK_METHOD(bool, WriteTemp, (const char *buf, int64_t stream_size), (override));
    MOCK_METHOD(int, CommitTemp, (), (override));

    MOCK_METHOD(int, OpenRead, (), (override));
    MOCK_METHOD(int, OpenWriteTemp, (), (override));

    MOCK_METHOD(void, CloseRead, (), (override));
    MOCK_METHOD(void, CloseWriteTemp, (), (override));

    MOCK_METHOD(int64_t, GetPositionRead, (), (override));
    MOCK_METHOD(int64_t, GetPositionWriteTemp, (), (override));

    MOCK_METHOD(uintmax_t, GetSize, (bool temp), (override));
    MOCK_METHOD(bool, GetExists, (bool temp), (override));
    MOCK_METHOD(bool, Delete, (bool temp), (override));
};

#endif // MOCKFILEIO_HPP
