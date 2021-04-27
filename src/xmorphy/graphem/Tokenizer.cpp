#include <xmorphy/graphem/Tokenizer.h>

#include <xmorphy/utils/UniCharFuncs.h>

namespace X
{

std::vector<std::shared_ptr<Token>> Tokenizer::analyze(const UniString & text) const
{
    std::vector<std::shared_ptr<Token>> result;
    size_t start_pos_byte = 0;
    for (size_t i = 0; i < text.length();)
    {
        std::shared_ptr<Token> r;
        size_t nextI = i;
        if (X::isalpha(text[i]))
        {
            nextI = cutWord(i, text);
            bool notAword = false;
            if (nextI == i)
            {
                nextI = cutWordNum(i, text);
                notAword = true;
            }
            UniString word = text.subString(i, nextI - i);
            if (notAword)
            {
                r = processWordNum(word);
            }
            else
            {
                r = processWord(word);
            }
        }
        else if (X::ispunct(text[i]))
        {
            nextI = cutPunct(i, text);
            UniString punct = text.subString(i, nextI - i);
            r = processPunct(punct);
        }
        else if (X::isdigit(text[i]))
        {
            nextI = cutNumber(i, text);
            bool notAnumber = false;
            if (nextI == i)
            { //Не продвинулись --> не цифра
                nextI = cutWordNum(i, text);
                notAnumber = true;
            }
            UniString num = text.subString(i, nextI - i);
            if (notAnumber)
            {
                r = processWordNum(num);
            }
            else
            {
                r = processNumber(num);
            }
        }
        else if (X::isspace(text[i]))
        {
            nextI = cutSeparator(i, text);
            UniString sep = text.subString(i, nextI - i);
            r = processSeparator(sep);
        }
        else if (X::iscntrl(text[i]))
        {
            nextI = cutTrash(i, text);
            UniString trash = text.subString(i, nextI - i);
            r = processHieroglyph(trash);
        }
        else
        {
            nextI = i + 1;
            r = std::make_shared<Token>(UniString(text[i]), TokenTypeTag::HIER);
        }

        r->setStartPosUnicode(i);
        r->setStartPosByte(start_pos_byte);
        i = nextI;
        start_pos_byte += r->getInner().byteLength();
        result.push_back(r);
    }
    return result;
}

std::shared_ptr<Token> Tokenizer::analyzeSingleWord(const UniString & word) const
{
    return processWord(word);
}

size_t Tokenizer::cutWordNum(size_t start, const UniString & str) const
{
    size_t i = start;
    bool alpha_num_found = false;
    while (i < str.length())
    {
        if (X::isalpha(str[i]) || X::isdigit(str[i]))
        {
            alpha_num_found = true;
            ++i;
        }
        else if (alpha_num_found && X::canConcatenateWord(str[i]))
        {
            ++i;
        }
        else
            break;
    }
    return i;
}

size_t Tokenizer::cutNumber(size_t start, const UniString & str) const
{
    size_t i = start;
    bool digit_found = false;
    while (i < str.length())
    {
        if (X::isdigit(str[i]))
        {
            digit_found = true;
            ++i;
        }
        else if (digit_found && X::canConcatenateWord(str[i]))
        {
            ++i;
        }
        else
            break;
    }

    if (i < str.length() && !X::isspace(str[i]) && !X::ispunct(str[i]))
        return start;
    return i;
}

size_t Tokenizer::cutSeparator(size_t start, const UniString & str) const
{
    size_t i = start;
    while (i < str.length() && X::isspace(str[i]))
    {
        i++;
    }
    return i;
}

size_t Tokenizer::cutPunct(size_t start, const UniString & str) const
{
    size_t i = start;
    char16_t first = str[i];
    while (i < str.length() && str[i] == first)
        ++i;
    if (i == start)
        return i + 1;
    return i;
}

size_t Tokenizer::cutWord(size_t start, const UniString & str) const
{
    size_t i = start;
    bool alpha_found = false;
    while (i < str.length())
    {
        if (X::isalpha(str[i]))
        {
            alpha_found = true;
            ++i;
        }
        else if (alpha_found && X::canConcatenateWord(str[i]))
        {
            ++i;
        }
        else
        {
            break;
        }
    }

    if (i < str.length() && !X::isspace(str[i]) && !X::ispunct(str[i]))
        return start;

    return i;
}

size_t Tokenizer::cutTrash(size_t start, const UniString & str) const
{
    size_t i = start;
    while (i < str.length() && X::iscntrl(str[i]))
    {
        i++;
    }

    return i;
}

std::shared_ptr<Token> Tokenizer::processWord(const UniString & str) const
{
    GraphemTag t = GraphemTag::UNKN;
    bool isUpperCase = true;
    bool isLowerCase = true;
    bool isLatin = true;
    bool isCyrrilic = true;
    bool capStart = X::isupper(str[0]);
    int capCounter = 0;
    bool hasLatin = false;
    bool hasCyrrilic = false;
    bool connected = false;
    for (size_t i = 0; i < str.length(); ++i)
    {
        auto chr = str[i];

        if (X::isalpha(chr))
        {
            if (!X::isascii(chr))
                isLatin = false;
            else
                hasLatin = true;

            if (!X::iscyrrilic(chr))
                isCyrrilic = false;
            else
                hasCyrrilic = true;
        }

        if (X::islower(chr))
        {
            isUpperCase = false;
        }
        else if (X::isupper(chr))
        {
            capCounter++;
            isLowerCase = false;
        }

        if (X::canConcatenateWord(chr))
            connected = true;
    }
    if (isUpperCase)
    {
        t |= GraphemTag::UPPER_CASE;
    }
    else if (isLowerCase)
    {
        t |= GraphemTag::LOWER_CASE;
    }
    else if (capCounter == 1 && capStart)
    {
        t |= GraphemTag::CAP_START;
    }
    else
    {
        t |= GraphemTag::MIXED;
    }

    if (isLatin)
    {
        t |= GraphemTag::LATIN;
    }
    else if (isCyrrilic)
    {
        t |= GraphemTag::CYRILLIC;
    }
    else if (hasLatin && hasCyrrilic)
    {
        t |= GraphemTag::MULTI_ENC;
    }

    if (connected)
        t |= GraphemTag::CONNECTED;

    Token * res = new Token(str, TokenTypeTag::WORD, t);
    return std::shared_ptr<Token>(res);
}

static GraphemTag processOnePunct(char16_t sym)
{
    GraphemTag t = GraphemTag::UNKN;
    if (sym == ',')
        t |= GraphemTag::COMMA;
    else if (sym == '.')
        t |= GraphemTag::DOT;
    else if (sym == ':')
        t |= GraphemTag::COLON;
    else if (sym == ';')
        t |= GraphemTag::SEMICOLON;
    else if (sym == '?')
        t |= GraphemTag::QUESTION_MARK;
    else if (sym == '!')
        t |= GraphemTag::EXCLAMATION_MARK;
    else if (sym == '"' || sym == u'»' || sym == u'«')
        t |= GraphemTag::QUOTE;
    else if (sym == '_')
        t |= GraphemTag::LOWER_DASH;
    else if (sym == '-' || sym == u'—')
        t |= GraphemTag::DASH;
    else if (sym == '(')
        t |= GraphemTag::PARENTHESIS_L;
    else if (sym == ')')
        t |= GraphemTag::PARENTHESIS_R;
    else
        t |= GraphemTag::UNCOMMON_PUNCT;
    return t;
}

std::shared_ptr<Token> Tokenizer::processPunct(const UniString & str) const
{
    GraphemTag t = GraphemTag::UNKN;
    if (str.length() > 1)
    {
        t |= GraphemTag::PUNCT_GROUP;
        bool allequal = true;
        char16_t first = str[0];
        for (size_t i = 1; i < str.length(); ++i)
        {
            if (str[i] != first)
                allequal = false;
        }
        if (allequal)
        {
            if (str.length() == 3 && first == '.')
                t |= GraphemTag::THREE_DOTS;
            else
                t |= processOnePunct(first);
        }
    }
    else
        t |= processOnePunct(str[0]);
    Token * res = new Token(str, TokenTypeTag::PNCT, t);
    return std::shared_ptr<Token>(res);
}
std::shared_ptr<Token> Tokenizer::processNumber(const UniString & number) const
{
    bool isBinary = true;
    bool isOct = number[0] == '0';
    GraphemTag t = GraphemTag::DECIMAL;
    bool connected = false;
    for (size_t i = 0; i < number.length(); ++i)
    {
        auto chr = number[i];
        if (chr != '0' && chr != '1')
        {
            isBinary = false;
        }
        if (chr == '8' || chr == '9')
        {
            isOct = false;
        }
        if (X::canConcatenateWord(chr))
            connected = true;
    }
    if (isBinary)
    {
        t |= GraphemTag::BINARY;
    }
    else if (isOct)
    {
        t |= GraphemTag::OCT;
    }
    if (connected)
        t |= GraphemTag::CONNECTED;
    Token * res = new Token(number, TokenTypeTag::NUMB, t);
    return std::shared_ptr<Token>(res);
}
std::shared_ptr<Token> Tokenizer::processSeparator(const UniString & sep) const
{
    GraphemTag t = GraphemTag::UNKN;
    if (sep.length() > 1)
    {
        t = GraphemTag::MULTI_SEP;
    }
    else
    {
        t = GraphemTag::SINGLE_SEP;
        auto sym = sep[0];
        if (sym == ' ')
        {
            t |= GraphemTag::SPACE;
        }
        else if (sym == '\t')
        {
            t |= GraphemTag::TAB;
        }
        else if (sym == '\n')
        {
            t |= GraphemTag::NEW_LINE;
        }
        else if (sym == '\r')
        {
            t |= GraphemTag::CR;
        }
    }
    Token * res = new Token(sep, TokenTypeTag::SEPR, t);
    return std::shared_ptr<Token>(res);
}
std::shared_ptr<Token> Tokenizer::processWordNum(const UniString & wn) const
{
    GraphemTag t = GraphemTag::UNKN;
    bool connected = false;
    bool isLatin = true;
    bool hasLatin = false;
    bool isCyrrilic = true;
    bool hasCyrrilic = false;
    for (std::size_t i = wn.length() - 1; i > 0; --i)
    {
        if (X::canConcatenateWord(wn[i]))
            connected = true;
        else if (X::isalpha(wn[i]))
        {
            if (X::iscyrrilic(wn[i]))
            {
                isLatin = false;
                hasCyrrilic = true;
            }
            else if (X::isascii(wn[i]))
            {
                isCyrrilic = false;
                hasLatin = true;
            }
        }
    }
    if (isCyrrilic)
    {
        t |= GraphemTag::CYRILLIC;
    }
    else if (isLatin)
    {
        t |= GraphemTag::LATIN;
    }

    if (hasLatin && hasCyrrilic)
        t |= GraphemTag::MIXED;

    if (connected)
        t |= GraphemTag::CONNECTED;

    Token * res = new Token(wn, TokenTypeTag::WRNM, t);
    return std::shared_ptr<Token>(res);
}

std::shared_ptr<Token> Tokenizer::processHieroglyph(const UniString & hir) const
{
    Token * res = new Token(hir, TokenTypeTag::HIER);
    return std::shared_ptr<Token>(res);
}

}
