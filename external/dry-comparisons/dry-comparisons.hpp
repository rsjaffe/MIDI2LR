#ifndef DRY_COMPARISONS_HPP_
#define DRY_COMPARISONS_HPP_

#include <utility>
#include <type_traits>
#include <tuple>
#include <functional>
#include <iosfwd>

namespace rollbear {

namespace internal {

struct {
    struct A {
        [[maybe_unused]] constexpr bool operator==(int) const { return false; }
    };
    template <typename T>
    [[maybe_unused]] static constexpr auto func(T* t) -> decltype(0 == *t) {
        return true;
    }
    template <typename>
    [[maybe_unused]] static constexpr auto func(...) -> bool { return false;}
    [[maybe_unused]] constexpr operator bool() const { return func<A>(nullptr);}
} constexpr commutative_eq{};

template <bool>
struct eq_type
{
    template <typename A, typename B>
    using type = decltype(std::declval<const A&>() == std::declval<B&>());
};
template <>
struct eq_type<false> {};

template <bool b, typename A, typename B>
using eq_type_t = typename eq_type<!commutative_eq && b>::template type<A,B>;


template <typename, typename = void>
struct printable;
template <typename ... Ts>
struct printable<std::tuple<Ts...>, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<const Ts&>())...>>
{
    using type = void;
};
template <typename T>
using printable_t = typename printable<T>::type;

template <typename F, typename ... Args>
struct bound
{
    using RT = std::invoke_result_t<F, Args...>;

    template <typename F_, typename ... As>
    constexpr bound(F_&& f_, As&& ... as) : f(std::forward<F_>(f_)), args(std::forward<As>(as)...) {}

    constexpr operator RT() const
    noexcept(std::is_nothrow_invocable_v<const F&, const Args&...>)
    {
        return std::apply(f, args);
    }

    F f;
    std::tuple<Args...> args;
};

template <typename ... Ts>
struct binder
{
    std::tuple<Ts...> values;
    template <typename T, typename F>
    constexpr bound<T, Ts...> bind(const F& f) const
    {
        return std::apply([&](auto&& ... vs){return bound<T, Ts...>{f, vs...};}, values);
    }
};

template <typename ... Ts>
class logical_tuple : std::tuple<Ts...>
{
    using tuple = std::tuple<Ts...>;
    constexpr const tuple& self() const { return *this; }
protected:
    using tuple::tuple;
    template <typename F>
    constexpr auto or_all(F&& f) const
    {
        return std::apply([&](const auto& ... v) { return (f(v) || ...);}, self());
    }
    template <typename F>
    constexpr auto and_all(F&& f) const
    {
        return std::apply([&](const auto& ... v) { return (f(v) && ...);}, self());
    }
    template <typename RT, typename ... Args>
    constexpr RT bind(Args&& ... args) const {

        return std::apply([&](auto&&... f) {
            binder<Args...> b{{args...}};
            return RT{b.template bind<Ts>(std::forward<decltype(f)>(f))...}; }, self());
    }
    template <typename Char, typename Traits>
    std::basic_ostream<Char, Traits>& print(const Char* label, std::basic_ostream<Char, Traits>& os) const
    {
        os << label << '{';
        std::apply([&os](const auto& ... v) {
            int first = 1;
            ((os << &","[std::exchange(first, 0)] << v),...);
        }, self());
        return os << '}';
    }
};
}

template <typename ... T>
class any_of : internal::logical_tuple<T...>
{
    using internal::logical_tuple<T...>::or_all;
    using internal::logical_tuple<T...>::and_all;
public:
    using internal::logical_tuple<T...>::logical_tuple;

