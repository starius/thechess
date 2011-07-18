/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

//

#include <cstdlib>
#include <string>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>

#include <Wt/WColor>
#include <Wt/WContainerWidget>
#include <Wt/WCssDecorationStyle>
#include <Wt/WFlags>
#include <Wt/WGridLayout>
#include <Wt/WImage>
#include <Wt/WPushButton>
#include <Wt/WSignal>
#include <Wt/WTable>
#include <Wt/WTemplate>
#include <Wt/WText>
#include <Wt/WWidget>
#include <Wt/WViewWidget>
#include <Wt/WBreak>

#include "widgets/BoardWidget.hpp"
#include "chess/move.hpp"
#include "chess/field.hpp"

namespace thechess {
namespace widgets {

struct ChessmanStat
{
    ChessmanStat(const chess::Board& board)
    {
        memset(stat, 0x00, sizeof(stat));
        THECHESS_XY_FOREACH(xy)
        {
            if (board.isset(xy))
            {
                stat[board.color(xy)][board.chessman(xy)] += 1;
            }
        }
    }

    int stat[chess::Color_count][chess::Chessman_count];
};

ChessmanStat full_stat = ChessmanStat(chess::Board());

class TakenChessmenImpl : public Wt::WContainerWidget
{
public:

    TakenChessmenImpl(const chess::Board& board):
    Wt::WContainerWidget(), taken_stat_(board)
    {
        print_color_(chess::white);
        new Wt::WBreak(this);
        print_color_(chess::black);
    }

private:
    ChessmanStat taken_stat_;

    void print_color_(chess::Color color)
    {
        print_field_(color, chess::queen);
        print_field_(color, chess::rock);
        print_field_(color, chess::knight);
        print_field_(color, chess::bishop);
        print_field_(color, chess::pawn);
    }

    void print_field_(chess::Color color, chess::Chessman chessman)
    {
        int c = full_stat.stat[color][chessman] - taken_stat_.stat[color][chessman];
        for (int i = 0; i < c; i++)
        {
            new Wt::WImage(BoardWidget::image(chess::Field(color, chessman)), this);
        }
    }
};

class TakenChessmen : public Wt::WViewWidget
{
public:
    TakenChessmen(const chess::Board& board, Wt::WContainerWidget* parent=0) :
    Wt::WViewWidget(parent), board_(board)
    {
    }

    Wt::WWidget* renderView()
    {
        return new TakenChessmenImpl(board_);
    }

private:
    const chess::Board& board_;
};

class BoardWidgetImpl : public Wt::WContainerWidget
{
public:
    BoardWidgetImpl(bool big, bool active, chess::Color bottom,
        const chess::Board& board) :
    Wt::WContainerWidget(), board_(board), bottom_(bottom),
    active_(active), activated_(false), big_(big), lastmove_show_(true),
    select_turn_into_flag_(false)
    {
        correct_bottom_();
        board_template_ = new Wt::WTemplate(tr(xml_message()), this);
        THECHESS_XY_FOREACH(xy)
        {
            Wt::WImage* img = new Wt::WImage();
            image_at(xy) = img;
            std::string id = str(boost::format("%d%d")
                    % (xy.x() + 1) % (xy.y() + 1));
            board_template_->bindWidget(id, img);
        }
        check_activate_();
        board_build_();
        select_turn_into_ = new Wt::WContainerWidget(this);
        turn_button_place_ = new Wt::WContainerWidget(this);
        Wt::WPushButton *turn_button =
            new Wt::WPushButton(tr("tc.game.Overturn_board"), turn_button_place_);
        turn_button->clicked().connect(this, &BoardWidgetImpl::turn);
        taken_chessmen_ = new TakenChessmen(board_, this);
    }

    const char* xml_message()
    {
        return bottom_ == chess::white ?
            "tc.game.board_white_template" : "tc.game.board_black_template";
    }

    Wt::WImage*& image_at(chess::Xy xy)
    {
        return images_[xy.i()];
    }

    Wt::Signal<chess::Move>& move()
    {
        return move_;
    }

