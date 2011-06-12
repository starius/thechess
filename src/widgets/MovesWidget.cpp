
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

#include "widgets/MovesWidget.hpp"
#include "chess/field.hpp"
#include "chess/move.hpp"

namespace thechess {
namespace widgets {

const int moves_column_width = 100;
const int n_column_width = 20;
const int moves_row_height = 35;
const int navigationBarHeight = 25; // set in wt.css

class MovesModel : public Wt::WAbstractTableModel
{
public:
    MovesModel(chess::CachedMoves* cached_moves, WObject *parent=0) :
    Wt::WAbstractTableModel(parent), cached_moves_(cached_moves),
    max_show_move_(0)
    {
    }

    int index2n(const Wt::WModelIndex& index) const
    {
        // -2 means wrong move number
        using namespace chess;
        int n;
        if (index.column() == 0)
        {
            n = -2;
        }
        else
        {
            Color color = (index.column() == 1) ? white : black;
            n = Moves::n_from_human(index.row() + 1, color);
            if (n >= cached_moves_->size())
            {
                n = -2;
            }
        }
        return n;
    }

    Wt::WModelIndex n2index(const int n) const
    {
        using chess::Moves;
        return index(Moves::n_to_human(n) - 1, Moves::order_int(n) +1);
    }

    virtual int rowCount(const Wt::WModelIndex& /*parent*/) const
    {
        return cached_moves_->human_size();
    }

    virtual int columnCount(const Wt::WModelIndex& /*parent*/) const
    {
        return 3;
    }

    virtual boost::any data(const Wt::WModelIndex& index,
        int role) const
    {
        if (role == Wt::DisplayRole)
        {
            if (index.column() == 0)
            {
                // id column
                return str(boost::format("%d.") % (index.row() + 1));
            }
            int n = index2n(index);
            if (n == -2)
            {
                return std::string("");
            }
            chess::Move move = cached_moves_->move_at(n);
            const chess::Board& board = cached_moves_->board_at(n);
            const chess::Board& board_after = cached_moves_->board_at(n + 1);
            std::string text = move.str(board, board_after);
            if (move.turn_into() != chess::chessman_null)
            {
                chess::Color color = board.color(move.from());
                chess::Field ti_field(color, move.turn_into());
                std::string ti_src =
                    wApp->resolveRelativeUrl(BoardWidget::image(ti_field));
                return str(boost::format(
                    "<table><tr><td style='vertical-align:middle;'>%s =</td>"
                    "<td><img src='%s' /></td></tr></table>") %
                    text % ti_src);
            }
            else
            {
                chess::Field field = board.field(move.from());
                std::string img =
                    wApp->resolveRelativeUrl(BoardWidget::image(field));
                return str(boost::format(
                    "<table><tr>"
                    "<td><img src='%s' /></td>"
                    "<td style='vertical-align:middle;'>%s</td>"
                    "</tr></table>") % img % text);
            }
        }
        return std::string("");
    }

    virtual Wt::WFlags<Wt::ItemFlag> flags(const Wt::WModelIndex& index) const
    {
        Wt::WFlags<Wt::ItemFlag> flags_(Wt::ItemIsXHTMLText);
        if (index.column() != 0)
        {
            flags_ |= Wt::ItemIsSelectable;
        }
        return flags_;
    }

