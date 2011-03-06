// vim: set noet ts=2 sw=2:
/* Board widget
 * Signal: move()
 */

#ifndef THECHESS_WIDGETS_BOARDWIDGET_H_
#define THECHESS_WIDGETS_BOARDWIDGET_H_

#include <Wt/WCompositeWidget>
#include <Wt/WImage>

#include "chess/board.hpp"
#include "chess/xy.hpp"
#include "chess/move.hpp"
#include "chess/field.hpp"

namespace Wt {
  class WContainerWidget;
}

namespace thechess {
namespace widgets {


class BoardWidgetImpl;


class BoardWidget : public Wt::WCompositeWidget
{
public:
    BoardWidget(bool big=false, bool active=false, chess::Color bottom=chess::white,
        const chess::Board& board=chess::Board(),
        Wt::WContainerWidget *parent=0);

    static std::string image(chess::Field field, bool big=false);

    void bottom_set(chess::Color bottom);
    void set(const chess::Board& board, chess::Move lastmove, bool active);

    void turn();
    Wt::Signal<chess::Move>& move();
    Wt::WContainerWidget* inner();

private:
    BoardWidgetImpl* impl_;
};








}
}

#endif // THECHESS_WIDGETS_BOARDWIDGET_H_
