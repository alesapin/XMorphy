#ifndef _OP_CORPORA_RUS_CORPORA_CONVERTER_H
#define _OP_CORPORA_RUS_CORPORA_CONVERTER_H
#include "ITagConverter.h"
#include "MorphTag.h"
#include "SpeechPartTag.h"
#include "RusCorpSPTag.h"
#include "RusCorpMorphTag.h"
#include <boost/bimap.hpp>
namespace base {
class OpCorporaRusCorporaConverter : public ITagConverter {
	void adjRuleF(SpeechPartTag& spFrom, MorphTag &mtFrom, ITag &spTo, ITag &mtTo) const;
	void verbRuleF(SpeechPartTag& spFrom, MorphTag &mtFrom, ITag &spTo, ITag &mtTo) const;
	void restRuleF(SpeechPartTag& spFrom, MorphTag &mtFrom, ITag &spTo, ITag &mtTo) const;
	void adjRuleB(ITag& spFrom, ITag &mtFrom, SpeechPartTag &spTo, MorphTag &mtTo) const;
	void verbRuleB(ITag& spFrom, ITag &mtFrom, SpeechPartTag &spTo, MorphTag &mtTo) const;
	void restRuleB(ITag &spFrom, ITag &mtFrom, SpeechPartTag &spTo, MorphTag &mtTo) const;

public:
	void convertForward(const SpeechPartTag &spFrom, const MorphTag &mtFrom, ITag &spTo, ITag &mtTo) const;
	void convertBackward(const ITag &spFrom, const ITag &mtFrom, SpeechPartTag &spTo, MorphTag &mtTo) const;
};
}

#endif
