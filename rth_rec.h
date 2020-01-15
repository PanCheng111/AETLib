#ifndef __RTH_REC_H__
#define __RTH_REC_H__

#include <cstddef>

#define RTH_DOMAIN 256
#define RTH_RTD_LENGTH  (10000+3)

typedef struct rthRec {
    int rtd[RTH_RTD_LENGTH];    // reuse time distribution, rtd[0] for cold miss
    int rtd_del[RTH_RTD_LENGTH];
    int read_rtd[RTH_RTD_LENGTH];

    double *mrc;
    size_t n;                   // 用来记录访问的字节总数
    size_t tot_penalty;         // 用来记录该类中所有访问的penalty总和
} rthRec;

size_t rthGet(rthRec *rth, size_t size, size_t last_access_time);
size_t rthUpdate(rthRec *rth, size_t ori_size, size_t cur_size, size_t last_access_time);
void rthClear(rthRec *rth);
void rthCalcMRC(rthRec *rth, size_t tot_memory, size_t PGAP);

#endif
