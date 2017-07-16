#ifndef I_TAG_CONVERTER_H
#define I_TAG_CONVERTER_H
#include "ITag.h"
#include "SpeechPartTag.h"
#include "MorphTag.h"
namespace base {
class ITagConverter {
public:
	virtual void convertForward(const SpeechPartTag &spFrom,const MorphTag &mtFrom, ITag &spTo, ITag &mtTo) const = 0;
	virtual void convertBackward(const ITag &spFrom, const ITag &mtFrom, SpeechPartTag &spTo, MorphTag &mtTo) const = 0;
};
}
#endif
