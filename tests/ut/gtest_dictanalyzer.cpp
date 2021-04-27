#include <gtest/gtest.h>

#include <incbin.h>
#include <xmorphy/morph/DictMorphAnalyzer.h>
#include <xmorphy/morph/SuffixDictAnalyzer.h>
#include <xmorphy/morph/HyphenAnalyzer.h>

using namespace X;

bool hasSP(const std::vector<ParsedPtr> & data, UniSPTag sp)
{
    for (size_t i = 0; i < data.size(); ++i)
        if (data[i]->sp == sp)
            return true;
    return false;
}

bool hasNF(const std::vector<ParsedPtr> & data, const UniString & nf)
{
    for (size_t i = 0; i < data.size(); ++i)
        if (data[i]->normalform == nf)
            return true;
    return false;
}

bool hasWF(const std::vector<ParsedPtr> & data, const UniString & nf)
{
    for (size_t i = 0; i < data.size(); ++i)
        if (data[i]->wordform == nf)
            return true;
    return false;
}


TEST(TestDictAnalyze, TestDict) {
    DictMorphAnalyzer proc;
    std::vector<ParsedPtr> result = proc.analyze(UniString("АККОМПАНИРУЕМ"));
    EXPECT_EQ(result[0]->normalform, UniString("АККОМПАНИРОВАТЬ"));
    EXPECT_EQ(result[0]->wordform, UniString("АККОМПАНИРУЕМ"));
    EXPECT_TRUE(hasSP(result, UniSPTag::VERB));

    std::vector<ParsedPtr> result1 = proc.analyze(UniString("СТАЛИ"));
    EXPECT_TRUE(hasNF(result1, UniString("СТАЛЬ")));
    EXPECT_TRUE(hasNF(result1, UniString("СТАТЬ")));
    EXPECT_TRUE(hasSP(result1, UniSPTag::NOUN));
    EXPECT_TRUE(hasSP(result1, UniSPTag::VERB));

    std::vector<ParsedPtr> lyd = proc.analyze(UniString("ЛЮДИ"));
    EXPECT_TRUE(hasNF(lyd, UniString("ЧЕЛОВЕК")));
    EXPECT_TRUE(hasSP(lyd, UniSPTag::NOUN));

    std::vector<ParsedPtr> synt = proc.synthesize(UniString("ИНТЕРЕСНЫЙ"), UniMorphTag::Fem | UniMorphTag::Nom);
    EXPECT_TRUE(hasWF(synt, UniString("ИНТЕРЕСНАЯ")));

    std::vector<ParsedPtr> synt1 = proc.synthesize(UniString("СТАЛИ"), UniMorphTag::Fem | UniMorphTag::Nom | UniMorphTag::Sing);
    EXPECT_TRUE(hasWF(synt1, UniString("СТАЛЬ")));

    std::vector<ParsedPtr> stal = proc.synthesize(UniString("СТАЛ"), UniMorphTag::Fem);
    EXPECT_TRUE(hasWF(stal, UniString("СТАЛА")));
}

