#include "DemoHead.h"

#if T_RandomDemo

#include "Random.h"
#include <iostream>

int main()
{
    Random random;
    random.generateFloat(1.35, 2.16);
    std::cout << "Print float random: " << std::endl;

    for(int ii = 0; ii < 10; ++ii)
    {
        std::cout << random.getFloat() << std::endl;
    }

    std::cout << std::endl;

    random.generateInt(10827, 15994);
    std::cout << "Print int random: " << std::endl;

    for(int ii = 0; ii <10; ++ii)
    {
        std::cout << random.getInt() << std::endl;
    }

    return 0;
}

#endif
