/**
 * @file
 * @copyright 2016-2019, Coveo Solutions Inc.
 *            Distributed under the Apache License, Version 2.0 (see <a href="https://github.com/coveo/linq/blob/master/LICENSE">LICENSE</a>).
 */

#include "coveo/seq/enumerable_tests.h"

#include <coveo/enumerable.h>
#include <coveo/test_framework.h>
#include "coveo/seq/enumerable_test_util.h"

#include <forward_list>
#include <list>
#include <vector>

namespace coveo_tests {
namespace detail {

// Simple struct that cannot be copied.
struct no_copy {
    int i_;

    no_copy(int i)
        : i_(i) { }
    no_copy(const no_copy&) = delete;
    no_copy& operator=(const no_copy&) = delete;
    
    friend bool operator==(const no_copy& a, const no_copy& b) {
        return a.i_ == b.i_;
    }
    friend std::ostream& operator<<(std::ostream& os, const no_copy& obj) {
        return os << obj.i_;
    }
};

} // detail

// empty tests

void test_with_empty_non_const_sequence()
{
    std::vector<int> vempty;
    auto empty_seq = coveo::enumerable<int>::empty();
    validate_enumerable(empty_seq, vempty, should_have_fast_size::yes);
    auto empty_cseq = empty_seq.as_const();
    validate_enumerable(empty_cseq, vempty, should_have_fast_size::yes);
}

void test_with_empty_const_sequence()
{
    const std::vector<int> vempty;
    auto empty_seq = coveo::enumerable<const int>::empty();
    validate_enumerable(empty_seq, vempty, should_have_fast_size::yes);
}

void test_with_empty_sequence()
{
    test_with_empty_non_const_sequence();
    test_with_empty_const_sequence();
}

// next delegate tests

void test_with_next_delegate_non_const()
{
    std::vector<int> vi = { 42 };
    int i = 0;
    auto seq_i = coveo::enumerable<int>([i]() mutable -> int* {
        if (i == 0) {
            i = 42;
            return &i;
        } else {
            return nullptr;
        }
    });
    validate_enumerable(seq_i, vi, should_have_fast_size::no);
    auto seq_ci = seq_i.as_const();
    validate_enumerable(seq_ci, vi, should_have_fast_size::no);
}

void test_with_next_delegate_const()
{
    const std::vector<int> vi = { 42 };
    int i = 0;
    auto seq_i = coveo::enumerable<const int>([i]() mutable -> const int* {
        if (i == 0) {
            i = 42;
            return &i;
        } else {
            return nullptr;
        }
    });
    validate_enumerable(seq_i, vi, should_have_fast_size::no);
}

void test_with_next_delegate()
{
    test_with_next_delegate_non_const();
    test_with_next_delegate_const();
}

// single element tests

void test_with_one_element_non_const_via_helper_method()
{
    std::vector<int> vone = { 42 };
    auto seq_one = coveo::enumerable<int>::for_one(42);
    validate_enumerable(seq_one, vone, should_have_fast_size::yes);
    auto seq_cone = seq_one.as_const();
    validate_enumerable(seq_cone, vone, should_have_fast_size::yes);
}

void test_with_one_element_const_via_helper_method()
{
    const std::vector<int> vone = { 42 };
    auto seq_one = coveo::enumerable<const int>::for_one(42);
    validate_enumerable(seq_one, vone, should_have_fast_size::yes);
}

void test_with_one_element_via_helper_function()
{
    const std::vector<int> vone = { 42 };
    auto seq_one = coveo::enumerate_one(42);
    validate_enumerable(seq_one, vone, should_have_fast_size::yes);
}

void test_with_one_element()
{
    test_with_one_element_non_const_via_helper_method();
    test_with_one_element_const_via_helper_method();
    test_with_one_element_via_helper_function();
}

// single element by ref tests

void test_with_one_element_by_non_const_ref_via_helper_method()
{
    int hangar = 23;
    std::vector<int> vone = { 23 };
    auto seq_one_ref = coveo::enumerable<int>::for_one_ref(hangar);
    validate_enumerable(seq_one_ref, vone, should_have_fast_size::yes);
    auto seq_cone_ref = seq_one_ref.as_const();
    validate_enumerable(seq_cone_ref, vone, should_have_fast_size::yes);
}

void test_with_one_element_by_non_const_ref_via_helper_function()
{
    int hangar = 23;
    std::vector<int> vone = { 23 };
    auto seq_one_ref = coveo::enumerate_one_ref(hangar);
    validate_enumerable(seq_one_ref, vone, should_have_fast_size::yes);
}

void test_with_one_element_by_const_ref_via_helper_method()
{
    const int hangar = 23;
    const std::vector<int> vone = { 23 };
    auto seq_one_ref = coveo::enumerable<const int>::for_one_ref(hangar);
    validate_enumerable(seq_one_ref, vone, should_have_fast_size::yes);
}

void test_with_one_element_by_const_ref_via_helper_function()
{
    const int hangar = 23;
    const std::vector<int> vone = { 23 };
    auto seq_one_ref = coveo::enumerate_one_ref(hangar);
    validate_enumerable(seq_one_ref, vone, should_have_fast_size::yes);
}

void test_with_one_element_by_ref()
{
    test_with_one_element_by_non_const_ref_via_helper_method();
    test_with_one_element_by_non_const_ref_via_helper_function();
    test_with_one_element_by_const_ref_via_helper_method();
    test_with_one_element_by_const_ref_via_helper_function();
}

// iterators range tests

void test_with_non_const_iterators_via_helper_method()
{
    std::vector<int> vforseq = { 42, 23, 66 };
    std::vector<int> vexpected = { 42, 23, 66 };
    auto seq_range = coveo::enumerable<int>::for_range(vforseq.begin(), vforseq.end());
    validate_enumerable(seq_range, vexpected, should_have_fast_size::yes);
    auto seq_crange = seq_range.as_const();
    validate_enumerable(seq_crange, vexpected, should_have_fast_size::yes);
}

void test_with_non_const_iterators_via_helper_function()
{
    std::vector<int> vforseq = { 42, 23, 66 };
    std::vector<int> vexpected = { 42, 23, 66 };
    auto seq_range = coveo::enumerate_range(vforseq.begin(), vforseq.end());
    validate_enumerable(seq_range, vexpected, should_have_fast_size::yes);
}

void test_with_const_iterators_via_helper_method()
{
    const std::vector<int> vforseq = { 42, 23, 66 };
    const std::vector<int> vexpected = { 42, 23, 66 };
    auto seq_range = coveo::enumerable<const int>::for_range(vforseq.begin(), vforseq.end());
    validate_enumerable(seq_range, vexpected, should_have_fast_size::yes);
}

void test_with_const_iterators_via_helper_function()
{
    const std::vector<int> vforseq = { 42, 23, 66 };
    const std::vector<int> vexpected = { 42, 23, 66 };
    auto seq_range = coveo::enumerate_range(vforseq.begin(), vforseq.end());
    validate_enumerable(seq_range, vexpected, should_have_fast_size::yes);
}

void test_with_iterators()
{
    test_with_non_const_iterators_via_helper_method();
    test_with_non_const_iterators_via_helper_function();
    test_with_const_iterators_via_helper_method();
    test_with_const_iterators_via_helper_function();
}

// external container tests

void test_with_non_const_external_container_via_constructor()
{
    std::forward_list<int> flcnt = { 42, 23, 66 };
    std::vector<int> vexpected = { 42, 23, 66 };
    auto seq_cnt = coveo::enumerable<int>(flcnt);
    validate_enumerable(seq_cnt, vexpected, should_have_fast_size::no);
    auto seq_ccnt = seq_cnt.as_const();
    validate_enumerable(seq_ccnt, vexpected, should_have_fast_size::no);
}

void test_with_non_const_external_container_via_helper_method()
{
    std::vector<int> vcnt = { 42, 23, 66 };
    std::vector<int> vexpected = { 42, 23, 66 };
    auto seq_cnt = coveo::enumerable<int>::for_container(vcnt);
    validate_enumerable(seq_cnt, vexpected, should_have_fast_size::yes);
}

void test_with_non_const_external_container_via_helper_function()
{
    std::vector<int> vcnt = { 42, 23, 66 };
    std::vector<int> vexpected = { 42, 23, 66 };
    auto seq_cnt = coveo::enumerate_container(vcnt);
    validate_enumerable(seq_cnt, vexpected, should_have_fast_size::yes);
}

void test_with_const_external_container_via_constructor()
{
    const std::forward_list<int> flcnt = { 42, 23, 66 };
    const std::vector<int> vexpected = { 42, 23, 66 };
    auto seq_cnt = coveo::enumerable<const int>(flcnt);
    validate_enumerable(seq_cnt, vexpected, should_have_fast_size::no);
}

void test_with_const_external_container_via_helper_method()
{
    const std::vector<int> vcnt = { 42, 23, 66 };
    const std::vector<int> vexpected = { 42, 23, 66 };
    auto seq_cnt = coveo::enumerable<const int>::for_container(vcnt);
    validate_enumerable(seq_cnt, vexpected, should_have_fast_size::yes);
}

void test_with_const_external_container_via_helper_function()
{
    const std::vector<int> vcnt = { 42, 23, 66 };
    const std::vector<int> vexpected = { 42, 23, 66 };
    auto seq_cnt = coveo::enumerate_container(vcnt);
    validate_enumerable(seq_cnt, vexpected, should_have_fast_size::yes);
}

void test_with_external_container()
{
    test_with_non_const_external_container_via_constructor();
    test_with_non_const_external_container_via_helper_method();
    test_with_non_const_external_container_via_helper_function();
    test_with_const_external_container_via_constructor();
    test_with_const_external_container_via_helper_method();
    test_with_const_external_container_via_helper_function();
}

// internal container tests

void test_with_internal_container_non_const_via_constructor()
{
    std::vector<int> vexpected = { 42, 23, 66 };
    auto seq_cnt_mv = coveo::enumerable<int>(std::forward_list<int> { 42, 23, 66 });
    validate_enumerable(seq_cnt_mv, vexpected, should_have_fast_size::no);
    auto seq_ccnt_mv = seq_cnt_mv.as_const();
    validate_enumerable(seq_ccnt_mv, vexpected, should_have_fast_size::no);
}

void test_with_internal_container_non_const_via_helper_method()
{
    std::vector<int> vexpected = { 42, 23, 66 };
    auto seq_cnt_mv = coveo::enumerable<int>::for_container(std::vector<int> { 42, 23, 66 });
    validate_enumerable(seq_cnt_mv, vexpected, should_have_fast_size::yes);
    auto seq_ccnt_mv = seq_cnt_mv.as_const();
    validate_enumerable(seq_ccnt_mv, vexpected, should_have_fast_size::yes);
}

void test_with_internal_container_const_via_constructor()
{
    const std::vector<int> vexpected = { 42, 23, 66 };
    auto seq_cnt_mv = coveo::enumerable<const int>(std::forward_list<int> { 42, 23, 66 });
    validate_enumerable(seq_cnt_mv, vexpected, should_have_fast_size::no);
}

void test_with_internal_container_const_via_helper_method()
{
    const std::vector<int> vexpected = { 42, 23, 66 };
    auto seq_cnt_mv = coveo::enumerable<const int>::for_container(std::vector<int> { 42, 23, 66 });
    validate_enumerable(seq_cnt_mv, vexpected, should_have_fast_size::yes);
}

void test_with_internal_container_via_helper_function()
{
    const std::vector<int> vexpected = { 42, 23, 66 };
    auto seq_cnt_mv = coveo::enumerate_container(std::vector<int> { 42, 23, 66 });
    validate_enumerable(seq_cnt_mv, vexpected, should_have_fast_size::yes);
}

void test_with_internal_container()
{
    test_with_internal_container_non_const_via_constructor();
    test_with_internal_container_non_const_via_helper_method();
    test_with_internal_container_const_via_constructor();
    test_with_internal_container_const_via_helper_method();
    test_with_internal_container_via_helper_function();
}

// array tests

void test_with_non_const_array_via_helper_method()
{
    int arr[] = { 42, 23, 66 };
    size_t arr_size = sizeof(arr) / sizeof(arr[0]);
    std::vector<int> vexpected = { 42, 23, 66 };
    auto seq_arr = coveo::enumerable<int>::for_array(arr, arr_size);
    validate_enumerable(seq_arr, vexpected, should_have_fast_size::yes);
    auto seq_carr = seq_arr.as_const();
    validate_enumerable(seq_carr, vexpected, should_have_fast_size::yes);
}

void test_with_non_const_array_via_helper_function()
{
    int arr[] = { 42, 23, 66 };
    size_t arr_size = sizeof(arr) / sizeof(arr[0]);
    std::vector<int> vexpected = { 42, 23, 66 };
    auto seq_arr = coveo::enumerate_array(arr, arr_size);
    validate_enumerable(seq_arr, vexpected, should_have_fast_size::yes);
}

void test_with_const_array_via_helper_method()
{
    const int arr[] = { 42, 23, 66 };
    size_t arr_size = sizeof(arr) / sizeof(arr[0]);
    const std::vector<int> vexpected = { 42, 23, 66 };
    auto seq_arr = coveo::enumerable<const int>::for_array(arr, arr_size);
    validate_enumerable(seq_arr, vexpected, should_have_fast_size::yes);
}

void test_with_const_array_via_helper_function()
{
    const int arr[] = { 42, 23, 66 };
    size_t arr_size = sizeof(arr) / sizeof(arr[0]);
    const std::vector<int> vexpected = { 42, 23, 66 };
    auto seq_arr = coveo::enumerate_array(arr, arr_size);
    validate_enumerable(seq_arr, vexpected, should_have_fast_size::yes);
}

void test_with_array()
{
    test_with_non_const_array_via_helper_method();
    test_with_non_const_array_via_helper_function();
    test_with_const_array_via_helper_method();
    test_with_const_array_via_helper_function();
}

// non-copyable object tests

void test_with_non_const_non_copyable_object()
{
    detail::no_copy an_obj(42);
    bool avail = true;
    auto seq = coveo::enumerable<detail::no_copy>([&an_obj, avail]() mutable {
        detail::no_copy* pobj = nullptr;
        if (avail) {
            pobj = &an_obj;
            avail = false;
        }
        return pobj;
    });
    std::list<detail::no_copy> lexpected;
    lexpected.emplace_back(42);
    validate_enumerable(seq, lexpected, should_have_fast_size::no);
    auto cseq = seq.as_const();
    validate_enumerable(cseq, lexpected, should_have_fast_size::no);
}

void test_with_const_non_copyable_object()
{
    const detail::no_copy an_obj(42);
    bool avail = true;
    auto seq = coveo::enumerable<const detail::no_copy>([&an_obj, avail]() mutable {
        const detail::no_copy* pobj = nullptr;
        if (avail) {
            pobj = &an_obj;
            avail = false;
        }
        return pobj;
    });
    const std::list<detail::no_copy> lexpected = []() {
        std::list<detail::no_copy> l;
        l.emplace_back(42);
        return l;
    }();
    validate_enumerable(seq, lexpected, should_have_fast_size::no);
}

void test_with_non_copyable_object()
{
    test_with_non_const_non_copyable_object();
    test_with_const_non_copyable_object();
}

// non-const -> const tests

void test_non_const_to_const_conversion()
{
    auto validat = [](const coveo::enumerable<const int>& seq) {
        const std::vector<int> vexpected = { 42, 23, 66 };
        validate_enumerable(seq, vexpected, should_have_fast_size::yes);
    };
    std::vector<int> vforseq = { 42, 23, 66 };
    auto seq = coveo::enumerable<int>(vforseq);
    validat(seq);
    coveo::enumerable<const int> cseq(std::move(seq));
    validat(cseq);
    seq = coveo::enumerable<int>(vforseq);
    cseq = seq;
    validat(cseq);
    cseq = std::move(seq);
    validat(cseq);
}

// all tests for coveo::enumerable

void enumerable_tests()
{
    test_with_empty_sequence();
    test_with_next_delegate();
    test_with_one_element();
    test_with_one_element_by_ref();
    test_with_iterators();
    test_with_external_container();
    test_with_internal_container();
    test_with_array();
    test_with_non_copyable_object();
    test_non_const_to_const_conversion();
}

} // coveo_tests
