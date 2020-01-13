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
#include <string>
#include <vector>

namespace py = pybind11;
using namespace base;
using namespace std;

struct MorphInfo
{
    std::string normal_form;
    UniMorphTag tag;
    UniSPTag sp;
    double probability;
    //AnalyzerTag analyzer;
};


struct WordForm
{
    std::string word_form;
    std::vector<MorphInfo> infos;
    TokenTypeTag token_type;
    //GraphemTag graphem_info;
};

class MorphAnalyzer
{
    tokenize::Tokenizer tok;

    analyze::Processor analyzer;
    disamb::SingleWordDisambiguate disamb;

public:
    std::vector<WordForm> analyze(const std::string & str, bool disambiguate=false)
    {
        std::vector<base::TokenPtr> tokens = tok.analyze(utils::UniString(str));
        std::vector<analyze::WordFormPtr> forms = analyzer.analyze(tokens);
        if (disambiguate)
            disamb.disambiguate(forms);

        std::vector<WordForm> result;
        for (auto wf_ptr : forms)
        {
            std::vector<MorphInfo> infos;
            for (const auto & info : wf_ptr->getMorphInfo())
            {
                MorphInfo new_info{
                    .normal_form = info.normalForm.getRawString(),
                    .tag = info.tag,
                    .sp = info.sp,
                    .probability = info.probability,
                        //.analyzer = info.at,
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
        .def_readonly_static("SYM", &UniSPTag::SYM);

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
        .def_readonly_static("Digit", &UniMorphTag::Digit);

    py::class_<TokenTypeTag>(m, "TokenTypeTag")
        .def_readonly_static("UNKN", &TokenTypeTag::UNKN)
        .def_readonly_static("WORD", &TokenTypeTag::WORD)
        .def_readonly_static("PNCT", &TokenTypeTag::PNCT)
        .def_readonly_static("SEPR", &TokenTypeTag::SEPR)
        .def_readonly_static("NUMB", &TokenTypeTag::NUMB)
        .def_readonly_static("WRNM", &TokenTypeTag::WRNM)
        .def_readonly_static("HIER", &TokenTypeTag::HIER);

    py::class_<MorphInfo>(m, "MorphInfo");

    py::class_<WordForm>(m, "WordForm");

    py::class_<MorphAnalyzer>(m, "MorphAnalyzer")
        .def(py::init<>())
        .def("analyze", &MorphAnalyzer::analyze);
}
