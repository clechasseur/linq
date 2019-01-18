// Copyright (c) 2016, Coveo Solutions Inc.
// Distributed under the MIT license (see LICENSE).

#ifndef COVEO_SEQUENCE_UTIL_H
#define COVEO_SEQUENCE_UTIL_H

#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>

namespace coveo {

// Forward declaration of enumerable, for type traits
template<typename> class enumerable;

// Traits class for elements in a sequence.  For sequences of references or
// std::reference_wrapper's, provides information about the referred type instead.
template<typename T>
struct seq_element_traits
{
    using value_type        = T;
    using const_value_type  = const value_type;
    using raw_value_type    = typename std::remove_cv<value_type>::type;

    using pointer           = value_type*;
    using reference         = value_type&;

    using const_pointer     = const_value_type*;
    using const_reference   = const_value_type&;
};
template<typename T> struct seq_element_traits<T&> : seq_element_traits<T> { };
template<typename T> struct seq_element_traits<T&&> : seq_element_traits<T> { };
template<typename T> struct seq_element_traits<std::reference_wrapper<T>> : seq_element_traits<T> { };

// Traits class for a sequence. A shorthand for seq_element_traits that infers the
// sequence's value_type from the return value of its iterators. Also provides the
// type of iterator used by the sequence.
template<typename Seq>
struct seq_traits : public seq_element_traits<decltype(*std::begin(std::declval<Seq&>()))>
{
    using iterator_type = typename std::decay<decltype(std::begin(std::declval<Seq&>()))>::type;
};
template<typename Seq> struct seq_traits<Seq&> : seq_traits<Seq> { };
template<typename Seq> struct seq_traits<Seq&&> : seq_traits<Seq> { };
template<typename Seq> struct seq_traits<std::reference_wrapper<Seq>> : seq_traits<Seq> { };

// Traits class used to identify enumerable objects.
template<typename> struct is_enumerable : std::false_type { };
template<typename T> struct is_enumerable<enumerable<T>> : std::true_type { };

} // coveo

#endif // COVEO_SEQUENCE_UTIL_H
