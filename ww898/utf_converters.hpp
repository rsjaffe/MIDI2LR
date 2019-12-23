/*
 * MIT License
 * 
 * Copyright (c) 2017-2019 Mikhail Pilin
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *  
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *  
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <ww898/utf_selector.hpp>
#include <ww898/utf_config.hpp>

#include <cstdint>
#include <iterator>
#include <string>

#if __cpp_lib_string_view >= 201606
#include <string_view>
#endif

namespace ww898 {
namespace utf {

namespace detail {

enum struct convz_impl { normal, binary_copy };

template<
    typename Utf,
    typename Outf,
    typename It,
    typename Oit,
    convz_impl>
struct convz_strategy
{
    Oit operator()(It it, Oit oit) const
    {
        auto const read_fn = [&it] { return *it++; };
        auto const write_fn = [&oit] (typename Outf::char_type const ch) { *oit++ = ch; };
        while (true)
        {
            auto const cp = Utf::read(read_fn);
            if (!cp)
                return oit;
            Outf::write(cp, write_fn);
        }
    }
};

template<
    typename Utf,
    typename Outf,
    typename It,
    typename Oit>
struct convz_strategy<Utf, Outf, It, Oit, convz_impl::binary_copy>
{
    Oit operator()(It it, Oit oit) const
    {
        while (true)
        {
            auto const ch = *it++;
            if (!ch)
                return oit;
            *oit++ = ch;
        }
    }
};

}

template<
    typename Utf,
    typename Outf,
    typename It,
    typename Oit>
Oit convz(It && it, Oit && oit)
{
    return detail::convz_strategy<Utf, Outf,
            typename std::decay<It>::type,
            typename std::decay<Oit>::type,
            std::is_same<Utf, Outf>::value
                ? detail::convz_impl::binary_copy
                : detail::convz_impl::normal>()(
        std::forward<It>(it),
        std::forward<Oit>(oit));
}

namespace detail {

enum struct conv_impl { normal, random_interator, binary_copy };

template<
    typename Utf,
    typename Outf,
    typename It,
    typename Oit,
    conv_impl>
struct conv_strategy final
{
    Oit operator()(It it, It const eit, Oit oit) const
    {
        auto const read_fn = [&it, &eit]
            {
                if (it == eit)
                    throw std::runtime_error("Not enough input");
                return *it++;
            };
        auto const write_fn = [&oit] (typename Outf::char_type const ch) { *oit++ = ch; };
        while (it != eit)
            Outf::write(Utf::read(read_fn), write_fn);
        return oit;
    }
};

template<
    typename Utf,
    typename Outf,
    typename It,
    typename Oit>
struct conv_strategy<Utf, Outf, It, Oit, conv_impl::random_interator> final
{
    Oit operator()(It it, It const eit, Oit oit) const
    {
        auto const write_fn = [&oit] (typename Outf::char_type const ch) { *oit++ = ch; };
        if (eit - it >= static_cast<typename std::iterator_traits<It>::difference_type>(Utf::max_supported_symbol_size))
        {
            auto const fast_read_fn = [&it] { return *it++; };
            auto const fast_eit = eit - Utf::max_supported_symbol_size;
            while (it < fast_eit)
                Outf::write(Utf::read(fast_read_fn), write_fn);
        }
        auto const read_fn = [&it, &eit]
            {
                if (it == eit)
                    throw std::runtime_error("Not enough input");
                return *it++;
            };
        while (it != eit)
            Outf::write(Utf::read(read_fn), write_fn);
        return oit;
    }
};

template<
    typename Utf,
    typename Outf,
    typename It,
    typename Oit>
struct conv_strategy<Utf, Outf, It, Oit, conv_impl::binary_copy> final
{
    Oit operator()(It it, It const eit, Oit oit) const
    {
        while (it != eit)
            *oit++ = *it++;
        return oit;
    }
};

}

template<
    typename Utf,
    typename Outf,
    typename It,
    typename Eit,
    typename Oit>
Oit conv(It && it, Eit && eit, Oit && oit)
{
    return detail::conv_strategy<Utf, Outf,
            typename std::decay<It>::type,
            typename std::decay<Oit>::type,
            std::is_same<Utf, Outf>::value
                ? detail::conv_impl::binary_copy
                : std::is_base_of<std::random_access_iterator_tag, typename std::iterator_traits<typename std::decay<It>::type>::iterator_category>::value
                    ? detail::conv_impl::random_interator
                    : detail::conv_impl::normal>()(
        std::forward<It>(it),
        std::forward<Eit>(eit),
        std::forward<Oit>(oit));
}

template<
    typename Outf,
    typename Ch,
    typename Oit>
Oit convz(Ch const * const str, Oit && oit)
{
    return convz<utf_selector_t<Ch>, Outf>(str, std::forward<Oit>(oit));
}

template<
    typename Och,
    typename Str>
std::basic_string<Och> convz(Str && str)
{
    std::basic_string<Och> res;
    convz<utf_selector_t<Och>>(std::forward<Str>(str), std::back_inserter(res));
    return res;
}

template<
    typename Outf,
    typename Ch,
    typename Oit>
Oit conv(std::basic_string<Ch> const & str, Oit && oit)
{
    return conv<utf_selector_t<Ch>, Outf>(str.cbegin(), str.cend(), std::forward<Oit>(oit));
}

#if __cpp_lib_string_view >= 201606
template<
    typename Outf,
    typename Ch,
    typename Oit>
Oit conv(std::basic_string_view<Ch> const & str, Oit && oit)
{
    return conv<utf_selector_t<Ch>, Outf>(str.cbegin(), str.cend(), std::forward<Oit>(oit));
}
#endif

template<
    typename Och,
    typename Str,
    typename std::enable_if<!std::is_same<typename std::decay<Str>::type, std::basic_string<Och>>::value, void *>::type = nullptr>
std::basic_string<Och> conv(Str && str)
{
    std::basic_string<Och> res;
    conv<utf_selector_t<Och>>(std::forward<Str>(str), std::back_inserter(res));
    return res;
}

template<
    typename Ch>
std::basic_string<Ch> conv(std::basic_string<Ch> str) throw()
{
    return str;
}

}}
