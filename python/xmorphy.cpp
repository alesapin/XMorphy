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
#include <ml/Disambiguator.h>
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

    bool operator==(const MorphInfo & o) const {
        return std::tie(sp, tag, analyzer, probability, normal_form) ==
            std::tie(o.sp, o.tag, o.analyzer, o.probability, o.normal_form);
    }
    bool operator!=(const MorphInfo & o) const {
        return !(*this == o);
    }

    bool operator<(const MorphInfo & o) const
    {
        return std::tie(sp, tag, analyzer, probability, normal_form) <
               std::tie(o.sp, o.tag, o.analyzer, o.probability, o.normal_form);
    }

    bool operator>(const MorphInfo& o) const {
        return std::tie(sp, tag, analyzer, probability, normal_form) >
               std::tie(o.sp, o.tag, o.analyzer, o.probability, o.normal_form);
    }
};


struct WordForm
{
public:
    std::string word_form;
    std::vector<MorphInfo> infos;
    TokenTypeTag token_type = base::TokenTypeTag::UNKN;
    GraphemTag graphem_info = base::GraphemTag::UNKN;
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

    bool operator<(const WordForm& o) const {
        if (word_form >= o.word_form)
            return false;
        if (infos.size() >= o.infos.size())
            return false;
        if (!(token_type < o.token_type))
            return false;
        return graphem_info < o.graphem_info;
    }

    bool operator>(const WordForm& o) const {
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

    const TokenTypeTag & getTokenType() const
    {
        return token_type;
    }

    void setTokenType(TokenTypeTag token_type_)
    {
        token_type = token_type_;
    }

    const GraphemTag& getGraphemTag() const {
        return graphem_info;
    }

    void setGraphemTag(GraphemTag graphem_info_)
    {
        graphem_info = graphem_info_;
    }

    std::string toString() const {
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
    std::optional<ml::Disambiguator> context_disamb;

public:
    MorphAnalyzer()
    {
        tok.emplace();
        analyzer.emplace();
        disamb.emplace();
        context_disamb.emplace();
    }
    std::vector<WordForm> analyze(const std::string& str, bool disambiguate_single=false, bool disambiguate_context = false) {
        std::vector<base::TokenPtr> tokens = tok->analyze(utils::UniString(str));
        std::vector<analyze::WordFormPtr> forms = analyzer->analyze(tokens);
        if (disambiguate_single)
            disamb->disambiguate(forms);

        if (disambiguate_context)
            context_disamb->disambiguate(forms);

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
                .graphem_info = wf_ptr->getTag(),
            };
            result.emplace_back(new_word_form);
        }
        return result;
    }

