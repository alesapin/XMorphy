#ifndef I_ENTITY_H
#define I_ENTITY_H
namespace base {
class IEntity {
    public:
        virtual ~IEntity() = 0;
};
inline IEntity::~IEntity() {}
}
#endif
