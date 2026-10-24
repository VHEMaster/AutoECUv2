#include "middlelayer_comm.h"
#include "config_comm.h"
void middlelayer_comm_init(void) { (void)ecu_comm_init(); }
void middlelayer_comm_loop_main(void) { ecu_comm_loop_main(); }
void middlelayer_comm_loop_slow(void) { }
void middlelayer_comm_loop_comm(void) { }
