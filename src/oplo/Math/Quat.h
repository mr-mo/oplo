#ifndef EPORT_QUAT_H
#define EPORT_QUAT_H

#include "Vector.h"
#include "Angle.h"
#include "Matrix.h"

#include <cmath>

namespace oplo
{

template< typename T >
class Quat
{
public:

	typedef Vector< T, 3 > vector_type;
	typedef T value_type;

	Quat()
	{
		setXyzw( (value_type)0, (value_type)0, (value_type)0, (value_type)1 );
	}

	Quat( value_type x, value_type y, value_type z, value_type w )
	{
		setXyzw( x, y, z, w );
	}

	Quat( const Quat& rhs )
	{
		std::copy( rhs.quat_, rhs.quat_ + 4, quat_ );
	}

	Quat& operator=( const Quat& rhs )
	{
		if( this != &rhs )
		{
			Quat tmp( rhs );
			tmp.swap( *this );
		}

		return *this;
	}

	void swap( Quat& rhs )
	{
		std::swap_ranges( quat_, quat_ + 4, rhs.quat_ );
	}

	value_type W() const { return quat_[3]; }
	value_type X() const { return quat_[0]; }
	value_type Y() const { return quat_[1]; }
	value_type Z() const { return quat_[2]; }

	void setX( value_type x ) { quat_[0] = x; }
	void setY( value_type y ) { quat_[1] = y; }
	void setZ( value_type z ) { quat_[2] = z; }
	void setW( value_type w ) { quat_[3] = w; }

	void setXyzw( value_type x, value_type y, value_type z, value_type w )
	{ 
		setX( x ); 
		setY( y ); 
		setZ( z ); 
		setW( w );
	}

	Quat& operator+=( const Quat&  rhs )
	{ 
		setX( X() + rhs.X() ); 
		setY( Y() + rhs.Y() ); 
		setZ( Z() + rhs.Z() ); 
		setW( W() + rhs.W() ); 
		return *this; 
	}

	Quat& operator-=( const Quat&  rhs )
	{ 
		setX( X() - rhs.X() ); 
		setY( Y() - rhs.Y() ); 
		setZ( Z() - rhs.Z() ); 
		setW( W() - rhs.W() ); 
		return *this; 
	}

	Quat& operator*=( const Quat&  rhs )
	{ 
		value_type  wt = 
				( W() * rhs.W() ) - 
				( X() * rhs.X() ) - 
				( Y() * rhs.Y() ) - 
				( Z() * rhs.Z() );
		value_type  xt = 
				( W() * rhs.X() ) + 
				( X() * rhs.W() ) + 
				( Y() * rhs.Z() ) - 
				( Z() * rhs.Y() );
		value_type  yt = 
				( W() * rhs.Y() ) + 
				( Y() * rhs.W() ) + 
				( Z() * rhs.X() ) - 
				( X() * rhs.Z() );
		value_type  zt = 
				( W() * rhs.Z() ) + 
				( Z() * rhs.W() ) + 
				( X() * rhs.Y() ) - 
				( Y() * rhs.X() );

		setXyzw( xt, yt, zt, wt );
		return *this;
	}

	Quat& operator/=( const Quat& rhs )
	{ 
		setX( X() / rhs.X() ); 
		setY( Y() / rhs.Y() ); 
		setZ( Z() / rhs.Z() ); 
		setW( W() / rhs.W() );  
		return *this; 
	}


	Quat& operator+=( value_type rhs )
	{ 
		setX( X() + rhs ); 
		setY( Y() + rhs ); 
		setZ( Z() + rhs ); 
		setW( W() + rhs ); 
		return *this; 
	}

	Quat& operator-=( value_type rhs )
	{ 
		setX( X() - rhs ); 
		setY( Y() - rhs ); 
		setZ( Z() - rhs ); 
		setW( W() - rhs ); 
		return *this; 
	}

