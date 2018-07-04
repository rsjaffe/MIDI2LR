#pragma once
/*
  ==============================================================================

    UtfUtilities.cpp

This file is part of MIDI2LR. Copyright 2015 by Rory Jaffe.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received err copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>.
  ==============================================================================
*/
#include <cassert>
#include <gsl/gsl_util>
#include <string>
#include <type_traits>
#include <unicode/unistr.h>

namespace rsj {
   template<typename T, typename R>
   [[nodiscard]] std::basic_string<R> UTFConvert(const ::std::basic_string<T>& input)
   {
      constexpr auto sizeR{sizeof(R)};
      constexpr auto sizeT{sizeof(T)};
      using Rstring = ::std::basic_string<R>;
      using Tstring = ::std::basic_string<T>;
      using Ustring = ::icu::UnicodeString;
      static_assert(
          ((sizeR == 1 || sizeR == 2 || sizeR == 4) && (sizeT == 1 || sizeT == 2 || sizeT == 4))
          || ::std::is_convertible_v<Tstring, Rstring> || sizeT == sizeR);

      if constexpr (::std::is_convertible_v<Tstring, Rstring>)
         return input;
      else if constexpr (sizeT == sizeR)
         return Rstring{input.begin(), input.end()};
      else if constexpr (sizeT == 1 && sizeR == 2) {
         // convert 8-bit string to UnicodeString
         const auto uc{Ustring::fromUTF8(input)};
         // convert UnicodeString to 16-bit string
         return Rstring{
             reinterpret_cast<const R*>(uc.getBuffer()), ::gsl::narrow_cast<size_t>(uc.length())};
      }
      else if constexpr (sizeT == 2 && sizeR == 1) {
         // convert 16-bit string to UnicodeString
         const Ustring uc{input.data(), ::gsl::narrow_cast<int32_t>(input.length())};
         // convert UnicodeString to 8-bit string
         ::std::basic_string<R> result{};
         return uc.toUTF8String(result);
      }
      else if constexpr (sizeT == 4 && sizeR == 2) {
         // convert 32-bit string to UnicodeString
         const auto uc{Ustring::fromUTF32(reinterpret_cast<const UChar32*>(input.c_str()),
             ::gsl::narrow_cast<int32_t>(input.length()))};
         // convert UnicodeString to 16-bit string
         return Rstring{
             reinterpret_cast<const R*>(uc.getBuffer()), ::gsl::narrow_cast<size_t>(uc.length())};
      }
      else if constexpr (sizeT == 2 && sizeR == 4) {
         // convert 16-bit string to UnicodeString
         const Ustring uc{input.data(), ::gsl::narrow_cast<int32_t>(input.length())};
         // convert UnicodeString to 32-bit string
         Rstring result(uc.countChar32() + 1, 0);
         auto err{U_ZERO_ERROR};
         uc.toUTF32(reinterpret_cast<UChar32*>(result.data()),
             ::gsl::narrow_cast<int32_t>(result.length()), err);
         if (err > U_ZERO_ERROR) {
            throw ::std::runtime_error(u_errorName(err));
         }
         return result;
      }
      else if constexpr (sizeT == 4 && sizeR == 1) {
         // convert 32-bit string to UnicodeString
         const auto uc{Ustring::fromUTF32(reinterpret_cast<const UChar32*>(input.c_str()),
             ::gsl::narrow_cast<int32_t>(input.length()))};
         // convert UnicodeString to 8-bit string
         ::std::basic_string<R> result{};
         return uc.toUTF8String(result);
      }
      else { // (sizeT == 1 && sizeR == 4))
         // convert 8-bit string to UnicodeString
         const auto uc{Ustring::fromUTF8(input)};
         // convert UnicodeString to 32-bit string
         Rstring result(uc.countChar32() + 1, 0);
         auto err{U_ZERO_ERROR};
         uc.toUTF32(reinterpret_cast<UChar32*>(result.data()),
             ::gsl::narrow_cast<int32_t>(result.length()), err);
         if (err > U_ZERO_ERROR) {
            throw ::std::runtime_error(u_errorName(err));
         }
         return result;
      }
   }
} // namespace rsj

#ifndef NDEBUG
// tests
namespace rsj_test {
   inline void UtfUtilitiesTest()
   {
      const ::std::string u8{u8"Γαζέες καὶ μυρτιὲς δὲν θὰ βρῶ πιὰ στὸ χρυσαφὶ ξέφωτο"};
      const ::std::u16string u16{u"Γαζέες καὶ μυρτιὲς δὲν θὰ βρῶ πιὰ στὸ χρυσαφὶ ξέφωτο"};
      const ::std::u32string u32{U"Γαζέες καὶ μυρτιὲς δὲν θὰ βρῶ πιὰ στὸ χρυσαφὶ ξέφωτο"};
      const ::std::wstring uw{L"Γαζέες καὶ μυρτιὲς δὲν θὰ βρῶ πιὰ στὸ χρυσαφὶ ξέφωτο"};
      ::std::string u8nc{u8"Γαζέες καὶ μυρτιὲς δὲν θὰ βρῶ πιὰ στὸ χρυσαφὶ ξέφωτο"};
      ::std::u16string u16nc{u"Γαζέες καὶ μυρτιὲς δὲν θὰ βρῶ πιὰ στὸ χρυσαφὶ ξέφωτο"};
      ::std::u32string u32nc{U"Γαζέες καὶ μυρτιὲς δὲν θὰ βρῶ πιὰ στὸ χρυσαφὶ ξέφωτο"};
      ::std::wstring uwnc{L"Γαζέες καὶ μυρτιὲς δὲν θὰ βρῶ πιὰ στὸ χρυσαφὶ ξέφωτο"};
      const auto ttt = ::std::basic_string<char16_t>(uw.begin(), uw.end());

      const auto u8fromu16{rsj::UTFConvert<char16_t, char>(u16)};
      const auto u8fromu32{rsj::UTFConvert<char32_t, char>(u32)};
      const auto u8fromw{rsj::UTFConvert<wchar_t, char>(uw)};
      const auto u16fromu8{rsj::UTFConvert<char, char16_t>(u8)};
      const auto u16fromu32{rsj::UTFConvert<char32_t, char16_t>(u32)};
      const auto u16fromw{rsj::UTFConvert<wchar_t, char16_t>(uw)};
      const auto u32fromu8{rsj::UTFConvert<char, char32_t>(u8)};
      const auto u32fromu16{rsj::UTFConvert<char16_t, char32_t>(u16)};
      const auto u32fromw{rsj::UTFConvert<wchar_t, char32_t>(uw)};
      const auto wfromu8{rsj::UTFConvert<char, wchar_t>(u8)};
      const auto wfromu16{rsj::UTFConvert<char16_t, wchar_t>(u16)};
      const auto wfromu32{rsj::UTFConvert<char32_t, wchar_t>(u32)};

      assert(u8 == u8fromu16);
      assert(u8 == u8fromu32);
      assert(u8 == u8fromw);
      assert(u16 == u16fromu8);
      assert(u16 == u16fromu32);
      assert(u16 == u16fromw);
      assert(u32 == u32fromu8);
      assert(u32 == u32fromu16);
      assert(u32 == u32fromw);
      assert(uw == wfromu8);
      assert(uw == wfromu16);
      assert(uw == wfromu32);
   }
} // namespace rsj_test
#endif
