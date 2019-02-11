/**
 * @file
 * @brief C++ LINQ operators.
 *
 * This file contains the definition of all LINQ operators, as well
 * as the entry points (e.g., <tt>coveo::linq::from()</tt>, etc.) and
 * the chaining operator (e.g. <tt>coveo::linq::operator|()</tt>).
 * This is the only file that needs to be included to use the library.
 *
 * @copyright 2016-2019, Coveo Solutions Inc.
 *            Distributed under the Apache License, Version 2.0 (see <a href="https://github.com/coveo/linq/blob/master/LICENSE">LICENSE</a>).
 */

/**
 * @mainpage <tt>coveo::linq</tt> (<a href="https://github.com/coveo/linq">GitHub</a>)
 * @tableofcontents
 * @section intro Introduction
 *
 * Welcome to the documentation of the <tt>coveo::linq</tt> library. This library implements
 * .NET-like <a href="https://en.wikipedia.org/wiki/Language_Integrated_Query">LINQ</a> operators in C++.
 * These can be chained to build powerful expressions to query, filter and transform data in any
 * type of sequence, like arrays, containers, etc. (anything that supports <tt>std::begin()</tt>
 * and <tt>std::end()</tt> should work).
 *
 * @section example Example
 *
 * Here is an example that chains many operators to produce a filtered, ordered sequence:
 *
 * @code
 *   #include <coveo/linq.h>
 *   #include <iostream>
 *
 *   int main()
 *   {
 *       const int FIRST[] = { 42, 23, 66, 13, 11, 7, 24, 10 };
 *       const int SECOND[] = { 67, 22, 13, 23, 41, 66, 6, 7, 10 };
 *
 *       using namespace coveo::linq;
 *
 *       auto is_even = [](int i) { return i % 2 == 0; };
 *
 *       auto seq = from(FIRST)
 *                | intersect(SECOND)                    // Intersect both arrays
 *                | where([](int i) { return i != 13; }) // I'm supersticious, remove 13
 *                | order_by_descending(is_even)         // Place even numbers first
 *                | then_by([](int i) { return i; });    // Then sort numbers ascending
 *
 *       std::cout << std::endl;
 *       for (auto&& elem : seq) {
 *           std::cout << elem << " ";
 *       }
 *       std::cout << std::endl;
 *       // Prints "10 66 7 23"
 *
 *       return 0;
 *   }
 * @endcode
 *
 * <tt>coveo::linq::operator|()</tt> is used to chain together the various LINQ operators,
 * and <tt>coveo::linq::from()</tt> is used as the "entry point" of the expression, providing
 * the initial sequence on which operators will be applied. The sequence is then transformed
 * by each operator, and the result is passed to the next operator.
 *
 * @section install Installing / requirements
 *
 * The <tt>coveo::linq</tt> library is header-only. Therefore, it is not
 * necessary to "build" it to use. Simply copy the @c lib directory with all
 * its files to a suitable place and add it to your project's include path.
 *
 * The library requires C++11 support. Several compilers meet
 * that requirements, including the following (as tested by
 * <a href="https://travis-ci.org/coveo/linq">Continuous</a>
 * <a href="https://ci.appveyor.com/project/clechasseur/linq/branch/master">Integration</a>):
 *
 * - GCC 5.4.1
 * - Clang 3.4
 * - Visual Studio 2015 Update 3
 *
 * @section help Help / bugs / etc.
 *
 * Need help with the library? Found a bug? Please visit the <tt>coveo::linq</tt>
 * GitHub project page at:
 *
 * &nbsp;&nbsp;&nbsp;&nbsp;https://github.com/coveo/linq
 *
 * There, you can file issues with questions or problems.
 *
 * @copyright 2016-2019, Coveo Solutions Inc.
 *            Distributed under the Apache License, Version 2.0 (see <a href="https://github.com/coveo/linq/blob/master/LICENSE">LICENSE</a>).
 */

#ifndef COVEO_LINQ_H
#define COVEO_LINQ_H

#include "coveo/linq/exception.h"

#include <coveo/enumerable.h>
#include <coveo/linq/detail/linq_detail.h>

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <memory>
#include <utility>

