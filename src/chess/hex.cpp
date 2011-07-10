/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */


#include <string>
#include <iostream>

typedef unsigned char byte;

inline byte hex2char(byte hex)
{
    return hex>0x60 ? hex-0x51 : (hex>0x40 ? hex-0x31 : hex - 0x30);
}

inline char char2hex(byte c)
{
    return c >= 0x0A ? c+(0x41-0x0A) : c+0x30;
}

std::string from_hex(const std::string hex)
{
    int l = hex.size() / 2;
    std::string r(l, '\0');
    for (int i = 0; i < l; i++)
    {
        r[i] = (hex2char(hex[i*2]) << 4) | (hex2char(hex[i*2 + 1]));
    }
    return r;
}

std::string to_hex(const std::string str)
{
    std::string r(str.size()*2, '\0');
    for (unsigned int i = 0; i < str.size(); i++)
    {
        r[i*2] = char2hex((byte)(str[i]) >> 4);
        r[i*2 + 1] = char2hex((byte)(str[i]) & 0x0F);
    }
    return r;
}
