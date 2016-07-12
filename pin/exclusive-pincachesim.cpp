#include <iostream>
#include <memory>

#include "../ExclusiveHierarchy.h"
#include "../CacheToolbox.h" //CacheConfig, addr_t
#include "pin.H"
#include "pinplay.H"

PINPLAY_ENGINE pinplay_engine;
KNOB<BOOL> KnobPinPlayLogger(KNOB_MODE_WRITEONCE,
                      "pintool", "log", "0",
                      "Activate the pinplay logger");
KNOB<BOOL> KnobPinPlayReplayer(KNOB_MODE_WRITEONCE,
                      "pintool", "replay", "0",
                      "Activate the pinplay replayer");

KNOB<unsigned> KnobL1Size(KNOB_MODE_WRITEONCE, "pintool", "L1-size", "", "L1 size");
KNOB<unsigned> KnobL2Size(KNOB_MODE_WRITEONCE, "pintool", "L2-size", "", "L2 size");
KNOB<unsigned> KnobAssoc(KNOB_MODE_WRITEONCE, "pintool", "a", "64", "Associativity");

using namespace std;

unique_ptr<SimpleExclusiveHierarchy> cache;

uint64_t accesses = 0;
uint64_t L1_hits = 0;
uint64_t L1_misses = 0;
uint64_t L2_hits = 0;
uint64_t L2_misses = 0;

void mem_access(void* address)
{
    ++accesses;
    const ExclusiveCache::ACCESS_STATUS access_status = cache->access((addr_t) address);

    switch (access_status)
    {
        case ExclusiveCache::ACCESS_STATUS::L1_HIT:
            ++L1_hits;
            break;
        case ExclusiveCache::ACCESS_STATUS::L1_MISS_L2_HIT:
            ++L1_misses;
            ++L2_hits;
            break;
        case ExclusiveCache::ACCESS_STATUS::L1_MISS_L2_MISS:
            ++L1_misses;
            ++L2_misses;
            break;
    }
}

void fini(int code, void* v)
{
    cout << "===L1===" << endl;
    cout << "Accesses: " << accesses << endl;
    cout << "Hits: " << L1_hits << endl;
    cout << "Misses: " << L1_misses << " ( " << L1_misses / (float) accesses * 100 << "% of all accesses)" << endl << endl;

    cout << "===L2===" << endl;
    cout << "Accesses: " << L1_misses << endl;
    cout << "Hits: " << L2_hits << endl;
    cout << "Misses: " << L2_misses << " ( " << L2_misses / (float) L1_misses * 100 << "% of all L2 accesses)" << endl;
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

    const unsigned L1_size = KnobL1Size.Value();
    const unsigned L2_size = KnobL2Size.Value();

    if (L1_size == 0 || L2_size == 0)
        throw runtime_error("L1 and L2 sizes must be specified, and greater than 0");

    cache.reset(new SimpleExclusiveHierarchy(CacheConfig(L1_size, KnobAssoc.Value(), 64), CacheConfig(L2_size, KnobAssoc.Value(), 64)));

    PIN_StartProgram();
}
