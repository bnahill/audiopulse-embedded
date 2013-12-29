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
	double asDouble() const{
		return ((double)i) / (1 << f_bits);
	}

	/*!
	 * @brief Convert a fractional number to a different format
	 */
	template<size_t ni_bits, size_t nf_bits>
	uFractional<ni_bits, nf_bits> normalize(){
		typedef uFractional<ni_bits, nf_bits> n_t;
		return n_t((typename n_t::internal_t) (i >> (f_bits - nf_bits)));
	}

	/*!
	 * @brief Resize the integer part of a number
	 *
	 * This will not change the value unless it exceeds the final format
	 */
	template<size_t ni_bits>
	uFractional<ni_bits, f_bits> resize(){
		typedef uFractional<ni_bits, f_bits> n_t;
		return n_t((typename n_t::internal_t) i);
	}

	template<size_t mi_bits, size_t mf_bits>
	uFractional<mi_bits + i_bits, mf_bits + f_bits> operator * (uFractional<mi_bits, mf_bits> &m){
		typedef typename IntLength<mi_bits + i_bits + mf_bits + f_bits>::unsigned_t res_t;
		return (res_t)m.i * i;
	}

	template<size_t mi_bits, size_t mf_bits>
	uFractional<max(mi_bits, i_bits), max(mf_bits, f_bits)> operator + (uFractional<mi_bits, mf_bits> &m){
		return m.i + i;
	}

	template<size_t mi_bits, size_t mf_bits>
	uFractional<max(mi_bits, i_bits), max(mf_bits, f_bits)> operator - (uFractional<mi_bits, mf_bits> &m){
		return m.i - i;
	}

private:
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

	/*!
	 * @brief Constructor for arbitrary signed fractionals
	 */
	template<size_t mi_bits, size_t mf_bits>
	constexpr sFractional(const sFractional<mi_bits, mf_bits> &val) :
		i(val.normalize<mi_bits, mf_bits>().i){}
	constexpr sFractional(const internal_t &val) :
			 i(val){}
	constexpr sFractional(const double &val) :
		i(val * (1 << f_bits)){}
	constexpr sFractional(const float &val) :
		i(val * (1 << f_bits)){}
	constexpr sFractional(const int &val) :
		i(((signed int)val) << f_bits){}
	double asDouble() const{
		return ((double)i) / (1 << f_bits);
	}

	/*!
	 * @brief Convert a fractional number to a different format
	 */
	template<size_t ni_bits, size_t nf_bits>
	sFractional<ni_bits, nf_bits> normalize(){
		typedef sFractional<ni_bits, nf_bits> n_t;
		return n_t((typename n_t::internal_t) (i >> (f_bits - nf_bits)));
	}

	/*!
	 * @brief Resize the integer part of a number
	 *
	 * This will not change the value unless it exceeds the final format
	 */
	template<size_t ni_bits>
	sFractional<ni_bits, f_bits> resize(){
		typedef sFractional<ni_bits, f_bits> n_t;
		return n_t((typename n_t::internal_t) i);
	}

	template<size_t mi_bits, size_t mf_bits>
	sFractional<mi_bits + i_bits, mf_bits + f_bits> operator * (sFractional<mi_bits, mf_bits> &m){
		typedef typename IntLength<mi_bits + i_bits + mf_bits + f_bits + 1>::signed_t res_t;
		return (res_t)m.i * i;
	}

	template<size_t mi_bits, size_t mf_bits>
	sFractional<max(mi_bits, i_bits), max(mf_bits, f_bits)> operator + (sFractional<mi_bits, mf_bits> &m){
		return m.i + i;
	}

	template<size_t mi_bits, size_t mf_bits>
	sFractional<max(mi_bits, i_bits), max(mf_bits, f_bits)> operator - (sFractional<mi_bits, mf_bits> &m){
		return m.i - i;
	}

private:
	internal_t i : i_bits + f_bits + 1;
};