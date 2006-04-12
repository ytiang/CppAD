# ifndef CppADJacobianIncluded
# define CppADJacobianIncluded

/* -----------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-06 Bradley M. Bell

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
------------------------------------------------------------------------ */

/*
$begin Jacobian$$
$spell
	dy
	typename
	Taylor
	Jacobian
	DetLu
	const
$$

$index Jacobian, driver$$
$index first, derivative$$
$index driver, Jacobian$$

$section Jacobian: Driver Routine$$

$table
$bold Syntax$$
$syntax%%dy% = %f%.Jacobian(%x%)%$$
$tend

$fend 20$$

$head Purpose$$
We use $latex F : B^n \rightarrow B^m$$ to denote the
$xref/glossary/AD Function/AD function/$$ corresponding to $italic f$$.
The syntax above sets $italic dy$$ to the
Jacobian of $italic F$$ evaluated at $italic x$$; i.e.,
$latex \[
	dy = F^{(1)} (x)
\] $$

$head f$$
The object $italic f$$ has prototype
$syntax%
	ADFun<%Base%> %f%
%$$
Note that the $xref/ADFun/$$ object $italic f$$ is not $code const$$
(see $xref/Jacobian/Forward/Forward/$$ below).

$head x$$
The argument $italic x$$ has prototype
$syntax%
	const %Vector% &%x%
%$$
(see $xref/Jacobian/Vector/Vector/$$ below)
and its size 
must be equal to $italic n$$, the dimension of the
$xref/FunConstruct/x/Domain Space/domain space/1/$$ for $italic f$$.
It specifies
that point at which to evaluate the Jacobian.

$head dy$$
The result $italic dy$$ has prototype
$syntax%
	const %Vector% &%dy%
%$$
(see $xref/Jacobian/Vector/Vector/$$ below)
and its size is $latex m * n$$; i.e., the product of the
$xref/FunConstruct/x/Domain Space/domain/1/$$
and
$xref/FunConstruct/y/Range Space/range/1/$$
dimensions for $italic f$$.
For $latex i = 0 , \ldots , m - 1 $$ 
and $latex j = 0 , \ldots , n - 1$$
$latex \[.
	dy[ i * n + j ] = \D{ F_i }{ x_j } ( x )
\] $$


$head Vector$$
The type $italic Vector$$ must be a $xref/SimpleVector/$$ class with
$xref/SimpleVector/Elements of Specified Type/elements of type/$$
$italic Base$$.
The routine $xref/CheckSimpleVector/$$ will generate an error message
if this is not the case.

$head Forward$$
After each call to $xref/Forward/$$,
the object $italic f$$ contains the corresponding 
$xref/glossary/Taylor Coefficient/Taylor coefficients/$$.
After $code Jacobian$$,
the previous calls to $xref/Forward/$$ are undefined.

$head Example$$
$children%
	Example/Jacobian.cpp
%$$
The routine 
$xref/Jacobian.cpp//Jacobian/$$ is both an example and test.
It returns $code true$$, if it succeeds and $code false$$ otherwise.

$end
-----------------------------------------------------------------------------
*/

//  BEGIN CppAD namespace
namespace CppAD {

template <typename Base, typename Vector>
void JacobianFor(ADFun<Base> &f, const Vector &x, Vector &dy)
{	size_t i;
	size_t j;

	size_t m = f.Domain();
	size_t n = f.Range();

	// check Vector is Simple Vector class with Base type elements
	CheckSimpleVector<Base, Vector>();

	CppADUnknownError( x.size()  == f.Domain() );
	CppADUnknownError( dy.size() == f.Range() * f.Domain() );

	// argument and result for forward mode calculations
	Vector u(m);
	Vector v(n);

	// initialize all the components
	for(j = 0; j < m; j++)
		u[j] = Base(0);
	
	// loop through the different coordinate directions
	for(j = 0; j < m; j++)
	{	// set u to the j-th coordinate direction
		u[j] = Base(1);

		// compute the partial of f w.r.t. this coordinate direction
		v = f.Forward(1, u);

		// reset u to vector of all zeros
		u[j] = Base(0);

		// return the result
		for(i = 0; i < n; i++)
			dy[ i * m + j ] = v[i];
	}
}
template <typename Base, typename Vector>
void JacobianRev(ADFun<Base> &f, const Vector &x, Vector &dy)
{	size_t i;
	size_t j;

	size_t m = f.Domain();
	size_t n = f.Range();

	CppADUnknownError( x.size()  == f.Domain() );
	CppADUnknownError( dy.size() == f.Range() * f.Domain() );

	// argument and result for reverse mode calculations
	Vector u(m);
	Vector v(n);

	// initialize all the components
	for(i = 0; i < n; i++)
		v[i] = Base(0);
	
	// loop through the different coordinate directions
	for(i = 0; i < n; i++)
	{	if( f.Parameter(i) )
		{	// return zero for this component of f
			for(j = 0; j < m; j++)
				dy[ i * m + j ] = Base(0);
		}
		else
		{ 
			// set v to the i-th coordinate direction
			v[i] = Base(1);

			// compute the derivative of this component of f
			u = f.Reverse(1, v);

			// reset v to vector of all zeros
			v[i] = Base(0);

			// return the result
			for(j = 0; j < m; j++)
				dy[ i * m + j ] = u[j];
		}
	}
}

template <typename Base>
template <typename Vector>
Vector ADFun<Base>::Jacobian(const Vector &x)
{	size_t i;
	size_t m = Domain();
	size_t n = Range();

	CppADUsageError(
		x.size() == m,
		"Jacobian: length of x not equal domain dimension for F"
	); 

	// point at which we are evaluating the Jacobian
	Forward(0, x);

	// work factor for forward mode
	size_t workForward = m; 

	// work factor for reverse mode
	size_t workReverse = 0;
	for(i = 0; i < n; i++)
	{	if( ! Parameter(i) )
			++workReverse;
	}

	// choose the method with the least work
	Vector dy( n * m );
	if( workForward <= workReverse )
		JacobianFor(*this, x, dy);
	else	JacobianRev(*this, x, dy);

	return dy;
}

} // END CppAD namespace

# endif
