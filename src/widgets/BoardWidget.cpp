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
#include "chess/HalfMove.hpp"
#include "chess/Piece.hpp"

namespace thechess {

class BoardWidgetImpl;

struct PieceStat {
    PieceStat(const Board& board) {
        memset(stat, 0x00, sizeof(stat));
        THECHESS_SQUARE_FOREACH (square) {
            if (board.isset(square)) {
                stat[board.color(square)][board.letter(square)] += 1;
            }
        }
    }

    int stat[COLOR_COUNT][LETTER_COUNT];
};

PieceStat full_stat = PieceStat(Board());

class TakenPiecesImpl : public Wt::WContainerWidget {
public:

    TakenPiecesImpl(const Board& board):
        Wt::WContainerWidget(), taken_stat_(board) {
        print_color_(WHITE);
        new Wt::WBreak(this);
        print_color_(BLACK);
    }

private:
    PieceStat taken_stat_;

    void print_color_(Color color) {
        print_piece_(color, QUEEN);
        print_piece_(color, ROCK);
        print_piece_(color, KNIGHT);
        print_piece_(color, BISHOP);
        print_piece_(color, PAWN);
    }

    void print_piece_(Color color, Letter letter) {
        int c = full_stat.stat[color][letter] - taken_stat_.stat[color][letter];
        for (int i = 0; i < c; i++) {
            new Wt::WImage(BoardWidget::image(Piece(color, letter)), this);
        }
    }
};

class TakenPieces : public Wt::WViewWidget {
public:
    TakenPieces(const Board& board, Wt::WContainerWidget* parent = 0) :
        Wt::WViewWidget(parent), board_(board) {
    }

    Wt::WWidget* renderView() {
        return new TakenPiecesImpl(board_);
    }

private:
    const Board& board_;
};

class DnDPiece : public Wt::WImage {
public:
    DnDPiece(Square square, BoardWidgetImpl* bwi):
        square_(square), bwi_(bwi)
    { }

    void activate();
    void dropEvent(Wt::WDropEvent dropEvent);

private:
    Square square_;
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
        THECHESS_SQUARE_FOREACH (square) {
            DnDPiece* img = new DnDPiece(square, this);
            image_at(square) = img;
            std::string id = str(boost::format("%d%d")
                                 % (square.file() + 1) % (square.rank() + 1));
            board_template_->bindWidget(id, img);
        }
        check_activate_();
        board_build_();
        select_turn_into_ = new Wt::WContainerWidget(this);
        turn_button_place_ = new Wt::WContainerWidget(this);
        Wt::WPushButton* turn_button =
            new Wt::WPushButton(tr("tc.game.Overturn_board"), turn_button_place_);
        turn_button->clicked().connect(this, &BoardWidgetImpl::turn);
        taken_pieces_ = new TakenPieces(board_, this);
    }

    const char* xml_message() {
        return bottom_ == WHITE ?
               "tc.game.board_white_template" : "tc.game.board_black_template";
    }

    DnDPiece*& image_at(Square square) {
        return images_[square.i()];
    }

    Wt::Signal<HalfMove>& half_move() {
        return move_;
    }

