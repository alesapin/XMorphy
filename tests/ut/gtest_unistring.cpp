#include <gtest/gtest.h>
#include <xmorphy/utils/UniString.h>
#include <locale>

using namespace std;
using namespace X;

TEST(TestString, TestFind) {
    UniString search("Привет В!т");
    UniString searching1("В");
    UniString searching2("П");

    EXPECT_EQ(7, search.find(searching1));
    EXPECT_EQ(0, search.find(searching2));
    EXPECT_EQ(search.length() - 2, search.find(UniString("!")));

    UniString empt("");
    EXPECT_EQ(0, empt.find(UniString("")));
    EXPECT_EQ(-1, empt.find(UniString("123")));
    EXPECT_EQ(0, search.find(empt));

    EXPECT_EQ(-1, search.find(empt, 1000));
    EXPECT_EQ(-1, search.find(searching2, 4));
    EXPECT_EQ(search.length() - 1, search.find(UniString("т"), 6));

    UniString obez("ПООБЕЗЬЯНОПОДОБНЕЕ");
    UniString obez1("ОБЕЗЬЯНОПОДОБНЕ");
    EXPECT_EQ(2, obez.find(obez1));
}

TEST(TestString, TestReplace) {
    UniString search("Зудило");
    EXPECT_EQ(UniString("Кудило"), search.replace(u'З', u'К'));

    UniString search1("имПрифи");
    EXPECT_EQ(UniString("ймПрйфй"), search1.replace(u'и', u'й'));

    UniString search2("преОБРазовал");
    EXPECT_EQ(UniString("преОВБазовал"), search2.replace(UniString("ОБР"), UniString("ОВБ")));

    UniString search3("дрпродр");
    EXPECT_EQ(UniString("далпродал"), search3.replace(UniString("др"), UniString("дал")));

    UniString search4("АФК");
    EXPECT_EQ(UniString("мем"), search4.replace(UniString("АФК"), UniString("мем")));

    UniString search5("АКАБ");
    EXPECT_EQ(search5, search5.replace(UniString("АФК"), UniString("мем")));

    UniString search6("АФАИК");
    EXPECT_EQ(UniString("АК"), search6.replace(UniString("ФАИ"), UniString("")));
}

TEST(TestString, TestCase) {
    UniString search("Привет В!");

    EXPECT_TRUE(search.toUpperCase().isUpperCase());
    EXPECT_TRUE(search.toLowerCase().isLowerCase());
    EXPECT_FALSE(search.isLowerCase() || search.isUpperCase());

    UniString empt("");
    EXPECT_FALSE(empt.toUpperCase().isUpperCase());
    EXPECT_FALSE(empt.toLowerCase().isLowerCase());
    EXPECT_EQ(empt, empt.toLowerCase());
    EXPECT_EQ(empt, empt.toLowerCase());
}

TEST(TestString, TestChars) {
    UniString letters("Пр!и@в етingoЫ");

    EXPECT_EQ(letters[0], u'П');
    EXPECT_EQ(letters[2], '!');
    EXPECT_EQ(letters[2], '!');
    EXPECT_EQ(letters[3], u'и');
    EXPECT_EQ(letters[6], ' ');
    EXPECT_EQ(letters[6], ' ');
    EXPECT_EQ(letters[letters.length() - 1], u'Ы');

    EXPECT_TRUE(X::ispunct(letters[2]));
    EXPECT_TRUE(X::isalpha(letters[0]));
    EXPECT_TRUE(X::isupper(letters[0]));
    EXPECT_TRUE(X::isspace(letters[6]));
    EXPECT_TRUE(X::isalpha(letters[9]));
    EXPECT_TRUE(X::islower(letters[9]));

    EXPECT_EQ(X::tolower(letters[0]), u'п');
    EXPECT_EQ(X::toupper(letters[2]), '!');
    EXPECT_EQ(X::toupper(letters[9]), 'I');

    UniString nums("1x23,123.444,01");

    EXPECT_TRUE(X::isdigit(nums[0]));
    EXPECT_FALSE(X::isdigit(nums[8]));
    EXPECT_TRUE(X::ispunct(nums[4]));
    UniString puncts("!?,.:;");
    for (size_t i = 0; i < puncts.length(); ++i) {
        EXPECT_TRUE(X::ispunct(puncts[i]));
    }
    char16_t diakr = u'á';
    EXPECT_TRUE(X::isalpha(diakr));
}

