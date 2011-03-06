#ifndef THECHESS_WIDGETS_RATING_CHANGES_HPP_
#define THECHESS_WIDGETS_RATING_CHANGES_HPP_

#include <Wt/WCompositeWidget>

#include "model/User.hpp"

namespace Wt {
  class WContainerWidget;
}

namespace thechess {
namespace widgets {

using model::UserPtr;

class RatingChangesImpl;

class RatingChanges : public Wt::WCompositeWidget
{
public:
    RatingChanges(UserPtr user, Wt::WContainerWidget* parent=0);
    void add_user(UserPtr user);

private:
    RatingChangesImpl* impl_;
};

}
}

#endif

