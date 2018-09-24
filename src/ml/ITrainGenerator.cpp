#include "ITrainGenerator.h"
namespace ml {

void ITrainGenerator::generate(std::istream& from, std::vector<TrainPair>& trainSet, std::vector<TrainPair>& testSet, double prob) const {
    std::cerr << "ENTERED\n";
    std::string row;
    std::vector<TrainPair> result;
    std::size_t counter = 1;
    while (from) {
        std::vector<analyze::WordFormPtr> trains;
        std::vector<base::ITag> targets;
        while (getline(from, row)) {
            boost::trim(row);
            if (row.empty() || row == "#") {
                break;
            }
            counter++;
            std::vector<std::string> parts;
            boost::split(parts, row, boost::is_any_of("\t"));
            utils::UniString word(parts[1]);
            base::ITag sp = base::SpeechPartTag::UNKN;
            base::ITag mt = base::MorphTag::UNKN;
            if (tt == TagType::UTAG) {
                std::tie(sp, mt) = build::getTags<base::UniSPTag, base::UniMorphTag>(parts[3] + "|" + parts[4]);
            } else {
                std::tie(sp, mt) = build::getTags<base::SpeechPartTag, base::MorphTag>(parts[2] + "|" + parts[3]);
            }
            std::vector<base::TokenPtr> token = tok.analyze(word);
            std::vector<analyze::WordFormPtr> forms = proc.analyze(token);
            analyze::WordFormPtr fp = forms[0];
            if (tt == TagType::UTAG) {
                conv.convert(fp);
            }
            trains.push_back(fp);
            targets.push_back(getTag(sp, mt));
            if (counter % 1000 == 0) {
                std::cerr << "Readed " << name << " " << counter;
            }
        }
        result.push_back(std::make_tuple(trains, targets));
    }
    std::tie(trainSet, testSet) = split<TrainPair>(result, prob);
}

void ITrainGenerator::generate(std::istream& from, std::vector<TrainPairStr>& trainSet, std::vector<TrainPairStr>& testSet, double prob) const {
    std::string row;
    std::vector<TrainPairStr> result;
    std::size_t counter = 1;
    while (from) {
        std::vector<analyze::WordFormPtr> trains;
        std::vector<utils::UniString> targets;
        while (getline(from, row)) {
            boost::trim(row);
            if (row.empty() || row == "#") {
                break;
            }
            counter++;
            std::vector<std::string> parts;
            boost::split(parts, row, boost::is_any_of("\t"));
            utils::UniString word(parts[1]);
            base::ITag sp = base::SpeechPartTag::UNKN;
            base::ITag mt = base::MorphTag::UNKN;
            if (tt == TagType::UTAG) {
                std::tie(sp, mt) = build::getTags<base::UniSPTag, base::UniMorphTag>(parts[3] + "|" + parts[4]);
            } else {
                std::tie(sp, mt) = build::getTags<base::SpeechPartTag, base::MorphTag>(parts[3] + "|" + parts[4]);
            }
            std::vector<base::TokenPtr> token = tok.analyze(word);
            std::vector<analyze::WordFormPtr> forms = proc.analyze(token);
            analyze::WordFormPtr fp = forms[0];

            if (tt == TagType::UTAG) {
                conv.convert(fp);
            }
            trains.push_back(fp);
            targets.push_back(utils::UniString(parts[2]).toUpperCase());
            if (counter % 1000 == 0) {
                std::cerr << "Readed " << name << " " << counter;
            }
        }
        result.push_back(std::make_tuple(trains, targets));
    }
    std::tie(trainSet, testSet) = split<TrainPairStr>(result, prob);
}
}
