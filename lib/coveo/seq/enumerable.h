/**
 * @file
 * @brief C++ implementation of .NET's IEnumerable-like data structure.
 *
 * This file contains the definition of <tt>coveo::enumerable</tt>, which
 * is a wrapper for a sequence similar to .NET's <tt>IEnumerable&lt;T&gt;</tt>.
 * Also includes helper methods and functions to create <tt>enumerable</tt>s
 * from single values, containers, etc.
 *
 * @copyright 2016-2019, Coveo Solutions Inc.
 *            Distributed under the Apache License, Version 2.0 (see <a href="https://github.com/coveo/linq/blob/master/LICENSE">LICENSE</a>).
 */

#ifndef COVEO_ENUMERABLE_H
#define COVEO_ENUMERABLE_H

#include "coveo/seq/sequence_util.h"
#include <coveo/seq/detail/enumerable_detail.h>

#include <cstddef>
#include <iterator>
#include <functional>
#include <memory>
#include <type_traits>

namespace coveo {

// Wrapper for a multipass, forward-only sequence of elements.
// Uses a delegate to fetch the elements to return.The sequence
// supports iteration via the standard begin/end methods.
template<typename T>
class enumerable
{
    // Allows compatible enumerables to be friend with each other
    template<typename> friend class enumerable;

public:
    // Types for elements in the sequence.
    using value_type        = typename seq_element_traits<T>::value_type;
    using raw_value_type    = typename seq_element_traits<T>::raw_value_type;
    using pointer           = typename seq_element_traits<T>::pointer;
    using reference         = typename seq_element_traits<T>::reference;

    // Delegate that returns next element in sequence, or nullptr when done.
    using next_delegate     = std::function<pointer()>;

    // Delegate that returns number of elements in sequence.
    using size_delegate     = std::function<std::size_t()>;

    // Forward declaration of iterator class.
    class iterator;

    // Even though begin() and end() are already const, some might want
    // to use const_iterator, to let's create an alias.
    using const_iterator    = iterator;

private:
    next_delegate zero_;    // Next delegate which we will clone to iterate sequence.
    size_delegate size_;    // Optional size delegate.

public:
    // Default constructor over empty sequence
    enumerable()
        : zero_([]() -> pointer { return nullptr; }),
          size_([]() -> std::size_t { return 0; }) { }

    // Constructor with next delegate and optional size delegate
    template<typename F,
             typename _S = std::nullptr_t,
             typename = typename std::enable_if<!is_enumerable<typename std::decay<F>::type>::value &&
                                                (!detail::has_begin<typename std::decay<F>::type>::value ||
                                                 !detail::has_end<typename std::decay<F>::type>::value ||
                                                 !detail::has_size_const_method<typename std::decay<F>::type>::value), void>::type>
    enumerable(F&& next, _S&& siz = nullptr)
        : zero_(std::forward<F>(next)), size_(std::forward<_S>(siz)) { }

    // Constructor with container
    template<typename C,
             typename = typename std::enable_if<!is_enumerable<typename std::decay<C>::type>::value &&
                                                detail::has_begin<typename std::decay<C>::type>::value &&
                                                detail::has_end<typename std::decay<C>::type>::value &&
                                                detail::has_size_const_method<typename std::decay<C>::type>::value, void>::type>
    enumerable(C&& cnt)
        : zero_(), size_() { *this = for_container(std::forward<C>(cnt)); }

    // Constructor allowing non-const to const construction
    template<typename U,
             typename = typename std::enable_if<!std::is_const<U>::value &&
                                                std::is_same<T, typename std::add_const<U>::type>::value, void>::type>
    enumerable(enumerable<U> e)
        : zero_(std::move(e.zero_)), size_(std::move(e.size_)) { }

    // Assignment operator for non-const to const assignment
    template<typename U,
             typename = typename std::enable_if<!std::is_const<U>::value &&
                                                std::is_same<T, typename std::add_const<U>::type>::value, void>::type>
    enumerable& operator=(enumerable<U> e) {
        zero_ = std::move(e.zero_);
        size_ = std::move(e.size_);
        return *this;
    }

    // Access to beginning or end of sequence
    iterator begin() const {
        return iterator(*this, false);
    }
    iterator cbegin() const {
        return begin();
    }

    iterator end() const {
        return iterator(*this, true);
    }
    iterator cend() const {
        return end();
    }

