// Copyright (c) 2019, Coveo Solutions Inc.
// Distributed under the MIT license (see LICENSE).

#ifndef COVEO_ENUMERABLE_TEST_UTIL_H
#define COVEO_ENUMERABLE_TEST_UTIL_H

#include <coveo/enumerable.h>
#include <coveo/test_framework.h>

#include <iterator>

namespace coveo_tests {

// Compares enumerable with content of container
enum class should_have_fast_size { no, yes };
template<typename E, typename C>
void validate_enumerable(const E& seq, const C& expected, const should_have_fast_size expect_fast_size) {
    auto eit = std::begin(expected);
    auto eend = std::end(expected);
    for (auto&& obj : seq) {
        COVEO_ASSERT(eit != eend);
        COVEO_ASSERT_EQUAL(*eit++, obj);
    }
    COVEO_ASSERT(eit == eend);
    eit = std::begin(expected);
    for (auto sit = seq.cbegin(), send = seq.cend(); sit != send; ++sit) {
        COVEO_ASSERT(eit != eend);
        COVEO_ASSERT_EQUAL(*eit++, *sit);
    }
    COVEO_ASSERT(eit == eend);
    COVEO_ASSERT_EQUAL(expect_fast_size == should_have_fast_size::yes, seq.has_fast_size());
    COVEO_ASSERT_EQUAL(expected.size(), seq.size());
}

} // coveo_tests

#endif // COVEO_ENUMERABLE_TEST_UTIL_H
