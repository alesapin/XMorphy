#include <xmorphy/graphem/SentenceSplitter.h>
#include <cctype>
#include <cstring>

namespace X
{

bool SentenceSplitter::eof()
{
    return buf.allRead() && istr.eof();
}

bool SentenceSplitter::readSentence(std::string & result, size_t max_size)
{
    if (max_size == 0)
        return false;
    char * last_space_pos = nullptr;
    char * potential_end_pos = nullptr;
    bool max_reached = false;
    size_t result_pos = 0;

    while(!eof())
    {
        bool read_until_end_of_line = false;
        if (buf.allRead())
        {
            istr.clear();
            istr.sync();
            istr.getline(buf.begin, buf.totalSize() + 1, '\n');
            size_t gcount = istr.gcount();
            if (!istr.fail())
            {
                /// What can be more idiotic than that?
                if (!istr.eof())
                    gcount -= 1;
                read_until_end_of_line = true;
            }
            buf.pos = buf.begin;
            buf.read_end = buf.pos + gcount;
        }
        for (auto current = buf.pos; current != buf.read_end; ++current)
        {
            if ((current - buf.pos) + result.size() == max_size)
            {
                max_reached = true;
                break;
            }

            if (*current == '?' || *current == '!' || *current == '.' || *current == '\n')
            {
                potential_end_pos = current;
                break;
            }

            if (std::isspace(*current))
                last_space_pos = current;
        }
        size_t sentence_size;
        bool finished = false;
        if (potential_end_pos != nullptr)
        {
            sentence_size = potential_end_pos - buf.pos + 1;
            result.resize(result.size() + sentence_size);
            std::memcpy(&result[result_pos], buf.pos, sentence_size);
            buf.pos += sentence_size;
            finished = true;
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
            finished = true;
        }
        else
        {
            sentence_size = buf.read_end - buf.pos;
            result.resize(result.size() + sentence_size);
            std::memcpy(&result[result_pos], buf.pos, sentence_size);
            buf.pos += sentence_size;
            result_pos += sentence_size;
        }

        if (finished || read_until_end_of_line)
            break;
    }

    return !result.empty();
}

}