namespace coveo {
namespace linq {

/**
 * @brief Entry point for a LINQ expression.
 * @headerfile linq.h <coveo/linq/linq.h>
 *
 * Entry point for the LINQ library. Specifies the initial sequence
 * on which the first operator will be applied. After this, use
 * <tt>coveo::linq::operator|</tt> to chain LINQ operators and apply
 * them in a specific order.
 *
 * Use like this:
 *
 * @code
 *   using namespace coveo::linq;
 *   auto result = from(some_sequence)
 *               | linq_operator(...)
 *               | ...;
 * @endcode
 */
template<typename Seq>
auto from(Seq&& seq) -> decltype(std::forward<Seq>(seq)) {
    return std::forward<Seq>(seq);
}

// Entry point for the LINQ library for a range
// delimited by two iterators. Use like this:
//
//   using namespace coveo::linq;
//   auto result = from_range(something.begin(), something.end())
//               | linq_operator(...)
//               | ...;
//
template<typename It>
auto from_range(It ibeg, It iend)
    -> decltype(enumerate_range(std::move(ibeg), std::move(iend)))
{
    return enumerate_range(std::move(ibeg), std::move(iend));
}

// Entry point for the LINQ library for a range
// of integer values. Use like this:
//
//   using namespace coveo::linq;
//   auto result = from_int_range(1, 10)    // 1, 2, 3...
//               | linq_operator(...)
//               | ...;
//
template<typename IntT>
auto from_int_range(IntT first, std::size_t count)
    -> coveo::enumerable<const typename std::decay<IntT>::type>
{
    std::vector<typename std::decay<IntT>::type> vvalues;
    vvalues.reserve(count);
    while (count-- > 0) {
        vvalues.push_back(first++);
    }
    return enumerate_container(std::move(vvalues));
}

// Entry point for the LINQ library for a range
// of repeated values. Use like this:
//
//   using namespace coveo::linq;
//   auto result = from_repeated(std::string("Life"), 7)    // "Life", "Life", "Life"...
//               | linq_operator(...)
//               | ...;
//
template<typename T>
auto from_repeated(const T& value, std::size_t count)
    -> coveo::enumerable<const typename std::decay<T>::type>
{
    std::vector<typename std::decay<T>::type> vvalues;
    vvalues.reserve(count);
    while (count-- > 0) {
        vvalues.push_back(value);
    }
    return enumerate_container(std::move(vvalues));
}

/**
 * @brief Applies LINQ operators and allows chaining.
 * @headerfile linq.h <coveo/linq/linq.h>
 *
 * Applies a given LINQ operator to the current sequence.
 * Can be used multiple times to chain many operators in a specific order.
 *
 * Use like this:
 *
 * @code
 *   using namespace coveo::linq;
 *   auto result = from(some_sequence)
 *               | linq_op_1(...)
 *               | linq_op_2(...);
 * @endcode
 */
template<typename Seq, typename Op>
auto operator|(Seq&& seq, Op&& op) -> decltype(std::forward<Op>(op)(std::forward<Seq>(seq))) {
    return std::forward<Op>(op)(std::forward<Seq>(seq));
}

// C++ LINQ operator: aggregate
// .NET equivalent: Aggregate

// Operator that aggregates all elements in a sequence using an aggregation function.
// Function receives previous aggregate and new element, must return new aggregate.
// Does not work on empty sequences.
template<typename F>
auto aggregate(const F& agg_f)
    -> detail::aggregate_impl_1<F>
{
    return detail::aggregate_impl_1<F>(agg_f);
}

// Same thing with an initial value for the aggregate.
// Works with empty sequences because of this.
template<typename Acc, typename F>
auto aggregate(const Acc& seed, const F& agg_f)
    -> detail::aggregate_impl_2<Acc, F>
{
    return detail::aggregate_impl_2<Acc, F>(seed, agg_f);
}

// Same thing with a result selector function that
// converts the final aggregate.
template<typename Acc, typename F, typename RF>
auto aggregate(const Acc& seed, const F& agg_f, const RF& result_f)
    -> detail::aggregate_impl_3<Acc, F, RF>
{
    return detail::aggregate_impl_3<Acc, F, RF>(seed, agg_f, result_f);
}

// C++ LINQ operator: all
// .NET equivalent: All

// Operator that checks if all elements in a sequence satisfy a given predicate.
// Works on empty sequences (returns true in such a case).
template<typename Pred>
auto all(const Pred& pred)
    -> detail::all_impl<Pred>
{
    return detail::all_impl<Pred>(pred);
}

// C++ LINQ operator: any
// .NET equivalent: Any

// Operator that checks if a sequence has elements.
template<typename = void>
auto any()
    -> detail::any_impl_0<>
{
    return detail::any_impl_0<>();
}

// Operator that checks if at least one element in a sequence satisfy a given predicate.
// Works on empty sequences (returns false in such a case).
template<typename Pred>
auto any(const Pred& pred)
    -> detail::any_impl_1<Pred>
{
    return detail::any_impl_1<Pred>(pred);
}

// C++ LINQ operator: average
// .NET equivalent: Average

// Operator that computes the average of all elements in a sequence
// using a function to get a numerical value for each.
// Does not work on empty sequences.
template<typename F>
auto average(const F& num_f)
    -> detail::average_impl<F>
{
    return detail::average_impl<F>(num_f);
}

// C++ LINQ operator: cast
// .NET equivalent: Cast

// Operator that casts (using static_cast) the elements
// in a sequence to a different type.
template<typename U>
auto cast()
    -> detail::select_impl<detail::indexless_selector_proxy<detail::cast_selector<U>>>
{
    return detail::select_impl<detail::indexless_selector_proxy<detail::cast_selector<U>>>(
        detail::indexless_selector_proxy<detail::cast_selector<U>>(detail::cast_selector<U>()));
}

// C++ LINQ operator: concat
// .NET equivalent: Concat

// Operator that concatenates the elements of two sequences.
// Sequences must have compatible elements for this to work.
template<typename Seq2>
auto concat(Seq2&& seq2)
    -> detail::concat_impl<Seq2>
{
    return detail::concat_impl<Seq2>(std::forward<Seq2>(seq2));
}

// C++ LINQ operator: contains
// .NET equivalent: Contains

// Operator that determines if a sequence contains a specific element.
// Uses operator== to compare the elements.
template<typename T>
auto contains(const T& obj)
    -> detail::contains_impl_1<T>
{
    return detail::contains_impl_1<T>(obj);
}

// Same thing with a predicate to compare the elements.
// The predicate always receives obj as its second argument.
template<typename T, typename Pred>
auto contains(const T& obj, const Pred& pred)
    -> detail::contains_impl_2<T, Pred>
{
    return detail::contains_impl_2<T, Pred>(obj, pred);
}

// C++ LINQ operator: count
// .NET equivalent: Count

// Operator that returns the number of elements in a sequence.
template<typename = void>
auto count()
    -> detail::count_impl_0<>
{
    return detail::count_impl_0<>();
}

// Operator that returns the number of elements in a sequence
// that satisfy a given predicate.
template<typename Pred>
auto count(const Pred& pred)
    -> detail::count_impl_1<Pred>
{
    return detail::count_impl_1<Pred>(pred);
}

// C++ LINQ operator: default_if_empty
// .NET equivalent: DefaultIfEmpty

// Operator that returns a sequence or, if it's empty, a sequence
// containing a single, default element.
template<typename = void>
auto default_if_empty()
    -> detail::default_if_empty_impl_0<>
{
    return detail::default_if_empty_impl_0<>();
}

// Same thing but with a specific default value if sequence is empty.
template<typename T>
auto default_if_empty(const T& obj)
    -> detail::default_if_empty_impl_1<T>
{
    return detail::default_if_empty_impl_1<T>(obj);
}

// C++ LINQ operator: distinct
// .NET equivalent: Distinct

// Operator that filters out duplicate elements in a sequence.
// Otherwise, returns elements in the same order.
template<typename = void>
auto distinct()
    -> detail::distinct_impl<detail::less<>>
{
    return detail::distinct_impl<detail::less<>>(detail::less<>());
}

// Same thing but with a predicate to compare the elements.
// The predicate must provide a strict ordering of elements, like std::less.
template<typename Pred>
auto distinct(Pred&& pred)
    -> detail::distinct_impl<Pred>
{
    return detail::distinct_impl<Pred>(std::forward<Pred>(pred));
}

// C++ LINQ operator: element_at
// .NET equivalent: ElementAt

// Operator that returns the nth element in a sequence.
// Throws if sequence does not have enough elements.
template<typename = void>
auto element_at(std::size_t n)
    -> detail::element_at_impl<>
{
    return detail::element_at_impl<>(n);
}

// C++ LINQ operator: element_at_or_default
// .NET equivalent: ElementAtOrDefault

// Operator that returns the nth element in a sequence or,
// if the sequence does not have enough elements, a
// default-initialized value.
template<typename = void>
auto element_at_or_default(std::size_t n)
    -> detail::element_at_or_default_impl<>
{
    return detail::element_at_or_default_impl<>(n);
}

// C++ LINQ operator: except
// .NET equivalent: Except

// Operator that returns all elements that are in the first sequence
// but not in the second sequence (essentially a set difference).
template<typename Seq2>
auto except(Seq2&& seq2)
    -> detail::except_impl<Seq2, detail::less<>>
{
    return detail::except_impl<Seq2, detail::less<>>(std::forward<Seq2>(seq2), detail::less<>());
}

// Same thing but with a predicate to compare the elements.
// The predicate must provide a strict ordering of the elements, like std::less.
template<typename Seq2, typename Pred>
auto except(Seq2&& seq2, Pred&& pred)
    -> detail::except_impl<Seq2, Pred>
{
    return detail::except_impl<Seq2, Pred>(std::forward<Seq2>(seq2), std::forward<Pred>(pred));
}

// C++ LINQ operqator: first
// .NET equivalent: First

// Operator that returns the first element in a sequence.
// Does not work on empty sequences.
template<typename = void>
auto first()
    -> detail::first_impl_0<>
{
    return detail::first_impl_0<>();
}

// Operator that returns the first element in a sequence
// that satisfies a predicate. Does not work on empty sequences.
template<typename Pred>
auto first(const Pred& pred)
    -> detail::first_impl_1<Pred>
{
    return detail::first_impl_1<Pred>(pred);
}

// C++ LINQ operator: first_or_default
// .NET equivalent: FirstOrDefault

// Operator that returns the first element in a sequence
// or a default-initialized value if it's empty.
template<typename = void>
auto first_or_default()
    -> detail::first_or_default_impl_0<>
{
    return detail::first_or_default_impl_0<>();
}

// Operator that returns the first element in a sequence
// that satistifies a predicate or, if none are found,
// a default-initialized value.
template<typename Pred>
auto first_or_default(const Pred& pred)
    -> detail::first_or_default_impl_1<Pred>
{
    return detail::first_or_default_impl_1<Pred>(pred);
}

// C++ LINQ operators: group_by, group_values_by, group_by_and_fold, group_values_by_and_fold
// .NET equivalent: GroupBy

// Operator that groups elements in a sequence according to their keys, as returned by a key selector.
// Returns a sequence of pairs whose first element is the common key and second element is a sequence
// of values matching that key.
template<typename KeySelector>
auto group_by(KeySelector&& key_sel)
    -> detail::group_by_impl<KeySelector,
                             detail::identity<>,
                             detail::pair_of<>,
                             detail::less<>>
{
    return detail::group_by_impl<KeySelector,
                                 detail::identity<>,
                                 detail::pair_of<>,
                                 detail::less<>>(std::forward<KeySelector>(key_sel),
                                                 detail::identity<>(),
                                                 detail::pair_of<>(),
                                                 detail::less<>());
}

// Same thing but with a predicate used to compare keys.
template<typename KeySelector,
         typename Pred>
auto group_by(KeySelector&& key_sel,
              Pred&& pred)
    -> detail::group_by_impl<KeySelector,
                             detail::identity<>,
                             detail::pair_of<>,
                             Pred>
{
    return detail::group_by_impl<KeySelector,
                                 detail::identity<>,
                                 detail::pair_of<>,
                                 Pred>(std::forward<KeySelector>(key_sel),
                                       detail::identity<>(),
                                       detail::pair_of<>(),
                                       std::forward<Pred>(pred));
}

// Operator that groups values according to keys, with both keys
// and values extracted from a sequence's elements using selectors.
template<typename KeySelector,
         typename ValueSelector>
auto group_values_by(KeySelector&& key_sel,
                     ValueSelector&& value_sel)
    -> detail::group_by_impl<KeySelector,
                             ValueSelector,
                             detail::pair_of<>,
                             detail::less<>>
{
    return detail::group_by_impl<KeySelector,
                                 ValueSelector,
                                 detail::pair_of<>,
                                 detail::less<>>(std::forward<KeySelector>(key_sel),
                                                 std::forward<ValueSelector>(value_sel),
                                                 detail::pair_of<>(),
                                                 detail::less<>());
}

// Same thing but with a predicate used to compare keys.
template<typename KeySelector,
         typename ValueSelector,
         typename Pred>
auto group_values_by(KeySelector&& key_sel,
                     ValueSelector&& value_sel,
                     Pred&& pred)
    -> detail::group_by_impl<KeySelector,
                             ValueSelector,
                             detail::pair_of<>,
                             Pred>
{
    return detail::group_by_impl<KeySelector,
                                 ValueSelector,
                                 detail::pair_of<>,
                                 Pred>(std::forward<KeySelector>(key_sel),
                                       std::forward<ValueSelector>(value_sel),
                                       detail::pair_of<>(),
                                       std::forward<Pred>(pred));
}

// Operator that groups a sequence's elements by keys as returned by a key selector,
// then folds each group into a final result using a result selector. The result
// selector is called with two arguments: the common key of all elements and a
// sequence of values matching that key.
template<typename KeySelector,
         typename ResultSelector>
auto group_by_and_fold(KeySelector&& key_sel,
                       ResultSelector&& result_sel)
    -> detail::group_by_impl<KeySelector,
                             detail::identity<>,
                             ResultSelector,
                             detail::less<>>
{
    return detail::group_by_impl<KeySelector,
                                 detail::identity<>,
                                 ResultSelector,
                                 detail::less<>>(std::forward<KeySelector>(key_sel),
                                                 detail::identity<>(),
                                                 std::forward<ResultSelector>(result_sel),
                                                 detail::less<>());
}

// Same thing with a predicate to compare the keys.
template<typename KeySelector,
         typename ResultSelector,
         typename Pred>
auto group_by_and_fold(KeySelector&& key_sel,
                       ResultSelector&& result_sel,
                       Pred&& pred)
    -> detail::group_by_impl<KeySelector,
                             detail::identity<>,
                             ResultSelector,
                             Pred>
{
    return detail::group_by_impl<KeySelector,
                                 detail::identity<>,
                                 ResultSelector,
                                 Pred>(std::forward<KeySelector>(key_sel),
                                       detail::identity<>(),
                                       std::forward<ResultSelector>(result_sel),
                                       std::forward<Pred>(pred));
}

// Operator that creates groups of keys and values from a sequence's
// elements using selectors, then uses a result selector on each group
// to create the final results.
template<typename KeySelector,
         typename ValueSelector,
         typename ResultSelector>
auto group_values_by_and_fold(KeySelector&& key_sel,
                              ValueSelector&& value_sel,
                              ResultSelector&& result_sel)
    -> detail::group_by_impl<KeySelector,
                             ValueSelector,
                             ResultSelector,
                             detail::less<>>
{
    return detail::group_by_impl<KeySelector,
                                 ValueSelector,
                                 ResultSelector,
                                 detail::less<>>(std::forward<KeySelector>(key_sel),
                                                 std::forward<ValueSelector>(value_sel),
                                                 std::forward<ResultSelector>(result_sel),
                                                 detail::less<>());
}

// Same thing with a predicate to compare the keys.
template<typename KeySelector,
         typename ValueSelector,
         typename ResultSelector,
         typename Pred>
auto group_values_by_and_fold(KeySelector&& key_sel,
                              ValueSelector&& value_sel,
                              ResultSelector&& result_sel,
                              Pred&& pred)
    -> detail::group_by_impl<KeySelector,
                             ValueSelector,
                             ResultSelector,
                             Pred>
{
    return detail::group_by_impl<KeySelector,
                                 ValueSelector,
                                 ResultSelector,
                                 Pred>(std::forward<KeySelector>(key_sel),
                                       std::forward<ValueSelector>(value_sel),
                                       std::forward<ResultSelector>(result_sel),
                                       std::forward<Pred>(pred));
}

// C++ LINQ operator: group_join
// .NET equivalent: GroupJoin

// Operator that extracts keys from an outer and inner sequences using key selectors,
// then creates groups of elements from inner sequence matching keys of elements in
// outer sequence. A result selector is then used to convert each group into a final result.
// The result selector is called with two arguments: an element from the outer sequence
// and a sequence of elements from the inner sequence whose keys match.
template<typename InnerSeq,
         typename OuterKeySelector,
         typename InnerKeySelector,
         typename ResultSelector>
auto group_join(InnerSeq&& inner_seq,
                OuterKeySelector&& outer_key_sel,
                InnerKeySelector&& inner_key_sel,
                ResultSelector&& result_sel)
    -> detail::group_join_impl<InnerSeq,
                               OuterKeySelector,
                               InnerKeySelector,
                               ResultSelector,
                               detail::less<>>
{
    return detail::group_join_impl<InnerSeq,
                                   OuterKeySelector,
                                   InnerKeySelector,
                                   ResultSelector,
                                   detail::less<>>(std::forward<InnerSeq>(inner_seq),
                                                   std::forward<OuterKeySelector>(outer_key_sel),
                                                   std::forward<InnerKeySelector>(inner_key_sel),
                                                   std::forward<ResultSelector>(result_sel),
                                                   detail::less<>());
}

// Same as above, with a predicate to compare the keys in both sequences.
template<typename InnerSeq,
         typename OuterKeySelector,
         typename InnerKeySelector,
         typename ResultSelector,
         typename Pred>
auto group_join(InnerSeq&& inner_seq,
                OuterKeySelector&& outer_key_sel,
                InnerKeySelector&& inner_key_sel,
                ResultSelector&& result_sel,
                Pred&& pred)
    -> detail::group_join_impl<InnerSeq,
                               OuterKeySelector,
                               InnerKeySelector,
                               ResultSelector,
                               Pred>
{
    return detail::group_join_impl<InnerSeq,
                                   OuterKeySelector,
                                   InnerKeySelector,
                                   ResultSelector,
                                   Pred>(std::forward<InnerSeq>(inner_seq),
                                         std::forward<OuterKeySelector>(outer_key_sel),
                                         std::forward<InnerKeySelector>(inner_key_sel),
                                         std::forward<ResultSelector>(result_sel),
                                         std::forward<Pred>(pred));
}

// C++ LINQ operator: intersect
// .NET equivalent: Intersect

// Operator that returns all elements that are found in two sequences.
// Essentially a set intersection.
template<typename Seq2>
auto intersect(Seq2&& seq2)
    -> detail::intersect_impl<Seq2, detail::less<>>
{
    return detail::intersect_impl<Seq2, detail::less<>>(std::forward<Seq2>(seq2),
                                                        detail::less<>());
}

// Same as above, with a predicate to compare the elements.
// The predicate must provide a strict ordering of the elements, like std::less.
template<typename Seq2, typename Pred>
auto intersect(Seq2&& seq2, Pred&& pred)
    -> detail::intersect_impl<Seq2, Pred>
{
    return detail::intersect_impl<Seq2, Pred>(std::forward<Seq2>(seq2),
                                              std::forward<Pred>(pred));
}

// C++ LINQ operator: join
// .NET equivalent: Join

// Operator that extracts keys from elements in two sequences and joins
// elements with matching keys using a result selector, like a database JOIN.
// The result selector is called with two arguments: an element from the outer
// sequence and an element from the inner sequence whose key matches.
template<typename InnerSeq,
         typename OuterKeySelector,
         typename InnerKeySelector,
         typename ResultSelector>
auto join(InnerSeq&& inner_seq,
          OuterKeySelector&& outer_key_sel,
          InnerKeySelector&& inner_key_sel,
          ResultSelector&& result_sel)
    -> detail::join_impl<InnerSeq,
                         OuterKeySelector,
                         InnerKeySelector,
                         ResultSelector,
                         detail::less<>>
{
    return detail::join_impl<InnerSeq,
                             OuterKeySelector,
                             InnerKeySelector,
                             ResultSelector,
                             detail::less<>>(std::forward<InnerSeq>(inner_seq),
                                             std::forward<OuterKeySelector>(outer_key_sel),
                                             std::forward<InnerKeySelector>(inner_key_sel),
                                             std::forward<ResultSelector>(result_sel),
                                             detail::less<>());
}

// Same as above, with a predicate to compare the keys.
template<typename InnerSeq,
         typename OuterKeySelector,
         typename InnerKeySelector,
         typename ResultSelector,
         typename Pred>
auto join(InnerSeq&& inner_seq,
          OuterKeySelector&& outer_key_sel,
          InnerKeySelector&& inner_key_sel,
          ResultSelector&& result_sel,
          Pred&& pred)
    -> detail::join_impl<InnerSeq,
                         OuterKeySelector,
                         InnerKeySelector,
                         ResultSelector,
                         Pred>
{
    return detail::join_impl<InnerSeq,
                             OuterKeySelector,
                             InnerKeySelector,
                             ResultSelector,
                             Pred>(std::forward<InnerSeq>(inner_seq),
                                   std::forward<OuterKeySelector>(outer_key_sel),
                                   std::forward<InnerKeySelector>(inner_key_sel),
                                   std::forward<ResultSelector>(result_sel),
                                   std::forward<Pred>(pred));
}

// C++ LINQ operqator: last
// .NET equivalent: Last

// Operator that returns the last element in a sequence.
// Does not work on empty sequences.
template<typename = void>
auto last()
    -> detail::last_impl_0<>
{
    return detail::last_impl_0<>();
}

// Operator that returns the last element in a sequence
// that satisfies a predicate. Does not work on empty sequences.
template<typename Pred>
auto last(const Pred& pred)
    -> detail::last_impl_1<Pred>
{
    return detail::last_impl_1<Pred>(pred);
}

// C++ LINQ operator: last_or_default
// .NET equivalent: LastOrDefault

// Operator that returns the last element in a sequence
// or a default-initialized value if it's empty.
template<typename = void>
auto last_or_default()
    -> detail::last_or_default_impl_0<>
{
    return detail::last_or_default_impl_0<>();
}

// Operator that returns the last element in a sequence
// that satistifies a predicate or, if none are found,
// a default-initialized value.
template<typename Pred>
auto last_or_default(const Pred& pred)
    -> detail::last_or_default_impl_1<Pred>
{
    return detail::last_or_default_impl_1<Pred>(pred);
}

// C++ LINQ operator: max
// .NET equivalent: Max

// Operator that returns the maximum value in a sequence.
// Does not work on empty sequences.
template<typename = void>
auto max()
    -> detail::max_impl_0<>
{
    return detail::max_impl_0<>();
}

// Operator that returns the maximum value in a sequence by
// projecting each element in the sequence into a different
// value using a selector. Does not work on empty sequences.
template<typename Selector>
auto max(const Selector& sel)
    -> detail::max_impl_1<Selector>
{
    return detail::max_impl_1<Selector>(sel);
}

// C++ LINQ operator: min
// .NET equivalent: Min

// Operator that returns the minimum value in a sequence.
// Does not work on empty sequences.
template<typename = void>
auto min()
    -> detail::min_impl_0<>
{
    return detail::min_impl_0<>();
}

// Operator that returns the minimum value in a sequence by
// projecting each element in the sequence into a different
// value using a selector. Does not work on empty sequences.
template<typename Selector>
auto min(const Selector& sel)
    -> detail::min_impl_1<Selector>
{
    return detail::min_impl_1<Selector>(sel);
}

// C++ LINQ operator: none
// No .NET equivalent.

// Operator that checks if no element in a sequence satisfy a given predicate.
// Works on empty sequences (returns true in such a case).
template<typename Pred>
auto none(const Pred& pred)
    -> detail::none_impl<Pred>
{
    return detail::none_impl<Pred>(pred);
}

// C++ LINQ operators: order_by/order_by_descending/then_by/then_by_descending
// .NET equivalents: OrderBy/OrderByDesdending/ThenBy/ThenByDescending

// Operator that orders a sequence by fetching a key for each element
// using a key selector and then sorting elements by their keys using
// operator< to compare the keys.
template<typename KeySelector>
auto order_by(KeySelector&& key_sel)
    -> detail::order_by_impl<detail::order_by_comparator<KeySelector, detail::less<>, false>>
{
    typedef detail::order_by_comparator<KeySelector, detail::less<>, false> comparator;
    return detail::order_by_impl<comparator>(detail::make_unique<comparator>(std::forward<KeySelector>(key_sel), detail::less<>()));
}

// As above, but uses the given predicate to compare the keys.
template<typename KeySelector, typename Pred>
auto order_by(KeySelector&& key_sel, Pred&& pred)
    -> detail::order_by_impl<detail::order_by_comparator<KeySelector, Pred, false>>
{
    typedef detail::order_by_comparator<KeySelector, Pred, false> comparator;
    return detail::order_by_impl<comparator>(detail::make_unique<comparator>(std::forward<KeySelector>(key_sel), std::forward<Pred>(pred)));
}

// As the first implementation above, but sorts keys descending,
// as if using operator>.
template<typename KeySelector>
auto order_by_descending(KeySelector&& key_sel)
    -> detail::order_by_impl<detail::order_by_comparator<KeySelector, detail::less<>, true>>
{
    typedef detail::order_by_comparator<KeySelector, detail::less<>, true> comparator;
    return detail::order_by_impl<comparator>(detail::make_unique<comparator>(std::forward<KeySelector>(key_sel), detail::less<>()));
}

// As above, but uses the given predicate to compare the keys.
template<typename KeySelector, typename Pred>
auto order_by_descending(KeySelector&& key_sel, Pred&& pred)
    -> detail::order_by_impl<detail::order_by_comparator<KeySelector, Pred, true>>
{
    typedef detail::order_by_comparator<KeySelector, Pred, true> comparator;
    return detail::order_by_impl<comparator>(detail::make_unique<comparator>(std::forward<KeySelector>(key_sel), std::forward<Pred>(pred)));
}

// Operator that further orders a sequence previously ordered via order_by
// using a different key selector.
template<typename KeySelector>
auto then_by(KeySelector&& key_sel)
    -> decltype(order_by(std::forward<KeySelector>(key_sel)))
{
    return order_by(std::forward<KeySelector>(key_sel));
}

// As above, but uses the given predicate to compare the new keys.
template<typename KeySelector, typename Pred>
auto then_by(KeySelector&& key_sel, Pred&& pred)
    -> decltype(order_by(std::forward<KeySelector>(key_sel), std::forward<Pred>(pred)))
{
    return order_by(std::forward<KeySelector>(key_sel), std::forward<Pred>(pred));
}

// Operator that further orders a sequence previously ordered via order_by
// using a different key selector, but in descending order.
template<typename KeySelector>
auto then_by_descending(KeySelector&& key_sel)
    -> decltype(order_by_descending(std::forward<KeySelector>(key_sel)))
{
    return order_by_descending(std::forward<KeySelector>(key_sel));
}

// As above, but uses the given predicate to compare the new keys.
template<typename KeySelector, typename Pred>
auto then_by_descending(KeySelector&& key_sel, Pred&& pred)
    -> decltype(order_by_descending(std::forward<KeySelector>(key_sel), std::forward<Pred>(pred)))
{
    return order_by_descending(std::forward<KeySelector>(key_sel), std::forward<Pred>(pred));
}

// C++ LINQ operator: reverse
// .NET equivalent: Reverse

// Operator that reverses the elements of a sequence.
template<typename = void>
auto reverse()
    -> detail::reverse_impl<>
{
    return detail::reverse_impl<>();
}

// C++ LINQ operators: select/select_with_index/select_many/select_many_with_index
// .NET equivalents: Select/SelectMany

// Operator that projects each element in a sequence into another form
// using a selector function, a little like std::transform.
template<typename Selector>
auto select(Selector&& sel)
    -> detail::select_impl<detail::indexless_selector_proxy<Selector>>
{
    return detail::select_impl<detail::indexless_selector_proxy<Selector>>(
        detail::indexless_selector_proxy<Selector>(std::forward<Selector>(sel)));
}

// As above, but selector also receives the index of each element in the sequence.
template<typename Selector>
auto select_with_index(Selector&& sel)
    -> detail::select_impl<Selector>
{
    return detail::select_impl<Selector>(std::forward<Selector>(sel));
}

// Operator that projects each element in a sequence into a sequence of
// elements in another form using a selector function then flattens all
// those sequences into one.
template<typename Selector>
auto select_many(Selector&& sel)
    -> detail::select_many_impl<detail::indexless_selector_proxy<Selector>>
{
    return detail::select_many_impl<detail::indexless_selector_proxy<Selector>>(
        detail::indexless_selector_proxy<Selector>(std::forward<Selector>(sel)));
}

// As above, but selector also receives the index of each element in the sequence.
template<typename Selector>
auto select_many_with_index(Selector&& sel)
    -> detail::select_many_impl<Selector>
{
    return detail::select_many_impl<Selector>(std::forward<Selector>(sel));
}

// C++ LINQ operator: sequence_equal
// .NET equivalent: SequenceEqual

// Operator that verifies if two sequences contain the same elements.
// Uses operator== to compare the elements.
template<typename Seq2>
auto sequence_equal(const Seq2& seq2)
    -> detail::sequence_equal_impl_1<Seq2>
{
    return detail::sequence_equal_impl_1<Seq2>(seq2);
}

// As above, but uses the given predicate to compare the elements.
template<typename Seq2, typename Pred>
auto sequence_equal(const Seq2& seq2, const Pred& pred)
    -> detail::sequence_equal_impl_2<Seq2, Pred>
{
    return detail::sequence_equal_impl_2<Seq2, Pred>(seq2, pred);
}

// C++ LINQ operator: single
// .NET equivalent: Single

// Operator that returns the single value in the given sequence.
// Throws an exception if the sequence is empty or if it has
// more than one value.
template<typename = void>
auto single()
    -> detail::single_impl_0<>
{
    return detail::single_impl_0<>();
}

// Operator that returns the single value in the given sequence
// that satisfies the given predicate. Throws an exception if the
// sequence contains no such element or more than one such element.
template<typename Pred>
auto single(const Pred& pred)
    -> detail::single_impl_1<Pred>
{
    return detail::single_impl_1<Pred>(pred);
}

// C++ LINQ operator: single_or_default
// .NET equivalent: SingleOrDefault

// Operator that returns the single value in the given sequence,
// or a default-initialized value if the sequence is empty or if
// it has more than one element.
template<typename = void>
auto single_or_default()
    -> detail::single_or_default_impl_0<>
{
    return detail::single_or_default_impl_0<>();
}

// Operator that returns the single value in the given sequence
// that satisfies the given predicate, or a default-initialized value
// if the sequence contains no such element or more than one such element.
template<typename Pred>
auto single_or_default(const Pred& pred)
    -> detail::single_or_default_impl_1<Pred>
{
    return detail::single_or_default_impl_1<Pred>(pred);
}

// C++ LINQ operator: skip
// .NET equivalent: Skip

// Operator that skips the X first elements of a sequence.
// If the sequence contains less than X elements, returns
// an empty sequence.
template<typename = void>
auto skip(std::size_t n)
    -> detail::skip_impl<detail::skip_n_pred<>>
{
    return detail::skip_impl<detail::skip_n_pred<>>(detail::skip_n_pred<>(n), n);
}

// C++ LINQ operators: skip_while, skip_while_with_index
// .NET equivalent: SkipWhile

// Operator that skips all first elements of a sequence
// that satisfy a given predicate. If the sequence contains
// only elements that satisfy the predicate, returns an
// empty sequence.
template<typename Pred>
auto skip_while(Pred&& pred)
    -> detail::skip_impl<detail::indexless_selector_proxy<Pred>>
{
    return detail::skip_impl<detail::indexless_selector_proxy<Pred>>(detail::indexless_selector_proxy<Pred>(std::forward<Pred>(pred)),
                                                                     static_cast<std::size_t>(-1));
}

// As above, but the predicate receives the index of the
// element in the sequence as second argument.
template<typename Pred>
auto skip_while_with_index(Pred&& pred)
    -> detail::skip_impl<Pred>
{
    return detail::skip_impl<Pred>(std::forward<Pred>(pred),
                                   static_cast<std::size_t>(-1));
}

// C++ LINQ operator: sum
// .NET equivalent: Sum

// Operator that computes the sum of all elements in a sequence
// using a function to get a numerical value for each.
// Does not work on empty sequences.
template<typename F>
auto sum(const F& num_f)
    -> detail::sum_impl<F>
{
    return detail::sum_impl<F>(num_f);
}

// C++ LINQ operator: take
// .NET equivalent: Take

// Operator that returns the X first elements of a sequence.
// If the sequence contains less than X elements, returns
// as much as possible.
template<typename = void>
auto take(std::size_t n)
    -> detail::take_impl<detail::skip_n_pred<>>
{
    return detail::take_impl<detail::skip_n_pred<>>(detail::skip_n_pred<>(n), n);
}

// C++ LINQ operators: take_while, take_while_with_index
// .NET equivalent: TakeWhile

// Operator that returns all first elements of a sequence
// that satisfy a given predicate. If the sequence contains
// no elements that satisfy the predicate, returns an
// empty sequence.
template<typename Pred>
auto take_while(Pred&& pred)
    -> detail::take_impl<detail::indexless_selector_proxy<Pred>>
{
    return detail::take_impl<detail::indexless_selector_proxy<Pred>>(detail::indexless_selector_proxy<Pred>(std::forward<Pred>(pred)),
                                                                     static_cast<std::size_t>(-1));
}

// As above, but the predicate receives the index of the
// element in the sequence as second argument.
template<typename Pred>
auto take_while_with_index(Pred&& pred)
    -> detail::take_impl<Pred>
{
    return detail::take_impl<Pred>(std::forward<Pred>(pred),
                                   static_cast<std::size_t>(-1));
}

// C++ LINQ operators: to, to_vector, to_associative, to_map
// .NET equivalents: ToArray, ToDictionary, ToList, ToLookup

// Operator that converts a sequence into a container of the given type.
template<typename Container>
auto to()
    -> detail::to_impl<Container>
{
    return detail::to_impl<Container>();
}

// Specialized version of the above for std::vector.
// Auto-detects the element type from the sequence.
template<typename = void>
auto to_vector()
    -> detail::to_vector_impl<>
{
    return detail::to_vector_impl<>();
}

// Operator that converts a sequence into a specific type
// of associative container (like std::map), using a key
// selector function to fetch a key for each element.
template<typename Container,
         typename KeySelector>
auto to_associative(const KeySelector& key_sel)
    -> detail::to_associative_impl_1<Container, KeySelector>
{
    return detail::to_associative_impl_1<Container, KeySelector>(key_sel);
}

// As above, but using an element selector to convert elements
// inserted as mapped values.
template<typename Container,
         typename KeySelector,
         typename ElementSelector>
auto to_associative(const KeySelector& key_sel,
                    const ElementSelector& elem_sel)
    -> detail::to_associative_impl_2<Container, KeySelector, ElementSelector>
{
    return detail::to_associative_impl_2<Container, KeySelector, ElementSelector>(key_sel, elem_sel);
}

// Specialized version of the above that returns an std::map.
// Auto-detects the key and mapped types from the sequence and selector.
template<typename KeySelector>
auto to_map(const KeySelector& key_sel)
    -> detail::to_map_impl_1<KeySelector>
{
    return detail::to_map_impl_1<KeySelector>(key_sel);
}

// As above, but using an element selector to convert elements
// inserted as mapped values.
template<typename KeySelector,
         typename ElementSelector>
auto to_map(const KeySelector& key_sel,
            const ElementSelector& elem_sel)
    -> detail::to_map_impl_2<KeySelector, ElementSelector>
{
    return detail::to_map_impl_2<KeySelector, ElementSelector>(key_sel, elem_sel);
}

// C++ LINQ operator: union_with
// .NET equivalent: Union

// Operator that returns all elements that are found in either of two sequences,
// excluding duplicates. Essentially a set union.
template<typename Seq2>
auto union_with(Seq2&& seq2)
    -> detail::union_impl<Seq2, detail::less<>>
{
    return detail::union_impl<Seq2, detail::less<>>(std::forward<Seq2>(seq2),
                                                    detail::less<>());
}

// Same as above, with a predicate to compare the elements.
// The predicate must provide a strict ordering of the elements, like std::less.
template<typename Seq2, typename Pred>
auto union_with(Seq2&& seq2, Pred&& pred)
    -> detail::union_impl<Seq2, Pred>
{
    return detail::union_impl<Seq2, Pred>(std::forward<Seq2>(seq2),
                                          std::forward<Pred>(pred));
}

// C++ LINQ operators: where, where_with_index
// .NET equivalent: Where

// Operator that only returns elements of a sequence that
// satisfy a given predicate.
template<typename Pred>
auto where(Pred&& pred)
    -> detail::where_impl<detail::indexless_selector_proxy<Pred>>
{
    return detail::where_impl<detail::indexless_selector_proxy<Pred>>(
        detail::indexless_selector_proxy<Pred>(std::forward<Pred>(pred)));
}

// As above, but the predicate receives the index of the
// element as second argument.
template<typename Pred>
auto where_with_index(Pred&& pred)
    -> detail::where_impl<Pred>
{
    return detail::where_impl<Pred>(std::forward<Pred>(pred));
}

// C++ LINQ operator: zip
// .NET equivalent: Zip

// Operator that iterates two sequences and passes elements with
// the same index to a result selector function, producing a sequence
// of the results. The resulting sequence will be as long as the
// shortest of the two input sequences.
template<typename Seq2,
         typename ResultSelector>
auto zip(Seq2&& seq2, ResultSelector&& result_sel)
    -> detail::zip_impl<Seq2, ResultSelector>
{
    return detail::zip_impl<Seq2, ResultSelector>(std::forward<Seq2>(seq2),
                                                  std::forward<ResultSelector>(result_sel));
}

} // linq
} // coveo

#endif // COVEO_LINQ_H
