#pragma once
#include <iostream>
#include <string>

namespace X
{

struct Buffer
{
    char * buf;
    char * begin;
    char * end;
    char * read_end;
    char * pos;

    Buffer(size_t buffer_size)
        : buf(new char[buffer_size])
        , begin(buf)
        , end(buf + buffer_size)
        , read_end(buf)
        , pos(buf)
    {
    }

    size_t totalSize() const
    {
        return end - begin;
    }

    size_t pendingData() const
    {
        return read_end - pos;
    }

    bool allRead() const
    {
        return pos >= read_end;
    }

    ~Buffer()
    {
        delete [] buf;
    }
};

class SentenceSplitter
{
private:
    std::istream & istr;
    Buffer buf;

public:
    SentenceSplitter(std::istream & istr_, size_t buffer_size_ = 1048576ULL)
        : istr(istr_)
        , buf(buffer_size_)
    {}

    bool readSentence(std::string & result, size_t max_size = 1048576ULL);
    bool eof();
};

}
