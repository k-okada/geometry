/*
Copyright (c) 2003-2006 Gino van den Bergen / Erwin Coumans  http://continuousphysics.com/Bullet/

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/



#ifndef TF_VECTOR3_H
#define TF_VECTOR3_H


#include "Scalar.h"
#include "MinMax.h"

namespace tf{

#define Vector3Data Vector3DoubleData
#define Vector3DataName "Vector3DoubleData"




/**@brief Vector3 can be used to represent 3D points and vectors.
 * It has an un-used w component to suit 16-byte alignment when Vector3 is stored in containers. This extra component can be used by derived classes (Quaternion?) or by user
 * Ideally, this class should be replaced by a platform optimized SIMD version that keeps the data in registers
 */
ATTRIBUTE_ALIGNED16(class) Vector3
{
public:

#if defined (__SPU__) && defined (__CELLOS_LV2__)
		btScalar	m_floats[4];
public:
	SIMD_FORCE_INLINE const vec_float4&	get128() const
	{
		return *((const vec_float4*)&m_floats[0]);
	}
public:
#else //__CELLOS_LV2__ __SPU__
#ifdef BT_USE_SSE // _WIN32
	union {
		__m128 mVec128;
		btScalar	m_floats[4];
	};
	SIMD_FORCE_INLINE	__m128	get128() const
	{
		return mVec128;
	}
	SIMD_FORCE_INLINE	void	set128(__m128 v128)
	{
		mVec128 = v128;
	}
#else
	btScalar	m_floats[4];
#endif
#endif //__CELLOS_LV2__ __SPU__

	public:

  /**@brief No initialization constructor */
	SIMD_FORCE_INLINE Vector3() {}

 
	
  /**@brief Constructor from scalars 
   * @param x X value
   * @param y Y value 
   * @param z Z value 
   */
	SIMD_FORCE_INLINE Vector3(const btScalar& x, const btScalar& y, const btScalar& z)
	{
		m_floats[0] = x;
		m_floats[1] = y;
		m_floats[2] = z;
		m_floats[3] = btScalar(0.);
	}

	
/**@brief Add a vector to this one 
 * @param The vector to add to this one */
	SIMD_FORCE_INLINE Vector3& operator+=(const Vector3& v)
	{

		m_floats[0] += v.m_floats[0]; m_floats[1] += v.m_floats[1];m_floats[2] += v.m_floats[2];
		return *this;
	}


  /**@brief Subtract a vector from this one
   * @param The vector to subtract */
	SIMD_FORCE_INLINE Vector3& operator-=(const Vector3& v) 
	{
		m_floats[0] -= v.m_floats[0]; m_floats[1] -= v.m_floats[1];m_floats[2] -= v.m_floats[2];
		return *this;
	}
  /**@brief Scale the vector
   * @param s Scale factor */
	SIMD_FORCE_INLINE Vector3& operator*=(const btScalar& s)
	{
		m_floats[0] *= s; m_floats[1] *= s;m_floats[2] *= s;
		return *this;
	}

  /**@brief Inversely scale the vector 
   * @param s Scale factor to divide by */
	SIMD_FORCE_INLINE Vector3& operator/=(const btScalar& s) 
	{
		btFullAssert(s != btScalar(0.0));
		return *this *= btScalar(1.0) / s;
	}

  /**@brief Return the dot product
   * @param v The other vector in the dot product */
	SIMD_FORCE_INLINE btScalar dot(const Vector3& v) const
	{
		return m_floats[0] * v.m_floats[0] + m_floats[1] * v.m_floats[1] +m_floats[2] * v.m_floats[2];
	}

  /**@brief Return the length of the vector squared */
	SIMD_FORCE_INLINE btScalar length2() const
	{
		return dot(*this);
	}

  /**@brief Return the length of the vector */
	SIMD_FORCE_INLINE btScalar length() const
	{
		return btSqrt(length2());
	}

  /**@brief Return the distance squared between the ends of this and another vector
   * This is symantically treating the vector like a point */
	SIMD_FORCE_INLINE btScalar distance2(const Vector3& v) const;

  /**@brief Return the distance between the ends of this and another vector
   * This is symantically treating the vector like a point */
	SIMD_FORCE_INLINE btScalar distance(const Vector3& v) const;

  /**@brief Normalize this vector 
   * x^2 + y^2 + z^2 = 1 */
	SIMD_FORCE_INLINE Vector3& normalize() 
	{
		return *this /= length();
	}

  /**@brief Return a normalized version of this vector */
	SIMD_FORCE_INLINE Vector3 normalized() const;

  /**@brief Rotate this vector 
   * @param wAxis The axis to rotate about 
   * @param angle The angle to rotate by */
	SIMD_FORCE_INLINE Vector3 rotate( const Vector3& wAxis, const btScalar angle );

  /**@brief Return the angle between this and another vector
   * @param v The other vector */
	SIMD_FORCE_INLINE btScalar angle(const Vector3& v) const 
	{
		btScalar s = btSqrt(length2() * v.length2());
		btFullAssert(s != btScalar(0.0));
		return btAcos(dot(v) / s);
	}
  /**@brief Return a vector will the absolute values of each element */
	SIMD_FORCE_INLINE Vector3 absolute() const 
	{
		return Vector3(
			btFabs(m_floats[0]), 
			btFabs(m_floats[1]), 
			btFabs(m_floats[2]));
	}
  /**@brief Return the cross product between this and another vector 
   * @param v The other vector */
	SIMD_FORCE_INLINE Vector3 cross(const Vector3& v) const
	{
		return Vector3(
			m_floats[1] * v.m_floats[2] -m_floats[2] * v.m_floats[1],
			m_floats[2] * v.m_floats[0] - m_floats[0] * v.m_floats[2],
			m_floats[0] * v.m_floats[1] - m_floats[1] * v.m_floats[0]);
	}

	SIMD_FORCE_INLINE btScalar triple(const Vector3& v1, const Vector3& v2) const
	{
		return m_floats[0] * (v1.m_floats[1] * v2.m_floats[2] - v1.m_floats[2] * v2.m_floats[1]) + 
			m_floats[1] * (v1.m_floats[2] * v2.m_floats[0] - v1.m_floats[0] * v2.m_floats[2]) + 
			m_floats[2] * (v1.m_floats[0] * v2.m_floats[1] - v1.m_floats[1] * v2.m_floats[0]);
	}

  /**@brief Return the axis with the smallest value 
   * Note return values are 0,1,2 for x, y, or z */
	SIMD_FORCE_INLINE int minAxis() const
	{
		return m_floats[0] < m_floats[1] ? (m_floats[0] <m_floats[2] ? 0 : 2) : (m_floats[1] <m_floats[2] ? 1 : 2);
	}

  /**@brief Return the axis with the largest value 
   * Note return values are 0,1,2 for x, y, or z */
	SIMD_FORCE_INLINE int maxAxis() const 
	{
		return m_floats[0] < m_floats[1] ? (m_floats[1] <m_floats[2] ? 2 : 1) : (m_floats[0] <m_floats[2] ? 2 : 0);
	}

	SIMD_FORCE_INLINE int furthestAxis() const
	{
		return absolute().minAxis();
	}

	SIMD_FORCE_INLINE int closestAxis() const 
	{
		return absolute().maxAxis();
	}

	SIMD_FORCE_INLINE void setInterpolate3(const Vector3& v0, const Vector3& v1, btScalar rt)
	{
		btScalar s = btScalar(1.0) - rt;
		m_floats[0] = s * v0.m_floats[0] + rt * v1.m_floats[0];
		m_floats[1] = s * v0.m_floats[1] + rt * v1.m_floats[1];
		m_floats[2] = s * v0.m_floats[2] + rt * v1.m_floats[2];
		//don't do the unused w component
		//		m_co[3] = s * v0[3] + rt * v1[3];
	}

  /**@brief Return the linear interpolation between this and another vector 
   * @param v The other vector 
   * @param t The ration of this to v (t = 0 => return this, t=1 => return other) */
	SIMD_FORCE_INLINE Vector3 lerp(const Vector3& v, const btScalar& t) const 
	{
		return Vector3(m_floats[0] + (v.m_floats[0] - m_floats[0]) * t,
			m_floats[1] + (v.m_floats[1] - m_floats[1]) * t,
			m_floats[2] + (v.m_floats[2] -m_floats[2]) * t);
	}

  /**@brief Elementwise multiply this vector by the other 
   * @param v The other vector */
	SIMD_FORCE_INLINE Vector3& operator*=(const Vector3& v)
	{
		m_floats[0] *= v.m_floats[0]; m_floats[1] *= v.m_floats[1];m_floats[2] *= v.m_floats[2];
		return *this;
	}

	 /**@brief Return the x value */
		SIMD_FORCE_INLINE const btScalar& getX() const { return m_floats[0]; }
  /**@brief Return the y value */
		SIMD_FORCE_INLINE const btScalar& getY() const { return m_floats[1]; }
  /**@brief Return the z value */
		SIMD_FORCE_INLINE const btScalar& getZ() const { return m_floats[2]; }
  /**@brief Set the x value */
		SIMD_FORCE_INLINE void	setX(btScalar x) { m_floats[0] = x;};
  /**@brief Set the y value */
		SIMD_FORCE_INLINE void	setY(btScalar y) { m_floats[1] = y;};
  /**@brief Set the z value */
		SIMD_FORCE_INLINE void	setZ(btScalar z) {m_floats[2] = z;};
  /**@brief Set the w value */
		SIMD_FORCE_INLINE void	setW(btScalar w) { m_floats[3] = w;};
  /**@brief Return the x value */
		SIMD_FORCE_INLINE const btScalar& x() const { return m_floats[0]; }
  /**@brief Return the y value */
		SIMD_FORCE_INLINE const btScalar& y() const { return m_floats[1]; }
  /**@brief Return the z value */
		SIMD_FORCE_INLINE const btScalar& z() const { return m_floats[2]; }
  /**@brief Return the w value */
		SIMD_FORCE_INLINE const btScalar& w() const { return m_floats[3]; }

	//SIMD_FORCE_INLINE btScalar&       operator[](int i)       { return (&m_floats[0])[i];	}      
	//SIMD_FORCE_INLINE const btScalar& operator[](int i) const { return (&m_floats[0])[i]; }
	///operator btScalar*() replaces operator[], using implicit conversion. We added operator != and operator == to avoid pointer comparisons.
	SIMD_FORCE_INLINE	operator       btScalar *()       { return &m_floats[0]; }
	SIMD_FORCE_INLINE	operator const btScalar *() const { return &m_floats[0]; }

	SIMD_FORCE_INLINE	bool	operator==(const Vector3& other) const
	{
		return ((m_floats[3]==other.m_floats[3]) && (m_floats[2]==other.m_floats[2]) && (m_floats[1]==other.m_floats[1]) && (m_floats[0]==other.m_floats[0]));
	}

	SIMD_FORCE_INLINE	bool	operator!=(const Vector3& other) const
	{
		return !(*this == other);
	}

	 /**@brief Set each element to the max of the current values and the values of another Vector3
   * @param other The other Vector3 to compare with 
   */
		SIMD_FORCE_INLINE void	setMax(const Vector3& other)
		{
			tfSetMax(m_floats[0], other.m_floats[0]);
			tfSetMax(m_floats[1], other.m_floats[1]);
			tfSetMax(m_floats[2], other.m_floats[2]);
			tfSetMax(m_floats[3], other.w());
		}
  /**@brief Set each element to the min of the current values and the values of another Vector3
   * @param other The other Vector3 to compare with 
   */
		SIMD_FORCE_INLINE void	setMin(const Vector3& other)
		{
			tfSetMin(m_floats[0], other.m_floats[0]);
			tfSetMin(m_floats[1], other.m_floats[1]);
			tfSetMin(m_floats[2], other.m_floats[2]);
			tfSetMin(m_floats[3], other.w());
		}

		SIMD_FORCE_INLINE void 	setValue(const btScalar& x, const btScalar& y, const btScalar& z)
		{
			m_floats[0]=x;
			m_floats[1]=y;
			m_floats[2]=z;
			m_floats[3] = btScalar(0.);
		}

		void	getSkewSymmetricMatrix(Vector3* v0,Vector3* v1,Vector3* v2) const
		{
			v0->setValue(0.		,-z()		,y());
			v1->setValue(z()	,0.			,-x());
			v2->setValue(-y()	,x()	,0.);
		}

		void	setZero()
		{
			setValue(btScalar(0.),btScalar(0.),btScalar(0.));
		}

		SIMD_FORCE_INLINE bool isZero() const 
		{
			return m_floats[0] == btScalar(0) && m_floats[1] == btScalar(0) && m_floats[2] == btScalar(0);
		}

		SIMD_FORCE_INLINE bool fuzzyZero() const 
		{
			return length2() < SIMD_EPSILON;
		}

		SIMD_FORCE_INLINE	void	serialize(struct	Vector3Data& dataOut) const;

		SIMD_FORCE_INLINE	void	deSerialize(const struct	Vector3Data& dataIn);

		SIMD_FORCE_INLINE	void	serializeFloat(struct	Vector3FloatData& dataOut) const;

		SIMD_FORCE_INLINE	void	deSerializeFloat(const struct	Vector3FloatData& dataIn);

		SIMD_FORCE_INLINE	void	serializeDouble(struct	Vector3DoubleData& dataOut) const;

		SIMD_FORCE_INLINE	void	deSerializeDouble(const struct	Vector3DoubleData& dataIn);

};

