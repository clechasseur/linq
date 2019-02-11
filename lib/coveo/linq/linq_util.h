/**
 * @file
 * @brief Utilities used to implement LINQ operators.
 *
 * This file contains utility functions useful when implementing LINQ operators.
 * This file is not necessary when using the builtin LINQ operators; only when
 * implementing new ones.
 *
 * @copyright 2016-2019, Coveo Solutions Inc.
 *            Distributed under the Apache License, Version 2.0 (see <a href="https://github.com/coveo/linq/blob/master/LICENSE">LICENSE</a>).
 */

#ifndef COVEO_LINQ_UTIL_H
#define COVEO_LINQ_UTIL_H

#include "coveo/linq/exception.h"

#include <coveo/seq/sequence_util.h>
#include <coveo/seq/detail/enumerable_detail.h>

#include <cstddef>
#include <functional>
#include <iterator>
#include <type_traits>

namespace coveo {
namespace linq {

/**
 * @brief Helper function to throw a <tt>coveo::linq::empty_sequence</tt> exception.
 * @headerfile linq_util.h <coveo/linq/linq_util.h>
 *
 * Throws an instance of <tt>coveo::linq::empty_sequence</tt> to indicate
 * that a LINQ operator does not work on the provided empty sequence.
 * Does not return.
 *
 * @see coveo::linq::empty_sequence
 */
template<typename = void>
[[noreturn]] void throw_linq_empty_sequence() {
    throw empty_sequence("empty_sequence");
}

/**
 * @brief Helper function to throw a <tt>coveo::linq::out_of_range</tt> exception.
 * @headerfile linq_util.h <coveo/linq/linq_util.h>
 *
 * Throws an instance of <tt>coveo::linq::out_of_range</tt> to indicate
 * that a LINQ operator has gone outside the range of the provided sequence.
 * Does not return.
 *
 * @see coveo::linq::out_of_range
 */
template<typename = void>
[[noreturn]] void throw_linq_out_of_range() {
    throw out_of_range("out_of_range");
}

/**
 * @brief Helper function to quickly reserve space in a container if possible.
 * @headerfile linq_util.h <coveo/linq/linq_util.h>
 *
 * Attempts to @c reserve space in container @c cnt to hold as many
 * elements as found in sequence @c seq. This is performed only if
 * it's possible to do so "quickly".
 *
 * - If @c seq is a <tt>coveo::enumerable</tt>, space is reserved
 *   by using its <tt>coveo::enumerable::size()</tt> method if
 *   <tt>coveo::enumerable::has_fast_size()</tt> returns @c true.
 * - If @c seq is a container with a @c size method, space is
 *   reserved by using that method.
 * - If @c seq is a sequence that uses random-access iterators,
 *   space is reserved by using <tt>std::distance</tt>.
 * - Otherwise, space is not reserved.
 *
 * @param cnt Container in which to reserve space.
 * @param seq Sequence to use to try to reserve space for.
 * @return @c true if space has actually been reserved in @c cnt.
 */
template<typename C, typename Seq>
auto try_reserve(C& cnt, const Seq& seq) -> typename std::enable_if<is_enumerable<Seq>::value, bool>::type
{
    const bool can_reserve = seq.has_fast_size();
    if (can_reserve) {
        cnt.reserve(seq.size());
    }
    return can_reserve;
}
template<typename C, typename Seq>
auto try_reserve(C& cnt, const Seq& seq) -> typename std::enable_if<!is_enumerable<Seq>::value &&
                                                                    coveo::detail::has_size_const_method<Seq>::value, bool>::type
{
    cnt.reserve(seq.size());
    return true;
}
template<typename C, typename Seq>
auto try_reserve(C& cnt, const Seq& seq) -> typename std::enable_if<!coveo::detail::has_size_const_method<Seq>::value &&
                                                                    std::is_base_of<std::random_access_iterator_tag,
                                                                                    typename std::iterator_traits<typename seq_traits<Seq>::iterator_type>::iterator_category>::value,
                                                                    bool>::type
{
    cnt.reserve(std::distance(std::begin(seq), std::end(seq)));
    return true;
}
template<typename C, typename Seq>
auto try_reserve(C&, const Seq&) -> typename std::enable_if<!coveo::detail::has_size_const_method<typename std::decay<Seq>::type>::value &&
                                                            !std::is_base_of<std::random_access_iterator_tag,
                                                                             typename std::iterator_traits<typename seq_traits<Seq>::iterator_type>::iterator_category>::value,
                                                            bool>::type
{
    // Can't reserve, no fast way of doing so
    return false;
}

/**
 * @brief Helper function to get a fast size delegate if possible.
 * @headerfile linq_util.h <coveo/linq/linq_util.h>
 *
 * Attempts to create a <tt>coveo::enumerable::size_delegate</tt>
 * that can quickly calculate the number of elements found in
 * sequence @c seq. A size delegate is returned only if it's
 * possible to calculate the number of elements "quickly".
 *
 * - If @c seq is a <tt>coveo::enumerable</tt>, its
 *   <tt>coveo::enumerable::size()</tt> method is used to produce
 *   the size delegate if <tt>coveo::enumerable::has_fast_size()</tt>
 *   returns @c true.
 * - If @c seq is a container with a @c size method, a size delegate
 *   is produced using that method.
 * - If @c seq is a sequence that uses random-access iterators,
 *   a size delegate is produced by using <tt>std::distance</tt>.
 * - Otherwise, no size delegate is produced.
 *
 * @param seq Sequence to calculate the number of elements of
 *            in order to produce the size delegate.
 * @return <tt>coveo::enumerable::size_delegate</tt> instance,
 *         or @c nullptr if it's not possible to quickly calculate
 *         the number of elements in @c seq.
 * @see coveo::enumerable::size_delegate
 */
template<typename Seq>
auto try_get_size_delegate(const Seq& seq) -> typename std::enable_if<is_enumerable<Seq>::value,
                                                                      std::function<std::size_t()>>::type
{
    std::function<std::size_t()> siz;
    if (seq.has_fast_size()) {
        const std::size_t size = seq.size();
        siz = [size]() -> std::size_t { return size; };
    }
    return siz;
}
template<typename Seq>
auto try_get_size_delegate(const Seq& seq) -> typename std::enable_if<!is_enumerable<Seq>::value &&
                                                                      coveo::detail::has_size_const_method<Seq>::value,
                                                                      std::function<std::size_t()>>::type
{
    const std::size_t size = seq.size();
    return [size]() -> std::size_t { return size; };
}
template<typename Seq>
auto try_get_size_delegate(const Seq& seq) -> typename std::enable_if<!coveo::detail::has_size_const_method<Seq>::value &&
                                                                      std::is_base_of<std::random_access_iterator_tag,
                                                                                      typename std::iterator_traits<typename seq_traits<Seq>::iterator_type>::iterator_category>::value,
                                                                      std::function<std::size_t()>>::type
{
    const std::size_t size = static_cast<std::size_t>(std::distance(std::begin(seq), std::end(seq)));
    return [size]() -> std::size_t { return size; };
}
template<typename Seq>
auto try_get_size_delegate(const Seq&) -> typename std::enable_if<!coveo::detail::has_size_const_method<Seq>::value &&
                                                                  !std::is_base_of<std::random_access_iterator_tag,
                                                                                   typename std::iterator_traits<typename seq_traits<Seq>::iterator_type>::iterator_category>::value,
                                                                  std::function<std::size_t()>>::type
{
    // No way to quickly determine size, don't try
    return nullptr;
}

} // linq
} // coveo

#endif // COVEO_LINQ_UTIL_H
