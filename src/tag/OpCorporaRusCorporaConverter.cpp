#include "OpCorporaRusCorporaConverter.h"
namespace base {
namespace {
static const boost::bimap<MorphTag, RusCorpMorphTag> SIMPLE_MAPING =
boost::assign::list_of<boost::bimap<MorphTag, RusCorpMorphTag>::relation>
(MorphTag::anim, RusCorpMorphTag::anim    )
(MorphTag::inan, RusCorpMorphTag::inan    )
(MorphTag::Ms_f, RusCorpMorphTag::m_f     )
(MorphTag::masc, RusCorpMorphTag::m       )
(MorphTag::femn, RusCorpMorphTag::f       )
(MorphTag::neut, RusCorpMorphTag::n       )
(MorphTag::sing, RusCorpMorphTag::sg      )
(MorphTag::plur, RusCorpMorphTag::pl      )
(MorphTag::Fixd, RusCorpMorphTag::_0      )
(MorphTag::nomn, RusCorpMorphTag::nom     )
(MorphTag::gent, RusCorpMorphTag::gen     )
(MorphTag::datv, RusCorpMorphTag::dat     )
(MorphTag::accs, RusCorpMorphTag::acc     )
(MorphTag::ablt, RusCorpMorphTag::ins     )
(MorphTag::loct, RusCorpMorphTag::loc     )
(MorphTag::voct, RusCorpMorphTag::voc     )
(MorphTag::gen1, RusCorpMorphTag::gen     )
(MorphTag::gen2, RusCorpMorphTag::gen2    )
(MorphTag::acc2, RusCorpMorphTag::acc2    )
(MorphTag::loc1, RusCorpMorphTag::loc     )
(MorphTag::loc2, RusCorpMorphTag::loc2    )
(MorphTag::Abbr, RusCorpMorphTag::abbr    )
(MorphTag::Name, RusCorpMorphTag::persn   )
(MorphTag::Surn, RusCorpMorphTag::famn    )
(MorphTag::Patr, RusCorpMorphTag::patrn   )
(MorphTag::Dist, RusCorpMorphTag::distort )
(MorphTag::Arch, RusCorpMorphTag::anom    ) //FIXME: Nasty bug (duplicate)
(MorphTag::Litr, RusCorpMorphTag::anom    )
(MorphTag::V_be, RusCorpMorphTag::distort )
(MorphTag::Supr, RusCorpMorphTag::supr    )
(MorphTag::Coun, RusCorpMorphTag::adnum   )
(MorphTag::perf, RusCorpMorphTag::pf      )
(MorphTag::impf, RusCorpMorphTag::ipf     )
(MorphTag::tran, RusCorpMorphTag::tran    )
(MorphTag::intr, RusCorpMorphTag::intr    )
(MorphTag::actv, RusCorpMorphTag::act     )
(MorphTag::pssv, RusCorpMorphTag::pass    )
(MorphTag::per1, RusCorpMorphTag::_1p     )
(MorphTag::per2, RusCorpMorphTag::_2p     )
(MorphTag::per3, RusCorpMorphTag::_3p     )
(MorphTag::pres, RusCorpMorphTag::praes   )
(MorphTag::past, RusCorpMorphTag::praet   )
(MorphTag::futr, RusCorpMorphTag::fut     )
(MorphTag::indc, RusCorpMorphTag::indic   )
(MorphTag::impr, RusCorpMorphTag::imper   );
}

void OpCorporaRusCorporaConverter::adjRuleF(SpeechPartTag& spFrom, MorphTag &mtFrom, ITag &spTo, ITag &mtTo) const {
	if (spFrom & SpeechPartTag::ADJF) {
		if (mtFrom.resetIfContains(MorphTag::Apro | MorphTag::Subx | MorphTag::Anph)) {
			spTo = RusCorpSPTag::APRO;
		} else if (mtFrom.resetIfContains(MorphTag::Apro | MorphTag::Subx)) {
			spTo = RusCorpSPTag::SPRO;
		} else if (mtFrom.resetIfContains(MorphTag::Apro)) {
			spTo = RusCorpSPTag::APRO;
		} else if (mtFrom.resetIfContains(MorphTag::Anum)) {
			spTo = RusCorpSPTag::ANUM;
		} else {
			spTo = RusCorpSPTag::A;
			mtTo |= RusCorpMorphTag::plen;
		}
	} else if (spFrom & SpeechPartTag::ADJS) {
		spTo = RusCorpSPTag::A;
		mtTo |= RusCorpMorphTag::brev;
	} else if (spFrom & SpeechPartTag::COMP) {
		spTo = RusCorpSPTag::A;
		if (mtFrom.resetIfContains(MorphTag::Cmp2)) {
			mtTo |= RusCorpMorphTag::comp2;
		} else if (mtFrom.resetIfContains(MorphTag::Supr)) {
			mtTo |= RusCorpMorphTag::supr;
		}
	}
}


void OpCorporaRusCorporaConverter::adjRuleB(ITag& spFrom, ITag &mtFrom, SpeechPartTag &spTo, MorphTag &mtTo) const {
	if (spFrom & RusCorpSPTag::APRO) {
		spTo = SpeechPartTag::ADJF;
		mtTo |= MorphTag::Apro;
	} else if (spFrom & RusCorpSPTag::SPRO) {
		spTo = SpeechPartTag::ADJF;
		mtTo |= MorphTag::Apro | MorphTag::Subx;
	} else if (spFrom & RusCorpSPTag::ANUM) {
		spTo = SpeechPartTag::ADJF;
		mtTo |= MorphTag::Anum;
	} else if (spFrom & RusCorpSPTag::A) {
		if (mtFrom & RusCorpMorphTag::plen) {
			spTo = SpeechPartTag::ADJF;
		} else if (mtFrom.resetIfContains(RusCorpMorphTag::brev)) {
			spTo = SpeechPartTag::ADJS;
		} else if (mtFrom.resetIfContains(RusCorpMorphTag::comp2)) {
			spTo = SpeechPartTag::COMP;
			mtTo |= MorphTag::Cmp2;
		} else if (mtFrom.resetIfContains(RusCorpMorphTag::supr)) {
			spTo = SpeechPartTag::COMP;
			mtTo |= MorphTag::Supr;
		} else if (mtFrom.resetIfContains(RusCorpMorphTag::comp)) {
			spTo = SpeechPartTag::COMP;
		} else {
			spTo |= SpeechPartTag::ADJF;
		}
	}
}

void OpCorporaRusCorporaConverter::verbRuleF(SpeechPartTag& spFrom, MorphTag &mtFrom, ITag &spTo, ITag &mtTo) const {
	if (spFrom & SpeechPartTag::INFN) {
		spTo = RusCorpSPTag::V;
		mtTo |= RusCorpMorphTag::inf;
	} else if (spFrom & SpeechPartTag::PRTF) {
		spTo = RusCorpSPTag::V;
		mtTo |= RusCorpMorphTag::partcp;
		if (mtFrom.resetIfContains(MorphTag::inan)) {
			 mtTo |= RusCorpMorphTag::plen;
		} else if (mtFrom.resetIfContains(MorphTag::anim)) {
			mtTo |= RusCorpMorphTag::plen;
		} else {
			mtTo |= RusCorpMorphTag::plen;
		}
	} else if (spFrom & SpeechPartTag::PRTS) {
		spTo = RusCorpSPTag::V;
		mtTo |= RusCorpMorphTag::partcp;
		if (mtFrom.resetIfContains(MorphTag::intr)) {
			mtTo |= RusCorpMorphTag::plen | RusCorpMorphTag::intr;
		} else {
			mtTo |= RusCorpMorphTag::tran | RusCorpMorphTag::brev;
		}
	} else if (spFrom & SpeechPartTag::GRND) {
		spTo = RusCorpSPTag::V;
		mtTo |= RusCorpMorphTag::ger;
	} else if (spFrom & SpeechPartTag::VERB) {
		spTo = RusCorpSPTag::V;
		if (mtFrom.resetIfContains(MorphTag::impr | MorphTag::incl | MorphTag::sing)) {
			mtTo |= RusCorpMorphTag::_1p | RusCorpMorphTag::imper | RusCorpMorphTag::pl;
		} else if (mtFrom.resetIfContains(MorphTag::impr | MorphTag::incl | MorphTag::plur)) {
			mtTo |= RusCorpMorphTag::_1p | RusCorpMorphTag::imper2 | RusCorpMorphTag::pl;
		} else if (mtFrom.resetIfContains(MorphTag::impr | MorphTag::excl)) {
			mtTo |=  RusCorpMorphTag::_2p | RusCorpMorphTag::imper;
		} else if (mtFrom.resetIfContains(MorphTag::impr | MorphTag::incl)) {
			mtTo |=  RusCorpMorphTag::_1p | RusCorpMorphTag::imper;
		}
	}
}


void OpCorporaRusCorporaConverter::verbRuleB(ITag& spFrom, ITag &mtFrom, SpeechPartTag &spTo, MorphTag &mtTo) const {
	if(spFrom & RusCorpSPTag::V) {
		if (mtFrom.resetIfContains(RusCorpMorphTag::inf)) {
			spTo = SpeechPartTag::INFN;
		} else if (mtFrom.resetIfContains( RusCorpMorphTag::partcp | RusCorpMorphTag::plen)) {
			spTo = SpeechPartTag::PRTF;
		} else if (mtFrom.resetIfContains(RusCorpMorphTag::intr | RusCorpMorphTag::partcp | RusCorpMorphTag::plen)) {
			spTo = SpeechPartTag::PRTS;
			mtTo |= MorphTag::intr;
		} else if (mtFrom.resetIfContains( RusCorpMorphTag::intr | RusCorpMorphTag::partcp | RusCorpMorphTag::brev)) {
			spTo = SpeechPartTag::PRTS;
		} else if (mtFrom.resetIfContains(RusCorpMorphTag::ger)) {
			spTo = SpeechPartTag::GRND;
		} else if (mtFrom.resetIfContains(RusCorpMorphTag::_1p | RusCorpMorphTag::imper | RusCorpMorphTag::pl)) {
			spTo = SpeechPartTag::VERB;
			mtTo |= MorphTag::impr | MorphTag::incl | MorphTag::sing;
		} else if (mtFrom.resetIfContains(RusCorpMorphTag::_1p | RusCorpMorphTag::imper2 | RusCorpMorphTag::pl)) {
			spTo = SpeechPartTag::VERB;
			mtTo |= MorphTag::impr | MorphTag::incl | MorphTag::plur;
		} else if (mtFrom.resetIfContains(RusCorpMorphTag::_2p | RusCorpMorphTag::imper)) {
			spTo = SpeechPartTag::VERB;
			mtTo |= MorphTag::impr | MorphTag::excl;
		} else if (mtFrom.resetIfContains(RusCorpMorphTag::_1p | RusCorpMorphTag::imper)) {
			spTo = SpeechPartTag::VERB;
			mtTo |= MorphTag::impr | MorphTag::incl;
		} else {
			spTo = SpeechPartTag::VERB;
		}
	}
}



void OpCorporaRusCorporaConverter::restRuleF(SpeechPartTag& spFrom, MorphTag &mtFrom, ITag &spTo, ITag &mtTo) const {
	if (spFrom & SpeechPartTag::NOUN) {
		spTo = RusCorpSPTag::S;
	} else if (spFrom & SpeechPartTag::NUMR) {
		mtFrom.resetIfContains(MorphTag::inan);
		spTo = RusCorpSPTag::NUM;
	} else if (spFrom & SpeechPartTag::PRED) {
		mtFrom.resetIfContains(MorphTag::pres);
		spTo = RusCorpSPTag::PRAEDIC;
	} else if (spFrom & SpeechPartTag::ADVB) {
		mtFrom.resetIfContains(MorphTag::Ques);
		spTo = RusCorpSPTag::ADVPRO;
	} else if (spFrom & SpeechPartTag::NPRO) {
		spTo = RusCorpSPTag::SPRO;
	} else if (spFrom & SpeechPartTag::PREP) {
		spTo = RusCorpSPTag::PR;
	} else if (spFrom & SpeechPartTag::PRCL) {
		spTo = RusCorpSPTag::PART;
	} else if (spFrom & SpeechPartTag::INTJ) {
		spTo = RusCorpSPTag::INTJ;
	} else if (spFrom & SpeechPartTag::CONJ) {
		if (mtFrom.resetIfContains(MorphTag::Prnt)) {
			spTo = RusCorpSPTag::PARENTH;
		} else {
			spTo = RusCorpSPTag::CONJ;
		}
	}
}

void OpCorporaRusCorporaConverter::restRuleB(ITag &spFrom, ITag &mtFrom, SpeechPartTag &spTo, MorphTag &mtTo) const {
	if (spFrom & RusCorpSPTag::S) {
		spTo = SpeechPartTag::NOUN;
	} else if (spFrom & RusCorpSPTag::NUM) {
		spTo = SpeechPartTag::NUMR;
	} else if (spFrom & RusCorpSPTag::PRAEDIC) {
		spTo = SpeechPartTag::PRED;
	} else if (spFrom & RusCorpSPTag::ADVPRO) {
		spTo = SpeechPartTag::ADVB;
		mtTo |= MorphTag::Ques;
	} else if (spFrom & RusCorpSPTag::ADV) {
		spTo = SpeechPartTag::ADVB;
	} else if (spFrom & RusCorpSPTag::SPRO) {
		spTo = SpeechPartTag::NPRO;
	} else if (spFrom & RusCorpSPTag::PR) {
		spTo = SpeechPartTag::PREP;
	} else if (spFrom & RusCorpSPTag::PART) {
		spTo = SpeechPartTag::PRCL;
	} else if (spFrom & RusCorpSPTag::INTJ) {
		spTo = SpeechPartTag::INTJ;
	} else if (spFrom & RusCorpSPTag::PARENTH) {
		spTo = SpeechPartTag::CONJ;
		mtTo |= MorphTag::Prnt;
	} else if (spFrom & RusCorpSPTag::CONJ) {
		spTo = SpeechPartTag::CONJ;
	}
}


void OpCorporaRusCorporaConverter::convertForward(const SpeechPartTag &spFrom, const MorphTag &mtFrom, ITag &spTo, ITag &mtTo) const {
	spTo = RusCorpSPTag::UNKN;
	mtTo = RusCorpMorphTag::UNKN;
	SpeechPartTag spf(spFrom);
	MorphTag mtf(mtFrom);
	adjRuleF(spf, mtf, spTo, mtTo);
	verbRuleF(spf, mtf, spTo, mtTo);
	restRuleF(spf, mtf, spTo, mtTo);
	for (auto itr = MorphTag::begin(); itr != MorphTag::end(); ++itr) {
		if (*itr & mtf) {
			if (SIMPLE_MAPING.left.count(*itr)) {
				mtTo |= SIMPLE_MAPING.left.at(*itr);
			}
		}
	}
}

void OpCorporaRusCorporaConverter::convertBackward(const ITag &spFrom, const ITag &mtFrom, SpeechPartTag &spTo, MorphTag &mtTo) const {
	spTo = SpeechPartTag::UNKN;
	mtTo = MorphTag::UNKN;
	ITag spf(spFrom);
	ITag mtf(mtFrom);
	adjRuleB(spf, mtf, spTo, mtTo);
	verbRuleB(spf, mtf, spTo, mtTo);
	restRuleB(spf, mtf, spTo, mtTo);
	for (auto itr = RusCorpMorphTag::begin(); itr != RusCorpMorphTag::end(); ++itr) {
 		if (*itr & mtf) {
			if (SIMPLE_MAPING.right.count(*itr)) {
				mtTo |= SIMPLE_MAPING.right.at(*itr);
			}
		}
	}
}
}
