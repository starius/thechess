/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WLineEdit>
#include <Wt/Wc/TableForm.hpp>

#include "widgets/chess/LinksDialog.hpp"
#include "Application.hpp"

namespace thechess {

LinksDialog::LinksDialog():
    WDialog(tr("tc.common.Links")) {
    f_ = new Wt::Wc::TableForm(contents());
    setClosable(true);
}

void LinksDialog::add_board(const Board& board) {
    url::StringNode* board_node = tApp->path().board();
    board_node->set_string(board.to_string());
    add_url(tApp->makeAbsoluteUrl(board_node->full_path()),
            tr("tc.common.Board"));
    // TODO bit.ly
}

void LinksDialog::add_moves(const Moves& moves) {
    url::StringNode* moves_node = tApp->path().moves();
    moves_node->set_string(moves.to_string());
    add_url(tApp->makeAbsoluteUrl(moves_node->full_path()),
            tr("tc.common.Moves"));
    // TODO bit.ly
}

void LinksDialog::add_url(const std::string& url, const Wt::WString& name) {
    Wt::WLineEdit* l = new Wt::WLineEdit(url);
    l->setTextSize(40);
    // TODO select all
    f_->item(name, "", l, l);
}

}

