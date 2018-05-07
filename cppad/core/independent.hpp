# ifndef CPPAD_CORE_INDEPENDENT_HPP
# define CPPAD_CORE_INDEPENDENT_HPP

/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-18 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the
                    Eclipse Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */
/*
---------------------------------------------------------------------------

$begin Independent$$
$spell
	op
	alloc
	num
	Cpp
	bool
	const
	var
	typename
$$

$section Declare Independent Variables and Start Recording$$

$head Syntax$$
$codei%Independent(%x%)
%$$
$codei%Independent(%x%, %record_compare%)
%$$
$codei%Independent(%x%, %abort_op_index%)
%$$

$head Purpose$$
Start recording
$cref/AD of Base/glossary/AD of Base/$$ operations
with $icode x$$ as the independent variable vector.
Once the
$cref/operation sequence/glossary/Operation/Sequence/$$ is completed,
it must be transferred to a function object; see below.

$head Start Recording$$
An operation sequence recording is started by the commands
$codei%
	Independent(%x%)
	Independent(%x%, %record_compare%)
	Independent(%x%, %abort_op_index%)
%$$

$head Stop Recording$$
The recording is stopped,
and the operation sequence is transferred to the AD function object $icode f$$,
using either the $cref/function constructor/FunConstruct/$$
$codei%
	ADFun<%Base%> %f%(%x%, %y%)
%$$
or the $cref/dependent variable specifier/Dependent/$$
$codei%
	%f%.Dependent(%x%, %y%)
%$$
The only other way to stop a recording is using
$cref abort_recording$$.
Between when the recording is started and when it stopped,
we refer to the elements of $icode x$$,
and the values that depend on the elements of $icode x$$,
as $codei%AD<%Base%>%$$ variables.

$head x$$
The vector $icode x$$ has prototype
$codei%
	%VectorAD% &%x%
%$$
(see $icode VectorAD$$ below).
The size of the vector $icode x$$, must be greater than zero,
and is the number of independent variables for this
AD operation sequence.

$head record_compare$$
This argument has prototype
$codei%
	bool %record_compare%
%$$
If it is present,
it specifies if AD $cref compare$$  operations are recorded.
It takes extra time and memory to record these operations.
On the other hand, they can be useful for detecting when and why
a functions recording would change; see $icode abort_op_index$$ below and
$cref compare_change$$.
If this argument is not present, the default value $code true$$ is used
for $icode record_compare$$.

$head abort_op_index$$
If this argument is present,
it specifies the operator index at which the execution is be aborted
by calling the CppAD $cref/error handler/ErrorHandler/$$.
When this error handler leads to an assert, the user
can inspect the call stack to see the source code corresponding to
this operator index; see
$cref/purpose/compare_change/op_index/Purpose/$$.
No abort will occur if $icode abort_op_index$$ is zero,
if $cref/NDEBUG/Faq/Speed/NDEBUG/$$ is defined, or
if the $icode record_compare$$ argument above is false.
If this argument is not present, the default value zero is used
for $icode abort_index$$.

$head VectorAD$$
The type $icode VectorAD$$ must be a $cref SimpleVector$$ class with
$cref/elements of type/SimpleVector/Elements of Specified Type/$$
$codei%AD<%Base%>%$$.
The routine $cref CheckSimpleVector$$ will generate an error message
if this is not the case.

$head Parallel Mode$$
Each thread can have one, and only one, active recording.
A call to $code Independent$$ starts the recording for the current thread.
The recording must be stopped by a corresponding call to
$codei%
	ADFun<%Base%> %f%( %x%, %y%)
%$$
or
$codei%
	%f%.Dependent( %x%, %y%)
%$$
or $cref abort_recording$$
preformed by the same thread; i.e.,
$cref/thread_alloc::thread_num/ta_thread_num/$$ must be the same.

$head Example$$
$children%
	example/general/independent.cpp
%$$
The file
$cref independent.cpp$$
contains an example and test of this operation.
It returns true if it succeeds and false otherwise.

$end
-----------------------------------------------------------------------------
*/
# include <cppad/local/independent.hpp>
/*!
\file core/independent.hpp
Declare the independent variables
*/

namespace CppAD { // BEGIN_CPPAD_NAMESPACE

/*!
Declaration of independent variables.

\tparam VectorAD
This is simple vector type with elements of type AD<Base>.

\param x
Vector of the independent variablers.

\param record_compare
should comparison operators be recorded.

\param abort_op_index
operator index at which execution will be aborted (during  the recording
of operations). The value zero corresponds to not aborting (will not match).
*/
template <typename VectorAD>
inline void Independent(
	VectorAD &x           ,
	bool record_compare   ,
	size_t abort_op_index )
{	typedef typename VectorAD::value_type ADBase;
	typedef typename ADBase::value_type   Base;
	CPPAD_ASSERT_KNOWN(
		ADBase::tape_ptr() == CPPAD_NULL,
		"Independent: cannot create a new tape because\n"
		"a previous tape is still active (for this thread).\n"
		"AD<Base>::abort_recording() would abort this previous recording."
	);
	local::ADTape<Base>* tape = ADBase::tape_manage(tape_manage_new);
	tape->Independent(x, record_compare, abort_op_index);
}
/*!
Declaration of independent variables using default for
record_compare and abort_op_index.

\tparam VectorAD
This is simple vector type with elements of type AD<Base>.

\param x
Vector of the independent variablers.
*/
template <typename VectorAD>
inline void Independent(VectorAD &x)
{	bool   record_compare = true;
	size_t abort_op_index = 0;
	Independent(x, record_compare, abort_op_index);
}
/*!
Declaration of independent variables using default for abort_op_index.

\tparam VectorAD
This is simple vector type with elements of type AD<Base>.

\param x
Vector of the independent variablers.
*/
template <typename VectorAD>
inline void Independent(VectorAD &x, bool record_compare)
{	size_t abort_op_index = 0;
	Independent(x, record_compare, abort_op_index);
}
/*!
Declaration of independent variables using default for record_compare.

\tparam VectorAD
This is simple vector type with elements of type AD<Base>.

\param x
Vector of the independent variablers.
*/
template <typename VectorAD>
inline void Independent(VectorAD &x, size_t abort_op_index)
{	bool   record_compare = true;
	Independent(x, record_compare, abort_op_index);
}

} // END_CPPAD_NAMESPACE

# endif