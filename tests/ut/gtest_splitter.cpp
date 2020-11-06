#include <gtest/gtest.h>
#include <xmorphy/graphem/SentenceSplitter.h>
#include <sstream>
#include <string>

TEST(TestSplit, TestSimple)
{
    using namespace X;
    std::string str{"Hello world. There are only three sentences. Or more?"};
    std::istringstream iss(str);
    SentenceSplitter splitter(iss);
    std::string first, second, third, fourth;
    EXPECT_TRUE(splitter.readSentence(first));
    EXPECT_TRUE(splitter.readSentence(second));
    EXPECT_TRUE(splitter.readSentence(third));
    EXPECT_FALSE(splitter.readSentence(fourth));
    EXPECT_EQ(first, "Hello world.");
    EXPECT_EQ(second, " There are only three sentences.");
    EXPECT_EQ(third, " Or more?");
    EXPECT_EQ(fourth, "");
    EXPECT_TRUE(splitter.eof());
}

TEST(TestSplit, TestMaxSize)
{
    using namespace X;
    std::string str{"Hello world. There are only three sentences. Or more?"};
    std::istringstream iss(str);
    SentenceSplitter splitter(iss);
    std::string first, second, third, fourth, fifth, sixs, seventh;
    EXPECT_TRUE(splitter.readSentence(first, 7));
    EXPECT_EQ(first, "Hello");
    EXPECT_TRUE(splitter.readSentence(second, 10));
    EXPECT_EQ(second, " world.");
    EXPECT_TRUE(splitter.readSentence(third, 9));
    EXPECT_EQ(third, " There");
    EXPECT_TRUE(splitter.readSentence(fourth, 26));
    EXPECT_EQ(fourth, " are only three sentences.");
    EXPECT_TRUE(splitter.readSentence(fifth, 2));
    EXPECT_EQ(fifth, " O");
    EXPECT_TRUE(splitter.readSentence(sixs, 1));
    EXPECT_EQ(sixs, "r");
    EXPECT_TRUE(splitter.readSentence(seventh, 100));
    EXPECT_EQ(seventh, " more?");
}

TEST(TestSplit, TestSmallBuffer4)
{
    using namespace X;
    std::string str{"Hello world. There are only three sentences. Or more?"};
    std::istringstream iss(str);
    SentenceSplitter splitter(iss, 4);
    std::string first, second, third, fourth;
    EXPECT_TRUE(splitter.readSentence(first));
    EXPECT_TRUE(splitter.readSentence(second));
    EXPECT_TRUE(splitter.readSentence(third));
    EXPECT_FALSE(splitter.readSentence(fourth));
    EXPECT_EQ(first, "Hello world.");
    EXPECT_EQ(second, " There are only three sentences.");
    EXPECT_EQ(third, " Or more?");
    EXPECT_EQ(fourth, "");
    EXPECT_TRUE(splitter.eof());
}

TEST(TestSplit, TestSmallBuffer3)
{
    using namespace X;
    std::string str{"Hello world. There are only three sentences. Or more?"};
    std::istringstream iss(str);
    SentenceSplitter splitter(iss, 3);
    std::string first, second, third, fourth;
    EXPECT_TRUE(splitter.readSentence(first));
    EXPECT_TRUE(splitter.readSentence(second));
    EXPECT_TRUE(splitter.readSentence(third));
    EXPECT_FALSE(splitter.readSentence(fourth));
    EXPECT_EQ(first, "Hello world.");
    EXPECT_EQ(second, " There are only three sentences.");
    EXPECT_EQ(third, " Or more?");
    EXPECT_EQ(fourth, "");
    EXPECT_TRUE(splitter.eof());
}

TEST(TestSplit, TestSmallBuffer2)
{
    using namespace X;
    std::string str{"Hello world. There are only three sentences. Or more?"};
    std::istringstream iss(str);
    SentenceSplitter splitter(iss, 2);
    std::string first, second, third, fourth;
    EXPECT_TRUE(splitter.readSentence(first));
    EXPECT_TRUE(splitter.readSentence(second));
    EXPECT_TRUE(splitter.readSentence(third));
    EXPECT_FALSE(splitter.readSentence(fourth));
    EXPECT_EQ(first, "Hello world.");
    EXPECT_EQ(second, " There are only three sentences.");
    EXPECT_EQ(third, " Or more?");
    EXPECT_EQ(fourth, "");
    EXPECT_TRUE(splitter.eof());
}

TEST(TestSplit, TestSmallBuffer1)
{
    using namespace X;
    std::string str{"Hello world. There are only three sentences. Or more?"};
    std::istringstream iss(str);
    SentenceSplitter splitter(iss, 1);
    std::string first, second, third, fourth;
    EXPECT_TRUE(splitter.readSentence(first));
    EXPECT_TRUE(splitter.readSentence(second));
    EXPECT_TRUE(splitter.readSentence(third));
    EXPECT_FALSE(splitter.readSentence(fourth));
    EXPECT_EQ(first, "Hello world.");
    EXPECT_EQ(second, " There are only three sentences.");
    EXPECT_EQ(third, " Or more?");
    EXPECT_EQ(fourth, "");
    EXPECT_TRUE(splitter.eof());
}

TEST(TestSplit, TestBufferSizeBorder)
{
    using namespace X;
    std::string str{"Hello! world."};
    std::istringstream iss(str);
    SentenceSplitter splitter(iss, 6);
    std::string first, second;
    EXPECT_TRUE(splitter.readSentence(first, 6));
    EXPECT_TRUE(splitter.readSentence(second, 7));
    EXPECT_EQ(first, "Hello!");
    EXPECT_EQ(second, " world.");
    EXPECT_TRUE(splitter.eof());
}

TEST(TestSplit, TestNoSpaces)
{
    using namespace X;
    std::string str{"111111111111111111111111111111111111111"};
    std::istringstream iss(str);
    SentenceSplitter splitter(iss, 3);
    std::string first, second, third;
    EXPECT_TRUE(splitter.readSentence(first, 6));
    EXPECT_TRUE(splitter.readSentence(second, 7));
    EXPECT_TRUE(splitter.readSentence(third));
    EXPECT_EQ(first, "111111");
    EXPECT_EQ(second, "1111111");
    EXPECT_EQ(third, "11111111111111111111111111");
    EXPECT_TRUE(splitter.eof());
}
