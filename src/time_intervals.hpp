/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_TIME_INTERVALS_
#define THECHESS_TIME_INTERVALS_

#include <string>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <Wt/WDateTime>

namespace thechess {

namespace ptime = boost::posix_time;

typedef ptime::time_duration Td;

const Td td_null = ptime::seconds(0);
const Td second = ptime::seconds(1);
const Td minute = ptime::minutes(1);
const Td hour = ptime::hours(1);
const Td day = hour * 24;
const Td week = day * 7;

std::string td2str(const Td& td);
Td operator -(const Wt::WDateTime& a, const Wt::WDateTime& b);
Wt::WDateTime operator +(const Wt::WDateTime& a, const Td& b);
Wt::WDateTime operator -(const Wt::WDateTime& a, const Td& b);
Wt::WDateTime& operator +=(Wt::WDateTime& a, const Td& b);
Wt::WDateTime& operator -=(Wt::WDateTime& a, const Td& b);
Td operator /(const Td& a, const double& b);
Td operator *(const Td& a, const double& b);
Td operator *(const double& b, const Td& a);
double operator /(const Td& a, const Td& b);
Wt::WDateTime now();

}

#endif
