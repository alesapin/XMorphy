#include <iostream>
#include <string>
#include <vector>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <xmorphy/graphem/Token.h>
#include <xmorphy/graphem/Tokenizer.h>
#include <xmorphy/ml/Disambiguator.h>
#include <xmorphy/ml/MorphemicSplitter.h>
#include <xmorphy/ml/SingleWordDisambiguate.h>
#include <xmorphy/morph/Processor.h>
#include <xmorphy/morph/WordFormPrinter.h>
#include <xmorphy/tag/GraphemTag.h>
#include <xmorphy/tag/MorphTag.h>
#include <xmorphy/tag/PhemTag.h>
#include <xmorphy/tag/TokenTypeTag.h>
#include <xmorphy/tag/UniMorphTag.h>
#include <xmorphy/tag/UniSPTag.h>
#include <xmorphy/tag/AnalyzerTag.h>

namespace py = pybind11;
using namespace std;

struct MorphInfo
{
public:
    std::string normal_form;
    X::UniMorphTag tag = X::UniMorphTag::UNKN;
    X::UniSPTag sp = X::UniSPTag::X;
    double probability;
    X::AnalyzerTag analyzer;
public:

    const std::string & getNormalFrom() const
    {
        return normal_form;
    }

    void setNormalForm(const std::string & normal_form_)
    {
        normal_form = normal_form_;
    }

    const X::UniMorphTag & getTag() const
    {
        return tag;
    }

    void setTag(X::UniMorphTag tag_)
    {
        tag = tag_;
    }

    const X::UniSPTag & getSP() const
    {
        return sp;
    }

    void setSP(X::UniSPTag sp_) {
        sp = sp_;
    }

    double getProbability() const
    {
        return probability;
    }

    void setProbability(double probability_)
    {
        probability = probability_;
    }

    const X::AnalyzerTag & getAnalyzerTag() const
    {
        return analyzer;
    }

    void setAnalyzerTag(X::AnalyzerTag analyzer_)
    {
        analyzer = analyzer_;
    }

    bool operator==(const MorphInfo & o) const
    {
        return std::tie(sp, tag, analyzer, probability, normal_form) ==
            std::tie(o.sp, o.tag, o.analyzer, o.probability, o.normal_form);
    }

    bool operator!=(const MorphInfo & o) const
    {
        return !(*this == o);
    }

    bool operator<(const MorphInfo & o) const
    {
        return std::tie(sp, tag, analyzer, probability, normal_form) <
               std::tie(o.sp, o.tag, o.analyzer, o.probability, o.normal_form);
    }

    bool operator>(const MorphInfo& o) const
    {
        return std::tie(sp, tag, analyzer, probability, normal_form) >
               std::tie(o.sp, o.tag, o.analyzer, o.probability, o.normal_form);
    }
};

struct WordForm
{
public:
    std::string word_form;
    std::vector<MorphInfo> infos;
    X::TokenTypeTag token_type = X::TokenTypeTag::UNKN;
    X::GraphemTag graphem_info = X::GraphemTag::UNKN;
    std::vector<X::PhemTag> phem_info;
public:

    bool operator==(const WordForm & o) const
    {
        if (o.infos.size() != infos.size())
            return false;
        for (size_t i = 0; i < o.infos.size(); ++i)
        {
            if (o.infos[i] != infos[i])
                return false;
        }
        return std::tie(word_form, token_type, graphem_info) == std::tie(o.word_form, o.token_type, o.graphem_info);
    }

    bool operator!=(const WordForm & o) const
    {
        return !(*this == o);
    }

    bool operator<(const WordForm& o) const
    {
        if (word_form >= o.word_form)
            return false;
        if (infos.size() >= o.infos.size())
            return false;
        if (!(token_type < o.token_type))
            return false;
        return graphem_info < o.graphem_info;
    }

    bool operator>(const WordForm& o) const
    {
        if (word_form > o.word_form)
            return true;
        if (infos.size() > o.infos.size())
            return true;

        for (size_t i = 0; i < infos.size(); ++i) {
            if (infos[i] < o.infos[i])
                return true;
            else if (infos[i] > o.infos[i])
                return false;
        }

        return token_type > o.token_type;
    }

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

