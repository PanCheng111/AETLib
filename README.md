# AET Lib

AET Lib is a C++ library for the Cache Model [AET](https://dl.acm.org/doi/10.5555/3026959.3026992) and its extension [Extended-AET,EAET](https://dl.acm.org/doi/10.1145/3265723.3265736), which uses `Average Eviction Time` as a measurement of data item movement in LRU cache, both hardware cache and in-memory key-value cache.

## How to install it
It is very easy to compile and install the AET Lib in different platforms (Linux, MacOS, SunOS). 

    git clone git@github.com:PanCheng111/AETLib.git
    cd AETLib
    make && make install

Then you can use AET model in your C++ code directly.

## How to use it
The are several steps in using AET to generate Miss Ratio Curve (MRC) and Write Back Curve (WBC). We will illustrate some APIs in following parts.

### specific models in AET Lib
We firstly define three models for different scenarios:

    #define MODEL0 0    // direct access, ignoring READ / WRITE, uniform data size
    #define MODEL1 1    // access concerning READ / WRITE, uniform data size
    #define MODEL2 2    // access concerning READ / WRITE / UPDATE / DELETE, non-uniform data size

And also we use an option structure to specify the configuration:

    typedef struct Options {
        size_t block_size;
        int model;
    } Options;

Here, when we set `option.model` to `MODEL0` and `MODEL1`, we need to set `block_size` to a fixed number (in bytes), e.g cache line size, 64, to represent the uniform access data size. In MODEL2, we can simply set `block_size = 0` since it won't used in the following part.

### init monitor
From the very beginning, we need to initialize the data structure for the monitor and prepare to record the trace from the AET user. `Options *option` induces configuration options for the AET monitor.

    void aetInitMonitor(Options *option);

### feed the trace into monitor
We have 

### generate Miss Ratio Curve (MRC) from monitor

### generate Write Back Curve (WBC) from monitor

### clear up & free the monitor