/**@brief Return the sum of two vectors (Point symantics)*/
SIMD_FORCE_INLINE Vector3 
operator+(const Vector3& v1, const Vector3& v2) 
{
	return Vector3(v1.m_floats[0] + v2.m_floats[0], v1.m_floats[1] + v2.m_floats[1], v1.m_floats[2] + v2.m_floats[2]);
}

/**@brief Return the elementwise product of two vectors */
SIMD_FORCE_INLINE Vector3 
operator*(const Vector3& v1, const Vector3& v2) 
{
	return Vector3(v1.m_floats[0] * v2.m_floats[0], v1.m_floats[1] * v2.m_floats[1], v1.m_floats[2] * v2.m_floats[2]);
}

/**@brief Return the difference between two vectors */
SIMD_FORCE_INLINE Vector3 
operator-(const Vector3& v1, const Vector3& v2)
{
	return Vector3(v1.m_floats[0] - v2.m_floats[0], v1.m_floats[1] - v2.m_floats[1], v1.m_floats[2] - v2.m_floats[2]);
}
/**@brief Return the negative of the vector */
SIMD_FORCE_INLINE Vector3 
operator-(const Vector3& v)
{
	return Vector3(-v.m_floats[0], -v.m_floats[1], -v.m_floats[2]);
}

