#include <gtest/gtest.h>

#include <incbin.h>
#include <morph/DictMorphAnalyzer.h>
#include <morph/SuffixDictAnalyzer.h>

using namespace X;

bool hasSP(const std::vector<ParsedPtr> & data, UniSPTag sp)
{
    for (size_t i = 0; i < data.size(); ++i)
        if (data[i]->sp == sp)
            return true;
    return false;
}

bool hasNF(const std::vector<ParsedPtr> & data, const utils::UniString & nf)
{
    for (size_t i = 0; i < data.size(); ++i)
        if (data[i]->normalform == nf)
            return true;
    return false;
}

bool hasWF(const std::vector<ParsedPtr> & data, const utils::UniString & nf)
{
    for (size_t i = 0; i < data.size(); ++i)
        if (data[i]->wordform == nf)
            return true;
    return false;
}


TEST(TestDictAnalyze, TestDict) {
    DictMorphAnalyzer proc;
    std::vector<ParsedPtr> result = proc.analyze(utils::UniString("АККОМПАНИРУЕМ"));
    EXPECT_EQ(result[0]->normalform, utils::UniString("АККОМПАНИРОВАТЬ"));
    EXPECT_EQ(result[0]->wordform, utils::UniString("АККОМПАНИРУЕМ"));
    EXPECT_TRUE(hasSP(result, UniSPTag::VERB));

    std::vector<ParsedPtr> result1 = proc.analyze(utils::UniString("СТАЛИ"));
    EXPECT_TRUE(hasNF(result1, utils::UniString("СТАЛЬ")));
    EXPECT_TRUE(hasNF(result1, utils::UniString("СТАТЬ")));
    EXPECT_TRUE(hasSP(result1, UniSPTag::NOUN));
    EXPECT_TRUE(hasSP(result1, UniSPTag::VERB));

    std::vector<ParsedPtr> lyd = proc.analyze(utils::UniString("ЛЮДИ"));
    EXPECT_TRUE(hasNF(lyd, utils::UniString("ЧЕЛОВЕК")));
    EXPECT_TRUE(hasSP(lyd, UniSPTag::NOUN));

    std::vector<ParsedPtr> synt = proc.synthesize(utils::UniString("ИНТЕРЕСНЫЙ"), UniMorphTag::Fem | UniMorphTag::Nom);
    EXPECT_TRUE(hasWF(synt, utils::UniString("ИНТЕРЕСНАЯ")));

    std::vector<ParsedPtr> synt1 = proc.synthesize(utils::UniString("СТАЛИ"), UniMorphTag::Fem | UniMorphTag::Nom | UniMorphTag::Sing);
    EXPECT_TRUE(hasWF(synt1, utils::UniString("СТАЛЬ")));

    std::vector<ParsedPtr> stal = proc.synthesize(utils::UniString("СТАЛ"), UniMorphTag::Fem);
    EXPECT_TRUE(hasWF(stal, utils::UniString("СТАЛА")));
}

TEST(TestDictAnalyze, TestSuffixDict) {
    SuffixDictAnalyzer proc;
    std::vector<ParsedPtr> result = proc.analyze(utils::UniString("КУЗЯВАЯ"));
    EXPECT_TRUE(hasNF(result, utils::UniString("КУЗЯВЫЙ")));
    EXPECT_TRUE(hasSP(result, UniSPTag::ADJ));
    EXPECT_TRUE(hasSP(result, UniSPTag::NOUN));

    std::vector<ParsedPtr> result1 = proc.analyze(utils::UniString("СЕПЕЛИВКИ"));
    EXPECT_TRUE(hasNF(result1, utils::UniString("СЕПЕЛИВКА")));
    EXPECT_TRUE(hasSP(result1, UniSPTag::NOUN));

    std::vector<ParsedPtr> result2 = proc.analyze(utils::UniString("СКРИШОЛЬНУЛАСЬ"));
    EXPECT_TRUE(hasNF(result2, utils::UniString("СКРИШОЛЬНУТЬСЯ")));
    EXPECT_TRUE(hasSP(result2, UniSPTag::VERB));

    std::vector<ParsedPtr> result3 = proc.analyze(utils::UniString("ДЕФАЛЬСИФИКАЦИЕЙ"));
    EXPECT_TRUE(hasNF(result3, utils::UniString("ДЕФАЛЬСИФИКАЦИЯ")));
    EXPECT_TRUE(hasSP(result3, UniSPTag::NOUN));

    std::vector<ParsedPtr> result4 = proc.analyze(utils::UniString("ДЕФАЛЬСИФИЦИРОВАЛА"));
    EXPECT_TRUE(hasNF(result4, utils::UniString("ДЕФАЛЬСИФИЦИРОВАЛ")));
    EXPECT_TRUE(hasSP(result4, UniSPTag::VERB));


    std::vector<ParsedPtr> synt = proc.synthesize(utils::UniString("ИНТЕРЛИВЫЙ"), UniMorphTag::Fem | UniMorphTag::Nom);
    EXPECT_TRUE(hasWF(synt, utils::UniString("ИНТЕРЛИВАЯ")));

    std::vector<ParsedPtr> synt1 = proc.synthesize(utils::UniString("ВКЛЕТАЛИ"), UniMorphTag::Fem | UniMorphTag::Nom | UniMorphTag::Sing);
    EXPECT_TRUE(hasWF(synt1, utils::UniString("ВКЛЕТАЛЬ")));

    std::vector<ParsedPtr> stal = proc.synthesize(utils::UniString("ШМЕНЕКАЛ"), UniMorphTag::Fem);
    EXPECT_TRUE(hasWF(stal, utils::UniString("ШМЕНЕКАЛА")));
}
