#ifndef OPLO_ANGLE_H
#define OPLO_ANGLE_H

#include "Constants.h"

namespace oplo
{

template< typename T >
class Angle
{
public:

	typedef T value_type;

	static Angle fromDeg(value_type v)
	{
		return Angle<value_type>(Constants<value_type>::degToRad(v));
	}

	Angle() : 
		m_data(0)
	{ }

	Angle( value_type v ) : 
		m_data( v )
	{ }

	Angle( Angle< value_type > const& rhs )
	{
		m_data = rhs.getRadians();
	}

	Angle& operator=( Angle< value_type > const& rhs )
	{
		m_data = rhs.getRadians();

		return *this;
	}

	value_type getRadians() const
	{
		return m_data;
	}

	value_type getDegrees() const
	{
		return Constants<value_type>::radToDeg(m_data);
	}

	void setRadians( value_type v )
	{
		m_data = v;
	}

	void setDegrees( value_type v )
	{
		m_data = Constants<value_type>::degToRad(v);
	}

	void normalize()
	{
		m_data = Constants<value_type>::normalizeRadians(m_data);
	}

private:

	value_type m_data;
};

typedef Angle<float> Anglef;
typedef Angle<double > Angled;


}

#endif