/**@brief Return the vector scaled by s */
SIMD_FORCE_INLINE Vector3 
operator*(const Vector3& v, const btScalar& s)
{
	return Vector3(v.m_floats[0] * s, v.m_floats[1] * s, v.m_floats[2] * s);
}

/**@brief Return the vector scaled by s */
SIMD_FORCE_INLINE Vector3 
operator*(const btScalar& s, const Vector3& v)
{ 
	return v * s; 
}

/**@brief Return the vector inversely scaled by s */
SIMD_FORCE_INLINE Vector3
operator/(const Vector3& v, const btScalar& s)
{
	btFullAssert(s != btScalar(0.0));
	return v * (btScalar(1.0) / s);
}

/**@brief Return the vector inversely scaled by s */
SIMD_FORCE_INLINE Vector3
operator/(const Vector3& v1, const Vector3& v2)
{
	return Vector3(v1.m_floats[0] / v2.m_floats[0],v1.m_floats[1] / v2.m_floats[1],v1.m_floats[2] / v2.m_floats[2]);
}

/**@brief Return the dot product between two vectors */
SIMD_FORCE_INLINE btScalar 
btDot(const Vector3& v1, const Vector3& v2) 
{ 
	return v1.dot(v2); 
}


/**@brief Return the distance squared between two vectors */
SIMD_FORCE_INLINE btScalar
btDistance2(const Vector3& v1, const Vector3& v2) 
{ 
	return v1.distance2(v2); 
}


