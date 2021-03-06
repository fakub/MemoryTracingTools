#include <fstream>
#include "pin.H"

std::ofstream TraceFile;
PIN_LOCK lock;
ADDRINT main_begin;
ADDRINT main_end;




static VOID RecordWriteAddrSize(ADDRINT addr, INT32 size) {
	if (size == 1)
		TraceFile << (CHAR)(addr & 0xff);
	
}















VOID Instruction_cb(INS ins, VOID *v) {
	ADDRINT ip = INS_Address(ins);
	if ((ip < main_begin) || (ip > main_end))
		return;
	
	if (INS_IsMemoryWrite(ins)) {
		INS_InsertPredicatedCall(
			ins, IPOINT_BEFORE, (AFUNPTR)RecordWriteAddrSize,
			IARG_MEMORYWRITE_EA,
			IARG_MEMORYWRITE_SIZE,
			IARG_END);
	}
}







void ImageLoad_cb(IMG Img, void *v) {
	PIN_GetLock(&lock, 0);
	if(IMG_IsMainExecutable(Img)) {
		main_begin = IMG_LowAddress(Img);
		main_end = IMG_HighAddress(Img);
	}
	PIN_ReleaseLock(&lock);
}

VOID Fini(INT32 code, VOID *v) {
	TraceFile.close();
}

int main(int argc, char *argv[]) {
	PIN_InitSymbols();
	PIN_Init(argc,argv);
	TraceFile.open("write_addr");
	if(TraceFile == NULL)
		return -1;
	IMG_AddInstrumentFunction(ImageLoad_cb, 0);
	INS_AddInstrumentFunction(Instruction_cb, 0);
	PIN_AddFiniFunction(Fini, 0);
	PIN_StartProgram();
	return 0;
}
