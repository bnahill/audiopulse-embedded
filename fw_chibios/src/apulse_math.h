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
#include <cstddef>

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
    static constexpr size_t bits = 8;
    typedef uint_fast8_t unsigned_fast_t;
    typedef uint8_t unsigned_t;
    typedef int_fast8_t signed_fast_t;
    typedef int8_t signed_t;
};
template<>
class IntLength<16>{
public:
    static constexpr size_t bits = 16;
    typedef uint_fast16_t unsigned_fast_t;
    typedef uint16_t unsigned_t;
    typedef int_fast16_t signed_fast_t;
    typedef int16_t signed_t;
};
template<>
class IntLength<32>{
public:
    static constexpr size_t bits = 32;
    typedef uint_fast32_t unsigned_fast_t;
    typedef uint32_t unsigned_t;
    typedef int_fast32_t signed_fast_t;
    typedef int32_t signed_t;
};
template<>
class IntLength<64>{
public:
    static constexpr size_t bits = 64;
    typedef uint_fast64_t unsigned_fast_t;
    typedef uint64_t unsigned_t;
    typedef int_fast64_t signed_fast_t;
    typedef int64_t signed_t;
};



template <size_t i_bits, size_t f_bits>
class uFractional {
public:
    typedef typename IntLength<i_bits + f_bits>::unsigned_t internal_t;
    static constexpr size_t bits =  IntLength<i_bits + f_bits>::bits;
    static constexpr size_t bits_i = i_bits;
    static constexpr size_t bits_f = f_bits;

    constexpr uFractional() {}
    template<size_t mi_bits, size_t mf_bits>
    //constexpr uFractional(const uFractional<mi_bits, mf_bits> &val) :
    //	i(val.normalize<i_bits, f_bits>().i){}
    constexpr uFractional(const uFractional<i_bits, f_bits> &val) :
        i(val.i){}
    //constexpr uFractional(const internal_t &val) :
    //	i(val){}
    constexpr uFractional(const double val) :
        i(val * (double)((typename IntLength<f_bits + 1>::unsigned_t)1 << f_bits)){}
    constexpr uFractional(const float val) :
        i(val * (float)((typename IntLength<f_bits + 1>::unsigned_t)1 << f_bits)){}
    constexpr double asDouble() const{
        return ((double)i) / (double)((1 << f_bits) - 1);
    }
    constexpr float asFloat() const {
        return ((float)i) / (float)(((unsigned)1) << (f_bits));
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
        return i + m.i;
    }

    template<size_t mi_bits, size_t mf_bits>
    constexpr uFractional<max(mi_bits, i_bits), max(mf_bits, f_bits)> operator - (uFractional<mi_bits, mf_bits> &m) const {
        return i - m.i;
    }

    constexpr uFractional<i_bits, f_bits> operator - (uFractional<i_bits, f_bits> &m) const {
        return i - m.i;
    }


    static constexpr uFractional<i_bits,f_bits> minval(){return (internal_t)0;}
    static constexpr uFractional<i_bits,f_bits> maxval(){return (internal_t)(-1);}

    internal_t i : i_bits + f_bits;
} __attribute__((packed));



/*!
 * @brief A signed fractional integer
 * @tparam i_bits The number of integer bits (not including sign)
 * @tparam f_bits The number of fractional bits
 */
template <size_t i_bits, size_t f_bits>
class sFractional {
public:
    typedef typename IntLength<i_bits + f_bits + 1>::signed_t internal_t;
    static constexpr size_t bits =  IntLength<i_bits + f_bits + 1>::bits;
    static constexpr size_t bits_i = i_bits;
    static constexpr size_t bits_f = f_bits;

    constexpr inline sFractional(const sFractional<i_bits, f_bits> &val) :
        i(val.i){}

    constexpr inline sFractional(int i) :
        i(i){}

    constexpr sFractional() :
        i(0){}