    void set(const chess::Board& board, chess::Move lastmove, bool active)
    {
        modify_undo_();
        color_noactive_undo_();
        lastmove_ = lastmove;
        board_ = board;
        taken_chessmen_->update();
        if (board_.test_shah())
        {
            shah_xy_ = board_.find_king(board_.order());
        }
        else
        {
            shah_xy_ = chess::xy_null;
        }
        active_ = active;
        check_activate_();
        board_build_();
        color_noactive_();
        if (active_)
        {
            modify_();
        }
    }

    void bottom_set(chess::Color bottom)
    {
        bottom_ = bottom;
        correct_bottom_();
        board_template_->setTemplateText(tr(xml_message()));
    }

    void turn()
    {
        bottom_set(chess::other_color(bottom_));
    }

    Wt::WContainerWidget* inner()
    {
        return turn_button_place_;
    }

private:
    chess::Board board_;
    chess::Color bottom_;
    bool active_;
    bool activated_;
    bool big_;
    bool lastmove_show_;

    chess::Xy from_;
    chess::Move lastmove_;
    chess::Xy shah_xy_;

    Wt::WGridLayout* layout_;
    Wt::WContainerWidget* select_turn_into_;
    Wt::WContainerWidget* turn_button_place_;
    TakenChessmen* taken_chessmen_;
    Wt::WTemplate* board_template_;
    bool select_turn_into_flag_;
    Wt::WImage* images_[64];

    Wt::Signal<chess::Move> move_;

    void color_black_white_(Wt::WImage* img)
    {
        img->decorationStyle().setBackgroundColor(Wt::WColor());
    }

    void color_black_white_(chess::Xy xy)
    {
        color_black_white_(image_at(xy));
    }

    void color_noactive_()
    {
        if (lastmove_show_ && lastmove_ != chess::move_null)
        {
            image_at(lastmove_.from())->decorationStyle()
            .setBackgroundColor(Wt::yellow);
            image_at(lastmove_.to())->decorationStyle()
            .setBackgroundColor(Wt::yellow);
        }
        if (shah_xy_ != chess::xy_null)
        {
            image_at(shah_xy_)->decorationStyle().setBackgroundColor(Wt::red);
        }
    }

    void color_noactive_undo_()
    {
        if (lastmove_show_ && lastmove_ != chess::move_null)
        {
            color_black_white_(lastmove_.from());
            color_black_white_(lastmove_.to());
        }
        if (shah_xy_ != chess::xy_null)
        {
            color_black_white_(shah_xy_);
        }
    }

    void board_build_()
    {
        THECHESS_XY_FOREACH(xy)
        {
            Wt::WImage* img = image_at(xy);
            img->setImageRef(BoardWidget::image(board_.field(xy), big_));
        }
    }


    void modify_from_()
    {
        THECHESS_XY_FOREACH(xy)
        {
            Wt::WImage* img = image_at(xy);
            if (board_.can_move(xy))
            {
                img->decorationStyle().setCursor(Wt::PointingHandCursor);
            }
        }
    }

    void modify_from_undo_()
    {
        THECHESS_XY_FOREACH(xy)
        {
            if (board_.color(xy) == board_.order())
            {
                Wt::WImage* img = image_at(xy);
                img->decorationStyle().setCursor(Wt::ArrowCursor);
            }
        }
    }

    void modify_to_()
    {
        bool can_move = false;
        THECHESS_XY_FOREACH(xy)
        {
            if (board_.test_move(chess::Move(from_, xy)))
            {
                Wt::WImage* img = image_at(xy);
                img->decorationStyle().setCursor(Wt::PointingHandCursor);
                can_move = true;
                if (board_.test_attack(chess::Move(from_, xy)))
                {
                    img->decorationStyle().setBackgroundColor(Wt::magenta);
                }
                else
                {
                    img->decorationStyle().setBackgroundColor(Wt::green);
                }
            }
        }
        Wt::WImage* from_img = image_at(from_);
        if (can_move)
        {
            from_img->decorationStyle().setBackgroundColor(Wt::darkCyan);
        }
        else
        {
            from_img->decorationStyle().setBackgroundColor(Wt::darkRed);
        }
    }

    void modify_to_undo_()
    {
        THECHESS_XY_FOREACH(xy)
        {
            Wt::WImage* img = image_at(xy);
            img->decorationStyle().setCursor(Wt::ArrowCursor);
            color_black_white_(img);
        }
    }

