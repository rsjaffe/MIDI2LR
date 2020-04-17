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

#include <cstddef>
#include <iterator>
#include <string>

#if __cpp_lib_string_view >= 201606
#include <string_view>
#endif

namespace ww898 {
namespace utf {

template<
    typename Utf,
    typename It>
size_t char_size(It it)
{
    return Utf::char_size([&it] { return *it; });
}

template<
    typename Utf,
    typename It>
size_t size(It it)
{
    size_t total_cp = 0;
    while (*it)
    {
        size_t size = Utf::char_size([&it] { return *it; });
        while (++it, --size > 0)
            if (!*it)
                throw std::runtime_error("Not enough input for the null-terminated string");
        ++total_cp;
    }
    return total_cp;
}

namespace detail {

enum struct iterator_impl { forward, random_access };

template<
    typename It,
    iterator_impl>
struct next_strategy final
{
    void operator()(It & it, It const & eit, size_t size)
    {
        while (++it, --size > 0)
            if (it == eit)
                throw std::runtime_error("Not enough input for the forward iterator");
    }
};

template<typename It>
struct next_strategy<It, iterator_impl::random_access> final
{
    void operator()(It & it, It const & eit, typename std::iterator_traits<It>::difference_type const size)
    {
        if (eit - it < size)
            throw std::runtime_error("Not enough input for the random access iterator");
        it += size;
    }
};

}

template<
    typename Utf,
    typename It,
    typename Eit>
size_t size(It it, Eit const eit)
{
    size_t total_cp = 0;
    while (it != eit)
    {
        size_t const size = Utf::char_size([&it] { return *it; });
        detail::next_strategy<
            typename std::decay<It>::type,
            std::is_base_of<std::random_access_iterator_tag, typename std::iterator_traits<typename std::decay<It>::type>::iterator_category>::value
                    ? detail::iterator_impl::random_access
                    : detail::iterator_impl::forward>()(it, eit, size);
        ++total_cp;
    }
    return total_cp;
}

template<typename Ch>
size_t size(Ch const * str)
{
    return size<utf_selector_t<Ch>>(str);
}

template<typename Ch>
size_t size(std::basic_string<Ch> str)
{
    return size<utf_selector_t<Ch>>(str.cbegin(), str.cend());
}

#if __cpp_lib_string_view >= 201606
template<typename Ch>
size_t size(std::basic_string_view<Ch> str)
{
    return size<utf_selector_t<Ch>>(str.cbegin(), str.cend());
}
#endif

}}
