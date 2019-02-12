/**
 * @file
 * @brief Implementation details of coveo::enumerable.
 *
 * This file contains implementation details used by coveo::enumerable.
 * It should not be necessary to use this file directly when using the class.
 * Code in the <tt>coveo::detail</tt> namespace is subject to change
 * in-between versions.
 *
 * @copyright 2016-2019, Coveo Solutions Inc.
 *            Distributed under the Apache License, Version 2.0 (see <a href="https://github.com/coveo/linq/blob/master/LICENSE">LICENSE</a>).
 */

#ifndef COVEO_ENUMERABLE_DETAIL_H
#define COVEO_ENUMERABLE_DETAIL_H

#include <cstdint>
#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>

namespace coveo {
namespace detail {

/**
 * @internal
 * @brief Type trait that detects @c begin.
 * @headerfile enumerable_detail.h <coveo/seq/detail/enumerable_detail.h>
 *
 * Type trait that can be used to know if <tt>std::begin(T)</tt> is valid.
 * Detects both <tt>std::begin()</tt> specializations and <tt>begin()</tt> methods.
 *
 * @tparam Type for which we need @c begin.
 */
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

/**
 * @internal
 * @brief Type trait that detects @c end.
 * @headerfile enumerable_detail.h <coveo/seq/detail/enumerable_detail.h>
 *
 * Type trait that can be used to know if <tt>std::end(T)</tt> is valid.
 * Detects both <tt>std::end()</tt> specializations and <tt>end()</tt> methods.
 *
 * @tparam Type for which we need @c end.
 */
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

/**
 * @internal
 * @brief Type trait that detects @c size.
 * @headerfile enumerable_detail.h <coveo/seq/detail/enumerable_detail.h>
 *
 * Type trait that can be used to know if type @c T has a <tt>size()</tt>
 * @c const method that returns a non-<tt>void</tt> result.
 *
 * @tparam Type for which we need @c size.
 */
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

/// @cond

/**
 * @internal
 * @brief Helper function to get a fast size delegate from iterators if possible.
 * @headerfile enumerable_detail.h <coveo/seq/detail/enumerable_detail.h>
 *
 * Attempts to create a <tt>coveo::enumerable::size_delegate</tt> that can
 * quickly calculate the number of elements in range <tt>[beg, end[</tt>.
 * A size delegate is returned only if it's possible to calculate the number
 * of elements "quickly".
 *
 * - If @c beg and @c end are random-access iterators,
 *   a size delegate is produced by using <tt>std::distance()</tt>.
 * - Otherwise, no size delegate is produced.
 *
 * @param beg Iterator pointing at beginning of range.
 * @param end Iterator pointing at end of range.
 * @return <tt>coveo::enumerable::size_delegate</tt> instance,
 *         or @c nullptr if it's not possible to quickly calculate
 *         the number of elements in <tt>[beg, end[</tt>
 * @see coveo::enumerable::size_delegate
 */
template<typename It>
auto get_size_delegate_for_iterators(const It& beg, const It& end) -> typename std::enable_if<std::is_base_of<std::random_access_iterator_tag,
                                                                                                              typename std::iterator_traits<It>::iterator_category>::value,
                                                                                              std::function<std::size_t()>>::type
{
    return [beg, end]() -> std::size_t { return std::distance(beg, end); };
}
template<typename It>
auto get_size_delegate_for_iterators(const It&, const It&) -> typename std::enable_if<!std::is_base_of<std::random_access_iterator_tag,
                                                                                                       typename std::iterator_traits<It>::iterator_category>::value,
                                                                                      std::function<std::size_t()>>::type
{
    // No way to quickly determine size, don't try
    return nullptr;
}

/// @endcond

} // detail
} // coveo

#endif // COVEO_ENUMERABLE_DETAIL_H
