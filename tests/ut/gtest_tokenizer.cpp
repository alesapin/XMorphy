#include <gtest/gtest.h>
#include <utils/UniString.h>
#include <graphem/Tokenizer.h>
#include <graphem/Token.h>
#include <tag/GraphemTag.h>
#include <tag/TokenTypeTag.h>

TEST(TestTokenize, TestWord) {
    using namespace utils;
    using namespace X;
    Tokenizer tok;
    UniString hi("Привет");
    GraphemTag t = GraphemTag::CAP_START | GraphemTag::CYRILLIC;
    std::vector<std::shared_ptr<Token>> result = tok.analyze(hi);
    EXPECT_EQ(result.size(),1);
    EXPECT_EQ(result[0]->getType(), TokenTypeTag::WORD);
    EXPECT_EQ(result[0]->toString(), hi);
    EXPECT_EQ(result[0]->getTag(), t);

    UniString ohi("ПрИвЕт");
    std::vector<std::shared_ptr<Token>> r = tok.analyze(ohi);
    EXPECT_EQ(r.size(), 1);
    GraphemTag ot = GraphemTag::MIXED;
    ot |= GraphemTag::CYRILLIC;
    EXPECT_EQ(r[0]->getType(), TokenTypeTag::WORD);
    EXPECT_EQ(r[0]->toString(), ohi);
    EXPECT_EQ(r[0]->getTag(), ot);

    UniString hi1("ПРИВЕТ");
    std::vector<std::shared_ptr<Token>> r1 = tok.analyze(hi1);
    EXPECT_EQ(r1.size(), 1);
    EXPECT_EQ(r1[0]->getType(), TokenTypeTag::WORD);
    GraphemTag t1 = GraphemTag::UPPER_CASE;
    t1 |= GraphemTag::CYRILLIC;
    EXPECT_EQ(r1[0]->toString(), hi1);
    EXPECT_EQ(r1[0]->getTag(), t1);


    UniString hi2("HeLLo");
    std::vector<std::shared_ptr<Token>> r2 = tok.analyze(hi2);
    EXPECT_EQ(r2.size(), 1);
    EXPECT_EQ(r2[0]->getType(), TokenTypeTag::WORD);
    GraphemTag t2 = GraphemTag::MIXED;
    t2 |= GraphemTag::LATIN;
    EXPECT_EQ(r2[0]->toString(), hi2);
    EXPECT_EQ(r2[0]->getTag(), t2);

    UniString hi3("приllo");
    std::vector<std::shared_ptr<Token>> r3 = tok.analyze(hi3);
    EXPECT_EQ(r3.size(), 1);
    EXPECT_EQ(r3[0]->getType(), TokenTypeTag::WORD);
    GraphemTag t3 = GraphemTag::LOWER_CASE;
    t3 |= GraphemTag::MULTI_ENC;
    EXPECT_EQ(r3[0]->toString(), hi3);
    EXPECT_EQ(r3[0]->getTag(), t3);

    UniString hi4("при123");
    std::vector<std::shared_ptr<Token>> r4 = tok.analyze(hi4);
    EXPECT_EQ(r4.size(), 1);
    EXPECT_EQ(r4[0]->getType(), TokenTypeTag::WRNM);
    EXPECT_EQ(r4[0]->toString(), hi4);
    EXPECT_EQ(r4[0]->getTag(), GraphemTag::CYRILLIC);
}

TEST(TestTokenize, TestNumber) {
    using namespace utils;
    using namespace X;
    X::Tokenizer tok;

    UniString n("1234");
    GraphemTag t = GraphemTag::DECIMAL;
    std::vector<std::shared_ptr<Token>> r = tok.analyze(n);
    EXPECT_EQ(r.size(), 1);
    EXPECT_EQ(r[0]->toString(), n);
    EXPECT_EQ(r[0]->getTag(), t);
}