TEST(TestString, TestSplit) {
    UniString letters("Пр!и@в етingoЫ");
    UniString left("Пр!и@в");
    UniString right("етingoЫ");

    EXPECT_EQ(letters.split(' ').size(), 2);
    EXPECT_EQ(left, letters.split(' ')[0]);
    EXPECT_EQ(right, letters.split(' ')[1]);
    EXPECT_EQ(letters.split(UniString("П")).size(), 2);
    EXPECT_EQ(letters.split(UniString("П"))[1], "р!и@в етingoЫ");
    EXPECT_EQ(letters.split(UniString("Ы")).size(), 2);
    EXPECT_EQ(letters.split(UniString("Ы"))[0], "Пр!и@в етingo");

    UniString empty("");
    EXPECT_EQ(empty.split(' ')[0], "");

    UniString space("   ");
    EXPECT_EQ(space.split(' ').size(), 4);
    for (size_t i = 0; i < 4; ++i) {
        EXPECT_EQ(space.split(' ')[i], "");
    }
}

TEST(TestString, TestOpeartor) {
    UniString first("Hi маша ツ!");
    UniString second("И тебе привет, catch!");
    UniString summary("Hi маша ツ!И тебе привет, catch!");
    EXPECT_EQ(summary, first + second);
    EXPECT_NE(summary, first);
    EXPECT_LT(first, second);
    EXPECT_LT(first, summary);
    EXPECT_GT(second, first);
    UniString empt("");
    EXPECT_EQ(empt, empt + empt);
    EXPECT_FALSE(empt < empt);
    EXPECT_FALSE(empt > empt);

    EXPECT_EQ(second, first = second);
    first = UniString("Руддщ!");
    EXPECT_NE(second, first);
    UniString copy(second);
    EXPECT_EQ(copy, second);
    EXPECT_EQ(copy, first = UniString(second));
}

TEST(TestString, TestSubString) {
    UniString exmpl("Авада-kedavraЫЫ!");

    EXPECT_EQ(UniString("А"), exmpl.subString(0, 1));
    EXPECT_EQ(exmpl, exmpl.subString(0, 10000));
    EXPECT_EQ(exmpl, exmpl.subString(0, exmpl.length()));
    EXPECT_EQ(UniString("Авада-kedavraЫЫ"), exmpl.subString(0, exmpl.length() - 1));

    EXPECT_EQ(UniString("-kedavraЫЫ!"), exmpl.subString(5));
    EXPECT_EQ(UniString("!"), exmpl.subString(exmpl.length() - 1, 1));

    UniString empt("");
    EXPECT_EQ(empt, exmpl.subString(0, 0));
    EXPECT_ANY_THROW(empt.subString(1, 1));
    EXPECT_ANY_THROW(exmpl.subString(1000, 0));
}

TEST(TestString, TestEndsWith) {
    UniString exmpl("Же не манж па си jur@Ы@В@?");
    UniString empt("");
    EXPECT_TRUE(exmpl.endsWith(UniString("r@Ы@В@?")));
    EXPECT_FALSE(exmpl.endsWith(UniString("xr@Ы@В@?")));
    EXPECT_TRUE(exmpl.endsWith(exmpl));
    EXPECT_TRUE(exmpl.endsWith(empt));
    EXPECT_FALSE(empt.endsWith(exmpl));
    EXPECT_TRUE(empt.endsWith(empt));
}