/**@brief Return the distance between two vectors */
SIMD_FORCE_INLINE btScalar
btDistance(const Vector3& v1, const Vector3& v2) 
{ 
	return v1.distance(v2); 
}

/**@brief Return the angle between two vectors */
SIMD_FORCE_INLINE btScalar
btAngle(const Vector3& v1, const Vector3& v2) 
{ 
	return v1.angle(v2); 
}

/**@brief Return the cross product of two vectors */
SIMD_FORCE_INLINE Vector3 
btCross(const Vector3& v1, const Vector3& v2) 
{ 
	return v1.cross(v2); 
}

SIMD_FORCE_INLINE btScalar
btTriple(const Vector3& v1, const Vector3& v2, const Vector3& v3)
{
	return v1.triple(v2, v3);
}

/**@brief Return the linear interpolation between two vectors
 * @param v1 One vector 
 * @param v2 The other vector 
 * @param t The ration of this to v (t = 0 => return v1, t=1 => return v2) */
SIMD_FORCE_INLINE Vector3 
lerp(const Vector3& v1, const Vector3& v2, const btScalar& t)
{
	return v1.lerp(v2, t);
}



SIMD_FORCE_INLINE btScalar Vector3::distance2(const Vector3& v) const
{
	return (v - *this).length2();
}

