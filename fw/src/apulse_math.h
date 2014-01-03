/*!
 (C) Copyright 2013 Ben Nahill
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 @file apulse_math.h
 @brief A small library of numerical tools
 @author Ben Nahill <bnahill@gmail.com>
 */

#ifndef __APULSE_MATH_H_
#define __APULSE_MATH_H_

#include <stdint.h>

/*!
 @brief Return the maximum of two values
 @return The maximum of a and b
 */
template<typename typea, typename typeb>
constexpr typea max(typea const &a, typeb const &b){
	return (a > (typea)b) ? a : (typea)b;
}

/*!
 @brief Return the minimum of two values
 @return The minimum of a and b
 */
template<typename typea, typename typeb>
constexpr typea min(typea const &a, typeb const &b){
	return (a < (typea)b) ? a : (typea)b;
}


/*!
 * @brief A class for accessing usable types from arbitrary bit lengths
 * @tparam n_bits The minimum number of bits required
 */
template <size_t n_bits>
class IntLength {
public:
	typedef typename IntLength<n_bits+1>::unsigned_fast_t unsigned_fast_t;
	typedef typename IntLength<n_bits+1>::unsigned_t unsigned_t;
	typedef typename IntLength<n_bits+1>::signed_fast_t signed_fast_t;
	typedef typename IntLength<n_bits+1>::signed_t signed_t;
};

template<>
class IntLength<8>{
public:
	typedef uint_fast8_t unsigned_fast_t;
	typedef uint8_t unsigned_t;
	typedef int_fast8_t signed_fast_t;
	typedef int8_t signed_t;
};
template<>
class IntLength<16>{
public:
	typedef uint_fast16_t unsigned_fast_t;
	typedef uint16_t unsigned_t;
	typedef int_fast16_t signed_fast_t;
	typedef int16_t signed_t;
};
template<>
class IntLength<32>{
public:
	typedef uint_fast32_t unsigned_fast_t;
	typedef uint32_t unsigned_t;
	typedef int_fast32_t signed_fast_t;
	typedef int32_t signed_t;
};
template<>
class IntLength<64>{
public:
	typedef uint_fast64_t unsigned_fast_t;
	typedef uint64_t unsigned_t;
	typedef int_fast64_t signed_fast_t;
	typedef int64_t signed_t;
};



template <size_t i_bits, size_t f_bits>
class uFractional {
public:
	typedef typename IntLength<i_bits + f_bits>::unsigned_t internal_t;
	typedef typename IntLength<(i_bits + f_bits) * 2>::unsigned_t double_t;

	template<size_t mi_bits, size_t mf_bits>
	constexpr uFractional(const uFractional<mi_bits, mf_bits> &val) :
		i(val.normalize<i_bits, f_bits>().i){}
	constexpr uFractional(const uFractional<i_bits, f_bits> &val) :
		i(val.i){}
	constexpr uFractional(const internal_t &val) :
		i(val){}
	constexpr uFractional(const double &val) :
		i(val * (1 << f_bits)){}
	constexpr uFractional(const float &val) :
		i(val * (1 << f_bits)){}
	constexpr uFractional(const int &val) :
		i(((unsigned int)val) << f_bits){}
	constexpr double asDouble() const{
		return ((double)i) / (1 << f_bits);
	}

	/*!
	 * @brief Convert a fractional number to a different format
	 */
	template<size_t ni_bits, size_t nf_bits>
	constexpr uFractional<ni_bits, nf_bits> normalize() const {
		typedef uFractional<ni_bits, nf_bits> n_t;
		return n_t((typename n_t::internal_t) (i >> (f_bits - nf_bits)));
	}

	/*!
	 * @brief Resize the integer part of a number
	 *
	 * This will not change the value unless it exceeds the final format
	 */
	template<size_t ni_bits>
	constexpr uFractional<ni_bits, f_bits> resize() const {
		typedef uFractional<ni_bits, f_bits> n_t;
		return n_t((typename n_t::internal_t) i);
	}

	template<size_t mi_bits, size_t mf_bits>
	constexpr uFractional<mi_bits + i_bits, mf_bits + f_bits> operator * (uFractional<mi_bits, mf_bits> &m) const {
		typedef typename IntLength<mi_bits + i_bits + mf_bits + f_bits>::unsigned_t res_t;
		return (res_t)m.i * i;
	}

