#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>
#include <tag/UniSPTag.h>
#include <tag/MorphTag.h>
#include <tag/GraphemTag.h>
#include <tag/TokenTypeTag.h>
#include <graphem/Tokenizer.h>
#include <graphem/Token.h>
#include <morph/Processor.h>
#include <disamb/SingleWordDisambiguate.h>
#include <IO/OpCorporaIO.h>
#include <string>
#include <vector>
#include <iostream>

namespace py = pybind11;
using namespace base;
using namespace std;

struct MorphInfo
{
public:
    std::string normal_form;
    UniMorphTag tag = base::UniMorphTag::UNKN;
    UniSPTag sp = base::UniSPTag::X;
    double probability;
    AnalyzerTag analyzer;
public:

    const std::string & getNormalFrom() const {
        return normal_form;
    }

    void setNormalForm(const std::string & normal_form_)
    {
        normal_form = normal_form_;
    }

    const UniMorphTag & getTag() const {
        return tag;
    }

    void setTag(UniMorphTag tag_)
    {
        tag = tag_;
    }

    const UniSPTag & getSP() const {
        return sp;
    }

    void setSP(UniSPTag sp_) {
        sp = sp_;
    }

    const double getProbability() const {
        return probability;
    }

    void setProbability(double probability_)
    {
        probability = probability_;
    }

    const AnalyzerTag & getAnalyzerTag() const {
        return analyzer;
    }

    void setAnalyzerTag(AnalyzerTag analyzer_) {
        analyzer = analyzer_;
    }
};


struct WordForm
{
public:
    std::string word_form;
    std::vector<MorphInfo> infos;
    TokenTypeTag token_type = base::TokenTypeTag::UNKN;
    //GraphemTag graphem_info;
public:

    const std::string & getWordFrom() const
    {
        return word_form;
    }

    void setWordForm(const std::string & word_form_)
    {
        word_form = word_form_;
    }

    const std::vector<MorphInfo> & getInfos() const
    {
        return infos;
    }

    void setInfos(const std::vector<MorphInfo> & infos_)
    {
        infos = infos_;
    }

    const TokenTypeTag & getTokenType() const
    {
        return token_type;
    }

    void setTokenType(TokenTypeTag token_type_)
    {
        token_type = token_type_;
    }

    std::string toString() const
    {
        if (token_type & base::TokenTypeTag::SEPR) {
            return "";
        }

        std::ostringstream os;

        for (const auto& mi : infos) {
            os << word_form << "\t";
            os << mi.normal_form << "\t";
            os << mi.sp << "\t";
            os << mi.tag << "\t";
            os << mi.analyzer;
            os << "\n";
        }
        std::string result = os.str();
        result.pop_back();
        return result;
    }
};

class MorphAnalyzer
{
private:
    std::optional<tokenize::Tokenizer> tok;

    std::optional<analyze::Processor> analyzer;
    std::optional<disamb::SingleWordDisambiguate> disamb;

public:
    MorphAnalyzer()
    {
        [[maybe_unused]] static bool once = []() {
            boost::locale::generator gen;
            std::locale loc = gen("ru_RU.UTF8");
            std::locale::global(loc);
            return true;
        }();

        tok.emplace();
        analyzer.emplace();
        disamb.emplace();
    }
    std::vector<WordForm> analyze(const std::string & str, bool disambiguate=false)
    {
        std::vector<base::TokenPtr> tokens = tok->analyze(utils::UniString(str));
        std::vector<analyze::WordFormPtr> forms = analyzer->analyze(tokens);
        if (disambiguate)
            disamb->disambiguate(forms);

        std::vector<WordForm> result;
        for (auto wf_ptr : forms)
        {
            std::vector<MorphInfo> infos;
            for (const auto & info : wf_ptr->getMorphInfo())
            {
                MorphInfo new_info{
                    .normal_form = info.normalForm.toLowerCase().getRawString(),
                    .tag = info.tag,
                    .sp = info.sp,
                    .probability = info.probability,
                    .analyzer = info.at,
                };
                infos.push_back(new_info);
            }
            WordForm new_word_form{
                .word_form = wf_ptr->getWordForm().getRawString(),
                .infos = std::move(infos),
                .token_type = wf_ptr->getType(),
                    //.graphem_info = wf_ptr->getTag(),
            };
            result.emplace_back(new_word_form);
        }
        return result;
    }

};