SIMD_FORCE_INLINE btScalar Vector3::distance(const Vector3& v) const
{
	return (v - *this).length();
}

SIMD_FORCE_INLINE Vector3 Vector3::normalized() const
{
	return *this / length();
} 

SIMD_FORCE_INLINE Vector3 Vector3::rotate( const Vector3& wAxis, const btScalar angle )
{
	// wAxis must be a unit lenght vector

	Vector3 o = wAxis * wAxis.dot( *this );
	Vector3 x = *this - o;
	Vector3 y;

	y = wAxis.cross( *this );

	return ( o + x * btCos( angle ) + y * btSin( angle ) );
}

class btVector4 : public Vector3
{
public:

	SIMD_FORCE_INLINE btVector4() {}


	SIMD_FORCE_INLINE btVector4(const btScalar& x, const btScalar& y, const btScalar& z,const btScalar& w) 
		: Vector3(x,y,z)
	{
		m_floats[3] = w;
	}


	SIMD_FORCE_INLINE btVector4 absolute4() const 
	{
		return btVector4(
			btFabs(m_floats[0]), 
			btFabs(m_floats[1]), 
			btFabs(m_floats[2]),
			btFabs(m_floats[3]));
	}



	btScalar	getW() const { return m_floats[3];}


		SIMD_FORCE_INLINE int maxAxis4() const
	{
		int maxIndex = -1;
		btScalar maxVal = btScalar(-BT_LARGE_FLOAT);
		if (m_floats[0] > maxVal)
		{
			maxIndex = 0;
			maxVal = m_floats[0];
		}
		if (m_floats[1] > maxVal)
		{
			maxIndex = 1;
			maxVal = m_floats[1];
		}
		if (m_floats[2] > maxVal)
		{
			maxIndex = 2;
			maxVal =m_floats[2];
		}
		if (m_floats[3] > maxVal)
		{
			maxIndex = 3;
			maxVal = m_floats[3];
		}
		
		
		

		return maxIndex;

	}


	SIMD_FORCE_INLINE int minAxis4() const
	{
		int minIndex = -1;
		btScalar minVal = btScalar(BT_LARGE_FLOAT);
		if (m_floats[0] < minVal)
		{
			minIndex = 0;
			minVal = m_floats[0];
		}
		if (m_floats[1] < minVal)
		{
			minIndex = 1;
			minVal = m_floats[1];
		}
		if (m_floats[2] < minVal)
		{
			minIndex = 2;
			minVal =m_floats[2];
		}
		if (m_floats[3] < minVal)
		{
			minIndex = 3;
			minVal = m_floats[3];
		}
		
		return minIndex;

	}


	SIMD_FORCE_INLINE int closestAxis4() const 
	{
		return absolute4().maxAxis4();
	}

	
 

  /**@brief Set x,y,z and zero w 
   * @param x Value of x
   * @param y Value of y
   * @param z Value of z
   */
		

/*		void getValue(btScalar *m) const 
		{
			m[0] = m_floats[0];
			m[1] = m_floats[1];
			m[2] =m_floats[2];
		}
*/
/**@brief Set the values 
   * @param x Value of x
   * @param y Value of y
   * @param z Value of z
   * @param w Value of w
   */
		SIMD_FORCE_INLINE void	setValue(const btScalar& x, const btScalar& y, const btScalar& z,const btScalar& w)
		{
			m_floats[0]=x;
			m_floats[1]=y;
			m_floats[2]=z;
			m_floats[3]=w;
		}


};


