/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <iostream>
#include <string>
#include <boost/format.hpp>

#include <Wt/WAbstractTableModel>
#include <Wt/WApplication>
#include <Wt/WHBoxLayout>
#include <Wt/WEnvironment>
#include <Wt/WCssDecorationStyle>
#include <Wt/WImage>
#include <Wt/WLength>
#include <Wt/WModelIndex>
#include <Wt/WPushButton>
#include <Wt/WString>
#include <Wt/WTableView>

#include "widgets/chess/MovesWidget.hpp"
#include "chess/Piece.hpp"
#include "chess/HalfMove.hpp"

namespace thechess {

const int MOVES_COLUMN_WIDTH = 100;
const int N_COLUMN_WIDTH = 20;
const int MOVES_ROW_HEIGHT = 35;
const int NAVIGATION_BAR_HEIGHT = 25; // set in wt.css

class MovesModel : public Wt::WAbstractTableModel {
public:
    MovesModel(CachedMoves* cached_moves, WObject* parent = 0) :
        Wt::WAbstractTableModel(parent), cached_moves_(cached_moves),
        max_show_move_(0) {
    }

    int index2n(const Wt::WModelIndex& index) const {
        // -2 means wrong half_move number
        int n;
        if (index.column() == 0) {
            n = -2;
        } else {
            Piece::Color color = (index.column() == 1) ? Piece::WHITE : Piece::BLACK;
            n = Moves::half_move_index(index.row() + 1, color);
            if (n >= cached_moves_->size()) {
                n = -2;
            }
        }
        return n;
    }

    Wt::WModelIndex n2index(const int n) const {
        return index(Moves::move_number(n) - 1, Moves::order_int(n) + 1);
    }

    virtual int rowCount(const Wt::WModelIndex& /*parent*/) const {
        return cached_moves_->moves_number();
    }

    virtual int columnCount(const Wt::WModelIndex& /*parent*/) const {
        return 3;
    }

    virtual boost::any data(const Wt::WModelIndex& index,
                            int role) const {
        if (role == Wt::DisplayRole) {
            if (index.column() == 0) {
                // id column
                return str(boost::format("%d.") % (index.row() + 1));
            }
            int n = index2n(index);
            if (n == -2) {
                return boost::any();
            }
            HalfMove half_move = cached_moves_->half_move(n);
            const Board& board = cached_moves_->board(n);
            const Board& board_after = cached_moves_->board(n + 1);
            std::string text = half_move.san(board, board_after, /*skip_pieces*/ true);
            char shah = ' ';
            if (board_after.test_shah()) {
                shah = board_after.test_end() == Board::CHECKMATE ? '#' : '+';
            }
            if (half_move.turn_into() != Piece::LETTER_NULL) {
                Piece::Color color = board.color(half_move.from());
                Piece ti_piece(color, half_move.turn_into());
                std::string ti_src =
                    wApp->resolveRelativeUrl(BoardWidget::image(ti_piece));
                return str(boost::format(
                               "<table><tr>"
                               "<td style='vertical-align:middle;'>%s =</td>"
                               "<td><img src='%s' /></td>"
                               "<td style='vertical-align:middle;'>%c</td>"
                               "</tr></table>") %
                           text % ti_src % shah);
            } else {
                Piece piece = board.piece(half_move.from());
                std::string img =
                    wApp->resolveRelativeUrl(BoardWidget::image(piece));
                return str(boost::format(
                               "<table><tr>"
                               "<td><img src='%s' /></td>"
                               "<td style='vertical-align:middle;'>%s%c</td>"
                               "</tr></table>") % img % text % shah);
            }
        }
        return boost::any();
    }

    virtual Wt::WFlags<Wt::ItemFlag> flags(const Wt::WModelIndex& index) const {
        Wt::WFlags<Wt::ItemFlag> flags_(Wt::ItemIsXHTMLText);
        if (index.column() != 0) {
            flags_ |= Wt::ItemIsSelectable;
        }
        return flags_;
    }