    template <typename U>
    constexpr auto operator==(const U& u) const
    noexcept(noexcept(((std::declval<const T&>() == u) || ...)))
    -> decltype(((std::declval<const T&>() == u) || ...))
    {
        return or_all([&](auto&& v) { return v == u;});
    }
#if !(defined(__cpp_impl_three_way_comparison) && __cpp_impl_three_way_comparison >= 201907)
    template <typename U>
    friend constexpr auto operator==(const U& u, const any_of& a)
    noexcept(noexcept(a == u))
    -> internal::eq_type_t<!std::is_same_v<U, any_of>, any_of, U>
    {
        return a == u;
    }
#endif
    template <typename U>
    constexpr auto operator!=(const U& u) const
    noexcept(noexcept(((std::declval<const T&>() != u) && ...)))
    -> decltype(((std::declval<const T&>() != u) && ...))
    {
        return and_all([&](auto v) { return v != u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, any_of>{}>>
    friend constexpr auto operator!=(const U& u, const any_of& a)
    noexcept(noexcept(a != u))
    -> decltype(a != u)
    {
        return a != u;
    }
    template <typename U>
    constexpr auto operator<(const U& u) const
    noexcept(noexcept(((std::declval<const T&>() < u) || ...)))
    -> decltype(((std::declval<const T&>() < u) || ...))
    {
        return or_all([&](auto&& v){ return v < u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, any_of>{}>>
    friend constexpr auto operator>(const U& u, const any_of& a)
    noexcept(noexcept(a < u))
    -> decltype(a < u)
    {
        return a < u;
    }
    template <typename U>
    constexpr auto operator<=(const U& u) const
    noexcept(noexcept(((std::declval<const T&>() <= u) || ...)))
    -> decltype(((std::declval<const T&>() <= u) || ...))
    {
        return or_all([&](auto&& v){ return v <= u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, any_of>{}>>
    friend constexpr auto operator>=(const U& u, const any_of& a)
    noexcept(noexcept(a <= u))
    -> decltype(a <= u)
    {
        return a <= u;
    }
    template <typename U>
    constexpr auto operator>(const U& u) const
    noexcept(noexcept(((std::declval<const T&>() > u) || ...)))
    -> decltype(((std::declval<const T&>() > u) || ...))
    {
        return or_all([&](auto&& v) { return v > u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, any_of>{}>>
    friend constexpr auto operator<(const U& u, const any_of& a)
    noexcept(noexcept(a > u))
    -> decltype(a > u)
    {
        return a > u;
    }
    template <typename U>
    constexpr auto operator>=(const U& u) const
    noexcept(noexcept(((std::declval<const T&>() >= u) || ...)))
    -> decltype(((std::declval<const T&>() >= u) || ...))
    {
        return or_all([&](auto&& v) { return v >= u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, any_of>{}>>
    friend constexpr auto operator<=(const U& u, const any_of& a)
    noexcept(noexcept(a >= u))
    -> decltype(a >= u)
    {
        return a >= u;
    }
    template <typename V = std::tuple<T...>, typename = internal::printable_t<V>>
    friend std::ostream& operator<<(std::ostream& os, const any_of& self)
    {
        return self.print("any_of", os);
    }
    constexpr explicit operator bool() const
    noexcept(noexcept((std::declval<const T&>() || ...)))
    {
        return or_all([](auto&& v) { return v;});
    }
    template <typename ... Ts>
    constexpr auto operator()(Ts&& ... ts) const
    noexcept(
    std::conjunction_v<std::is_nothrow_move_constructible<T>...> &&
    std::conjunction_v<std::is_nothrow_copy_constructible<Ts>...>)
    -> std::enable_if_t<std::conjunction_v<std::is_copy_constructible<Ts>...>&&
                        std::conjunction_v<std::is_invocable<T, Ts...>...>,
            any_of<internal::bound<T, Ts...>...>>
    {
        using RT = any_of<internal::bound<T, Ts...>...>;
        return this->template bind<RT>(std::forward<Ts>(ts)...);
    }
};

template <typename ... T>
class none_of : internal::logical_tuple<T...>
{
    using internal::logical_tuple<T...>::or_all;
    using internal::logical_tuple<T...>::and_all;
public:
    using internal::logical_tuple<T...>::logical_tuple;
    template <typename U>
    constexpr auto operator==(const U& u) const
    noexcept(noexcept(!((std::declval<const T&>() == u) || ...)))
    -> decltype(!((std::declval<const T&>() == u) || ...))
    {
        return !or_all([&](auto&& v) { return v == u;});
    }
#if !(defined(__cpp_impl_three_way_comparison) && __cpp_impl_three_way_comparison >= 201907)
    template <typename U>
    friend constexpr auto operator==(const U& u, const none_of& a)
    noexcept(noexcept(a == u))
    -> internal::eq_type_t<!std::is_same<U, none_of>{}, none_of, U>
    {
        return a == u;
    }
#endif
    template <typename U>
    constexpr auto operator!=(const U& u) const
    noexcept(noexcept(!((std::declval<const T&>() != u) && ...)))
    -> decltype(!((std::declval<const T&>() != u) && ...))
    {
        return !and_all([&](auto && v){return v != u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, none_of>{}>>
    friend constexpr auto operator!=(const U& u, const none_of& a)
    noexcept(noexcept(a != u))
    -> decltype(a != u)
    {
        return a != u;
    }
    template <typename U>
    constexpr auto operator<(const U& u) const
    noexcept(noexcept(!((std::declval<const T&>() < u) || ...)))
    -> decltype(!((std::declval<const T&>() < u) || ...))
    {
        return !or_all([&](auto&& v){ return v < u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, none_of>{}>>
    friend constexpr auto operator>(const U& u, const none_of& a)
    noexcept(noexcept(a < u))
    -> decltype(a < u)
    {
        return a < u;
    }
    template <typename U>
    constexpr auto operator<=(const U& u) const
    noexcept(noexcept(!((std::declval<const T&>() <= u) || ...)))
    -> decltype(!((std::declval<const T&>() <= u) || ...))
    {
        return !or_all([&](auto&& v){ return v <= u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, none_of>{}>>
    friend constexpr auto operator>=(const U& u, const none_of& a)
    noexcept(noexcept(a <= u))
    -> decltype(a <= u)
    {
        return a <= u;
    }
    template <typename U>
    constexpr auto operator>(const U& u) const
    noexcept(noexcept(!((std::declval<const T&>() > u) || ...)))
    -> decltype(!((std::declval<const T&>() > u) || ...))
    {
        return !or_all([&](auto&& v) { return v > u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, none_of>{}>>
    friend constexpr auto operator<(const U& u, const none_of& a)
    noexcept(noexcept(a > u))
    -> decltype(a > u)
    {
        return a > u;
    }
    template <typename U>
    constexpr auto operator>=(const U& u) const
    noexcept(noexcept(!((std::declval<const T&>() >= u) || ...)))
    -> decltype(!((std::declval<const T&>() >= u) || ...))
    {
        return !or_all([&](auto&& v){ return v >= u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, none_of>{}>>
    friend constexpr auto operator<=(const U& u, const none_of& a)
    noexcept(noexcept(a >= u))
    -> decltype(a >= u)
    {
        return a >= u;
    }
    template <typename V = std::tuple<T...>, typename = internal::printable_t<V>>
    friend std::ostream& operator<<(std::ostream& os, const none_of& self)
    {
        return self.print("none_of", os);
    }
    constexpr explicit operator bool() const
    noexcept(noexcept(!(std::declval<const T&>() || ...)))
    {
        return !or_all([](auto&& v) { return v;});
    }

    template <typename ... Ts>
    constexpr auto operator()(Ts&& ... ts) const
    noexcept(
            std::conjunction_v<std::is_nothrow_move_constructible<T>...> &&
            std::conjunction_v<std::is_nothrow_copy_constructible<Ts>...>)
    -> std::enable_if_t<std::conjunction_v<std::is_copy_constructible<Ts>...>&&
                        std::conjunction_v<std::is_invocable<T, Ts...>...>,
            none_of<internal::bound<T, Ts...>...>>
    {
        using RT = none_of<internal::bound<T, Ts...>...>;
        return this->template bind<RT>(std::forward<Ts>(ts)...);
    }
};

template <typename ... T>
class all_of : internal::logical_tuple<T...>
{
    using internal::logical_tuple<T...>::or_all;
    using internal::logical_tuple<T...>::and_all;
public:
    using internal::logical_tuple<T...>::logical_tuple;

    template <typename U>
    constexpr auto operator==(const U& u) const
    noexcept(noexcept(((std::declval<const T&>() == u) && ...)))
    -> decltype(((std::declval<const T&>() == u) && ...))
    {
        return and_all([&](auto&& v){ return v == u;});
    }
#if !(defined(__cpp_impl_three_way_comparison) && __cpp_impl_three_way_comparison >= 201907)
    template <typename U>
    friend constexpr auto operator==(const U& u, const all_of& a)
    noexcept(noexcept(a == u))
    -> internal::eq_type_t<!std::is_same<U, all_of>{}, all_of, U>
    {
        return a == u;
    }
#endif
    template <typename U>
    constexpr auto operator!=(const U& u) const
    noexcept(noexcept(((std::declval<const T&>() != u) || ...)))
    -> decltype(((std::declval<const T&>() != u) || ...))
    {
        return or_all([&](auto&& v){return v != u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, all_of>{}>>
    friend constexpr auto operator!=(const U& u, const all_of& a)
    noexcept(noexcept(a != u))
    -> decltype(a != u)
    {
        return a != u;
    }
    template <typename U>
    constexpr auto operator<(const U& u) const
    noexcept(noexcept(((std::declval<const T&>() < u) && ...)))
    -> decltype(((std::declval<const T&>() < u) && ...))
    {
        return and_all([&](auto&& v){ return v < u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, all_of>{}>>
    friend constexpr auto operator>(const U& u, const all_of& a)
    noexcept(noexcept(a < u))
    -> decltype(a < u)
    {
        return a < u;
    }
    template <typename U>
    constexpr auto operator<=(const U& u) const
    noexcept(noexcept(((std::declval<const T&>() <= u) && ...)))
    -> decltype(((std::declval<const T&>() <= u) && ...))
    {
        return and_all([&](auto&& v){ return v <= u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, all_of>{}>>
    friend constexpr auto operator>=(const U& u, const all_of& a)
    noexcept(noexcept(a <= u))
    -> decltype(a <= u)
    {
        return a <= u;
    }
    template <typename U>
    constexpr auto operator>(const U& u) const
    noexcept(noexcept(((std::declval<const T&>() > u) && ...)))
    -> decltype(((std::declval<const T&>() > u) && ...))
    {
        return and_all([&](auto&& v){ return v > u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, all_of>{}>>
    friend constexpr auto operator<(const U& u, const all_of& a)
    noexcept(noexcept(a > u))
    -> decltype(a > u)
    {
        return a > u;
    }
    template <typename U>
    constexpr auto operator>=(const U& u) const
    noexcept(noexcept(((std::declval<const T&>() >= u) && ...)))
    -> decltype(((std::declval<const T&>() >= u) && ...))
    {
        return and_all([&](auto&& v){ return v >= u;});
    }
    template <typename U, typename = std::enable_if_t<!std::is_same<U, all_of>{}>>
    friend constexpr auto operator<=(const U& u, const all_of& a)
    noexcept(noexcept(a >= u))
    -> decltype(a >= u)
    {
        return a >= u;
    }
    template <typename V = std::tuple<T...>, typename = internal::printable_t<V>>
    friend std::ostream& operator<<(std::ostream& os, const all_of& self)
    {
        return self.print("all_of", os);
    }
    constexpr explicit operator bool() const
    noexcept(noexcept((std::declval<const T&>() && ...)))
    {
        return and_all([](auto&& v) -> bool { return v;});
    }
    template <typename ... Ts>
    constexpr auto operator()(Ts&& ... ts) const
    noexcept(
            std::conjunction_v<std::is_nothrow_move_constructible<T>...> &&
            std::conjunction_v<std::is_nothrow_copy_constructible<Ts>...>)
    -> std::enable_if_t<std::conjunction_v<std::is_copy_constructible<Ts>...>&&
                        std::conjunction_v<std::is_invocable<T, Ts...>...>,
            all_of<internal::bound<T, Ts...>...>>
    {
        using RT = all_of<internal::bound<T, Ts...>...>;
        return this->template bind<RT>(std::forward<Ts>(ts)...);
    }
};


template <typename ... T>
any_of(T&& ...) -> any_of<T...>;
template <typename ... T>
none_of(T&& ...) -> none_of<T...>;
template <typename ... T>
all_of(T&& ...) -> all_of<T...>;

}

#endif
