#include "Phemmer.h"
namespace phem {

void Phemmer::phemise(std::vector<analyze::WordFormPtr>& seq) const {
    for (auto ptr : seq) {
        classifier->classify(ptr);
        corrector->correct(ptr);
    }
}
}
