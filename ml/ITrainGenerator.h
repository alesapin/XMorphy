#ifndef _I_TRAIN_GENERATOR
#define _I_TRAIN_GENERATOR
#include "../morph/Processor.h"
#include "../graphem/Tokenizer.h"
#include "../tag/OpCorporaUDConverter.h"
namespace ml {
using TrainPair = std::tuple<std::vector<analyze::WordFormPtr>, std::vector<base::ITag>>;
using TrainPairStr = std::tuple<std::vector<analyze::WordFormPtr>, std::vector<utils::UniString>>;

enum class TagType {
    UTAG,
    OPTAG
};

class
    ITrainGenerator {
protected:
    analyze::Processor proc;
    tokenize::Tokenizer tok;
    base::OpCorporaUDConverter conv;

    mutable std::uniform_real_distribution<double> unif;
    mutable std::random_device rd;
    mutable std::mt19937 randEngine;
    TagType tt;

    std::string name;
    double randDouble() const {
        return unif(randEngine);
    }

public:
    ITrainGenerator(const analyze::Processor& proc, const tokenize::Tokenizer& tok, const base::OpCorporaUDConverter& conv, const std::string& name, TagType tt = TagType::UTAG)
        : proc(proc)
        , tok(tok)
        , conv(conv)
        , unif(0, 1)
        , rd()
        , randEngine(rd())
        , tt(tt)
        , name(name)

    {
    }
    virtual void generate(std::istream& from, std::vector<TrainPair>& trainSet, std::vector<TrainPair>& testSet, double prob) const;
    virtual base::ITag getTag(base::ITag &sp, base::ITag &mt) const = 0;
    virtual void generate(std::istream& from, std::vector<TrainPairStr>& trainSet, std::vector<TrainPairStr>& testSet, double prob) const;

    template <typename T>
    std::tuple<std::vector<T>, std::vector<T>> split(const std::vector<T>& all, double prop) const {
        std::vector<T> train, test;
        for (const T& p : all) {
            if (randDouble() > prop) {
                train.push_back(p);
            } else {
                test.push_back(p);
            }
        }
        return std::make_tuple(train, test);
    }
};
}
#endif
