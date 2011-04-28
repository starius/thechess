#ifndef THECHESS_WAPPLICATION_HPP_
#define THECHESS_WAPPLICATION_HPP_

#include <vector>

namespace Wt {
    class WEnvironment;
    class WApplication;
    class WContainerWidget;
    class WImage;
    class WBorderLayout;
    class WWidget;
}
#include <Wt/WApplication>
#include <Wt/WSignal>
#include <Wt/Dbo/ptr>
namespace dbo = Wt::Dbo;

#include "model/User.hpp"
#include "model/Game.hpp"
#include "ThechessSession.hpp"
#include "widgets/MainMenu.hpp"

#define tApp ((ThechessApplication*)wApp)

namespace thechess {

using model::UserPtr;
using model::GamePtr;

class ThechessSession;

class ThechessApplication : public Wt::WApplication
{
public:
    ThechessApplication(const Wt::WEnvironment& env);
    ~ThechessApplication();

    static ThechessSession& session() { return *session_; }
    static void set_session_(ThechessSession& s) { session_ = &s; }

    UserPtr user() const { return user_; }
    void after_user_change_();
    void set_user(UserPtr user);
    void logout();

    void view(UserPtr user);
    void view(GamePtr game);

    template<typename M> void list_view()
    {
    }

private:
    static ThechessSession* session_;
    UserPtr user_;

    void cookie_session_read_();
    void cookie_session_write_();

    void onPathChange_();
    Wt::WBorderLayout* layout_();
    Wt::WContainerWidget* mainpanel_();
    void set_mainpanel_(Wt::WWidget* widget);

    template<typename W>
    void show_(std::string path)
    {
        setInternalPath(path);
        set_mainpanel_(new W());
    }

    template<typename W, typename I>
    void show_(I item, std::string path)
    {
        setInternalPath(path);
        set_mainpanel_(new W(item));
    }

    template<typename M>
    void object_view_(const char* path)
    {
        std::string id_str = internalPathNextPart(path);
        if (id_str.empty())
        {
            list_view<M>();
        }
        else
        {
            int id = atoi(id_str.c_str());
            if (id)
            {
                dbo::Transaction t(tApp->session());
                dbo::ptr<M> o = tApp->session().find<M>()
                .where("id = ?").bind(id);
                if (o)
                {
                    view(o);
                }
                t.commit();
            }
        }
    }

friend class widgets::MainMenuImpl;
};

template<> void ThechessApplication::list_view<model::Game>();

}

#endif
