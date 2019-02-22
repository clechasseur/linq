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
 * @mainpage notitle
 * @tableofcontents
 * @section intro Introduction
 *
 * Welcome to the documentation of the <tt>coveo::linq</tt> library. This library implements
 * .NET-like <a href="https://en.wikipedia.org/wiki/Language_Integrated_Query">LINQ</a> operators in C++.
 * These can be chained to build powerful expressions to query, filter and transform data in any
 * type of sequence, like arrays, containers, etc. (anything that supports <tt>std::begin()</tt>
 * and <tt>std::end()</tt> should work).
 *
 * If this is your first time with the library, start at @ref linq "LINQ expressions".
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
 * <tt>coveo::linq::operator|()</tt> is used to @ref linq_chaining "chain together" the various
 * @ref linq_operators "LINQ operators", and <tt>coveo::linq::from()</tt> is used as the
 * @ref linq_entry_points "entry point" of the expression, providing the initial sequence on
 * which operators will be applied. The sequence is then transformed by each operator, and
 * the result is passed to the next operator.
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

/**
 * @defgroup linq LINQ expressions
 *
 * LINQ expressions are comprised of three distinct parts: an @ref linq_entry_points "entry point",
 * one or more @ref linq_operators "operators" and a glue operator to @ref linq_chaining "chain"
 * those elements together. The entry point provides the initial sequence on which to operate and
 * each LINQ operator in turn applies a change to that sequence - be it sorting, filtering, etc.
 *
 * Here is an example with all elements. Here, we filter a sequence of numbers to only keep those
 * above 20, then we order them by numerical value.
 *
 * @code
 *   const int NUMBERS[] = { 42, 23, 11, 66, 7 };
 *
 *   using namespace coveo::linq;
 *   auto seq = from(NUMBERS)
 *            | where([](int i) { return i >= 20; })
 *            | order_by([](int i) { return i; });
 *   // seq contains 23, 42, 66
 * @endcode
 *
 * <tt>coveo::linq::from()</tt> is the usual entry point for a LINQ expression, but there are a
 * few others. <tt>coveo::linq::operator|()</tt> is used to chain the LINQ operators together.
 * As for LINQ operators, there are too many of them to list them here. For more information, see:
 *
 * - @ref linq_entry_points
 * - @ref linq_chaining
 * - @ref linq_operators
 */

/**
 * @ingroup linq
 * @defgroup linq_entry_points LINQ expression entry points
 * @brief Functions to start a LINQ expression.
 *
 * An "entry point" is a function that provides the initial sequence on which to apply LINQ operators.
 * It also provides a nice look to the expression, a bit like a database query.
 *
 * The usual entry point for a LINQ expression is <tt>coveo::linq::from()</tt>, which simply returns
 * the sequence passed in. Other entry points generate a sequence for use in the expression.
 */

/**
 * @ingroup linq
 * @defgroup linq_chaining LINQ operator chaining
 * @brief How to chain LINQ operators into an expression.
 *
 * In order to create powerful LINQ expressions, operators need to be chained together so as to be
 * applied in a specific order. To do this, <tt>coveo::linq::operator|()</tt> has been overloaded.
 * Every @ref linq_operators "LINQ operator" returns a function object that is applied on the
 * current sequence by <tt>coveo::linq::operator|()</tt>. By aligning the instances of @c | in the
 * code, LINQ expressions are easy to read:
 *
 * @code
 *   using namespace coveo::linq;
 *   auto seq = from(some_sequence)
 *            | linq_op_1(...)
 *            | linq_op_2(...)
 *            | ...;
 * @endcode
 */

/**
 * @ingroup linq
 * @defgroup linq_operators LINQ operators
 * @brief Information and reference of LINQ operators.
 *
 * A <em>LINQ operator</em> is designed to accept a sequence as input, perform some specific task with it
 * and return a result, which is usually (but not necessarily) another sequence. For example, a LINQ operator
 * might filter the elements of a sequence to remove unwanted ones, or it could concatenate the sequence with
 * another. Some LINQ operators are inspired by database operations, like <tt>coveo::linq::join()</tt>, but
 * not all of them have an equivalent in the database world.
 *
 * Some LINQ operators are identified as @b terminal. This means that they return a result that is not a sequence,
 * but a single value. Because of this, they can only appear at the end of a LINQ expression, since no other
 * operator can be chained after them.
 *
 * In the <tt>coveo::linq</tt> library, LINQ operators are implemented as functions that return a
 * <em>function object</em> implementing the operator logic. The function object is then @e applied
 * to the sequence by <tt>coveo::linq::operator|()</tt>. For more information on this and on how to
 * develop custom LINQ operators, see @ref linq_custom_operators.
 *
 * The <tt>coveo::linq</tt> library includes many different operators for various needs. Most of them
 * are inspired by a corresponding operator in the .NET library, although usage is often different.
 * For a list of operators, see @ref linq_operators_list.
 */

/**
 * @ingroup linq_operators
 * @defgroup linq_operators_list List of built-in LINQ operators
 * @brief Reference for all built-in LINQ operators.
 *
 * This page lists all LINQ operators implemented in the <tt>coveo::linq</tt> library.
 * They are listed alphabetically and grouped by purpose.
 */

