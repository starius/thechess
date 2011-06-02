#ifndef THECHESS_MODEL_OBJECT_HPP_
#define THECHESS_MODEL_OBJECT_HPP_

namespace thechess {
namespace model {

enum ObjectType {
    GameObject,
    UserObject,
    CompetitionObject
};

struct Object
{
    ObjectType object_type;
    int id;
};

inline bool operator<(const model::Object& a, const model::Object& b)
{
    return a.id<b.id || (a.id==b.id && a.object_type<b.object_type);
}

}
}

#endif

