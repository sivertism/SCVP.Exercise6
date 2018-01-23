#ifndef DRAM_H
#define DRAM_H

#include <systemc.h>
#include <iostream>
#include <tlm.h>

// C - Capacity in bytes
template<int unsigned C=1024>
class memory : sc_module, tlm::tlm_fw_transport_if<>{
private:
    unsigned char mem[C];

public:
    tlm::tlm_target_socket<> tSocket;

    SC_CTOR(memory) : tSocket("tSocket"), mem({0}){
        tSocket.bind(*this);
    }

    void b_transport(tlm::tlm_generic_payload &trans, sc_time &delay){
        if (trans.get_address() >= C){
            trans.set_response_status( tlm::TLM_ADDRESS_ERROR_RESPONSE );
            return;
        }

        if (trans.get_data_length() != 1){
            trans.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
            return;
        }

        if(trans.get_command() == tlm::TLM_WRITE_COMMAND){
            memcpy(&mem[trans.get_address()],  // destination
                    trans.get_data_ptr(),       // source
                    trans.get_data_length());   // size
        } else if (trans.get_command() == tlm::TLM_READ_COMMAND){
            memcpy(trans.get_data_ptr(),
                   &mem[trans.get_address()],
                   trans.get_data_length());
        } else {
            SC_REPORT_FATAL(this->name(), "Payload command not supported.");
        }

        delay += sc_time(20, SC_NS);
        trans.set_response_status(tlm::TLM_OK_RESPONSE);
    }

    //dummy method
    virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& trans,
                                               tlm::tlm_phase& phase,
                                               sc_time& delay) {
        SC_REPORT_FATAL(this->name(), "not implemented");
        return tlm::TLM_ACCEPTED;
    }

    //dummy method
    virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload &trans,
                                    tlm::tlm_dmi &data){
        SC_REPORT_ERROR(this->name(), "not implemented");
        return false;
    }

    //dummy method
    unsigned int transport_dbg(tlm::tlm_generic_payload &trans){
        SC_REPORT_FATAL(this->name(), "not implemented");
        return 0;
    }
};

#endif // DRAM_H