    const X::TokenTypeTag & getTokenType() const
    {
        return token_type;
    }

    void setTokenType(X::TokenTypeTag token_type_)
    {
        token_type = token_type_;
    }

    const X::GraphemTag& getGraphemTag() const
    {
        return graphem_info;
    }

    void setGraphemTag(X::GraphemTag graphem_info_)
    {
        graphem_info = graphem_info_;
    }

    const std::vector<X::PhemTag> getPhemInfo() const
    {
        return phem_info;
    }

    void setPhemInfo(const std::vector<X::PhemTag> & phem_info_)
    {
        phem_info = phem_info_;
    }

    std::string toString() const
    {
        if (token_type & X::TokenTypeTag::SEPR)
            return "";

        std::ostringstream os;

        for (const auto& mi : infos)
        {
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
    std::optional<X::Tokenizer> tok;

    std::optional<X::Processor> analyzer;
    std::optional<X::SingleWordDisambiguate> disamb;
    std::optional<X::Disambiguator> context_disamb;
    std::optional<X::MorphemicSplitter> splitter;

public:
    MorphAnalyzer()
    {
        tok.emplace();
        analyzer.emplace();
        disamb.emplace();
        context_disamb.emplace();
        splitter.emplace();
    }
    std::vector<WordForm> analyze(const std::string& str, bool disambiguate_single = false, bool disambiguate_context = false, bool morphemic_split = false)
    {
        std::vector<X::TokenPtr> tokens = tok->analyze(utils::UniString(str));
        std::vector<X::WordFormPtr> forms = analyzer->analyze(tokens);
        if (disambiguate_single)
            disamb->disambiguate(forms);

        if (disambiguate_context)
            context_disamb->disambiguate(forms);


        std::vector<WordForm> result;
        for (auto wf_ptr : forms)
        {
            if (morphemic_split)
                splitter->split(wf_ptr);

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
                .graphem_info = wf_ptr->getTag(),
                .phem_info = wf_ptr->getPhemInfo(),
            };
            result.emplace_back(new_word_form);
        }
        return result;
    }

    WordForm analyzeSingleWord(const std::string & str, bool disambiguate = false, bool morphemic_split = false)
    {
        X::TokenPtr token = tok->analyzeSingleWord(utils::UniString(str));
        X::WordFormPtr form = analyzer->analyzeSingleToken(token);

        if (disambiguate)
            disamb->disambiguateSingleForm(form);

        if (morphemic_split)
            splitter->split(form);

        std::vector<MorphInfo> infos;
        for (const auto& info : form->getMorphInfo())
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
            .word_form = form->getWordForm().getRawString(),
            .infos = std::move(infos),
            .token_type = form->getType(),
            .graphem_info = form->getTag(),
            .phem_info = form->getPhemInfo(),
        };
        return new_word_form;
    }
};


