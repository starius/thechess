/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <cstdlib>
#include <string>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>

#include <Wt/WColor>
#include <Wt/WContainerWidget>
#include <Wt/WCssDecorationStyle>
#include <Wt/WFlags>
#include <Wt/WImage>
#include <Wt/WPushButton>
#include <Wt/WCheckBox>
#include <Wt/WSignal>
#include <Wt/WTable>
#include <Wt/WTemplate>
#include <Wt/WText>
#include <Wt/WAnchor>
#include <Wt/WWidget>
#include <Wt/WViewWidget>
#include <Wt/WBreak>
#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/Wc/util.hpp>

#include "widgets/chess/BoardWidget.hpp"
#include "widgets/chess/LinksDialog.hpp"
#include "chess/HalfMove.hpp"
#include "chess/Piece.hpp"
#include "model/all.hpp"
#include "Application.hpp"
#include "Path.hpp"
#include "config.hpp"

namespace thechess {

class BoardWidgetImpl;

class TakenPiecesImpl : public Wt::WContainerWidget {
public:

    TakenPiecesImpl(const Board& board):
        Wt::WContainerWidget(), taken_stat_(board) {
        print_color(Piece::WHITE);
        new Wt::WBreak(this);
        print_color(Piece::BLACK);
    }

private:
    Board::TakenPieceStat taken_stat_;
    int row_size_;

    void print_color(Piece::Color color) {
        row_size_ = 0;
        print_piece(color, Piece::QUEEN);
        print_piece(color, Piece::ROOK);
        print_piece(color, Piece::KNIGHT);
        print_piece(color, Piece::BISHOP);
        print_piece(color, Piece::PAWN);
    }

