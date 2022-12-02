/*
 * cache.c
 *
 * 20493-01 Computer Architecture
 * Term Project on Implementation of Cache Mechanism
 *
 * Skeleton Code Prepared by Prof. HyungJune Lee
 * Nov 16, 2022
 *
 */


#include <stdio.h>
#include <string.h>
#include "cache_impl.h"

extern int num_cache_hits;
extern int num_cache_misses;

extern int num_bytes;
extern int num_access_cycles;

extern int global_timestamp;

cache_entry_t cache_array[CACHE_SET_SIZE][DEFAULT_CACHE_ASSOC];
int memory_array[DEFAULT_MEMORY_SIZE_WORD];


/* DO NOT CHANGE THE FOLLOWING FUNCTION */
void init_memory_content() {
    unsigned char sample_upward[16] = {0x001, 0x012, 0x023, 0x034, 0x045, 0x056, 0x067, 0x078, 0x089, 0x09a, 0x0ab, 0x0bc, 0x0cd, 0x0de, 0x0ef};
    unsigned char sample_downward[16] = {0x0fe, 0x0ed, 0x0dc, 0x0cb, 0x0ba, 0x0a9, 0x098, 0x087, 0x076, 0x065, 0x054, 0x043, 0x032, 0x021, 0x010};
    int index, i=0, j=1, gap = 1;
    
    for (index=0; index < DEFAULT_MEMORY_SIZE_WORD; index++) {
        memory_array[index] = (sample_upward[i] << 24) | (sample_upward[j] << 16) | (sample_downward[i] << 8) | (sample_downward[j]);
        if (++i >= 16)
            i = 0;
        if (++j >= 16)
            j = 0;
        
        if (i == 0 && j == i+gap)
            j = i + (++gap);
            
        //printf("mem[%d] = %#x\n", index, memory_array[index]);
    }
}   

/* DO NOT CHANGE THE FOLLOWING FUNCTION */
void init_cache_content() {
    int i, j;
    
    for (i=0; i<CACHE_SET_SIZE; i++) {
        for (j=0; j < DEFAULT_CACHE_ASSOC; j++) {
            cache_entry_t *pEntry = &cache_array[i][j];
            pEntry->valid = 0;
            pEntry->tag = -1;
            pEntry->timestamp = 0;
        }
    }
}

/* DO NOT CHANGE THE FOLLOWING FUNCTION */
/* This function is a utility function to print all the cache entries. It will be useful for your debugging */
void print_cache_entries() {
    int i, j, k;
    
    for (i=0; i<CACHE_SET_SIZE; i++) {
        printf("[Set %d] ", i);
        printf("\n");//
        for (j=0; j <DEFAULT_CACHE_ASSOC; j++) {
            cache_entry_t *pEntry = &cache_array[i][j];
            printf("V: %d Tag: %#x Time: %d Data: ", pEntry->valid, pEntry->tag, pEntry->timestamp);
            for (k=0; k<DEFAULT_CACHE_BLOCK_SIZE_BYTE; k++) {
                printf("%#x(%d) ", pEntry->data[k], k);
            }
            //printf("\t");
            printf("\n"); //
        }
        printf("\n");
    }
}

int check_cache_data_hit(void *addr, char type) {//hit이면 그 값을, miss면 -1리턴

    /* Fill out here */
    //set계산하고
    //tag 계산해서 같은지 확인, 같으면 hitㅇㅇ, 리턴값 설정
    //다르면 리턴값 ??


    char buf[100];
    sprintf(buf, "%s", (char*)addr);

    
    int address=0;
    for(int i=0;i<10;i++){
        if(buf[i]=='\0')break;
        address=address*10+buf[i]-'0';
    }



    int val=-1; 

    int set=(int)(address/DEFAULT_CACHE_BLOCK_SIZE_BYTE)%(CACHE_SET_SIZE) ;
    int tag=(int)(address/DEFAULT_CACHE_BLOCK_SIZE_BYTE)/(CACHE_SET_SIZE) ;


    for(int i=0;i<DEFAULT_CACHE_ASSOC;i++){
        if(cache_array[set][i].tag == tag){ //HIT!!
            // val=0; int size=0;
            // if(type=='b')size=1;
            // else if (type=='h')size=2;
            // else{ size=4; }
            // int startbyte=address%8;    //8도 뭘로 바꿔야됨

            // for(int j=startbyte+size-1;j>=startbyte;j--){
            //     unsigned int v=cache_array[set][i].data[j];
            //     val=val*256+v+256;
            // }
            int size;
            if(type=='b')size=1;
            else if (type=='h')size=2;
            else{ size=4; }
            int startbyte=address%8;    //8도 뭘로 바꿔야됨
            unsigned int val=0;
            for(int j=startbyte+size-1;j>=startbyte;j--){
                int v=cache_array[set][i].data[j];
                if(v>=0)val=val*256+v;
                else{
                    char outputData[10];
                    sprintf(outputData, "%x", cache_array[set][i].data[j]);
                    if(outputData[6]>='a'&&outputData[6]<='f')v=outputData[6]-'a'+10;
                    else{ v=outputData[6]-'0'; }
                    v*=16;
                    if(outputData[7]>='a'&&outputData[7]<='f')v=v+outputData[7]-'a'+10;
                    else{ v=v+outputData[7]-'0'; }
                    val=val*256+v;
                }
            }
            cache_array[set][i].timestamp=global_timestamp++;
            break;
        }
    }
    return val;



    /* Return the data */
    //return 0;    
}

