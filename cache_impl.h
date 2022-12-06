/*
 * cache_impl.h
 *
 * 20493-01 Computer Architecture
 * Term Project on Implementation of Cache Mechanism
 *
 * Skeleton Code Prepared by Prof. HyungJune Lee
 * Nov 16, 2022
 *
 */

/* DO NOT CHANGE THE FOLLOWING DEFINITIONS EXCEPT 'DEFAULT_CACHE_ASSOC */

#ifndef _CACHE_IMPL_H_
#define _CACHE_IMPL_H_

#define WORD_SIZE_BYTE                   4          //1 word = 4 bytes
#define DEFAULT_CACHE_SIZE_BYTE          32         //cache size = 32 bytes
#define DEFAULT_CACHE_BLOCK_SIZE_BYTE    8          //block size = 8 bytes
#define DEFAULT_CACHE_ASSOC              4          /* This can be changed to 1(for direct mapped cache) or 4(for fully assoc cache) */
#define DEFAULT_MEMORY_SIZE_WORD         128        //main memory size is 128 words = 128 * 4 bytes
#define CACHE_ACCESS_CYCLE               1          //cache access event takes one clock cycle
#define MEMORY_ACCESS_CYCLE              100        //cache miss event takes a hundred clock cycles
#define CACHE_SET_SIZE                   ((DEFAULT_CACHE_SIZE_BYTE)/(DEFAULT_CACHE_BLOCK_SIZE_BYTE*DEFAULT_CACHE_ASSOC))    //cache set size is obtained by the formula given above

/* Function Prototypes */
void init_memory_content();                         //Function to initialize memory value
void init_cache_content();                          //Function to initialize cache content
void print_cache_entries();                         //Function to print all cache entries for debugging
int check_cache_data_hit(void* addr, char type);    //Function that checks whether a cache hit event occurs
int access_memory(void *addr, char type);           //Functions that access data


/* Cache Entry Structure */
typedef struct cache_entry {                        //a structure to check the cache more systematically
    int valid;                                      //variable for checking a valid bit
    int tag;                                        //variable for checking a tag of the cache
    int timestamp;                                  //variable for most recent access time
    char data[DEFAULT_CACHE_BLOCK_SIZE_BYTE];       //data from memory
} cache_entry_t;                                    //define type as cache_entry_t


#endif
