#include <locale>
#include <gtest/gtest.h>
#include <xmorphy/utils/UniString.h>
#include <xmorphy/graphem/Tokenizer.h>
#include <xmorphy/morph/Processor.h>
#include <xmorphy/morph/WordForm.h>
#include <xmorphy/morph/DictMorphAnalyzer.h>

using namespace X;
using namespace std;

TEST(TestSyntesise, TestBase)
{
    UniString word("ШАРОМ");
    Tokenizer tok;
    Processor proc;
    auto tokres = tok.analyze(word);
    std::vector<WordFormPtr> anres = proc.analyze(tokres);
    std::vector<WordFormPtr> synres = proc.synthesize(word, UniMorphTag::Plur | UniMorphTag::Ins);
    EXPECT_NE(anres[0]->getMorphInfo().begin()->tag, synres[0]->getMorphInfo().begin()->tag);
    EXPECT_EQ(synres[0]->getWordForm(), UniString("ШАРАМИ"));

    std::vector<WordFormPtr> synres2 = proc.synthesize(anres[0], UniMorphTag::Plur | UniMorphTag::Nom);

    EXPECT_TRUE(synres2[0]->getMorphInfo().begin()->tag.contains(UniMorphTag::Plur | UniMorphTag::Nom));
    EXPECT_EQ(synres2[0]->getWordForm(), UniString("ШАРЫ"));

    std::vector<WordFormPtr> synres3 = proc.synthesize(UniString("белая"), UniMorphTag::Masc);
    for (size_t i = 0; i < synres3.size(); ++i)
        EXPECT_TRUE(synres3[i]->getMorphInfo().begin()->tag.contains(UniMorphTag::Masc));
    EXPECT_EQ(synres3[0]->getWordForm(), UniString("БЕЛЫЙ"));

    std::vector<WordFormPtr> synres4 = proc.synthesize(UniString("белая"), UniMorphTag::Masc | UniMorphTag::Gen);
    for (size_t i = 0; i < synres4.size(); ++i)
        EXPECT_TRUE(synres4[i]->getMorphInfo().begin()->tag.contains(UniMorphTag::Masc | UniMorphTag::Gen));
    EXPECT_EQ(synres4[0]->getWordForm(), UniString("БЕЛОГО"));

}

TEST(TestSyntesise, TestGenerate)
{
    UniString word("МЕТАН");
    DictMorphAnalyzer analyzer;
    std::vector<ParsedPtr> result = analyzer.generate(word);
    for (auto & parsed : result)
        EXPECT_EQ(parsed->normalform, word);
}
