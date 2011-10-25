/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

/** \file
Time duration constanrs and integration with WDateTime.
*/

#ifndef THECHESS_TIME_INTERVALS_
#define THECHESS_TIME_INTERVALS_

#include <string>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <Wt/WDateTime>

namespace thechess {

namespace ptime = boost::posix_time;

/** Time duartion */
typedef ptime::time_duration Td;

/** Null time duration */
const Td TD_NULL = ptime::seconds(0);

/** One second */
const Td SECOND = ptime::seconds(1);

/** One minute */
const Td MINUTE = ptime::minutes(1);

/** One hour */
const Td HOUR = ptime::hours(1);

/** One day */
const Td DAY = HOUR * 24;

/** One week */
const Td WEEK = DAY * 7;

/** Convert time duration to string */
std::string td2str(const Td& td);

/** Return time duration between two datetimes */
Td operator -(const Wt::WDateTime& a, const Wt::WDateTime& b);

/** Increase the datetime by the time duration */
Wt::WDateTime operator +(const Wt::WDateTime& a, const Td& b);

/** Decrease the datetime by the time duration */
Wt::WDateTime operator -(const Wt::WDateTime& a, const Td& b);

/** Increase the datetime by the time duration */
Wt::WDateTime& operator +=(Wt::WDateTime& a, const Td& b);

/** Decrease the datetime by the time duration */
Wt::WDateTime& operator -=(Wt::WDateTime& a, const Td& b);

/** Divide the datetime */
Td operator /(const Td& a, const double& b);

/** Multiply the datetime */
Td operator *(const Td& a, const double& b);

/** Multiply the datetime */
Td operator *(const double& b, const Td& a);

/** Divide the datetime */
double operator /(const Td& a, const Td& b);

/** Current datetime */
Wt::WDateTime now();

}

#endif

