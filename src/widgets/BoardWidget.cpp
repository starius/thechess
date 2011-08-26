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
#include <Wt/WApplication>
#include <Wt/WEnvironment>

#include "widgets/BoardWidget.hpp"
#include "chess/move.hpp"
#include "chess/field.hpp"

namespace thechess {

class BoardWidgetImpl;

struct ChessmanStat {
    ChessmanStat(const Board& board) {
        memset(stat, 0x00, sizeof(stat));
        THECHESS_XY_FOREACH (xy) {
            if (board.isset(xy)) {
                stat[board.color(xy)][board.chessman(xy)] += 1;
            }
        }
    }

    int stat[Color_count][Chessman_count];
};

ChessmanStat full_stat = ChessmanStat(Board());

class TakenChessmenImpl : public Wt::WContainerWidget {
public:

    TakenChessmenImpl(const Board& board):
        Wt::WContainerWidget(), taken_stat_(board) {
        print_color_(white);
        new Wt::WBreak(this);
        print_color_(black);
    }

private:
    ChessmanStat taken_stat_;

    void print_color_(Color color) {
        print_field_(color, queen);
        print_field_(color, rock);
        print_field_(color, knight);
        print_field_(color, bishop);
        print_field_(color, pawn);
    }

    void print_field_(Color color, Chessman chessman) {
        int c = full_stat.stat[color][chessman] - taken_stat_.stat[color][chessman];
        for (int i = 0; i < c; i++) {
            new Wt::WImage(BoardWidget::image(Field(color, chessman)), this);
        }
    }
};

class TakenChessmen : public Wt::WViewWidget {
public:
    TakenChessmen(const Board& board, Wt::WContainerWidget* parent = 0) :
        Wt::WViewWidget(parent), board_(board) {
    }

    Wt::WWidget* renderView() {
        return new TakenChessmenImpl(board_);
    }

private:
    const Board& board_;
};

class DnDChessman : public Wt::WImage {
public:
    DnDChessman(Xy xy, BoardWidgetImpl* bwi):
        xy_(xy), bwi_(bwi)
    { }

    void activate();
    void dropEvent(Wt::WDropEvent dropEvent);

private:
    Xy xy_;
    BoardWidgetImpl* bwi_;
};

class BoardWidgetImpl : public Wt::WContainerWidget {
public:
    BoardWidgetImpl(bool big, bool active, Color bottom,
                    const Board& board) :
        Wt::WContainerWidget(), board_(board), bottom_(bottom),
        active_(active), activated_(false), big_(big), lastmove_show_(true),
        select_turn_into_flag_(false) {
        correct_bottom_();
        board_template_ = new Wt::WTemplate(tr(xml_message()), this);
        THECHESS_XY_FOREACH (xy) {
            DnDChessman* img = new DnDChessman(xy, this);
            image_at(xy) = img;
            std::string id = str(boost::format("%d%d")
                                 % (xy.x() + 1) % (xy.y() + 1));
            board_template_->bindWidget(id, img);
        }
        check_activate_();
        board_build_();
        select_turn_into_ = new Wt::WContainerWidget(this);
        turn_button_place_ = new Wt::WContainerWidget(this);
        Wt::WPushButton* turn_button =
            new Wt::WPushButton(tr("tc.game.Overturn_board"), turn_button_place_);
        turn_button->clicked().connect(this, &BoardWidgetImpl::turn);
        taken_chessmen_ = new TakenChessmen(board_, this);
    }

    const char* xml_message() {
        return bottom_ == white ?
               "tc.game.board_white_template" : "tc.game.board_black_template";
    }

    DnDChessman*& image_at(Xy xy) {
        return images_[xy.i()];
    }

    Wt::Signal<Move>& move() {
        return move_;
    }

    void set(const Board& board, Move lastmove, bool active) {
        modify_undo_();
        color_noactive_undo_();
        lastmove_ = lastmove;
        board_ = board;
        taken_chessmen_->update();
        if (board_.test_shah()) {
            shah_xy_ = board_.find_king(board_.order());
        } else {
            shah_xy_ = xy_null;
        }
        active_ = active;
        check_activate_();
        board_build_();
        color_noactive_();
        if (active_) {
            modify_();
        }
    }

    void bottom_set(Color bottom) {
        bottom_ = bottom;
        correct_bottom_();
        board_template_->setTemplateText(tr(xml_message()));
    }

