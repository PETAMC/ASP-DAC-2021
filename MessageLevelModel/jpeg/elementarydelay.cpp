#include <elementarydelay.hpp>
#include <cstdlib>
#include <iostream>

int DelayOffsetRead(int numTokens)
{
	return (t_init_r+t_p+t_pr_r+t_r*numTokens+t_rl*(numTokens-1)+t_po_r+t_w);
}

int DelayOffsetWrite(int numTokens)
{
	return (t_init_w+t_pr_w+t_p+t_w*numTokens+t_wl*(numTokens-1)+t_po_w+t_w);
}
