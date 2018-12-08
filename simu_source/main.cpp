#include <iostream>
#include <stdarg.h>
#include <cmath>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <bitset>
#include "inst.hpp"
#include "stat.cpp"
#include "dump.cpp"
#include "settings.cpp"
#include "decoder.cpp"
#include "io.cpp"
#include "reg_mem.cpp"
#include "fpu.cpp"
#include "disasm.cpp"
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
    try {
        core.main_loop();
    } 
    catch (int e) 
    {
        core.info();
        return -1;
    }
    return 0;
}
