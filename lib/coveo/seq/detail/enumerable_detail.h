// Copyright (c) 2016, Coveo Solutions Inc.
// Distributed under the MIT license (see LICENSE).

// Implementation details of coveo::seq::enumerable.

#ifndef COVEO_ENUMERABLE_DETAIL_H
#define COVEO_ENUMERABLE_DETAIL_H

#include <cstdint>
#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>

namespace coveo {
namespace detail {

// Type trait that can be used to know if std::begin(T) is valid.
// Detects both std::begin specializations and begin methods.
template<typename T>
class has_begin
{
    static_assert(sizeof(std::int_least8_t) != sizeof(std::int_least32_t),
                  "has_begin only works if int_least8_t has a different size than int_least32_t");

    template<typename C> static std::int_least8_t  test(decltype(std::begin(std::declval<C>()))*);  // Will be selected if std::begin(C) works
    template<typename C> static std::int_least32_t test(...);                                       // Will be selected otherwise
public:
    static constexpr bool value = sizeof(test<T>(nullptr)) == sizeof(std::int_least8_t);
};

// Type trait that can be used to know if std::end(T) is valid.
// Detects both std::end specializations and end methods.
template<typename T>
class has_end
{
    static_assert(sizeof(std::int_least8_t) != sizeof(std::int_least32_t),
                  "has_end only works if int_least8_t has a different size than int_least32_t");

    template<typename C> static std::int_least8_t  test(decltype(std::end(std::declval<C>()))*);    // Will be selected if std::end(C) works
    template<typename C> static std::int_least32_t test(...);                                       // Will be selected otherwise
public:
    static const bool value = sizeof(test<T>(nullptr)) == sizeof(std::int_least8_t);
};

// Type trait that can be used to know if a type has a size() const method
// that returns a non-void result.
template<typename T>
class has_size_const_method
{
    static_assert(sizeof(std::int_least8_t) != sizeof(std::int_least32_t),
                  "has_size_const_method only works if int_least8_t has a different size than int_least32_t");

    template<typename C> static std::int_least8_t  test(typename std::enable_if<!std::is_void<decltype(std::declval<const C>().size())>::value, void*>::type);  // Will be selected if C has size() that does not return void
    template<typename C> static std::int_least32_t test(...);                                                                                                   // Will be selected otherwise
public:
    static const bool value = sizeof(test<T>(nullptr)) == sizeof(std::int_least8_t);
};

// Returns a size delegate for a sequence if iterators can provide that information quickly.
template<typename It>
auto get_size_delegate_for_iterators(const It& beg, const It& end) -> typename std::enable_if<std::is_base_of<std::random_access_iterator_tag,
                                                                                                              typename std::iterator_traits<It>::iterator_category>::value,
                                                                                              std::function<std::size_t()>>::type
{
    return [beg, end]() -> std::size_t { return std::distance(beg, end); };
};
template<typename It>
auto get_size_delegate_for_iterators(const It&, const It&) -> typename std::enable_if<!std::is_base_of<std::random_access_iterator_tag,
                                                                                                       typename std::iterator_traits<It>::iterator_category>::value,
                                                                                      std::function<std::size_t()>>::type
{
    // No way to quickly determine size, don't try
    return nullptr;
};

} // detail
} // coveo

#endif // COVEO_ENUMERABLE_DETAIL_H
