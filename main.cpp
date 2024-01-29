#include <fp_scratch/fp_scratch.hpp>
#include <iostream>

int main()
{
    using namespace fp_scratch;
    using namespace fp_scratch::math_operators;

    std::cout << detail::bits_to_uint<2>({true, false}) << std::endl;

    std::cout << float32_t{}.to_float() << std::endl;

    std::cout << from_float(0.0134567f).to_float() << std::endl;
    std::cout << from_float(2.0f).to_float() << std::endl;
    std::cout << from_float(-2.0f).to_float() << std::endl;
    std::cout << from_float(99.0f).to_float() << std::endl;

    std::cout << (-from_float(100.0)).to_float() << std::endl;
    std::cout << (from_float(99.0) + from_float(20.0)).to_float() << std::endl;
    std::cout << (from_float(100.0) + from_float(-20.0)).to_float() << std::endl;
    std::cout << (from_float(100.0) - from_float(-20.0)).to_float() << std::endl;
    std::cout << (from_float(100.0) * from_float(0.5)).to_float() << std::endl;
    std::cout << (from_float(100.0) / from_float(10.0)).to_float() << std::endl;

    return 0;
}