    void print_piece(Piece::Color color, Piece::Letter letter) {
        int c = taken_stat_.stat[color][letter];
        for (int i = 0; i < c; i++) {
            new Wt::WImage(BoardWidget::image(Piece(color, letter)), this);
            row_size_ += 1;
            if (row_size_ == 8) {
                row_size_ = 0;
                new Wt::WBreak(this);
            }
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
    typedef BoardWidget::LinksHandler LinksHandler;

    BoardWidgetImpl(bool big, bool active, Piece::Color bottom,
                    const Board& board) :
        Wt::WContainerWidget(), board_(board), bottom_(bottom),
        active_(active), activated_(false),
        taken_pieces_(0),
        select_turn_into_flag_(false),
        dialog_(0) {
        Wt::WPushButton* b;
        lastmove_show_ = User::has_s(SWITCH_LASTMOVE);
        big_ = User::has_s(SWITCH_BIG);
        correct_bottom();
        board_template_ = new Wt::WTemplate(tr(xml_message()), this);
        THECHESS_SQUARE_FOREACH (square) {
            DnDPiece* img = new DnDPiece(square, this);
            image_at(square) = img;
            std::string id = str(boost::format("%d%d")
                                 % (square.file() + 1) % (square.rank() + 1));
            board_template_->bindWidget(id, img);
        }
        check_activate();
        board_build();
        select_turn_into_ = new Wt::WContainerWidget(this);
        turn_button_place_ = new Wt::WContainerWidget(this);
        Wt::WPushButton* links = new Wt::WPushButton(tr("tc.common.Links"));
        turn_button_place_->addWidget(links);
        links->clicked().connect(this, &BoardWidgetImpl::show_links);
        set_links_handler(boost::bind(&BoardWidgetImpl::links, this, _1));
        Wt::WPushButton* turn_button = new Wt::WPushButton(turn_button_place_);
        turn_button->setText(tr("tc.game.Overturn_board"));
        turn_button->clicked().connect(this, &BoardWidgetImpl::turn);
        if (!User::has_s(SWITCH_LESS_GAME_INFO)) {
            taken_pieces_ = new TakenPieces(board_, this);
        }
        lastmove_box_ = new Wt::WCheckBox(this);
        lastmove_box_->setText(tr("tc.game.Highlight_lastmove"));
        lastmove_box_->setChecked(lastmove_show_);
        lastmove_box_->changed().connect(
            boost::bind(&BoardWidgetImpl::show_lastmove, this,
                        boost::bind<bool>(&Wt::WAbstractToggleButton::isChecked,
                                          lastmove_box_)));
        if (!wApp->environment().ajax()) {
            b = new Wt::WPushButton(tr("tc.common.Apply"), this);
            b->clicked().connect(
                boost::bind(&BoardWidgetImpl::show_lastmove, this,
                            boost::bind<bool>(
                                &Wt::WAbstractToggleButton::isChecked,
                                lastmove_box_)));
        }
        new Wt::WBreak(this);
        big_box_ = new Wt::WCheckBox(this);
        big_box_->setText(tr("tc.game.Enlarged_images"));
        big_box_->setChecked(big_);
        big_box_->changed().connect(this, &BoardWidgetImpl::big_changed);
        if (!wApp->environment().ajax()) {
            b = new Wt::WPushButton(tr("tc.common.Apply"), this);
            b->clicked().connect(this, &BoardWidgetImpl::big_changed);
        }
    }

    ~BoardWidgetImpl() {
        delete dialog_;
        dialog_ = 0;
    }

    const char* xml_message() {
        return bottom_ == Piece::WHITE ?
               "tc.game.board_white_template" : "tc.game.board_black_template";
    }

    DnDPiece*& image_at(Square square) {
        return images_[square.i()];
    }

    Wt::Signal<HalfMove>& half_move() {
        return move_;
    }

    void set(const Board& board, HalfMove lastmove, bool active) {
        modify_undo();
        color_noactive_undo();
        lastmove_ = lastmove;
        board_ = board;
        if (taken_pieces_) {
            taken_pieces_->update();
        }
        if (board_.test_shah()) {
            shah_square_ = board_.find_king(board_.order());
        } else {
            shah_square_ = Square();
        }
        active_ = active;
        check_activate();
        board_build();
        color_noactive();
        if (active_) {
            modify();
        }
    }

    void bottom_set(Piece::Color bottom) {
        bottom_ = bottom;
        correct_bottom();
        board_template_->setTemplateText(tr(xml_message()));
    }

    void turn() {
        bottom_set(Piece::other_color(bottom_));
    }

    Wt::WContainerWidget* inner() {
        return turn_button_place_;
    }

    void show_lastmove(bool show = true) {
        color_noactive_undo();
        lastmove_show_ = show;
        User::set_s(SWITCH_LASTMOVE, show);
        modify();
        lastmove_box_->setChecked(show);
    }

    void show_lastmove_checkbox(bool show) {
        lastmove_box_->setHidden(!show);
    }

    void set_links_handler(const LinksHandler& links_handler) {
        links_handler_ = links_handler;
    }

    void links(LinksDialog* dialog) {
        dialog->add_board(board_);
    }

private:
    friend class DnDPiece;

    Board board_;
    Piece::Color bottom_;
    bool active_;
    bool activated_;
    bool big_;
    bool lastmove_show_;

    Square from_;
    HalfMove lastmove_;
    Square shah_square_;

    Wt::WContainerWidget* select_turn_into_;
    Wt::WContainerWidget* turn_button_place_;
    TakenPieces* taken_pieces_;
    Wt::WTemplate* board_template_;
    bool select_turn_into_flag_;
    DnDPiece* images_[64];
    Wt::WImage* draggable_;
    Wt::WCheckBox* lastmove_box_;
    Wt::WCheckBox* big_box_;

    Wt::Signal<HalfMove> move_;
    LinksHandler links_handler_;
    LinksDialog* dialog_;

    void color_black_white(Wt::WImage* img) {
        img->decorationStyle().setBackgroundColor(Wt::WColor());
    }

    void color_black_white(Square square) {
        color_black_white(image_at(square));
    }

    void color_noactive() {
        if (lastmove_show_ && lastmove_) {
            image_at(lastmove_.from())->decorationStyle()
            .setBackgroundColor(Wt::yellow);
            image_at(lastmove_.to())->decorationStyle()
            .setBackgroundColor(Wt::yellow);
        }
        if (shah_square_) {
            image_at(shah_square_)->decorationStyle()
            .setBackgroundColor(Wt::red);
        }
    }

    void color_noactive_undo() {
        if (lastmove_show_ && lastmove_) {
            color_black_white(lastmove_.from());
            color_black_white(lastmove_.to());
        }
        if (shah_square_) {
            color_black_white(shah_square_);
        }
    }

    void board_build() {
        THECHESS_SQUARE_FOREACH (square) {
            Wt::WImage* img = image_at(square);
            img->setImageRef(BoardWidget::image(board_.piece(square), big_));
        }
    }

    void modify_from() {
        THECHESS_SQUARE_FOREACH (square) {
            Wt::WImage* img = image_at(square);
            if (board_.can_move(square)) {
                img->decorationStyle().setCursor(Wt::PointingHandCursor);
            }
        }
    }

    void modify_from_undo() {
        THECHESS_SQUARE_FOREACH (square) {
            if (board_.color(square) == board_.order()) {
                Wt::WImage* img = image_at(square);
                img->decorationStyle().setCursor(Wt::ArrowCursor);
            }
        }
    }

    void modify_to() {
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

    void modify_to_undo() {
        color_black_white();
    }

    void color_black_white() {
        THECHESS_SQUARE_FOREACH (square) {
            Wt::WImage* img = image_at(square);
            img->decorationStyle().setCursor(Wt::ArrowCursor);
            color_black_white(img);
        }
    }

    void modify_undo() {
        if (!from_) {
            modify_from_undo();
        } else {
            modify_to_undo();
        }
        if (select_turn_into_flag_) {
            print_select_undo();
        }
        from_ = Square();
    }

    void modify() {
        color_noactive();
        if (!from_) {
            modify_from();
        } else {
            modify_to();
        }
    }

    void onclick(Square square) {
        if (select_turn_into_flag_) {
            return;
        }
        if (!active_) {
            return;
        }
        if (square == from_) {
            modify_undo();
            from_ = Square();
            modify();
        } else if (board_.color(square) == board_.order()) {
            modify_undo();
            from_ = square;
            modify();
        } else if (from_) {
            try_move(HalfMove(from_, square));
        }
    }

    void try_move(const HalfMove half_move) {
        if (board_.test_move(half_move)) {
            if (half_move.could_turn_into(board_)) {
                select_turn_into_flag_ = true;
                print_select(half_move);
                return;
            } else {
                move_.emit(half_move);
            }
        } else {
            modify_undo();
            from_ = Square();
            modify();
        }
    }

    void print_select(HalfMove half_move) {
        static Piece::Letter cc[] =
        { Piece::QUEEN, Piece::ROOK, Piece::BISHOP, Piece::KNIGHT };
        BOOST_FOREACH (Piece::Letter c, cc) {
            Piece cm = Piece(board_.order(), c);
            std::string path = BoardWidget::image(cm, big_);
            Wt::WImage* img = new Wt::WImage(path, select_turn_into_);
            img->clicked().connect(boost::bind(
                                       &BoardWidgetImpl::select_onclick,
                                       this, c, half_move));
            img->decorationStyle().setCursor(Wt::PointingHandCursor);
        }
    }

    void print_select_undo() {
        select_turn_into_->clear();
        select_turn_into_flag_ = false;
    }

    void select_onclick(Piece::Letter letter, HalfMove half_move) {
        if (!active_) {
            return;
        }
        modify_undo();
        from_ = Square();
        half_move.turn_into(letter);
        move_.emit(half_move);
    }

    int file(Square::File file) {
        return (bottom_ == Piece::WHITE ? (int)file : (7 - (int)file)) + 1;
    }

    int rank(Square::Rank rank) {
        return (bottom_ == Piece::BLACK ? (int)rank : (7 - (int)rank)) + 1;
    }

    Square::File file(int file) {
        return (Square::File)
               (bottom_ == Piece::WHITE ? (file - 1) : (7 - (file - 1)));
    }

    Square::Rank rank(int rank) {
        return (Square::Rank)(bottom_ == Piece::BLACK ?
                              (rank - 1) : (7 - (rank - 1)));
    }

    void correct_bottom() {
        if (bottom_ == Piece::COLOR_NULL) {
            bottom_ = Piece::WHITE;
        }
    }

    void check_activate() {
        if (!activated_ && active_) {
            activated_ = true;
            draggable_ = new Wt::WImage(this);
            if (wApp->environment().ajax())
                doJavaScript(
                    // FIXME https://bitbucket.org/starius/thechess/issue/34
                    "$(" + draggable_->jsRef() + ").hide();"
                    "$(" + draggable_->jsRef() + ").css('z-index', 10);"
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

    void big_changed() {
        big_ = big_box_->isChecked();
        User::set_s(SWITCH_BIG, big_);
        board_build();
    }

    void show_links() {
        if (dialog_) {
            return;
        }
        dialog_ = new LinksDialog();
        if (!links_handler_.empty()) {
            links_handler_(dialog_);
        }
        dialog_->show();
        dialog_->finished().connect(this, &BoardWidgetImpl::dialog_closed);
    }

    void dialog_closed() {
        delete dialog_;
        dialog_ = 0;
    }

};

void DnDPiece::activate() {
    setDraggable("letter", bwi_->draggable_);
    acceptDrops("letter");
    if (wApp->environment().ajax()) {
        mouseWentDown().connect(boost::bind(&BoardWidgetImpl::onclick,
                                            bwi_, square_));
        std::string js_id = bwi_->draggable_->jsRef();
        mouseWentDown().connect(
            "function(sender, e) {"
            "$(" + js_id + ").attr('src', $(sender).attr('src'));"
            "}");
    } else {
        clicked().connect(boost::bind(&BoardWidgetImpl::onclick,
                                      bwi_, square_));
    }
}

void DnDPiece::dropEvent(Wt::WDropEvent dropEvent) {
    DnDPiece* source = downcast<DnDPiece*>(dropEvent.source());
    if (source->square_ != square_) {
        bwi_->try_move(HalfMove(source->square_, square_));
    }
    doJavaScript("$(" + bwi_->draggable_->jsRef() + ").hide();");
}

std::string BoardWidget::image(Piece piece, bool big) {
    return str(boost::format("img/chess/board/%d-%d%s.GIF")
               % (2 - (int)piece.color())
               % (int)piece.letter()
               % (big ? "-B" : ""));
}

BoardWidget::BoardWidget(bool big, bool active, Piece::Color bottom,
                         const Board& board, Wt::WContainerWidget* parent) :
    WCompositeWidget(parent) {
    impl_ = new BoardWidgetImpl(big, active, bottom, board);
    setImplementation(impl_);
}

void BoardWidget::bottom_set(Piece::Color bottom) {
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

void BoardWidget::show_lastmove(bool show) {
    impl_->show_lastmove(show);
}

void BoardWidget::show_lastmove_checkbox(bool show) {
    impl_->show_lastmove_checkbox(show);
}

void BoardWidget::set_links_handler(const LinksHandler& links_handler) {
    impl_->set_links_handler(links_handler);
}

void BoardWidget::links(LinksDialog* dialog) {
    impl_->links(dialog);
}

}

