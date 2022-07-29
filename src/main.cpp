#include "interface.hpp"

#include <iostream>
#include <stdexcept>

int main()
{
    try
    {
        Interface interface;
        interface.run();

        return EXIT_SUCCESS;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception thrown: " << e.what() << '\n';
    }
    catch (...)
    {
        std::cerr << "Unknow exception thrown\n";
    }

    return EXIT_FAILURE;
}