    void set(const Board& board, HalfMove lastmove, bool active) {
        modify_undo_();
        color_noactive_undo_();
        lastmove_ = lastmove;
        board_ = board;
        taken_pieces_->update();
        if (board_.test_shah()) {
            shah_square_ = board_.find_king(board_.order());
        } else {
            shah_square_ = Square();
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
    friend class DnDPiece;

    Board board_;
    Color bottom_;
    bool active_;
    bool activated_;
    bool big_;
    bool lastmove_show_;

    Square from_;
    HalfMove lastmove_;
    Square shah_square_;

    Wt::WGridLayout* layout_;
    Wt::WContainerWidget* select_turn_into_;
    Wt::WContainerWidget* turn_button_place_;
    TakenPieces* taken_pieces_;
    Wt::WTemplate* board_template_;
    bool select_turn_into_flag_;
    DnDPiece* images_[64];
    Wt::WImage* draggable_;

    Wt::Signal<HalfMove> move_;

    void color_black_white_(Wt::WImage* img) {
        img->decorationStyle().setBackgroundColor(Wt::WColor());
    }

    void color_black_white_(Square square) {
        color_black_white_(image_at(square));
    }

    void color_noactive_() {
        if (lastmove_show_ && lastmove_ != MOVE_NULL) {
            image_at(lastmove_.from())->decorationStyle()
            .setBackgroundColor(Wt::yellow);
            image_at(lastmove_.to())->decorationStyle()
            .setBackgroundColor(Wt::yellow);
        }
        if (shah_square_ != Square()) {
            image_at(shah_square_)->decorationStyle().setBackgroundColor(Wt::red);
        }
    }

    void color_noactive_undo_() {
        if (lastmove_show_ && lastmove_ != MOVE_NULL) {
            color_black_white_(lastmove_.from());
            color_black_white_(lastmove_.to());
        }
        if (shah_square_ != Square()) {
            color_black_white_(shah_square_);
        }
    }

    void board_build_() {
        THECHESS_SQUARE_FOREACH (square) {
            Wt::WImage* img = image_at(square);
            img->setImageRef(BoardWidget::image(board_.piece(square), big_));
        }
    }

    void modify_from_() {
        THECHESS_SQUARE_FOREACH (square) {
            Wt::WImage* img = image_at(square);
            if (board_.can_move(square)) {
                img->decorationStyle().setCursor(Wt::PointingHandCursor);
            }
        }
    }

    void modify_from_undo_() {
        THECHESS_SQUARE_FOREACH (square) {
            if (board_.color(square) == board_.order()) {
                Wt::WImage* img = image_at(square);
                img->decorationStyle().setCursor(Wt::ArrowCursor);
            }
        }
    }

    void modify_to_() {
        bool can_move = false;
        THECHESS_SQUARE_FOREACH (square) {
            if (board_.test_move(HalfMove(from_, square))) {
                Wt::WImage* img = image_at(square);
                img->decorationStyle().setCursor(Wt::PointingHandCursor);
                can_move = true;
                if (board_.test_attack(HalfMove(from_, square))) {
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
        THECHESS_SQUARE_FOREACH (square) {
            Wt::WImage* img = image_at(square);
            img->decorationStyle().setCursor(Wt::ArrowCursor);
            color_black_white_(img);
        }
    }

    void modify_undo_() {
        if (from_ == Square()) {
            modify_from_undo_();
        } else {
            modify_to_undo_();
        }
        if (select_turn_into_flag_) {
            print_select_undo_();
        }
        from_ = Square();
    }

    void modify_() {
        color_noactive_();
        if (from_ == Square()) {
            modify_from_();
        } else {
            modify_to_();
        }
    }

    void onclick_(Square square) {
        if (select_turn_into_flag_) {
            return;
        }
        if (!active_) {
            return;
        }
        if (square == from_) {
            modify_undo_();
            from_ = Square();
            modify_();
        } else if (board_.color(square) == board_.order()) {
            modify_undo_();
            from_ = square;
            modify_();
        } else if (from_ != Square()) {
            try_move_(HalfMove(from_, square));
        }
    }

    void try_move_(const HalfMove half_move) {
        if (board_.test_move(half_move)) {
            if (half_move.could_turn_into(board_)) {
                select_turn_into_flag_ = true;
                print_select_(half_move);
                return;
            } else {
                modify_undo_();
                lastmove_ = half_move;
                from_ = Square();
                move_.emit(half_move);
            }
        } else {
            modify_undo_();
            from_ = Square();
            modify_();
        }
    }

    void print_select_(HalfMove half_move) {
        static Letter cc[] =
        { QUEEN, ROCK, BISHOP, KNIGHT };
        BOOST_FOREACH (Letter c, cc) {
            Piece cm = Piece(board_.order(), c);
            std::string path = BoardWidget::image(cm, big_);
            Wt::WImage* img = new Wt::WImage(path, select_turn_into_);
            img->clicked().connect(boost::bind(
                                       &BoardWidgetImpl::select_onclick_, this, c, half_move));
            img->decorationStyle().setCursor(Wt::PointingHandCursor);
        }
    }

    void print_select_undo_() {
        select_turn_into_->clear();
        select_turn_into_flag_ = false;
    }

    void select_onclick_(Letter letter, HalfMove half_move) {
        if (!active_) {
            return;
        }
        modify_undo_();
        from_ = Square();
        half_move.turn_into(letter);
        lastmove_ = half_move;
        move_.emit(half_move);
    }

    int file(Square::File file) {
        return (bottom_ == WHITE ? (int)file : (7 - (int)file)) + 1;
    }

    int rank(Square::Rank rank) {
        return (bottom_ == BLACK ? (int)rank : (7 - (int)rank)) + 1;
    }

    Square::File file(int file) {
        return (Square::File)(bottom_ == WHITE ? (file - 1) : (7 - (file - 1)));
    }

    Square::Rank rank(int rank) {
        return (Square::Rank)(bottom_ == BLACK ? (rank - 1) : (7 - (rank - 1)));
    }

    void correct_bottom_() {
        if (bottom_ == COLOR_NULL) {
            bottom_ = WHITE;
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
            THECHESS_SQUARE_FOREACH (square) {
                DnDPiece* img = image_at(square);
                img->activate();
            }
        }
    }

};

void DnDPiece::activate() {
    setDraggable("letter", bwi_->draggable_);
    acceptDrops("letter");
    if (wApp->environment().ajax()) {
        mouseWentDown().connect(boost::bind(&BoardWidgetImpl::onclick_, bwi_, square_));
        mouseWentDown().connect(
            "function(sender, e) {"
            "$(" + bwi_->draggable_->jsRef() + ").attr('src', $(sender).attr('src'));"
            "}");
    } else {
        clicked().connect(boost::bind(&BoardWidgetImpl::onclick_, bwi_, square_));
    }
}

void DnDPiece::dropEvent(Wt::WDropEvent dropEvent) {
    DnDPiece* source = dynamic_cast<DnDPiece*>(dropEvent.source());
    if (source->square_ != square_) {
        bwi_->try_move_(HalfMove(source->square_, square_));
    }
    doJavaScript("$(" + bwi_->draggable_->jsRef() + ").hide();");
}

std::string BoardWidget::image(Piece piece, bool big) {
    return str(boost::format("img/chess/board/%d-%d%s.GIF")
               % (2 - (int)piece.color())
               % (int)piece.letter()
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
                      HalfMove lastmove, bool active) {
    impl_->set(board, lastmove, active);
}

void BoardWidget::turn() {
    impl_->turn();
}

Wt::Signal<HalfMove>& BoardWidget::half_move() {
    return impl_->half_move();
}

Wt::WContainerWidget* BoardWidget::inner() {
    return impl_->inner();
}

}