int find_entry_index_in_set(int cache_index) {
    int entry_index=-1;

    /* Check if there exists any empty cache space by checking 'valid' */
    for(int i=0;i<DEFAULT_CACHE_ASSOC;i++){
        if(cache_array[cache_index][i].valid==0){
            entry_index=i; break;
        }
    }
    if(entry_index==-1){
            for(int i=0;i<DEFAULT_CACHE_ASSOC;i++){
                if(cache_array[cache_index][i].timestamp>entry_index){
                    entry_index=i;
                }
            }
    }

    /* Otherwise, search over all entries to find the least recently used entry by checking 'timestamp' */

    return entry_index; 
}

int access_memory(void *addr, char type) {
    
    /* Fetch the data from the main memory and copy them to the cache */
    /* void *addr: addr is byte address, whereas your main memory address is word address due to 'int memory_array[]' */

    /* You need to invoke find_entry_index_in_set() for copying to the cache */
    
    char buf[100];
    sprintf(buf, "%s", (char*)addr);
    
    int address=0;
    for(int i=0;i<10;i++){
        if(buf[i]=='\0')break;
        address=address*10+buf[i]-'0';
    }


    int Address=((int)(address/DEFAULT_CACHE_BLOCK_SIZE_BYTE))*DEFAULT_CACHE_BLOCK_SIZE_BYTE/WORD_SIZE_BYTE;


    
    unsigned int memoryData=memory_array[Address];

    char data[DEFAULT_CACHE_BLOCK_SIZE_BYTE];

    for(int i=0;i<4;i++){
        data[i]=memoryData%(256);
        memoryData/=256;
    }

    memoryData=memory_array[Address+1];

    for(int i=4;i<8;i++){
        data[i]=memoryData%256;
        memoryData/=256;
    }

    int set=(int)(address/DEFAULT_CACHE_BLOCK_SIZE_BYTE)%(CACHE_SET_SIZE) ;
    int tag=(int)(address/DEFAULT_CACHE_BLOCK_SIZE_BYTE)/(CACHE_SET_SIZE) ;

    int index=find_entry_index_in_set(set);
    cache_array[set][index].valid=1;
    cache_array[set][index].tag=tag;
    cache_array[set][index].timestamp=global_timestamp++;
    for(int i=0;i<8;i++){
        cache_array[set][index].data[i]=data[i];
    }

   
    int size;
    if(type=='b')size=1;
    else if (type=='h')size=2;
    else{ size=4; }
    int startbyte=address%8;    //8도 뭘로 바꿔야됨
    unsigned int val=0;
    for(int j=startbyte+size-1;j>=startbyte;j--){
        int v=cache_array[set][index].data[j];
        if(v>=0)val=val*256+v;
        else{
            char outputData[10];
            sprintf(outputData, "%x", cache_array[set][index].data[j]);
            if(outputData[6]>='a'&&outputData[6]<='f')v=outputData[6]-'a'+10;
            else{ v=outputData[6]-'0'; }
            v*=16;
            if(outputData[7]>='a'&&outputData[7]<='f')v=v+outputData[7]-'a'+10;
            else{ v=v+outputData[7]-'0'; }
            val=val*256+v;
        }
    }
    /* Return the accessed data with a suitable type */    

    return val; //datatype생각해봐야함 지금은 걍 1words 리턴
}
/*

5. 교수님이 쓰라고 정해준 변수 안쓴거 있나 보자

학부때 죽었다 생각하고 교수가되어서 합창을 하라 -형준리ㅡ
*/