    void move_changed(int move_i) {
        int first = Moves::move_number(move_i) - 1;
        int last = Moves::move_number(max_show_move_) - 1;
        if (move_i <= max_show_move_) {
            beginRemoveRows(Wt::WModelIndex(), first, last);
            endRemoveRows();
        }
        if (first > last) {
            beginInsertRows(Wt::WModelIndex(), first, first);
            endInsertRows();
        }
        dataChanged().emit(n2index(move_i), n2index(move_i));
        max_show_move_ = move_i;
    }

private:
    CachedMoves* cached_moves_;
    int max_show_move_;
};

class MovesWidget::MovesWidgetImpl : public Wt::WContainerWidget {
public:
    MovesWidgetImpl(const Moves& moves,
                    bool big, bool active, int max_moves,
                    bool append_only, Piece::Color bottom) :
        Wt::WContainerWidget(), cached_moves_(moves),
        current_move_(moves.size() - 1), max_moves_(max_moves),
        used_moves_(0), append_only_(append_only),
        active_(active), activated_(false) {
        Wt::WHBoxLayout* layout = new Wt::WHBoxLayout();
        setLayout(layout, Wt::AlignTop);
        layout->addWidget(board_widget_ =
                              new BoardWidget(big, this->active(), bottom));
        check_activate();
        Wt::WPushButton* goto_first =
            new Wt::WPushButton("<<", board_widget_->inner());
        goto_first->clicked().connect(this, &MovesWidgetImpl::goto_first);
        Wt::WPushButton* goto_prev =
            new Wt::WPushButton("<", board_widget_->inner());
        goto_prev->clicked().connect(this, &MovesWidgetImpl::goto_prev);
        Wt::WPushButton* goto_next =
            new Wt::WPushButton(">", board_widget_->inner());
        goto_next->clicked().connect(this, &MovesWidgetImpl::goto_next);
        Wt::WPushButton* goto_last =
            new Wt::WPushButton(">>", board_widget_->inner());
        goto_last->clicked().connect(this, &MovesWidgetImpl::goto_last);
        moves_model_ = new MovesModel(&cached_moves_, this);
        layout->addWidget(moves_table_view_ = new Wt::WTableView());
        moves_table_view_->setModel(moves_model_);
        int moves_per_page = big ? 20 : 10;
        int width = wApp->environment().ajax() ?
                    ((MOVES_COLUMN_WIDTH + 20) * 2 + (N_COLUMN_WIDTH + 7) + 20) : 400;
        int height = moves_per_page * MOVES_ROW_HEIGHT;
        height += wApp->environment().ajax() ? 0 : NAVIGATION_BAR_HEIGHT;
        moves_table_view_->resize(width, height);
        moves_table_view_->setRowHeight(MOVES_ROW_HEIGHT);
        moves_table_view_->setHeaderHeight(0);
        for (int i = 1; i < 3; i++) {
            moves_table_view_->setColumnWidth(i, MOVES_COLUMN_WIDTH);
        }
        moves_table_view_->setColumnWidth(0, N_COLUMN_WIDTH);
        moves_table_view_->setSelectionBehavior(Wt::SelectItems);
        moves_table_view_->setSelectionMode(Wt::SingleSelection);
        moves_table_view_->clicked().connect(this, &MovesWidgetImpl::onselect_);
        goto_move_(current_move_); // last half_move
    }

    const Moves& moves() const {
        return (Moves&)cached_moves_;
    }

    const Board& board() const {
        return cached_moves_.board(cached_moves_.size());
    }

    Wt::Signal<HalfMove>& half_move() {
        return move_signal_;
    }

    int current_move() const {
        return current_move_;
    }

    void add_move(const HalfMove& half_move) {
        int i = cached_moves_.size() - 1;
        if (i == -1 || half_move != cached_moves_.half_move(i)) {
            int n = cached_moves_.size();
            reset_move_(n, half_move);
            goto_move_(n);
        }
    }

    void bottom_set(Piece::Color bottom) {
        board_widget_->bottom_set(bottom);
    }

    void reset() {
        cached_moves_ = CachedMoves();
        goto_move_(-1);
        moves_model_->move_changed(0);
    }

    void set_moves(const Moves& moves) {
        cached_moves_ = CachedMoves(moves);
        current_move_ = cached_moves_.size() - 1;
        moves_model_->move_changed(0);
        goto_move_(current_move_);
    }

