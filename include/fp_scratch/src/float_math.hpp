#pragma once

#include "float_type.hpp"

namespace fp_scratch
{
    template <uint8_t num_exp_bits, uint8_t num_mantissa_bits>
    constexpr auto negate(Float<num_exp_bits, num_mantissa_bits> x)
    {
        auto y = x;
        y.sign_bit = !x.sign_bit;
        return y;
    }

    template <uint8_t num_exp_bits, uint8_t num_mantissa_bits>
    constexpr auto add(Float<num_exp_bits, num_mantissa_bits> a, Float<num_exp_bits, num_mantissa_bits> b)
    {
        using primitive_t = typename Float<num_exp_bits, num_mantissa_bits>::primitive_type;

        const auto a_exp_int = detail::bits_to_sint<num_exp_bits>(a.exponent);
        const auto b_exp_int = detail::bits_to_sint<num_exp_bits>(b.exponent);
        const auto exp_difference = a_exp_int - b_exp_int;

        auto a_mantissa_int = static_cast<int64_t>(detail::bits_to_uint<num_mantissa_bits>(a.mantissa));
        auto b_mantissa_int = static_cast<int64_t>(detail::bits_to_uint<num_mantissa_bits>(b.mantissa));

        const auto shift = static_cast<uint64_t>(detail::pow10<primitive_t>(std::abs(exp_difference)));
        if (exp_difference > 0)
            b_mantissa_int /= shift;
        if (exp_difference < 0)
            a_mantissa_int /= shift;

        a_mantissa_int = a.sign_bit ? -a_mantissa_int : a_mantissa_int;
        b_mantissa_int = b.sign_bit ? -b_mantissa_int : b_mantissa_int;
        const auto mantissa_sum = a_mantissa_int + b_mantissa_int;

        Float<num_exp_bits, num_mantissa_bits> y{};
        y.sign_bit = mantissa_sum < 0;
        y.exponent = detail::sint_to_bits<num_exp_bits>(std::max(a_exp_int, b_exp_int));
        y.mantissa = detail::uint_to_bits<num_mantissa_bits>(static_cast<uint64_t>(std::abs(mantissa_sum)));

        // @TODO: handle mantissa over/underflow

        return y;
    }

    template <uint8_t num_exp_bits, uint8_t num_mantissa_bits>
    constexpr auto subtract(Float<num_exp_bits, num_mantissa_bits> a, Float<num_exp_bits, num_mantissa_bits> b)
    {
        return add(a, negate(b));
    }

    template <uint8_t num_exp_bits, uint8_t num_mantissa_bits>
    constexpr auto multiply(Float<num_exp_bits, num_mantissa_bits> a, Float<num_exp_bits, num_mantissa_bits> b)
    {
        using primitive_t = typename Float<num_exp_bits, num_mantissa_bits>::primitive_type;

        const auto a_exp_int = detail::bits_to_sint<num_exp_bits>(a.exponent);
        const auto b_exp_int = detail::bits_to_sint<num_exp_bits>(b.exponent);

        const auto a_mantissa_int = detail::bits_to_uint<num_mantissa_bits>(a.mantissa);
        const auto b_mantissa_int = detail::bits_to_uint<num_mantissa_bits>(b.mantissa);

        const auto y_exp_int = a_exp_int + b_exp_int;
        const auto norm = static_cast<uint64_t>(detail::get_mantissa_multiplier<num_mantissa_bits, primitive_t>());
        const auto y_mantissa_int = a_mantissa_int * b_mantissa_int / norm;

        Float<num_exp_bits, num_mantissa_bits> y{};
        y.sign_bit = a.sign_bit ^ b.sign_bit;
        y.exponent = detail::sint_to_bits<num_exp_bits>(y_exp_int);
        y.mantissa = detail::uint_to_bits<num_mantissa_bits>(y_mantissa_int);

        // @TODO: re-normalize

        return y;
    }

    template <uint8_t num_exp_bits, uint8_t num_mantissa_bits>
    constexpr auto reciprocal(Float<num_exp_bits, num_mantissa_bits> x)
    {
        auto y = x;
        y.exponent = detail::sint_to_bits<num_exp_bits>(-detail::bits_to_sint(x.exponent));
        return y;
    }

    template <uint8_t num_exp_bits, uint8_t num_mantissa_bits>
    constexpr auto divide(Float<num_exp_bits, num_mantissa_bits> a, Float<num_exp_bits, num_mantissa_bits> b)
    {
        return multiply(a, reciprocal(b));
    }

    namespace math_operators
    {
        template <uint8_t num_exp_bits, uint8_t num_mantissa_bits>
        constexpr auto operator-(Float<num_exp_bits, num_mantissa_bits> x)
        {
            return negate(x);
        }

        template <uint8_t num_exp_bits, uint8_t num_mantissa_bits>
        constexpr auto operator+(Float<num_exp_bits, num_mantissa_bits> a, Float<num_exp_bits, num_mantissa_bits> b)
        {
            return add(a, b);
        }

        template <uint8_t num_exp_bits, uint8_t num_mantissa_bits>
        constexpr auto operator-(Float<num_exp_bits, num_mantissa_bits> a, Float<num_exp_bits, num_mantissa_bits> b)
        {
            return subtract(a, b);
        }

        template <uint8_t num_exp_bits, uint8_t num_mantissa_bits>
        constexpr auto operator*(Float<num_exp_bits, num_mantissa_bits> a, Float<num_exp_bits, num_mantissa_bits> b)
        {
            return multiply(a, b);
        }

        template <uint8_t num_exp_bits, uint8_t num_mantissa_bits>
        constexpr auto operator/(Float<num_exp_bits, num_mantissa_bits> a, Float<num_exp_bits, num_mantissa_bits> b)
        {
            return divide(a, b);
        }
    } // namespace math_operators
} // namespace fp_scratch
