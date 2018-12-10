// Copyright (c) 2016, Coveo Solutions Inc.
// Distributed under the MIT license (see LICENSE).

#include "coveo/linq/linq_tests.h"

#include <coveo/enumerable.h>
#include <coveo/linq.h>
#include <coveo/test_framework.h>

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <cstdint>
#include <forward_list>
#include <functional>
#include <iostream>
#include <random>
#include <sstream>
#include <vector>

namespace coveo_tests {
namespace linq {
namespace detail {

// Implementation of equal with four iterators like in C++14
template<typename It1, typename It2>
bool equal(It1 ibeg1, It1 iend1, It2 ibeg2, It2 iend2) {
    return std::distance(ibeg1, iend1) == std::distance(ibeg2, iend2) &&
           std::equal(ibeg1, iend1, ibeg2);
};

// Pair-like struct containing two ints, comparing with first only.
struct two_ints {
    int first;
    int second;
    
    two_ints(int i = 0) : first(i), second(i) { }

    friend bool operator<(const two_ints& a, const two_ints& b) {
        return a.first < b.first;
    }
    friend bool operator==(const two_ints& a, const two_ints& b) {
        return a.first == b.first;
    }
};

// Compares a sequence of two_int's second members with a sequence of ints.
template<typename TwoIntsSeq, typename IntSeq>
bool seq_second_equal(const TwoIntsSeq& tiseq, const IntSeq& iseq) {
    auto ticur = std::begin(tiseq);
    auto tiend = std::end(tiseq);
    auto icur = std::begin(iseq);
    auto iend = std::end(iseq);
    bool eq = std::distance(ticur, tiend) == std::distance(icur, iend);
    for (; eq && ticur != tiend && icur != iend; ++ticur, ++icur) {
        eq = ticur->second == *icur;
    }
    return eq;
}

} // detail

// Run all tests for coveo::linq operators
void linq_tests()
{
    // from
    {
        const auto v = { 42, 23, 66 };
        const std::vector<int> expected = { 42, 23, 66 };

        using namespace coveo::linq;
        auto seq = from(v);
        COVEO_ASSERT(detail::equal(std::begin(seq), std::end(seq),
                                   std::begin(expected), std::end(expected)));
    }

    // from_range
    {
        const auto v = { 42, 23, 66 };
        const std::vector<int> expected = { 42, 23, 66 };

        using namespace coveo::linq;
        auto seq = from_range(std::begin(v), std::end(v));
        COVEO_ASSERT(detail::equal(std::begin(seq), std::end(seq),
                                   std::begin(expected), std::end(expected)));
    }

    // from_int_range
    {
        const std::vector<int> expected = { 42, 43, 44, 45, 46, 47, 48 };

        using namespace coveo::linq;
        auto seq = from_int_range(42, 7);
        COVEO_ASSERT(detail::equal(std::begin(seq), std::end(seq),
                                   std::begin(expected), std::end(expected)));
    }

    // from_repeated
    {
        const std::vector<int> expected = { 42, 42, 42, 42, 42, 42, 42 };

        using namespace coveo::linq;
        auto seq = from_repeated(42, 7);
        COVEO_ASSERT(detail::equal(std::begin(seq), std::end(seq),
                                   std::begin(expected), std::end(expected)));
    }

    // aggregate
    {
        const std::vector<int> v = { 42, 23, 66 };

        using namespace coveo::linq;
        auto agg = from(v)
                 | aggregate([](int a, int b) { return a + b; });
        COVEO_ASSERT(agg == 131);
    }
    {
        const std::vector<int> ev;

        using namespace coveo::linq;
        COVEO_ASSERT_THROW(from(ev)
                         | aggregate([](int a, int b) { return a + b; }));
    }
    {
        const char STR[] = "world!";

        using namespace coveo::linq;
        auto agg = from(coveo::enumerate_array(STR, 6))
                 | aggregate(std::string("Hello, "),
                             [](const std::string& agg, char c) { return agg + c; });
        COVEO_ASSERT(agg == "Hello, world!");
    }
    {
        const char NUMS[] = "31337";

        using namespace coveo::linq;
        auto agg = from(coveo::enumerate_array(NUMS, 5))
                 | aggregate(std::string(""),
                             [](const std::string& agg, char c) { return agg + c; },
                             [](const std::string& agg) {
                                 std::istringstream iss(agg);
                                 int n = 0;
                                 iss >> n;
                                 return n;
                             });
        COVEO_ASSERT(agg == 31337);
    }

    // all
    {
        const std::vector<int> v = { 42, 23, 66 };

        using namespace coveo::linq;
        COVEO_ASSERT(from(v) | all([](int i) { return i > 11; }));
        COVEO_ASSERT(!(from(v) | all([](int i) { return i % 2 == 0; })));
    }

    // any
    {
        std::vector<int> v = { 42, 23, 66 };

        using namespace coveo::linq;
        COVEO_ASSERT(from(v) | any());

        v.clear();
        COVEO_ASSERT(!(from(v) | any()));
    }

    // average
    {
        const std::vector<int> v = { 42, 23, 66 };

        using namespace coveo::linq;
        auto avg_int = from(v)
                     | average([](int i) { return i; });
        COVEO_ASSERT(avg_int == 43);

        auto avg_dbl = from(v)
                     | average([](int i) { return double(i); });
        COVEO_ASSERT(avg_dbl >= 43.66 && avg_dbl < 43.67);
    }
    {
        const std::vector<int> ev;

        using namespace coveo::linq;
        COVEO_ASSERT_THROW(from(ev)
                         | average([](int i) { return i; }));
    }

    // cast
    {
        const std::vector<int> vi = { 42, 23, 66 };
        const std::vector<double> vd = { 42.0, 23.0, 66.0 };

        using namespace coveo::linq;
        auto seq_d = from(vi)
                   | cast<double>();
        COVEO_ASSERT(detail::equal(std::begin(seq_d), std::end(seq_d),
                                   std::begin(vd), std::end(vd)));
        COVEO_ASSERT(seq_d.has_fast_size());
        COVEO_ASSERT(seq_d.size() == vd.size());
    }
    {
        const std::vector<double> vd = { 42.0, 23.0, 66.0 };

        using namespace coveo::linq;
        auto seq_d = from(std::forward_list<int>{ 42, 23, 66 })
                   | cast<double>();
        COVEO_ASSERT(detail::equal(std::begin(seq_d), std::end(seq_d),
                                   std::begin(vd), std::end(vd)));
        COVEO_ASSERT(!seq_d.has_fast_size());
        COVEO_ASSERT(seq_d.size() == vd.size());
    }

    // concat
    {
        const std::vector<int> a = { 42, 23 };
        const std::vector<int> b = { 66, 67 };
        const std::vector<int> ab = { 42, 23, 66, 67, 11, 7 };

        using namespace coveo::linq;
        auto all = from(a)
                 | concat(b)
                 | concat(std::vector<int>{ 11, 7 });
        COVEO_ASSERT(detail::equal(std::begin(all), std::end(all),
                                   std::begin(ab), std::end(ab)));
        COVEO_ASSERT(all.has_fast_size());
        COVEO_ASSERT(all.size() == ab.size());
    }
    {
        std::vector<int> v1 = { 42, 23 };
        std::vector<int> v2 = { 66, 67 };
        const std::vector<int> expected = { 43, 24, 67, 68, 12, 8 };

        using namespace coveo::linq;
        auto all = from(v1)
                 | concat(v2)
                 | concat(std::vector<int>{ 11, 7 });
        for (int& i : all) {
            ++i;
        }
        COVEO_ASSERT(detail::equal(std::begin(all), std::end(all),
                                   std::begin(expected), std::end(expected)));
    }

    // contains
    {
        const std::vector<int> v = { 42, 23, 66 };

        using namespace coveo::linq;
        COVEO_ASSERT(from(v) | contains(23));

        auto eq_str_int = [](int i, const std::string& s) {
            std::ostringstream oss;
            oss << i;
            return oss.str() == s;
        };
        COVEO_ASSERT(from(v) | contains("23", eq_str_int));
    }

    // count
    {
        const std::vector<int> v = { 42, 23, 66 };

        using namespace coveo::linq;
        COVEO_ASSERT((from(v) | count()) == 3);
        COVEO_ASSERT((from(v) | count([](int i) { return i % 2 == 0; })) == 2);
    }

    // default_if_empty
    {
        const std::vector<int> v;
        const std::vector<int> v_def = { 0 };
        const std::vector<int> not_v = { 42 };

        using namespace coveo::linq;
        auto def = from(v)
                 | default_if_empty();
        COVEO_ASSERT(detail::equal(std::begin(def), std::end(def),
                                   std::begin(v_def), std::end(v_def)));

        auto def_n = from(v)
                   | default_if_empty(42);
        COVEO_ASSERT(detail::equal(std::begin(def_n), std::end(def_n),
                                   std::begin(not_v), std::end(not_v)));
    }

    // distinct
    {
        const std::vector<int> v = { 42, 23, 66, 42, 67, 66, 23, 11 };
        const std::vector<int> v2 = { 42, 23, 66, 42, 67, 66, 23, 11 };
        const std::vector<int> v_no_dup = { 42, 23, 66, 67, 11 };

        using namespace coveo::linq;
        auto no_dup = from(v)
                    | distinct();
        COVEO_ASSERT(detail::equal(std::begin(no_dup), std::end(no_dup),
                                   std::begin(v_no_dup), std::end(v_no_dup)));
        COVEO_ASSERT(!no_dup.has_fast_size());
        COVEO_ASSERT(no_dup.size() == v_no_dup.size());

        auto no_dup_2 = from(v2)
                      | distinct([](int i, int j) { return i > j; });
        COVEO_ASSERT(detail::equal(std::begin(no_dup_2), std::end(no_dup_2),
                                   std::begin(v_no_dup), std::end(v_no_dup)));
        COVEO_ASSERT(!no_dup_2.has_fast_size());
        COVEO_ASSERT(no_dup_2.size() == v_no_dup.size());
    }
    {
        std::vector<detail::two_ints> v = { 42, 23, 66, 42, 67, 66, 23, 11 };
        const std::vector<int> expected = { 84, 46, 132, 42, 134, 66, 23, 22 };

        using namespace coveo::linq;
        auto no_dup = from(v)
                    | distinct();
        for (auto&& ti : no_dup) {
            ti.second *= 2;
        }
        COVEO_ASSERT(detail::seq_second_equal(v, expected));
    }
    {
        const std::vector<int> v_no_dup = { 42, 23, 66, 67, 11 };

        using namespace coveo::linq;
        auto no_dup = from(std::forward_list<int>{ 42, 23, 66, 42, 67, 66, 23, 11 })
                    | distinct();
        COVEO_ASSERT(detail::equal(std::begin(no_dup), std::end(no_dup),
                                   std::begin(v_no_dup), std::end(v_no_dup)));
        COVEO_ASSERT(!no_dup.has_fast_size());
        COVEO_ASSERT(no_dup.size() == v_no_dup.size());
    }

    // element_at
    {
        const std::vector<int> v = { 42, 23, 66 };

        using namespace coveo::linq;
        COVEO_ASSERT((from(v) | element_at(1)) == 23);
        COVEO_ASSERT_THROW(from(v) | element_at(3));
    }
    {
        std::vector<int> v = { 42, 23, 66 };

        using namespace coveo::linq;
        (from(v) | element_at(1)) *= 2;
        COVEO_ASSERT(v[1] == 46);
    }

    // element_at_or_default
    {
        const std::vector<int> v = { 42, 23, 66 };

        using namespace coveo::linq;
        COVEO_ASSERT((from(v) | element_at_or_default(1)) == 23);
        COVEO_ASSERT((from(v) | element_at_or_default(3)) == 0);
    }

    // except
    {
        const std::vector<int> v = { 42, 23, 66, 42, 23, 67, 11, 66, 7 };
        const std::vector<int> not_v = { 66, 23, 11 };
        const std::vector<int> res = { 42, 42, 67, 7 };

        using namespace coveo::linq;
        auto lres = from(v)
                  | except(not_v);
        COVEO_ASSERT(detail::equal(std::begin(lres), std::end(lres),
                                   std::begin(res), std::end(res)));
        COVEO_ASSERT(!lres.has_fast_size());
        COVEO_ASSERT(lres.size() == res.size());
    }
    {
        std::vector<detail::two_ints> v = { 42, 23, 66, 42, 23, 67, 11, 66, 7 };
        const std::vector<detail::two_ints> not_v = { 66, 23, 11 };
        const std::vector<int> expected = { 84, 23, 66, 84, 23, 134, 11, 66, 14 };

        using namespace coveo::linq;
        auto lres = from(v)
                  | except(not_v);
        for (auto&& ti : lres) {
            ti.second *= 2;
        }
        COVEO_ASSERT(detail::seq_second_equal(v, expected));
    }
    {
        const std::vector<int> res = { 42, 42, 67, 7 };

        using namespace coveo::linq;
        auto lres = from(std::forward_list<int>{ 42, 23, 66, 42, 23, 67, 11, 66, 7 })
                  | except(std::forward_list<int>{ 66, 23, 11 });
        COVEO_ASSERT(detail::equal(std::begin(lres), std::end(lres),
                                   std::begin(res), std::end(res)));
        COVEO_ASSERT(!lres.has_fast_size());
        COVEO_ASSERT(lres.size() == res.size());
    }

    // first
    {
        const std::vector<int> v = { 42, 23, 66 };

        using namespace coveo::linq;
        COVEO_ASSERT((from(v)
                    | first()) == 42);
        COVEO_ASSERT((from(v)
                    | first([](int i) { return i % 2 != 0; })) == 23);
    }
    {
        std::vector<int> v = { 42, 23, 66 };
        const std::vector<int> expected = { 43, 22, 66 };

        using namespace coveo::linq;
        (from(v) | first([](int i) { return i % 2 != 0; })) -= 1;
        (from(v) | first()) += 1;
        COVEO_ASSERT(detail::equal(std::begin(v), std::end(v),
                                   std::begin(expected), std::end(expected)));
    }

    // first_or_default
    {
        const std::vector<int> v = { 42, 23, 66 };
        const std::vector<int> e;

        using namespace coveo::linq;
        COVEO_ASSERT((from(v)
                    | first_or_default()) == 42);
        COVEO_ASSERT((from(e)
                    | first_or_default()) == 0);
        COVEO_ASSERT((from(v)
                    | first_or_default([](int i) { return i > 60; })) == 66);
        COVEO_ASSERT((from(v)
                    | first_or_default([](int i) { return i > 100; })) == 0);
    }

    // group_by, group_values_by, group_by_and_fold, group_values_by_and_fold
    {
        enum class oddity { odd = 1, even = 2 };
        const std::vector<int> v = { 42, 23, 66, 11, 7 };
        const std::vector<int> odd_group = { 23, 11, 7 };
        const std::vector<int> even_group = { 42, 66 };

        using namespace coveo::linq;
        auto groups = from(v)
                    | group_by([](int i) { return i % 2 == 0 ? oddity::even : oddity::odd; });
        auto icurg = std::begin(groups);
        auto iendg = std::end(groups);
        COVEO_ASSERT(icurg != iendg);
        COVEO_ASSERT(std::get<0>(*icurg) == oddity::odd);
        COVEO_ASSERT(detail::equal(std::begin(std::get<1>(*icurg)), std::end(std::get<1>(*icurg)),
                                   std::begin(odd_group), std::end(odd_group)));
        ++icurg;
        COVEO_ASSERT(icurg != iendg);
        COVEO_ASSERT(std::get<0>(*icurg) == oddity::even);
        COVEO_ASSERT(detail::equal(std::begin(std::get<1>(*icurg)), std::end(std::get<1>(*icurg)),
                                   std::begin(even_group), std::end(even_group)));
        ++icurg;
        COVEO_ASSERT(icurg == iendg);
        COVEO_ASSERT(!groups.has_fast_size());
        COVEO_ASSERT(groups.size() == 2);
    }
    {
        enum class oddity { odd = 1, even = 2 };
        const std::vector<int> v = { 42, 23, 66, 11, 7 };
        const std::vector<int> odd_group = { 23, 11, 7 };
        const std::vector<int> even_group = { 42, 66 };

        using namespace coveo::linq;
        auto groups = from(v)
                    | group_by([](int i) { return i % 2 == 0 ? oddity::even : oddity::odd; },
                               coveo::linq::detail::greater<>());
        auto icurg = std::begin(groups);
        auto iendg = std::end(groups);
        COVEO_ASSERT(icurg != iendg);
        COVEO_ASSERT(std::get<0>(*icurg) == oddity::even);
        COVEO_ASSERT(detail::equal(std::begin(std::get<1>(*icurg)), std::end(std::get<1>(*icurg)),
                                   std::begin(even_group), std::end(even_group)));
        ++icurg;
        COVEO_ASSERT(icurg != iendg);
        COVEO_ASSERT(std::get<0>(*icurg) == oddity::odd);
        COVEO_ASSERT(detail::equal(std::begin(std::get<1>(*icurg)), std::end(std::get<1>(*icurg)),
                                   std::begin(odd_group), std::end(odd_group)));
        ++icurg;
        COVEO_ASSERT(icurg == iendg);
        COVEO_ASSERT(!groups.has_fast_size());
        COVEO_ASSERT(groups.size() == 2);
    }
    {
        enum class oddity { odd = 1, even = 2 };
        const std::vector<int> odd_group = { 23, 11, 7 };
        const std::vector<int> even_group = { 42, 66 };

        using namespace coveo::linq;
        auto groups = from(std::forward_list<int>{ 42, 23, 66, 11, 7 })
                    | group_by([](int i) { return i % 2 == 0 ? oddity::even : oddity::odd; });
        auto icurg = std::begin(groups);
        auto iendg = std::end(groups);
        COVEO_ASSERT(icurg != iendg);
        COVEO_ASSERT(std::get<0>(*icurg) == oddity::odd);
        COVEO_ASSERT(detail::equal(std::begin(std::get<1>(*icurg)), std::end(std::get<1>(*icurg)),
                                   std::begin(odd_group), std::end(odd_group)));
        ++icurg;
        COVEO_ASSERT(icurg != iendg);
        COVEO_ASSERT(std::get<0>(*icurg) == oddity::even);
        COVEO_ASSERT(detail::equal(std::begin(std::get<1>(*icurg)), std::end(std::get<1>(*icurg)),
                                   std::begin(even_group), std::end(even_group)));
        ++icurg;
        COVEO_ASSERT(icurg == iendg);
        COVEO_ASSERT(!groups.has_fast_size());
        COVEO_ASSERT(groups.size() == 2);
    }
    {
        enum class oddity { odd = 1, even = 2 };
        const std::vector<int> v = { 42, 23, 66, 11, 7 };
        const std::vector<int> odd_group = { 230, 110, 70 };
        const std::vector<int> even_group = { 420, 660 };

        using namespace coveo::linq;
        auto groups = from(v)
                    | group_values_by([](int i) { return i % 2 == 0 ? oddity::even : oddity::odd; },
                                      [](int i) { return i * 10; });
        auto icurg = std::begin(groups);
        auto iendg = std::end(groups);
        COVEO_ASSERT(icurg != iendg);
        COVEO_ASSERT(std::get<0>(*icurg) == oddity::odd);
        COVEO_ASSERT(detail::equal(std::begin(std::get<1>(*icurg)), std::end(std::get<1>(*icurg)),
                                   std::begin(odd_group), std::end(odd_group)));
        ++icurg;
        COVEO_ASSERT(icurg != iendg);
        COVEO_ASSERT(std::get<0>(*icurg) == oddity::even);
        COVEO_ASSERT(detail::equal(std::begin(std::get<1>(*icurg)), std::end(std::get<1>(*icurg)),
                                   std::begin(even_group), std::end(even_group)));
        ++icurg;
        COVEO_ASSERT(icurg == iendg);
        COVEO_ASSERT(!groups.has_fast_size());
        COVEO_ASSERT(groups.size() == 2);
    }
    {
        enum class oddity { odd = 1, even = 2 };
        const std::vector<int> v = { 42, 23, 66, 11, 7 };
        const std::vector<int> odd_group = { 230, 110, 70 };
        const std::vector<int> even_group = { 420, 660 };

        using namespace coveo::linq;
        auto groups = from(v)
                    | group_values_by([](int i) { return i % 2 == 0 ? oddity::even : oddity::odd; },
                                      [](int i) { return i * 10; },
                                      coveo::linq::detail::greater<>());
        auto icurg = std::begin(groups);
        auto iendg = std::end(groups);
        COVEO_ASSERT(icurg != iendg);
        COVEO_ASSERT(std::get<0>(*icurg) == oddity::even);
        COVEO_ASSERT(detail::equal(std::begin(std::get<1>(*icurg)), std::end(std::get<1>(*icurg)),
                                   std::begin(even_group), std::end(even_group)));
        ++icurg;
        COVEO_ASSERT(icurg != iendg);
        COVEO_ASSERT(std::get<0>(*icurg) == oddity::odd);
        COVEO_ASSERT(detail::equal(std::begin(std::get<1>(*icurg)), std::end(std::get<1>(*icurg)),
                                   std::begin(odd_group), std::end(odd_group)));
        ++icurg;
        COVEO_ASSERT(icurg == iendg);
        COVEO_ASSERT(!groups.has_fast_size());
        COVEO_ASSERT(groups.size() == 2);
    }
    {
        enum class oddity { odd = 1, even = 2 };
        const std::vector<int> odd_group = { 230, 110, 70 };
        const std::vector<int> even_group = { 420, 660 };

        using namespace coveo::linq;
        auto groups = from(std::forward_list<int>{ 42, 23, 66, 11, 7 })
                    | group_values_by([](int i) { return i % 2 == 0 ? oddity::even : oddity::odd; },
                                      [](int i) { return i * 10; });
        auto icurg = std::begin(groups);
        auto iendg = std::end(groups);
        COVEO_ASSERT(icurg != iendg);
        COVEO_ASSERT(std::get<0>(*icurg) == oddity::odd);
        COVEO_ASSERT(detail::equal(std::begin(std::get<1>(*icurg)), std::end(std::get<1>(*icurg)),
                                   std::begin(odd_group), std::end(odd_group)));
        ++icurg;
        COVEO_ASSERT(icurg != iendg);
        COVEO_ASSERT(std::get<0>(*icurg) == oddity::even);
        COVEO_ASSERT(detail::equal(std::begin(std::get<1>(*icurg)), std::end(std::get<1>(*icurg)),
                                   std::begin(even_group), std::end(even_group)));
        ++icurg;
        COVEO_ASSERT(icurg == iendg);
        COVEO_ASSERT(!groups.has_fast_size());
        COVEO_ASSERT(groups.size() == 2);
    }
    {
        enum class oddity { odd = 1, even = 2 };
        const std::vector<int> v = { 42, 23, 66, 11, 7 };
        const std::vector<int> size_by_oddity_v = { 3, 2 };
        using namespace coveo::linq;
        auto res = from(v)
                 | group_by_and_fold([](int i) { return i % 2 == 0 ? oddity::even : oddity::odd; },
                                     [](oddity, const coveo::enumerable<const int>& nums) {
                                         return std::distance(std::begin(nums), std::end(nums));
                                     });
        COVEO_ASSERT(detail::equal(std::begin(res), std::end(res),
                                   std::begin(size_by_oddity_v), std::end(size_by_oddity_v)));
        COVEO_ASSERT(!res.has_fast_size());
        COVEO_ASSERT(res.size() == size_by_oddity_v.size());
    }
    {
        enum class oddity { odd = 1, even = 2 };
        const std::vector<int> v = { 42, 23, 66, 11, 7 };
        const std::vector<int> size_by_oddity_v = { 2, 3 };
        using namespace coveo::linq;
        auto res = from(v)
                 | group_by_and_fold([](int i) { return i % 2 == 0 ? oddity::even : oddity::odd; },
                                     [](oddity, const coveo::enumerable<const int>& nums) {
                                         return std::distance(std::begin(nums), std::end(nums));
                                     },
                                     coveo::linq::detail::greater<>());
        COVEO_ASSERT(detail::equal(std::begin(res), std::end(res),
                                   std::begin(size_by_oddity_v), std::end(size_by_oddity_v)));
        COVEO_ASSERT(!res.has_fast_size());
        COVEO_ASSERT(res.size() == size_by_oddity_v.size());
    }
    {
        enum class oddity { odd = 1, even = 2 };
        const std::vector<int> size_by_oddity_v = { 3, 2 };
        using namespace coveo::linq;
        auto res = from(std::forward_list<int>{ 42, 23, 66, 11, 7 })
                 | group_by_and_fold([](int i) { return i % 2 == 0 ? oddity::even : oddity::odd; },
                                     [](oddity, const coveo::enumerable<const int>& nums) {
                                         return std::distance(std::begin(nums), std::end(nums));
                                     });
        COVEO_ASSERT(detail::equal(std::begin(res), std::end(res),
                                   std::begin(size_by_oddity_v), std::end(size_by_oddity_v)));
        COVEO_ASSERT(!res.has_fast_size());
        COVEO_ASSERT(res.size() == size_by_oddity_v.size());
    }
    {
        enum class oddity { odd = 1, even = 2 };
        const std::vector<int> v = { 42, 23, 66, 11, 7 };
        const std::vector<int> somewhat_size_by_oddity_v = { 233, 422 };
        using namespace coveo::linq;
        auto res = from(v)
                 | group_values_by_and_fold([](int i) { return i % 2 == 0 ? oddity::even : oddity::odd; },
                                            [](int i) { return i * 10; },
                                            [](oddity, const coveo::enumerable<const int>& nums) {
                                                return std::distance(std::begin(nums), std::end(nums)) + *std::begin(nums);
                                            });
        COVEO_ASSERT(detail::equal(std::begin(res), std::end(res),
                                   std::begin(somewhat_size_by_oddity_v), std::end(somewhat_size_by_oddity_v)));
        COVEO_ASSERT(!res.has_fast_size());
        COVEO_ASSERT(res.size() == somewhat_size_by_oddity_v.size());
    }
    {
        enum class oddity { odd = 1, even = 2 };
        const std::vector<int> v = { 42, 23, 66, 11, 7 };
        const std::vector<int> somewhat_size_by_oddity_v = { 422, 233 };
        using namespace coveo::linq;
        auto res = from(v)
                 | group_values_by_and_fold([](int i) { return i % 2 == 0 ? oddity::even : oddity::odd; },
                                            [](int i) { return i * 10; },
                                            [](oddity, const coveo::enumerable<const int>& nums) {
                                                return std::distance(std::begin(nums), std::end(nums)) + *std::begin(nums);
                                            },
                                            coveo::linq::detail::greater<>());
        COVEO_ASSERT(detail::equal(std::begin(res), std::end(res),
                                   std::begin(somewhat_size_by_oddity_v), std::end(somewhat_size_by_oddity_v)));
        COVEO_ASSERT(!res.has_fast_size());
        COVEO_ASSERT(res.size() == somewhat_size_by_oddity_v.size());
    }
    {
        enum class oddity { odd = 1, even = 2 };
        const std::vector<int> somewhat_size_by_oddity_v = { 233, 422 };
        using namespace coveo::linq;
        auto res = from(std::forward_list<int>{ 42, 23, 66, 11, 7 })
                 | group_values_by_and_fold([](int i) { return i % 2 == 0 ? oddity::even : oddity::odd; },
                                            [](int i) { return i * 10; },
                                            [](oddity, const coveo::enumerable<const int>& nums) {
                                                return std::distance(std::begin(nums), std::end(nums)) + *std::begin(nums);
                                            });
        COVEO_ASSERT(detail::equal(std::begin(res), std::end(res),
                                   std::begin(somewhat_size_by_oddity_v), std::end(somewhat_size_by_oddity_v)));
        COVEO_ASSERT(!res.has_fast_size());
        COVEO_ASSERT(res.size() == somewhat_size_by_oddity_v.size());
    }

    // group_join
    {
        enum class oddity { odd = 1, even = 2 };
        const std::vector<int> out_v = { 42, 23, 66 };
        const std::vector<int> in_v = { 11, 7, 6, 66, 9, 22 };
        const std::vector<int> in_odd_v = { 11, 7, 9 };
        const std::vector<int> in_even_v = { 6, 66, 22 };
        const std::vector<std::tuple<int, const std::vector<int>&>> expected = {
            std::make_tuple(42, std::ref(in_even_v)),
            std::make_tuple(23, std::ref(in_odd_v)),
            std::make_tuple(66, std::ref(in_even_v)),
        };
        auto get_oddity = [](int i) { return i % 2 == 0 ? oddity::even : oddity::odd; };

        using namespace coveo::linq;
        auto res = from(out_v)
                 | group_join(in_v, get_oddity, get_oddity,
                              [](int o, const coveo::enumerable<const int>& i_s) {
                                  return std::make_tuple(o, std::vector<int>(std::begin(i_s), std::end(i_s)));
                              });
        auto icurex = expected.cbegin();
        for (auto&& r : res) {
            COVEO_ASSERT(icurex != expected.cend());
            COVEO_ASSERT(std::get<0>(r) == std::get<0>(*icurex));
            auto&& exp_seq = std::get<1>(*icurex);
            auto&& act_seq = std::get<1>(r);
            COVEO_ASSERT(detail::equal(std::begin(act_seq), std::end(act_seq),
                                       std::begin(exp_seq), std::end(exp_seq)));
            ++icurex;
        }
        COVEO_ASSERT(!res.has_fast_size());
        COVEO_ASSERT(res.size() == expected.size());
    }
    {
        enum class oddity { odd = 1, even = 2 };
        const std::vector<int> out_v = { 42, 23, 66 };
        const std::vector<int> in_v = { 11, 7, 6, 66, 9, 22 };
        const std::vector<int> in_odd_v = { 11, 7, 9 };
        const std::vector<int> in_even_v = { 6, 66, 22 };
        const std::vector<std::tuple<int, const std::vector<int>&>> expected = {
            std::make_tuple(42, std::ref(in_even_v)),
            std::make_tuple(23, std::ref(in_odd_v)),
            std::make_tuple(66, std::ref(in_even_v)),
        };
        auto get_oddity = [](int i) { return i % 2 == 0 ? oddity::even : oddity::odd; };

        using namespace coveo::linq;
        auto res = from(out_v)
                 | group_join(in_v, get_oddity, get_oddity,
                              [](int o, const coveo::enumerable<const int>& i_s) {
                                  return std::make_tuple(o, std::vector<int>(std::begin(i_s), std::end(i_s)));
                              },
                              coveo::linq::detail::greater<>());
        auto icurex = expected.cbegin();
        for (auto&& r : res) {
            COVEO_ASSERT(icurex != expected.cend());
            COVEO_ASSERT(std::get<0>(r) == std::get<0>(*icurex));
            auto&& exp_seq = std::get<1>(*icurex);
            auto&& act_seq = std::get<1>(r);
            COVEO_ASSERT(detail::equal(std::begin(act_seq), std::end(act_seq),
                                       std::begin(exp_seq), std::end(exp_seq)));
            ++icurex;
        }
        COVEO_ASSERT(!res.has_fast_size());
        COVEO_ASSERT(res.size() == expected.size());
    }
    {
        enum class oddity { odd = 1, even = 2 };
        const std::vector<int> in_odd_v = { 11, 7, 9 };
        const std::vector<int> in_even_v = { 6, 66, 22 };
        const std::vector<std::tuple<int, const std::vector<int>&>> expected = {
            std::make_tuple(42, std::ref(in_even_v)),
            std::make_tuple(23, std::ref(in_odd_v)),
            std::make_tuple(66, std::ref(in_even_v)),
        };
        auto get_oddity = [](int i) { return i % 2 == 0 ? oddity::even : oddity::odd; };

        using namespace coveo::linq;
        auto res = from(std::forward_list<int>{ 42, 23, 66 })
                 | group_join(std::forward_list<int>{ 11, 7, 6, 66, 9, 22 },
                              get_oddity, get_oddity,
                              [](int o, const coveo::enumerable<const int>& i_s) {
                                  return std::make_tuple(o, std::vector<int>(std::begin(i_s), std::end(i_s)));
                              });
        auto icurex = expected.cbegin();
        for (auto&& r : res) {
            COVEO_ASSERT(icurex != expected.cend());
            COVEO_ASSERT(std::get<0>(r) == std::get<0>(*icurex));
            auto&& exp_seq = std::get<1>(*icurex);
            auto&& act_seq = std::get<1>(r);
            COVEO_ASSERT(detail::equal(std::begin(act_seq), std::end(act_seq),
                                       std::begin(exp_seq), std::end(exp_seq)));
            ++icurex;
        }
        COVEO_ASSERT(!res.has_fast_size());
        COVEO_ASSERT(res.size() == expected.size());
    }

    // intersect
    {
        const std::vector<int> v1 = { 42, 23, 66, 11 };
        const std::vector<int> v2 = { 11, 7, 67, 42, 22 };
        const std::vector<int> expected = { 42, 11 };

        using namespace coveo::linq;
        auto intersection = from(v1)
                          | intersect(v2);
        COVEO_ASSERT(detail::equal(std::begin(intersection), std::end(intersection),
                                   std::begin(expected), std::end(expected)));
        COVEO_ASSERT(!intersection.has_fast_size());
        COVEO_ASSERT(intersection.size() == expected.size());
    }
    {
        const std::vector<int> v1 = { 42, 23, 66, 11 };
        const std::vector<int> v2 = { 11, 7, 67, 42, 22 };
        const std::vector<int> expected = { 42, 11 };

        using namespace coveo::linq;
        auto intersection = from(v1)
                          | intersect(v2, coveo::linq::detail::greater<>());
        COVEO_ASSERT(detail::equal(std::begin(intersection), std::end(intersection),
                                   std::begin(expected), std::end(expected)));
        COVEO_ASSERT(!intersection.has_fast_size());
        COVEO_ASSERT(intersection.size() == expected.size());
    }
    {
        std::vector<detail::two_ints> v1 = { 42, 23, 66, 11 };
        const std::vector<detail::two_ints> v2 = { 11, 7, 67, 42, 22 };
        const std::vector<int> expected = { 84, 23, 66, 22 };

        using namespace coveo::linq;
        auto intersection = from(v1)
                          | intersect(v2);
        for (auto&& ti : intersection) {
            ti.second *= 2;
        }
        COVEO_ASSERT(detail::seq_second_equal(v1, expected));
    }
    {
        const std::vector<int> expected = { 42, 11 };

        using namespace coveo::linq;
        auto intersection = from(std::forward_list<int>{ 42, 23, 66, 11 })
                          | intersect(std::forward_list<int>{ 11, 7, 67, 42, 22 });
        COVEO_ASSERT(detail::equal(std::begin(intersection), std::end(intersection),
                                   std::begin(expected), std::end(expected)));
        COVEO_ASSERT(!intersection.has_fast_size());
        COVEO_ASSERT(intersection.size() == expected.size());
    }

    // join
    {
        enum class oddity { odd = 1, even = 2 };
        const std::vector<int> out_v = { 42, 23, 66 };
        const std::vector<int> in_v = { 11, 7, 6, 66, 9, 22 };
        const std::vector<std::tuple<int, int>> expected = {
            std::make_tuple(42, 6), std::make_tuple(42, 66), std::make_tuple(42, 22),
            std::make_tuple(23, 11), std::make_tuple(23, 7), std::make_tuple(23, 9),
            std::make_tuple(66, 6), std::make_tuple(66, 66), std::make_tuple(66, 22),
        };
        auto get_oddity = [](int i) { return i % 2 == 0 ? oddity::even : oddity::odd; };

        using namespace coveo::linq;
        auto res = from(out_v)
                 | join(in_v, get_oddity, get_oddity,
                        [](int o, int i) { return std::make_tuple(o, i); });
        auto icurex = expected.cbegin();
        for (auto&& r : res) {
            COVEO_ASSERT(icurex != expected.cend());
            COVEO_ASSERT(r == *icurex);
            ++icurex;
        }
        COVEO_ASSERT(!res.has_fast_size());
        COVEO_ASSERT(res.size() == expected.size());
    }
    {
        enum class oddity { odd = 1, even = 2 };
        const std::vector<int> out_v = { 42, 23, 66 };
        const std::vector<int> in_v = { 11, 7, 6, 66, 9, 22 };
        const std::vector<std::tuple<int, int>> expected = {
            std::make_tuple(42, 6), std::make_tuple(42, 66), std::make_tuple(42, 22),
            std::make_tuple(23, 11), std::make_tuple(23, 7), std::make_tuple(23, 9),
            std::make_tuple(66, 6), std::make_tuple(66, 66), std::make_tuple(66, 22),
        };
        auto get_oddity = [](int i) { return i % 2 == 0 ? oddity::even : oddity::odd; };

        using namespace coveo::linq;
        auto res = from(out_v)
                 | join(in_v, get_oddity, get_oddity,
                        [](int o, int i) { return std::make_tuple(o, i); },
                        coveo::linq::detail::greater<>());
        auto icurex = expected.cbegin();
        for (auto&& r : res) {
            COVEO_ASSERT(icurex != expected.cend());
            COVEO_ASSERT(r == *icurex);
            ++icurex;
        }
        COVEO_ASSERT(!res.has_fast_size());
        COVEO_ASSERT(res.size() == expected.size());
    }
    {
        enum class oddity { odd = 1, even = 2 };
        const std::vector<std::tuple<int, int>> expected = {
            std::make_tuple(42, 6), std::make_tuple(42, 66), std::make_tuple(42, 22),
            std::make_tuple(23, 11), std::make_tuple(23, 7), std::make_tuple(23, 9),
            std::make_tuple(66, 6), std::make_tuple(66, 66), std::make_tuple(66, 22),
        };
        auto get_oddity = [](int i) { return i % 2 == 0 ? oddity::even : oddity::odd; };

        using namespace coveo::linq;
        auto res = from(std::forward_list<int>{ 42, 23, 66 })
                 | join(std::forward_list<int>{ 11, 7, 6, 66, 9, 22 },
                        get_oddity, get_oddity,
                        [](int o, int i) { return std::make_tuple(o, i); });
        auto icurex = expected.cbegin();
        for (auto&& r : res) {
            COVEO_ASSERT(icurex != expected.cend());
            COVEO_ASSERT(r == *icurex);
            ++icurex;
        }
        COVEO_ASSERT(!res.has_fast_size());
        COVEO_ASSERT(res.size() == expected.size());
    }

    // last
    {
        const std::vector<int> v = { 42, 23, 66, 11, 24 };

        using namespace coveo::linq;
        COVEO_ASSERT((from(v)
                    | last()) == 24);
        COVEO_ASSERT((from(v)
                    | last([](int i) { return i % 2 != 0; })) == 11);
    }
    {
        const std::forward_list<int> v = { 42, 23, 66, 11, 24 };

        using namespace coveo::linq;
        COVEO_ASSERT((from(v)
                    | last()) == 24);
        COVEO_ASSERT((from(v)
                    | last([](int i) { return i % 2 != 0; })) == 11);
    }
    {
        std::vector<int> v = { 42, 23, 66, 11, 24 };
        const std::vector<int> expected = { 42, 23, 66, 10, 25 };

        using namespace coveo::linq;
        (from(v) | last([](int i) { return i % 2 != 0; })) -= 1;
        (from(v) | last()) += 1;
        COVEO_ASSERT(detail::equal(std::begin(v), std::end(v),
                                   std::begin(expected), std::end(expected)));
    }

    // last_or_default
    {
        const std::vector<int> v = { 42, 23, 66, 11, 24 };
        const std::vector<int> e;

        using namespace coveo::linq;
        COVEO_ASSERT((from(v)
                    | last_or_default()) == 24);
        COVEO_ASSERT((from(e)
                    | last_or_default()) == 0);
        COVEO_ASSERT((from(v)
                    | last_or_default([](int i) { return i > 30; })) == 66);
        COVEO_ASSERT((from(v)
                    | last_or_default([](int i) { return i > 100; })) == 0);
    }
    {
        const std::forward_list<int> v = { 42, 23, 66, 11, 24 };
        const std::forward_list<int> e;

        using namespace coveo::linq;
        COVEO_ASSERT((from(v)
                    | last_or_default()) == 24);
        COVEO_ASSERT((from(e)
                    | last_or_default()) == 0);
        COVEO_ASSERT((from(v)
                    | last_or_default([](int i) { return i > 30; })) == 66);
        COVEO_ASSERT((from(v)
                    | last_or_default([](int i) { return i > 100; })) == 0);
    }

    // max
    {
        const std::vector<int> v = { 42, 23, 66, 11, 24 };

        using namespace coveo::linq;
        COVEO_ASSERT((from(v) | max()) == 66);
        COVEO_ASSERT((from(v) | max([](int i) { return -i; })) == -11);
    }

    // min
    {
        const std::vector<int> v = { 42, 23, 66, 11, 24 };

        using namespace coveo::linq;
        COVEO_ASSERT((from(v) | min()) == 11);
        COVEO_ASSERT((from(v) | min([](int i) { return -i; })) == -66);
    }

    // order_by/order_by_descending/then_by/then_by_descending
    {
        const std::vector<int> v = { 42, 23, 66, 11, 24 };
        const std::vector<int> expected = { 11, 23, 24, 42, 66 };

        using namespace coveo::linq;
        auto seq = from(v)
                 | order_by([](int i) { return i; });
        COVEO_ASSERT(detail::equal(std::begin(seq), std::end(seq),
                                   std::begin(expected), std::end(expected)));
        COVEO_ASSERT(seq.has_fast_size());
        COVEO_ASSERT(seq.size() == expected.size());
    }
    {
        const std::vector<int> v = { 42, 23, 66, 11, 24 };
        const std::vector<int> expected = { 66, 42, 24, 23, 11 };

        using namespace coveo::linq;
        auto seq = from(std::vector<int>(v))
                 | order_by([](int i) { return i; }, std::greater<int>());
        COVEO_ASSERT(detail::equal(std::begin(seq), std::end(seq),
                                   std::begin(expected), std::end(expected)));
        COVEO_ASSERT(seq.has_fast_size());
        COVEO_ASSERT(seq.size() == expected.size());
    }
    {
        const std::vector<int> expected = { 11, 23, 24, 42, 66 };

        using namespace coveo::linq;
        auto seq = from(std::forward_list<int>{ 42, 23, 66, 11, 24 })
                 | order_by([](int i) { return i; });
        COVEO_ASSERT(detail::equal(std::begin(seq), std::end(seq),
                                   std::begin(expected), std::end(expected)));
        COVEO_ASSERT(seq.has_fast_size());
        COVEO_ASSERT(seq.size() == expected.size());
    }
    {
        const std::vector<int> v = { 42, 23, 66, 11, 24 };
        const std::vector<int> expected = { 66, 42, 24, 23, 11 };

        using namespace coveo::linq;
        auto seq = from(v)
                 | order_by_descending([](int i) { return i; });
        COVEO_ASSERT(detail::equal(std::begin(seq), std::end(seq),
                                   std::begin(expected), std::end(expected)));
        COVEO_ASSERT(seq.has_fast_size());
        COVEO_ASSERT(seq.size() == expected.size());
    }
    {
        const std::vector<int> v = { 42, 23, 66, 11, 24 };
        const std::vector<int> expected = { 11, 23, 24, 42, 66 };

        using namespace coveo::linq;
        auto seq = from(std::vector<int>(v))
                 | order_by_descending([](int i) { return i; }, std::greater<int>());
        COVEO_ASSERT(detail::equal(std::begin(seq), std::end(seq),
                                   std::begin(expected), std::end(expected)));
        COVEO_ASSERT(seq.has_fast_size());
        COVEO_ASSERT(seq.size() == expected.size());
    }
    {
        const std::vector<int> expected = { 66, 42, 24, 23, 11 };

        using namespace coveo::linq;
        auto seq = from(std::forward_list<int>{ 42, 23, 66, 11, 24 })
                 | order_by_descending([](int i) { return i; });
        COVEO_ASSERT(detail::equal(std::begin(seq), std::end(seq),
                                   std::begin(expected), std::end(expected)));
        COVEO_ASSERT(seq.has_fast_size());
        COVEO_ASSERT(seq.size() == expected.size());
    }
    {
        const std::vector<std::string> v = { "grape", "passionfruit", "banana", "mango",
                                             "orange", "raspberry", "apple", "blueberry" };
        const std::vector<std::string> expected = { "apple", "grape", "mango", "banana", "orange",
                                                    "blueberry", "raspberry", "passionfruit" };

        using namespace coveo::linq;
        auto seq = from(v)
                 | order_by([](const std::string& a) { return a.size(); })
                 | then_by([](const std::string& a) { return a; });
        COVEO_ASSERT(detail::equal(std::begin(seq), std::end(seq),
                                   std::begin(expected), std::end(expected)));
        COVEO_ASSERT(seq.has_fast_size());
        COVEO_ASSERT(seq.size() == expected.size());
    }
    {
        const std::vector<std::string> expected = { "apple", "grape", "mango", "banana", "orange",
                                                    "blueberry", "raspberry", "passionfruit" };

        using namespace coveo::linq;
        auto seq = from(std::forward_list<std::string>{ "grape", "passionfruit", "banana", "mango",
                                                        "orange", "raspberry", "apple", "blueberry" })
                 | order_by([](const std::string& a) { return a.size(); })
                 | then_by([](const std::string& a) { return a; });
        COVEO_ASSERT(detail::equal(std::begin(seq), std::end(seq),
                                   std::begin(expected), std::end(expected)));
        COVEO_ASSERT(seq.has_fast_size());
        COVEO_ASSERT(seq.size() == expected.size());
    }
    {
        const std::vector<std::string> v = { "grape", "passionfruit", "banana", "mango",
                                             "orange", "raspberry", "apple", "blueberry" };
        const std::vector<std::string> expected = { "passionfruit", "raspberry", "blueberry",
                                                    "orange", "banana", "mango", "grape", "apple" };

        using namespace coveo::linq;
        auto seq = from(v)
                 | order_by_descending([](const std::string& a) { return a.size(); })
                 | then_by_descending([](const std::string& a) { return a; });
        COVEO_ASSERT(detail::equal(std::begin(seq), std::end(seq),
                                   std::begin(expected), std::end(expected)));
        COVEO_ASSERT(seq.has_fast_size());
        COVEO_ASSERT(seq.size() == expected.size());
    }
    {
        const std::vector<std::string> expected = { "passionfruit", "raspberry", "blueberry",
                                                    "orange", "banana", "mango", "grape", "apple" };

        using namespace coveo::linq;
        auto seq = from(std::forward_list<std::string>{ "grape", "passionfruit", "banana", "mango",
                                                        "orange", "raspberry", "apple", "blueberry" })
                 | order_by_descending([](const std::string& a) { return a.size(); })
                 | then_by_descending([](const std::string& a) { return a; });
        COVEO_ASSERT(detail::equal(std::begin(seq), std::end(seq),
                                   std::begin(expected), std::end(expected)));
        COVEO_ASSERT(seq.has_fast_size());
        COVEO_ASSERT(seq.size() == expected.size());
    }
    {
        std::vector<int> v = { 42, 23, 66 };
        const std::vector<int> expected = { 84, 46, 132 };

        using namespace coveo::linq;
        auto seq = from(v)
                 | order_by([](int i) { return i; })
                 | then_by_descending([](int i) { return i; });
        for (auto&& i : seq) {
            i *= 2;
        }
        COVEO_ASSERT(detail::equal(std::begin(v), std::end(v),
                                   std::begin(expected), std::end(expected)));
    }

    // reverse
    {
        const std::vector<int> v = { 42, 23, 66, 11, 24 };
        const std::vector<int> expected = { 24, 11, 66, 23, 42 };

        using namespace coveo::linq;
        auto seq = from(v)
                 | reverse();
        COVEO_ASSERT(detail::equal(std::begin(seq), std::end(seq),
                                   std::begin(expected), std::end(expected)));
        COVEO_ASSERT(seq.has_fast_size());
        COVEO_ASSERT(seq.size() == expected.size());
    }
    {
        const std::forward_list<int> v = { 42, 23, 66, 11, 24 };
        const std::forward_list<int> expected = { 24, 11, 66, 23, 42 };

        using namespace coveo::linq;
        auto seq = from(v)
                 | reverse();
        COVEO_ASSERT(detail::equal(std::begin(seq), std::end(seq),
                                   std::begin(expected), std::end(expected)));
        COVEO_ASSERT(seq.has_fast_size());
        COVEO_ASSERT(seq.size() == 5);
    }
    {
        std::vector<int> v = { 42, 23, 66 };
        const std::vector<int> expected = { 84, 46, 132 };

        using namespace coveo::linq;
        auto seq = from(v)
                 | reverse();
        for (auto&& i : seq) {
            i *= 2;
        }
        COVEO_ASSERT(detail::equal(std::begin(v), std::end(v),
                                   std::begin(expected), std::end(expected)));
    }
    {
        std::forward_list<int> v = { 42, 23, 66 };
        const std::vector<int> expected = { 84, 46, 132 };

        using namespace coveo::linq;
        auto seq = from(v)
                 | reverse();
        for (auto&& i : seq) {
            i *= 2;
        }
        COVEO_ASSERT(detail::equal(std::begin(v), std::end(v),
                                   std::begin(expected), std::end(expected)));
    }

    // select/select_with_index/select_many/select_many_with_index
    {
        const std::vector<int> v = { 42, 23, 66 };
        const std::vector<std::string> expected = { "4242", "2323", "6666" };
        auto our_itoa = [](int i) -> std::string {
            std::ostringstream oss;
            oss << i;
            return oss.str();
        };
        auto our_dblstr = [](const std::string& s) -> std::string {
            return s + s;
        };

        using namespace coveo::linq;
        auto seq = from(v)
                 | select(our_itoa)
                 | select(our_dblstr);
        COVEO_ASSERT(detail::equal(std::begin(seq), std::end(seq),
                                   std::begin(expected), std::end(expected)));
        COVEO_ASSERT(seq.has_fast_size());
        COVEO_ASSERT(seq.size() == expected.size());
    }
    {
        const std::vector<std::string> expected = { "4242", "2323", "6666" };
        auto our_itoa = [](int i) -> std::string {
            std::ostringstream oss;
            oss << i;
            return oss.str();
        };
        auto our_dblstr = [](const std::string& s) -> std::string {
            return s + s;
        };

        using namespace coveo::linq;
        auto seq = from(std::forward_list<int>{ 42, 23, 66 })
                 | select(our_itoa)
                 | select(our_dblstr);
        COVEO_ASSERT(detail::equal(std::begin(seq), std::end(seq),
                                   std::begin(expected), std::end(expected)));
        COVEO_ASSERT(!seq.has_fast_size());
        COVEO_ASSERT(seq.size() == expected.size());
    }
    {
        const std::vector<int> v = { 42, 23, 66 };
        const std::vector<std::string> expected = { "43", "2525", "696969" };
        auto our_itoa = [](int i, std::size_t idx) -> std::string {
            std::ostringstream oss;
            oss << static_cast<std::size_t>(i) + idx + 1;
            return oss.str();
        };
        auto our_mulstr = [](const std::string& s, std::size_t idx) -> std::string {
            std::string r;
            for (std::size_t i = 0; i <= idx; ++i) {
                r += s;
            }
            return r;
        };

        using namespace coveo::linq;
        auto seq = from(v)
                 | select_with_index(our_itoa)
                 | select_with_index(our_mulstr);
        COVEO_ASSERT(detail::equal(std::begin(seq), std::end(seq),
                                   std::begin(expected), std::end(expected)));
        COVEO_ASSERT(seq.has_fast_size());
        COVEO_ASSERT(seq.size() == expected.size());
    }
    {
        const std::vector<std::string> expected = { "43", "2525", "696969" };
        auto our_itoa = [](int i, std::size_t idx) -> std::string {
            std::ostringstream oss;
            oss << static_cast<std::size_t>(i) + idx + 1;
            return oss.str();
        };
        auto our_mulstr = [](const std::string& s, std::size_t idx) -> std::string {
            std::string r;
            for (std::size_t i = 0; i <= idx; ++i) {
                r += s;
            }
            return r;
        };

        using namespace coveo::linq;
        auto seq = from(std::forward_list<int>{ 42, 23, 66 })
                 | select_with_index(our_itoa)
                 | select_with_index(our_mulstr);
        COVEO_ASSERT(detail::equal(std::begin(seq), std::end(seq),
                                   std::begin(expected), std::end(expected)));
        COVEO_ASSERT(!seq.has_fast_size());
        COVEO_ASSERT(seq.size() == expected.size());
    }
    {
        const std::vector<int> v = { 42, 23, 66 };
        const std::vector<std::string> expected = { "42", "24", "23", "32", "66", "66" };
        auto our_itoa = [](int i) {
            std::vector<std::string> ov;
            std::ostringstream oss;
            oss << i;
            ov.push_back(oss.str());
            ov.push_back(oss.str());
            std::reverse(ov.back().begin(), ov.back().end());
            return coveo::enumerate_container(std::move(ov));
        };

        using namespace coveo::linq;
        auto seq = from(v)
                 | select_many(our_itoa);
        COVEO_ASSERT(detail::equal(std::begin(seq), std::end(seq),
                                   std::begin(expected), std::end(expected)));
        COVEO_ASSERT(!seq.has_fast_size());
        COVEO_ASSERT(seq.size() == expected.size());
    }
    {
        const std::vector<std::string> expected = { "42", "24", "23", "32", "66", "66" };
        auto our_itoa = [](int i) {
            std::vector<std::string> ov;
            std::ostringstream oss;
            oss << i;
            ov.push_back(oss.str());
            ov.push_back(oss.str());
            std::reverse(ov.back().begin(), ov.back().end());
            return coveo::enumerate_container(std::move(ov));
        };

        using namespace coveo::linq;
        auto seq = from(std::forward_list<int>{ 42, 23, 66 })
                 | select_many(our_itoa);
        COVEO_ASSERT(detail::equal(std::begin(seq), std::end(seq),
                                   std::begin(expected), std::end(expected)));
        COVEO_ASSERT(!seq.has_fast_size());
        COVEO_ASSERT(seq.size() == expected.size());
    }
    {
        const std::vector<int> v = { 42, 23, 66 };
        const std::vector<std::string> expected = { "43", "34", "25", "52", "69", "96" };
        auto our_itoa = [](int i, std::size_t idx) {
            std::vector<std::string> ov;
            std::ostringstream oss;
            oss << static_cast<std::size_t>(i) + idx + 1;
            ov.push_back(oss.str());
            ov.push_back(oss.str());
            std::reverse(ov.back().begin(), ov.back().end());
            return coveo::enumerate_container(std::move(ov));
        };

        using namespace coveo::linq;
        auto seq = from(v)
                 | select_many_with_index(our_itoa);
        COVEO_ASSERT(detail::equal(std::begin(seq), std::end(seq),
                                   std::begin(expected), std::end(expected)));
        COVEO_ASSERT(!seq.has_fast_size());
        COVEO_ASSERT(seq.size() == expected.size());
    }
    {
        const std::vector<std::string> expected = { "43", "34", "25", "52", "69", "96" };
        auto our_itoa = [](int i, std::size_t idx) {
            std::vector<std::string> ov;
            std::ostringstream oss;
            oss << static_cast<std::size_t>(i) + idx + 1;
            ov.push_back(oss.str());
            ov.push_back(oss.str());
            std::reverse(ov.back().begin(), ov.back().end());
            return coveo::enumerate_container(std::move(ov));
        };

        using namespace coveo::linq;
        auto seq = from(std::forward_list<int>{ 42, 23, 66 })
                 | select_many_with_index(our_itoa);
        COVEO_ASSERT(detail::equal(std::begin(seq), std::end(seq),
                                   std::begin(expected), std::end(expected)));
        COVEO_ASSERT(!seq.has_fast_size());
        COVEO_ASSERT(seq.size() == expected.size());
    }

    // sequence_equal
    {
        const std::vector<int> v = { 42, 23, 66, 11, 24 };
        const std::forward_list<int> expected = { 42, 23, 66, 11, 24 };

        using namespace coveo::linq;
        COVEO_ASSERT(from(v) | sequence_equal(expected));
    }
    {
        const std::vector<int> v = { 42, 23, 66, 11, 24 };
        const std::forward_list<int> expected = { -42, 23, -66, -11, 24 };
        auto fuzzy_equal = [](int i, int j) {
            return std::abs(i) == std::abs(j);
        };

        using namespace coveo::linq;
        COVEO_ASSERT(from(v) | sequence_equal(expected, fuzzy_equal));
    }

    // single
    {
        const std::vector<int> zero_v;
        const std::vector<int> one_v = { 42 };
        const std::vector<int> two_v = { 42, 23 };

        using namespace coveo::linq;
        COVEO_ASSERT_THROW(from(zero_v) | single());
        COVEO_ASSERT((from(one_v) | single()) == 42);
        COVEO_ASSERT_THROW(from(two_v) | single());
    }
    {
        const std::vector<int> zero_v;
        const std::vector<int> no_42_v = { 23, 66, 11 };
        const std::vector<int> one_42_v = { 42, 23, 66, 11 };
        const std::vector<int> two_42_v = { 42, 23, 66, 42, 11 };
        auto equal_to_42 = std::bind(std::equal_to<int>(), std::placeholders::_1, 42);

        using namespace coveo::linq;
        COVEO_ASSERT_THROW(from(zero_v) | single(equal_to_42));
        COVEO_ASSERT_THROW(from(no_42_v) | single(equal_to_42));
        COVEO_ASSERT((from(one_42_v) | single(equal_to_42)) == 42);
        COVEO_ASSERT_THROW(from(two_42_v) | single(equal_to_42));
    }
    {
        std::vector<int> v = { 42 };
        const std::vector<int> expected = { 43 };

        using namespace coveo::linq;
        (from(v) | single()) += 1;
        COVEO_ASSERT(detail::equal(std::begin(v), std::end(v),
                                   std::begin(expected), std::end(expected)));
    }
    {
        std::vector<int> v = { 23, 42, 66 };
        const std::vector<int> expected = { 23, 84, 66 };
        auto equal_to_42 = std::bind(std::equal_to<int>(), std::placeholders::_1, 42);

        using namespace coveo::linq;
        (from(v) | single(equal_to_42)) *= 2;
        COVEO_ASSERT(detail::equal(std::begin(v), std::end(v),
                                   std::begin(expected), std::end(expected)));
    }

    // single_or_default
    {
        const std::vector<int> zero_v;
        const std::vector<int> one_v = { 42 };
        const std::vector<int> two_v = { 42, 23 };

        using namespace coveo::linq;
        COVEO_ASSERT((from(zero_v) | single_or_default()) == 0);
        COVEO_ASSERT((from(one_v) | single_or_default()) == 42);
        COVEO_ASSERT((from(two_v) | single_or_default()) == 0);
    }
    {
        const std::vector<int> zero_v;
        const std::vector<int> no_42_v = { 23, 66, 11 };
        const std::vector<int> one_42_v = { 42, 23, 66, 11 };
        const std::vector<int> two_42_v = { 42, 23, 66, 42, 11 };
        auto equal_to_42 = std::bind(std::equal_to<int>(), std::placeholders::_1, 42);

        using namespace coveo::linq;
        COVEO_ASSERT((from(zero_v) | single_or_default(equal_to_42)) == 0);
        COVEO_ASSERT((from(no_42_v) | single_or_default(equal_to_42)) == 0);
        COVEO_ASSERT((from(one_42_v) | single_or_default(equal_to_42)) == 42);
        COVEO_ASSERT((from(two_42_v) | single_or_default(equal_to_42)) == 0);
    }

    // skip
    {
        const std::vector<int> v = { 42, 23, 66, 11, 24 };
        const std::vector<int> last_two = { 11, 24 };
        const std::vector<int> none;

        using namespace coveo::linq;
        auto e_skip_3 = from(v)
                      | skip(3);
        auto e_skip_9 = from(v)
                      | skip(9);
        COVEO_ASSERT(detail::equal(std::begin(e_skip_3), std::end(e_skip_3),
                                   std::begin(last_two), std::end(last_two)));
        COVEO_ASSERT(detail::equal(std::begin(e_skip_9), std::end(e_skip_9),
                                   std::begin(none), std::end(none)));
        COVEO_ASSERT(e_skip_3.has_fast_size());
        COVEO_ASSERT(e_skip_9.has_fast_size());
        COVEO_ASSERT(e_skip_3.size() == last_two.size());
        COVEO_ASSERT(e_skip_9.size() == none.size());
    }
    {
        std::vector<detail::two_ints> v = { 42, 23, 66, 11, 24 };
        const std::vector<int> expected = { 42, 23, 66, 22, 48 };

        using namespace coveo::linq;
        auto e_skip_3 = from(v)
                      | skip(3);
        for (auto&& ti : e_skip_3) {
            ti.second *= 2;
        }
        COVEO_ASSERT(detail::seq_second_equal(v, expected));
    }
    {
        const std::vector<int> last_two = { 11, 24 };

        using namespace coveo::linq;
        auto e_skip_3 = from(std::forward_list<int>{ 42, 23, 66, 11, 24 })
                      | skip(3);
        COVEO_ASSERT(detail::equal(std::begin(e_skip_3), std::end(e_skip_3),
                                   std::begin(last_two), std::end(last_two)));
        COVEO_ASSERT(!e_skip_3.has_fast_size());
        COVEO_ASSERT(e_skip_3.size() == last_two.size());
    }

    // skip_while
    {
        const std::vector<int> v = { 42, 23, 66, 11, 24 };
        const std::vector<int> v_66_and_up = { 66, 11, 24 };
        const std::vector<int> none;

        using namespace coveo::linq;
        auto e_after_60 = from(v)
                        | skip_while([](int i) { return i < 60; });
        auto e_after_90 = from(v)
                        | skip_while([](int i) { return i < 90; });
        COVEO_ASSERT(detail::equal(std::begin(e_after_60), std::end(e_after_60),
                                   std::begin(v_66_and_up), std::end(v_66_and_up)));
        COVEO_ASSERT(detail::equal(std::begin(e_after_90), std::end(e_after_90),
                                   std::begin(none), std::end(none)));
        COVEO_ASSERT(!e_after_60.has_fast_size());
        COVEO_ASSERT(!e_after_90.has_fast_size());
        COVEO_ASSERT(e_after_60.size() == v_66_and_up.size());
        COVEO_ASSERT(e_after_90.size() == none.size());
    }
    {
        std::vector<detail::two_ints> v = { 42, 23, 66, 11, 24 };
        const std::vector<int> expected = { 42, 23, 132, 22, 48 };

        using namespace coveo::linq;
        auto e_after_60 = from(v)
                        | skip_while([](const detail::two_ints& ti) { return ti.first < 60; });
        for (auto&& ti : e_after_60) {
            ti.second *= 2;
        }
        COVEO_ASSERT(detail::seq_second_equal(v, expected));
    }
    {
        const std::vector<int> v_66_and_up = { 66, 11, 24 };

        using namespace coveo::linq;
        auto e_after_60 = from(std::forward_list<int>{ 42, 23, 66, 11, 24 })
                        | skip_while([](int i) { return i < 60; });
        COVEO_ASSERT(detail::equal(std::begin(e_after_60), std::end(e_after_60),
                                   std::begin(v_66_and_up), std::end(v_66_and_up)));
        COVEO_ASSERT(!e_after_60.has_fast_size());
        COVEO_ASSERT(e_after_60.size() == v_66_and_up.size());
    }

    // skip_while_with_index
    {
        const std::vector<int> v = { 42, 23, 66, 11, 24 };
        const std::vector<int> v_66_and_up = { 66, 11, 24 };
        const std::vector<int> v_24_and_up = { 24 };

        using namespace coveo::linq;
        auto e_after_60 = from(v)
                        | skip_while_with_index([](int i, std::size_t n) { return i < 60 && n < 4; });
        auto e_after_90 = from(v)
                        | skip_while_with_index([](int i, std::size_t n) { return i < 90 && n < 4; });
        COVEO_ASSERT(detail::equal(std::begin(e_after_60), std::end(e_after_60),
                                   std::begin(v_66_and_up), std::end(v_66_and_up)));
        COVEO_ASSERT(detail::equal(std::begin(e_after_90), std::end(e_after_90),
                                   std::begin(v_24_and_up), std::end(v_24_and_up)));
        COVEO_ASSERT(!e_after_60.has_fast_size());
        COVEO_ASSERT(!e_after_90.has_fast_size());
        COVEO_ASSERT(e_after_60.size() == v_66_and_up.size());
        COVEO_ASSERT(e_after_90.size() == v_24_and_up.size());
    }
    {
        std::vector<detail::two_ints> v = { 42, 23, 66, 11, 24 };
        const std::vector<int> expected = { 42, 23, 66, 11, 48 };

        using namespace coveo::linq;
        auto e_after_4th = from(v)
                         | skip_while_with_index([](const detail::two_ints&, std::size_t n) { return n < 4; });
        for (auto&& ti : e_after_4th) {
            ti.second *= 2;
        }
        COVEO_ASSERT(detail::seq_second_equal(v, expected));
    }
    {
        const std::vector<int> v_66_and_up = { 66, 11, 24 };

        using namespace coveo::linq;
        auto e_after_60 = from(std::forward_list<int>{ 42, 23, 66, 11, 24 })
                        | skip_while_with_index([](int i, std::size_t n) { return i < 60 && n < 4; });
        COVEO_ASSERT(detail::equal(std::begin(e_after_60), std::end(e_after_60),
                                   std::begin(v_66_and_up), std::end(v_66_and_up)));
        COVEO_ASSERT(!e_after_60.has_fast_size());
        COVEO_ASSERT(e_after_60.size() == v_66_and_up.size());
    }

    // sum
    {
        const std::vector<int> v = { 42, 23, 66 };

        using namespace coveo::linq;
        auto sum_int = from(v)
                     | sum([](int i) { return i; });
        COVEO_ASSERT(sum_int == 131);

        auto sum_dbl = from(v)
                     | sum([](int i) { return double(i); });
        COVEO_ASSERT(sum_dbl >= 131.0 && sum_dbl < 131.01);
    }
    {
        const std::vector<int> ev;

        using namespace coveo::linq;
        COVEO_ASSERT_THROW(from(ev)
                         | sum([](int i) { return i; }));
    }

    // take
    {
        const std::vector<int> v = { 42, 23, 66, 11, 24 };
        const std::vector<int> first_three = { 42, 23, 66 };
        const std::vector<int> none;

        using namespace coveo::linq;
        auto e_take_3 = from(v)
                      | take(3);
        auto e_take_0 = from(v)
                      | take(0);
        COVEO_ASSERT(detail::equal(std::begin(e_take_3), std::end(e_take_3),
                                   std::begin(first_three), std::end(first_three)));
        COVEO_ASSERT(detail::equal(std::begin(e_take_0), std::end(e_take_0),
                                   std::begin(none), std::end(none)));
        COVEO_ASSERT(e_take_3.has_fast_size());
        COVEO_ASSERT(e_take_0.has_fast_size());
        COVEO_ASSERT(e_take_3.size() == first_three.size());
        COVEO_ASSERT(e_take_0.size() == none.size());
    }
    {
        std::vector<detail::two_ints> v = { 42, 23, 66, 11, 24 };
        const std::vector<int> expected = { 84, 46, 132, 11, 24 };

        using namespace coveo::linq;
        auto e_take_3 = from(v)
                      | take(3);
        for (auto&& ti : e_take_3) {
            ti.second *= 2;
        }
        COVEO_ASSERT(detail::seq_second_equal(v, expected));
    }
    {
        const std::vector<int> first_three = { 42, 23, 66 };

        using namespace coveo::linq;
        auto e_take_3 = from(std::forward_list<int>{ 42, 23, 66, 11, 24 })
                      | take(3);
        COVEO_ASSERT(detail::equal(std::begin(e_take_3), std::end(e_take_3),
                                   std::begin(first_three), std::end(first_three)));
        COVEO_ASSERT(!e_take_3.has_fast_size());
        COVEO_ASSERT(e_take_3.size() == first_three.size());
    }

    // take_while
    {
        const std::vector<int> v = { 42, 23, 66, 11, 24 };
        const std::vector<int> v_before_66 = { 42, 23 };

        using namespace coveo::linq;
        auto e_before_60 = from(v)
                        | take_while([](int i) { return i < 60; });
        auto e_before_90 = from(v)
                        | take_while([](int i) { return i < 90; });
        COVEO_ASSERT(detail::equal(std::begin(e_before_60), std::end(e_before_60),
                                   std::begin(v_before_66), std::end(v_before_66)));
        COVEO_ASSERT(detail::equal(std::begin(e_before_90), std::end(e_before_90),
                                   std::begin(v), std::end(v)));
        COVEO_ASSERT(!e_before_60.has_fast_size());
        COVEO_ASSERT(!e_before_90.has_fast_size());
        COVEO_ASSERT(e_before_60.size() == v_before_66.size());
        COVEO_ASSERT(e_before_90.size() == v.size());
    }
    {
        std::vector<detail::two_ints> v = { 42, 23, 66, 11, 24 };
        const std::vector<int> expected = { 84, 46, 66, 11, 24 };
        
        using namespace coveo::linq;
        auto e_before_60 = from(v)
                         | take_while([](const detail::two_ints& ti) { return ti.first < 60; });
        for (auto&& ti : e_before_60) {
            ti.second *= 2;
        }
        COVEO_ASSERT(detail::seq_second_equal(v, expected));
    }
    {
        const std::vector<int> v_before_66 = { 42, 23 };

        using namespace coveo::linq;
        auto e_before_60 = from(std::forward_list<int>{ 42, 23, 66, 11, 24 })
                        | take_while([](int i) { return i < 60; });
        COVEO_ASSERT(detail::equal(std::begin(e_before_60), std::end(e_before_60),
                                   std::begin(v_before_66), std::end(v_before_66)));
        COVEO_ASSERT(!e_before_60.has_fast_size());
        COVEO_ASSERT(e_before_60.size() == v_before_66.size());
    }

    // take_while_with_index
    {
        const std::vector<int> v = { 42, 23, 66, 11, 24 };
        const std::vector<int> v_before_66 = { 42, 23 };
        const std::vector<int> v_before_5th = { 42, 23, 66, 11 };

        using namespace coveo::linq;
        auto e_before_60 = from(v)
                        | take_while_with_index([](int i, std::size_t n) { return i < 60 && n < 4; });
        auto e_before_90 = from(v)
                        | take_while_with_index([](int i, std::size_t n) { return i < 90 && n < 4; });
        COVEO_ASSERT(detail::equal(std::begin(e_before_60), std::end(e_before_60),
                                   std::begin(v_before_66), std::end(v_before_66)));
        COVEO_ASSERT(detail::equal(std::begin(e_before_90), std::end(e_before_90),
                                   std::begin(v_before_5th), std::end(v_before_5th)));
        COVEO_ASSERT(!e_before_60.has_fast_size());
        COVEO_ASSERT(!e_before_90.has_fast_size());
        COVEO_ASSERT(e_before_60.size() == v_before_66.size());
        COVEO_ASSERT(e_before_90.size() == v_before_5th.size());
    }
    {
        std::vector<detail::two_ints> v = { 42, 23, 66, 11, 24 };
        const std::vector<int> expected = { 84, 46, 132, 22, 24 };

        using namespace coveo::linq;
        auto e_before_4th = from(v)
                          | take_while_with_index([](const detail::two_ints&, std::size_t n) { return n < 4; });
        for (auto&& ti : e_before_4th) {
            ti.second *= 2;
        }
        COVEO_ASSERT(detail::seq_second_equal(v, expected));
    }
    {
        const std::vector<int> v_before_66 = { 42, 23 };

        using namespace coveo::linq;
        auto e_before_60 = from(std::forward_list<int>{ 42, 23, 66, 11, 24 })
                        | take_while_with_index([](int i, std::size_t n) { return i < 60 && n < 4; });
        COVEO_ASSERT(detail::equal(std::begin(e_before_60), std::end(e_before_60),
                                   std::begin(v_before_66), std::end(v_before_66)));
        COVEO_ASSERT(!e_before_60.has_fast_size());
        COVEO_ASSERT(e_before_60.size() == v_before_66.size());
    }

    // to/to_vector/to_associative/to_map
    {
        const std::vector<int> v = { 42, 23, 66, 11, 24 };
        const std::forward_list<int> fl = { 42, 23, 66, 11, 24 };

        using namespace coveo::linq;
        auto linq_fl = from(v)
                     | to<std::forward_list<int>>();
        COVEO_ASSERT(linq_fl == fl);
    }
    {
        const std::forward_list<int> fl = { 42, 23, 66, 11, 24 };

        using namespace coveo::linq;
        auto linq_fl = from(std::vector<int>{ 42, 23, 66, 11, 24 })
                     | to<std::forward_list<int>>();
        COVEO_ASSERT(linq_fl == fl);
    }
    {
        const std::forward_list<int> fl = { 42, 23, 66, 11, 24 };
        const std::vector<int> v = { 42, 23, 66, 11, 24 };

        using namespace coveo::linq;
        auto linq_v = from(fl)
                    | to_vector();
        COVEO_ASSERT(linq_v == v);
    }
    {
        const std::vector<int> v = { 42, 23, 66, 11, 24 };

        using namespace coveo::linq;
        auto linq_v = from(std::forward_list<int>{ 42, 23, 66, 11, 24 })
                    | to_vector();
        COVEO_ASSERT(linq_v == v);
    }
    {
        const std::vector<std::pair<int, std::string>> v = { { 42, "Life" }, { 23, "Hangar" } };
        auto pair_first = [](std::pair<int, std::string> p) { return p.first; };

        using namespace coveo::linq;
        auto linq_m = from(v)
                    | to_associative<std::map<int, std::pair<int, std::string>>>(pair_first);
        auto it = std::begin(linq_m);
        COVEO_ASSERT(it->first == 23);
        COVEO_ASSERT(it->second.first == 23);
        COVEO_ASSERT(it->second.second == "Hangar");
        ++it;
        COVEO_ASSERT(it->first == 42);
        COVEO_ASSERT(it->second.first == 42);
        COVEO_ASSERT(it->second.second == "Life");
        COVEO_ASSERT(++it == std::end(linq_m));
    }
    {
        const std::vector<std::pair<int, std::string>> v = { { 42, "Life" }, { 23, "Hangar" } };
        auto pair_first = [](std::pair<int, std::string> p) { return p.first; };
        auto pair_second = [](std::pair<int, std::string> p) { return p.second; };

        using namespace coveo::linq;
        auto linq_m = from(v)
                    | to_associative<std::map<int, std::string>>(pair_first, pair_second);
        auto it = std::begin(linq_m);
        COVEO_ASSERT(it->first == 23);
        COVEO_ASSERT(it->second == "Hangar");
        ++it;
        COVEO_ASSERT(it->first == 42);
        COVEO_ASSERT(it->second == "Life");
        COVEO_ASSERT(++it == std::end(linq_m));
    }
    {
        auto pair_first = [](std::pair<int, std::string> p) { return p.first; };

        using namespace coveo::linq;
        auto linq_m = from(std::vector<std::pair<int, std::string>>{ { 42, "Life" }, { 23, "Hangar" } })
                    | to_associative<std::map<int, std::pair<int, std::string>>>(pair_first);
        auto it = std::begin(linq_m);
        COVEO_ASSERT(it->first == 23);
        COVEO_ASSERT(it->second.first == 23);
        COVEO_ASSERT(it->second.second == "Hangar");
        ++it;
        COVEO_ASSERT(it->first == 42);
        COVEO_ASSERT(it->second.first == 42);
        COVEO_ASSERT(it->second.second == "Life");
        COVEO_ASSERT(++it == std::end(linq_m));
    }
    {
        const std::vector<std::pair<int, std::string>> v = { { 42, "Life" }, { 23, "Hangar" } };
        auto pair_first = [](std::pair<int, std::string> p) { return p.first; };

        using namespace coveo::linq;
        auto linq_m = from(v)
                    | to_map(pair_first);
        auto it = std::begin(linq_m);
        COVEO_ASSERT(it->first == 23);
        COVEO_ASSERT(it->second.first == 23);
        COVEO_ASSERT(it->second.second == "Hangar");
        ++it;
        COVEO_ASSERT(it->first == 42);
        COVEO_ASSERT(it->second.first == 42);
        COVEO_ASSERT(it->second.second == "Life");
        COVEO_ASSERT(++it == std::end(linq_m));
    }
    {
        const std::vector<std::pair<int, std::string>> v = { { 42, "Life" }, { 23, "Hangar" } };
        auto pair_first = [](std::pair<int, std::string> p) { return p.first; };
        auto pair_second = [](std::pair<int, std::string> p) { return p.second; };

        using namespace coveo::linq;
        auto linq_m = from(v)
                    | to_map(pair_first, pair_second);
        auto it = std::begin(linq_m);
        COVEO_ASSERT(it->first == 23);
        COVEO_ASSERT(it->second == "Hangar");
        ++it;
        COVEO_ASSERT(it->first == 42);
        COVEO_ASSERT(it->second == "Life");
        COVEO_ASSERT(++it == std::end(linq_m));
    }
    {
        auto pair_first = [](std::pair<int, std::string> p) { return p.first; };

        using namespace coveo::linq;
        auto linq_m = from(std::vector<std::pair<int, std::string>>{ { 42, "Life" }, { 23, "Hangar" } })
                    | to_map(pair_first);
        auto it = std::begin(linq_m);
        COVEO_ASSERT(it->first == 23);
        COVEO_ASSERT(it->second.first == 23);
        COVEO_ASSERT(it->second.second == "Hangar");
        ++it;
        COVEO_ASSERT(it->first == 42);
        COVEO_ASSERT(it->second.first == 42);
        COVEO_ASSERT(it->second.second == "Life");
        COVEO_ASSERT(++it == std::end(linq_m));
    }

    // union_with
    {
        const std::vector<int> v1 = { 42, 23, 66, 42, 67, 66, 23, 11 };
        const std::vector<int> v2 = { 11, 7, 67, 24, 44, 42, 44 };
        const std::vector<int> v_union = { 42, 23, 66, 67, 11, 7, 24, 44 };

        using namespace coveo::linq;
        auto union1 = from(v1)
                    | union_with(v2);
        COVEO_ASSERT(detail::equal(std::begin(union1), std::end(union1),
                                   std::begin(v_union), std::end(v_union)));
        COVEO_ASSERT(!union1.has_fast_size());
        COVEO_ASSERT(union1.size() == v_union.size());

        auto union2 = from(v1)
                    | union_with(v2, [](int i, int j) { return i > j; });
        COVEO_ASSERT(detail::equal(std::begin(union2), std::end(union2),
                                   std::begin(v_union), std::end(v_union)));
        COVEO_ASSERT(!union2.has_fast_size());
        COVEO_ASSERT(union2.size() == v_union.size());
    }
    {
        std::vector<detail::two_ints> v1 = { 42, 23, 66 };
        std::vector<detail::two_ints> v2 = { 11, 7, 23, 42, 67 };
        const std::vector<int> expected1 = { 84, 46, 132 };
        const std::vector<int> expected2 = { 22, 14, 23, 42, 134 };

        using namespace coveo::linq;
        auto union1 = from(v1)
                    | union_with(v2);
        for (auto&& ti : union1) {
            ti.second *= 2;
        }
        COVEO_ASSERT(detail::seq_second_equal(v1, expected1));
        COVEO_ASSERT(detail::seq_second_equal(v2, expected2));
    }
    {
        const std::vector<int> v_union = { 42, 23, 66, 67, 11, 7, 24, 44 };

        using namespace coveo::linq;
        auto union1 = from(std::forward_list<int>{ 42, 23, 66, 42, 67, 66, 23, 11 })
                    | union_with(std::forward_list<int>{ 11, 7, 67, 24, 44, 42, 44 });
        COVEO_ASSERT(detail::equal(std::begin(union1), std::end(union1),
                                   std::begin(v_union), std::end(v_union)));
        COVEO_ASSERT(!union1.has_fast_size());
        COVEO_ASSERT(union1.size() == v_union.size());
    }

    // where
    {
        const std::vector<int> v = { 42, 23, 66, 11, 7, 24 };
        const std::vector<int> expected_odd = { 23, 11, 7 };
        const std::vector<int> expected_div_3 = { 42, 66, 24 };
        auto is_odd = [](int i) { return i % 2 != 0; };
        auto is_div_3 = [](int i) { return i % 3 == 0; };

        using namespace coveo::linq;
        auto e1 = from(v)
                | where(is_odd);
        COVEO_ASSERT(detail::equal(std::begin(e1), std::end(e1),
                                   std::begin(expected_odd), std::end(expected_odd)));
        COVEO_ASSERT(!e1.has_fast_size());
        COVEO_ASSERT(e1.size() == expected_odd.size());

        auto e2 = from(v)
                | where(is_div_3);
        COVEO_ASSERT(detail::equal(std::begin(e2), std::end(e2),
                                   std::begin(expected_div_3), std::end(expected_div_3)));
        COVEO_ASSERT(!e2.has_fast_size());
        COVEO_ASSERT(e2.size() == expected_div_3.size());
    }
    {
        std::vector<detail::two_ints> v = { 42, 23, 66, 11, 7, 24 };
        const std::vector<int> expected = { 42, 46, 66, 22, 14, 24 };
        auto is_odd = [](const detail::two_ints& ti) { return ti.first % 2 != 0; };

        using namespace coveo::linq;
        auto e = from(v)
               | where(is_odd);
        for (auto&& ti : e) {
            ti.second *= 2;
        }
        COVEO_ASSERT(detail::seq_second_equal(v, expected));
    }
    {
        const std::vector<int> expected_odd = { 23, 11, 7 };
        auto is_odd = [](int i) { return i % 2 != 0; };

        using namespace coveo::linq;
        auto e1 = from(std::forward_list<int>{ 42, 23, 66, 11, 7, 24 })
                | where(is_odd);
        COVEO_ASSERT(detail::equal(std::begin(e1), std::end(e1),
                                   std::begin(expected_odd), std::end(expected_odd)));
        COVEO_ASSERT(!e1.has_fast_size());
        COVEO_ASSERT(e1.size() == expected_odd.size());
    }

    // where_with_index
    {
        const std::vector<int> v = { 42, 23, 66, 11, 7, 24 };
        const std::vector<int> expected_odd_idx = { 23, 11, 24 };
        auto is_odd_idx = [](int, std::size_t idx) { return idx % 2 != 0; };

        using namespace coveo::linq;
        auto e = from(v)
               | where_with_index(is_odd_idx);
        COVEO_ASSERT(detail::equal(std::begin(e), std::end(e),
                                   std::begin(expected_odd_idx), std::end(expected_odd_idx)));
        COVEO_ASSERT(!e.has_fast_size());
        COVEO_ASSERT(e.size() == expected_odd_idx.size());
    }
    {
        std::vector<detail::two_ints> v = { 42, 23, 66, 11, 7, 24 };
        const std::vector<int> expected = { 42, 46, 66, 22, 7, 48 };
        auto is_odd_idx = [](const detail::two_ints&, std::size_t idx) { return idx % 2 != 0; };

        using namespace coveo::linq;
        auto e = from(v)
               | where_with_index(is_odd_idx);
        for (auto&& ti : e) {
            ti.second *= 2;
        }
        COVEO_ASSERT(detail::seq_second_equal(v, expected));
    }
    {
        const std::vector<int> expected_odd_idx = { 23, 11, 24 };
        auto is_odd_idx = [](int, std::size_t idx) { return idx % 2 != 0; };

        using namespace coveo::linq;
        auto e = from(std::forward_list<int>{ 42, 23, 66, 11, 7, 24 })
               | where_with_index(is_odd_idx);
        COVEO_ASSERT(detail::equal(std::begin(e), std::end(e),
                                   std::begin(expected_odd_idx), std::end(expected_odd_idx)));
        COVEO_ASSERT(!e.has_fast_size());
        COVEO_ASSERT(e.size() == expected_odd_idx.size());
    }

    // zip
    {
        const std::vector<int> v1 = { 42, 23, 66 };
        const std::vector<int> v2 = { 11, 7, 24, 67 };
        const std::vector<int> expected = { 53, 30, 90 };
        auto add = [](int i, int j) { return i + j; };

        using namespace coveo::linq;
        auto zipped = from(v1)
                    | zip(v2, add);
        COVEO_ASSERT(detail::equal(std::begin(zipped), std::end(zipped),
                                   std::begin(expected), std::end(expected)));
        COVEO_ASSERT(zipped.has_fast_size());
        COVEO_ASSERT(zipped.size() == expected.size());
    }
    {
        const std::vector<int> expected = { 53, 30, 90 };
        auto add = [](int i, int j) { return i + j; };

        using namespace coveo::linq;
        auto zipped = from(std::forward_list<int>{ 42, 23, 66 })
                    | zip(std::forward_list<int>{ 11, 7, 24, 67 },
                          add);
        COVEO_ASSERT(detail::equal(std::begin(zipped), std::end(zipped),
                                   std::begin(expected), std::end(expected)));
        COVEO_ASSERT(!zipped.has_fast_size());
        COVEO_ASSERT(zipped.size() == expected.size());
    }
}

// Run tests of chaining LINQ operators.
void chaining_tests()
{
    struct student {
        uint32_t id;
        bool male;
        std::string first_name;
        std::string last_name;
    };
    struct course {
        uint32_t id;
        std::string name;
    };
    struct registration {
        uint32_t student_id;
        uint32_t course_id;
    };
    const std::vector<student> v_students = {
        { 1000, true, "John", "Peterson" },
        { 1001, false, "Lynn", "Sinclair" },
        { 1002, true, "Paul", "Rickman" },
        { 1003, true, "Robert", "McFly" },
    };
    const std::vector<course> v_courses {
        { 1000, "Chemistry 1" },
        { 1001, "Mathematics 1" },
        { 1002, "Chemistry Adv. 1" },
        { 1003, "History 2" },
        { 1004, "Social Studies" },
    };
    const std::vector<registration> v_registrations = {
        { 1000, 1001 },
        { 1000, 1003 },
        { 1001, 1000 },
        { 1001, 1001 },
        { 1001, 1004 },
        { 1002, 1001 },
        { 1002, 1002 },
        { 1002, 1003 },
        { 1003, 1003 },
        { 1003, 1004 },
    };

    using namespace coveo::linq;

    // Display courses males are registered to
    {
        struct streg { student stu; registration reg; };
        struct stcourse { student stu; course c; };
        auto seq = from(v_students)
                 | where([](const student& stu) { return stu.male; })
                 | join(v_registrations,
                        [](const student& stu) { return stu.id; },
                        [](const registration& reg) { return reg.student_id; },
                        [](const student& stu, const registration& reg) {
                            return streg { stu, reg };
                        })
                 | join(v_courses,
                        [](const streg& st_reg) { return st_reg.reg.course_id; },
                        [](const course& c) { return c.id; },
                        [](const streg& st_reg, const course& c) {
                            return stcourse { st_reg.stu, c };
                        })
                 | order_by([](const stcourse& st_c) { return st_c.stu.last_name; })
                 | then_by_descending([](const stcourse& st_c) { return st_c.c.name; });

        std::cout << std::endl;
        for (auto&& st_c : seq) {
            std::cout << st_c.stu.last_name << ", "
                      << st_c.stu.first_name << "\t"
                      << st_c.c.name << std::endl;
        }
        std::cout << std::endl;
    }

    // Display how many students are registered to each course
    {
        struct regcourse { course c; uint32_t stu_id; };
        struct coursenumst { course c; std::size_t num_st; };
        auto seq = from(v_courses)
                 | group_join(v_registrations,
                              [](const course& c) { return c.id; },
                              [](const registration& reg) { return reg.course_id; },
                              [](const course& c, const coveo::enumerable<const registration>& regs) {
                                  return coursenumst { c, regs.size() };
                              })
                 | order_by([](const coursenumst& c_numst) { return c_numst.c.name; });

        std::cout << std::endl;
        for (auto&& c_numst : seq) {
            std::cout << c_numst.c.name << "\t" << c_numst.num_st << std::endl;
        }
        std::cout << std::endl;
    }

    // Let's try that again, this time taking a more convoluted route that
    // used to trigger some form of corruption on Linux because of a bug in select()
    {
        struct regcourse { course c; uint32_t stu_id; };
        struct coursenumst { course c; std::size_t num_st; };
        auto seq = from(v_registrations)
                 | join(v_courses,
                        [](const registration& reg) { return reg.course_id; },
                        [](const course& c) { return c.id; },
                        [](const registration& reg, const course& c) {
                            return regcourse { c, reg.student_id };
                        })
                 | group_values_by([](const regcourse& c_stid) { return c_stid.c; },
                                   [](const regcourse& c_stid) { return c_stid.stu_id; },
                                   [](const course& course1, const course& course2) { return course1.id < course2.id; })
                 | select([](std::tuple<course, coveo::enumerable<const uint32_t>> c_stids) {
                              auto num = from(std::get<1>(c_stids))
                                       | count();
                              return coursenumst { std::get<0>(c_stids), num };
                          })
                 | order_by([](const coursenumst& c_numst) { return c_numst.c.name; });

        const std::vector<coursenumst> expected {
            { v_courses[0], 1 },
            { v_courses[2], 1 },
            { v_courses[3], 3 },
            { v_courses[1], 3 },
            { v_courses[4], 2 },
        };
        auto expcur = std::begin(expected);
        auto expend = std::end(expected);
        auto actcur = std::begin(seq);
        auto actend = std::end(seq);
        for (; expcur != expend && actcur != actend; ++expcur, ++actcur) {
            auto&& exp = *expcur;
            auto&& act = *actcur;
            COVEO_ASSERT_EQUAL(exp.c.id, act.c.id);
            COVEO_ASSERT_EQUAL(exp.c.name, act.c.name);
            COVEO_ASSERT_EQUAL(exp.num_st, act.num_st);
        }
        COVEO_ASSERT(expcur == expend);
        COVEO_ASSERT(actcur == actend);
    }
}

// Runs tests for specific bugs
void bugs_tests()
{
    using namespace coveo::linq;

    // sequence_equal used not to work with the product of order_by
    {
        const std::vector<int> v = { 42, 23, 66 };
        auto e1 = from(v)
                | order_by([](int i) { return i; });
        auto e2 = from(v)
                | order_by([](int i) { return i; });
        COVEO_ASSERT(from(e1)
                   | sequence_equal(e2));
    }

    // Some operators used to invalidate their references when iterating,
    // breaking chaining and other stuff.
    {
        const std::forward_list<std::string> v = { "42", "23", "66" };
        auto to_int = [](const std::string& s) {
            std::istringstream iss(s);
            int i = 0;
            iss >> i;
            return i;
        };

        auto seq = from(v)
                 | select(to_int)
                 | order_by([](int i) { return i; });

        const std::vector<int> expected = { 23, 42, 66 };
        COVEO_ASSERT(detail::equal(std::begin(seq), std::end(seq),
                                   std::begin(expected), std::end(expected)));
    }
    {
        const std::forward_list<std::string> v = { "42 23", "66 67", "11 7" };
        auto to_ints = [](const std::string& s) {
            std::istringstream iss(s);
            int i = 0, j = 0;
            iss >> i >> j;
            return std::vector<int> { i, j };
        };

        auto seq = from(v)
                 | select_many(to_ints)
                 | order_by([](int i) { return i; });

        const std::vector<int> expected = { 7, 11, 23, 42, 66 , 67 };
        COVEO_ASSERT(detail::equal(std::begin(seq), std::end(seq),
                                   std::begin(expected), std::end(expected)));
    }
    {
        const std::forward_list<int> v1 = { 42, 23, 66 };
        const std::forward_list<int> v2 = { 67, 11, 7 };

        auto seq = from(v1)
                 | zip(v2, [](int i, int j) { return std::make_pair(i, j); })
                 | order_by([](const std::pair<int, int>& p) { return p.first; });

        const std::vector<std::pair<int, int>> expected = { { 23, 11 }, { 42, 67 }, { 66, 7 } };
        COVEO_ASSERT(detail::equal(std::begin(seq), std::end(seq),
                                   std::begin(expected), std::end(expected)));
    }

    // reverse used to not keep a copy of the sequence if moved,
    // resulting in an invalid sequence afterwards.
    {
        auto seq = from(std::vector<int>{ 42, 23, 66 })
                 | reverse();

        const std::vector<int> expected = { 66, 23, 42 };
        COVEO_ASSERT(detail::equal(std::begin(seq), std::end(seq),
                                   std::begin(expected), std::end(expected)));
    }
}

// Runs all benchmarks for coveo::linq operators
void linq_benchmarks()
{
    // Generate random identifiers
    std::mt19937_64 rand;
    std::uniform_int_distribution<size_t> dist_all;
    const size_t NUM_IDS = 10000000;
    std::vector<size_t> ids;
    ids.resize(NUM_IDS);
    std::generate_n(ids.begin(), NUM_IDS, std::bind(dist_all, std::ref(rand)));

    {
        std::cout << "Benchmarking reverse: LINQ version" << std::endl;
        auto start_marker = std::chrono::steady_clock::now();

        using namespace coveo::linq;
        auto seq = from(coveo::enumerate_container(ids))
                 | reverse();

        auto end_marker = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_s = end_marker - start_marker;
        std::cout << "Benchmark completed in " << elapsed_s.count() << "s" << std::endl;

        auto it = seq.begin();
        for (size_t i = 0; i < 2; ++i) {
            std::cout << *it++ << " ";
        }
        std::cout << "..." << std::endl;
    }
    {
        std::cout << "Benchmarking reverse: std version" << std::endl;
        auto start_marker = std::chrono::steady_clock::now();

        auto seq = coveo::enumerate_container(ids);
        std::vector<size_t> ids2(seq.begin(), seq.end());
        std::reverse(ids2.begin(), ids2.end());

        auto end_marker = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_s = end_marker - start_marker;
        std::cout << "Benchmark completed in " << elapsed_s.count() << "s" << std::endl;

        auto it = ids2.begin();
        for (size_t i = 0; i < 2; ++i) {
            std::cout << *it++ << " ";
        }
        std::cout << "..." << std::endl;
    }
}

} // linq
} // coveo_tests
