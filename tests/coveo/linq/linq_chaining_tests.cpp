// Copyright (c) 2019, Coveo Solutions Inc.
// Distributed under the MIT license (see LICENSE).

#include "coveo/linq/linq_chaining_tests.h"

#include <coveo/enumerable.h>
#include <coveo/linq.h>
#include <coveo/test_framework.h>

#include <cstdint>
#include <string>
#include <vector>

namespace coveo_tests {
namespace linq {
namespace detail {

// Database-like structures of students and courses
struct student {
    uint32_t id;
    bool male;
    std::string first_name;
    std::string last_name;
    bool operator==(const student& right) const {
        return id == right.id &&
               male == right.male &&
               first_name == right.first_name &&
               last_name == right.last_name;
    }
};
struct course {
    uint32_t id;
    std::string name;
    bool operator==(const course& right) const {
        return id == right.id &&
               name == right.name;
    }
};
struct registration {
    uint32_t student_id;
    uint32_t course_id;
    bool operator==(const registration& right) const {
        return student_id == right.student_id &&
               course_id == right.course_id;
    }
};
const std::vector<student> STUDENTS = {
    { 1000, true, "John", "Peterson" },
    { 1001, false, "Lynn", "Sinclair" },
    { 1002, true, "Paul", "Rickman" },
    { 1003, true, "Robert", "McFly" },
};
const std::vector<course> COURSES {
    { 1000, "Chemistry 1" },
    { 1001, "Mathematics 1" },
    { 1002, "Chemistry Adv. 1" },
    { 1003, "History 2" },
    { 1004, "Social Studies" },
};
const std::vector<registration> REGISTRATIONS = {
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

} // detail

void test_chaining_multiple_joins()
{
    using namespace coveo::linq;

    // This computes courses that males are registered to.
    struct streg { detail::student stu; detail::registration reg; };
    struct stcourse { detail::student stu; detail::course c; };
    auto seq = from(detail::STUDENTS)
             | where([](const detail::student& stu) { return stu.male; })
             | join(detail::REGISTRATIONS,
                    [](const detail::student& stu) { return stu.id; },
                    [](const detail::registration& reg) { return reg.student_id; },
                    [](const detail::student& stu, const detail::registration& reg) {
                        return streg { stu, reg };
                    })
             | join(detail::COURSES,
                    [](const streg& st_reg) { return st_reg.reg.course_id; },
                    [](const detail::course& c) { return c.id; },
                    [](const streg& st_reg, const detail::course& c) {
                        return stcourse { st_reg.stu, c };
                    })
             | order_by([](const stcourse& st_c) { return st_c.stu.last_name; })
             | then_by_descending([](const stcourse& st_c) { return st_c.c.name; });

    const std::vector<stcourse> expected = {
        { detail::STUDENTS[3], detail::COURSES[4] },
        { detail::STUDENTS[3], detail::COURSES[3] },
        { detail::STUDENTS[0], detail::COURSES[1] },
        { detail::STUDENTS[0], detail::COURSES[3] },
        { detail::STUDENTS[2], detail::COURSES[1] },
        { detail::STUDENTS[2], detail::COURSES[3] },
        { detail::STUDENTS[2], detail::COURSES[2] },
    };
    auto expcur = std::begin(expected);
    auto expend = std::end(expected);
    auto actcur = std::begin(seq);
    auto actend = std::end(seq);
    for (; expcur != expend && actcur != actend; ++expcur, ++actcur) {
        auto&& exp = *expcur;
        auto&& act = *actcur;
        COVEO_ASSERT(act.stu == exp.stu);
        COVEO_ASSERT(act.c == exp.c);
    }
    COVEO_ASSERT(expcur == expend);
    COVEO_ASSERT(actcur == actend);
}

void test_chaining_group_join()
{
    using namespace coveo::linq;

    // This computes how many students are registered to each course.
    struct regcourse { detail::course c; uint32_t stu_id; };
    struct coursenumst { detail::course c; std::size_t num_st; };
    auto seq = from(detail::COURSES)
             | group_join(detail::REGISTRATIONS,
                          [](const detail::course& c) { return c.id; },
                          [](const detail::registration& reg) { return reg.course_id; },
                          [](const detail::course& c, const coveo::enumerable<const detail::registration>& regs) {
                              return coursenumst { c, regs.size() };
                          })
             | order_by([](const coursenumst& c_numst) { return c_numst.c.name; });

    const std::vector<coursenumst> expected {
        { detail::COURSES[0], 1 },
        { detail::COURSES[2], 1 },
        { detail::COURSES[3], 3 },
        { detail::COURSES[1], 3 },
        { detail::COURSES[4], 2 },
    };
    auto expcur = std::begin(expected);
    auto expend = std::end(expected);
    auto actcur = std::begin(seq);
    auto actend = std::end(seq);
    for (; expcur != expend && actcur != actend; ++expcur, ++actcur) {
        auto&& exp = *expcur;
        auto&& act = *actcur;
        COVEO_ASSERT(act.c == exp.c);
        COVEO_ASSERT_EQUAL(exp.num_st, act.num_st);
    }
    COVEO_ASSERT(expcur == expend);
    COVEO_ASSERT(actcur == actend);
}

void test_chaining_convoluted_joins()
{
    using namespace coveo::linq;

    // This is similar to test_group_join, but more convoluted.
    // This used to trigger a corruption bug in select().
    struct regcourse { detail::course c; uint32_t stu_id; };
    struct coursenumst { detail::course c; std::size_t num_st; };
    auto seq = from(detail::REGISTRATIONS)
             | join(detail::COURSES,
                    [](const detail::registration& reg) { return reg.course_id; },
                    [](const detail::course& c) { return c.id; },
                    [](const detail::registration& reg, const detail::course& c) {
                        return regcourse { c, reg.student_id };
                    })
             | group_values_by([](const regcourse& c_stid) { return c_stid.c; },
                               [](const regcourse& c_stid) { return c_stid.stu_id; },
                               [](const detail::course& course1, const detail::course& course2) {
                                   return course1.id < course2.id;
                               })
             | select([](std::tuple<detail::course, coveo::enumerable<const uint32_t>> c_stids) {
                          auto num = from(std::get<1>(c_stids))
                                   | count();
                          return coursenumst { std::get<0>(c_stids), num };
                      })
             | order_by([](const coursenumst& c_numst) { return c_numst.c.name; });

    const std::vector<coursenumst> expected {
        { detail::COURSES[0], 1 },
        { detail::COURSES[2], 1 },
        { detail::COURSES[3], 3 },
        { detail::COURSES[1], 3 },
        { detail::COURSES[4], 2 },
    };
    auto expcur = std::begin(expected);
    auto expend = std::end(expected);
    auto actcur = std::begin(seq);
    auto actend = std::end(seq);
    for (; expcur != expend && actcur != actend; ++expcur, ++actcur) {
        auto&& exp = *expcur;
        auto&& act = *actcur;
        COVEO_ASSERT(act.c == exp.c);
        COVEO_ASSERT_EQUAL(exp.num_st, act.num_st);
    }
    COVEO_ASSERT(expcur == expend);
    COVEO_ASSERT(actcur == actend);
}

void chaining_tests()
{
    test_chaining_multiple_joins();
    test_chaining_group_join();
    test_chaining_convoluted_joins();
}

} // linq
} // coveo_tests
