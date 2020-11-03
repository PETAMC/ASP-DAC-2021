#ifndef ELEMENTARYDELAY_HPP
#define ELEMENTARYDELAY_HPP

#include <vector>
#include <string>
#include <systemc.h>
#include <cstdlib>

const int 	t_r=8,  					//read
			t_p=8, 						//polling
			t_w=5, 						//write			
			t_rl=14, 					//wait to the next read
			t_wl=13, 					//wait to the next write
			t_pl=7,						//wait to the next polling
			t_r_loop=22, 				//t_r_loop=t_r+t_rl
			t_w_loop=18, 				//t_w_loop=t_w+t_wl
			t_p_loop=15,				//t_p_loop=t_p+t_pl
			t_pr_r=15,					//t_pre_read
			t_po_r=11,					//t_post_read
			t_pr_w=15,					//t_pre_write
			t_po_w=9,					//t_post_write
			t_init_r=15,				//t_init_read
			t_init_w=16;				//t_init_write

int DelayOffsetRead(int numTokens);
int DelayOffsetWrite(int numTokens);


#endif