    /*!
         * @brief Constructor for arbitrary signed fractionals
         */
    template< size_t mi_bits, size_t mf_bits >
    //constexpr inline sFractional(const sFractional<mi_bits, mf_bits> &val) :
    //        i( val.normalize< i_bits, f_bits >().i){}
    static constexpr inline sFractional<i_bits, f_bits> fromInternal(const internal_t val){
        return *reinterpret_cast< const sFractional<i_bits, f_bits> *>(&val);
    }
    constexpr inline sFractional(const double val) :
        i(val * ( (typename IntLength<i_bits + f_bits + 1>::unsigned_t)1 << f_bits) - 1){}
    //constexpr sFractional(const float &val) :
    //	i(val * (1 << f_bits)){}
    //	constexpr sFractional(const int &val) :
    //		i(((signed int)val) << f_bits){}
    constexpr double asDouble() const {
        return ((double)i) / (double)(((unsigned)1) << (f_bits));
    }
    constexpr float asFloat() const {
        return ((float)i) / (float)(((unsigned)1) << (f_bits));
    }

    void setInternal(internal_t internal){
        i = internal;
    }

    /*!
         * @brief Convert a fractional number to a different format
         */
    template<int ni_bits, int nf_bits>
    constexpr inline sFractional<ni_bits, nf_bits> normalize() const {
        typedef sFractional<ni_bits, nf_bits> n_t;
        return (nf_bits < f_bits) ?
                    n_t::fromInternal((typename n_t::internal_t) (i >> (f_bits - nf_bits))) :
                    n_t::fromInternal((typename n_t::internal_t) (i << (nf_bits - f_bits)));
    }

    /*!
         * @brief Resize the integer part of a number
         *
         * This will not change the value unless it exceeds the final format
         */
    template<size_t ni_bits>
    constexpr inline sFractional<ni_bits, f_bits> resize() const {
        typedef sFractional<ni_bits, f_bits> n_t;
        return n_t((typename n_t::internal_t) i);
    }

    template<size_t mi_bits, size_t mf_bits>
    sFractional<mi_bits + i_bits, mf_bits + f_bits> operator * (sFractional<mi_bits, mf_bits> const m) const {
        typedef typename IntLength<mi_bits + i_bits + mf_bits + f_bits + 1>::signed_t res_t;
        return sFractional<mi_bits + i_bits, mf_bits + f_bits>::fromInternal((res_t)m.i * (res_t)i);
    }


    constexpr sFractional<2 * i_bits, 2 * f_bits> square() {
        return *this * *this;
    }

    static sFractional<i_bits, f_bits> mk_frac(internal_t a, internal_t b){
        typedef typename IntLength<2*(i_bits + f_bits) + 1>::signed_t double_len_t;
        double_len_t shifted = ((double_len_t)a) << (i_bits + f_bits);
        shifted = (shifted / ((double_len_t)b)) >> (i_bits + 1);
        return sFractional<i_bits, f_bits>::fromInternal((sFractional<i_bits, f_bits>::internal_t) shifted);
    }

    template<size_t mi_bits, size_t mf_bits>
    constexpr inline sFractional<mi_bits + i_bits, mf_bits + f_bits> operator * (uFractional<mi_bits, mf_bits> const m) const {
        typedef sFractional<mi_bits + i_bits, mf_bits + f_bits> res_t;
        typedef typename res_t::internal_t res_internal_t;
        return res_t::fromInternal((res_internal_t)i * m.i);
    }

    template<size_t mi_bits, size_t mf_bits>
    constexpr inline sFractional<max(mi_bits, i_bits), max(mf_bits, f_bits)> operator + (sFractional<mi_bits, mf_bits> const m) const {
        typedef sFractional<max(mi_bits, i_bits), max(mf_bits, f_bits)> res_t;
        typedef typename res_t::internal_t res_internal_t;
        return res_t::fromInternal((res_internal_t)i + m.i);
    }