    void move_changed(int move_i)
    {
        int first = chess::Moves::n_to_human(move_i) - 1;
        int last = chess::Moves::n_to_human(max_show_move_) - 1;
        if (move_i <= max_show_move_)
        {
            beginRemoveRows(Wt::WModelIndex(), first, last);
            endRemoveRows();
        }
        if (first > last)
        {
            beginInsertRows(Wt::WModelIndex(), first, first);
            endInsertRows();
        }
        dataChanged().emit(n2index(move_i), n2index(move_i));
        max_show_move_ = move_i;
    }

private:
    chess::CachedMoves* cached_moves_;
    int max_show_move_;
};


class MovesWidgetImpl : public Wt::WContainerWidget
{
public:
    MovesWidgetImpl(const chess::Moves& moves,
        bool big, bool active, int max_moves,
        bool append_only, chess::Color bottom) :
    Wt::WContainerWidget(), cached_moves_(moves),
    current_move_(moves.size() - 1), max_moves_(max_moves),
    used_moves_(0), append_only_(append_only),
    active_(active), activated_(false)
    {
        Wt::WHBoxLayout *layout = new Wt::WHBoxLayout();
        setLayout(layout, Wt::AlignTop);
        layout->addWidget(board_widget_ =
            new BoardWidget(big, this->active(), bottom));
        check_activate_();
        Wt::WPushButton* goto_first =
            new Wt::WPushButton("<<", board_widget_->inner());
        goto_first->clicked().connect(this, &MovesWidgetImpl::goto_first_);
        Wt::WPushButton* goto_prev =
            new Wt::WPushButton("<", board_widget_->inner());
        goto_prev->clicked().connect(this, &MovesWidgetImpl::goto_prev_);
        Wt::WPushButton* goto_next =
            new Wt::WPushButton(">", board_widget_->inner());
        goto_next->clicked().connect(this, &MovesWidgetImpl::goto_next_);
        Wt::WPushButton* goto_last =
            new Wt::WPushButton(">>", board_widget_->inner());
        goto_last->clicked().connect(this, &MovesWidgetImpl::goto_last_);

        moves_model_ = new MovesModel(&cached_moves_, this);
        layout->addWidget(moves_table_view_ = new Wt::WTableView());
        moves_table_view_->setModel(moves_model_);
        int moves_per_page = big ? 20 : 10;
        int width = wApp->environment().ajax() ?
            ((moves_column_width + 20) * 2 + (n_column_width + 7) + 20) : 400;
        int height = moves_per_page * moves_row_height;
        height += wApp->environment().ajax() ? 0 : navigationBarHeight;
        moves_table_view_->resize(width, height);
        moves_table_view_->setRowHeight(moves_row_height);
        moves_table_view_->setHeaderHeight(0);
        for (int i = 1; i < 3; i++)
        {
            moves_table_view_->setColumnWidth(i, moves_column_width);
        }
        moves_table_view_->setColumnWidth(0, n_column_width);
        moves_table_view_->setSelectionBehavior(Wt::SelectItems);
        moves_table_view_->setSelectionMode(Wt::SingleSelection);
        moves_table_view_->clicked().connect(this, &MovesWidgetImpl::onselect_);
        goto_move_(current_move_); // last move
    }

    const chess::Moves& moves() const
    {
        return (chess::Moves&)cached_moves_;
    }

    const chess::Board& board() const
    {
        return cached_moves_.board_at(cached_moves_.size());
    }

    Wt::Signal<chess::Move>& move()
    {
        return move_signal_;
    }

    int current_move() const
    {
        return current_move_;
    }

    void add_move(const chess::Move& move)
    {
        int i = cached_moves_.size() - 1;
        if (i == -1 || move != cached_moves_.move_at(i))
        {
            int n = cached_moves_.size();
            reset_move_(n, move);
            goto_move_(n);
        }
    }

    void bottom_set(chess::Color bottom)
    {
        board_widget_->bottom_set(bottom);
    }

    void reset()
    {
        cached_moves_ = chess::CachedMoves();
        goto_move_(-1);
        moves_model_->move_changed(0);
    }

    void set_moves(const chess::Moves& moves)
    {
        cached_moves_ = chess::CachedMoves(moves);
        current_move_ = cached_moves_.size() - 1;
        moves_model_->move_changed(0);
        goto_move_(current_move_);
    }

    void set_active(bool active)
    {
        active_ = active;
        check_activate_();
        move_select_();
    }

private:
    BoardWidget* board_widget_;
    MovesModel* moves_model_;
    Wt::WTableView* moves_table_view_;
    Wt::Signal<chess::Move> move_signal_;

    chess::CachedMoves cached_moves_;
    int current_move_;
    int max_moves_;
    int used_moves_;
    bool append_only_;
    bool active_;
    bool activated_;

