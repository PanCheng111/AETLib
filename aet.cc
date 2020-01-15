#include "aet.h"
#include "rth_rec.h"

#include <cstring>
#include <cstdlib>
#include <unordered_map>

static rthRec *rth_rec = NULL;
static size_t block_size = 64;

std::unordered_map<size_t, size_t> access_time;

/**
 * Initialize the data structure for the monitor and
 * prepare to record the trace from the AET user.
 * Options *option: some configuration options for the AET monitor.
 */
void aetInitMonitor(Options *option) {
    rth_rec = (rthRec *)malloc(sizeof(rthRec));
    rthClear(rth_rec);
    if (option->block_size != 0) block_size = option->block_size;
}


/**
 * Access uniform data size. (like hardware cache, 64B).
 * size_t addr:     hashed key, if the key is not a number; otherwise you can directly use the addr.
 * bool isWrite:    true for write, false for read.
 */ 
void aetAccessUniform(size_t addr, bool isWrite) {
    size_t last_access_time = access_time[addr];
    if (!isWrite) last_access_time = rthGet(rth_rec, block_size, last_access_time);
    else last_access_time = rthUpdate(rth_rec, block_size, block_size, last_access_time);
    access_time[addr] = last_access_time;
}

/**
 * Access non-uniform data size. (like KV-cache, Redis, Memcached).
 * size_t addr:     hashed key, if the key is not a number; otherwise you can directly use the addr.
 * size_t oriSize:  the original data size of the key-value.
 * size_t curSize:  the current data size of the key-value, leaving it equal to 0 if it is READ operation.
 * bool isWrite:    true for write, false for read.
 */ 
void aetAccessNonUniform(size_t addr, size_t oriSize, size_t curSize, bool isWrite) {
    size_t last_access_time = access_time[addr];
    if (!isWrite) last_access_time = rthGet(rth_rec, oriSize, last_access_time);
    else last_access_time = rthUpdate(rth_rec, oriSize, curSize, last_access_time);
    access_time[addr] = last_access_time;
}

/**
 * Calculate the Miss Ratio Curve (MRC) in the monitor.
 * size_t tot_mem:      the maximum memory (in bytes) in MRC.
 * size_t granularity:  the granularity of the MRC.
 * double *mrc:         results (MRC) stored in it, must be sure there is enough space to store all the results.
 */ 
void aetCalculateMRC(size_t tot_mem, size_t granularity, double *mrc) {
    rthCalcMRC(rth_rec, tot_mem, granularity);
    memcpy(mrc, rth_rec->mrc, sizeof(double) * (tot_mem / granularity + 1));
    if (rth_rec->mrc != NULL) free(rth_rec->mrc);
}

/**
 * Calculate the Write Back Curve (WBC) in the monitor.
 * size_t tot_mem:      the maximum memory (in bytes) in WBC.
 * size_t granularity:  the granularity of the WBC.
 * double *wbc:         results (WBC) stored in it.
 */ 
void aetCalculateWBC(size_t tot_mem, size_t granularity, double *wbc) {
    (void)tot_mem;
    (void)granularity;
    (void)wbc;
}

/**
 * Clear the data in the monitor.
 */
void aetClearMonitor() {
    rthClear(rth_rec);
}

/**
 * Free the monitor entirely.
 */ 
void aetFreeMonitor() {
    rthClear(rth_rec);
    free(rth_rec);
}
