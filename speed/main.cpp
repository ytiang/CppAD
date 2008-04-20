/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-08 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the 
                    Common Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */

# include <cstring>
# include <cstdlib>
# include <cassert>
# include <cstddef>
# include <iostream>
# include <cppad/vector.hpp>
# include <cppad/speed/det_grad_33.hpp>
# include <cppad/speed/det_33.hpp>
# include <cppad/speed_test.hpp>
# include <cppad/speed/uniform_01.hpp>
# include <cppad/poly.hpp>

# ifdef ADOLC
# define AD_PACKAGE "adolc"
# endif
# ifdef CPPAD
# define AD_PACKAGE "cppad"
# endif
# ifdef DOUBLE
# define AD_PACKAGE "double"
# endif
# ifdef FADBAD
# define AD_PACKAGE "fadbad"
# endif
# ifdef PROFILE
# define AD_PACKAGE "profile"
# endif
# ifdef SACADO
# define AD_PACKAGE "sacado"
# endif

/*
$begin speed_main$$
$spell
	bool
	ddp
	cppad
	adolc
	fadbad
	sacado
	CppAD
	det
	lu
$$

$index cppad, speed test$$
$index speed, test cppad$$
$index test, cppad speed$$

$section Speed Testing Main Program$$

$head Syntax$$
$syntax%speed/%package%/%package% %option% %seed%
%$$

$head Purpose$$
A version of this program runs the correctness tests
or the speed tests for one AD package identified by $italic package$$.

$head package$$

$subhead AD Package$$
The value
$italic package$$ can specify one of the following AD packages:
$cref/adolc/speed_adolc/$$,
$cref/cppad/speed_cppad/$$, 
$cref/fadbad/speed_fadbad/$$, 
$cref/sacado/speed_sacado/$$.

$subhead double$$
The value
$italic package$$ can be $code double$$ in which case
the function values (instead of derivatives) are computed
using double precision operations.
This enables one to compare the speed of computing function
values in $code double$$ to the speed of the derivative computations.
(It is often useful to divide the speed of the derivative computation by
the speed of the function evaluation in $code double$$.) 

$subhead profile$$
In the special case where $italic package$$ is $code profile$$,
the CppAD package is compiled and run with profiling to aid in determining
where it is spending most of its time.

$head option$$
It the argument $italic option$$ specifies which test to run
and has the following possible values:
$cref/correct/speed_main/option/correct/$$,
$cref/speed/speed_main/option/speed/$$,
$cref/det_minor/link_det_minor/$$,
$cref/det_lu/link_det_lu/$$,
$cref/poly/link_poly/$$,
$cref/sparse_hessian/link_sparse_hessian/$$.

$subhead correct$$
If $italic option$$ is equal to $code correct$$,
all of the correctness tests are run.

$subhead speed$$
If $italic option$$ is equal to $code speed$$,
all of the speed tests are run.

$head Return Values$$
Each of the $italic option$$ values specified below
corresponds to a particular test.
The function call corresponding to each test
has a $code bool$$ return value.
If a particular $italic package$$ supports a test,
it should return $code true$$ when the corresponding function is called.
Otherwise (if the return value is $code false$$),
the test is skipped whenever the corresponding
$italic package$$ and $italic option$$ are specified.

$head seed$$
$index uniform_01$$
The random number simulator $cref/uniform_01/$$ is initialized with
the call
$syntax%
	uniform_01(%seed%)
%$$
before any of the testing routines (listed above) are called.

$head Correctness Results$$
An output line is generated for each correctness test
stating of the test passed or failed.

$head Speed Results$$
For each speed test, corresponds to three lines of output.
The name of the package and test are printed on the first line,
the vector of problem sizes are printed on the next line,
and the rates corresponding to the different problem sizes are
printed on the third line.
The rate is the number of times per second that the calculation was repeated.


$childtable%
	speed/link_det_lu.cpp%
	speed/link_det_minor.cpp%
	speed/link_poly.cpp%
	speed/link_sparse_hessian.cpp%
	speed/link_ode.cpp
%$$

$end 
-----------------------------------------------------------------------------
*/

