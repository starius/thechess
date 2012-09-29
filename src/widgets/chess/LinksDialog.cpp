/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <sstream>

#include <Wt/WLineEdit>
#include <Wt/WTextArea>
#include <Wt/WAnchor>
#include <Wt/Wc/TableForm.hpp>

#include "widgets/chess/LinksDialog.hpp"
#include "Application.hpp"

namespace thechess {

LinksDialog::LinksDialog():
    WDialog(tr("tc.common.Links")) {
    f_ = new Wt::Wc::TableForm(contents());
    setClosable(true);
}

void LinksDialog::add_board(const Board& board, const Moves* moves) {
    url::StringNode* board_node = tApp->path().board();
    board_node->set_string(board.to_string());
    add_url(board_node->full_path(), tr("tc.common.Board"));
    add_url(tApp->path().board_games()->full_path(), tr("tc.game.With_board"));
    std::stringstream fen;
    if (moves) {
        moves->fen(fen);
    } else {
        board.fen(fen, 0, 0);
    }
    add_text(fen.str(), tr("tc.game.FEN"));
    // TODO bit.ly
}

void LinksDialog::add_moves(const Moves& moves) {
    url::StringNode* moves_node = tApp->path().moves();
    moves_node->set_string(moves.to_string());
    add_url(moves_node->full_path(), tr("tc.common.Moves"));
    add_url(tApp->path().moves_challenge()->full_path(),
            tr("tc.game.Challenge"));
    add_url(tApp->path().moves_games()->full_path(),
            tr("tc.game.Started_with"));
    add_san(moves);
    // TODO bit.ly
}

void LinksDialog::add_game_move(int game, int move_n) {
    tApp->path().game_view()->set_integer_value(game);
    url::IntegerNode* move_node = tApp->path().game_move_view();
    move_node->set_integer_value(move_n);
    add_url(move_node->full_path(),
            tr("tc.common.Game") + " + " + tr("tc.common.Moves"));
}

void LinksDialog::add_game(int game) {
    Wt::WString format("/thechess.js?type=game&game={1}&div=thechess_game_{1}");
    std::string url = tApp->makeAbsoluteUrl(format.arg(game).toUTF8());
    std::string c_url = tApp->makeAbsoluteUrl("js/jquery.countdown.min.js");
    add_textarea(tr("tc.game.external_code").arg(game).arg(url).arg(c_url),
                 tr("tc.common.Game"));
}

void LinksDialog::add_text(const std::string& text, const Wt::WString& name,
                           const std::string& internal_path) {
    Wt::WContainerWidget* c = new Wt::WContainerWidget();
    Wt::WLineEdit* l = new Wt::WLineEdit(text, c);
    l->setTextSize(40);
    l->focussed().connect("function(o, e) {"
                          "$(o).select();"
                          // Work around Chrome's little problem
                          "$(o).mouseup(function() {"
                          // Prevent further mouseup intervention
                          "$(o).unbind('mouseup');"
                          "return false;"
                          "});"
                          "}");
    if (!internal_path.empty()) {
        Wt::WAnchor* open = new Wt::WAnchor(c);
        open->setText(tr("tc.common.Open"));
        open->setRefInternalPath(internal_path);
    }
    f_->item(name, "", l, c);
}

void LinksDialog::add_textarea(const Wt::WString& text,
                               const Wt::WString& name) {
    Wt::WTextArea* t = new Wt::WTextArea(text);
    t->setColumns(80);
    f_->item(name, "", t, t);
}

void LinksDialog::add_url(const std::string& path, const Wt::WString& name) {
    std::string url = tApp->makeAbsoluteUrl(path);
    add_text(url, name, path);
}

void LinksDialog::add_san(const Moves& moves) {
    std::stringstream san;
    moves.pgn(san, /* result */ "");
    add_textarea(san.str(), tr("tc.game.SAN"));
}

}

