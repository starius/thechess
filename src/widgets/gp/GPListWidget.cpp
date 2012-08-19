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
#include <Wt/Wc/Pager.hpp>

#include "widgets/gp/GPListWidget.hpp"
#include "Application.hpp"

namespace thechess {

class GPListModel : public GPListWidget::BaseQM {
public:
    enum {
        N_COLUMN,
        NAME_COLUMN,
        DESCRIPTION_COLUMN,
        CREATED_COLUMN,
        LIMIT_STD_COLUMN,
        LIMIT_PRIVATE_COLUMN,
        NORATING_COLUMN,
        FIRST_DRAW_COLUMN
    };

    GPListModel(Wt::WObject* parent = 0) :
        GPListWidget::BaseQM(parent) {
        set_query();
        addColumn("id", tr("tc.common.number"));
        addColumn("name", tr("tc.common.Name"));
        addColumn("description", tr("tc.common.Description"));
        addColumn("created", tr("tc.common.created"));
        addColumn("limit_std", tr("tc.game.limit_std"));
        addColumn("limit_private_init", tr("tc.game.limit_private_init"));
        addColumn("norating", tr("tc.game.norating"));
        addColumn("first_draw", tr("tc.game.first_draw"));
    }

    boost::any data(const Wt::WModelIndex& index,
                    int role = Wt::DisplayRole) const {
        dbo::Transaction t(tApp->session());
        const GPPtr& o = resultRow(index.row());
        if (role == Wt::DisplayRole) {
            if (index.column() == LIMIT_STD_COLUMN) {
                return td2str(o->limit_std());
            } else if (index.column() == LIMIT_PRIVATE_COLUMN) {
                return td2str(o->limit_private_init());
            }
        } else if (role == Wt::LinkRole && index.column() == N_COLUMN) {
            return tApp->path().gp_view()->get_link(o.id());
        }
        return GPListWidget::BaseQM::data(index, role);
    }

    static Wt::WString tr(const char* key) {
        return Wt::WString::tr(key);
    }

    void set_query(bool only_my = false) {
        GPListWidget::Q q = tApp->session().find<GP>();
        if (only_my) {
            q.where("init_id = ?").bind(tApp->user());
        }
        setQuery(q, /* keep_columns */ true);
    }

    Wt::WModelIndex result_to_index(const GPPtr& ptr) {
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

class GPListView : public Wt::WTableView {
public:
    GPListView(GPListModel* model, Wt::WContainerWidget* p = 0):
        Wt::WTableView(p) {
        setModel(model);
        resize(770, 450);
        setColumnWidth(GPListModel::N_COLUMN, 40);
        setColumnWidth(GPListModel::NAME_COLUMN, 170);
        setColumnWidth(GPListModel::DESCRIPTION_COLUMN, 210);
        setColumnWidth(GPListModel::CREATED_COLUMN, 80);
        setColumnWidth(GPListModel::LIMIT_STD_COLUMN, 60);
        setColumnWidth(GPListModel::LIMIT_PRIVATE_COLUMN, 60);
        setColumnWidth(GPListModel::NORATING_COLUMN, 40);
        setColumnWidth(GPListModel::FIRST_DRAW_COLUMN, 30);
        setRowHeaderCount(1); // fixed id_columns when scrolling
    }

protected:
    WWidget* createPageNavigationBar() {
        return new Wt::Wc::Pager(this);
    }
};

GPListWidget::GPListWidget(Wt::WContainerWidget* p):
    Wt::WContainerWidget(p) {
    manager();
    model_ = new GPListModel(this);
    apply();
    view_ = new GPListView(model_, this);
    view_->setSelectionMode(Wt::SingleSelection);
}

GPPtr GPListWidget::gp() const {
    GPPtr result;
    if (!view_->selectedIndexes().empty()) {
        Wt::WModelIndex selected_index = *(view_->selectedIndexes().begin());
        result = model_->resultRow(selected_index.row());
    }
    return result;
}

void GPListWidget::set_gp(const GPPtr& gp) {
    Wt::WModelIndex index = model_->result_to_index(gp);
    if (index.isValid()) {
        view_->select(index);
        //view_->scrollTo(index); // FIXME http://redmine.emweb.be/issues/1380
    }
}

void GPListWidget::apply() {
    bool only_my = only_my_->isChecked() && tApp->user();
    model_->set_query(only_my);
}

void GPListWidget::manager() {
    only_my_ = new Wt::WCheckBox(tr("tc.common.Only_my"), this);
    only_my_->changed().connect(this, &GPListWidget::apply);
    if (!tApp->user()) {
        only_my_->setEnabled(false);
    }
    if (!tApp->environment().ajax()) {
        Wt::WPushButton* apply_button =
            new Wt::WPushButton(tr("tc.common.Apply"), this);
        apply_button->clicked().connect(this, &GPListWidget::apply);
    }
}

}

