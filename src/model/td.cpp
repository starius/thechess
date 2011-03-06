
#include <Wt/Dbo/SqlConnection>
#include <Wt/Dbo/SqlStatement>
#include <Wt/Dbo/WtSqlTraits>

#include "model/td.hpp"

namespace Wt {
namespace Dbo {

const char* sql_value_traits<thechess::Td>::type(SqlConnection* conn,
    int /* size */)
{
    return conn->dateTimeType(SqlTime);
}

void sql_value_traits<thechess::Td>::bind(const thechess::Td& v,
    SqlStatement *statement, int column,
    int /* size */)
{
    statement->bind(column, v);
}

bool sql_value_traits<thechess::Td>::read(thechess::Td& v,
    SqlStatement *statement, int column, int /*size*/)
{
    thechess::Td value;
    bool notNull = statement->getResult(column, &value);
    if (notNull)
    {
        v = value;
    }
    return notNull;
}

}
}

