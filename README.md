# `coveo::linq` [![Build Status](https://travis-ci.org/coveo/linq.svg?branch=master)](https://travis-ci.org/coveo/linq) [![Build status](https://ci.appveyor.com/api/projects/status/c1448sbkqpi42ufe/branch/master?svg=true)](https://ci.appveyor.com/project/clechasseur/linq-8y8vt/branch/master) [![Build status](https://ci.appveyor.com/api/projects/status/adaf1e105mcqqmpw/branch/master?svg=true)](https://ci.appveyor.com/project/clechasseur/linq/branch/master) [![GitHub license](https://img.shields.io/badge/license-Apache%202-blue.svg)](https://raw.githubusercontent.com/coveo/linq/master/LICENSE)
A C++ library implementing LINQ operators similar to .NET's, like `select`, `where`, etc.

## TL;DR
```c++
#include <coveo/linq.h>
#include <iostream>

int main()
{
    const int FIRST[] = { 42, 23, 66, 13, 11, 7, 24, 10 };
    const int SECOND[] = { 67, 22, 13, 23, 41, 66, 6, 7, 10 };

    using namespace coveo::linq;

    auto is_even = [](int i) { return i % 2 == 0; };

    auto seq = from(FIRST)
             | intersect(SECOND)                    // Intersect both arrays
             | where([](int i) { return i != 13; }) // I'm supersticious, remove 13
             | order_by_descending(is_even)         // Place even numbers first
             | then_by([](int i) { return i; });    // Then sort numbers ascending

    std::cout << std::endl;
    for (auto&& elem : seq) {
        std::cout << elem << " ";
    }
    std::cout << std::endl;
    // Prints "10 66 7 23"

    return 0;
}
```

## Installing
The library is header-only. Therefore, to add it to your project, simply copy the content of the `lib` directory to a suitable place in your structure and add that path to your include paths. Look at the `test` project/makefile for examples.

## Compiler support
`coveo::linq` requires a C++ compiler that supports the C++11 standard. It has been successfully tested with the following compilers; YMMV.

* Microsoft Visual Studio 2015 Update 3
* GCC 5.4.1
* Clang 3.4
