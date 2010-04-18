#include "netutil.h"

gsDelta_t gsDelta_t::operator=(const gsDelta_t& o){
	_tick = o._tick;
	return (*this);
}