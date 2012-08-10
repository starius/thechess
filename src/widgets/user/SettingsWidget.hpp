/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_SETTINGSWIDGET_HPP_
#define THECHESS_WIDGETS_SETTINGSWIDGET_HPP_

#include <Wt/WGlobal>
#include <Wt/WCompositeWidget>

namespace thechess {

class SettingsWidget : public Wt::WCompositeWidget {
public:
    SettingsWidget(Wt::WContainerWidget* parent = 0);
private:
    class SettingsWidgetImpl;

    SettingsWidgetImpl* impl_;
};

}

#endif