    // Access to size of sequence
    bool has_fast_size() const {
        // If we have a delegate, size() should be reasonably fast
        return size_ != nullptr;
    }
    std::size_t size() const {
        // If we have a delegate, use it, otherwise use distance.
        return size_ != nullptr ? size_()
                                : static_cast<std::size_t>(std::distance(begin(), end()));
    }

    // Returns const version of this enumerable
    auto as_const() const -> enumerable<typename std::add_const<T>::type> {
        return enumerable<typename std::add_const<T>::type>(*this);
    }

public:
    // Iterator for the elements in an enumerable's sequence.
    class iterator
    {
    public:
        // Standard iterator typedefs, plus a few more
        using iterator_category = std::forward_iterator_tag;
        using value_type        = typename enumerable<T>::value_type;
        using raw_value_type    = typename enumerable<T>::raw_value_type;
        using difference_type   = std::ptrdiff_t;
        using pointer           = typename enumerable<T>::pointer;
        using reference         = typename enumerable<T>::reference;

    private:
        const enumerable<T>* pparent_ = nullptr;        // Parent enumerable or nullptr if unbound
        mutable next_delegate next_;                    // Delegate used to fetch elements
        mutable pointer pcur_ = nullptr;                // Pointer to current element or nullptr when at end of sequence
        mutable bool has_cur_ = true;                   // Whether pcur_ has been loaded for current element
        size_t pos_ = 0;                                // Position in sequence, to compare iterators

        // Fetches value of pcur_, late-loading it if needed
        pointer get_pcur() const {
            if (!has_cur_) {
                pcur_ = next_();
                has_cur_ = true;
            }
            return pcur_;
        }
    
    public:
        // Default constructor
        iterator() = default;

        // Constructor from enumerable
        iterator(const enumerable<T>& parent, bool is_end)
            : pparent_(&parent), next_(!is_end ? parent.zero_ : next_delegate()), has_cur_(is_end) { }

        // Copy/move semantics
        iterator(const iterator& obj) = default;
        iterator(iterator&& obj)
            : pparent_(obj.pparent_), next_(std::move(obj.next_)), pcur_(obj.pcur_),
              has_cur_(obj.has_cur_), pos_(obj.pos_)
        {
            obj.pparent_ = nullptr;
            obj.pcur_ = nullptr;
            obj.has_cur_ = true;
            obj.pos_ = 0;
        }

        iterator& operator=(const iterator& obj) = default;
        iterator& operator=(iterator&& obj) {
            pparent_ = obj.pparent_;
            next_ = std::move(obj.next_);
            pcur_ = obj.pcur_;
            has_cur_ = obj.has_cur_;
            pos_ = obj.pos_;

            obj.pparent_ = nullptr;
            obj.pcur_ = nullptr;
            obj.has_cur_ = true;
            obj.pos_ = 0;

            return *this;
        }
        
        // Element access
        reference operator*() const {
            return *get_pcur();
        }
        pointer operator->() const {
            return get_pcur();
        }
        
        // Move to next element (pre/post versions)
        iterator& operator++() {
            if (has_cur_) {
                pcur_ = nullptr;
                has_cur_ = false;
            } else {
                next_();
            }
            ++pos_;
            return *this;
        }
        iterator operator++(int) {
            iterator it(*this);
            ++*this;
            return it;
        }
        
        // Iterator comparison
        friend bool operator==(const iterator& left, const iterator& right) {
            return left.pparent_ == right.pparent_ &&
                   (left.get_pcur() == nullptr) == (right.get_pcur() == nullptr) &&
                   (left.get_pcur() == nullptr || left.pos_ == right.pos_);
        }
        friend bool operator!=(const iterator& left, const iterator& right) {
            return !(left == right);
        }
    };

public:
    // Helper static methods

    // Returns enumerable over empty sequence.
    static enumerable<T> empty() {
        return enumerable<T>();
    }

    // Returns enumerable over sequence of one element. Stores the element (moving it if possible).
    template<typename U>
    static enumerable<T> for_one(U&& obj) {
        auto spobj = std::make_shared<value_type>(std::forward<U>(obj));
        bool available = true;
        return enumerable<T>([spobj, available]() mutable {
            pointer pobj = nullptr;
            if (available) {
                pobj = spobj.get();
                available = false;
            }
            return pobj;
        }, []() -> std::size_t {
            return 1;
        });
    }

