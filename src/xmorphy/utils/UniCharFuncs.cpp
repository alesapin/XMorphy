#include <xmorphy/utils/UniCharFuncs.h>

#include <cctype>
#include <limits>
#include <unicode/uchar.h>

namespace X
{
bool isupper(char16_t c)
{
    return u_isupper(c);
}
bool islower(char16_t c)
{
    return u_islower(c);
}
bool isdigit(char16_t c)
{
    return u_isdigit(c);
}
bool isalpha(char16_t c)
{
    return u_isalpha(c);
}
bool isalnum(char16_t c)
{
    return u_isalnum(c);
}
bool ispunct(char16_t c)
{
    if (isascii(c))
        return std::ispunct(static_cast<char>(c));
    return u_ispunct(c);
}
bool isspace(char16_t c)
{
    return u_isspace(c);
}
bool iscntrl(char16_t c)
{
    return u_iscntrl(c);
}
char16_t tolower(char16_t c)
{
    return u_tolower(c);
}

char16_t toupper(char16_t c)
{
    return u_toupper(c);
}

bool isascii(char16_t c)
{
    return c < 256;
}

bool iscyrrilic(char16_t c)
{
    return (1040 <= c && c <= 1103) || (c == 1105 || c == 1025);
}

bool canConcatenateWord(char16_t c)
{
    return c == '_' || c == '-';
}

}
