#include <xmorphy/graphem/SentenceSplitter.h>
#include <cctype>
#include <cstring>

namespace X
{


bool SentenceSplitter::readIntoBuffer()
{
    istr.read(buf.begin, buf.totalSize());
    size_t gcount = istr.gcount();
    if (gcount != 0)
    {
        buf.pos = buf.begin;
        buf.read_end = buf.pos + gcount;
        return true;
    }
    else
    {
        if (istr.eof())
            return false;
        else
            throw std::runtime_error("Undexpected state while reading from istream");
    }
}

bool SentenceSplitter::eof()
{
    return buf.allRead() && !readIntoBuffer();
}

bool SentenceSplitter::readSentence(std::string & result, size_t max_size)
{
    if (max_size == 0)
        return false;
    char * last_space_pos = nullptr;
    char * potential_end_pos = nullptr;
    bool max_reached = false;
    size_t result_pos = 0;

    bool something_read = false;
    while(!eof())
    {
        something_read = true;
        for (auto current = buf.pos; current != buf.read_end; ++current)
        {
            if ((current - buf.pos) + result.size() == max_size)
            {
                max_reached = true;
                break;
            }

            if (*current == '?' || *current == '!' || *current == '.')
            {
                potential_end_pos = current;
                break;
            }

            if (std::isspace(*current))
                last_space_pos = current;
        }
        size_t sentence_size;
        if (potential_end_pos != nullptr)
        {
            sentence_size = potential_end_pos - buf.pos + 1;
            result.resize(result.size() + sentence_size);
            std::memcpy(&result[result_pos], buf.pos, sentence_size);
            buf.pos += sentence_size;
            break;
        }
        else if (max_reached)
        {
            if (last_space_pos != nullptr && last_space_pos - buf.pos > 0)
            {
                sentence_size = last_space_pos - buf.pos;
                result.resize(result.size() + sentence_size);
                std::memcpy(&result[result_pos], buf.pos, sentence_size);
                buf.pos += sentence_size;
            }
            else
            {
                sentence_size = max_size - result.size();
                result.resize(result.size() + sentence_size);
                std::memcpy(&result[result_pos], buf.pos, sentence_size);
                buf.pos += sentence_size;
            }
            break;
        }
        else
        {
            sentence_size = buf.read_end - buf.pos;
            result.resize(result.size() + sentence_size);
            std::memcpy(&result[result_pos], buf.pos, sentence_size);
            buf.pos += sentence_size;
            result_pos += sentence_size;
        }
    }

    return something_read;
}

}
