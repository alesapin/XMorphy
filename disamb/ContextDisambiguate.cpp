#include "ContextDisambiguate.h"
namespace disamb {
void ContextDisambiguator::filterInfos(analyze::WordFormPtr wf, std::function<bool(const analyze::MorphInfo&)> filt, std::function<void(analyze::MorphInfo&)> apply) const {
    std::set<analyze::MorphInfo>& infos = wf->getMorphInfo();
    if (infos.size() < 2) {
        return;
    }
    bool isAny = false;
    for (auto& info : infos) {
        if (!filt(info)) {
            isAny = true;
            break;
        }
    }
    if (!isAny) {
        std::vector<analyze::MorphInfo> chinfos(infos.begin(), infos.end());
        for (auto& info : chinfos) {
            apply(info);
        }
        wf->setMorphInfo(std::set<analyze::MorphInfo>(chinfos.begin(), chinfos.end()));
    } else {
        for (auto itr = infos.begin(); itr != infos.end();) {
            if (filt(*itr) && infos.size() > 1) {
                infos.erase(itr++);
            } else {
                itr++;
            }
        }
    }
}

void ContextDisambiguator::disambiguate(std::vector<analyze::WordFormPtr>& sequence) const {
    for (auto itr = sequence.begin(); itr != sequence.end();) {
        if (!((*itr)->getType() & (base::TokenTypeTag::WORD | base::TokenTypeTag::PNCT | base::TokenTypeTag::NUMB | base::TokenTypeTag::WRNM))) {
            itr = sequence.erase(itr);
        } else {
            ++itr;
        }
    }
    std::vector<base::SpeechPartTag> realSps = spcls.classify(sequence);
    for (std::size_t i = 0; i < sequence.size(); ++i) {
        filterInfos(sequence[i], [&](const analyze::MorphInfo& mi) { return mi.sp != realSps[i]; }, [&](analyze::MorphInfo& mi) { mi.sp = realSps[i]; });
    }
    std::vector<base::MorphTag> realGenders = gencls.classify(sequence);
    for (std::size_t i = 0; i < sequence.size(); ++i) {
        filterInfos(sequence[i], [&](const analyze::MorphInfo& mi) { return realGenders[i] != base::MorphTag::UNKN && !(mi.tag & realGenders[i]); }, [&](analyze::MorphInfo& mi) { mi.tag.setGender(realGenders[i]); });
    }
    std::vector<base::MorphTag> realNumbers = numcls.classify(sequence);
    for (std::size_t i = 0; i < sequence.size(); ++i) {
        filterInfos(sequence[i], [&](const analyze::MorphInfo& mi) { return realNumbers[i] != base::MorphTag::UNKN && !(mi.tag & realNumbers[i]); }, [&](analyze::MorphInfo& mi) { mi.tag.setNumber(realNumbers[i]); });
    }
    std::vector<base::MorphTag> realCases = casecls.classify(sequence);
    for (std::size_t i = 0; i < sequence.size(); ++i) {
        filterInfos(sequence[i], [&](const analyze::MorphInfo& mi) { return realCases[i] != base::MorphTag::UNKN && !(mi.tag & realCases[i]); }, [&](analyze::MorphInfo& mi) { mi.tag.setCase(realCases[i]); });
    }
}
}