    void modify_undo_()
    {
        if (from_ == chess::xy_null)
        {
            modify_from_undo_();
        }
        else
        {
            modify_to_undo_();
        }
        if (select_turn_into_flag_)
        {
            print_select_undo_();
        }
        from_ = chess::xy_null;
    }

    void modify_()
    {
        color_noactive_();
        if (from_ == chess::xy_null)
        {
            modify_from_();
        }
        else
        {
            modify_to_();
        }
    }

    void onclick_(chess::Xy xy)
    {
        if (select_turn_into_flag_) return;
        if (!active_) return;
        if (xy == from_)
        {
            modify_undo_();
            from_ = chess::xy_null;
            modify_();
        }
        else if (board_.color(xy) == board_.order())
        {
            modify_undo_();
            from_ = xy;
            modify_();
        }
        else if (from_ != chess::xy_null)
        {
            if (board_.test_move(chess::Move(from_, xy)))
            {
                chess::Move move(from_, xy);
                if (move.could_turn_into(board_))
                {
                    select_turn_into_flag_ = true;
                    print_select_(move);
                    return;
                }
                else
                {
                    modify_undo_();
                    lastmove_ = move;
                    from_ = chess::xy_null;
                    move_.emit(move);
                }
            }
            else
            {
                modify_undo_();
                from_ = chess::xy_null;
                modify_();
            }
        }
    }

    void print_select_(chess::Move move)
    {
        static chess::Chessman cc[] =
            { chess::queen, chess::rock, chess::bishop, chess::knight };
        BOOST_FOREACH(chess::Chessman c, cc )
        {
            chess::Field cm = chess::Field(board_.order(), c);
            std::string path = BoardWidget::image(cm, big_);
            Wt::WImage* img = new Wt::WImage(path, select_turn_into_);
            img->clicked().connect(boost::bind(
                &BoardWidgetImpl::select_onclick_, this, c, move));
            img->decorationStyle().setCursor(Wt::PointingHandCursor);
        }
    }

    void print_select_undo_()
    {
        select_turn_into_->clear();
        select_turn_into_flag_ = false;
    }

    void select_onclick_(chess::Chessman chessman, chess::Move move)
    {
        if (!active_) return;
        modify_undo_();
        from_ = chess::xy_null;
        move.turn_into(chessman);
        lastmove_ = move;
        move_.emit(move);
    }

    int x(chess::Xname x)
    {
        return (bottom_==chess::white ? (int)x : (7 - (int)x)) + 1;
    }

    int y(chess::Yname y)
    {
        return (bottom_==chess::black ? (int)y : (7 - (int)y)) + 1;
    }

    chess::Xname x(int x)
    {
        return (chess::Xname)(bottom_==chess::white ? (x-1) : (7 - (x-1)));
    }

    chess::Yname y(int y)
    {
        return (chess::Yname)(bottom_==chess::black ? (y-1) : (7 - (y-1)));
    }

    void correct_bottom_()
    {
        if (bottom_ == chess::color_null)
        {
            bottom_ = chess::white;
        }
    }

    void check_activate_()
    {
        if (!activated_ && active_)
        {
            activated_ = true;
            THECHESS_XY_FOREACH(xy)
            {
                Wt::WImage* img = image_at(xy);
                img->clicked()
                .connect(boost::bind(&BoardWidgetImpl::onclick_,
                    this, xy));
            }
        }
    }

};




std::string BoardWidget::image(chess::Field field, bool big)
{
    return str(boost::format("img/chess/board/%d-%d%s.GIF")
    % (2 - (int)field.color())
    % (int)field.chessman()
    % (big ? "-B" : ""));
}

BoardWidget::BoardWidget(bool big, bool active, chess::Color bottom,
    const chess::Board& board, Wt::WContainerWidget *parent) :
WCompositeWidget(parent)
{
    impl_ = new BoardWidgetImpl(big, active, bottom, board);
    setImplementation(impl_);
}

void BoardWidget::bottom_set(chess::Color bottom)
{
    impl_->bottom_set(bottom);
}

void BoardWidget::set(const chess::Board& board,
    chess::Move lastmove, bool active)
{
    impl_->set(board, lastmove, active);
}

void BoardWidget::turn()
{
    impl_->turn();
}

Wt::Signal<chess::Move>& BoardWidget::move()
{
    return impl_->move();
}

Wt::WContainerWidget* BoardWidget::inner()
{
    return impl_->inner();
}

}
}
