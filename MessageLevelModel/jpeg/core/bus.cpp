#include <core/bus.hpp>
#include <iostream>

namespace core
{

Bus::Bus(sc_core::sc_module_name name)
    : sc_core::sc_module(name)
    , target_socket("target_bus_socket")
    , initiator_socket("initiator_bus_socket")
{
    this->target_socket.register_b_transport(this, &Bus::b_transport);
}



// generates slave address spaces - 0x10 per slave
void Bus::end_of_elaboration()
{
    size_t slaves = this->initiator_socket.size();

    for(unsigned int i = 0; i < slaves; i++)
    {
        std::cerr << "Slave connected to Bus: " << i << ": " << this->starts[i] << " - " << this->ends[i] << std::endl;
    }

    size_t masters = this->target_socket.size();
}



void Bus::b_transport(int id, tlm::tlm_generic_payload& trans, sc_core::sc_time& delay)
{
    sc_dt::uint64 global_addr = trans.get_address();
    int slaveid               = this->AddressToSlaveID(global_addr);
    if(slaveid < 0)
    {
        trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
        std::cerr << "\e[1;31m" << this->name() << ": "  << "\e[1;31mInvalid address " << global_addr << std::endl;
        return;
    }
    sc_dt::uint64 slave_addr  = global_addr - this->starts[slaveid];

    trans.set_address(slave_addr);
    this->initiator_socket[slaveid]->b_transport(trans, delay);
    trans.set_address(global_addr);
}


int Bus::AddressToSlaveID(unsigned int addr)
{
    for(unsigned int i = 0; i < this->starts.size(); i++)
    {
        if( (addr >= this->starts[i]) && (addr <= this->ends[i]))
            return i;
    }

    return -1;
}

} // namespace core

// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

