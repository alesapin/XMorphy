#pragma once

namespace X
{
bool isupper(char16_t c);
bool islower(char16_t c);
bool isdigit(char16_t c);
bool isalpha(char16_t c);
bool isalnum(char16_t c);
bool ispunct(char16_t c);
bool isspace(char16_t c);
bool iscntrl(char16_t c);
bool isascii(char16_t c);
bool iscyrrilic(char16_t c);

bool canConcatenateWord(char16_t c);

char16_t tolower(char16_t c);
char16_t toupper(char16_t c);
}
