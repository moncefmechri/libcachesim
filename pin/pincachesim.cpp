#include <iostream>

#include "../LRUCache.h"
#include "../CacheToolbox.h" //addr_t
#include "pin.H"
#include "pinplay.H"

PINPLAY_ENGINE pinplay_engine;
KNOB<BOOL> KnobPinPlayLogger(KNOB_MODE_WRITEONCE,
                      "pintool", "log", "0",
                      "Activate the pinplay logger");
KNOB<BOOL> KnobPinPlayReplayer(KNOB_MODE_WRITEONCE,
                      "pintool", "replay", "0",
                      "Activate the pinplay replayer");

using namespace std;

LRUCache cache(32768, 64, 64);
uint64_t accesses = 0;
uint64_t hits = 0;
uint64_t misses = 0;

void mem_access(void* address)
{
    ++accesses;
    const bool cache_hit = cache.access((addr_t) address);

    if (cache_hit)
        ++hits;
    else
        ++misses;
}

void fini(int code, void* v)
{
    cout << "Accesses: " << accesses << endl;
    cout << "Hits: " << hits << endl;
    cout << "Misses: " << misses << " ( " << misses / (float) accesses * 100 << "% of all accesses)" << endl;
}

void instruction(INS ins, void* v)
{
    const int nb_mem_operands = INS_MemoryOperandCount(ins);
    
    for (int mem_op = 0; mem_op < nb_mem_operands; ++mem_op)
    {
        if (INS_MemoryOperandIsRead(ins, mem_op))
        {
            INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR) mem_access, IARG_MEMORYOP_EA, mem_op, IARG_END);
        }
        
        //An instruction could both read and write memory so we always test for both
        if (INS_MemoryOperandIsWritten(ins, mem_op))
        {
            INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR) mem_access, IARG_MEMORYOP_EA, mem_op, IARG_END);
        }
    }
}

int main(int argc, char** argv)
{
    PIN_Init(argc, argv);
    pinplay_engine.Activate(argc, argv,
          KnobPinPlayLogger, KnobPinPlayReplayer);

    INS_AddInstrumentFunction(instruction, 0);
    PIN_AddFiniFunction(fini, 0);

    PIN_StartProgram();
}