# define CPPAD_DECLARE_SPEED(name)                          \
	extern bool available_##name(void);                 \
	extern bool correct_##name(bool is_package_double); \
	extern void speed_##name(size_t size, size_t repeat)

CPPAD_DECLARE_SPEED(det_lu);
CPPAD_DECLARE_SPEED(det_minor);
CPPAD_DECLARE_SPEED(poly);
CPPAD_DECLARE_SPEED(sparse_hessian);

namespace {
	using std::cout;
	using std::endl;

	// ------------------------------------------------------
	// output vector in form readable by octave or matlab
	// convert size_t to int to avoid warning by MS compiler
	void output(const CppAD::vector<size_t> &v)
	{	size_t i= 0, n = v.size();
		cout << "[ ";
		while(i < n)
		{	cout << int(v[i++]);
			if( i < n )
				cout << ", ";
		}
		cout << " ]";
	}

	// ----------------------------------------------------------------
	// function that runs one correctness case
	static size_t Run_ok_count    = 0;
	static size_t Run_error_count = 0;
	bool run_correct(bool correct_case(bool), const char *case_name)
	{	bool ok;
		cout << AD_PACKAGE << "_" << case_name << "_ok = ";
# ifdef DOUBLE
		ok = correct_case(true);
# else
		ok = correct_case(false);
# endif
		if( ok )
		{	cout << " true" << endl;
			Run_ok_count++;
		}
		else
		{	cout << " false" << endl;
			Run_error_count++;
		}
		return ok;
	}
	// ----------------------------------------------------------------
	// function that runs one speed case
	void Run_speed(
		void speed_case(size_t size, size_t repeat) , 
		CppAD::vector<size_t>              size_vec ,
		std::string                       case_name )
	{	double time_min = 1.;
		cout << AD_PACKAGE << "_" << case_name << "_size = ";
		output(size_vec);
		cout << endl;

		CppAD::vector<size_t> rate_vec( size_vec.size() );
		rate_vec = CppAD::speed_test(speed_case, size_vec, time_min);
		cout << AD_PACKAGE << "_" << case_name << "_rate = ";
		output(rate_vec);
		cout << endl;
		return;
	}
}

