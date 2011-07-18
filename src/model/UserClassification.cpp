/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */


#include "model/UserClassification.hpp"

namespace thechess {
namespace model {

Wt::WString classification2str(Classification c)
{
    if (c == SUPER_GRANDMASTER)
        return Wt::WString::tr("tc.user.classification_super_grandmaster");
    if (c == GRANDMASTER)
        return Wt::WString::tr("tc.user.classification_grandmaster");
    if (c == INTERNATIONAL_MASTER)
        return Wt::WString::tr("tc.user.classification_intarnational_master");
    if (c == NATIONAL_MASTER)
        return Wt::WString::tr("tc.user.classification_national_master");
    if (c == EXPERT)
        return Wt::WString::tr("tc.user.classification_expert");
    if (c == CLASSIFICATION_A)
        return Wt::WString::tr("tc.user.classification_a");
    if (c == CLASSIFICATION_B)
        return Wt::WString::tr("tc.user.classification_b");
    if (c == CLASSIFICATION_C)
        return Wt::WString::tr("tc.user.classification_c");
    if (c == CLASSIFICATION_D)
        return Wt::WString::tr("tc.user.classification_d");
    if (c == CLASSIFICATION_E)
        return Wt::WString::tr("tc.user.classification_e");
    if (c == CLASSIFICATION_F)
        return Wt::WString::tr("tc.user.classification_f");
    if (c == CLASSIFICATION_G)
        return Wt::WString::tr("tc.user.classification_g");
    if (c == CLASSIFICATION_H)
        return Wt::WString::tr("tc.user.classification_h");
    if (c == CLASSIFICATION_I)
        return Wt::WString::tr("tc.user.classification_i");
    if (c == CLASSIFICATION_J)
        return Wt::WString::tr("tc.user.classification_j");
    return Wt::WString::tr("tc.user.classification_none");
}

}
}


