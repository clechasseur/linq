/**
 * @file
 * @brief Utilities used by <tt>coveo::enumerable</tt>.
 *
 * @copyright 2016-2019, Coveo Solutions Inc.
 *            Distributed under the Apache License, Version 2.0 (see <a href="https://github.com/coveo/linq/blob/master/LICENSE">LICENSE</a>).
 */

#ifndef COVEO_SEQUENCE_UTIL_H
#define COVEO_SEQUENCE_UTIL_H

#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>

namespace coveo {

/// @cond

// Forward declaration of enumerable, for type traits
template<typename> class enumerable;

/// @endcond

/**
 * @brief Traits class for elements in a sequence.
 * @headerfile sequence_util.h <coveo/seq/sequence_util.h>
 *
 * Traits class containing definitions pertaining to the elements of a sequence.
 * For sequences of references or <tt>std::reference_wrapper</tt>s, provides
 * information about the referred type instead.
 *
 * @tparam T Type of elements in the sequence.
 */
template<typename T>
struct seq_element_traits
{
    /**
     * @brief Type of element in the sequence.
     *
     * Type of the sequence's elements. Corresponds to @c T.
     */
    using value_type = T;

    /**
     * @brief Type of element in the sequence, but @c const.
     *
     * Same as <tt>coveo::seq_element_traits::value_type</tt>, but @c const.
     */
    using const_value_type = const value_type;

    /**
     * @brief Raw type of element in the sequence.
     *
     * Same as <tt>coveo::seq_element_traits::value_type</tt>, but "raw", e.g. without @c const or @c volatile.
     */
    using raw_value_type = typename std::remove_cv<value_type>::type;

    /**
     * @brief Pointer to a sequence element.
     *
     * Pointer to an element in the sequence.
     * Corresponds to <tt>coveo::seq_element_traits::value_type*</tt>.
     */
    using pointer = value_type*;
    
    /**
     * @brief Reference to a sequence element.
     *
     * Reference to an element in the sequence.
     * Corresponds to <tt>coveo::seq_element_traits::value_type&</tt>.
     */
    using reference = value_type&;

    /**
     * @brief Pointer to a @c const sequence element.
     *
     * Pointer to a @c const element in the sequence.
     * Corresponds to <tt>coveo::seq_element_traits::const_value_type*</tt>.
     */
    using const_pointer = const_value_type*;

    /**
     * @brief Reference to a @c const sequence element.
     *
     * Reference to a @c const element in the sequence.
     * Corresponds to <tt>coveo::seq_element_traits::const_value_type&</tt>.
     */
    using const_reference = const_value_type&;
};
/// @cond
template<typename T> struct seq_element_traits<T&> : seq_element_traits<T> { };
template<typename T> struct seq_element_traits<T&&> : seq_element_traits<T> { };
template<typename T> struct seq_element_traits<std::reference_wrapper<T>> : seq_element_traits<T> { };
/// @endcond

/**
 * @brief Traits class for a sequence.
 * @headerfile sequence_util.h <coveo/seq/sequence_util.h>
 *
 * Traits class containing definitions pertaining to a sequence. A shorthand
 * for <tt>coveo::seq_element_traits</tt> that infers the sequence's @c value_type
 * from the return value of its iterators. Also provides the type of iterator
 * used by the sequence.
 *
 * @tparam Seq Type of sequence.
 */
template<typename Seq>
struct seq_traits : public seq_element_traits<decltype(*std::begin(std::declval<Seq&>()))>
{
    /**
     * @brief Type of iterator used by the sequence.
     *
     * Type of iterator used by the sequence, which is defined as the type
     * of iterator returned when calling <tt>std::begin()</tt>.
     */
    using iterator_type = typename std::decay<decltype(std::begin(std::declval<Seq&>()))>::type;
};
/// @cond
template<typename Seq> struct seq_traits<Seq&> : seq_traits<Seq> { };
template<typename Seq> struct seq_traits<Seq&&> : seq_traits<Seq> { };
template<typename Seq> struct seq_traits<std::reference_wrapper<Seq>> : seq_traits<Seq> { };
/// @endcond

#ifdef DOXYGEN_INVOKED
/**
 * @brief Traits class to identify enumerable objects.
 * @headerfile sequence_util.h <coveo/seq/sequence_util.h>
 *
 * Traits class that can be used to identify enumerable objects.
 *
 * @tparam T Type to identify.
 */
template<typename T> struct is_enumerable;
#else
template<typename T> struct is_enumerable : std::false_type { };
template<typename T> struct is_enumerable<enumerable<T>> : std::true_type { };
#endif

} // coveo

#endif // COVEO_SEQUENCE_UTIL_H
