#include <core/slave.hpp>
#include <iostream>

namespace core
{

Memory::Memory(sc_core::sc_module_name name, size_t size, sc_core::sc_time& readdelay, sc_core::sc_time& writedelay)
    : sc_core::sc_module(name)
    , target_socket("target_socket")
    , memory(size, 0)
    , readdelay(readdelay)
    , writedelay(writedelay)
{
    std::cerr << "\e[1;30mMemory constructor called for \033[0m" << this->name() << std::endl;
    this->target_socket.bind(*this);
}



int Memory::CheckIndex(int index)
{
    if(index >= this->memory.size() or index < 0)
        return -1;

    return 0;
}



void Memory::b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& delay)
{
    sc_dt::uint64 index       = trans.get_address();
    unsigned int  data_length = trans.get_data_length();
    unsigned int  width       = trans.get_streaming_width();
    unsigned int* data        = reinterpret_cast< unsigned int* >(trans.get_data_ptr());

    // check for valid payload attributes
    if(this->CheckIndex(index) != 0)
    {
        std::cerr << "\e[1;31m" << this->name() << ": "  << "\e[1;31mInvalid address " 
                  << index << std::endl;
        trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
        return;
    }
    else if(trans.get_byte_enable_ptr() != NULL)
    {
        std::cerr << "\e[1;31m" << this->name() << ": "  << "\e[1;31mInvalid trans.get_byte_enable_ptr value!" << std::endl;
        trans.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
        return;
    }
    else if(width != 4)
    {
        std::cerr << "\e[1;31m" << this->name() << ": "  << "\e[1;31mInvalid streaming_width! Must always be 4" << std::endl;
        trans.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
        return;
    }
    else if(data_length != 4)
    {
        std::cerr << "\e[1;31m" << this->name() << ": "  << "\e[1;31mInvalid data_length! Must always be 4" << std::endl;
        trans.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
        return;
    }

    switch(trans.get_command())
    {
        case tlm::TLM_WRITE_COMMAND:
            this->memory[index] = *data;
            sc_core::wait(this->writedelay);
            break;

        case tlm::TLM_READ_COMMAND:
            *data = this->memory[index];
            sc_core::wait(this->readdelay);
            break;

        case tlm::TLM_IGNORE_COMMAND:
        default:
            std::cerr << "\e[1;31m" << this->name() << ": "  << "\e[1;31mInvalid command!" << std::endl;
            break;
    }

    trans.set_response_status(tlm::TLM_OK_RESPONSE);
}


tlm::tlm_sync_enum Memory::nb_transport_fw(tlm::tlm_generic_payload&, tlm::tlm_phase&, sc_core::sc_time&)
{
    std::cerr << "\e[0;33mnb_transport_fw called" << std::endl;
    return tlm::TLM_COMPLETED; 
}

bool Memory::get_direct_mem_ptr(tlm::tlm_generic_payload&, tlm::tlm_dmi&)
{
    std::cerr << "\e[0;33mget_direct_mem_ptr called" << std::endl;
    return false; 
}

unsigned int Memory::transport_dbg(tlm::tlm_generic_payload&)
{
    std::cerr << "\e[0;33mtransport_dbg called" << std::endl;
    return 0; 
}

} // namespace core
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