    void turn() {
        bottom_set(other_color(bottom_));
    }

    Wt::WContainerWidget* inner() {
        return turn_button_place_;
    }

private:
    friend class DnDChessman;

    Board board_;
    Color bottom_;
    bool active_;
    bool activated_;
    bool big_;
    bool lastmove_show_;

    Xy from_;
    Move lastmove_;
    Xy shah_xy_;

    Wt::WGridLayout* layout_;
    Wt::WContainerWidget* select_turn_into_;
    Wt::WContainerWidget* turn_button_place_;
    TakenChessmen* taken_chessmen_;
    Wt::WTemplate* board_template_;
    bool select_turn_into_flag_;
    DnDChessman* images_[64];
    Wt::WImage* draggable_;

    Wt::Signal<Move> move_;

    void color_black_white_(Wt::WImage* img) {
        img->decorationStyle().setBackgroundColor(Wt::WColor());
    }

    void color_black_white_(Xy xy) {
        color_black_white_(image_at(xy));
    }

    void color_noactive_() {
        if (lastmove_show_ && lastmove_ != move_null) {
            image_at(lastmove_.from())->decorationStyle()
            .setBackgroundColor(Wt::yellow);
            image_at(lastmove_.to())->decorationStyle()
            .setBackgroundColor(Wt::yellow);
        }
        if (shah_xy_ != xy_null) {
            image_at(shah_xy_)->decorationStyle().setBackgroundColor(Wt::red);
        }
    }

    void color_noactive_undo_() {
        if (lastmove_show_ && lastmove_ != move_null) {
            color_black_white_(lastmove_.from());
            color_black_white_(lastmove_.to());
        }
        if (shah_xy_ != xy_null) {
            color_black_white_(shah_xy_);
        }
    }

    void board_build_() {
        THECHESS_XY_FOREACH (xy) {
            Wt::WImage* img = image_at(xy);
            img->setImageRef(BoardWidget::image(board_.field(xy), big_));
        }
    }

    void modify_from_() {
        THECHESS_XY_FOREACH (xy) {
            Wt::WImage* img = image_at(xy);
            if (board_.can_move(xy)) {
                img->decorationStyle().setCursor(Wt::PointingHandCursor);
            }
        }
    }

    void modify_from_undo_() {
        THECHESS_XY_FOREACH (xy) {
            if (board_.color(xy) == board_.order()) {
                Wt::WImage* img = image_at(xy);
                img->decorationStyle().setCursor(Wt::ArrowCursor);
            }
        }
    }

    void modify_to_() {
        bool can_move = false;
        THECHESS_XY_FOREACH (xy) {
            if (board_.test_move(Move(from_, xy))) {
                Wt::WImage* img = image_at(xy);
                img->decorationStyle().setCursor(Wt::PointingHandCursor);
                can_move = true;
                if (board_.test_attack(Move(from_, xy))) {
                    img->decorationStyle().setBackgroundColor(Wt::magenta);
                } else {
                    img->decorationStyle().setBackgroundColor(Wt::green);
                }
            }
        }
        Wt::WImage* from_img = image_at(from_);
        if (can_move) {
            from_img->decorationStyle().setBackgroundColor(Wt::darkCyan);
        } else {
            from_img->decorationStyle().setBackgroundColor(Wt::darkRed);
        }
    }

    void modify_to_undo_() {
        THECHESS_XY_FOREACH (xy) {
            Wt::WImage* img = image_at(xy);
            img->decorationStyle().setCursor(Wt::ArrowCursor);
            color_black_white_(img);
        }
    }

    void modify_undo_() {
        if (from_ == xy_null) {
            modify_from_undo_();
        } else {
            modify_to_undo_();
        }
        if (select_turn_into_flag_) {
            print_select_undo_();
        }
        from_ = xy_null;
    }

    void modify_() {
        color_noactive_();
        if (from_ == xy_null) {
            modify_from_();
        } else {
            modify_to_();
        }
    }

    void onclick_(Xy xy) {
        if (select_turn_into_flag_) {
            return;
        }
        if (!active_) {
            return;
        }
        if (xy == from_) {
            modify_undo_();
            from_ = xy_null;
            modify_();
        } else if (board_.color(xy) == board_.order()) {
            modify_undo_();
            from_ = xy;
            modify_();
        } else if (from_ != xy_null) {
            try_move_(Move(from_, xy));
        }
    }

