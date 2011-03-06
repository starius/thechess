#ifndef THECHESS_WIDGETS_GAMESREATEWIDGET_HPP_
#define THECHESS_WIDGETS_GAMESREATEWIDGET_HPP_

#include <Wt/WContainerWidget>
#include <Wt/Dbo/ptr>
namespace dbo = Wt::Dbo;

#include "model/User.hpp"
#include "chess/field.hpp"

namespace Wt {
    class WComboBox;
};

namespace thechess {
namespace widgets {

using model::UserPtr;

class GameParametersWidget;

class GameCreateWidget : public Wt::WContainerWidget
{
public:
    GameCreateWidget(UserPtr user, Wt::WContainerWidget* p=0);
    GameCreateWidget(Wt::WContainerWidget* p=0);

private:
    GameParametersWidget* gpw_;
    bool with_user_;
    UserPtr user_;
    Wt::WComboBox* color_;

    void print_();
    void button_handler_();
    chess::Color selected_color_() const;
};

}
}

#endif