    template<size_t mi_bits, size_t mf_bits>
    constexpr inline sFractional<max(mi_bits, i_bits), max(mf_bits, f_bits)> operator - (sFractional<mi_bits, mf_bits> const m) const {
        typedef sFractional<max(mi_bits, i_bits), max(mf_bits, f_bits)> res_t;
        typedef typename res_t::internal_t res_internal_t;
        return res_t::fromInternal((res_internal_t)i - m.i);
    }

    constexpr inline sFractional<i_bits, f_bits> operator-() const {
        return sFractional<i_bits, f_bits>::fromInternal(-i);
    }


    static constexpr sFractional<i_bits,f_bits> minval(){return (internal_t)(1 << (f_bits + i_bits));}
    static constexpr sFractional<i_bits,f_bits> maxval(){return (internal_t)minval().i - 1;}

    internal_t i : i_bits + f_bits + 1;
} __attribute__((packed));

/*!
 @brief Compute dst = a * B
 */
template<typename Ts, typename T>
void vector_mult_scalar(Ts a, T const * B, T * dst, size_t n){
    while(n >= 8){
        T b1 = B[0];
        T b2 = B[1];
        T b3 = B[2];
        T b4 = B[3];
        T b5 = B[4];
        T b6 = B[5];
        T b7 = B[6];
        T b8 = B[7];

        B += 8;

        T out1 = (b1 * a);
        T out2 = (b2 * a);
        T out3 = (b3 * a);
        T out4 = (b4 * a);
        T out5 = (b5 * a);
        T out6 = (b6 * a);
        T out7 = (b7 * a);
        T out8 = (b8 * a);

        dst[0] = out1;
        dst[1] = out2;
        dst[2] = out3;
        dst[3] = out4;
        dst[4] = out5;
        dst[5] = out6;
        dst[6] = out7;
        dst[7] = out8;

        dst += 8;

        n -= 8;
    }
    while(n){
        *dst++ = (*B++ * a);
        n -= 1;
    }
}

template<typename T>
T round_unsigned(float x){
    return static_cast<T>(x+0.5);
}

/*!
 @brief Compute dst = aX + bY

 This performs each computation in groups of 4 outputs at a time.
 This is more-or-less optimized for 32-bit types
 */