TEST(TestTokenize, TestPunct){
    using namespace utils;
    using namespace X;
    X::Tokenizer tok;

    UniString p(",");
    GraphemTag t = GraphemTag::COMMA;
    std::vector<std::shared_ptr<Token>> r = tok.analyze(p);
    EXPECT_EQ(r.size(), 1);
    EXPECT_EQ(r[0]->toString(), p);
    EXPECT_EQ(r[0]->getTag(), t);

    UniString p1("...");
    GraphemTag t1 = GraphemTag::THREE_DOTS;
    t1 |= GraphemTag::PUNCT_GROUP;
    std::vector<std::shared_ptr<Token>> r1 = tok.analyze(p1);
    EXPECT_EQ(r1.size(), 1);
    EXPECT_EQ(r1[0]->toString(), p1);
    EXPECT_EQ(r1[0]->getTag(), t1);

    UniString p3("?!");
    GraphemTag t3 = GraphemTag::QUESTION_MARK;
    std::vector<std::shared_ptr<Token>> r3 = tok.analyze(p3);
    EXPECT_EQ(r3.size(), 2);
    EXPECT_EQ(r3[0]->toString(), utils::UniString("?"));
    EXPECT_EQ(r3[0]->getTag(), t3);

    UniString p4("----");
    GraphemTag t4 = GraphemTag::PUNCT_GROUP | GraphemTag::DASH;
    std::vector<std::shared_ptr<Token>> r4 = tok.analyze(p4);
    EXPECT_EQ(r4.size(), 1);
    EXPECT_EQ(r4[0]->toString(), utils::UniString("----"));
    EXPECT_EQ(r4[0]->getTag(), t4);

}

TEST(TestTokenize, TestSeparator) {
    using namespace utils;
    using namespace X;
    X::Tokenizer tok;

    UniString s(" ");
    GraphemTag t = GraphemTag::SPACE;
    t |= GraphemTag::SINGLE_SEP;
    std::vector<std::shared_ptr<Token>> r = tok.analyze(s);
    EXPECT_EQ(r.size(), 1);
    EXPECT_EQ(r[0]->toString(), s);
    EXPECT_EQ(r[0]->getTag(), t);

    UniString s1("    ");
    GraphemTag t1 = GraphemTag::MULTI_SEP;
    std::vector<std::shared_ptr<Token>> r1 = tok.analyze(s1);
    EXPECT_EQ(r1.size(), 1);
    EXPECT_EQ(r1[0]->toString(), s1);
    EXPECT_EQ(r1[0]->getTag(), t1);

    UniString s2(" \n");
    GraphemTag t2 = GraphemTag::MULTI_SEP;
    std::vector<std::shared_ptr<Token>> r2 = tok.analyze(s2);
    EXPECT_EQ(r2.size(), 1);
    EXPECT_EQ(r2[0]->toString(), s2);
    EXPECT_EQ(r2[0]->getTag(), t2);
}

