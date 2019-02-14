/**
 * @file
 * @copyright 2016-2019, Coveo Solutions Inc.
 *            Distributed under the Apache License, Version 2.0 (see <a href="https://github.com/coveo/linq/blob/master/LICENSE">LICENSE</a>).
 */

#include <coveo/linq/all_tests.h>
#include <coveo/test_framework.h>

#include <iostream>
#include <string>

// Test program entry point.
int main()
{
    int ret = 0;

#ifndef COVEO_LINQ_BENCHMARKS
    std::cout << "Running tests..." << std::endl;
    ret = coveo_tests::run_tests(&coveo_tests::linq::all_tests);
#else
    std::cout << "Running benchmarks..." << std::endl;
    ret = coveo_tests::run_tests(&coveo_tests::linq::all_benchmarks);
#endif
    std::cout << "Done." << std::endl;
    
    return ret;
}
