#ifndef __INCSDMUONHWh
#define __INCSDMUONHWh

#ifndef MUON_INT_LEVEL
#define MUON_INT_LEVEL 2
#endif
#ifndef MUON_INT_VEC
#define MUON_INT_VEC    65
#endif

void 	MUONIntHandler(int nArg);
int   		MUONConfIntCBLT();
int 		MUONRomoConfigAll();
void 	MUONStartTrig();
void 	MUONStopTrig();
void 	MUONStop();

int TRG_MUDT_syn3();

/*below for update fpga logic*/

#endif
