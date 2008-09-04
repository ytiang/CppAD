# ifndef CPPAD_IPOPT_CPPAD_NLP_INCLUDED
# define CPPAD_IPOPT_CPPAD_NLP_INCLUDED
/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-08 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the 
                    Common Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */
/*
$begin ipopt_cppad_nlp$$
$spell
	bool
	doesn't
	nan
	inf
	naninf
	std
	maxiter
	infeasibility
	obj
	const
	optimizer
	ipopt_cppad_nlp.hpp
	fg_info.eval
	retape
	CppAD
	
$$
$section Nonlinear Programming Using Ipopt and CppAD$$

$head Syntax$$
$codei%# include "ipopt_cppad_nlp.hpp"
%$$
$codei%# ipopt_cppad_solution %solution%;
%$$
$codei%ipopt_cppad_nlp %cppad_nlp%(
	%n%, %m%, %x_i%, %x_l%, %x_u%, %g_l%, %g_u%, %fg_info%, &%solution%
)%$$

$head Purpose$$
The class $code ipopt_cppad_nlp$$ is used to solve nonlinear programming
problems of the form
$latex \[
\begin{array}{rll}
{\rm minimize}      & f(x) 
\\
{\rm subject \; to} & g^l \leq g(x) \leq g^u
\\
                    & x^l  \leq x   \leq x^u
\end{array}
\] $$
This is done using 
$href%
	https://www.coin-or.org/projects/Ipopt%
	Ipopt
%$$
optimizer and 
$href%
	http://www.coin-or.org/CppAD/%
	CppAD
%$$
Algorithmic Differentiation package.

$head Warning$$
This is only and example use of CppAD.
It is expected that this class will be improved and that
its user interface may change in ways that are not backward compatible.

$head fg(x)$$
The function $latex fg : \R^n \rightarrow \R^{m+1}$$ is defined by
$latex \[
\begin{array}{rcl}
	fg_0 (x)     & = & f(x)         \\
	fg_1 (x)     & = & g_0 (x)      \\
	             & \vdots &         \\
	fg_m (x)     & = & g_{m-1} (x)
	\end{array}
\] $$

$head NumberVector$$
The type $codei NumberVector$$ is defined by the 
$codei ipopt_cppad_nlp.hpp$$ include file to be a 
$cref/SimpleVector/$$ class with elements of type
$code Ipopt::Number$$.

$head ADNumber$$
The type $codei ADNumber$$ is defined by the 
$codei ipopt_cppad_nlp.hpp$$ include file to be a 
an AD type that can be used to compute derivatives.

$head ADVector$$
The type $codei ADVector$$ is defined by the 
$codei ipopt_cppad_nlp.hpp$$ include file to be a 
$cref/SimpleVector/$$ class with elements of type
$code ADNumber$$. 

$head n$$
The argument $icode n$$ has prototype
$codei%
	size_t %n%
%$$
It specifies the dimension of the argument space; 
i.e., $latex x \in \R^n$$.

$head m$$
The argument $icode m$$ has prototype
$codei%
	size_t %m%
%$$
It specifies the dimension of the range space for $latex g$$; 
i.e., $latex g : \R^n \rightarrow \R^m$$.

$head x_i$$
The argument $icode x_i$$ has prototype
$codei%
	const NumberVector& %x_i%
%$$
and its size is equal to $latex n$$.
It specifies the initial point where Ipopt starts the optimization process.

$head x_l$$
The argument $icode x_l$$ has prototype
$codei%
	const NumberVector& %x_l%
%$$
and its size is equal to $latex n$$.
It specifies the lower limits for the argument in the optimization problem;
i.e., $latex x^l$$.

$head x_u$$
The argument $icode x_u$$ has prototype
$codei%
	const NumberVector& %x_u%
%$$
and its size is equal to $latex n$$.
It specifies the upper limits for the argument in the optimization problem;
i.e., $latex x^u$$.

$head g_l$$
The argument $icode g_l$$ has prototype
$codei%
	const NumberVector& %g_l%
%$$
and its size is equal to $latex m$$.
It specifies the lower limits for the constraints in the optimization problem;
i.e., $latex g^l$$.

$head g_u$$
The argument $icode g_u$$ has prototype
$codei%
	const NumberVector& %g_u%
%$$
and its size is equal to $latex n$$.
It specifies the upper limits for the constraints in the optimization problem;
i.e., $latex g^u$$.

$head fg_info$$
The argument $icode fg_info$$ has prototype
$codei%
	ipopt_cppad_fg_info* %fg_info%
%$$
where the object $codei%*%fg_info%$$ is a member of
a class (referred to as $icode FG_info$$)
that is derived from the base class $code ipopt_cppad_fg_info$$.
Certain virtual member functions of $code ipopt_cppad_fg_info$$ are used to 
compute the value of $latex fg(x)$$.
The specifications for these member functions is given below:

$subhead fg_info->eval$$
This member function has the prototype
$codei%
	virtual ADVector ipopt_cppad_fg_info::fg_eval(const ADVector& %x%) = 0;
%$$
This prototype is pure virtual and hence it must be defined in the 
derived class $icode FG_info$$.
$pre

$$
This function computes the value of $cref/fg(x)/ipopt_cppad_nlp/fg(x)/$$.
If $icode x$$ is an $code ADVector$$ of size $icode n$$ 
and $icode fg$$ is an $code ADVector$$ of size $icode m+1$$
the syntax
$codei%
	%fg% = %fg_info%->eval(%x%)
%$$
set $icode fg$$ to the vector $latex fg(x)$$.

$subhead fg_info->retape$$
This member function has the prototype
$codei%
	virtual bool ipopt_cppad_fg_info::retape(void)
%$$
If $icode retape$$ has type $code bool$$,
the syntax
$codei%
        %retape% = %fg_info%->retape()
%$$
sets $icode retape$$ to true or false.
If $icode retape$$ is true, 
$code ipopt_cppad_nlp$$ will retape the operation sequence for
every new $icode x$$ value. 
The program should use much less memory and run faster if $icode retape$$
is false.
You can test both the true and false cases to make sure 
the operation sequence does not depend on $icode x$$.
$pre

$$
The $code ipopt_cppad_fg_info$$ implementation of this function
sets $icode retape$$ to true.

$head solution$$
After the optimization process is completed, $icode solution$$ contains
the following information:

$subhead status$$
The $icode status$$ field of $icode solution$$ has prototype
$codei%
	ipopt_cppad_solution::solution_status %solution%.status
%$$
It is the final Ipopt status for the optimizer. 
Here is a list of the possible values for the status:

$table
$icode status$$ $cnext Meaning
$rnext
not_defined $cnext
The optimizer did not return a final status to this $code ipopt_cppad_nlp$$
object.
$rnext
unknown $cnext
The status returned by the optimizer is not defined in the Ipopt
documentation for $code finalize_solution$$.
$rnext
success $cnext
Algorithm terminated successfully at a point satisfying the convergence 
tolerances (see Ipopt options).
$rnext
maxiter_exceeded $cnext
The maximum number of iterations was exceeded (see Ipopt options).
$rnext
stop_at_tiny_step $cnext
Algorithm terminated because progress was very slow.
$rnext
stop_at_acceptable_point $cnext
Algorithm stopped at a point that was converged, 
not to the 'desired' tolerances, but to 'acceptable' tolerances 
(see Ipopt options).
$rnext
local_infeasibility $cnext
Algorithm converged to a non-feasible point
(problem may have no solution).
$rnext
user_requested_stop $cnext
This return value should not happen.
$rnext
diverging_iterates $cnext
It the iterates are diverging.
$rnext
restoration_failure $cnext
Restoration phase failed, algorithm doesn't know how to proceed.
$rnext
error_in_step_computation $cnext
An unrecoverable error occurred while Ipopt tried to 
compute the search direction.
$rnext
invalid_number_detected $cnext
Algorithm received an invalid number (such as $code nan$$ or $code inf$$) 
from the users function $icode%fg_info%.eval%$$ or from the CppAD evaluations
of its derivatives
(see the Ipopt option $code check_derivatives_for_naninf$$).
$rnext
internal_error $cnext
An unknown Ipopt internal error occurred.
Contact the Ipopt authors through the mailing list.
$tend

$subhead x$$
The $code x$$ field of $icode solution$$ has prototype
$codei%
	NumberVector %solution%.x
%$$
and its size is equal to $latex n$$.
It is the final $latex x$$ value for the optimizer.

$subhead z_l$$
The $code z_l$$ field of $icode solution$$ has prototype
$codei%
	NumberVector %solution%.z_l
%$$
and its size is equal to $latex n$$.
It is the final Lagrange multipliers for the 
lower bounds on $latex x$$.

$subhead z_u$$
The $code z_u$$ field of $icode solution$$ has prototype
$codei%
	NumberVector %solution%.z_u
%$$
and its size is equal to $latex n$$.
It is the final Lagrange multipliers for the 
upper bounds on $latex x$$.

$subhead g$$
The $code g$$ field of $icode solution$$ has prototype
$codei%
	NumberVector %solution%.g
%$$
and its size is equal to $latex m$$.
It is the final value for the constraint function $latex g(x)$$.

$subhead lambda$$
The $code lambda$$ field of $icode solution$$ has prototype
$codei%
	NumberVector %solution%.lambda
%$$
and its size is equal to $latex m$$.
It is the final value for the 
Lagrange multipliers corresponding to the constraint function.

$subhead obj_value$$
The $code obj_value$$ field of $icode solution$$ has prototype
$codei%
	Number %solution%.obj_value
%$$
It is the final value of the objective function $latex f(x)$$.


$children%
	example/ipopt_cppad.cpp
%$$

$head Example$$
The file 
$cref/ipopt_cppad.cpp/$$ is an example and test of $code ipopt_cppad_nlp$$.
It returns true if it succeeds and false otherwise.

$end
-----------------------------------------------------------------------------
*/