PYBIND11_MODULE(pyxmorphy, m) {
    py::class_<UniSPTag>(m, "UniSPTag")
        .def_readonly_static("X", &UniSPTag::X)
        .def_readonly_static("ADJ", &UniSPTag::ADJ)
        .def_readonly_static("ADV", &UniSPTag::ADV)
        .def_readonly_static("INTJ", &UniSPTag::INTJ)
        .def_readonly_static("NOUN", &UniSPTag::NOUN)
        .def_readonly_static("PROPN", &UniSPTag::PROPN)
        .def_readonly_static("VERB", &UniSPTag::VERB)
        .def_readonly_static("ADP", &UniSPTag::ADP)
        .def_readonly_static("AUX", &UniSPTag::AUX)
        .def_readonly_static("CONJ", &UniSPTag::CONJ)
        .def_readonly_static("SCONJ", &UniSPTag::SCONJ)
        .def_readonly_static("DET", &UniSPTag::DET)
        .def_readonly_static("NUM", &UniSPTag::NUM)
        .def_readonly_static("PART", &UniSPTag::PART)
        .def_readonly_static("PRON", &UniSPTag::PRON)
        .def_readonly_static("PUNCT", &UniSPTag::PUNCT)
        .def_readonly_static("H", &UniSPTag::H)
        .def_readonly_static("R", &UniSPTag::R)
        .def_readonly_static("Q", &UniSPTag::Q)
        .def_readonly_static("SYM", &UniSPTag::SYM)
        .def("__str__", &UniSPTag::toString);

    py::class_<UniMorphTag>(m, "UniMorphTag")
        .def_readonly_static("UNKN", &UniMorphTag::UNKN)
        .def_readonly_static("Masc", &UniMorphTag::Masc)
        .def_readonly_static("Fem", &UniMorphTag::Fem)
        .def_readonly_static("Neut", &UniMorphTag::Neut)
        .def_readonly_static("Anim", &UniMorphTag::Anim)
        .def_readonly_static("Inan", &UniMorphTag::Inan)
        .def_readonly_static("Plur", &UniMorphTag::Plur)
        .def_readonly_static("Ins", &UniMorphTag::Ins)
        .def_readonly_static("Acc", &UniMorphTag::Acc)
        .def_readonly_static("Nom", &UniMorphTag::Nom)
        .def_readonly_static("Dat", &UniMorphTag::Dat)
        .def_readonly_static("Gen", &UniMorphTag::Gen)
        .def_readonly_static("Loc", &UniMorphTag::Loc)
        .def_readonly_static("Voc", &UniMorphTag::Voc)
        .def_readonly_static("Cmp", &UniMorphTag::Cmp)
        .def_readonly_static("Sup", &UniMorphTag::Sup)
        .def_readonly_static("Pos", &UniMorphTag::Pos)
        .def_readonly_static("Fin", &UniMorphTag::Fin)
        .def_readonly_static("Inf", &UniMorphTag::Inf)
        .def_readonly_static("Conv", &UniMorphTag::Conv)
        .def_readonly_static("Imp", &UniMorphTag::Imp)
        .def_readonly_static("Ind", &UniMorphTag::Ind)
        .def_readonly_static("_1", &UniMorphTag::_1)
        .def_readonly_static("_2", &UniMorphTag::_2)
        .def_readonly_static("_3", &UniMorphTag::_3)
        .def_readonly_static("Fut", &UniMorphTag::Fut)
        .def_readonly_static("Past", &UniMorphTag::Past)
        .def_readonly_static("Pres", &UniMorphTag::Pres)
        .def_readonly_static("Notpast", &UniMorphTag::Notpast)
        .def_readonly_static("Brev", &UniMorphTag::Brev)
        .def_readonly_static("Act", &UniMorphTag::Act)
        .def_readonly_static("Pass", &UniMorphTag::Pass)
        .def_readonly_static("Mid", &UniMorphTag::Mid)
        .def_readonly_static("Digit", &UniMorphTag::Digit)
        .def("__str__", &UniMorphTag::toString);

    py::class_<AnalyzerTag>(m, "AnalyzerTag")
        .def_readonly_static("UNKN", &AnalyzerTag::UNKN)
        .def_readonly_static("DICT", &AnalyzerTag::DICT)
        .def_readonly_static("PREF", &AnalyzerTag::PREF)
        .def_readonly_static("SUFF", &AnalyzerTag::SUFF)
        .def_readonly_static("HYPH", &AnalyzerTag::HYPH)
        .def("__str__", &AnalyzerTag::toString);

    py::class_<TokenTypeTag>(m, "TokenTypeTag")
        .def_readonly_static("UNKN", &TokenTypeTag::UNKN)
        .def_readonly_static("WORD", &TokenTypeTag::WORD)
        .def_readonly_static("PNCT", &TokenTypeTag::PNCT)
        .def_readonly_static("SEPR", &TokenTypeTag::SEPR)
        .def_readonly_static("NUMB", &TokenTypeTag::NUMB)
        .def_readonly_static("WRNM", &TokenTypeTag::WRNM)
        .def_readonly_static("HIER", &TokenTypeTag::HIER)
        .def("__str__", &TokenTypeTag::toString);

    py::class_<MorphInfo>(m, "MorphInfo")
        .def(py::init<>())
        .def_property("normal_form", &MorphInfo::getNormalFrom, &MorphInfo::setNormalForm)
        .def_property("sp", &MorphInfo::getSP, &MorphInfo::setSP)
        .def_property("tag", &MorphInfo::getTag, &MorphInfo::setTag)
        .def_property("probability", &MorphInfo::getProbability, &MorphInfo::setProbability)
        .def_property("analyzer_tag", &MorphInfo::getAnalyzerTag, &MorphInfo::setAnalyzerTag);

    py::class_<WordForm>(m, "WordForm")
        .def(py::init<>())
        .def_property("word_form", &WordForm::getWordFrom, &WordForm::setWordForm)
        .def_property("infos", &WordForm::getInfos, &WordForm::setInfos)
        .def_property("token_type", &WordForm::getTokenType, &WordForm::setInfos)
        .def("__str__", &WordForm::toString);

    py::class_<MorphAnalyzer>(m, "MorphAnalyzer")
        .def(py::init<>())
        .def("analyze", &MorphAnalyzer::analyze);
}