    void check_activate_()
    {
        if (!activated_ && this->active())
        {
            activated_ = true;
            board_widget_->move().connect(this, &MovesWidgetImpl::onmove_);
        }
    }

    void reset_move_(int n, const chess::Move& move)
    {
        cached_moves_.reset_move(n, move);
        moves_model_->move_changed(n);
    }

    void onmove_(const chess::Move& move)
    {
        if (active())
        {
            current_move_ += 1;
            used_moves_ += 1;
            reset_move_(current_move_, move);
            goto_move_(current_move_);
            move_signal_.emit(move);
        }
    }

    void onselect_(const Wt::WModelIndex& index)
    {
        int n = moves_model_->index2n(index);
        if (n == -2 && index.column() == 2)
        {
            // after last move
            n = cached_moves_.size() - 1;
        }
        if (n != -2)
        {
            current_move_ = n;
            move_select_();
        }

    }

    // -1 means start position
    void goto_move_(int n=-1)
    {
        current_move_ = n;
        move_select_();
        history_select_();
    }

    void goto_first_()
    {
        goto_move_(-1);
    }

    void goto_last_()
    {
        goto_move_(cached_moves_.size() - 1);
    }

    void goto_prev_()
    {
        if (current_move_ > -1)
        {
            goto_move_(current_move_ - 1);
        }
    }

    void goto_next_()
    {
        if (current_move_ < cached_moves_.size() - 1)
        {
            goto_move_(current_move_ + 1);
        }
    }

    bool active() const
    {
        std::cout << "active " << ( active_ &&
            (max_moves_ == -1 || used_moves_ < max_moves_) &&
            (!append_only_ || current_move_ == cached_moves_.size() - 1) )<< std::endl;
        return active_ &&
            (max_moves_ == -1 || used_moves_ < max_moves_) &&
            (!append_only_ || current_move_ == cached_moves_.size() - 1);
    }

    void move_select_()
    {
        const chess::Board& board = cached_moves_.board_at(current_move_ + 1);
        chess::Move lastmove = chess::move_null;
        if (current_move_ != -1)
        {
            lastmove = cached_moves_.move_at(current_move_);
        }
        board_widget_->set(board, lastmove, active());
    }

    void history_select_()
    {
        int page = (chess::Moves::n_to_human(current_move_) - 1) /
            moves_table_view_->pageSize();
        moves_table_view_->setCurrentPage(page); // html

        //FIXME - wron row is shown
        Wt::WModelIndex index = moves_model_->n2index(current_move_);
        moves_table_view_->scrollTo(index); // ajax

        if (current_move_ > -1)
        {
            moves_table_view_
                ->select(moves_model_->n2index(current_move_));
        }
        else
        {
            moves_table_view_->setSelectedIndexes(Wt::WModelIndexSet());
        }
    }

};


MovesWidget::MovesWidget(const chess::Moves& moves,
    bool big, bool active, int max_moves,
    bool append_only, chess::Color bottom,
    Wt::WContainerWidget* parent) :
WCompositeWidget(parent)
{
    impl_ = new MovesWidgetImpl(moves, big, active,
        max_moves, append_only, bottom);
    setImplementation(impl_);
}

const chess::Moves& MovesWidget::moves() const
{
    return impl_->moves();
}

const chess::Board& MovesWidget::board() const
{
    return impl_->board();
}


Wt::Signal<chess::Move>& MovesWidget::move()
{
    return impl_->move();
}

void MovesWidget::add_move(const chess::Move& move)
{
    impl_->add_move(move);
}

void MovesWidget::bottom_set(chess::Color bottom)
{
    impl_->bottom_set(bottom);
}

void MovesWidget::reset()
{
    impl_->reset();
}

void MovesWidget::set_moves(const chess::Moves& moves)
{
    impl_->set_moves(moves);
}

void MovesWidget::set_active(bool active)
{
    impl_->set_active(active);
}

int MovesWidget::current_move() const
{
    return impl_->current_move();
}

}
}