# include <cppad/cppad.hpp>
# include <coin/IpIpoptApplication.hpp>
# include <coin/IpTNLP.hpp>

typedef CppAD::AD<Ipopt::Number>       ADNumber;
typedef CppAD::vector<size_t>          SizeVector;
typedef CppAD::vector<Ipopt::Number>   NumberVector;
typedef CppAD::vector<ADNumber>        ADVector;

/*
Class for return solution values.
*/
class ipopt_cppad_fg_info
{
public:
	// constructor
	ipopt_cppad_fg_info(void)
	{ }
	// make destructor virtual so that derived class destructor gets called
	virtual ~ipopt_cppad_fg_info(void)
	{ }
	// default definition for retape
	virtual bool retape(void)
	{	return true; }
	// fg_eval is pure virtual so that it must be defined by derived class
	virtual ADVector fg_eval(const ADVector& x) = 0;
};

class ipopt_cppad_solution 
{
public:
	enum solution_status {
		not_defined,
		success,
		maxiter_exceeded,
		stop_at_tiny_step,
		stop_at_acceptable_point,
		local_infeasibility,
		user_requested_stop,
		feasible_point_found,
		diverging_iterates,
		restoration_failure,
		error_in_step_computation,
		invalid_number_detected,
		too_few_degrees_of_freedom,
		internal_error,
		unknown
	}  status;
	NumberVector      x;
	NumberVector      z_l;
	NumberVector      z_u;
	NumberVector      g;
	NumberVector      lambda;
	Ipopt::Number     obj_value;

