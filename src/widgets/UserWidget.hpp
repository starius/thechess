#ifndef THECHESS_WIDGETS_USEREWIDGET_HPP_
#define THECHESS_WIDGETS_USEREWIDGET_HPP_

#include <Wt/WCompositeWidget>
#include <Wt/Dbo/ptr>
namespace dbo = Wt::Dbo;
#include "model/User.hpp"

namespace Wt {
    class WContainerWidget;
}

namespace thechess {
namespace widgets {

using model::UserPtr;

class UserWidgetImpl;

class UserWidget : public Wt::WCompositeWidget
{
public:
    UserWidget(UserPtr user, Wt::WContainerWidget* parent=0);
private:
    UserWidgetImpl* impl_;
};

}
}

#endif
