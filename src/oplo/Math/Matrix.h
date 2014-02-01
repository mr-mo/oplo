#ifndef EPORT_MATRIX_H
#define EPORT_MATRIX_H

#include "Vector.h"

namespace oplo
{

//This class is somewhat lacking, I build on it when I find I actually need some functionality
template< typename Precision >
class Matrix4
{
public:
	typedef Precision			value_type;
	typedef Precision*			pointer_type;
	typedef const Precision*	const_pointer_type;

	typedef Matrix4< Precision > SelfType;
	typedef Vector< Precision, 4 > VectorType4;
	typedef Vector< Precision, 3 > VectorType3;

	Matrix4()
	{}

	Matrix4(value_type ident_value)
	{
		makeIdentity( ident_value );
	}

	Matrix4(const SelfType& rhs)
	{
		memcpy(m_data, rhs.m_data, sizeof(Precision)* 16);
	}

	Matrix4(const value_type* rhs)
	{
		for (int i = 0; i < 16; ++i)
		{
			m_data[0] = rhs[0];
		}
	}

	SelfType& operator=( const SelfType& rhs )
	{
		for (int i = 0; i < 16; ++i)
		{
			m_data[0] = rhs[0];
		}

		return *this;
	}

	value_type& operator[]( unsigned idx )
	{
		return m_data[ idx ];
	}

	value_type operator[]( unsigned idx ) const
	{
		return m_data[idx];
	}

	value_type operator()( unsigned row, unsigned col ) const
	{ 
		return m_colMaj[ col ][ row ];
	}

	value_type& operator()( unsigned row, unsigned col )
	{ 
		return m_colMaj[col][row];
	}

	value_type& at( unsigned row, unsigned col )
	{
		return m_colMaj[col][row];
	}

	value_type at( unsigned row, unsigned col ) const
	{
		return m_colMaj[col][row];
	}

	template< typename T >
	void transform(oplo::Vector< T, 3 >& vVector) const
	{
		T x, y, z;
		x = at(0, 0) * vVector.X() + at(1, 0) * vVector.Y() + at(2, 0) * vVector.Z() + at(3, 0);
		y = at(0, 1) * vVector.X() + at(1, 1) * vVector.Y() + at(2, 1) * vVector.Z() + at(3, 0);
		z = at(0, 2) * vVector.X() + at(1, 2) * vVector.Y() + at(2, 2) * vVector.Z() + at(3, 0);

		vVector.SetXyz(x, y, z);
	}

	template< typename T >
	void transform(oplo::Vector< T, 4 >& vVector) const
	{
		T x, y, z, w;
		x = at( 0, 0 ) * vVector.X() + at( 1, 0 ) * vVector.Y() + at( 2, 0 ) * vVector.Z() + at( 3, 0 ) * vVector.W();
		y = at( 0, 1 ) * vVector.X() + at( 1, 1 ) * vVector.Y() + at( 2, 1 ) * vVector.Z() + at( 3, 1 ) * vVector.W();
		z = at( 0, 2 ) * vVector.X() + at( 1, 2 ) * vVector.Y() + at( 2, 2 ) * vVector.Z() + at( 3, 2 ) * vVector.W();
		w = at( 0, 3 ) * vVector.X() + at( 1, 3 ) * vVector.Y() + at( 2, 3 ) * vVector.Z() + at( 3, 3 ) * vVector.W();

		vVector.SetXyzw( x, y, z, w );
	}

	template< typename T, unsigned Sz >
	void rotate(oplo::Vector< T, Sz >& vVector) const
	{
		T x, y, z;
		x = at(0, 0) * vVector.X() + at(1, 0) * vVector.Y() + at(2, 0) * vVector.Z();
		y = at(0, 1) * vVector.X() + at(1, 1) * vVector.Y() + at(2, 1) * vVector.Z();
		z = at(0, 2) * vVector.X() + at(1, 2) * vVector.Y() + at(2, 2) * vVector.Z();

		vVector.SetXyz(x, y, z);
	}

	SelfType& operator*=( SelfType const& rhs )
	{
		SelfType m( *this );
		makeIdentity( value_type( 0 ) );

		for( unsigned i = 0; i < 4; ++i )
		{
			for( unsigned j = 0; j < 4; ++j )
			{
				for( unsigned k = 0; k < 4; ++k )
				{
					at( i, j ) += m( i, k ) * rhs( k, j );
				}
			}
		}

		return *this;
	}

	pointer_type begin()
	{
		return m_data;
	}

	pointer_type end()
	{
		return m_data + 16;
	}

	const_pointer_type begin() const
	{
		return m_data;
	}

	const_pointer_type end() const
	{
		return m_data + 16;
	}

	void setZero()
	{
		for (int i = 0; i < 16; ++i)
		{
			m_data[i] = 0;
		}
	}