	ipopt_cppad_solution(void)
	{	status = not_defined; }
};

/* 
Class for interfacing a problem to IPOPT and using CppAD for derivative 
and sparsity pattern calculations.
*/
class ipopt_cppad_nlp : public Ipopt::TNLP
{
	typedef Ipopt::Number                  Number;
	typedef Ipopt::Index                   Index;
	typedef Ipopt::TNLP::IndexStyleEnum    IndexStyleEnum;
	typedef CppAD::vectorBool              BoolVector;
	typedef CppAD::vector<Ipopt::Index>    IndexVector;
public:
	// constructor 
	ipopt_cppad_nlp(
		size_t n                         , 
		size_t m                         ,
		const NumberVector    &x_i       ,
		const NumberVector    &x_l       ,
		const NumberVector    &x_u       ,
		const NumberVector    &g_l       ,
		const NumberVector    &g_u       ,
		ipopt_cppad_fg_info*   fg_info   ,
		ipopt_cppad_solution*  solution
  	);


	// default destructor 
	virtual ~ipopt_cppad_nlp();

	// return info about the nlp
	virtual bool get_nlp_info(
		Index&          n           , 
		Index&          m           , 
		Index&          nnz_jac_g   ,
		Index&          nnz_h_lag   , 
		IndexStyleEnum& index_style
	);

	// return bounds for my problem 
	virtual bool get_bounds_info(
		Index           n   , 
		Number*         x_l , 
		Number*         x_u ,
		Index           m   , 
		Number*         g_l , 
		Number*         g_u   
	);

	// return the starting point for the algorithm 
	virtual bool get_starting_point(
		Index          n            , 
		bool           init_x       , 
		Number*        x            ,
		bool           init_z       , 
		Number*        z_L          , 
		Number*        z_U          ,
		Index          m            ,
		bool           init_lambda  ,
		Number*        lambda
	);

	// return the objective value 
	virtual bool eval_f(
		Index          n           , 
		const Number*  x           , 
		bool           new_x       , 
		Number&        obj_value
	);

