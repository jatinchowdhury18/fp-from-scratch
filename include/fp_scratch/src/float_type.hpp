#pragma once

#include "bit_details.hpp"

namespace fp_scratch
{
    template <uint8_t num_exp_bits, uint8_t num_mantissa_bits>
    struct Float
    {
        bool sign_bit{};
        std::array<bool, num_exp_bits> exponent{};
        std::array<bool, num_mantissa_bits> mantissa{};

        static constexpr uint8_t total_bits = 1 + num_exp_bits + num_mantissa_bits;
        static_assert(total_bits <= 128);

        using primitive_type =
            std::conditional_t<total_bits <= 32, float, std::conditional_t<total_bits <= 64, double, long double>>;

        [[nodiscard]] constexpr auto to_float() const -> primitive_type
        {
            const auto exp_int = detail::bits_to_sint<num_exp_bits>(exponent);

            const auto mantissa_int = detail::bits_to_uint<num_mantissa_bits>(mantissa);
            const auto mantissa_float = static_cast<primitive_type>(mantissa_int) / detail::get_mantissa_multiplier<num_mantissa_bits, primitive_type>();

            const auto abs_val = detail::pow10<primitive_type> (exp_int) * mantissa_float;
            return sign_bit ? -abs_val : abs_val;
        }
    };

    using float32_t = Float<8, 23>;
    static_assert(sizeof(float32_t) == 32);
    static_assert(std::is_same_v<float32_t::primitive_type, float>);

    template <uint8_t num_exp_bits = 8, uint8_t num_mantissa_bits = 23, typename float_t>
        requires std::is_floating_point_v<float_t>
    constexpr auto from_float(float_t value)
    {
        const auto is_negative = value < float_t{};
        const auto abs_value = is_negative ? -value : value;
        const auto exp_value = detail::get_exponent_sint(abs_value);
        const auto mantissa_value = detail::get_mantissa_uint<num_mantissa_bits>(abs_value, exp_value);

        Float<num_exp_bits, num_mantissa_bits> result{};
        result.sign_bit = is_negative;
        result.exponent = detail::sint_to_bits<num_exp_bits>(exp_value);
        result.mantissa = detail::uint_to_bits<num_mantissa_bits>(mantissa_value);

        return result;
    }
} // namespace fp_scratch
