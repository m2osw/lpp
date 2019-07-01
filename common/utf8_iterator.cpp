/*    utf8_iterator.cpp -- convert between wchar_t and UTF-8 encodings
 *    Copyright (C) 2000-2015  Made to Order Software Corporation
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *    Authors
 *    Alexis Wilke   alexis@m2osw.com
 */

/** \file
 * \brief Implementation of the UTF-8 functions.
 *
 * This file is the implementation of the UTF-8 functions of the libutf8
 * library. It simply is a set of functions to convert between different
 * character sets in a lossless manner. At this point it supports UTF-8,
 * UCS-4, and UTF-16 formats.
 *
 * Contrary to many of the system functions, these functions do not take
 * anything from the system in account (the locale can be anything, it does
 * not change the exact behavior of these functions.)
 *
 * Also similar functionality is found on Unices and MS-Windows, it was
 * simpler to just implement these few functions than to try to have a
 * converter that is sure not to use a locale and this way we can use
 * standard strings (std::string and std::wstring) instead of having to
 * call C functions.
 */

// self
//
#include "utf8_iterator.hpp"

// C++ lib
//
#include <iostream>



namespace lpp
{


namespace
{



/** \brief Convert one multi-byte character to a wide character.
 *
 * This function converts UTF-8 bytes from \p mb to one UTF-32
 * wide character and saves the result in \p wc. The function
 * automatically increases the pointer in \p mb and simultaneously
 * decreases the \p len parameter.
 *
 * \p wc holds the resulting wide character, a character between
 * `'\0'` (NUL) and `0x10FFFF` and it returns the number of bytes
 * that were used from \p mb. If a bad character is encountered,
 * then the function returns -1 and the bad sequence of bytes is
 * skipped so only one error will be reported for one bad sequence.
 *
 * Bad characters when converting UTF-8 to wide characters are:
 *
 * \li The stream includes bytes 0x80 to 0xBF without an introducer.
 * \li The stream does not include the right number of 0x80 to 0xBF
 *     bytes after an introducer.
 * \li The input ends too early and cannot accommodate the last
 *     encoded character.
 * \li The codes 0xF8 to 0xFF were found in the input string.
 * \li The resulting \p wc value would be larger than 0x10FFFF.
 * \li The resulting \p wc value represents a UTF-16 surrogate
 *     value (a number between 0xD800 and 0xDFFF).
 *
 * Code points between 0xD800 and 0xDFFF are not valid characters.
 * These represent low and high surrogates in UTF-16 (2 are
 * necessary to encode one character of 17 or more bits.)
 *
 * The function returns 0 and sets \p wc to the NUL character (`U'\0'`)
 * if the \p len parameter is zero (i.e. empty string.)
 *
 * \note
 * The function converts a NUL character (`'\0'`) in the
 * input string as a NUL wide character (`U'\0'`) and returns 1. It
 * does not see the NUL character as the end of the string.
 *
 * \warning
 * The function does not throw on invalid input. It is the responsibility
 * of the caller to do so if necessary. This is useful to very an UTF-8
 * string without having to catch an exception.
 *
 * \param[out] wc  The output wide character variable.
 * \param[in,out] mb  The multi-byte input string pointer, returned at the
 *                    following byte.
 * \param[in,out] len  The number of characters left in mb.
 *
 * \return The number of bytes read or -1 if invalid bytes were found.
 */
int mbstowc(char32_t & wc, char const * & mb, size_t & len)
{
    auto skip = [](char const * & skip_mb, size_t & skip_len)
    {
        for(unsigned char b(0)
            ; skip_len > 0 && (b = *skip_mb, (b >= 0x80 && b <= 0xBF) || b >= 0xF5)
            ; ++skip_mb , --skip_len);
    };

    // default output character is NUL
    //
    wc = U'\0';

    // already done?
    //
    if(len <= 0)
    {
        return 0;
    }

    // we eat one character from the source minimum
    //
    unsigned char c(*mb++);
    --len;

    if(c < 0x80)
    {
        wc = c;
        return 1;
    }

    // invalid stream?
    //
    if((c >= 0x80 && c <= 0xBF) || c >= 0xF5)
    {
        // this is bad UTF-8, skip all the invalid bytes
        //
        skip(mb, len);
        return -1;
    }

    char32_t w(U'\0');
    size_t cnt(0);

    if(c >= 0xF0)
    {
        w = c & 0x07;
        cnt = 3;
    }
    else if(c >= 0xE0)
    {
        w = c & 0x0F;
        cnt = 2;
    }
    else /*if(c >= 0xC0)*/    // always true so we don't have to check
    {
        w = c & 0x1F;
        cnt = 1;
    }

    // enough data in the input? if not, that's an error
    //
    if(len < cnt)
    {
        skip(mb, len);
        return -1;
    }
    len -= cnt;

    for(size_t l(cnt); l > 0; --l, mb++)
    {
        c = *mb;
        if(c < 0x80 || c > 0xBF)
        {
            // we got an invalid sequence!
            // restore whatever is left in len
            //
            len += l;
            return -1;
        }
        w = (w << 6) | (c & 0x3F);
    }

    if(w >= 0x110000
    || (w >= 0x00D800 && w <= 0x00DFFF))
    {
        // character out of range or UTF-16 surrogate
        // it can happen with sequences starting with 0xF7
        //
        return -1;
    }

    wc = w;

    return static_cast<int>(cnt + 1);
}


/** \brief Compute the UTF-8 encoded representation of wc.
 *
 * This function transforms the UTF-32 character \p wc in a
 * UTF-8 encoded series of bytes (called a multi-byte encoded
 * character.) The resulting string is null (`'\0'`) terminated.
 *
 * The \p mb buffer should be at least MBS_MIN_BUFFER_LENGTH bytes.
 * If less space is required, the function does not report a problem,
 * though. This allows to get the total size of a conversion and then
 * do the full conversion to that one buffer without the need to
 * add unnecessary bytes at the end of your destination buffer.
 *
 * \code
 * ...
 * char mb[MBS_MIN_BUFFER_LENGTH];
 *
 * wctombs(mb, big_char, sizeof(mb));
 * ...
 * \endcode
 *
 * The function does not encode invalid characters. When such is
 * passed to the function, the \p mb string is turned in a null
 * terminated string and the function returns 0. We avoid an
 * exception here because that way you can quickly check whether
 * a string of `char32_t` characters is valid or not.
 *
 * \note
 * Unicode defines valid characters only between zero (0) and 0x10FFFF.
 * Therefore this function encodes the character using 1 to 4 bytes plus
 * one for the null terminator.
 *
 * \warning
 * The function does not raise an error if the input \p wc character
 * is considered invalid (a UTF-16 surrogate or larger than 0x10FFFF.)
 * Instead it returns 0 and sets the \p mb string to the empty string.
 *
 * \exception libutf8_logic_exception
 * The function raises this exception if the destination buffer is too
 * small for the conversion. Don't forget that we add a null terminator
 * so if the character needs 3 UTF-8 bytes, we will check for a buffer
 * of at least 4 bytes to consider it valid.
 *
 * \param[out] mb  The output buffer, it will always be null terminated.
 * \param[in] wc  The wide character to convert.
 * \param[in] len  The length of \p mb.
 *
 * \return The number of bytes in mb, not including the null terminator.
 */
int wctombs(char * mb, char32_t wc, size_t len)
{
    auto verify_length = [&len](size_t required_len)
    {
        if(len < required_len)
        {
            throw std::logic_error("wctombs() called with an output buffer which is too small.");
        }
    };

    if(wc < 0x80)
    {
        verify_length(2);

        /* this will also encode '\0'... */
        mb[0] = static_cast<char>(wc);
        mb[1] = '\0';
        return 1;
    }
    if(wc < 0x800)
    {
        verify_length(3);

        mb[0] = static_cast<char>((wc >> 6) | 0xC0);
        mb[1] = (wc & 0x3F) | 0x80;
        mb[2] = '\0';
        return 2;
    }

    // avoid encoding the UTF-16 surrogate because those code points do not
    // represent characters
    //
    if(wc < 0xD800 || wc > 0xDFFF)
    {
        if(wc < 0x10000)
        {
            verify_length(4);

            mb[0] = static_cast<char>((wc >> 12) | 0xE0);
            mb[1] = ((wc >> 6) & 0x3F) | 0x80;
            mb[2] = (wc & 0x3F) | 0x80;
            mb[3] = '\0';
            return 3;
        }
        if(wc < 0x110000)
        {
            verify_length(5);

            mb[0] = static_cast<char>((wc >> 18) | 0xF0);
            mb[1] = ((wc >> 12) & 0x3F) | 0x80;
            mb[2] = ((wc >> 6) & 0x3F) | 0x80;
            mb[3] = (wc & 0x3F) | 0x80;
            mb[4] = '\0';
            return 4;
        }
    }

    verify_length(1);

    /* an invalid wide character */
    mb[0] = '\0';
    return -1;
}



} // no name namespace



utf8_iterator::utf8_iterator(std::string const & str, bool end)
    : f_str(str)
    , f_pos(end ? str.length() : 0)
{
}


utf8_iterator & utf8_iterator::operator ++ ()
{
    increment();
    return *this;
}


utf8_iterator utf8_iterator::operator ++ (int) // post-increment
{
    utf8_iterator it(*this);
    increment();
    return it;
}


utf8_iterator & utf8_iterator::operator -- ()
{
    decrement();
    return *this;
}


utf8_iterator utf8_iterator::operator -- (int) // post-decrement
{
    utf8_iterator it(*this);
    decrement();
    return it;
}


char32_t utf8_iterator::operator * () const
{
    if(f_pos >= f_str.length())
    {
        return EOF;
    }
    char const * s(f_str.c_str() + f_pos);
    char32_t wc(U'\0');
    size_t len(f_str.length() - f_pos);
    if(mbstowc(wc, s, len) < 0)
    {
        f_good = false;
    }
    return wc;
}


bool utf8_iterator::operator == (std::string::iterator it) const
{
    return static_cast<std::string::size_type>(it - f_str.begin()) == f_pos;
}


bool utf8_iterator::operator != (std::string::iterator it) const
{
    return static_cast<std::string::size_type>(it - f_str.begin()) != f_pos;
}


bool utf8_iterator::operator == (std::string::const_iterator it) const
{
    return static_cast<std::string::size_type>(it - f_str.cbegin()) == f_pos;
}


bool utf8_iterator::operator != (std::string::const_iterator it) const
{
    return static_cast<std::string::size_type>(it - f_str.cbegin()) != f_pos;
}


bool operator == (std::string::iterator it, utf8_iterator const & rhs)
{
    return static_cast<std::string::size_type>(it - rhs.f_str.begin()) == rhs.f_pos;
}


bool operator != (std::string::iterator it, utf8_iterator const & rhs)
{
    return static_cast<std::string::size_type>(it - rhs.f_str.begin()) != rhs.f_pos;
}


bool operator == (std::string::const_iterator it, utf8_iterator const & rhs)
{
    return static_cast<std::string::size_type>(it - rhs.f_str.cbegin()) == rhs.f_pos;
}


bool operator != (std::string::const_iterator it, utf8_iterator const & rhs)
{
    return static_cast<std::string::size_type>(it - rhs.f_str.cbegin()) != rhs.f_pos;
}


void utf8_iterator::increment()
{
    auto skip = [&]()
    {
        for(unsigned char b(0)
            ; f_pos < f_str.length()
                && (b = static_cast<unsigned char>(f_str[f_pos]),
                            (b >= 0x80 && b <= 0xBF) || b >= 0xF5)
            ; ++f_pos);
        f_good = false;
    };

    if(f_pos >= f_str.length())
    {
        return;
    }

    // increment is easy we can just get the current byte and we know
    // the size of the character in UTF-8
    //
    unsigned char c(static_cast<unsigned char>(f_str[f_pos]));

    if(c < 0x80)
    {
        ++f_pos;
    }
    else if(c <= 0xBF || c >= 0xF5)
    {
        // ?! invalid UTF-8 ?!
        //
        skip();
    }
    else if(c >= 0xF0)
    {
        f_pos += 4;
        if(c == 0xF4 && f_pos - 3 < f_str.length())
        {
            c = static_cast<unsigned char>(f_str[f_pos - 3]);
            if(c >= 0x90)
            {
                f_pos -= 3;
                skip();
            }
        }
    }
    else if(c >= 0xE0)
    {
        f_pos += 3;
    }
    else /*if(c >= 0xC0)*/    // always true so we don't have to check
    {
        f_pos += 2;
    }
    if(f_pos > f_str.length())
    {
        f_pos = f_str.length();
        f_good = false;
    }
}


void utf8_iterator::decrement()
{
    if(f_pos == 0)
    {
        return;
    }

    // decrement requires us to search for the previous starting byte
    // which means we need to scan the string
    //
    while(f_pos > 0)
    {
        --f_pos;
        unsigned char c(static_cast<unsigned char>(f_str[f_pos]));
        if(c < 0x80
        || c >= 0xC0)
        {
            break;
        }
    }
}


std::string::size_type utf8_iterator::operator - (std::string::const_iterator it) const
{
    return static_cast<std::string::size_type>(f_str.cbegin() + f_pos - it);
}


std::string::size_type operator - (std::string::const_iterator it, utf8_iterator const & rhs)
{
    return static_cast<std::string::size_type>(it - rhs.f_str.cbegin() - rhs.f_pos);
}


bool utf8_iterator::good() const
{
    return f_good;
}


bool utf8_iterator::bad() const
{
    return !f_good;
}


std::string wctombs(char32_t wc)
{
    char mb[8];
    int const sz(wctombs(mb, wc, sizeof(mb)));
    return std::string(mb, sz);
}



} // lpp namespace
// vim: ts=4 sw=4 et