	// Method to return the gradient of the objective 
	virtual bool eval_grad_f(
		Index          n           , 
		const Number*  x           , 
		bool           new_x       , 
		Number*        grad_f
  	);

	// return the constraint residuals
	virtual bool eval_g(
		Index          n           , 
		const Number*  x           , 
		bool           new_x       , 
		Index          m           , 
		Number*        g
	);

	// Method to return:
	// 1) The structure of the jacobian (if "values" is NULL)
	// 2) The values of the jacobian (if "values" is not NULL)
	virtual bool eval_jac_g(
		Index          n           , 
		const Number*  x           , 
		bool           new_x       ,
		Index          m           , 
		Index          nele_jac    , 
		Index*         iRow        , 
		Index*         jCol        ,
		Number*        values
	);

	// Method to return:
	//  1) structure of hessian of the lagrangian (if "values" is NULL)
	//  2) values of hessian of the lagrangian (if "values" is not NULL)
	virtual bool eval_h(
		Index          n           , 
		const Number*  x           , 
		bool           new_x       ,
		Number         obj_factor  , 
		Index          m           , 
		const Number*  lambda      ,
		bool           new_lambda  , 
		Index          nele_hess   , 
		Index*         iRow        ,
		Index*         jCol        , 
		Number*        values
	);

	// called when the algorithm is completed so the TNLP can 
	// store/write the solution 
	virtual void finalize_solution(
		Ipopt::SolverReturn       status      ,
		Index                      n          , 
		const Number*              x          , 
		const Number*              z_L        , 
		const Number*              z_U        ,
		Index                      m          , 
		const Number*              g          , 
		const Number*              lambda     ,
		Number                     obj_value  ,
		const Ipopt::IpoptData*           ip_data    ,
		Ipopt::IpoptCalculatedQuantities* ip_cq
	);
private:
	/*
 	Values passed in by user
	*/
	// dimension of the domain space
	const size_t                    n_;
	// number of components in g
	const size_t                    m_;
	// initial x
	const NumberVector              x_i_;
	// limits for x and g
	const NumberVector              x_l_;
	const NumberVector              x_u_;
	const NumberVector              g_l_;
	const NumberVector              g_u_;
	// Users function that evaluates f and g
	ipopt_cppad_fg_info* const      fg_info_;
	// does operation sequence chage with argument value
	const bool                      retape_;
	// object for storing final solution results
	ipopt_cppad_solution* const     solution_;
	/*
 	Computed values
	*/
	// CppAD sparsity patterns
	BoolVector                       pattern_jac_fg_;
	BoolVector                       pattern_h_lag_;
	// Ipopt sparsity structure for Jacobian of g
	size_t                           nnz_jac_g_;
	SizeVector                       iRow_jac_g_;
	SizeVector                       jCol_jac_g_;
	// Ipopt sparsity structure for Hessian of Lagragian
	size_t                           nnz_h_lag_;
	SizeVector                       iRow_h_lag_;
	SizeVector                       jCol_h_lag_;
	// CppAD function object for both f and g as one function
	CppAD::ADFun<Number>             fg_fun_;
	/*
 	Methods
	*/
	// Methods to block default compiler methods.
	ipopt_cppad_nlp(const ipopt_cppad_nlp&);
	ipopt_cppad_nlp& operator=(const ipopt_cppad_nlp&);

	// Methods used by public methods
	static void record_fg_fun(
		size_t                m       ,
		size_t                n       ,
		ADVector&             x_vec   , 
		ipopt_cppad_fg_info  *fg_info , 
		CppAD::ADFun<Number>& fg_fun
	);
	static void compute_pattern_jac_fg(
		size_t                m              ,
		size_t                n              ,
		CppAD::ADFun<Number>& fg_fun         ,
		BoolVector&           pattern_jac_fg 
	);
	static void compute_pattern_h_lag(
		size_t                m              ,
		size_t                n              ,
		CppAD::ADFun<Number>& fg_fun         ,
		BoolVector&           pattern_h_lag 
	);
	static void compute_structure_jac_g(
		size_t                m              ,
		size_t                n              ,
		const BoolVector&     pattern_jac_fg ,
		size_t&               nnz_jac_g      ,
		SizeVector&           iRow_jac_g     ,
		SizeVector&           jCol_jac_g
	);
	static void compute_structure_h_lag(
		size_t                m              ,
		size_t                n              ,
		const BoolVector&     pattern_h_lag  ,
		size_t&               nnz_h_lag      ,
		SizeVector&           iRow_h_lag     ,
		SizeVector&           jCol_h_lag
	);

};



# endif
