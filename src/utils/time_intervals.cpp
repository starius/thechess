/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include "utils/time_intervals.hpp"

namespace thechess {

static Wt::WDateTime time(Wt::WDateTime::currentDateTime());

std::string td2str(const Td& td) {
    return ptime::to_simple_string(ptime::seconds(td.total_seconds()));
}

Td operator -(const Wt::WDateTime& a, const Wt::WDateTime& b) {
    return a.toPosixTime() - b.toPosixTime();
}

Wt::WDateTime operator +(const Wt::WDateTime& a, const Td& b) {
    return Wt::WDateTime::fromPosixTime(a.toPosixTime() + b);
}

Wt::WDateTime operator -(const Wt::WDateTime& a, const Td& b) {
    return a + (-b);
}

Wt::WDateTime& operator +=(Wt::WDateTime& a, const Td& b) {
    return a = a + b;
}

Wt::WDateTime& operator -=(Wt::WDateTime& a, const Td& b) {
    return a = a - b;
}

Td operator /(const Td& a, const double& b) {
    return ptime::milliseconds(double(a.total_milliseconds()) /  b);
}

Td operator *(const Td& a, const double& b) {
    return ptime::milliseconds(double(a.total_milliseconds()) *  b);
}

Td operator *(const double& b, const Td& a) {
    return a * b;
}

double operator /(const Td& a, const Td& b) {
    return double(a.total_milliseconds()) / double(b.total_milliseconds());
}

Wt::WDateTime now() {
    return Wt::WDateTime::currentDateTime();
}

}

