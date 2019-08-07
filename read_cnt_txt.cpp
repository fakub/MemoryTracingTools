#include <stdio.h>
#include "pin.H"

FILE * TraceFile;
PIN_LOCK lock;
ADDRINT main_begin;
ADDRINT main_end;

static ADDRINT ReadAddr;
static INT32 ReadSize;

static VOID RecordReadAddrSize(ADDRINT addr, INT32 size) {
	ReadAddr = addr;
	ReadSize = size;
	
}

static VOID RecordMemRead(ADDRINT ip) {
	if (ReadSize == 1) {
		UINT8 memdump[1];   // [256]
		PIN_GetLock(&lock, ip);
		PIN_SafeCopy(memdump, (void *)ReadAddr, 1);
		fprintf(TraceFile, "W %p 0x01 0x%02x\n", (VOID *)ReadAddr, memdump[0]);
		//~ if (ReadSize <= 32) {
			//~ fprintf(TraceFile, "W %p 0x%02x 0x", (VOID *)ReadAddr, ReadSize);
			//~ for (INT32 i=0; i<ReadSize; i++) fprintf(TraceFile, "%02x", memdump[i]); fprintf(TraceFile, "\n");}
		//~ else {fprintf(stderr, "More than 32 bytes written, not recording.\n");}
		PIN_ReleaseLock(&lock);
	}
}

VOID Instruction_cb(INS ins, VOID *v) {
	ADDRINT ip = INS_Address(ins);
	if ((ip < main_begin) || (ip > main_end))
		return;
	
	if (INS_IsMemoryRead(ins)) {
		INS_InsertPredicatedCall(
			ins, IPOINT_BEFORE, (AFUNPTR)RecordReadAddrSize,
			IARG_MEMORYREAD_EA,
			IARG_MEMORYREAD_SIZE,
			IARG_END);
		if (INS_HasFallThrough(ins)) {
			INS_InsertCall(
				ins, IPOINT_AFTER, (AFUNPTR)RecordMemRead,
				IARG_INST_PTR,
				IARG_END);
		}
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
	fclose(TraceFile);
}

int main(int argc, char *argv[]) {
	PIN_InitSymbols();
	PIN_Init(argc,argv);
	TraceFile = fopen("read_cnt.txt", "w");
	if(TraceFile == NULL)
		return -1;
	IMG_AddInstrumentFunction(ImageLoad_cb, 0);
	INS_AddInstrumentFunction(Instruction_cb, 0);
	PIN_AddFiniFunction(Fini, 0);
	PIN_StartProgram();
	return 0;
}
