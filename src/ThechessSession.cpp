#include <boost/foreach.hpp>

#include "ThechessSession.hpp"

#include "model/Game.hpp"
#include "model/User.hpp"
#include "model/Competition.hpp"
#include "model/CookieSession.hpp"
#include "config.hpp"
#include "TaskTracker.hpp"

namespace dbo = Wt::Dbo;
namespace c = thechess::config;

namespace thechess
{

ThechessSession::ThechessSession() :
#ifdef THECHESS_SQLITE
    connection_(c::sqliteDb)
#elif THECHESS_MYSQL
    connection_(c::mysql_db, c::mysql_user,
        c::mysql_password, c::mysql_host)
#elif THECHESS_POSTGRES
    connection_(c::postgres)
#endif
{
    using model::User;
    using model::Game;
    using model::Competition;
    using model::GamePtr;
    using model::CookieSession;

    setConnection(connection_);

#ifdef THECHESS_MYSQL
    connection_.executeSql("SET sql_mode='ANSI'");
    connection_.executeSql("SET storage_engine='InnoDB'");
    connection_.executeSql("set names 'utf8'");
    connection_.executeSql("set character set 'utf8'");
    connection_.executeSql("set character_set_server='utf8'");
    connection_.executeSql("set character_set_database='utf8'");
    connection_.executeSql("set collation_server='utf8_general_ci'");
    connection_.executeSql("set collation_database='utf8_general_ci'");
#endif

#ifdef THECHESS_SQLITE
    connection_.executeSql("PRAGMA cache_size = 3000");
    connection_.executeSql("PRAGMA synchronous = normal");
    connection_.executeSql("PRAGMA journal_mode = wal");
    connection_.executeSql("PRAGMA locking_mode = EXCLUSIVE");
#endif

    mapClass<User>("thechess_user");
    mapClass<Game>("thechess_game");
    mapClass<CookieSession>("thechess_cookie_session");
    mapClass<Competition>("thechess_competition");

}



}