	template<size_t mi_bits, size_t mf_bits>
	constexpr uFractional<max(mi_bits, i_bits), max(mf_bits, f_bits)> operator + (uFractional<mi_bits, mf_bits> &m) const {
		return m.i + i;
	}

	template<size_t mi_bits, size_t mf_bits>
	constexpr uFractional<max(mi_bits, i_bits), max(mf_bits, f_bits)> operator - (uFractional<mi_bits, mf_bits> &m) const {
		return m.i - i;
	}

//private:
	internal_t i : i_bits + f_bits;
};



/*!
 * @brief A signed fractional integer
 * @tparam i_bits The number of integer bits (not including sign)
 * @tparam f_bits The number of fractional bits
 */
template <size_t i_bits, size_t f_bits>
class sFractional {
public:
	typedef typename IntLength<i_bits + f_bits + 1>::signed_t internal_t;

	constexpr sFractional(const sFractional<i_bits, f_bits> &val) :
			 i(val.i){}

	constexpr sFractional() {}

	/*!
	 * @brief Constructor for arbitrary signed fractionals
	 */
	template<size_t mi_bits, size_t mf_bits>
	constexpr sFractional(const sFractional<mi_bits, mf_bits> &val) :
		i(val.normalize<i_bits, f_bits>().i){}
	constexpr sFractional(const internal_t &val) :
			 i(val){}
	constexpr sFractional(const double &val) :
		i(val * (1 << f_bits)){}
	constexpr sFractional(const float &val) :
		i(val * (1 << f_bits)){}
//	constexpr sFractional(const int &val) :
//		i(((signed int)val) << f_bits){}
	constexpr double asDouble() const{
		return ((double)i) / (double)(((unsigned)1) << (f_bits));
	}

	void setInternal(internal_t internal){
		i = internal;
	}
	
	/*!
	 * @brief Convert a fractional number to a different format
	 */
	template<size_t ni_bits, size_t nf_bits>
	constexpr sFractional<ni_bits, nf_bits> normalize() const {
		typedef sFractional<ni_bits, nf_bits> n_t;
		return n_t((typename n_t::internal_t) (i >> (f_bits - nf_bits)));
	}

	/*!
	 * @brief Resize the integer part of a number
	 *
	 * This will not change the value unless it exceeds the final format
	 */
	template<size_t ni_bits>
	constexpr sFractional<ni_bits, f_bits> resize() const {
		typedef sFractional<ni_bits, f_bits> n_t;
		return n_t((typename n_t::internal_t) i);
	}

	template<size_t mi_bits, size_t mf_bits>
	sFractional<mi_bits + i_bits, mf_bits + f_bits> operator * (sFractional<mi_bits, mf_bits> const &m) const {
		typedef typename IntLength<mi_bits + i_bits + mf_bits + f_bits + 1>::signed_t res_t;
		sFractional<mi_bits + i_bits, mf_bits + f_bits> out;
		out.setInternal((res_t)m.i * (res_t)i);
		return out;
	}


	constexpr sFractional<2 * i_bits, 2 * f_bits> square() {
		return *this * *this;
	}

	template<size_t mi_bits, size_t mf_bits>
	constexpr sFractional<mi_bits + i_bits, mf_bits + f_bits> operator * (uFractional<mi_bits, mf_bits> const &m) const {
		typedef typename IntLength<mi_bits + i_bits + mf_bits + f_bits + 1>::signed_t res_t;
		return (res_t)m.i * i;
	}

	template<size_t mi_bits, size_t mf_bits>
	constexpr sFractional<max(mi_bits, i_bits), max(mf_bits, f_bits)> operator + (sFractional<mi_bits, mf_bits> const &m) const {
		return m.i + i;
	}

	template<size_t mi_bits, size_t mf_bits>
	constexpr sFractional<max(mi_bits, i_bits), max(mf_bits, f_bits)> operator - (sFractional<mi_bits, mf_bits> const &m) const {
		return m.i - i;
	}

//private:
	internal_t i : i_bits + f_bits + 1;
};

/*!
 @brief Compute dst = a * B
 */
