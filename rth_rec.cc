#include "rth_rec.h"

#include <cstdlib>
#include <cstring>

/**
 * 比较大小的函数
 */
static size_t maxer(size_t a, size_t b) {
    if (a > b) return a;
    return b;
} 

/**
 * 用来将一个长的reuse time进行压缩，以便存储
 */ 
static size_t domain_value_to_index(size_t value) {
    size_t loc = 0, step = 1;
    int index = 0;
    while (loc + step * RTH_DOMAIN < value) {
        loc += step * RTH_DOMAIN;
        step *= 2;
        index += RTH_DOMAIN;
    }
    while (loc < value) {
        index++;
        loc += step;
    }
    return index;
}

/**
 * 求一个压缩后的reuse time原来对应的大小（概数）
 */
// static size_t domain_index_to_value(size_t index) {
//     size_t value = 0, step = 1;
//     while (index > RTH_DOMAIN) {
//         value += step * RTH_DOMAIN;
//         step *= 2;
//         index -= RTH_DOMAIN;
//     }
//     while (index > 0) {
//         value += step;
//         index --;
//     }
//     return value;
// }

/**
 * 用于记录GET操作的效果，按照字节记录RTH
 * 返回值为该节点更新后的access time
 */ 
size_t rthGet(rthRec *rth, size_t size, size_t last_access_time) {
    size_t len = size;
    int isMiss = 0;
    if (last_access_time == 0) isMiss = 1;
    if (!isMiss) {
        int rt = (int)domain_value_to_index((rth->n - last_access_time));
        rth->rtd[rt] += len;
        rth->read_rtd[rt] += 1;
    }
    else {
        rth->rtd[0] += len;
        rth->read_rtd[0] += 1;
    }
    // 更新该key节点的last_acces_time
    last_access_time = rth->n;
    rth->n += len;
    return last_access_time;
}

/**
 * 用于记录UPDATE操作的效果，按照字节记录RTH
 * 返回值为该节点更新后的access time
 */
size_t rthUpdate(rthRec *rth, size_t ori_size, size_t cur_size, size_t last_access_time) {
    size_t b = ori_size, a = cur_size;
    int rt = (int)domain_value_to_index((rth->n - last_access_time));
    if (last_access_time != 0) { // 如果不是第一次写，则可以计算rt
        if (a>b) {
            rth->rtd[rt] += b;
            rth->rtd[0] += a - b;
        }
        else {
            rth->rtd[rt] += a;
            rth->rtd_del[rt] += b - a;
        }
    }
    else {
        // 第一次写操作，直接默认全部miss
        rth->rtd[0] += a;
    }
    // 更新该key节点的last_acces_time
    last_access_time = rth->n;
    rth->n += maxer(a, b);
    return last_access_time;
} 

/**
 * 计算MRC曲线的API，其中，tot_memory是总内存大小（in bytes），PGAP是访问分配粒度（in bytes），mrc是返回值
 */
void rthCalcMRC(rthRec *rth, size_t tot_memory, size_t PGAP) {
    double sum = 0, tot = 0, N = 0;
    size_t step = 1; int dom = 1,dT = 1;
    size_t T = 0;

    double read_sum = 0;
    double read_N = 0;
    for (int i = 0; i < RTH_RTD_LENGTH; i++) {
        read_N += rth->read_rtd[i];
        N += rth->rtd[i] + rth->rtd_del[i];
    //    if (rth->rtd[i] > 0 || rth->rtd_del[i] > 0 || rth->read_rtd[i] > 0)
    //        printf("rtd[%d]=%d, read_rtd[%d]=%d, rtd_del[%d]=%d\n", i, rth->rtd[i], i, rth->read_rtd[i], i, rth->rtd_del[i]);
    }
    rth->mrc = (double *)malloc((tot_memory / PGAP) * sizeof(double) + 10);
    memset(rth->mrc, 0, tot_memory / PGAP * sizeof(double) + 10);
    for (size_t i = 0; i < tot_memory / PGAP + 1; i ++)
        rth->mrc[i] = 1.0;
    if (read_N == 0) return;
    for (size_t c = PGAP; c <= tot_memory; c += PGAP) {
        while (dT < RTH_RTD_LENGTH) {
    		double d = 1.0 * (rth->rtd[dT] + rth->rtd_del[dT]);
    		double tmp = step - (sum * step + d * (1 + step) / 2) / N;
    		if (tot + tmp > c) break;
            tot += tmp;
            T += step;
    		if (++dom > RTH_DOMAIN) {
                dom = 1;
                step *= 2;
            }
    		sum += d;
            read_sum += 1.0 * rth->read_rtd[dT];
    		dT ++;
        }
    	size_t mid, be = 0, ed = step;
    	while (be < ed - 1) {
    		mid = (be + ed) >> 1;
    		double d = 1.0 * (rth->rtd[dT] + rth->rtd_del[dT]);
    		double tmp = mid - (sum * mid + d * (1 + mid) * mid / 2 / step) / N;
    		if (tot + tmp >= c) ed = mid; else be = mid;
        }
        double miss_ratio = (read_N - read_sum - 1.0 * rth->read_rtd[dT] / step * ed) / read_N;
       // if (c / PGAP == 4096) System.out.println("4mb, miss_ratio=" + miss_ratio);
        rth->mrc[(int)(c / PGAP)] = miss_ratio;
    }    
}

/**
 * 清空所有统计信息
 */
void rthClear(rthRec *rth) {
    memset(rth->rtd, 0, sizeof(rth->rtd));
    memset(rth->rtd_del, 0, sizeof(rth->rtd_del));
    memset(rth->read_rtd, 0, sizeof(rth->read_rtd));
    //memset(rth->mrc, 0, sizeof(rth->mrc));
    if (rth->mrc != NULL) free(rth->mrc);
    rth->mrc = NULL;
    rth->n = 0;
    rth->tot_penalty = 0;
}