TEST(TestTokenize, TestSerious) {
    using namespace utils;
    using namespace X;
    Tokenizer tok;

    UniString s("Привет МУНК 123. Я,было11 good... ツ\n");

    std::vector<std::shared_ptr<Token>> r = tok.analyze(s);
    EXPECT_EQ(r.size(), 16);

    UniString fWord("Привет");
    EXPECT_EQ(r[0]->toString(), fWord);
    EXPECT_EQ(r[0]->getTag(), (GraphemTag::CAP_START | GraphemTag::CYRILLIC));

    UniString sps(" ");
    EXPECT_EQ(r[1]->toString(), sps);
    EXPECT_EQ(r[1]->getTag(), (GraphemTag::SPACE | GraphemTag::SINGLE_SEP)) ;

    UniString mih("МУНК");
    EXPECT_EQ(r[2]->toString(), mih);
    EXPECT_EQ(r[2]->getTag(), (GraphemTag::UPPER_CASE | GraphemTag::CYRILLIC));

    UniString tte("123");
    EXPECT_EQ(r[4]->toString(), tte);
    EXPECT_EQ(r[4]->getTag(), GraphemTag::DECIMAL);

    UniString dt(".");
    EXPECT_EQ(r[5]->toString(), dt);
    EXPECT_EQ(r[5]->getTag(), GraphemTag::DOT);

    UniString w("Я");
    EXPECT_EQ(r[7]->toString(), w);
    EXPECT_EQ(r[7]->getTag(), (GraphemTag::UPPER_CASE | GraphemTag::CYRILLIC) );

    UniString com(",");
    EXPECT_EQ(r[8]->toString(), com);
    EXPECT_EQ(r[8]->getTag(), GraphemTag::COMMA);

    UniString bs("было11");
    EXPECT_EQ(r[9]->toString(), bs);
    EXPECT_EQ(r[9]->getType(), TokenTypeTag::WRNM);
    EXPECT_EQ(r[9]->getTag(), GraphemTag::CYRILLIC);

    UniString g("good");
    EXPECT_EQ(r[11]->toString(), g);
    EXPECT_EQ(r[11]->getTag(), (GraphemTag::LOWER_CASE | GraphemTag::LATIN));

    UniString ddd("...");
    EXPECT_EQ(r[12]->toString(), ddd);
    EXPECT_EQ(r[12]->getTag(), (GraphemTag::THREE_DOTS | GraphemTag::PUNCT_GROUP));

    UniString hir("ツ");
    EXPECT_EQ(r[14]->toString(), hir);
    EXPECT_EQ(r[14]->getType(), TokenTypeTag::WORD);
    EXPECT_EQ(r[14]->getTag(), GraphemTag::UPPER_CASE);

    UniString last("\n");
    EXPECT_EQ(r[15]->toString(), last);
    EXPECT_EQ(r[15]->getTag(), (GraphemTag::NEW_LINE | GraphemTag::SINGLE_SEP));

    UniString p4("«Если тебе повезло, и ты в молодости жил в Париже, то, где бы ты ни был потом, он до конца дней твоих останется с тобой….» («Праздник, который всегда с тобой», 1960). Платонова раздражает, с моральной точки зрения, описание первой встречи лейтенанта Генри с медсестрой Кетрин Баркли — «циничный, грубоватый лаконизм изложения, «мужественное» пренебрежение первой пощёчиной, «многоопытная» уверенность в близком поцелуе» и т.д");
    std::vector<std::shared_ptr<Token>> r2 = tok.analyze(p4);
    int counter = 0;
    for(size_t i = 0; i < r2.size(); ++i){
        if(r2[i]->getType() & TokenTypeTag::PNCT){
            TokenPtr pnct = r2[i];
            if (pnct->getTag() & GraphemTag::DOT){
                counter++;
            }
        }
    }
    EXPECT_EQ(counter, 3);
}


TEST(TestTokenize, TestHyph) {
    using namespace utils;
    using namespace X;
    Tokenizer tok;
    UniString s("светло-синий");

    auto results = tok.analyze(s);
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(results[0]->getType(), TokenTypeTag::WORD);
    EXPECT_EQ(results[0]->getTag(), GraphemTag::CONNECTED | GraphemTag::LOWER_CASE | GraphemTag::CYRILLIC);

    UniString s1("Dry-cleaning");
    auto results1 = tok.analyze(s1);
    EXPECT_EQ(results1.size(), 1);
    EXPECT_EQ(results1[0]->getType(), TokenTypeTag::WORD);
    EXPECT_EQ(results1[0]->getTag(), GraphemTag::CONNECTED | GraphemTag::CAP_START | GraphemTag::LATIN);

    UniString s2("Комсомольск-на-Амуре");
    auto results2 = tok.analyze(s2);
    EXPECT_EQ(results2.size(), 1);
    EXPECT_EQ(results2[0]->getType(), TokenTypeTag::WORD);
    EXPECT_EQ(results2[0]->getTag(), GraphemTag::CONNECTED | GraphemTag::MIXED | GraphemTag::CYRILLIC);


    UniString s3("Ил-2");
    auto results3 = tok.analyze(s3);
    EXPECT_EQ(results3.size(), 1);
    EXPECT_EQ(results3[0]->getType(), TokenTypeTag::WRNM);
    EXPECT_EQ(results3[0]->getTag(), GraphemTag::CONNECTED | GraphemTag::CYRILLIC);

    UniString s4("2019-10-01");
    auto results4 = tok.analyze(s4);

    EXPECT_EQ(results4.size(), 1);
    EXPECT_EQ(results4[0]->getType(), TokenTypeTag::NUMB);
    EXPECT_EQ(results4[0]->getTag(), GraphemTag::CONNECTED | GraphemTag::DECIMAL);


    UniString s5("Книга - планшет");
    auto results5 = tok.analyze(s5);
    EXPECT_EQ(results5.size(), 5);
}
