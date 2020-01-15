#ifndef __AET_H__
#define __AET_H__

#include <cstddef>

#define MODEL0 0    // direct access, ignoring READ / WRITE, uniform data size
#define MODEL1 1    // access concerning READ / WRITE, uniform data size
#define MODEL2 2    // access concerning READ / WRITE / UPDATE / DELETE, non-uniform data size

typedef struct Options {
    size_t block_size;
    int model;
} Options;

/**
 * Initialize the data structure for the monitor and
 * prepare to record the trace from the AET user.
 * Options *option: some configuration options for the AET monitor.
 */
void aetInitMonitor(Options *option);

/**
 * Access uniform data size. (like hardware cache, 64B).
 * size_t addr:     hashed key, if the key is not a number; otherwise you can directly use the addr.
 * bool isWrite:    true for write, false for read.
 */ 
void aetAccessUniform(size_t addr, bool isWrite);

/**
 * Access non-uniform data size. (like KV-cache, Redis, Memcached).
 * size_t addr:     hashed key, if the key is not a number; otherwise you can directly use the addr.
 * size_t oriSize:  the original data size of the key-value.
 * size_t curSize:  the current data size of the key-value. 
 * bool isWrite:    true for write, false for read.
 */ 
void aetAccessNonUniform(size_t addr, size_t oriSize, size_t curSize, bool isWrite);

/**
 * Calculate the Miss Ratio Curve (MRC) in the monitor.
 * size_t tot_mem:      the maximum memory (in bytes) in MRC.
 * size_t granularity:  the granularity of the MRC.
 * double *mrc:         results (MRC) stored in it.
 */ 
void aetCalculateMRC(size_t tot_mem, size_t granularity, double *mrc);

/**
 * Calculate the Write Back Curve (WBC) in the monitor.
 * size_t tot_mem:      the maximum memory (in bytes) in WBC.
 * size_t granularity:  the granularity of the WBC.
 * double *wbc:         results (WBC) stored in it.
 */ 
//void aetCalculateWBC(size_t tot_mem, size_t granularity, double *wbc);

/**
 * Clear the data in the monitor.
 */
void aetClearMonitor();

/**
 * Free the monitor entirely.
 */ 
void aetFreeMonitor();


#endif
