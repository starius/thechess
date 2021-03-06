/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WTable>
#include <Wt/WAnchor>
#include <Wt/WDateTime>
#include <Wt/WPushButton>
#include <Wt/Auth/AuthWidget>
#include <Wt/Wc/util.hpp>
#include <Wt/Wc/SWFStore.hpp>
#include <Wt/Wc/Countdown.hpp>
#include <Wt/Wc/CachedContents.hpp>

#include "model/all.hpp"
#include "widgets/MainWidget.hpp"
#include "widgets/MainMenu.hpp"
#include "widgets/MainPage.hpp"
#include "widgets/Footer.hpp"
#include "widgets/TopBlock.hpp"
#include "widgets/Header.hpp" // FIXME
#include "widgets/user/AuthWidget.hpp"
#include "widgets/game/MyGamesList.hpp"
#include "widgets/chess/BoardWidget.hpp"
#include "widgets/chess/MovesWidget.hpp"
#include "widgets/game/GameWidget.hpp"
#include "widgets/game/GameCreateWidget.hpp"
#include "widgets/game/GameListWidget.hpp"
#include "widgets/gp/GPListWidget.hpp"
#include "widgets/gp/GPWidget.hpp"
#include "widgets/competition/CompetitionWidget.hpp"
#include "widgets/competition/CompetitionCreateWidget.hpp"
#include "widgets/competition/CompetitionListWidget.hpp"
#include "widgets/cp/CPWidget.hpp"
#include "widgets/user/UserWidget.hpp"
#include "widgets/user/UserListWidget.hpp"
#include "widgets/user/VirtualsWidget.hpp"
#include "widgets/user/SettingsWidget.hpp"
#include "widgets/user/IpList.hpp"
#include "widgets/user/AllSessions.hpp"
#include "widgets/user/IpBanList.hpp"
#include "widgets/user/NewIpBan.hpp"
#include "widgets/user/EditIpBan.hpp"
#include "widgets/comment/CommentList.hpp"
#include "widgets/comment/ForumCommentWidget.hpp"
#include "widgets/comment/ForumEdit.hpp"
#include "widgets/comment/ChatCommentWidget.hpp"
#include "widgets/team/TeamList.hpp"
#include "widgets/team/TeamWidget.hpp"
#include "chess/Moves.hpp"
#include "chess/Board.hpp"
#include "chess/Piece.hpp"
#include "log.hpp"
#include "Path.hpp" // FIXME
#include "Application.hpp" // FIXME

namespace thechess {

class MyCachedContents : public Wt::Wc::CachedContents {
public:
    MyCachedContents():
        path_(tApp->path()) {
        int widgets_cache_size = Options::instance()->widgets_cache_size();
        set_cache_size(widgets_cache_size);
        wApp->internalPathChanged()
        .connect(this, &MyCachedContents::my_open_url);
    }

