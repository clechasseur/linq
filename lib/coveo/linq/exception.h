/**
 * @file
 * @brief Exception classes used by LINQ operators.
 *
 * @copyright 2016-2019, Coveo Solutions Inc.
 *            Distributed under the Apache License, Version 2.0 (see <a href="https://github.com/coveo/linq/blob/master/LICENSE">LICENSE</a>).
 */

#ifndef COVEO_LINQ_EXCEPTION_H
#define COVEO_LINQ_EXCEPTION_H

#include <stdexcept>

namespace coveo {
namespace linq {

/**
 * @brief Empty sequence exception.
 * @headerfile exception.h <coveo/linq/exception.h>
 *
 * Subclass of <tt>std::logic_error</tt> used by LINQ operators
 * that cannot be applied to an empty sequence.
 *
 * @see coveo::linq::throw_linq_empty_sequence()
 */
class empty_sequence : public std::logic_error
{
public:
    empty_sequence() = delete;
    using std::logic_error::logic_error;
};

/**
 * @brief Out-of-range exception.
 * @headerfile exception.h <coveo/linq/exception.h>
 *
 * Subclass of <tt>std::out_of_range</tt> used by LINQ operators
 * when going out of a sequence's range.
 *
 * @see coveo::linq::throw_linq_out_of_range()
 */
class out_of_range : public std::out_of_range
{
public:
    out_of_range() = delete;
    using std::out_of_range::out_of_range;
};

} // linq
} // coveo

#endif // COVEO_LINQ_EXCEPTION_H
