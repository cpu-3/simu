#include <iostream>
#include <stdarg.h>
#include <cmath>
#include <fstream>
#include "inst.hpp"
#include "dump.cpp"
#include "settings.cpp"
#include "decoder.cpp"
#include "io.cpp"
#include "reg_mem.cpp"
#include "core.cpp"

int main(int argc, const char **argv)
{
    if (argc == 1)
    {
        std::cout << "Usage: " << argv[0] << " program file" << std::endl;
        return 0;
    }
    Settings s = Settings(argc == 2 ? "" : argv[2]);
    Core core((std::string(argv[1])), &s);
    core.main_loop();
    return 0;
}
