#pragma once

#include <array>

namespace fp_scratch::detail
{
    template <size_t N>
    constexpr auto bits_to_uint(const std::array<bool, N> &bits)
    {
        uint64_t result{};
        for (size_t i = 0; i < N; ++i)
        {
            if (bits[i])
                result += 1 << (N - 1 - i);
        }
        return result;
    }
    static_assert(bits_to_uint<2>({false, false}) == 0);
    static_assert(bits_to_uint<2>({false, true}) == 1);
    static_assert(bits_to_uint<2>({true, false}) == 2);
    static_assert(bits_to_uint<2>({true, true}) == 3);

    template <size_t N>
        requires(N > 1)
    constexpr auto bits_to_sint(const std::array<bool, N> &bits)
    {
        std::array<bool, N - 1> unsigned_bits{};
        std::copy(bits.begin() + 1, bits.end(), unsigned_bits.begin());
        const auto unsigned_value = static_cast<int64_t>(bits_to_uint(unsigned_bits));
        return bits[0] ? -(1 << (N - 1)) + unsigned_value : unsigned_value; // 2's complement (I think?)
    }
    static_assert(bits_to_sint<2>({false, false}) == 0);
    static_assert(bits_to_sint<2>({false, true}) == 1);
    static_assert(bits_to_sint<2>({true, false}) == -2);
    static_assert(bits_to_sint<2>({true, true}) == -1);
    static_assert(bits_to_sint<3>({false, false, true}) == 1);
    static_assert(bits_to_sint<3>({true, true, true}) == -1);
    static_assert(bits_to_sint<3>({true, true, false}) == -2);
    static_assert(bits_to_sint<3>({true, false, true}) == -3);
    static_assert(bits_to_sint<3>({true, false, false}) == -4);

    template <size_t N, typename uint_t>
        requires std::is_integral_v<uint_t> && std::is_unsigned_v<uint_t>
    constexpr auto uint_to_bits(uint_t int_value)
    {
        std::array<bool, N> result{};

        for (size_t i = 0; i < N; ++i)
            result[i] = (int_value >> (N - 1 - i)) & 1ul;

        return result;

        // @TODO: what if the int doesn't fit into the bits?
    }
    static_assert(uint_to_bits<2>(0ul) == std::array{false, false});
    static_assert(uint_to_bits<2>(1ul) == std::array{false, true});
    static_assert(uint_to_bits<2>(2ul) == std::array{true, false});
    static_assert(uint_to_bits<2>(3ul) == std::array{true, true});

    template <size_t N, typename int_t>
        requires std::is_integral_v<int_t> && std::is_signed_v<int_t> && (N > 1)
    constexpr auto sint_to_bits(int_t int_value)
    {
        const auto is_negative = int_value < 0;
        const auto unsigned_value = static_cast<uint64_t>(is_negative ? (1 << (N - 1)) + int_value : int_value);
        const auto unsigned_bits = uint_to_bits<N - 1>(unsigned_value);

        std::array<bool, N> result{};
        result[0] = is_negative;
        std::copy(unsigned_bits.begin(), unsigned_bits.end(), result.begin() + 1);

        return result;
    }
    static_assert(sint_to_bits<2>(0) == std::array{false, false});
    static_assert(sint_to_bits<2>(1) == std::array{false, true});
    static_assert(sint_to_bits<2>(-2) == std::array{true, false});
    static_assert(sint_to_bits<2>(-1) == std::array{true, true});
    static_assert(sint_to_bits<3>(1) == std::array{false, false, true});
    static_assert(sint_to_bits<3>(-1) == std::array{true, true, true});
    static_assert(sint_to_bits<3>(-2) == std::array{true, true, false});
    static_assert(sint_to_bits<3>(-3) == std::array{true, false, true});
    static_assert(sint_to_bits<3>(-4) == std::array{true, false, false});

    template <typename float_t>
        requires std::is_floating_point_v<float_t>
    constexpr auto get_exponent_sint(float_t abs_value)
    {
        int64_t result{};
        while (abs_value >= float_t{10})
        {
            result += 1;
            abs_value /= float_t{10};
        }

        while (abs_value < float_t{1})
        {
            result -= 1;
            abs_value *= float_t{10};
        }

        return result;
    }
    static_assert(get_exponent_sint(0.01) == -2);
    static_assert(get_exponent_sint(0.05) == -2);
    static_assert(get_exponent_sint(0.1) == -1);
    static_assert(get_exponent_sint(1.0) == 0);
    static_assert(get_exponent_sint(10.0) == 1);
    static_assert(get_exponent_sint(50.0) == 1);
    static_assert(get_exponent_sint(100.0) == 2);
    static_assert(get_exponent_sint(999.0) == 2);

    template <typename float_t, typename int_t>
        requires std::is_integral_v<int_t>
    constexpr auto pow10(int_t exp)
    {
        const auto abs_exp = exp < int_t{0} ? -exp : exp;
        float_t result{1};
        for (int i = 0; i < abs_exp; ++i)
            result *= float_t{10};

        if constexpr (std::is_signed_v<int_t>)
        {
            if (exp < 0)
                return float_t{1} / result;
        }

        return result;
    }

    template <uint8_t num_mantissa_bits, typename float_t>
        requires std::is_floating_point_v<float_t>
    constexpr auto get_mantissa_multiplier()
    {
        return pow10<float_t>(static_cast<uint64_t>(std::floor(std::log10(float_t{1 << num_mantissa_bits}))) - 1);
    }

    template <uint8_t num_mantissa_bits, typename float_t, typename sint_t>
        requires std::is_floating_point_v<float_t> && std::is_integral_v<sint_t> && std::is_signed_v<sint_t>
    constexpr auto get_mantissa_uint(float_t abs_value, sint_t exponent)
    {
        const auto float_value = abs_value * pow10<float_t>(-exponent);
        const auto uint_value =
            static_cast<uint64_t>(float_value * get_mantissa_multiplier<num_mantissa_bits, float_t>());
        return uint_value;
    }
} // namespace fp_scratch::detail
