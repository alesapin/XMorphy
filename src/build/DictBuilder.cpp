#include "DictBuilder.h"
namespace build {

void DictBuilder::buildMorphDict(std::unique_ptr<MorphDict>& dict, const RawDict & rd) {
    LoadFunc dictLoader = std::bind(&DictBuilder::mainDictLoader, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

    auto mainDict = loadClassicDict(rd, dictLoader, [](std::map<std::string, ParaPairArray>&) {});
    dict = utils::make_unique<MorphDict>(encPars, mainDict, prefs, tags, sufs);
}

void DictBuilder::mainDictLoader(std::map<std::string, ParaPairArray>& m, const WordsArray& w, const TagsArray& t) const {
    Paradigm p = parseOnePara(w, t);
    for (std::size_t i = 0; i < w.size(); ++i) {
        m[w[i].getRawString()].data.emplace_back(ParaPair{paras.at(p).first, i, paras.at(p).second});
    }
}

DictPtr DictBuilder::loadClassicDict(
    const RawDict & rd, LoadFunc loader, FilterFunc filter) const {

    dawg::BuildFactory<ParaPairArray> factory;
    std::map<std::string, ParaPairArray> allData;
    std::size_t counter = 0;
    for (std::size_t i = 0; i < rd.size(); ++i) {
        auto [words, tags] = rd[i];
        if (!words.empty()) {
            loader(allData, words, tags);
        }
        counter += 1;
        if (counter % 1000 == 0) {
            std::cerr << "Dict loading: " << counter << std::endl;
        }
    }
    filter(allData);
    for (auto itr : allData) {
        factory.insertOrLink(itr.first, itr.second);
    }
    return factory.build();
}

void DictBuilder::suffixDictLoader(std::map<std::string, ParaPairArray>& m, const WordsArray& w, const TagsArray& t) const {
    Paradigm p = parseOnePara(w, t);
    base::UniSPTag sp = std::get<1>(p[0]);
    if (paras.at(p).second < minParaCount || base::FIXED_UNISPS.count(sp)) {
        return;
    }
    std::size_t paranumCur = paras.at(p).first;
    for (std::size_t i = 0; i < w.size(); ++i) {
        for (std::size_t j = 1; j <= 5; ++j) {
            if (j < w[i].length()) {
                utils::UniString cut = w[i].rcut(j);
                std::string raw = cut.getRawString();
                bool found = false;
                for (auto& para : m[raw].data) {
                    if (para.paraNum == paranumCur) {
                        para.freq++;
                        found = true;
                    }
                }
                if (!found) {
                    m[raw].data.push_back(ParaPair{paranumCur, i, 1});
                }
            }
        }
    }
}

void DictBuilder::filterSuffixDict(std::map<std::string, ParaPairArray>& m) const {
    for (auto& pair : m) { // clear not frequent
        for (auto itr = pair.second.data.begin(); itr != pair.second.data.end();) {
            if (itr->freq <= minFlexFreq) {
                itr = pair.second.data.erase(itr);
            } else {
                ++itr;
            }
        }
    }
    for (auto& pair : m) {
        std::map<base::UniSPTag, std::pair<std::size_t, std::size_t>> counter;
        for (std::size_t i = 0; i < pair.second.data.size(); ++i) {
            ParaPair p = pair.second.data[i];
            EncodedLexemeGroup g = encPars[p.paraNum][p.formNum];
            TagPair tp = tags.right.at(std::get<1>(g));
            if (counter.count(tp.first) == 0 || counter[tp.first].first < p.freq) {
                counter[tp.first] = std::make_pair(p.freq, i);
            }
        }
        ParaPairArray newParas;
        for (auto itr : counter) {
            for (std::size_t i = 0; i < pair.second.data.size(); ++i) {
                if (pair.second.data[i].paraNum == pair.second.data[itr.second.second].paraNum) {
                    newParas.data.push_back(pair.second.data[i]);
                }
            }
        }
        pair.second.data = newParas.data;
    }
    for (auto itr = m.begin(); itr != m.end();) {
        if (itr->second.data.empty()) {
            itr = m.erase(itr);
        } else {
            itr++;
        }
    }
}

void DictBuilder::buildSuffixDict(std::unique_ptr<SuffixDict>& dict, const RawDict & rd) {
    LoadFunc dictLoader = std::bind(&DictBuilder::suffixDictLoader, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    FilterFunc filter = std::bind(&DictBuilder::filterSuffixDict, this, std::placeholders::_1);
    auto suffixDict = loadClassicDict(rd, dictLoader, filter);
    dict = utils::make_unique<SuffixDict>(encPars, suffixDict);
}

PrefixDict loadPrefixDict(std::istream & is) 
{
    std::string row;
    std::set<utils::UniString> result;
    while (std::getline(is, row)) {
        result.insert(utils::UniString(row));
    }
    return result;
}

void buildDisambDict(std::unique_ptr<DisambDict>& dict, std::istream& is) {
    std::string row;
    std::map<std::string, std::size_t> counter;

    std::size_t ccc = 0;
    while (std::getline(is, row)) {
        boost::trim(row);
        if (row.empty() || row == "#")
            continue;
        ccc++;
        std::vector<std::string> parts;
        boost::split(parts, row, boost::is_any_of("\t"));
        utils::UniString word(parts[1]);
        base::UniSPTag sp = base::UniSPTag::X;
        base::UniMorphTag mt = base::UniMorphTag::UNKN;
        std::tie(sp, mt) = getTags<base::UniSPTag, base::UniMorphTag>(parts[2] + "|" + parts[3]);
        std::string rawSp = to_raw_string(sp);
        std::string rawMt = to_raw_string(mt);
        counter[word.toUpperCase().replace(utils::UniCharacter::YO, utils::UniCharacter::YE).getRawString() + DISAMBIG_SEPARATOR + rawSp + DISAMBIG_SEPARATOR + rawMt] += 1;
        if (ccc % 1000 == 0) {
            std::cerr << "Disamb Dict loading: " << ccc;
        }
    }

    dawg::BuildFactory<std::size_t> factory;
    for (auto itr : counter) {
        factory.insertOrLink(itr.first, itr.second);
    }
    DisambDictPtr dct = factory.build();
    dict = utils::make_unique<DisambDict>(dct);
}

namespace {
PhemMarkup parseRawPhem(const utils::UniString& rawPhem) {
    PhemMarkup result;
    std::vector<utils::UniString> parts = rawPhem.split(utils::UniCharacter("|"));
    for (const utils::UniString& part : parts) {
        std::size_t start = 1;
        base::PhemTag partTag = base::PhemTag::UNKN;
        if (part.startsWith(utils::UniCharacter("*"))) {
            partTag = base::PhemTag::PREFIX;
        } else if (part.startsWith(utils::UniCharacter("-"))) {
            partTag = base::PhemTag::SUFFIX;
        } else if (part.startsWith(utils::UniCharacter("+"))) {
            partTag = base::PhemTag::ENDING;
        } else {
            partTag = base::PhemTag::ROOT;
            start = 0;
        }
        for (std::size_t i = start; i < part.length(); ++i) {
            result.append(partTag);
        }
    }
    return result;
}

std::map<utils::UniString, std::size_t> turnSortedSequenceIntoCountedMap(std::set<utils::UniString>&& data) {
    std::map<utils::UniString, std::size_t> result;
    for (auto itr = data.begin(); itr != data.end(); ++itr) {
        const utils::UniString& word = *itr;
        for (std::size_t i = 0; i < word.length(); ++i) {
            utils::UniString cut = word.subString(0, word.length() - i);
            if (result.count(cut)) { // if we already encounded some cut, then we already encounded all subcuts
                break;
            }
            result[cut] = 1;
            utils::UniCharacter prev("");
            for (auto innerItr = std::next(itr); innerItr != data.end(); ++innerItr) {
                if (innerItr->length() <= cut.length())
                    break; // it cannot' starts with cut
                utils::UniCharacter afterCut = (*innerItr)[cut.length()];
                if (innerItr->startsWith(cut)) {
                    if (afterCut != prev) { // inc only if next symbol is different
                        result[cut]++;
                        prev = afterCut;
                    }
                } else {
                    break; //all sequential strings are not starting with cut
                }
            }
        }
    }
    return result;
}
InnerCounterPhemDictPtr mapToFactory(std::map<utils::UniString, std::size_t>&& mp) {
    dawg::BuildFactory<std::size_t> factory;
    for (auto itr : mp) {
        factory.insertOrLink(itr.first.getRawString(), itr.second);
    }
    return factory.build();
}
std::tuple<InnerCounterPhemDictPtr, InnerCounterPhemDictPtr> buildCountPhemDict(std::shared_ptr<RawDict> rd) {
    std::set<utils::UniString> forwardSet, backwardSet;
    for (std::size_t i = 0; i < rd->size(); ++i) {
        WordsArray words;
        std::tie(words, std::ignore) = rd->operator[](i);
        for (const auto& wrd : words) {
            forwardSet.insert(wrd);
            backwardSet.insert(wrd.reverse());
        }
    }
    std::map<utils::UniString, std::size_t> fmap = turnSortedSequenceIntoCountedMap(std::move(forwardSet));
    InnerCounterPhemDictPtr forward = mapToFactory(std::move(fmap));
    std::map<utils::UniString, std::size_t> bmap = turnSortedSequenceIntoCountedMap(std::move(backwardSet));
    InnerCounterPhemDictPtr backward = mapToFactory(std::move(bmap));
    return std::make_tuple(forward, backward);
}
InnerPhemDictPtr buildNormalPhemDict(std::istream& is) {
    std::string row;

    std::size_t ccc = 0;

    dawg::BuildFactory<PhemMarkup> factory;
    while (std::getline(is, row)) {
        boost::trim(row);
        if (row.empty() || row == "#")
            continue;
        ccc++;
        std::vector<std::string> parts;
        boost::split(parts, row, boost::is_any_of("\t"));
        utils::UniString word(parts[0]);
        utils::UniString parse(parts[1]);
        PhemMarkup markUp = parseRawPhem(parse);
        factory.insertOrLink(word.toUpperCase().replace(utils::UniCharacter::YO, utils::UniCharacter::YE).getRawString(), markUp);
        if (ccc % 1000 == 0) {
            std::cerr << "Phem Dict loading: " << ccc;
        }
    }

    return factory.build();
}
}

void buildPhemDict(std::unique_ptr<PhemDict>& dict, std::istream& is, std::shared_ptr<RawDict> rd) {
    InnerPhemDictPtr main = buildNormalPhemDict(is);
    InnerCounterPhemDictPtr forward, backward;
    std::tie(forward, backward) = buildCountPhemDict(rd);
    dict = utils::make_unique<PhemDict>(main, forward, backward);
}
}