// main program that runs all the tests
int main(int argc, char *argv[])
{	bool ok = true;

	const char *option[]= {
		"correct",
		"speed",
		"det_lu",
		"det_minor",
		"poly",
		"sparse_hessian"
	};
	const size_t n_option  = sizeof(option) / sizeof(option[0]);
	const size_t option_correct   = 0;
	const size_t option_speed     = 1;
	const size_t option_det_lu    = 2;
	const size_t option_det_minor = 3;
	const size_t option_poly      = 4;
	const size_t option_sparse_hessian = 5;
	assert( n_option == option_sparse_hessian+1 );

	size_t i;
	size_t match = n_option;
	int    iseed = 0;
	if( argc == 3 )
	{	for(i = 0; i < n_option; i++)
			if( strcmp(option[i], argv[1]) == 0 )
				match = i;
		iseed = std::atoi( argv[2] );
	}
	if( match == n_option  || iseed <= 0 )
	{	cout << "usage: ./" << AD_PACKAGE << " option seed" << endl;
		cout << "option choices: " << option[0];
		for(i = 1; i < n_option; i++)
			cout << ", " << option[i];
		cout << "." << endl << "seed value: ";
		cout << "a positive integer used as a random seed." << endl;
		cout << endl;
		return 1;
	}


	// initialize the random number simulator
	CppAD::uniform_01(size_t(iseed));

	// arguments needed for speed tests
	size_t n_size   = 5;
	CppAD::vector<size_t> size_det_lu(n_size);
	CppAD::vector<size_t> size_det_minor(n_size);
	CppAD::vector<size_t> size_poly(n_size);
	CppAD::vector<size_t> size_sparse_hessian(n_size);
	for(i = 0; i < n_size; i++) 
	{	size_det_lu[i]      = 3 * i + 1;
		size_det_minor[i]   = i + 1;
		size_poly[i]        = 8 * i + 1;
		size_sparse_hessian[i] = 10 * (i + 1);
	}
# ifndef NDEBUG
	size_t base_count = CPPAD_TRACK_COUNT();
# endif

	switch(match)
	{
		// run all the correctness tests
		case option_correct:
		if( available_det_lu() ) ok &= run_correct(
			correct_det_lu,    "det_lu"       
		);
		if( available_det_minor() ) ok &= run_correct(
			correct_det_minor, "det_minor"    
		);
		if( available_poly() ) ok &= run_correct(
			correct_poly,      "poly"         
		);
		if( available_sparse_hessian() ) ok &= run_correct(
			correct_sparse_hessian, "sparse_hessian"         
		);
		// summarize results
		assert( ok || (Run_error_count > 0) );
		if( ok )
		{	cout	<< "All " << int(Run_ok_count) 
				<< " correctness tests passed." << endl;
		}
		else
		{	cout	<< int(Run_error_count) 
				<< " correctness tests failed." << endl;
		}
		break;
		// ---------------------------------------------------------
		// run all the speed tests 
		case option_speed:
		if( available_det_lu() ) Run_speed(
		speed_det_lu,    size_det_lu,    "det_lu"
		);
		if( available_det_minor() ) Run_speed(
		speed_det_minor, size_det_minor, "det_minor"
		);
		if( available_poly() ) Run_speed(
		speed_poly,      size_poly,      "poly"
		);
		if( available_sparse_hessian() ) Run_speed(
		speed_sparse_hessian,  size_sparse_hessian,   "sparse_hessian"
		);
		ok = true;
		break;
		// ---------------------------------------------------------

		case option_det_lu:
		if( ! available_det_lu() )
		{	cout << AD_PACKAGE << ": option " << argv[1] 
			     << " not available" << endl; 
			exit(1);
		}
		ok &= run_correct(correct_det_lu,           "det_lu");
		Run_speed(speed_det_lu,    size_det_lu,     "det_lu");
		break;
		// ---------------------------------------------------------

		case option_det_minor:
		if( ! available_det_minor() )
		{	cout << AD_PACKAGE << ": option " << argv[1] 
			     << " not available" << endl; 
			exit(1);
		}
		ok &= run_correct(correct_det_minor,       "det_minor");
		Run_speed(speed_det_minor, size_det_minor, "det_minor");
		break;
		// ---------------------------------------------------------

		case option_poly:
		if( ! available_poly() )
		{	cout << AD_PACKAGE << ": option " << argv[1] 
			     << " not available" << endl; 
			exit(1);
		}
		ok &= run_correct(correct_poly,            "poly");
		Run_speed(speed_poly,      size_poly,      "poly");
		break;
		// ---------------------------------------------------------

		case option_sparse_hessian:
		if( ! available_sparse_hessian() )
		{	cout << AD_PACKAGE << ": option " << argv[1] 
			     << " not available" << endl; 
			exit(1);
		}
		ok &= run_correct(correct_sparse_hessian, "sparse_hessian");
		Run_speed(
		speed_sparse_hessian, size_sparse_hessian,  "sparse_hessian");
		break;
		// ---------------------------------------------------------
		
		default:
		assert(0);
	}
# ifndef NDEBUG
	if( CPPAD_TRACK_COUNT() == base_count )
	{	Run_ok_count++;
		cout << "No memory leak detected" << endl;
	}
	else
	{	ok = false;
		Run_error_count++;
		cout << "Memory leak detected" << endl;
        }
# endif
	return static_cast<int>( ! ok );
}
