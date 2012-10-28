/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_BOARDWIDGET_H_
#define THECHESS_WIDGETS_BOARDWIDGET_H_

#include <boost/function.hpp>

#include <Wt/WGlobal>
#include <Wt/WCompositeWidget>
#include <Wt/WImage>

#include "chess/Board.hpp"
#include "chess/Square.hpp"
#include "chess/HalfMove.hpp"
#include "chess/Piece.hpp"
#include "widgets/global.hpp"

namespace thechess {

class BoardWidgetImpl;

/** Chess board widget.
This widget is used to show board state or to input one chess move from user.
*/
class BoardWidget : public Wt::WCompositeWidget {
public:
    /** Handler of LinksDialog */
    typedef boost::function<void(LinksDialog*)> LinksHandler;

    /** Constructor.
    \param big    Whether big images are used. (Deprecated, not used)
    \param active Whether user could select a piece and perform a move
        (and emit half_move()).
    \param bottom Board side, showed at the bottom side.
    \param board  The board state.
    \param parent The parent.
    */
    BoardWidget(bool big = false, bool active = false,
                Piece::Color bottom = Piece::WHITE,
                const Board& board = Board(),
                Wt::WContainerWidget* parent = 0);

    /** Return URL to the piece image */
    static std::string image(Piece piece, bool big = false);

    /** Set the color of board bottom side */
    void bottom_set(Piece::Color bottom);

    /** Update board state, last half-move performed and activeness */
    void set(const Board& board, HalfMove lastmove, bool active);

    /** Flip the board (bottom_set(opposite)) */
    void turn();

    /** The signal emitted when the half-move is performed */
    Wt::Signal<HalfMove>& half_move();

    /** Get container widget with a button "Flip" */
    Wt::WContainerWidget* inner();

    /** Show or hide lastmove highlighting */
    void show_lastmove(bool show = true);

    /** Show or hide lastmove checkbox.
    By default, the checkbox is shown.
    */
    void show_lastmove_checkbox(bool show = true);

    /** Show or hide taken pieces.
    By default, taken pieces are shown.
    */
    void show_taken(bool show = true);

    /** Set links handler.
    This handler will be called with LinksDialog already with board link
    and should add another links.
    */
    void set_links_handler(const LinksHandler& links_handler);

    /** Add BoardWidget specific things to LinksDialog */
    void links(LinksDialog* dialog);

private:
    BoardWidgetImpl* impl_;
};

}

#endif

