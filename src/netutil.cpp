#include "netutil.h"

gDelta_t gDelta_t::operator=(const gDelta_t& o){
	_tick = o._tick;
	return (*this);
}