/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_AUTH_WIDGET_HPP_
#define THECHESS_AUTH_WIDGET_HPP_

#include <Wt/WGlobal>
#include <Wt/Auth/AuthWidget>

namespace thechess {

/** Auth widget */
class AuthWidget : public Wt::Auth::AuthWidget {
public:
    /** Constructor */
    AuthWidget(Wt::WContainerWidget* parent = 0);

protected:
    /** Return registration model */
    Wt::Auth::RegistrationModel* createRegistrationModel();
};

}

#endif

