#ifndef EPORT_PLANE_H
#define EPORT_PLANE_H

#include "Vector.h"

namespace oplo
{

template< typename T >
class Plane
{
public:

	typedef Vector< T, 3 > vector_type;
	typedef T value_type;

	Plane() 
	{}

	~Plane() 
	{}

	void set(vector_type const& p0, vector_type const& p1, vector_type const& p2)
	{
		vector_type u = p0 - p1;
		vector_type v = p2 - p1;
		m_n = oplo::Cross(v, u);
		m_n.normalize();
		m_d = -(m_n.dot(p1));
	}

	void set(vector_type const& n, T d)
	{
		m_d = d;
		m_n = n;
	}

	T distance( vector_type const& p ) const
	{
		return m_d + m_n.dot(p);
	}

	const vector_type& getNormal() const
	{
		return m_n;
	}

	T getD() const
	{
		return m_d;
	}

private:
	value_type m_d;
	vector_type m_n;
};

}

#endif