    void set_active(bool active) {
        active_ = active;
        check_activate();
        move_select();
    }

private:
    BoardWidget* board_widget_;
    MovesModel* moves_model_;
    Wt::WTableView* moves_table_view_;
    Wt::Signal<HalfMove> move_signal_;

    CachedMoves cached_moves_;
    int current_move_;
    int max_moves_;
    int used_moves_;
    bool append_only_;
    bool active_;
    bool activated_;

    void check_activate() {
        if (!activated_ && this->active()) {
            activated_ = true;
            board_widget_->half_move().connect(this, &MovesWidgetImpl::onmove_);
        }
    }

    void reset_move_(int n, const HalfMove& half_move) {
        cached_moves_.reset_half_move(n, half_move);
        moves_model_->move_changed(n);
    }

    void onmove_(const HalfMove& half_move) {
        if (active()) {
            current_move_ += 1;
            used_moves_ += 1;
            reset_move_(current_move_, half_move);
            goto_move_(current_move_);
            move_signal_.emit(half_move);
        }
    }

    void onselect_(const Wt::WModelIndex& index) {
        int n = moves_model_->index2n(index);
        if (n == -2 && index.column() == 2) {
            // after last half_move
            n = cached_moves_.size() - 1;
        }
        if (n != -2) {
            current_move_ = n;
            move_select();
        }
    }

    // -1 means start position
    void goto_move_(int n = -1) {
        current_move_ = n;
        move_select();
        history_select();
    }

    void goto_first() {
        goto_move_(-1);
    }

    void goto_last() {
        goto_move_(cached_moves_.size() - 1);
    }

    void goto_prev() {
        if (current_move_ > -1) {
            goto_move_(current_move_ - 1);
        }
    }

    void goto_next() {
        if (current_move_ < cached_moves_.size() - 1) {
            goto_move_(current_move_ + 1);
        }
    }

    bool active() const {
        std::cout << "active " << (active_ &&
                                   (max_moves_ == -1 || used_moves_ < max_moves_) &&
                                   (!append_only_ || current_move_ == cached_moves_.size() - 1)) << std::endl;
        return active_ &&
               (max_moves_ == -1 || used_moves_ < max_moves_) &&
               (!append_only_ || current_move_ == cached_moves_.size() - 1);
    }

    void move_select() {
        const Board& board = cached_moves_.board(current_move_ + 1);
        HalfMove lastmove;
        if (current_move_ != -1) {
            lastmove = cached_moves_.half_move(current_move_);
        }
        board_widget_->set(board, lastmove, active());
    }

    void history_select() {
        //FIXME - wron row is shown
        Wt::WModelIndex index = moves_model_->n2index(current_move_);
        moves_table_view_->scrollTo(index); // ajax
        if (current_move_ > -1) {
            moves_table_view_
            ->select(moves_model_->n2index(current_move_));
        } else {
            moves_table_view_->setSelectedIndexes(Wt::WModelIndexSet());
        }
    }

};

MovesWidget::MovesWidget(const Moves& moves,
                         bool big, bool active, int max_moves,
                         bool append_only, Piece::Color bottom,
                         Wt::WContainerWidget* parent) :
    WCompositeWidget(parent) {
    impl_ = new MovesWidgetImpl(moves, big, active,
                                max_moves, append_only, bottom);
    setImplementation(impl_);
}

const Moves& MovesWidget::moves() const {
    return impl_->moves();
}

const Board& MovesWidget::board() const {
    return impl_->board();
}

Wt::Signal<HalfMove>& MovesWidget::half_move() {
    return impl_->half_move();
}

void MovesWidget::add_move(const HalfMove& half_move) {
    impl_->add_move(half_move);
}

void MovesWidget::bottom_set(Piece::Color bottom) {
    impl_->bottom_set(bottom);
}

void MovesWidget::reset() {
    impl_->reset();
}

void MovesWidget::set_moves(const Moves& moves) {
    impl_->set_moves(moves);
}

void MovesWidget::set_active(bool active) {
    impl_->set_active(active);
}

int MovesWidget::current_move() const {
    return impl_->current_move();
}

}
