#ifndef IFILEIO_HPP
#define IFILEIO_HPP

#include <cstdint>
#include <ios>

class IFileIO {
  public:
    virtual void Read(char *buf, int64_t stream_size) = 0;
    virtual void WriteTemp(char *buf, int64_t stream_size) = 0;
    virtual auto CommitTemp() -> int = 0;

    virtual auto OpenRead() -> int = 0;
    virtual auto OpenWriteTemp() -> int = 0;

    virtual void CloseRead() = 0;
    virtual void CloseWriteTemp() = 0;

    virtual auto GetPositionRead() -> int64_t = 0;
    virtual auto GetPositionWriteTemp() -> int64_t = 0;

    virtual auto GetExists(bool temp) -> bool = 0;
    virtual auto Delete(bool temp) -> bool = 0; 
};

#endif // IFILEIO_HPP
