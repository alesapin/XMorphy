#include <locale>
#include <gtest/gtest.h>
#include <xmorphy/utils/UniString.h>
#include <xmorphy/graphem/Tokenizer.h>
#include <xmorphy/morph/Processor.h>
#include <xmorphy/morph/WordForm.h>

using namespace X;
using namespace std;

TEST(TestSyntesise, TestBase)
{
    utils::UniString word("ШАРОМ");
    Tokenizer tok;
    Processor proc;
    auto tokres = tok.analyze(word);
    std::vector<WordFormPtr> anres = proc.analyze(tokres);
    std::vector<WordFormPtr> synres = proc.synthesize(word, UniMorphTag::Plur | UniMorphTag::Ins);
    EXPECT_NE(anres[0]->getMorphInfo().begin()->tag, synres[0]->getMorphInfo().begin()->tag);
    EXPECT_EQ(synres[0]->getWordForm(), utils::UniString("ШАРАМИ"));

    std::vector<WordFormPtr> synres2 = proc.synthesize(anres[0], UniMorphTag::Plur | UniMorphTag::Nom);

    EXPECT_TRUE(synres2[0]->getMorphInfo().begin()->tag.contains(UniMorphTag::Plur | UniMorphTag::Nom));
    EXPECT_EQ(synres2[0]->getWordForm(), utils::UniString("ШАРЫ"));

    std::vector<WordFormPtr> synres3 = proc.synthesize(utils::UniString("белая"), UniMorphTag::Masc);
    for (size_t i = 0; i < synres3.size(); ++i)
        EXPECT_TRUE(synres3[i]->getMorphInfo().begin()->tag.contains(UniMorphTag::Masc));
    EXPECT_EQ(synres3[0]->getWordForm(), utils::UniString("БЕЛЫЙ"));
}
