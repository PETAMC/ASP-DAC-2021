#ifndef AXI_INTERCONNECT_HPP
#define AXI_INTERCONNECT_HPP

#include <hardware/interconnect.hpp>

class AXI
    : public Interconnect
{
    public:
        AXI(const char* name)
            : Interconnect(name) {};

};


#endif
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