template<typename T>
void vector_mult_scalar(T a, T const * B, T * dst, size_t n){
	while(n >= 8){
		T b1 = *B++;
		T b2 = *B++;
		T b3 = *B++;
		T b4 = *B++;
		T b5 = *B++;
		T b6 = *B++;
		T b7 = *B++;
		T b8 = *B++;

		T out1 = (b1 * a);
		T out2 = (b2 * a);
		T out3 = (b3 * a);
		T out4 = (b4 * a);
		T out5 = (b5 * a);
		T out6 = (b6 * a);
		T out7 = (b7 * a);
		T out8 = (b8 * a);

		*dst++ = out1;
		*dst++ = out2;
		*dst++ = out3;
		*dst++ = out4;
		*dst++ = out5;
		*dst++ = out6;
		*dst++ = out7;
		*dst++ = out8;

		n -= 8;
	}
	while(n){
		*dst++ = (*B++ * a);
		n -= 1;
	}
}

/*!
 @brief Compute dst = aX + bY

 This performs each computation in groups of 4 outputs at a time.
 This is more-or-less optimized for 32-bit types
 */
template<typename T>
void vector_dual_mult_scalar_sum(T const a,
                                 T const b,
                                 T const * X,
                                 T const * Y,
                                 T * dst,
                                 size_t n){
	while(n >= 4){
		T x1 = *X++;
		T x2 = *X++;
		T x3 = *X++;
		T x4 = *X++;

		T y1 = *Y++;
		T y2 = *Y++;
		T y3 = *Y++;
		T y4 = *Y++;

		T out1 = (x1 * a) + (y1 * b);
		T out2 = (x2 * a) + (y2 * b);
		T out3 = (x3 * a) + (y3 * b);
		T out4 = (x4 * a) + (y4 * b);

		*dst++ = out1;
		*dst++ = out2;
		*dst++ = out3;
		*dst++ = out4;

		n -= 4;
	}
	while(n){
		*dst++ = (*X++ * a + *Y++ * b);
		n -= 1;
	}
}

/*!
 @brief Compute the real power of a complex transform output from ARM libraries
 @param X The input array of complex values, as returned from CMSIS-DSP
 @param dst A buffer for output -- may be the same as input
  It must be able to hold at least (n/2 + 1) elements
 @param n The number of input elements
 */
template<typename T, typename To>
void complex_power(T const * X, To * dst, size_t n){
	// First bin is real X[0]
	*dst++ = *X * *X;
	X++;
	// Second bin is real X[N/2]
	To tmp = *X * *X;
	X++;
	n -= 2;
	while(n >= 8){
		T x1 = *X++;
		T x2 = *X++;
		T x3 = *X++;
		T x4 = *X++;
		T x5 = *X++;
		T x6 = *X++;
		T x7 = *X++;
		T x8 = *X++;

		To out1 = (To)(x1*x1) + (To)(x2*x2);
		To out2 = (To)(x3*x3) + (To)(x4*x4);
		To out3 = (To)(x5*x5) + (To)(x6*x6);
		To out4 = (To)(x7*x7) + (To)(x8*x8);


		*dst++ = out1;
		*dst++ = out2;
		*dst++ = out3;
		*dst++ = out4;

		n -= 8;
	}
	while(n > 1){
		T x1 = *X++;
		T x2 = *X++;
		*dst++ = (To)(x1*x1) + (To)(x2*x2);
	}
	*dst = tmp;
}

template<typename T>
void zero4(T * dst, uint32_t n){
	register uint32_t loops = (n + 15) >> 4;
	if(!n) return; // But why would anyone do this?
	switch(n & 0xF){
	case 0: do { *dst++ = 0;
	case 15: *dst++ = 0;
	case 14: *dst++ = 0;
	case 13: *dst++ = 0;
	case 12: *dst++ = 0;
	case 11: *dst++ = 0;
	case 10: *dst++ = 0;
	case 9: *dst++ = 0;
	case 8: *dst++ = 0;
	case 7: *dst++ = 0;
	case 6: *dst++ = 0;
	case 5: *dst++ = 0;
	case 4: *dst++ = 0;
	case 3: *dst++ = 0;
	case 2: *dst++ = 0;
	case 1: *dst++ = 0;
		} while(--loops > 0);
	}
}

#endif // __APULSE_MATH_H_