///btSwapVector3Endian swaps vector endianness, useful for network and cross-platform serialization
SIMD_FORCE_INLINE void	btSwapScalarEndian(const btScalar& sourceVal, btScalar& destVal)
{
	unsigned char* dest = (unsigned char*) &destVal;
	unsigned char* src  = (unsigned char*) &sourceVal;
	dest[0] = src[7];
    dest[1] = src[6];
    dest[2] = src[5];
    dest[3] = src[4];
    dest[4] = src[3];
    dest[5] = src[2];
    dest[6] = src[1];
    dest[7] = src[0];
}
///btSwapVector3Endian swaps vector endianness, useful for network and cross-platform serialization
SIMD_FORCE_INLINE void	btSwapVector3Endian(const Vector3& sourceVec, Vector3& destVec)
{
	for (int i=0;i<4;i++)
	{
		btSwapScalarEndian(sourceVec[i],destVec[i]);
	}

}

///btUnSwapVector3Endian swaps vector endianness, useful for network and cross-platform serialization
SIMD_FORCE_INLINE void	btUnSwapVector3Endian(Vector3& vector)
{

	Vector3	swappedVec;
	for (int i=0;i<4;i++)
	{
		btSwapScalarEndian(vector[i],swappedVec[i]);
	}
	vector = swappedVec;
}

SIMD_FORCE_INLINE void btPlaneSpace1 (const Vector3& n, Vector3& p, Vector3& q)
{
  if (btFabs(n.z()) > SIMDSQRT12) {
    // choose p in y-z plane
    btScalar a = n[1]*n[1] + n[2]*n[2];
    btScalar k = btRecipSqrt (a);
    p.setValue(0,-n[2]*k,n[1]*k);
    // set q = n x p
    q.setValue(a*k,-n[0]*p[2],n[0]*p[1]);
  }
  else {
    // choose p in x-y plane
    btScalar a = n.x()*n.x() + n.y()*n.y();
    btScalar k = btRecipSqrt (a);
    p.setValue(-n.y()*k,n.x()*k,0);
    // set q = n x p
    q.setValue(-n.z()*p.y(),n.z()*p.x(),a*k);
  }
}


struct	Vector3FloatData
{
	float	m_floats[4];
};

struct	Vector3DoubleData
{
	double	m_floats[4];

};

SIMD_FORCE_INLINE	void	Vector3::serializeFloat(struct	Vector3FloatData& dataOut) const
{
	///could also do a memcpy, check if it is worth it
	for (int i=0;i<4;i++)
		dataOut.m_floats[i] = float(m_floats[i]);
}

SIMD_FORCE_INLINE void	Vector3::deSerializeFloat(const struct	Vector3FloatData& dataIn)
{
	for (int i=0;i<4;i++)
		m_floats[i] = btScalar(dataIn.m_floats[i]);
}


SIMD_FORCE_INLINE	void	Vector3::serializeDouble(struct	Vector3DoubleData& dataOut) const
{
	///could also do a memcpy, check if it is worth it
	for (int i=0;i<4;i++)
		dataOut.m_floats[i] = double(m_floats[i]);
}

SIMD_FORCE_INLINE void	Vector3::deSerializeDouble(const struct	Vector3DoubleData& dataIn)
{
	for (int i=0;i<4;i++)
		m_floats[i] = btScalar(dataIn.m_floats[i]);
}


SIMD_FORCE_INLINE	void	Vector3::serialize(struct	Vector3Data& dataOut) const
{
	///could also do a memcpy, check if it is worth it
	for (int i=0;i<4;i++)
		dataOut.m_floats[i] = m_floats[i];
}

SIMD_FORCE_INLINE void	Vector3::deSerialize(const struct	Vector3Data& dataIn)
{
	for (int i=0;i<4;i++)
		m_floats[i] = dataIn.m_floats[i];
}

}

#endif //SIMD__VECTOR3_H
