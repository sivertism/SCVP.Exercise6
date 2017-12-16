#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <iostream>
#include <regex>
#include <string>
#include <iomanip>

#include <systemc.h>
#include <tlm.h>


class processor : public sc_module, tlm::tlm_bw_transport_if<>
{
  private:
    std::ifstream file;
    sc_time cycleTime;

    // Method:
    void process();

  public:

    tlm::tlm_initiator_socket<> iSocket;

    processor(sc_module_name,
              std::string pathToTrace,
              sc_time cycleTime);

    SC_HAS_PROCESS(processor);

    // Dummy method:
    void invalidate_direct_mem_ptr(sc_dt::uint64 start_range,
                                   sc_dt::uint64 end_range)
    {
        SC_REPORT_FATAL(this->name(),"invalidate_direct_mem_ptr not implement");
    }

    // Dummy method:
    tlm::tlm_sync_enum nb_transport_bw(
            tlm::tlm_generic_payload& trans,
            tlm::tlm_phase& phase,
            sc_time& delay)
    {
        SC_REPORT_FATAL(this->name(),"nb_transport_bw is not implemented");
        return tlm::TLM_ACCEPTED;
    }


};
processor::processor(sc_module_name,
                     std::string pathToFile,
                     sc_time cycleTime) :
    file(pathToFile), cycleTime(cycleTime)
{
    if (!file.is_open())
        SC_REPORT_FATAL(name(), "Could not open trace");

    SC_THREAD(process);

    iSocket.bind(*this);
}

void processor::process()
{
    wait(SC_ZERO_TIME);
    std::string line;
    tlm::tlm_generic_payload trans;
    unsigned long long cycles = 0;
    unsigned long long address = 0;
    std::string dataString;
    unsigned char data[4];
    bool read = true;

    while(std::getline(file,line))
    {

        std::regex reW("(\\d+)\\s*:\\s*write\\s+0x([\\d\\w]+)\\s+0x([\\d\\w]+)");
        std::regex reR("(\\d+)\\s*:\\s*read\\s+0x([\\d\\w]+)");
        std::smatch matchW;
        std::smatch matchR;

        if (std::regex_search(line, matchW, reW) && matchW.size() > 1)
        {
            cycles = std::stoull(matchW.str(1), nullptr, 10);
            read = false;
            address = std::stoull(matchW.str(2), nullptr, 16);
            dataString = matchW.str(3);
            for(int i = 0; i < 4; i++)
            {
                data[i] = (unsigned char) std::stoi(dataString.substr(i*2,2),
                                                    nullptr,
                                                    16);
            }
        }
        else if (std::regex_search(line, matchR, reR) && matchR.size() > 1)
        {
            cycles = std::stoull(matchR.str(1), nullptr, 10);
            read = true;
            address = std::stoull(matchR.str(2), nullptr, 16);
        }
        else
        {
            SC_REPORT_FATAL(name(), "Syntax error in trace");
        }


        sc_time delay;

        if(sc_time_stamp() <= cycles * cycleTime)
        {
            delay = cycles * cycleTime - sc_time_stamp();
        }
        else
        {
            delay = sc_time(0, SC_NS);
        }

        trans.set_address(address);
        trans.set_data_length(4);
        trans.set_command(read?tlm::TLM_READ_COMMAND:tlm::TLM_WRITE_COMMAND);
        trans.set_data_ptr(data);
        iSocket->b_transport(trans, delay);

        wait(delay);

        std::cout << std::setfill(' ') << std::setw(4)
                  << name() << " "
                  << std::setfill(' ') << std::setw(10)
                  << sc_time_stamp() << " "
                  << std::setfill(' ') << std::setw(5)
                  << (read ? "read" : "write") << " 0x"
                  << std::setfill('0') << std::setw(8)
                  << address
                  << " 0x" << std::hex
                  << std::setfill('0') << std::setw(2)
                  << (unsigned int)data[0]
                  << std::setfill('0') << std::setw(2)
                  << (unsigned int)data[1]
                  << std::setfill('0') << std::setw(2)
                  << (unsigned int)data[2]
                  << std::setfill('0') << std::setw(2)
                  << (unsigned int)data[3]
                  << std::endl;
    }

    // End Simulation because there are no events.
}

#endif // UTILS_H