template<typename T>
void weighted_vector_sum(T const a,
                         T const * X,
                         T const b,
                         T const * Y,
                         T * dst,
                         size_t n){
    //static_assert(T::bits == 32, "Using weighted_vector_sum for wrong size");
    while(n >= 4){
        register T x1 __asm__("r4") = X[0];
        register T x2 __asm__("r5") = X[1];
        register T x3 __asm__("r6") = X[2];
        register T x4 __asm__("r7") = X[3];

        register T y1 __asm__("r8") = Y[0];
        register T y2 __asm__("r9") = Y[1];
        register T y3 __asm__("r10") = Y[2];
        register T y4 __asm__("r11") = Y[3];

        //__asm__("ldmia %0!,{%1,%2, %3, %4}"
        //      : "+&r"(X), "=r"(x1), "=r"(x2), "=r"(x3), "=r"(x4));

        X += 4;
        Y += 4;

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
 @brief Compute dst = aX + Y
 @param a The scalar multiplier for X
 @param X The first input vector
 @param Y The second input vector (which will not be multiplied)
 @param dst The target location
 @param n The number of words to process

 This performs each computation in groups of 4 outputs at a time.
 This is more-or-less optimized for 32-bit types
 */
template<typename T, typename Ts, typename Tr>
void vector_mac(Ts const a,
                T const * X,
                Tr const * Y,
                Tr * dst,
                size_t n){
    while(n >= 4){
        T x1 = X[0];
        T x2 = X[1];
        T x3 = X[2];
        T x4 = X[3];

        Tr y1 = Y[0];
        Tr y2 = Y[1];
        Tr y3 = Y[2];
        Tr y4 = Y[3];

        X += 4;
        Y += 4;

        Tr out1 = (x1 * a) + (y1);
        Tr out2 = (x2 * a) + (y2);
        Tr out3 = (x3 * a) + (y3);
        Tr out4 = (x4 * a) + (y4);

        *dst++ = out1;
        *dst++ = out2;
        *dst++ = out3;
        *dst++ = out4;

        n -= 4;
    }
    while(n){
        *dst++ = (*X++ * a + *Y++);
        n -= 1;
    }
}



/*!
 @brief Compute the real power of a complex transform output from ARM libraries
   and compute weighted sum with the previous average PSD
 @param a A scalar to multiply with power of X
 @param X The input array of complex values, as returned from CMSIS-DSP
 @param Y The old PSD to which the new transform will be added
 @param dst A buffer for output -- may be the same as input
  It must be able to hold at least (n/2 + 1) elements
 @param n The number of input elements

 Computes (roughly) dst = a * mag(X)^2 + Y
 X is formatted a bit funny, hence the "roughly"

 @note Y may refer to the same buffer as dst
 */
template<typename T, typename Ts, typename Tpwr>
void complex_psd_mac(Ts a,
                     T const * X,
                     Tpwr const * Y,
                     Tpwr * dst,
                     size_t n){

    dst[0] = X[0]*X[0]*a + Y[0];
    X += 1;
    dst += 1;
    Y += 1;
    n -= 1;

    while(n >= 8){
        T x1 = X[0];
        T x2 = X[1];
        T x3 = X[2];
        T x4 = X[3];
        T x5 = X[4];
        T x6 = X[5];
        T x7 = X[6];
        T x8 = X[7];

        X += 8;

        Tpwr pwr1 = (x1*x1) + (x2*x2);
        Tpwr pwr2 = (x3*x3) + (x4*x4);
        Tpwr pwr3 = (x5*x5) + (x6*x6);
        Tpwr pwr4 = (x7*x7) + (x8*x8);

        Tpwr old1 = Y[0];
        Tpwr old2 = Y[1];
        Tpwr old3 = Y[2];
        Tpwr old4 = Y[3];

        Y += 4;

        old1 = (pwr1 * a) + (old1);
        old2 = (pwr2 * a) + (old2);
        old3 = (pwr3 * a) + (old3);
        old4 = (pwr4 * a) + (old4);

        dst[0] = old1;
        dst[1] = old2;
        dst[2] = old3;
        dst[3] = old4;

        dst += 4;

        n -= 8;
    }
    while(n > 1){
        T x1 = *X++;
        T x2 = *X++;
        Tpwr pwr1 = (x1*x1) + (x2*x2);
        *dst++ = pwr1 * a + *Y++;
        n -= 2;
    }
    if(n == 1){
        dst[0] = X[0]*X[0]*a + Y[0];
    }
}

/*!
 @brief Compute the real power of a complex transform output from ARM libraries
   and add it to the previous average
 @param X The input array of complex values, as returned from CMSIS-DSP
 @param Y The old PSD to which the new transform will be added
 @param dst A buffer for output -- may be the same as input
  It must be able to hold at least (n/2 + 1) elements
 @param n The number of input elements

 Computes (roughly) dst = mag(X)^2 + Y
 X is formatted a bit funny, hence the "roughly"

 @note Y may refer to the same buffer as dst
 */
template<typename T, typename Tpwr>
void complex_psd_acc(T const * X,
                     Tpwr const * Y,
                     Tpwr * dst,
                     size_t n){

    while(n >= 8){
        T x1 = X[0];
        T x2 = X[1];
        T x3 = X[2];
        T x4 = X[3];
        T x5 = X[4];
        T x6 = X[5];
        T x7 = X[6];
        T x8 = X[7];

        X += 8;

        Tpwr pwr1 = (x1*x1) + (x2*x2);
        Tpwr pwr2 = (x3*x3) + (x4*x4);
        Tpwr pwr3 = (x5*x5) + (x6*x6);
        Tpwr pwr4 = (x7*x7) + (x8*x8);

        Tpwr old1 = Y[0];
        Tpwr old2 = Y[1];
        Tpwr old3 = Y[2];
        Tpwr old4 = Y[3];

        Y += 4;

        old1 = (pwr1) + (old1);
        old2 = (pwr2) + (old2);
        old3 = (pwr3) + (old3);
        old4 = (pwr4) + (old4);

        dst[0] = old1;
        dst[1] = old2;
        dst[2] = old3;
        dst[3] = old4;

        dst += 4;

        n -= 8;
    }
    while(n > 1){
        T x1 = *X++;
        T x2 = *X++;
        Tpwr pwr1 = (x1*x1) + (x2*x2);
        *dst++ = pwr1 + *Y++;
        n -= 2;
    }
}

template<typename T>
class RangeChecker {
public:
    RangeChecker() :
        min(0), max(0){}

    void check(T val){
        if(val > max){max = val;}
        if(val < min){min = val;}
    }

    void reset(){min = 0; max = 0;}

    T min, max;
};

/*!
 @brief Compute the real power of a complex transform output from ARM libraries
   and add it to the previous average
 @param a A scalar to multiply with power output
 @param X The input array of complex values, as returned from CMSIS-DSP
 @param dst A buffer for output -- may be the same as input
  It must be able to hold at least (n/2 + 1) elements
 @param n The number of input elements

 Computes (roughly) dst = a * mag(X)^2 + b * Y
 X is formatted a bit funny, hence the "roughly"

 @note Y may refer to the same buffer as dst
 */
template<typename T, typename Tpwr>
void complex_power_avg_update(Tpwr a,
                              T const * X,
                              Tpwr b,
                              Tpwr const * Y,
                              Tpwr * dst,
                              size_t n){
    // 	// First bin is real X[0]
    // 	*dst++ = (Tpwr)(*X * *X) * a + (Tpwr)(*Y * b);
    // 	X++;
    // 	// Second bin is real X[N/2]
    // 	dst[n/2] = (Tpwr)(*X * *X) * a + (Tpwr)(Y[n/2] * b);
    // 	Y++;
    // 	X++;
    // 	n -= 2;
    while(n >= 8){
        T x1 = X[0];
        T x2 = X[1];
        T x3 = X[2];
        T x4 = X[3];
        T x5 = X[4];
        T x6 = X[5];
        T x7 = X[6];
        T x8 = X[7];

        X += 8;

        Tpwr pwr1 = (x1*x1) + (x2*x2);
        Tpwr pwr2 = (x3*x3) + (x4*x4);
        Tpwr pwr3 = (x5*x5) + (x6*x6);
        Tpwr pwr4 = (x7*x7) + (x8*x8);

        Tpwr old1 = Y[0];
        Tpwr old2 = Y[1];
        Tpwr old3 = Y[2];
        Tpwr old4 = Y[3];

        Y += 4;

        old1 = (pwr1 * a) + (old1 * b);
        old2 = (pwr2 * a) + (old2 * b);
        old3 = (pwr3 * a) + (old3 * b);
        old4 = (pwr4 * a) + (old4 * b);

        dst[0] = old1;
        dst[1] = old2;
        dst[2] = old3;
        dst[3] = old4;

        dst += 4;

        n -= 8;
    }
    while(n > 1){
        T x1 = *X++;
        T x2 = *X++;
        Tpwr pwr1 = (x1*x1) + (x2*x2);
        *dst++ = pwr1 * a + *Y++ * b;
        n -= 2;
    }
}

template<typename T, typename Tpwr>
void complex_to_psd(T const * X, Tpwr * dst, uint32_t n){
    while(n >= 4){
        T x1 = X[0];
        T x2 = X[1];
        T x3 = X[2];
        T x4 = X[3];
        T x5 = X[4];
        T x6 = X[5];
        T x7 = X[6];
        T x8 = X[7];

        X += 8;

        Tpwr pwr1 = (Tpwr)(x1*x1) + (Tpwr)(x2*x2);
        Tpwr pwr2 = (Tpwr)(x3*x3) + (Tpwr)(x4*x4);
        Tpwr pwr3 = (Tpwr)(x5*x5) + (Tpwr)(x6*x6);
        Tpwr pwr4 = (Tpwr)(x7*x7) + (Tpwr)(x8*x8);

        *dst++ = pwr1;
        *dst++ = pwr2;
        *dst++ = pwr3;
        *dst++ = pwr4;

        n -= 4;
    }
    while(n){
        T x1 = *X++;
        T x2 = *X++;
        *dst++ = (x1*x1) + (x2*x2);
        n -= 1;
    }
}


/*!
 @brief An unrolled zeroing
 @param dst Destination
 @param n Number of elements
 */
template<typename T>
void zero16(T * dst, uint32_t n){
    while(n >= 16){
        dst[0] = 0;
        dst[1] = 0;
        dst[2] = 0;
        dst[3] = 0;
        dst[4] = 0;
        dst[5] = 0;
        dst[6] = 0;
        dst[7] = 0;
        dst[8] = 0;
        dst[9] = 0;
        dst[10] = 0;
        dst[11] = 0;
        dst[12] = 0;
        dst[13] = 0;
        dst[14] = 0;
        dst[15] = 0;
        dst += 16;
        n -= 16;
    }
    while(n--){
        *dst++ = 0;
    }
}

//template<typename Tdat, typename Tcoeff, typename Tout, size_t Nblock>
//class BiQuad {
//public:
//	typedef union {
//		Tout out[Nblock];
//		Tout a;
//	} out_t;
//	typedef Tdat const in_t[Nblock];
//	BiQuad(Tcoeff a1, Tcoeff a2, Tcoeff b0, Tcoeff b1, Tcoeff b2) :
//	    a1(a1), a2(a2), b0(b0), b1(b1), b2(b2),
//	    z_a0(0), z_a1(1), z_b0(0), z_b1(0)
//	{}

//	void push(in_t samples, Tout dst[]){
//		Tdat out;
//		for(auto& x : samples){
//			out = (b0 * x) + (b1 * z_b0) + (b2 * z_b1) +
//			      (a1 * z_a0) + (a2 * z_a1);
//			z_a1 = z_a0;
//			z_a0 = out;

//			z_b1 = z_b0;
//			z_b0 = x;
//			dst++ = out;
//		}
//	}

//private:
//	Tcoeff const a1, a2, b0, b1, b2;
//	Tdat z_a0, z_a1;
//	Tout z_b0, z_b1;
//};

//template<typename Tin, typename Tcoeff, typename Tout>
//class BiQuad {
//	BiQuad(Tcoeff a1, Tcoeff a2, Tcoeff b0, Tcoeff b1, Tcoeff b2) :
//	    a1(a1), a2(a2), b0(b0), b1(b1), b2(b2),
//	    z_a0(0), z_a1(1), z_b0(0), z_b1(0){}

//	Tout push(Tin const x[], Tout dst[], size_t n){
//		Tout out;
//		while(n--){
//			out = (b0 * x) + (b1 * z_b0) + (b2 * z_b1) +
//				  (a1 * z_a0) + (a2 * z_a1);
//			z_a1 = z_a0;
//			z_a0 = out;

//			z_b1 = z_b0;
//			z_b0 = x++;
//			*(dst++) = out;
//		}
//	}

//private:
//	Tcoeff const a1, a2, b0, b1, b2;
//	Tin z_a0, z_a1;
//	Tout z_b0, z_b1;
//};

//template<typename... Tbiquads>
//class BiQuadCascade {
//public:
//	BiQuadCascade(BiQuad<Tin, Tcoeff, Tinter> &first,
//	              BiQuad<Tinter, Tcoeff, Tinter> * others)
//	{}

//	void push(Tin const * samples, Tout dst[]){

//	}

//private:
//	Tcoeff const a1, a2, b0, b1, b2;
//	Tdat z_a0, z_a1;
//	Tout z_b0, z_b1;
//};

#endif // __APULSE_MATH_H_