    WordForm analyzeSingleWord(const std::string & str, bool disambiguate)
    {
        base::TokenPtr token = tok->analyzeSingleWord(utils::UniString(str));
        analyze::WordFormPtr form = analyzer->analyzeSingleToken(token);

        if (disambiguate)
            disamb->disambiguateSingleForm(form);

        std::vector<MorphInfo> infos;
        for (const auto& info : form->getMorphInfo()) {
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
        };
        return new_word_form;
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
        .def("__str__", &UniSPTag::toString)
        .def("__eq__", &UniSPTag::operator==)
        .def("__ne__", &UniSPTag::operator!=)
        .def("__lt__", &UniSPTag::operator<)
        .def("__gt__", &UniSPTag::operator>);

    py::class_<UniMorphTag>(m, "UniMorphTag")
        .def_readonly_static("UNKN", &UniMorphTag::UNKN)
        .def_readonly_static("Masc", &UniMorphTag::Masc)
        .def_readonly_static("Fem", &UniMorphTag::Fem)
        .def_readonly_static("Neut", &UniMorphTag::Neut)
        .def_readonly_static("Anim", &UniMorphTag::Anim)
        .def_readonly_static("Inan", &UniMorphTag::Inan)
        .def_readonly_static("Sing", &UniMorphTag::Sing)
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
        .def("__eq__", &UniMorphTag::operator==)
        .def("__ne__", &UniMorphTag::operator!=)
        .def("__lt__", &UniMorphTag::operator<)
        .def("__gt__", &UniMorphTag::operator>)
        .def("__str__", &UniMorphTag::toString)
        .def("get_case", py::overload_cast<>(&UniMorphTag::getCase, py::const_))
        .def("get_number", py::overload_cast<>(&UniMorphTag::getNumber, py::const_))
        .def("get_gender", py::overload_cast<>(&UniMorphTag::getGender, py::const_))
        .def("get_tense", py::overload_cast<>(&UniMorphTag::getTense, py::const_))
        .def("get_animacy", py::overload_cast<>(&UniMorphTag::getAnimacy, py::const_));

    py::class_<GraphemTag>(m, "GraphemTag")
        .def_readonly_static("UNKN", &GraphemTag::UNKN)
        .def_readonly_static("CYRILLIC", &GraphemTag::CYRILLIC)
        .def_readonly_static("LATIN", &GraphemTag::LATIN)
        .def_readonly_static("UPPER_CASE", &GraphemTag::UPPER_CASE)
        .def_readonly_static("LOWER_CASE", &GraphemTag::LOWER_CASE)
        .def_readonly_static("MIXED", &GraphemTag::MIXED)
        .def_readonly_static("CAP_START", &GraphemTag::CAP_START)
        .def_readonly_static("ABBR", &GraphemTag::ABBR)
        .def_readonly_static("NAM_ENT", &GraphemTag::NAM_ENT)
        .def_readonly_static("MULTI_WORD", &GraphemTag::MULTI_WORD)
        .def_readonly_static("SINGLE_WORD", &GraphemTag::SINGLE_WORD)
        .def_readonly_static("COMMA", &GraphemTag::COMMA)
        .def_readonly_static("DOT", &GraphemTag::DOT)
        .def_readonly_static("COLON", &GraphemTag::COLON)
        .def_readonly_static("SEMICOLON", &GraphemTag::SEMICOLON)
        .def_readonly_static("QUESTION_MARK", &GraphemTag::QUESTION_MARK)
        .def_readonly_static("EXCLAMATION_MARK", &GraphemTag::EXCLAMATION_MARK)
        .def_readonly_static("THREE_DOTS", &GraphemTag::THREE_DOTS)
        .def_readonly_static("QUOTE", &GraphemTag::QUOTE)
        .def_readonly_static("DASH", &GraphemTag::DASH)
        .def_readonly_static("PARENTHESIS_L", &GraphemTag::PARENTHESIS_L)
        .def_readonly_static("PARENTHESIS_R", &GraphemTag::PARENTHESIS_R)
        .def_readonly_static("UNCOMMON_PUNCT", &GraphemTag::UNCOMMON_PUNCT)
        .def_readonly_static("PUNCT_GROUP", &GraphemTag::PUNCT_GROUP)
        .def_readonly_static("LOWER_DASH", &GraphemTag::LOWER_DASH)
        .def_readonly_static("DECIMAL", &GraphemTag::DECIMAL)
        .def_readonly_static("BINARY", &GraphemTag::BINARY)
        .def_readonly_static("OCT", &GraphemTag::OCT)
        .def_readonly_static("HEX", &GraphemTag::HEX)
        .def_readonly_static("SPACE", &GraphemTag::SPACE)
        .def_readonly_static("TAB", &GraphemTag::TAB)
        .def_readonly_static("NEW_LINE", &GraphemTag::NEW_LINE)
        .def_readonly_static("CR", &GraphemTag::CR)
        .def_readonly_static("SINGLE_SEP", &GraphemTag::SINGLE_SEP)
        .def_readonly_static("MULTI_SEP", &GraphemTag::MULTI_SEP)
        .def("__eq__", &GraphemTag::operator==)
        .def("__ne__", &GraphemTag::operator!=)
        .def("__lt__", &GraphemTag::operator<)
        .def("__gt__", &GraphemTag::operator>)
        .def("__str__", &GraphemTag::toString);

    py::class_<AnalyzerTag>(m, "AnalyzerTag")
        .def_readonly_static("UNKN", &AnalyzerTag::UNKN)
        .def_readonly_static("DICT", &AnalyzerTag::DICT)
        .def_readonly_static("PREF", &AnalyzerTag::PREF)
        .def_readonly_static("SUFF", &AnalyzerTag::SUFF)
        .def_readonly_static("HYPH", &AnalyzerTag::HYPH)
        .def("__eq__", &AnalyzerTag::operator==)
        .def("__ne__", &AnalyzerTag::operator!=)
        .def("__lt__", &AnalyzerTag::operator<)
        .def("__gt__", &AnalyzerTag::operator>)
        .def("__str__", &AnalyzerTag::toString);

    py::class_<TokenTypeTag>(m, "TokenTypeTag")
        .def_readonly_static("UNKN", &TokenTypeTag::UNKN)
        .def_readonly_static("WORD", &TokenTypeTag::WORD)
        .def_readonly_static("PNCT", &TokenTypeTag::PNCT)
        .def_readonly_static("SEPR", &TokenTypeTag::SEPR)
        .def_readonly_static("NUMB", &TokenTypeTag::NUMB)
        .def_readonly_static("WRNM", &TokenTypeTag::WRNM)
        .def_readonly_static("HIER", &TokenTypeTag::HIER)
        .def("__eq__", &TokenTypeTag::operator==)
        .def("__ne__", &TokenTypeTag::operator!=)
        .def("__lt__", &TokenTypeTag::operator<)
        .def("__gt__", &TokenTypeTag::operator>)
        .def("__str__", &TokenTypeTag::toString);

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
