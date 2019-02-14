/**
 * @file
 * @copyright 2016-2019, Coveo Solutions Inc.
 *            Distributed under the Apache License, Version 2.0 (see <a href="https://github.com/coveo/linq/blob/master/LICENSE">LICENSE</a>).
 */

#include "coveo/linq/all_tests.h"

#include <coveo/seq/enumerable_tests.h>
#include <coveo/linq/linq_operator_tests.h>
#include <coveo/linq/linq_chaining_tests.h>

namespace coveo_tests {
namespace linq {

// Runs all tests for coveo::enumerable and coveo::linq
void all_tests()
{
    // enumerable
    enumerable_tests();

    // linq
    operator_tests();
    chaining_tests();
    bugs_tests();
}

// Runs all benchmarks for coveo::enumerable and coveo::linq
void all_benchmarks()
{
    // linq
    linq_benchmarks();
}

} // linq
} // coveo_tests