	Quat& operator*=( value_type rhs )
	{ 
		setX( X() * rhs ); 
		setY( Y() * rhs ); 
		setZ( Z() * rhs ); 
		setW( W() * rhs ); 
		return *this; 
	}

	Quat& operator/=( value_type rhs )
	{ 
		setX( X() / rhs ); 
		setY( Y() / rhs ); 
		setZ( Z() / rhs ); 
		setW( W() / rhs ); 
		return *this; 
	}

	value_type lengthSqrd() const
	{ 
		return X() * X() + Y() * Y() + Z() * Z() + W() * W(); 
	}

	value_type length() const
	{ 
		return std::sqrt( lengthSqrd() ); 
	}

	void conjugate()
	{ 
		setX( -X() ); 
		setY( -Y() ); 
		setZ( -Z() ); 
	}

	void invert()
	{ 
		*this /= lengthSqrd(); 
		conjugate(); 
	}

	void normalize()
	{ 
		*this /= length(); 
	}

	void fromAxisAngle(Angle< value_type > const& angle, const vector_type& axis)
	{
		value_type sine = sin( angle.getRadians() / 2 );
        value_type cosine = cos( angle.getRadians() / 2 );

		setXyzw( axis.X() * sine, axis.Y() * sine, axis.Z() * sine, cosine );
		normalize();
	}

	void getAngle( Angle<value_type>& angle )
	{
		angle.SetRadians(2 * acos( W() ) );
	}

	void GetAxis( vector_type& axis )
	{
		value_type scalar = std::sqrt( 1 - W() * W() );

		if( scalar != 0 )
		{
			 axis.SetXyz( 
				X() / scalar,
		 		Y() / scalar,
		 		Z() / scalar );
		}
		else
		{
			 axis.SetXyz( 
				X(),
		 		Y(),
		 		Z() );
		}
	}

	void getMatrix( Matrix4<value_type>& m ) const
	{
		value_type xx = X() * X();
		value_type xy = X() * Y();
		value_type xz = X() * Z();
		value_type xw = X() * W();

		value_type yy = Y() * Y();
		value_type yz = Y() * Z();
		value_type yw = Y() * W();

		value_type zz = Z() * Z();
		value_type zw = Z() * W();
		

		m(0,0) = 1 - 2 * ( yy + zz );
		m(0,1) = 2 * ( xy - zw );
		m(0,2) = 2 * ( xz + yw );
		m(0,3) = 0.f;

		m(1,0) = 2 * ( xy + zw );
		m(1,1) = 1 - 2 * ( xx + zz );
		m(1,2) = 2 * ( yz - xw );
		m(1,3) = 0.f;

		m(2,0) = 2 * ( xz - yw );
		m(2,1) = 2 * ( yz + xw );
		m(2,2) = 1 - 2 * ( xx + yy );
		m(2,3) = 0;

		m(0,3) = 0;
		m(1,3) = 0;
		m(2,3) = 0;
		m(3,3) = 1;
	}

	void setMatrix( const Matrix4<value_type>& m )
	{
		value_type trace = m( 0, 0 ) + m( 1, 1 ) + m( 2, 2 );

		if( trace > 0 )
		{
			value_type s = sqrt( trace + m( 3, 3 ) );
			SetW( s * value_type( 0.5 ) );
			s = value_type( 0.5 ) / s;
			SetX( ( m( 1, 2 ) - m( 2, 1 ) ) * s );
			SetY( ( m( 2, 0 ) - m( 0, 2 ) ) * s );
			SetZ( ( m( 0, 1 ) - m( 1, 0 ) ) * s );
		}
		else
		{
			const unsigned int idx[] = { 1, 2, 0 };
			int i, j, k;
			i = 0;
		
			if( m( 1, 1 ) > m( i, i ) )
				i = 1;

			if( m( 2, 2 ) > m( i, i ) )
				i = 2;

			j = idx[i];
			k = idx[j];

			float s = sqrt( (  m( i, j ) - ( m( j, j ) + m( k, k ) ) ) + 1 );

			quat_[i] = s * value_type( 0.5 );

			if( s != 0 )
			{
				s = value_type( 0.5 ) / s;
			}

			quat_[j] = ( m( i, j ) + m( j, i ) ) * s;
			quat_[k] = ( m( i, k ) + m( k, i ) ) * s;
			quat_[3] = ( m( j, k ) - m( k, j ) ) * s;
		}
	}

protected:

	value_type quat_[4];
};

template< typename T >
Quat< T > operator+( const Quat<T>& lhs, const Quat<T>& rhs )
{
	Quat<T> r = lhs;
	r += rhs;
	return r;
}

template< typename T >
Quat< T > operator-( const Quat<T>& lhs, const Quat<T>& rhs )
{
	Quat<T> r = lhs;
	r -= rhs;
	return r;
}

template< typename T >
Quat< T > operator/( const Quat<T>& lhs, const Quat<T>& rhs )
{
	Quat<T> r = lhs;
	r /= rhs;
	return r;
}

template< typename T >
Quat< T > operator*( const Quat<T>& lhs, const Quat<T>& rhs )
{
	Quat<T> r = lhs;
	r *= rhs;
	return r;
}

template< typename T >
Quat< T > operator+( const Quat<T>& lhs, T rhs )
{
	Quat<T> r = lhs;
	r += rhs;
	return r;
}

template< typename T >
Quat< T > operator-( const Quat<T>& lhs, T rhs )
{
	Quat<T> r = lhs;
	r -= rhs;
	return r;
}

template< typename T >
Quat< T > operator/( const Quat<T>& lhs, T rhs )
{
	Quat<T> r = lhs;
	r /= rhs;
	return r;
}

template< typename T >
Quat< T > operator*( const Quat<T>& lhs, T rhs )
{
	Quat<T> r = lhs;
	r *= rhs;
	return r;
}

template< typename T >
Vector< T, 3 > TransformVector( const Quat< T >& lhs, Vector< T, 3 > const& rhs )
{
	Quat< T > conj = lhs; 

	conj.invert();
	Quat< T > vector_quat( rhs.X(), rhs.Y(), rhs.Z(), (T)0 );

	vector_quat = lhs * vector_quat * conj;

	return Vector< T, 3 >( vector_quat.X(), vector_quat.Y(), vector_quat.Z() );
}

template< typename T >
Vector< T, 3 > ViewVector( Quat< T > const& q )
{
	T x2 = q.X() + q.X(), y2 = q.Y() + q.Y(), z2 = q.Z() + q.Z();
	T xx = q.X() * x2, xz = q.X() * z2;
	T yy = q.Y() * y2, yz = q.Y() * z2;
	T wx = q.W() * x2, wy = q.W()* y2;
	return -Vector< T, 3 >(xz+wy, yz-wx, 1-(xx+yy));
}

template< typename T >
Vector< T, 3 > UpVector( Quat< T > const& q )
{
	T x2 = q.X() + q.X(), y2 = q.Y() + q.Y(), z2 = q.Z() + q.Z();
	T xx = q.X() * x2, xy = q.X() * y2;
	T yz = q.Y() * z2, zz = q.Z() * z2;
	T wx = q.W() * x2, wz = q.W()*  z2;

	return Vector< T, 3 >(xy-wz, 1-(xx+zz), yz+wx);
}

template< typename T >
Vector< T, 3 > RightVector( Quat< T > const& q )
{
	T y2 = q.Y() + q.Y(), z2 = q.Z() + q.Z();
	T xy = q.X() * y2, xz = q.X() * z2;
	T yy = q.Y() * y2, zz = q.Z() * z2;
	T wy = q.W() * y2, wz = q.W()*  z2;
	return Vector< T, 3 >( 1 - (yy+zz), xy + wz, xz - wy );
}

}

#endif