	void makeIdentity( value_type ident = 1 )
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				if (i == j)
					m_colMaj[i][j] = ident;
				else
					m_colMaj[i][j] = 0;
			}
		}
	}

	void setColumn( unsigned col, const VectorType4& v4 )
	{
		m_colMaj[col][0] = v4.X();
		m_colMaj[col][1] = v4.Y();
		m_colMaj[col][2] = v4.Z();
		m_colMaj[col][3] = v4.W();
	}

	void setColumn( unsigned col, const VectorType3& v3 )
	{
		m_colMaj[col][0] = v3.X();
		m_colMaj[col][1] = v3.Y();
		m_colMaj[col][2] = v3.Z();
	}

	void normalizeRotation()
	{
		VectorType3 axis0(m_data[0], m_data[4], m_data[8]);
		VectorType3 axis1(m_data[1], m_data[5], m_data[9]);
		VectorType3 axis2(m_data[2], m_data[6], m_data[10]);

		axis0.normalize();
		axis1.normalize();
		axis2.normalize();

		m_data[0] = axis0[0];
		m_data[1] = axis1[0];
		m_data[2] = axis2[0];

		m_data[4] = axis0[1];
		m_data[5] = axis1[1];
		m_data[6] = axis2[1];

		m_data[8] = axis0[2];
		m_data[9] = axis1[2];
		m_data[10] = axis2[2];
	}

	void transpose()
	{
		std::swap(m_colMaj[0][1], m_colMaj[1][0]);
		std::swap(m_colMaj[0][2], m_colMaj[2][0]);
		std::swap(m_colMaj[0][3], m_colMaj[3][0]);
		std::swap(m_colMaj[1][2], m_colMaj[2][1]);
		std::swap(m_colMaj[1][3], m_colMaj[3][1]);
		std::swap(m_colMaj[2][3], m_colMaj[3][2]);
	}

	void invert()
	{
		Matrix4<value_type> minv;

		value_type r1[8], r2[8], r3[8], r4[8];
		value_type *s[4], *tmprow;

        s[0] = &r1[0];
        s[1] = &r2[0];
        s[2] = &r3[0];
        s[3] = &r4[0];

        int i,j,p,jj;
        for(i=0;i<4;i++) {
            for(j=0;j<4;j++) {
                s[i][j] = at(i,j);
                if(i==j) s[i][j+4] = 1.0;
                else     s[i][j+4] = 0.0;
            }
        }

		value_type scp[4];
        for(i=0;i<4;i++) 
		{
            scp[i] = fabs(s[i][0]);
            for(j=1;j<4;j++)
                if(fabs(s[i][j]) > scp[i]) scp[i] = fabs(s[i][j]);
            if(scp[i] == 0.0) 
			{
				*this = minv;
				return;
			}
        }

        int pivot_to;
		value_type scp_max;
        for(i=0;i<4;i++) {
            // select pivot row
            pivot_to = i;
            scp_max = fabs(s[i][i]/scp[i]);
            // find out which row should be on top
            for(p=i+1;p<4;p++)
                if (fabs(s[p][i]/scp[p]) > scp_max) 
				{
                    scp_max = fabs(s[p][i]/scp[p]);
                    pivot_to = p;
                }

            // Pivot if necessary
            if(pivot_to != i) {
                tmprow = s[i];
                s[i] = s[pivot_to];
                s[pivot_to] = tmprow;
				value_type tmpscp;
                tmpscp = scp[i];
                scp[i] = scp[pivot_to];
                scp[pivot_to] = tmpscp;
            }

			value_type mji;
            // perform gaussian elimination
            for(j=i+1;j<4;j++) {
                mji = s[j][i]/s[i][i];
                s[j][i] = 0.0;
                for(jj=i+1;jj<8;jj++)
                    s[j][jj] -= mji*s[i][jj];
            }
        }

        if(s[3][3] == 0.0) 
		{
			*this = minv;
			return;
		}

        //
        // Now we have an upper triangular matrix.
        //
        //  x x x x | y y y y
        //  0 x x x | y y y y 
        //  0 0 x x | y y y y
        //  0 0 0 x | y y y y
        //
        //  we'll back substitute to get the inverse
        //
        //  1 0 0 0 | z z z z
        //  0 1 0 0 | z z z z
        //  0 0 1 0 | z z z z
        //  0 0 0 1 | z z z z 
        //

		value_type mij;
        for(i=3;i>0;i--) {
            for(j=i-1;j > -1; j--) {
                mij = s[j][i]/s[i][i];
                for(jj=j+1;jj<8;jj++)
                    s[j][jj] -= mij*s[i][jj];
            }
        }

        for(i=0;i<4;i++)
            for(j=0;j<4;j++)
                minv(i,j) = s[i][j+4] / s[i][i];

		*this = minv;
	}

private:

	union
	{
		value_type m_data[ 16 ];
		value_type m_colMaj[ 4 ][ 4 ];
	};
};

template< typename T >
Matrix4< T > operator*(const Matrix4< T >& rhs, const Matrix4< T >& lhs)
{
	Matrix4< T > m(rhs);
	m *= lhs;
	return m;
}

	}

#endif
