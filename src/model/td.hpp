/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_TD_HPP_
#define THECHESS_MODEL_TD_HPP_

#include <Wt/Dbo/SqlTraits>

#include "time_intervals.hpp"

namespace Wt {
namespace Dbo {

class SqlConnection;
class SqlStatement;

template<>
struct sql_value_traits<thechess::Td, void> {
    static const bool specialized = true;

    static const char *type(SqlConnection *conn, int size);
    static void bind(const thechess::Td& v, SqlStatement *statement,
                     int column, int size);
    static bool read(thechess::Td& v, SqlStatement *statement,
                     int column, int size);
};

}
}

#endif
