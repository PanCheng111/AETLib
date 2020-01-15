# AET Lib

AET Lib is a C++ library for the Cache Model [AET](https://dl.acm.org/doi/10.5555/3026959.3026992) and its extension [Extended-AET,EAET](https://dl.acm.org/doi/10.1145/3265723.3265736), which uses `Average Eviction Time` as a measurement of data item movement in LRU cache, both hardware cache and in-memory key-value cache.

## How to install it
It is very easy to compile and install the AET Lib in different platforms (Linux, MacOS, SunOS). 

```bash
git clone git@github.com:PanCheng111/AETLib.git
cd AETLib
make && make install
```

Then you can use AET model in your C++ code directly.

## How to use it
The are several steps in using AET to generate Miss Ratio Curve (MRC) and Write Back Curve (WBC) after the `#include "aet.h"`. We will illustrate some APIs in following parts.

### specific models in AET Lib
We firstly define three models for different scenarios:

```cpp
#define MODEL0 0    // direct access, ignoring READ / WRITE, uniform data size
#define MODEL1 1    // access concerning READ / WRITE, uniform data size
#define MODEL2 2    // access concerning READ / WRITE / UPDATE / DELETE, non-uniform data size
```

And also we use an option structure to specify the configuration:

```cpp
typedef struct Options {
    size_t block_size;
    int model;
} Options;
```

Here, when we set `option.model` to `MODEL0` and `MODEL1`, we need to set `block_size` to a fixed number (in bytes), e.g cache line size, 64, to represent the uniform access data size. In MODEL2, we can simply set `block_size = 0` since it won't used in the following part.

### init monitor
From the very beginning, we need to initialize the data structure for the monitor and prepare to record the trace from the AET user. `Options *option` induces configuration options for the AET monitor.
```cpp
void aetInitMonitor(Options *option);
```

### feed the trace into monitor
We have two different APIs to feed the trace into monitor:
* Uniform Data Access
* Non-uniform Data Access

```cpp
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
```

### generate Miss Ratio Curve (MRC) from monitor

```cpp
/**
 * Calculate the Miss Ratio Curve (MRC) in the monitor.
 * size_t tot_mem:      the maximum memory (in bytes) in MRC.
 * size_t granularity:  the granularity of the MRC.
 * double *mrc:         results (MRC) stored in it.
 */ 
void aetCalculateMRC(size_t tot_mem, size_t granularity, double *mrc);
```

### generate Write Back Curve (WBC) from monitor

```cpp
/**
 * Calculate the Write Back Curve (WBC) in the monitor.
 * size_t tot_mem:      the maximum memory (in bytes) in WBC.
 * size_t granularity:  the granularity of the WBC.
 * double *wbc:         results (WBC) stored in it.
 */ 
void aetCalculateWBC(size_t tot_mem, size_t granularity, double *wbc);
```

### clear up & free the monitor
We can clear up the data in the monitor using this API, which means we can start a fresh new monitoring:
```cpp
/**
 * Clear the data in the monitor.
 */
void aetClearMonitor();
```

And also, in the last, remember to free the data structures in the AET monitor, in order to avoid memory leaking.
```cpp
/**
 * Free the monitor entirely.
 */ 
void aetFreeMonitor();
```
