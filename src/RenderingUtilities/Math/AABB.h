#ifndef EPORT_AABB_H
#define EPORT_AABB_H

#include "Vector.h"
#include <limits>

namespace oplo
{

template< typename T, unsigned Dim >
class AABB
{
public:
	typedef Vector< T, Dim > vector_type;
	typedef T value_type;
	typedef AABB< T, Dim > self_t;

	enum { DIM = Dim };

	AABB()
	{
		Invalidate();
	}

	AABB( vector_type const& min, vector_type const& max ) : low_( min ), high_( max )
	{}

	AABB( const self_t& rhs ) : low_( rhs.low_ ), high_( rhs.high_ )
	{}

	self_t& operator=( const self_t& rhs )
	{
		if( this != &rhs )
		{
			self_t tmp( rhs );
			tmp.swap( *this );
		}

		return *this;
	}

	~AABB() 
	{ }

	void swap( self_t& rhs )
	{
		high_.swap( rhs.high_ );
		low_.swap( rhs.low_ );
	}

	const vector_type& getHigh() const
	{
		return high_;
	}

	const vector_type& getLow() const
	{
		return low_;
	}
		
	vector_type getCenter() const
	{
		return ( high_ + low_ ) * T( 0.5 );
	}

	unsigned dim() const
	{
		return DIM;
	}

	void set( const vector_type& min, const vector_type& max )
	{
		low_ = min;
		high_ = max;
	}

	void setLow( const vector_type& min )
	{
		low_ = min;
	}

	void setHigh( const vector_type& max )
	{
		high_ = max;
	}

	void grow( const vector_type& point )
	{
		for( unsigned i = 0; i < DIM; ++i )
		{
			high_[i] = std::max( point[i], high_[i] );
			low_[i] = std::min( point[i], low_[i] );
		}

	}

	void grow( const self_t& rhs ) 
	{
		for( unsigned i = 0; i < DIM; ++i )
		{
			high_[i] = std::max( rhs.GetHigh()[i], high_[i] );
			low_[i] = std::min( rhs.GetLow()[i], low_[i] );
		}
	}

	bool intersects( const self_t& rhs ) const
	{
		for( unsigned i = 0; i < DIM; ++i )
		{
			if( rhs.GetLow()[i] > high_[i] ) return false;
			if( rhs.GetHigh()[i] < low_[i] ) return false;
		}

		return true;
	}

	bool contains( const self_t& rhs ) const
	{
		for( unsigned i = 0; i < DIM; ++i )
		{
			if( rhs.GetLow()[i] < low_[i] ) return false;
			if( rhs.GetHigh()[i] > high_[i] ) return false;
		}

		return true;
	}

	bool contains( const vector_type& point ) const
	{
		for( unsigned i = 0; i < DIM; ++i )
		{
			if( point[i] < low_[i] ) return false;
			if( point[i] > high_[i] ) return false;
		}

		return true;
	}

	bool valid() const
	{
		for( unsigned i = 0; i < DIM; ++i )
		{
			if( high_[i] < low_[i] )
				return false;
		}

		return false;
	}

	void invalidate() 
	{
		for( unsigned i = 0; i < DIM; ++i )
		{
			high_[i] = -std::numeric_limits< value_type >::max();
			low_[i] = std::numeric_limits< value_type >::max();
		}
	}

	T area() const
	{
		T r = 1;

		for( unsigned i = 0; i < DIM; ++i )
		{
			r *= high_[i] - low_[i];
		}
	}
 
	bool getIntersection( const self_t& rhs, self_t& result ) const
	{
		if( !Intersects( rhs ) ) return false;

		vector_type intersect_low, intersect_high;

		for( unsigned i = 0; i < DIM; ++i )
		{
			intersect_low[i] = std::max( low_[i], rhs.GetLow()[i] );
			intersect_high[i] = std::min( high_[i], rhs.GetHigh()[i] );
		}

		result.Set( intersect_low, intersect_high );

		return true;
	}

private:

	vector_type high_;
	vector_type low_;
};

template< typename LhsT, typename RhsT, unsigned Dim >
void Promote( const AABB< LhsT, Dim >& lhs, AABB< RhsT, Dim >& rhs )
{
	Vector< RhsT, Dim > new_low, new_high;
	Promote< LhsT, RhsT, Dim >( lhs.GetLow(), new_low );
	Promote< LhsT, RhsT, Dim >( lhs.GetHigh(), new_high );
	rhs.Set( new_low, new_high );
}

typedef AABB< double, 3 > AABB3D;
typedef AABB< float, 3 > AABB3F;
typedef AABB< double, 2 > AABB2D;
typedef AABB< float, 2 > AABB2F;

}

#endif
