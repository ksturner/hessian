// Copyright (C) 2004 Aldratech Ltd. See the LICENSE file for licensing information.
/*
 This file is part of hessiancpp.

    hessiancpp is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    hessiancpp is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with hessiancpp; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include "wrappers.h"

#if defined(__HP_aCC) && ! defined(_HP_NAMESPACE_STD)
#include <locale.h>
#else
#include <locale>
#endif

#define UTF8_LOC "en_US.utf8"

#define replacement_marker 0xfffd
#define word_joiner 0x2060
#define bom 0xfeef

using namespace std;

namespace hessian {
	namespace wrappers {
		int Object::_created = 0;
		int Object::_deleted = 0;
		int Object::_copied = 0;
	}

  namespace wrappers
  {
    wstring String::utf8_to_wstring(const string& str)
    {
      int len = str.size();
      wstring sb;
      sb.reserve(len);
      for (int indx = 0; indx < len; ++indx)
      {
        int ch1 = str[indx] & 0xff;
        if (ch1 < 0x80) // 1 byte
        {
          sb.append(1, (wchar_t) ch1);
          continue;
        }
        else if ((ch1 & 0xe0) == 0xc0) // 2 bytes
        {
          if (indx + 1 < len) // not out of bounds
          {
            int ch2 = str[++indx];
            wchar_t v =
              ((ch1 & 0x1f) << 6) +
              ((ch2 & 0x3f));
            // 2 byte sequence from 0x80 to 0x7ff
            if ((v >= 0x80) && ((ch2 & 0xc0) == 0x80))
            {
              sb.append(1, v);
              continue;
            }
          }
        }
        else if ((ch1 & 0xf0) == 0xe0) // 3 bytes
        {
          if (indx + 2 < len) // not out of bounds
          {
            int ch2 = str[++indx];
            if ((ch2 & 0xc0) == 0x80)
            {
              int ch3 = str[++indx];
              wchar_t v =
                ((ch1 & 0x0f) << 12) +
                ((ch2 & 0x3f) << 6) +
                ((ch3 & 0x3f));
              // 3 byte sequence from 0x800 to 0xfffd
              // d800-dfff not legal
              if ((v >= 0x800) && (v < 0xfffe) && ((ch3 & 0xc0) == 0x80))
              {
                if ((v < 0xd800) || (v > 0xdfff))
                {
                  sb.append(1, v);
                  continue;
                }
              }
            }
          }
        }
        else if ((ch1 & 0xf8) == 0xf0) // 4 bytes
        {
          if (indx + 3 < len) // not out of bounds
          {
            int ch2 = str[++indx];
            if ((ch2 & 0xc0) == 0x80)
            {
              int ch3 = str[++indx];
              if ((ch3 & 0xc0) == 0x80)
              {
                int ch4 = str[++indx];
                wchar_t v =
                  ((ch1 & 0x07) << 18) +
                  ((ch2 & 0x3f) << 12) +
                  ((ch3 & 0x3f) << 6) +
                  ((ch4 & 0x3f));
                // 4 byte sequence from 0x10000 to 0x10ffff
                if ((v >= 0x10000) && ((ch4 & 0xc0) == 0x80))
                {
                  sb.append(1, v);
                  continue;
                }
              }
            }
          }
        }
        // illegal utf-8 sequence found, substitute with replacement marker
        sb.append(1, (wchar_t) replacement_marker);
      }
      return sb;
    }

    wstring String::to_wstring(const string& str)
    {
      int len = str.size();
#if defined(__HP_aCC) && ! defined(_HP_NAMESPACE_STD)
      // pre-ansi C++ path
      wchar_t wch;
      const char* buf = str.c_str();
      wstring sb;
      sb.reserve(len);
      for (int indx = 0; indx < len; )
      {
        size_t frag_size = mbtowc(&wch, buf + indx, len - indx);
        if (frag_size == ((size_t) -1))
        {
          sb.append(1, (wchar_t) replacement_marker);
          ++indx;
        }
        else
        {
          sb.append(1, wch);
          indx += frag_size;
        }
      }
      return sb;
#else
      locale loc("");
      typedef mbstate_t state_type;
      typedef codecvt<wchar_t, char, state_type> codecvt_type;
      state_type mystate;
      const char* s_next;
      const char* s_head = str.data();
      const char* s_tail = s_head + len;
      wchar_t* w_next;
      wstring wstr(L" ", len);
      wchar_t* w_head = &(*wstr.begin());
      use_facet<codecvt_type>(loc).in(mystate,
        s_head, s_tail, s_next,
        w_head , &(*wstr.end()), w_next);
      int cvt_len = w_next - w_head;
      wstr.resize(cvt_len);
      return wstr;
#endif
    }

    static int more_cap(int cur)
    {
      int new_cap = cur + (cur / 2) + 16;
      if (new_cap < cur) new_cap = 0x7fffffff;
      return new_cap;
    }

    string String::to_utf8_string(const wstring& wstr)
    {
      int len = wstr.size();
      string sb;
      sb.reserve(len + 16);
      for (int indx = 0; indx < len; ++indx)
      {
        wchar_t wch = wstr[indx];
        if (wch >= 0xd800)
        {
          if (wch > 0x10ffff) wch = replacement_marker; // too large
          else if (wch <= 0xdfff) wch = replacement_marker; // bad surrogate
          else if ((wch == 0xfffe) || (wch == 0xffff)) wch=replacement_marker;
          else if (wch == bom) wch = word_joiner;
        }
        if (wch < 0x80)
          sb.append(1, (char) wch);
        else if (wch < 0x800) // 2 byte sequence
        {
          sb.append(1, (char) ((wch >> 6) | 0xc0));
          sb.append(1, (char) ((wch & 0x3f) | 0x80));
        }
        else if (wch < 0x10000) // 3 byte sequence
        {
          sb.append(1, (char) ((wch >> 12) | 0xe0));
          sb.append(1, (char) (((wch >> 6) & 0x3f) | 0x80));
          sb.append(1, (char) ((wch & 0x3f) | 0x80));
        }
        else // 4 byte sequence
        {
          sb.append(1, (char) ((wch >> 18) | 0xf0));
          sb.append(1, (char) (((wch >> 12) & 0x3f) | 0x80));
          sb.append(1, (char) (((wch >> 6) & 0x3f) | 0x80));
          sb.append(1, (char) ((wch & 0x3f) | 0x80));
        }
      }
      return sb;
    }

    string String::to_string(const wstring& wstr)
    {
      int wlen = wstr.size();
#if defined(__HP_aCC) && ! defined(_HP_NAMESPACE_STD)
      // pre-ansi C++ path
      char buf[16];
      string sb;
      sb.reserve(wlen+16);
      for (int indx = 0; indx < wlen; ++indx)
      {
        wchar_t wch = wstr[indx];
        size_t frag_len = wctomb(buf, wch);
        if (frag_len == ((size_t) -1))
          sb.append(1, '?'); // no translation available
        else
          sb.append(buf, frag_len);
      }
      return sb;
#else
      locale loc("");
      typedef mbstate_t state_type;
      typedef codecvt<wchar_t, char, state_type> codecvt_type;
      state_type mystate;
      const wchar_t* w_head = wstr.data();
      const wchar_t* w_work = w_head;
      const wchar_t* w_tail = w_head + wlen;
      const wchar_t* w_next;
      string str(" ", more_cap(wlen));
      char* s_head = &(*str.begin());
      char* s_work = s_head;
      char* s_next;
      for (;;)
      {
        codecvt_base::result res =
          std::use_facet<codecvt_type>(loc).out(mystate,
            w_work, w_tail, w_next,
            s_work, &(*str.end()), s_next);
        wlen = s_next - s_head;
        if (res == codecvt_base::partial)
        {
          str.resize(more_cap(str.size()));
          s_head = &(*str.begin());
          s_work = s_head + wlen;
          w_work = w_next;
        }
        else
        {
          str.resize(wlen);
          return str;
        }
      }
#endif
    }

    String::String(const string& utf8_str)
      : Object("String"), _value(utf8_to_wstring(utf8_str))
    {
    }

    String::basic_type String::value() const
    {
      return to_string(_value);
    }

    Fault::basic_type Fault::code() const
    {
      return String::to_utf8_string(_code);
    }

    Fault::basic_type Fault::message() const
    {
      return String::to_utf8_string(_message);
    }
  }
}

