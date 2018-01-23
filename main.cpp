#include <systemc>
#include <iostream>
#include <iomanip>
#include "memory.h"
#include "processor.h"
#include "bus.h"


int sc_main (int __attribute__((unused)) sc_argc,
             char __attribute__((unused)) *sc_argv[])
{
    processor cpu0("cpu0","/home/sivert/git/SCVP.Exercise6/stimuli1.txt", sc_time(1,SC_NS));
    processor cpu1("cpu1","/home/sivert/git/SCVP.Exercise6/stimuli2.txt", sc_time(1,SC_NS));
    memory<512> memory0("memory0");
    memory<512> memory1("memory1");

    // Bind both processors and memories to bus
    Bus bus("bus");
    cpu0.iSocket.bind(bus.tSocket[0]);
    cpu1.iSocket.bind(bus.tSocket[1]);
    bus.iSocket[0].bind(memory0.tSocket);
    bus.iSocket[1].bind(memory1.tSocket);

    std::cout << std::endl << "Name "
              << std::setfill(' ') << std::setw(10)
              << "Time" << " "
              << std::setfill(' ') << std::setw(5)
              << "CMD" << "   "
              << std::setfill(' ') << std::setw(8)
              << "Address"
              << "   " << std::hex
              << std::setfill(' ') << std::setw(8)
              << "Data"
              << " " << std::endl
              << "-------------------------------------------"
              << std::endl;

    sc_start();

    std::cout << std::endl;
    return 0;
}