TEST(TestDictAnalyze, TestSuffixDict) {
    SuffixDictAnalyzer proc;
    std::vector<ParsedPtr> result = proc.analyze(UniString("КУЗЯВАЯ"));
    EXPECT_TRUE(hasNF(result, UniString("КУЗЯВЫЙ")));
    EXPECT_TRUE(hasSP(result, UniSPTag::ADJ));
    EXPECT_TRUE(hasSP(result, UniSPTag::NOUN));

    std::vector<ParsedPtr> result1 = proc.analyze(UniString("СЕПЕЛИВКИ"));
    EXPECT_TRUE(hasNF(result1, UniString("СЕПЕЛИВКА")));
    EXPECT_TRUE(hasSP(result1, UniSPTag::NOUN));

    std::vector<ParsedPtr> result2 = proc.analyze(UniString("СКРИШОЛЬНУЛАСЬ"));
    EXPECT_TRUE(hasNF(result2, UniString("СКРИШОЛЬНУТЬСЯ")));
    EXPECT_TRUE(hasSP(result2, UniSPTag::VERB));

    std::vector<ParsedPtr> result3 = proc.analyze(UniString("ДЕФАЛЬСИФИКАЦИЕЙ"));
    EXPECT_TRUE(hasNF(result3, UniString("ДЕФАЛЬСИФИКАЦИЯ")));
    EXPECT_TRUE(hasSP(result3, UniSPTag::NOUN));

    std::vector<ParsedPtr> result4 = proc.analyze(UniString("ДЕФАЛЬСИФИЦИРОВАЛА"));
    EXPECT_TRUE(hasNF(result4, UniString("ДЕФАЛЬСИФИЦИРОВАТЬ")));
    EXPECT_TRUE(hasSP(result4, UniSPTag::VERB));


    std::vector<ParsedPtr> synt = proc.synthesize(UniString("ИНТЕРЛИВЫЙ"), UniMorphTag::Fem | UniMorphTag::Nom);
    EXPECT_TRUE(hasWF(synt, UniString("ИНТЕРЛИВАЯ")));

    std::vector<ParsedPtr> synt1 = proc.synthesize(UniString("ВКЛЕТАЛИ"), UniMorphTag::Fem | UniMorphTag::Nom | UniMorphTag::Sing);
    EXPECT_TRUE(hasWF(synt1, UniString("ВКЛЕТАЛЬ")));

    std::vector<ParsedPtr> stal = proc.synthesize(UniString("ШМЕНЕКАЛ"), UniMorphTag::Fem);
    EXPECT_TRUE(hasWF(stal, UniString("ШМЕНЕКАЛА")));
}


TEST(TestDictAnalyze, TestHyphDict)
{
    HyphenAnalyzer proc;
    std::vector<ParsedPtr> result = proc.analyze(UniString("ЧЕЛОВЕК-ГОРА"));
    EXPECT_TRUE(hasWF(result, UniString("ЧЕЛОВЕК-ГОРА")));
    EXPECT_TRUE(hasNF(result, UniString("ЧЕЛОВЕК-ГОРА")));
    EXPECT_TRUE(hasSP(result, UniSPTag::NOUN));

    std::vector<ParsedPtr> result1 = proc.analyze(UniString("ЧЕЛОВЕКОМ-ГОРОЙ"));
    EXPECT_TRUE(hasWF(result1, UniString("ЧЕЛОВЕКОМ-ГОРОЙ")));
    EXPECT_TRUE(hasNF(result1, UniString("ЧЕЛОВЕК-ГОРА")));
    EXPECT_TRUE(hasSP(result1, UniSPTag::NOUN));

    std::vector<ParsedPtr> result2 = proc.analyze(UniString("ПО-СТАРИКОВСКИ"));
    EXPECT_TRUE(hasWF(result2, UniString("ПО-СТАРИКОВСКИ")));
    EXPECT_TRUE(hasNF(result2, UniString("ПО-СТАРИКОВСКИ")));
    EXPECT_TRUE(hasSP(result2, UniSPTag::ADV));

    std::vector<ParsedPtr> result3 = proc.analyze(UniString("ИНЖЕНЕРНО-ТЕХНИЧЕСКОМУ"));
    EXPECT_TRUE(hasWF(result3, UniString("ИНЖЕНЕРНО-ТЕХНИЧЕСКОМУ")));
    EXPECT_TRUE(hasNF(result3, UniString("ИНЖЕНЕРНО-ТЕХНИЧЕСКИЙ")));
    EXPECT_TRUE(hasSP(result3, UniSPTag::ADJ));

    std::vector<ParsedPtr> result4 = proc.analyze(UniString("ЧАК-ЧАКА"));
    EXPECT_TRUE(hasWF(result4, UniString("ЧАК-ЧАКА")));
    EXPECT_TRUE(hasNF(result4, UniString("ЧАК-ЧАК")));
    EXPECT_TRUE(hasSP(result4, UniSPTag::NOUN));

    std::vector<ParsedPtr> result5 = proc.analyze(UniString("КОНТР-АДМИРАЛ-ИНЖЕНЕРУ"));
    EXPECT_TRUE(hasWF(result5, UniString("КОНТР-АДМИРАЛ-ИНЖЕНЕРУ")));
    EXPECT_TRUE(hasNF(result5, UniString("КОНТР-АДМИРАЛ-ИНЖЕНЕР")));
    EXPECT_TRUE(hasSP(result5, UniSPTag::NOUN));
}