PYBIND11_MODULE(pyxmorphy, m) {
    py::class_<X::UniSPTag>(m, "UniSPTag")
        .def_readonly_static("X", &X::UniSPTag::X)
        .def_readonly_static("ADJ", &X::UniSPTag::ADJ)
        .def_readonly_static("ADV", &X::UniSPTag::ADV)
        .def_readonly_static("INTJ", &X::UniSPTag::INTJ)
        .def_readonly_static("NOUN", &X::UniSPTag::NOUN)
        .def_readonly_static("PROPN", &X::UniSPTag::PROPN)
        .def_readonly_static("VERB", &X::UniSPTag::VERB)
        .def_readonly_static("ADP", &X::UniSPTag::ADP)
        .def_readonly_static("AUX", &X::UniSPTag::AUX)
        .def_readonly_static("CONJ", &X::UniSPTag::CONJ)
        .def_readonly_static("SCONJ", &X::UniSPTag::SCONJ)
        .def_readonly_static("DET", &X::UniSPTag::DET)
        .def_readonly_static("NUM", &X::UniSPTag::NUM)
        .def_readonly_static("PART", &X::UniSPTag::PART)
        .def_readonly_static("PRON", &X::UniSPTag::PRON)
        .def_readonly_static("PUNCT", &X::UniSPTag::PUNCT)
        .def_readonly_static("H", &X::UniSPTag::H)
        .def_readonly_static("R", &X::UniSPTag::R)
        .def_readonly_static("Q", &X::UniSPTag::Q)
        .def_readonly_static("SYM", &X::UniSPTag::SYM)
        .def("__str__", &X::UniSPTag::toString)
        .def("__eq__", &X::UniSPTag::operator==)
        .def("__ne__", &X::UniSPTag::operator!=)
        .def("__lt__", &X::UniSPTag::operator<)
        .def("__gt__", &X::UniSPTag::operator>);

    py::class_<X::UniMorphTag>(m, "UniMorphTag")
        .def_readonly_static("UNKN", &X::UniMorphTag::UNKN)
        .def_readonly_static("Masc", &X::UniMorphTag::Masc)
        .def_readonly_static("Fem", &X::UniMorphTag::Fem)
        .def_readonly_static("Neut", &X::UniMorphTag::Neut)
        .def_readonly_static("Anim", &X::UniMorphTag::Anim)
        .def_readonly_static("Inan", &X::UniMorphTag::Inan)
        .def_readonly_static("Sing", &X::UniMorphTag::Sing)
        .def_readonly_static("Plur", &X::UniMorphTag::Plur)
        .def_readonly_static("Ins", &X::UniMorphTag::Ins)
        .def_readonly_static("Acc", &X::UniMorphTag::Acc)
        .def_readonly_static("Nom", &X::UniMorphTag::Nom)
        .def_readonly_static("Dat", &X::UniMorphTag::Dat)
        .def_readonly_static("Gen", &X::UniMorphTag::Gen)
        .def_readonly_static("Loc", &X::UniMorphTag::Loc)
        .def_readonly_static("Voc", &X::UniMorphTag::Voc)
        .def_readonly_static("Cmp", &X::UniMorphTag::Cmp)
        .def_readonly_static("Sup", &X::UniMorphTag::Sup)
        .def_readonly_static("Pos", &X::UniMorphTag::Pos)
        .def_readonly_static("Fin", &X::UniMorphTag::Fin)
        .def_readonly_static("Inf", &X::UniMorphTag::Inf)
        .def_readonly_static("Conv", &X::UniMorphTag::Conv)
        .def_readonly_static("Imp", &X::UniMorphTag::Imp)
        .def_readonly_static("Ind", &X::UniMorphTag::Ind)
        .def_readonly_static("_1", &X::UniMorphTag::_1)
        .def_readonly_static("_2", &X::UniMorphTag::_2)
        .def_readonly_static("_3", &X::UniMorphTag::_3)
        .def_readonly_static("Fut", &X::UniMorphTag::Fut)
        .def_readonly_static("Past", &X::UniMorphTag::Past)
        .def_readonly_static("Pres", &X::UniMorphTag::Pres)
        .def_readonly_static("Notpast", &X::UniMorphTag::Notpast)
        .def_readonly_static("Short", &X::UniMorphTag::Short)
        .def_readonly_static("Act", &X::UniMorphTag::Act)
        .def_readonly_static("Pass", &X::UniMorphTag::Pass)
        .def_readonly_static("Mid", &X::UniMorphTag::Mid)
        .def_readonly_static("Digit", &X::UniMorphTag::Digit)
        .def("__eq__", &X::UniMorphTag::operator==)
        .def("__ne__", &X::UniMorphTag::operator!=)
        .def("__lt__", &X::UniMorphTag::operator<)
        .def("__gt__", &X::UniMorphTag::operator>)
        .def("__str__", &X::UniMorphTag::toString)
        .def("get_case", py::overload_cast<>(&X::UniMorphTag::getCase, py::const_))
        .def("get_number", py::overload_cast<>(&X::UniMorphTag::getNumber, py::const_))
        .def("get_gender", py::overload_cast<>(&X::UniMorphTag::getGender, py::const_))
        .def("get_tense", py::overload_cast<>(&X::UniMorphTag::getTense, py::const_))
        .def("get_animacy", py::overload_cast<>(&X::UniMorphTag::getAnimacy, py::const_));

    py::class_<X::GraphemTag>(m, "GraphemTag")
        .def_readonly_static("UNKN", &X::GraphemTag::UNKN)
        .def_readonly_static("CYRILLIC", &X::GraphemTag::CYRILLIC)
        .def_readonly_static("LATIN", &X::GraphemTag::LATIN)
        .def_readonly_static("UPPER_CASE", &X::GraphemTag::UPPER_CASE)
        .def_readonly_static("LOWER_CASE", &X::GraphemTag::LOWER_CASE)
        .def_readonly_static("MIXED", &X::GraphemTag::MIXED)
        .def_readonly_static("CAP_START", &X::GraphemTag::CAP_START)
        .def_readonly_static("ABBR", &X::GraphemTag::ABBR)
        .def_readonly_static("NAM_ENT", &X::GraphemTag::NAM_ENT)
        .def_readonly_static("MULTI_WORD", &X::GraphemTag::MULTI_WORD)
        .def_readonly_static("SINGLE_WORD", &X::GraphemTag::SINGLE_WORD)
        .def_readonly_static("COMMA", &X::GraphemTag::COMMA)
        .def_readonly_static("DOT", &X::GraphemTag::DOT)
        .def_readonly_static("COLON", &X::GraphemTag::COLON)
        .def_readonly_static("SEMICOLON", &X::GraphemTag::SEMICOLON)
        .def_readonly_static("QUESTION_MARK", &X::GraphemTag::QUESTION_MARK)
        .def_readonly_static("EXCLAMATION_MARK", &X::GraphemTag::EXCLAMATION_MARK)
        .def_readonly_static("THREE_DOTS", &X::GraphemTag::THREE_DOTS)
        .def_readonly_static("QUOTE", &X::GraphemTag::QUOTE)
        .def_readonly_static("DASH", &X::GraphemTag::DASH)
        .def_readonly_static("PARENTHESIS_L", &X::GraphemTag::PARENTHESIS_L)
        .def_readonly_static("PARENTHESIS_R", &X::GraphemTag::PARENTHESIS_R)
        .def_readonly_static("UNCOMMON_PUNCT", &X::GraphemTag::UNCOMMON_PUNCT)
        .def_readonly_static("PUNCT_GROUP", &X::GraphemTag::PUNCT_GROUP)
        .def_readonly_static("LOWER_DASH", &X::GraphemTag::LOWER_DASH)
        .def_readonly_static("DECIMAL", &X::GraphemTag::DECIMAL)
        .def_readonly_static("BINARY", &X::GraphemTag::BINARY)
        .def_readonly_static("OCT", &X::GraphemTag::OCT)
        .def_readonly_static("HEX", &X::GraphemTag::HEX)
        .def_readonly_static("SPACE", &X::GraphemTag::SPACE)
        .def_readonly_static("TAB", &X::GraphemTag::TAB)
        .def_readonly_static("NEW_LINE", &X::GraphemTag::NEW_LINE)
        .def_readonly_static("CR", &X::GraphemTag::CR)
        .def_readonly_static("SINGLE_SEP", &X::GraphemTag::SINGLE_SEP)
        .def_readonly_static("MULTI_SEP", &X::GraphemTag::MULTI_SEP)
        .def("__eq__", &X::GraphemTag::operator==)
        .def("__ne__", &X::GraphemTag::operator!=)
        .def("__lt__", &X::GraphemTag::operator<)
        .def("__gt__", &X::GraphemTag::operator>)
        .def("__str__", &X::GraphemTag::toString);

    py::class_<X::AnalyzerTag>(m, "AnalyzerTag")
        .def_readonly_static("UNKN", &X::AnalyzerTag::UNKN)
        .def_readonly_static("DICT", &X::AnalyzerTag::DICT)
        .def_readonly_static("PREF", &X::AnalyzerTag::PREF)
        .def_readonly_static("SUFF", &X::AnalyzerTag::SUFF)
        .def_readonly_static("HYPH", &X::AnalyzerTag::HYPH)
        .def("__eq__", &X::AnalyzerTag::operator==)
        .def("__ne__", &X::AnalyzerTag::operator!=)
        .def("__lt__", &X::AnalyzerTag::operator<)
        .def("__gt__", &X::AnalyzerTag::operator>)
        .def("__str__", &X::AnalyzerTag::toString);

    py::class_<X::TokenTypeTag>(m, "TokenTypeTag")
        .def_readonly_static("UNKN", &X::TokenTypeTag::UNKN)
        .def_readonly_static("WORD", &X::TokenTypeTag::WORD)
        .def_readonly_static("PNCT", &X::TokenTypeTag::PNCT)
        .def_readonly_static("SEPR", &X::TokenTypeTag::SEPR)
        .def_readonly_static("NUMB", &X::TokenTypeTag::NUMB)
        .def_readonly_static("WRNM", &X::TokenTypeTag::WRNM)
        .def_readonly_static("HIER", &X::TokenTypeTag::HIER)
        .def("__eq__", &X::TokenTypeTag::operator==)
        .def("__ne__", &X::TokenTypeTag::operator!=)
        .def("__lt__", &X::TokenTypeTag::operator<)
        .def("__gt__", &X::TokenTypeTag::operator>)
        .def("__str__", &X::TokenTypeTag::toString);

    py::class_<X::PhemTag>(m, "PhemTag")
        .def_readonly_static("UNKN", &X::PhemTag::UNKN)
        .def_readonly_static("PREF", &X::PhemTag::PREF)
        .def_readonly_static("ROOT", &X::PhemTag::ROOT)
        .def_readonly_static("SUFF", &X::PhemTag::SUFF)
        .def_readonly_static("END", &X::PhemTag::END)
        .def_readonly_static("LINK", &X::PhemTag::LINK)
        .def_readonly_static("HYPH", &X::PhemTag::HYPH)
        .def_readonly_static("POSTFIX", &X::PhemTag::POSTFIX)
        .def_readonly_static("B_SUFF", &X::PhemTag::B_SUFF)
        .def_readonly_static("B_PREF", &X::PhemTag::B_PREF)
        .def_readonly_static("B_ROOT", &X::PhemTag::B_ROOT)
        .def("__eq__", &X::PhemTag::operator==)
        .def("__ne__", &X::PhemTag::operator!=)
        .def("__lt__", &X::PhemTag::operator<)
        .def("__gt__", &X::PhemTag::operator>)
        .def("__str__", &X::PhemTag::toString);

    py::class_<MorphInfo>(m, "MorphInfo")
        .def(py::init<>())
        .def_property("normal_form", &MorphInfo::getNormalFrom, &MorphInfo::setNormalForm)
        .def_property("sp", &MorphInfo::getSP, &MorphInfo::setSP)
        .def_property("tag", &MorphInfo::getTag, &MorphInfo::setTag)
        .def_property("probability", &MorphInfo::getProbability, &MorphInfo::setProbability)
        .def_property("analyzer_tag", &MorphInfo::getAnalyzerTag, &MorphInfo::setAnalyzerTag)
        .def("__eq__", &MorphInfo::operator==)
        .def("__ne__", &MorphInfo::operator!=)
        .def("__lt__", &MorphInfo::operator<)
        .def("__gt__", &MorphInfo::operator>);

    py::class_<WordForm>(m, "WordForm")
        .def(py::init<>())
        .def_property("word_form", &WordForm::getWordFrom, &WordForm::setWordForm)
        .def_property("infos", &WordForm::getInfos, &WordForm::setInfos)
        .def_property("token_type", &WordForm::getTokenType, &WordForm::setInfos)
        .def_property("graphem_info", &WordForm::getGraphemTag, &WordForm::setGraphemTag)
        .def_property("phem_info", &WordForm::getPhemInfo, &WordForm::setPhemInfo)
        .def("__eq__", &WordForm::operator==)
        .def("__ne__", &WordForm::operator!=)
        .def("__lt__", &WordForm::operator<)
        .def("__gt__", &WordForm::operator>)
        .def("__str__", &WordForm::toString);

    py::class_<MorphAnalyzer>(m, "MorphAnalyzer")
        .def(py::init<>())
        .def("analyze", &MorphAnalyzer::analyze)
        .def("analyze_single_word", &MorphAnalyzer::analyzeSingleWord);
}
