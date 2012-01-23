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
#include <Wt/WEnvironment>

#include "widgets/cp/CPListWidget.hpp"
#include "Application.hpp"

namespace thechess {

class CPListModel : public CPListWidget::BaseQM {
public:
    static const int N_COLUMN = 0;
    static const int NAME_COLUMN = 1;
    static const int TYPE_COLUMN = 2;
    static const int MIN_RATING_COLUMN = 3;
    static const int MAX_RATING_COLUMN = 4;
    static const int MIN_CLASSIFICATION_COLUMN = 5;
    static const int MAX_CLASSIFICATION_COLUMN = 6;
    static const int FORCE_START_DELAY_COLUMN = 7;

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

    void set_query(bool only_my = false) {
        CPListWidget::Q q = tApp->session().find<CP>();
        if (only_my) {
            q.where("init_id = ?").bind(tApp->user().id());
        }
        setQuery(q, /* keep_columns */ true);
    }

    Wt::WModelIndex result_to_index(const CPPtr& ptr) {
        sort(N_COLUMN);
        int result = -1;
        if (ptr) {
            try {
                int row = ptr.id() - 1;
                if (resultRow(row).id() == ptr.id()) {
                    result = row;
                } else {
                    // shift
                    row -= resultRow(row).id() - ptr.id();
                    if (resultRow(row).id() == ptr.id()) {
                        result = row;
                    }
                }
            } catch (...)
            { }
        }
        return result != -1 ? index(result, N_COLUMN) : Wt::WModelIndex();
    }
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
    }
};

CPListWidget::CPListWidget(Wt::WContainerWidget* p):
    Wt::WContainerWidget(p) {
    manager();
    model_ = new CPListModel(this);
    apply();
    view_ = new CPListView(model_, this);
    view_->setSelectionMode(Wt::SingleSelection);
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
    Wt::WModelIndex index = model_->result_to_index(cp);
    if (index.isValid()) {
        view_->select(index);
        view_->scrollTo(index);
    }
}

void CPListWidget::apply() {
    bool only_my = only_my_->isChecked() && tApp->user();
    model_->set_query(only_my);
}

void CPListWidget::manager() {
    only_my_ = new Wt::WCheckBox(tr("tc.common.Only_my"), this);
    only_my_->changed().connect(this, &CPListWidget::apply);
    if (!tApp->user()) {
        only_my_->setEnabled(false);
    }
    if (!tApp->environment().ajax()) {
        Wt::WPushButton* apply_button =
            new Wt::WPushButton(tr("tc.common.Apply"), this);
        apply_button->clicked().connect(this, &CPListWidget::apply);
    }
}

}

