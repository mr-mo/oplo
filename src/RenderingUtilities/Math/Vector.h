#ifndef EPORT_VECTOR_H
#define EPORT_VECTOR_H

#include <algorithm>
#include <functional>
#include <sstream>
#include <cmath>

	namespace oplo
	{

#define ACCESS_ME(x)		return static_cast< const Derived* >( this )->at(x)
#define SET_ME(x,y)			static_cast< Derived* >( this )->at(x) = y

	template< typename T, unsigned Sz, typename Derived >
	class Accessors : public Accessors< T, Sz - 1, Derived >
	{
	protected:
		Accessors() { }
		~Accessors() 
		{}
	};

	template< typename T, typename Derived >
	class Accessors< T, 0, Derived >
	{
	protected:
		Accessors() { }
		~Accessors() 
		{}
	};

	template< typename T, typename Derived >
	class Accessors< T, 1, Derived >
	{
	protected:
		Accessors() { }
		~Accessors() 
		{ }
	public:
		T X() const				{ ACCESS_ME(0); }
		void setX( T v )		{ SET_ME(0,v); }
	};

	template< typename T, typename Derived >
	class Accessors< T, 2, Derived > : public Accessors< T, 1, Derived >
	{
	protected:
		Accessors() { }
		~Accessors() 
		{ }
	public:
		T Y() const				{ ACCESS_ME(1); }
		void setY( T v )		{ SET_ME(1,v); }
		void setXy( T x, T y )	{ setX( x ); setY( y ); } 
	};

	template< typename T, typename Derived >
	class Accessors< T, 3, Derived > : public Accessors< T, 2, Derived >
	{
	protected:
		Accessors() { }
		~Accessors()
		{ }
	public:
		T Z() const						{ ACCESS_ME(2); }
		void setZ( T v )				{ SET_ME(2,v); }
		void setXyz( T x, T y, T z )	{ setX( x ); setY( y ); setZ( z ); } 
	};


	template< typename T, typename Derived >
	class Accessors< T, 4, Derived > : public Accessors< T, 3, Derived >
	{
	protected:
		Accessors() { }
		~Accessors()
		{ }
	public:
		T W() const							{ ACCESS_ME(3); }
		void setW( T v )					{ SET_ME(3,v); }
		void setXyzw( T x, T y, T z, T w )	{ setX( x ); setY( y ); setZ( z ); setW( w ); } 
	};

#undef ACCESS_ME
#undef SET_ME

	template< typename T, unsigned Sz >
	class Vector : public Accessors< T, Sz, Vector< T, Sz > >
	{
	public:

		typedef Vector< T, Sz > self_t;
		typedef T				value_type;
		typedef T*				pointer_type;
		typedef T&				reference_type;
		typedef const T*		const_pointer_type;
		typedef const T&		const_reference_type;

		Vector() {}

		//These will generate compile time errors if an inappropriate constructor is used for the vector dimension
		//unfortunately we cannot fully instantiate the class with template class Vector< T, N > anymore
		Vector( T x )					
		{ 
			setX( x ); 
		}
		
		Vector( T x, T y )				
		{ 
			setXy( x, y ); 
		}
		
		Vector( T x, T y, T z )			
		{ 
			setXyz( x, y, z ); 
		}
		
		Vector( T x, T y, T z, T w )	
		{ 
			setXyzw( x, y, z, w ); 
		}

		Vector( const self_t& rhs ) 
		{ 
			std::copy( rhs.begin(), rhs.end(), begin() ); 
		}

		~Vector()
		{ }

		self_t& operator=( const self_t& rhs )
		{
			if( this != &rhs )
			{
				self_t tmp( rhs );
				tmp.swap( *this );
			}

			return *this;
		}

		bool operator!=( const self_t& rhs ) const
		{
			for( unsigned i = 0; i < Sz; ++i )
			{
				if( data_[i] != rhs[i] ) 
					return true;
			}

			return false;
		}

		bool operator==( const self_t& rhs ) const
		{
			return !( *this != rhs );
		}

		reference_type operator[]( unsigned i )				{ return m_data[i]; }
		reference_type at(unsigned i)						{ check_idx(i); return m_data[i]; }

		const_reference_type operator[](unsigned i) const	{ return m_data[i]; }
		const_reference_type at(unsigned i) const			{ check_idx(i); return m_data[i]; }

		pointer_type begin()								{ return m_data; }
		pointer_type end()									{ return m_data + Sz; }
		
		const_pointer_type begin() const					{ return m_data; }
		const_pointer_type end() const						{ return m_data + Sz; }

		void swap( self_t& rhs )	
		{ 
			std::swap_ranges( begin(), end(), rhs.begin() ); 
		}

		unsigned size() const		
		{ 
			return Sz; }


		void setZero()
		{
			memset(begin(), 0, size() * sizeof(T));
		}

		self_t& operator-()
		{  
			for (unsigned i = 0; i < Sz; ++i)
				m_data[i] = -m_data[i];

			return *this; 
		}

		self_t& operator+=(const self_t& rhs)
		{
			for (unsigned i = 0; i < Sz; ++i)
			{
				m_data[i] += rhs[i];
			}

			return *this;
		}

		self_t& operator-=( const self_t& rhs )
		{ 
			for (unsigned i = 0; i < Sz; ++i)
			{
				m_data[i] -= rhs[i];
			}
			
			return *this;
		}

		self_t& operator*=( const self_t& rhs )
		{ 
			for (unsigned i = 0; i < Sz; ++i)
			{
				m_data[i] *= rhs[i];
			}

			return *this;
		}

		self_t& operator/=(const self_t& rhs)
		{
			for (unsigned i = 0; i < Sz; ++i)
			{
				m_data[i] /= rhs[i];
			}

			return *this;
		}

		self_t& operator+=( value_type rhs )
		{ 
			for (unsigned i = 0; i < Sz; ++i)
			{
				m_data[i] += rhs;
			}

			return *this;
		}

		self_t& operator-=( value_type rhs )
		{ 
			for (unsigned i = 0; i < Sz; ++i)
			{
				m_data[i] -= rhs;
			}

			return *this;
		}

		self_t& operator/=( value_type rhs )
		{ 
			for (unsigned i = 0; i < Sz; ++i)
			{
				m_data[i] /= rhs;
			}

			return *this;
		}

		self_t& operator*=( value_type rhs )
		{ 
			for (unsigned i = 0; i < Sz; ++i)
			{
				m_data[i] *= rhs;
			}

			return *this;
		}

		value_type dot( const self_t& rhs )  const
		{
			value_type r = (value_type)0;

			for (unsigned i = 0; i < Sz; ++i)
			{
				r += m_data[i] * rhs[i];
			}

			return r;
		}

		value_type innerProduct( const self_t& rhs ) const
		{
			return dot( rhs ); 
		}

		value_type lengthSqrd()  const
		{
			return dot(*this);
		}

		value_type length()  const
		{
			return std::sqrt( lengthSqrd() );
		}

		void normalize()
		{
			*this *= 1 / length();
		}

		template<typename NT>
		Vector<NT, Sz> cast() const
		{
			Vector<NT, Sz> v;
			v[0] = static_cast<NT>(m_data[0]);
			v[1] = static_cast<NT>(m_data[1]);
			v[2] = static_cast<NT>(m_data[2]);
			return v;
		}

	protected:
		void check_idx( unsigned idx ) const
		{
			if( idx >= Sz )
			{
				throw std::out_of_range( "Out of range exception from Vector<>" );
			}
		}

		T m_data[ Sz ];
	};


	template< typename T, unsigned Sz >
	Vector< T, Sz > operator+( const Vector< T, Sz >& lhs, const Vector< T, Sz >& rhs )
	{  
		Vector< T, Sz > tmp( lhs );
		tmp += rhs;
		return tmp;
	}

	template< typename T, unsigned Sz >
	Vector< T, Sz > operator-( const Vector< T, Sz >& lhs, const Vector< T, Sz >& rhs )
	{  
		Vector< T, Sz > tmp( lhs );
		tmp -= rhs;
		return tmp;
	}

	template< typename T, unsigned Sz >
	Vector< T, Sz > operator*( const Vector< T, Sz >& lhs, const Vector< T, Sz >& rhs )
	{  
		Vector< T, Sz > tmp( lhs );
		tmp *= rhs;
		return tmp;
	}


	template< typename T, unsigned Sz >
	Vector< T, Sz > operator/( const Vector< T, Sz >& lhs, const Vector< T, Sz >& rhs )
	{  
		Vector< T, Sz > tmp( lhs );
		tmp /= rhs;
		return tmp;
	}

	template< typename T, unsigned Sz >
	Vector< T, Sz > operator+( const Vector< T, Sz >& lhs, T rhs )
	{  
		Vector< T, Sz > tmp( lhs );
		tmp += rhs;
		return tmp;
	}

	template< typename T, unsigned Sz >
	Vector< T, Sz > operator-( const Vector< T, Sz >& lhs, T rhs )
	{  
		Vector< T, Sz > tmp( lhs );
		tmp -= rhs;
		return tmp;
	}

	template< typename T, unsigned Sz >
	Vector< T, Sz > operator*( const Vector< T, Sz >& lhs, T rhs )
	{  
		Vector< T, Sz > tmp( lhs );
		tmp *= rhs;
		return tmp;
	}


	template< typename T, unsigned Sz >
	Vector< T, Sz > operator/( const Vector< T, Sz >& lhs, T rhs )
	{  
		Vector< T, Sz > tmp( lhs );
		tmp /= rhs;
		return tmp;
	}

	template< typename LhsT, typename RhsT, unsigned Sz >
	void Promote( Vector< LhsT, Sz > const& lhs, Vector< RhsT, Sz >& rhs )
	{
		for( unsigned i = 0; i < Sz; ++i )
		{
			rhs[i] = (RhsT)lhs[i];
		}
	}

	template< typename T >
	Vector< T, 3 > Cross( Vector< T, 3 > const& lhs, Vector< T, 3 > const& rhs )
	{
		return Vector< T, 3 >( ( lhs.Y() * rhs.Z() ) - ( lhs.Z() * rhs.Y() ),
								( lhs.Z() * rhs.X() ) - ( lhs.X() * rhs.Z() ),
								( lhs.X() * rhs.Y() ) - ( lhs.Y() * rhs.X() ) );
	}

	typedef Vector< unsigned int, 3 >	Vec3UI;
	typedef Vector< unsigned int, 2 >	Vec2UI;
	typedef Vector< double, 3 >			Vec3D;
	typedef Vector< double, 2 >			Vec2D;
	typedef Vector< float, 4 >			Vec4F;
	typedef Vector< float, 3 >			Vec3F;
	typedef Vector< float, 2 >			Vec2F;

	}

#endif