    // Returns enumerable over sequence of one external element.
    static enumerable<T> for_one_ref(reference obj) {
        bool available = true;
        return enumerable<T>([&obj, available]() mutable {
            pointer pobj = nullptr;
            if (available) {
                pobj = std::addressof(obj);
                available = false;
            }
            return pobj;
        }, []() -> std::size_t {
            return 1;
        });
    }

    // Returns enumerable over sequence bound by two iterators.
    template<typename It>
    static enumerable<T> for_range(It ibeg, It iend) {
        auto it = std::move(ibeg);
        return enumerable<T>([it, iend]() mutable {
            pointer pobj = nullptr;
            if (it != iend) {
                reference robj = *it;
                pobj = std::addressof(robj);
                ++it;
            }
            return pobj;
        }, detail::get_size_delegate_for_iterators(it, iend));
    }

    // Returns enumerable over a container, stored externally.
    template<typename C>
    static enumerable<T> for_container(C& cnt) {
        auto it = std::begin(cnt);
        auto end = std::end(cnt);
        return enumerable<T>([it, end]() mutable {
            pointer pobj = nullptr;
            if (it != end) {
                reference robj = *it;
                pobj = std::addressof(robj);
                ++it;
            }
            return pobj;
        }, [&cnt]() -> std::size_t {
            return cnt.size();
        });
    }

    // Returns enumerable over a container, stored internally (by moving it).
    template<typename C,
             typename = typename std::enable_if<!std::is_reference<C>::value, void>::type>
    static enumerable<T> for_container(C&& cnt) {
        auto spcnt = std::make_shared<C>(std::move(cnt));
        auto it = std::begin(*spcnt);
        auto end = std::end(*spcnt);
        return enumerable<T>([spcnt, it, end]() mutable {
            pointer pobj = nullptr;
            if (it != end) {
                reference robj = *it;
                pobj = std::addressof(robj);
                ++it;
            }
            return pobj;
        }, [spcnt]() -> std::size_t {
            return spcnt->size();
        });
    }

    // Returns enumerable over dynamic array.
    static enumerable<T> for_array(pointer parr, size_t siz) {
        return for_range(parr, parr + siz);
    }
};

// Helper functions corresponding to the helper static methods in enumerable.

// Returns enumerable for sequence of one element, stored internally (moved if possible).
template<typename U>
auto enumerate_one(U&& obj) -> enumerable<typename seq_element_traits<U>::const_value_type> {
    return enumerable<typename seq_element_traits<U>::const_value_type>::for_one(std::forward<U>(obj));
}

// Returns enumerable for sequence of one element, stored externally.
template<typename T>
auto enumerate_one_ref(T& obj) -> enumerable<T> {
    return enumerable<T>::for_one_ref(obj);
}

// Returns enumerable for sequence defined by two iterators.
template<typename It>
auto enumerate_range(It ibeg, It iend)
    -> enumerable<typename seq_element_traits<decltype(*std::declval<It>())>::value_type>
{
    return enumerable<typename seq_element_traits<decltype(*std::declval<It>())>::value_type>::for_range(
        std::move(ibeg), std::move(iend));
}

// Returns enumerable for container, stored externally.
template<typename C>
auto enumerate_container(C& cnt)
    -> enumerable<typename seq_element_traits<decltype(*std::begin(std::declval<C>()))>::value_type>
{
    return enumerable<typename seq_element_traits<decltype(*std::begin(std::declval<C>()))>::value_type>::for_container(
        cnt);
}

// Returns enumerable for container, stored internally (by moving it).
template<typename C,
         typename = typename std::enable_if<!std::is_reference<C>::value, void>::type>
auto enumerate_container(C&& cnt)
    -> enumerable<typename seq_element_traits<decltype(*std::begin(std::declval<C>()))>::const_value_type>
{
    return enumerable<typename seq_element_traits<decltype(*std::begin(std::declval<C>()))>::const_value_type>::for_container(
        std::move(cnt));
}

// Returns enumerable for dynamic array.
template<typename T>
auto enumerate_array(T* parr, size_t siz) -> enumerable<T> {
    return enumerable<T>::for_array(parr, siz);
}

} // coveo

#endif // COVEO_ENUMERABLE_H