    void try_move_(const Move move) {
        if (board_.test_move(move)) {
            if (move.could_turn_into(board_)) {
                select_turn_into_flag_ = true;
                print_select_(move);
                return;
            } else {
                modify_undo_();
                lastmove_ = move;
                from_ = xy_null;
                move_.emit(move);
            }
        } else {
            modify_undo_();
            from_ = xy_null;
            modify_();
        }
    }

    void print_select_(Move move) {
        static Chessman cc[] =
        { queen, rock, bishop, knight };
        BOOST_FOREACH (Chessman c, cc) {
            Field cm = Field(board_.order(), c);
            std::string path = BoardWidget::image(cm, big_);
            Wt::WImage* img = new Wt::WImage(path, select_turn_into_);
            img->clicked().connect(boost::bind(
                                       &BoardWidgetImpl::select_onclick_, this, c, move));
            img->decorationStyle().setCursor(Wt::PointingHandCursor);
        }
    }

    void print_select_undo_() {
        select_turn_into_->clear();
        select_turn_into_flag_ = false;
    }

    void select_onclick_(Chessman chessman, Move move) {
        if (!active_) {
            return;
        }
        modify_undo_();
        from_ = xy_null;
        move.turn_into(chessman);
        lastmove_ = move;
        move_.emit(move);
    }

    int x(Xname x) {
        return (bottom_ == white ? (int)x : (7 - (int)x)) + 1;
    }

    int y(Yname y) {
        return (bottom_ == black ? (int)y : (7 - (int)y)) + 1;
    }

    Xname x(int x) {
        return (Xname)(bottom_ == white ? (x - 1) : (7 - (x - 1)));
    }

    Yname y(int y) {
        return (Yname)(bottom_ == black ? (y - 1) : (7 - (y - 1)));
    }

    void correct_bottom_() {
        if (bottom_ == color_null) {
            bottom_ = white;
        }
    }

    void check_activate_() {
        if (!activated_ && active_) {
            activated_ = true;
            draggable_ = new Wt::WImage(this);
            if (wApp->environment().ajax())
                doJavaScript(
                    "$(" + jsRef() + ").mouseleave(function(e) {"
                    "$(" + draggable_->jsRef() + ").hide();"
                    "});"
                );
            THECHESS_XY_FOREACH (xy) {
                DnDChessman* img = image_at(xy);
                img->activate();
            }
        }
    }

};

void DnDChessman::activate() {
    setDraggable("chessman", bwi_->draggable_);
    acceptDrops("chessman");
    if (wApp->environment().ajax()) {
        mouseWentDown().connect(boost::bind(&BoardWidgetImpl::onclick_, bwi_, xy_));
        mouseWentDown().connect(
            "function(sender, e) {"
            "$(" + bwi_->draggable_->jsRef() + ").attr('src', $(sender).attr('src'));"
            "}");
    } else {
        clicked().connect(boost::bind(&BoardWidgetImpl::onclick_, bwi_, xy_));
    }
}

void DnDChessman::dropEvent(Wt::WDropEvent dropEvent) {
    DnDChessman* source = dynamic_cast<DnDChessman*>(dropEvent.source());
    if (source->xy_ != xy_) {
        bwi_->try_move_(Move(source->xy_, xy_));
    }
    doJavaScript("$(" + bwi_->draggable_->jsRef() + ").hide();");
}

std::string BoardWidget::image(Field field, bool big) {
    return str(boost::format("img/chess/board/%d-%d%s.GIF")
               % (2 - (int)field.color())
               % (int)field.chessman()
               % (big ? "-B" : ""));
}

BoardWidget::BoardWidget(bool big, bool active, Color bottom,
                         const Board& board, Wt::WContainerWidget* parent) :
    WCompositeWidget(parent) {
    impl_ = new BoardWidgetImpl(big, active, bottom, board);
    setImplementation(impl_);
}

void BoardWidget::bottom_set(Color bottom) {
    impl_->bottom_set(bottom);
}

void BoardWidget::set(const Board& board,
                      Move lastmove, bool active) {
    impl_->set(board, lastmove, active);
}

void BoardWidget::turn() {
    impl_->turn();
}

Wt::Signal<Move>& BoardWidget::move() {
    return impl_->move();
}

Wt::WContainerWidget* BoardWidget::inner() {
    return impl_->inner();
}

}
