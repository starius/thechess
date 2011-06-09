#ifndef THECHESS_MODEL_OBJECT_HPP_
#define THECHESS_MODEL_OBJECT_HPP_

#include <Wt/Dbo/Session>
#include <Wt/WDateTime>

#include "chess/move.hpp"

namespace dbo = Wt::Dbo;

namespace thechess {
namespace model {

enum ObjectType {
    NoEvent,
    GameObject,
    UserObject,
    CompetitionObject
}; // change Object::reread() after changing this

enum GameEvent {
    ge_any = 0, // ie, simple move
    ge_comment, // change of comment
    ge_dialog, // propose, agree, discard of draw, mistake, or pause
    ge_mistake, // rollback of mistake
    ge_state // confirm, start, end, cancel
};

class ThechessEvent;

struct Object
{
    Object(ObjectType ot, int i);

    ObjectType object_type;
    int id;

    bool operator<(const Object& b) const
    {
        return id<b.id || (id==b.id && object_type<b.object_type);
    }

    bool operator==(const Object& b) const
    {
        return id == b.id && object_type == b.object_type;
    }

    void reread(dbo::Session& session) const;
    Wt::WDateTime process(ThechessEvent& event, dbo::Session& session) const;
};

class ThechessEvent : public Object
{
public:
    chess::Move move;

    ThechessEvent(const Object& object);
    ThechessEvent(ObjectType ot, int i);
    ThechessEvent(ObjectType ot, int i,
        GameEvent ge, chess::Move m=chess::move_null);

    GameEvent game_event() const;
    int raw_event() const;
    void set_event(GameEvent event);
    operator bool() const;

private:
    int event_;
};

}
}

#endif