/**
 * @ingroup linq_operators
 * @defgroup linq_custom_operators Implementing custom LINQ operators
 * @brief How to design and implement your own LINQ operators.
 *
 * TODO
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
 * @ingroup linq_entry_points
 * @brief Standard LINQ expression entry point.
 * @headerfile linq.h <coveo/linq/linq.h>
 *
 * Standard entry point for a LINQ expression. Specifies the initial
 * sequence on which the first operator will be applied. After this,
 * use <tt>coveo::linq::operator|()</tt> to chain LINQ operators and apply
 * them in a specific order (see @ref linq_chaining "chaining").
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

/**
 * @ingroup linq_entry_points
 * @brief LINQ expression entry point from iterators.
 * @headerfile linq.h <coveo/linq/linq.h>
 *
 * Entry point for a LINQ expression that produces a sequence
 * of elements delimited by two iterators. After this, use
 * <tt>coveo::linq::operator|()</tt> to chain LINQ operators and apply
 * them in a specific order (see @ref linq_chaining "chaining").
 *
 * Use like this:
 *
 * @code
 *   using namespace coveo::linq;
 *   auto result = from_range(something.begin(), something.end())
 *               | linq_operator(...)
 *               | ...;
 * @endcode
 *
 * @see coveo::enumerate_range()
 */
template<typename It>
auto from_range(It ibeg, It iend)
    -> decltype(enumerate_range(std::move(ibeg), std::move(iend)))
{
    return enumerate_range(std::move(ibeg), std::move(iend));
}

/**
 * @ingroup linq_entry_points
 * @brief LINQ expression entry point from range of numbers.
 * @headerfile linq.h <coveo/linq/linq.h>
 *
 * Entry point for a LINQ expression that produces a sequence
 * of incrementing numbers from a starting point. After this, use
 * <tt>coveo::linq::operator|()</tt> to chain LINQ operators and apply
 * them in a specific order (see @ref linq_chaining "chaining").
 *
 * Use like this:
 *
 * @code
 *   using namespace coveo::linq;
 *   auto result = from_int_range(1, 10)    // 1, 2, 3...
 *               | linq_operator(...)
 *               | ...;
 * @endcode
 */
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

/**
 * @ingroup linq_entry_points
 * @brief LINQ expression entry point from a repeated value.
 * @headerfile linq.h <coveo/linq/linq.h>
 *
 * Entry point for a LINQ expression that produces a sequence
 * created by repeating a given value multiple times.. After this, use
 * <tt>coveo::linq::operator|()</tt> to chain LINQ operators and apply
 * them in a specific order (see @ref linq_chaining "chaining").
 *
 * Use like this:
 *
 * @code
 *   using namespace coveo::linq;
 *   auto result = from_repeated(std::string("Life"), 7)    // "Life", "Life", "Life"...
 *               | linq_operator(...)
 *               | ...;
 * @endcode
 */
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
 * @ingroup linq_chaining
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

/**
 * @ingroup linq_operators_list
 * @defgroup linq_op_aggregate aggregate
 * @brief Aggregates values in a sequence to produce a single value.
 *
 * The @c aggregate operator, as its name implies, can be used to @e aggregate all values
 * in a sequence into a single value. To achieve this, it needs an <em>aggregation function</em>
 * that will be called repeatedly to add each element in the sequence to the aggregate.
 *
 * This is a @b terminal operator.
 *
 * <b>.NET equivalent:</b> Aggregate
 */

/**
 * @ingroup linq_op_aggregate
 * @brief Aggregates values using an aggregation function.
 *
 * Aggregates all elements in a sequence by repeatedly calling an <em>aggregation function</em>.
 * The function receives two parameters: the current aggregate value, and the sequence element
 * to add. The function must then add the element to the aggregate and return a new aggregate
 * value. On the first call, the aggregate value is the first sequence element.
 *
 * Does not work on empty sequences.
 *
 * @param agg_f Aggregation function.
 * @return (Once applied) Final aggregate value.
 * @exception coveo::linq::empty_sequence The sequence contains no elements.
 */
template<typename F>
auto aggregate(const F& agg_f)
    -> detail::aggregate_impl_1<F>
{
    return detail::aggregate_impl_1<F>(agg_f);
}

/**
 * @ingroup linq_op_aggregate
 * @brief Aggregates values using an aggregation function, starting with a seed.
 *
 * Aggregates all elements in a sequence by repeatedly calling an <em>aggregation function</em>.
 * The function receives two parameters: the current aggregate value, and the sequence element
 * to add. The function must then add the element to the aggregate and return a new aggregate
 * value. On the first call, the aggregate value is equal to the provided @c seed.
 *
 * @param seed Initial aggregate value.
 * @param agg_f Aggregation function.
 * @return (Once applied) Final aggregate value.
 */
template<typename Acc, typename F>
auto aggregate(const Acc& seed, const F& agg_f)
    -> detail::aggregate_impl_2<Acc, F>
{
    return detail::aggregate_impl_2<Acc, F>(seed, agg_f);
}