    void my_open_url(const std::string& url) {
        Wt::Wc::url::Node* node = path_.parse(url);
        if (node == path_.game_view() ||
                node == path_.settings_page() ||
                node == path_.main_page() ||
                node == path_.random_competitor() ||
                node == path_.all_comments() ||
                node == path_.all_sessions() ||
                node == path_.all_banned_ip() ||
                node == path_.user_list() ||
                node == path_.user_view() ||
                node == path_.post() ||
                node == path_.forum_edit() ||
                node == path_.post_comment() ||
                node == path_.chat_comment()) {
            // ignore games because
            // 1) they load fast
            // 2) mymenu_->select_game()
            node->open(/* change_path */ false);
        } else {
            open_url(url);
        }
    }

protected:
    void open_url_impl(const std::string& url) {
        path_.open(url);
    }

private:
    Path& path_;
};

enum {
    LOGO_IN_TOP,
    AUTH_IN_TOP,
    TOP_IN_TOP
};

enum {
    MENU_IN_MIDDLE,
    CONTENTS_IN_MIDDLE,
    GAME_LIST_IN_MIDDLE
};

MainWidget::MainWidget(Wt::WContainerWidget* parent):
    Wt::WContainerWidget(parent),
    mymenu_(0), countup_(0), countup_updates_(0) {
    Wt::WTable* top = new Wt::WTable(this);
    Wt::WAnchor* logo = new Wt::WAnchor();
    logo->setText(tr("tc.common.Logo"));
    logo->setLink(Wt::WLink(Wt::WLink::InternalPath, "/")); // main page
    top->elementAt(0, 0)->addWidget(logo);
    auth_place_ = top->elementAt(0, AUTH_IN_TOP);
    top_place_ = top->elementAt(0, TOP_IN_TOP);
    Wt::WTable* middle = new Wt::WTable(this);
    middle->resize(Wt::WLength(100, Wt::WLength::Percentage),
                   Wt::WLength::Auto);
    menu_place_ = middle->elementAt(0, MENU_IN_MIDDLE);
    clock_and_locale_ = new Wt::WContainerWidget(menu_place_);
    clock_and_locale_->setStyleClass("no-wrap");
    menu_place_->addWidget(new Wt::WBreak);
    Wt::WPushButton* refresh;
    refresh = new Wt::WPushButton(tr("tc.common.Refresh"), menu_place_);
    refresh->clicked().connect(boost::bind(&MainWidget::refresh_url, this));
    Wt::WTableCell* contents_cell = middle->elementAt(0, CONTENTS_IN_MIDDLE);
    contents_place_ = new MyCachedContents;
    contents_cell->addWidget(contents_place_);
    contents_cell->resize(Wt::WLength(80, Wt::WLength::Percentage),
                          Wt::WLength::Auto);
    mygames_place_ = middle->elementAt(0, GAME_LIST_IN_MIDDLE);
    addWidget(new Footer());
    show_countup();
    show_is_banned();
}

void MainWidget::show_menu(Path* path) {
    menu_place_->addWidget(new MainMenu(path));
}

typedef MainWidget::LocaleSetter LocaleSetter;

static Wt::WWidget* create_locale_setter(const LocaleSetter& locale_setter,
        const std::string& locale) {
    Wt::WImage* result = new Wt::WImage("img/locale/" + locale + ".png");
    result->setHeight(16);
    result->clicked().connect(boost::bind(locale_setter, locale));
    result->setMargin(5, Wt::Left | Wt::Right);
    result->decorationStyle().setCursor(Wt::PointingHandCursor);
    return result;
}

void MainWidget::add_locale_setters(const LocaleSetter& locale_setter) {
    clock_and_locale_->addWidget(create_locale_setter(locale_setter, "en"));
    clock_and_locale_->addWidget(create_locale_setter(locale_setter, "ru"));
    clock_and_locale_->addWidget(create_locale_setter(locale_setter, "uk"));
    clock_and_locale_->addWidget(create_locale_setter(locale_setter, "be"));
}

MainMenu* MainWidget::main_menu() {
    return downcast<MainMenu*>(menu_place_->widget(menu_place_->count() - 1));
}

void MainWidget::update_my_games() {
    mygames_place_->clear();
    mymenu_ = 0;
    if (tApp->user()) {
        mymenu_ = new MyGamesList(tApp->user());
        mygames_place_->addWidget(mymenu_);
    }
}

void MainWidget::clear_cached_contents() {
    contents_place_->clear();
}

void MainWidget::main_page() {
    set_contents(new MainPage());
    wApp->setTitle(tr("tc.title.MainPage"));
}

void MainWidget::user_view(const UserPtr& user) {
    set_contents(new UserWidget(user));
    wApp->setTitle(tr("tc.title.UserWidget").arg(user->username20()));
}

void MainWidget::virtuals_of_user(const UserPtr& user) {
    set_contents(new VirtualsWidget(user));
    wApp->setTitle(tr("tc.title.VirtualsWidget_of").arg(user->username20()));
}

void MainWidget::virtuals_of_user_pair(const UserPtr& u_user,
                                       const UserPtr& v_user) {
    dbo::Query<BDPair> pairs = BD::pairs(tApp->session());
    pairs.where("U.user_id in(?,?) and V.user_id in(?,?)")
    .bind(u_user).bind(v_user).bind(u_user).bind(v_user);
    set_contents(new VirtualsWidget(pairs));
    wApp->setTitle(tr("tc.title.VirtualsWidget_of")
                   .arg(u_user->username20() + " - " + v_user->username20()));
}

void MainWidget::ip_of_user(const UserPtr& user) {
    set_contents(new IpList(user));
    wApp->setTitle(tr("tc.title.VirtualsWidget_of").arg(user->username20()));
}

void MainWidget::games_of_user(const UserPtr& user) {
    set_contents(new GameListWidget(user));
    wApp->setTitle(tr("tc.title.GameListWidget_of").arg(user->username20()));
}

void MainWidget::competitions_of_user(const UserPtr& user) {
    set_contents(new CompetitionListWidget(user));
    wApp->setTitle(tr("tc.title.CompetitionListWidget_of")
                   .arg(user->username20()));
}

void MainWidget::all_virtuals() {
    set_contents(new VirtualsWidget());
    wApp->setTitle(tr("tc.title.VirtualsWidget"));
}

void MainWidget::settings_page() {
    if (tApp->user()) {
        set_contents(new SettingsWidget());
        wApp->setTitle(tr("tc.title.SettingsWidget"));
    } else {
        set_contents(new Wt::WContainerWidget());
    }
}

void MainWidget::user_list() {
    set_contents(new UserListWidget());
    wApp->setTitle(tr("tc.title.UserListWidget"));
}

void MainWidget::update_password() {
    const Wt::Auth::User& user = auth_widget()->login().user();
    if (user.isValid()) {
        auth_widget()->letUpdatePassword(user, /* promptPassword */ true);
        // FIXME promptPassword does not work in Wt 3.2.1
        // FIXME Freezes, memory leak
    }
}

void MainWidget::game_view(const GamePtr& game) {
    set_contents(new GameWidget(game));
    if (game->name().empty()) {
        wApp->setTitle(tr("tc.title.GameWidget").arg(game.id()));
    } else {
        wApp->setTitle(tr("tc.title.GameWidget").arg(game->name()));
    }
    if (mymenu_) {
        // do this after GameWidget reread game
        mymenu_->select_game(game);
    }
}

void MainWidget::game_view(const GamePtr& game, int move_number) {
    GameWidget* widget = new GameWidget(game);
    widget->set_half_move(Moves::half_move_index(move_number, Piece::WHITE));
    set_contents(widget);
    wApp->setTitle(tr("tc.title.GameWidget").arg(game.id()));
    if (mymenu_) {
        // do this after GameWidget reread game
        mymenu_->select_game(game);
    }
}

void MainWidget::game_list() {
    set_contents(new GameListWidget());
    wApp->setTitle(tr("tc.title.GameListWidget"));
}

void MainWidget::game_new() {
    set_contents(new GameCreateWidget());
    wApp->setTitle(tr("tc.title.GameCreateWidget"));
}

void MainWidget::gp_list() {
    set_contents(new GPListWidget());
    wApp->setTitle(tr("tc.title.GPListWidget"));
}

void MainWidget::gp_view(const GPPtr& gp) {
    Wt::WContainerWidget* c = new Wt::WContainerWidget();
    // FIXME
    c->addWidget(new Header(tr("tc.game.Parameters") + " " + TO_S(gp.id())));
    c->addWidget(new GPWidget(gp.get()));
    // FIXME
    Wt::WAnchor* challenge = new Wt::WAnchor(c);
    tApp->path().gp_view()->set_integer_value(gp.id());
    challenge->setLink(tApp->path().gp_challenge()->link());
    challenge->setText(tr("tc.game.Challenge"));
    challenge->addStyleClass("thechess-main-button");
    set_contents(c);
    wApp->setTitle(tr("tc.title.GPWidget").arg(gp.id()));
}

void MainWidget::gp_challenge(const GPPtr& gp) {
    set_contents(new GameCreateWidget(gp));
    wApp->setTitle(tr("tc.title.GameCreateWidget"));
}

void MainWidget::user_challenge(const UserPtr& user) {
    set_contents(new GameCreateWidget(user));
    wApp->setTitle(tr("tc.title.GameCreateWidget_with")
                   .arg(user->username20()));
}

void MainWidget::competition_view(const CompetitionPtr& c) {
    set_contents(new CompetitionWidget(c));
    if (c->name().empty()) {
        wApp->setTitle(tr("tc.title.CompetitionWidget").arg(c.id()));
    } else {
        wApp->setTitle(tr("tc.title.CompetitionWidget").arg(c->name()));
    }
}

void MainWidget::games_of_competition(const CompetitionPtr& c) {
    set_contents(new GameListWidget(c));
    if (c->name().empty()) {
        wApp->setTitle(tr("tc.title.GameListWidget_of").arg(c.id()));
    } else {
        wApp->setTitle(tr("tc.title.GameListWidget_of").arg(c->name()));
    }
}

void MainWidget::virtuals_in_competition(const CompetitionPtr& c) {
    dbo::Query<BDPair> pairs;
    c->virtuals_pairs(pairs);
    set_contents(new VirtualsWidget(pairs));
    wApp->setTitle(tr("tc.title.VirtualsWidget_of").arg(c->name()));
}

void MainWidget::competition_list() {
    set_contents(new CompetitionListWidget());
    wApp->setTitle(tr("tc.title.CompetitionListWidget"));
}

void MainWidget::competition_new() {
    set_contents(new CompetitionCreateWidget());
    wApp->setTitle(tr("tc.title.CompetitionCreateWidget"));
}

void MainWidget::cp_view(const CPPtr& cp) {
    Wt::WContainerWidget* c = new Wt::WContainerWidget();
    c->addWidget(new Header(tr("tc.competition.Parameters") +
                            " " + TO_S(cp.id())));
    CPWidget* cp_widget = new CPWidget(cp.get());
    cp_widget->hide_gp_selector();
    c->addWidget(cp_widget);
    if (cp->gp()) {
        c->addWidget(new Wt::WBreak);
        Wt::WAnchor* a = new Wt::WAnchor(c);
        a->setText(tr("tc.game.Parameters") + " " + TO_S(cp->gp().id()));
        a->setLink(tApp->path().gp_view()->get_link(cp->gp().id()));
    }
    c->addWidget(new Wt::WBreak);
    Wt::WAnchor* challenge = new Wt::WAnchor(c);
    tApp->path().cp_view()->set_integer_value(cp.id());
    challenge->setLink(tApp->path().cp_challenge()->link());
    challenge->setText(tr("tc.competition.Create_welcome"));
    challenge->addStyleClass("thechess-main-button");
    set_contents(c);
    wApp->setTitle(tr("tc.title.CPWidget").arg(cp.id()));
}

void MainWidget::cp_challenge(const CPPtr& cp) {
    set_contents(new CompetitionCreateWidget(cp));
    wApp->setTitle(tr("tc.title.CompetitionCreateWidget"));
}

void MainWidget::board_view(const std::string& data) {
    BoardWidget* b;
    b = new BoardWidget(/* big */ false, /* active */ true,
                                  /* bottom */ Piece::WHITE, Board(data, true));
    b->show_lastmove_checkbox(false);
    set_contents(b);
    wApp->setTitle(tr("tc.title.BoardWidget"));
}

void MainWidget::board_games(const std::string& data) {
    set_contents(new GameListWidget(Board(data, true)));
    wApp->setTitle(tr("tc.title.GameListWidget"));
}

void MainWidget::moves_view(const std::string& m) {
    set_contents(new MovesWidget(Moves(m, /* check */ true),
                                 /* big */ false, /* active */ true));
    wApp->setTitle(tr("tc.title.MovesWidget_moves"));
}

void MainWidget::moves_games(const std::string& data) {
    set_contents(new GameListWidget(Moves(data, /* check */ true)));
    wApp->setTitle(tr("tc.title.GameListWidget"));
}

void MainWidget::moves_challenge(const std::string& data) {
    GameCreateWidget* gcw = new GameCreateWidget();
    gcw->set_moves(Moves(data, /* check */ true));
    set_contents(gcw);
    wApp->setTitle(tr("tc.title.GameCreateWidget"));
}

void MainWidget::moves_widget() {
    set_contents(new MovesWidget(Moves(), /* big */ false, /* active */ true));
    wApp->setTitle(tr("tc.title.MovesWidget"));
}

void MainWidget::forum_topics() {
    set_contents(new CommentList(Comment::FORUM_TOPIC));
    wApp->setTitle(tr("tc.title.CommentList_topics"));
}

void MainWidget::forum_topic_posts(const CommentPtr& topic) {
    set_contents(new CommentList(Comment::FORUM_POST, topic));
    wApp->setTitle(tr("tc.title.CommentList_topic").arg(topic->text()));
}

void MainWidget::forum_all_posts() {
    set_contents(new CommentList(Comment::FORUM_POST));
    wApp->setTitle(tr("tc.title.CommentList_posts"));
}

void MainWidget::forum_post(const CommentPtr& post_or_text) {
    CommentPtr post = post_or_text->type() == Comment::FORUM_POST ?
                      post_or_text : post_or_text->parent();
    if (post && post->children().size()) {
        CommentPtr post_text = post->children().front();
        set_contents(new CommentList(Comment::FORUM_COMMENT, post_text));
        wApp->setTitle(tr("tc.title.CommentList_post").arg(post->text()));
    }
}

void MainWidget::forum_post_comment(const CommentPtr& comment) {
    CommentPtr post_text = comment->root();
    set_contents(new ForumCommentWidget(comment));
    wApp->setTitle(tr("tc.title.CommentList_forum_comment").arg(comment.id()));
}

void MainWidget::forum_edit(const CommentPtr& post_or_text) {
    set_contents(new ForumEdit(post_or_text));
    wApp->setTitle(tr("tc.title.CommentList_edit"));
}

void MainWidget::chat_comment(const CommentPtr& comment) {
    set_contents(new ChatCommentWidget(comment));
    wApp->setTitle(tr("tc.title.ChatCommentWidget"));
}

void MainWidget::admin_log() {
    set_contents(new CommentList(Comment::LOG_ENTRY));
    wApp->setTitle(tr("tc.title.CommentList_admin_log"));
}

void MainWidget::all_banned_ip() {
    set_contents(new IpBanList());
    wApp->setTitle(tr("tc.title.IpBanList"));
}

void MainWidget::all_sessions() {
    set_contents(new AllSessions());
    wApp->setTitle(tr("tc.title.AllSessions"));
}

void MainWidget::banned_ip(const std::string& ip) {
    Wt::WContainerWidget* c = new Wt::WContainerWidget();
    c->addWidget(new NewIpBan(ip, ""));
    c->addWidget(new IpBanList(ip));
    c->addWidget(new IpList(ip));
    CommentList* comments = new CommentList(Comment::NO_TYPE);
    comments->set_ip(ip);
    c->addWidget(comments);
    set_contents(c);
    wApp->setTitle(tr("tc.title.IpBanList"));
}

void MainWidget::edit_banned_ip(const IpBanPtr& ban) {
    set_contents(new EditIpBan(ban));
    wApp->setTitle(tr("tc.title.IpBanList"));
}

void MainWidget::new_ip_ban(const std::string& ip, const UserPtr& user) {
    set_contents(new NewIpBan(ip, user_a(user.id())));
    wApp->setTitle(tr("tc.title.NewIpBan"));
}

void MainWidget::all_comments() {
    set_contents(new CommentList(Comment::NO_TYPE));
    wApp->setTitle(tr("tc.title.CommentList_all_comments"));
}

void MainWidget::my_messages() {
    set_contents(new CommentList(Comment::PRIVATE_MESSAGE));
    wApp->setTitle(tr("tc.title.CommentList_my_messages"));
}

void MainWidget::user_comments(const UserPtr& user) {
    set_contents(new CommentList(Comment::NO_TYPE, CommentPtr(), user));
    wApp->setTitle(tr("tc.title.CommentList_user_comments")
                   .arg(user->username20()));
}

void MainWidget::global_chat() {
    set_contents(new CommentList(Comment::CHAT_MESSAGE));
    wApp->setTitle(tr("tc.title.CommentList_global_chat"));
}

void MainWidget::teams_list() {
    set_contents(new TeamList);
    wApp->setTitle(tr("tc.title.TeamList"));
}

void MainWidget::team_view(const TeamPtr& team) {
    set_contents(new TeamWidget(team));
    if (team->name().empty()) {
        wApp->setTitle(tr("tc.title.TeamWidget").arg(team.id()));
    } else {
        wApp->setTitle(tr("tc.title.TeamWidget").arg(team->name()));
    }
}

void MainWidget::team_competitions(const TeamPtr& team) {
    set_contents(new CompetitionListWidget(team));
    wApp->setTitle(tr("tc.title.CompetitionListWidget_of")
                   .arg(team->name()));
}

Wt::Auth::AuthWidget* MainWidget::auth_widget() {
    return downcast<Wt::Auth::AuthWidget*>(auth_place_->widget(0));
}

void MainWidget::set_auth_widget() {
    auth_place_->clear();
    auth_place_->addWidget(new AuthWidget());
}

void MainWidget::set_top_block_shown(bool shown) {
    top_place_->clear();
    if (shown) {
        top_place_->addWidget(new TopBlock());
    }
}

void MainWidget::set_contents(WWidget* widget) {
    contents_place_->set_contents_raw(widget);
}

static void reset_countup(Wt::Wc::Countdown* countup) {
    Wt::WDateTime midnight = Wt::WDateTime(now().date());
    midnight -= tApp->timezone_diff();
    if (now() - midnight > DAY) {
        midnight += DAY;
    }
    countup->set_since(midnight);
    countup->set_format("HM");
    Wt::WDateTime next_midnight = midnight.addDays(1);
    Td wait = std::min(next_midnight - now() + SECOND, 15 * MINUTE);
    Wt::Wc::schedule_action(wait, Wt::Wc::bound_post(boost::bind(reset_countup,
                            countup)));
    Wt::Wc::updates_trigger();
}

void MainWidget::update_countup() {
    countup_updates_ += 1;
    if (countup_updates_ <= 3) {
        reset_countup(countup_);
    }
}

void MainWidget::show_countup() {
    countup_ = new Wt::Wc::Countdown(clock_and_locale_,
                                     /* load javascript */ false);
    countup_->setInline(true);
    reset_countup(countup_);
}

void MainWidget::show_is_banned() {
    if (IpBan::am_i_banned()) {
        menu_place_->addWidget(new Wt::WBreak());
        Wt::WAnchor* ban = new Wt::WAnchor;
        ban->setText(tr("tc.user.This_ip_banned"));
        ban->setRef("/html/banned.html");
        menu_place_->addWidget(ban);
    }
}

void MainWidget::refresh_url() {
    contents_place_->remove_from_cache(wApp->internalPath());
    contents_place_->open_url(wApp->internalPath());
}

}

