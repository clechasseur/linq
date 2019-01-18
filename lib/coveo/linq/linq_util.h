// Copyright (c) 2019, Coveo Solutions Inc.
// Distributed under the MIT license (see LICENSE).

// Utilities used to implement linq operators.

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

// Utility methods to throw LINQ-specific exceptions.
template<typename = void>
[[noreturn]] void throw_linq_empty_sequence() {
    throw empty_sequence("empty_sequence");
}
template<typename = void>
[[noreturn]] void throw_linq_out_of_range() {
    throw out_of_range("out_of_range");
}

// Helper that reserves space in a container based on the number of elements in a sequence
// if it's possible to do so quickly (e.g. with random-access iterators or size())
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

// Helper that returns a size_delegate for a sequence if it's possible to quickly calculate
// its size (e.g. with random-access iterators or size())
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