/**
 * @ingroup linq_op_aggregate
 * @brief Aggregates values using aggregation function, seed and result selector.
 *
 * Aggregates all elements in a sequence by repeatedly calling an <em>aggregation function</em>.
 * The function receives two parameters: the current aggregate value, and the sequence element
 * to add. The function must then add the element to the aggregate and return a new aggregate
 * value. On the first call, the aggregate value is equal to the provided @c seed. Once the
 * final aggregate value is computed, it is passed to a <em>result selector</em> to produce
 * a final value to return.
 *
 * @param seed Initial aggregate value.
 * @param agg_f Aggregation function.
 * @param result_f Function used to produce final result from aggregate.
 * @return (Once applied) Result returned by @c result_f.
 */
template<typename Acc, typename F, typename RF>
auto aggregate(const Acc& seed, const F& agg_f, const RF& result_f)
    -> detail::aggregate_impl_3<Acc, F, RF>
{
    return detail::aggregate_impl_3<Acc, F, RF>(seed, agg_f, result_f);
}

/**
 * @ingroup linq_operators_list
 * @defgroup linq_op_all all
 * @brief Checks if all elements in a sequence satisfy a predicate.
 *
 * The @c all operator scans a sequence and validates that all its elements
 * satisfy a given @e predicate.
 *
 * This is a @b terminal operator.
 *
 * <b>.NET equivalent:</b> All
 */

/**
 * @ingroup linq_op_all
 * @brief Checks elements in a sequence against a predicate.
 *
 * Scans a sequence and calls a @e predicate with each element. The predicate
 * must return @c true if the element satisfies the predicate. The final result
 * will indicate if all elements satisfy the predicate.
 *
 * Works on empty sequences (returns @c true in such a case).
 *
 * @param pred Predicate to satisfy.
 * @return (Once applied) @c true if all elements in sequence satisfy @c pred.
 */
template<typename Pred>
auto all(const Pred& pred)
    -> detail::all_impl<Pred>
{
    return detail::all_impl<Pred>(pred);
}

/**
 * @ingroup linq_operators_list
 * @defgroup linq_op_any any
 * @brief Checks if a sequence has elements.
 *
 * The @c any operator checks if a sequence has elements, or if it has elements
 * that satisfy a given @e predicate.
 *
 * This is a @b terminal operator.
 *
 * <b>.NET equivalent:</b> Any
 */

/**
 * @ingroup linq_op_any
 * @brief Checks for any element.
 *
 * Checks if a sequence has elements.
 *
 * @return (Once applied) @c true if sequence has at least one element.
 */
template<typename = void>
auto any()
    -> detail::any_impl_0<>
{
    return detail::any_impl_0<>();
}

/**
 * @ingroup linq_op_any
 * @brief Checks for any element that satisfy a predicate.
 *
 * Checks if a sequence has at least one element that satisfy a @e predicate.
 * The predicate is called with each element and must return @c true if the
 * element satisfy the predicate. The final result indicates if there's at least
 * one element that satisfy the predicate.
 *
 * Works on empty sequences (returns @c false in such a case).
 *
 * @param pred Predicate to satisfy.
 * @return (Once applied) @c true if at least one element in sequence satisfies @c pred.
 */
template<typename Pred>
auto any(const Pred& pred)
    -> detail::any_impl_1<Pred>
{
    return detail::any_impl_1<Pred>(pred);
}

/**
 * @ingroup linq_operators_list
 * @defgroup linq_op_average average
 * @brief Computes average of a sequence of values.
 *
 * The @c average operator computes the average of all values in a sequence. To achieve this,
 * it needs a <em>numerical function</em> to extract a numeric value from each sequence element.
 *
 * This is a @b terminal operator.
 *
 * <b>.NET equivalent:</b> Average
 */

/**
 * @ingroup linq_op_average
 * @brief Computes average using numerical function.
 *
 * Computes the average of all elements in a sequence by repeatedly calling a <em>numerical function</em>.
 * The function receives an element as parameter and must return a numerical value for the element.
 * The final result is the average of all such numerical values.
 *
 * Does not work on empty sequences.
 *
 * @param num_f Function to get numerical value for each element.
 * @return (Once applied) Average of all extracted numerical values.
 * @exception coveo::linq::empty_sequence The sequence contains no elements.
 */
template<typename F>
auto average(const F& num_f)
    -> detail::average_impl<F>
{
    return detail::average_impl<F>(num_f);
}

/**
 * @ingroup linq_operators_list
 * @defgroup linq_op_cast cast
 * @brief Casts sequence elements to another type.
 *
 * The @c cast operator modifies a sequence by casting all its element to another type.
 *
 * <b>.NET equivalent:</b> Cast
 */

/**
 * @ingroup linq_op_cast
 * @brief Casts sequence elements to another type.
 *
 * Casts each element in a sequence to another type and returns a new sequence
 * of all such modified elements. The elements are cast using @c static_cast.
 *
 * @tparam U New type to cast elements to.
 * @return (Once applied) Sequence of cast elements.
 */
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
