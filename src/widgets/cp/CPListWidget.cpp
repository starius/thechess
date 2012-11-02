/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <sstream>
#include <boost/foreach.hpp>

#include <Wt/WTableView>
#include <Wt/Dbo/Transaction>
#include <Wt/Dbo/QueryModel>
#include <Wt/WString>
#include <Wt/WPushButton>
#include <Wt/WCheckBox>
#include <Wt/WLineEdit>
#include <Wt/WEnvironment>
#include <Wt/Wc/Pager.hpp>
#include <Wt/Wc/util.hpp>

#include "widgets/cp/CPListWidget.hpp"
#include "widgets/cp/CompetitionTypeWidget.hpp"
#include "Application.hpp"

namespace thechess {

class CPListModel : public CPListWidget::BaseQM {
public:
    enum {
        N_COLUMN,
        NAME_COLUMN,
        TYPE_COLUMN,
        MIN_RATING_COLUMN,
        MAX_RATING_COLUMN,
        MIN_CLASSIFICATION_COLUMN,
        MAX_CLASSIFICATION_COLUMN,
        FORCE_START_DELAY_COLUMN
    };

    CPListModel(Wt::WObject* parent = 0) :
        CPListWidget::BaseQM(parent) {
        set_query();
        addColumn("id", tr("tc.common.number"));
        addColumn("name", tr("tc.common.Name"));
        addColumn("type", tr("tc.competition.Type"));
        addColumn("min_rating", tr("tc.competition.min_rating"));
        addColumn("max_rating", tr("tc.competition.max_rating"));
        addColumn("min_classification",
                  tr("tc.competition.min_classification"));
        addColumn("max_classification",
                  tr("tc.competition.max_classification"));
        addColumn("force_start_delay", tr("tc.game.force_start"));
    }

    boost::any data(const Wt::WModelIndex& index,
                    int role = Wt::DisplayRole) const {
        dbo::Transaction t(tApp->session());
        const CPPtr& o = resultRow(index.row());
        if (role == Wt::DisplayRole) {
            if (index.column() == TYPE_COLUMN) {
                return Competition::type2str(o->type());
            } else if (index.column() == MIN_CLASSIFICATION_COLUMN) {
                return User::classification2str(o->min_classification());
            } else if (index.column() == MAX_CLASSIFICATION_COLUMN) {
                return User::classification2str(o->max_classification());
            } else if (index.column() == FORCE_START_DELAY_COLUMN) {
                return td2str(o->force_start_delay());
            }
        } else if (role == Wt::LinkRole && index.column() == N_COLUMN) {
            return tApp->path().cp_view()->get_link(o.id());
        }
        return CPListWidget::BaseQM::data(index, role);
    }

    static Wt::WString tr(const char* key) {
        return Wt::WString::tr(key);
    }

    void set_only_my(bool only_my) {
        if (only_my != only_my_) {
            only_my_ = only_my;
            set_query();
        }
    }

    void set_type(CompetitionType type) {
        if (type != type_) {
            type_ = type;
            set_query();
        }
    }

    void set_name_like(const Wt::WString& name_like) {
        if (name_like != name_like_) {
            name_like_ = name_like;
            set_query();
        }
    }

    void set_query() {
        CPListWidget::Q q = tApp->session().find<CP>();
        if (only_my_) {
            q.where("init_id = ?").bind(tApp->user());
        }
        if (type_ != NO_COMPETITION_TYPE) {
            q.where("type = ?").bind(type_);
        }
        if (!name_like_.empty()) {
            q.where("(name like ? or id = ?)");
            q.bind("%" + name_like_ + "%");
            q.bind(Wt::Wc::str2int(name_like_.toUTF8()));
        }
        if (cp_) {
            q.orderBy("id = " + TO_S(cp_.id()) +
                      " desc, competitions_size desc");
        } else {
            q.orderBy("competitions_size desc");
        }
        setQuery(q, /* keep_columns */ true);
    }

    void set_cp(const CPPtr& cp) {
        cp_ = cp;
        set_query();
    }

private:
    bool only_my_;
    CPPtr cp_;
    CompetitionType type_;
    Wt::WString name_like_;
};

class CPListView : public Wt::WTableView {
public:
    CPListView(CPListModel* model, Wt::WContainerWidget* p = 0):
        Wt::WTableView(p) {
        setModel(model);
        resize(770, 450);
        setColumnWidth(CPListModel::N_COLUMN, 40);
        setColumnWidth(CPListModel::NAME_COLUMN, 170);
        setColumnWidth(CPListModel::TYPE_COLUMN, 75);
        setColumnWidth(CPListModel::MIN_RATING_COLUMN, 40);
        setColumnWidth(CPListModel::MAX_RATING_COLUMN, 40);
        setColumnWidth(CPListModel::MIN_CLASSIFICATION_COLUMN, 70);
        setColumnWidth(CPListModel::MAX_CLASSIFICATION_COLUMN, 70);
        setColumnWidth(CPListModel::FORCE_START_DELAY_COLUMN, 60);
        setRowHeaderCount(1); // fixed id_columns when scrolling
        setSelectable(true);
    }

protected:
    WWidget* createPageNavigationBar() {
        return new Wt::Wc::Pager(this);
    }
};

CPListWidget::CPListWidget(Wt::WContainerWidget* p):
    Wt::WContainerWidget(p) {
    manager();
    model_ = new CPListModel(this);
    view_ = new CPListView(model_, this);
    view_->setSelectionMode(Wt::SingleSelection);
    apply();
    select_first();
}

CPPtr CPListWidget::cp() const {
    CPPtr result;
    if (!view_->selectedIndexes().empty()) {
        Wt::WModelIndex selected_index = *(view_->selectedIndexes().begin());
        result = model_->resultRow(selected_index.row());
    }
    return result;
}

void CPListWidget::set_cp(const CPPtr& cp) {
    model_->set_cp(cp);
    if (model_->rowCount() && model_->resultRow(0) == cp) {
        select_first();
    }
}

void CPListWidget::apply() {
    bool only_my = only_my_->isChecked() && tApp->user();
    User::set_s(SWITCH_ONLY_MY_CP, only_my);
    model_->set_only_my(only_my);
    model_->set_type(type_->value());
    model_->set_name_like(name_like_->text());
    view_->setCurrentPage(0);
}

void CPListWidget::manager() {
    only_my_ = new Wt::WCheckBox(tr("tc.common.Only_my"), this);
    only_my_->setChecked(User::has_s(SWITCH_ONLY_MY_CP));
    only_my_->changed().connect(this, &CPListWidget::apply);
    if (!tApp->user()) {
        only_my_->setEnabled(false);
    }
    type_ = new CompetitionTypeWidget(/* with_all */ true, this);
    type_->changed().connect(this, &CPListWidget::apply);
    name_like_ = new Wt::WLineEdit(this);
    name_like_->setEmptyText(tr("tc.common.Name"));
    name_like_->enterPressed().connect(this, &CPListWidget::apply);
    if (!tApp->environment().ajax()) {
        Wt::WPushButton* apply_button =
            new Wt::WPushButton(tr("tc.common.Apply"), this);
        apply_button->clicked().connect(this, &CPListWidget::apply);
    }
}

void CPListWidget::select_first() {
    if (model_->rowCount()) {
        view_->select(model_->index(0, 0));
    }
}

}

