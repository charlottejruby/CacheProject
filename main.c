/*
 * main.c
 *
 * 20493-01 Computer Architecture
 * Term Project on Implementation of Cache Mechanism
 *
 * Skeleton Code Prepared by Prof. HyungJune Lee
 * Nov 16, 2022
 *
 */

#include <stdio.h>
#include "cache_impl.h"

int num_cache_hits = 0;
int num_cache_misses = 0;

int num_bytes = 0;
int num_access_cycles = 0;

int global_timestamp = 0;

int retrieve_data(void *addr, char data_type) {
    int value_returned = -1; /* accessed data */

    /* Invoke check_cache_data_hit() */
    value_returned=check_cache_data_hit(addr,data_type); //hit이면 그 값을, miss면 -1리턴

    /* In case of the cache miss event, access the main memory by invoking access_memory() */
    if(value_returned ==-1){
        value_returned = access_memory(addr,data_type);
        num_cache_misses++;
    }
    else{
        num_cache_hits++;
    }

    return value_returned;    
}

int main(void) {
    FILE *ifp = NULL, *ofp = NULL;
    unsigned long int access_addr; /* byte address (located at 1st column) in "access_input.txt" */
    char access_type; /* 'b'(byte), 'h'(halfword), or 'w'(word) (located at 2nd column) in "access_input.txt" */
    int accessed_data; /* This is the data that you want to retrieve first from cache, and then from memory */ 
    
    init_memory_content();
    init_cache_content();

    ifp = fopen("access_input.txt", "r");
    if (ifp == NULL) {
        printf("Can't open input file\n");
        return -1;
    }
    ofp = fopen("access_output.txt", "w");
    if (ofp == NULL) {
        printf("Can't open output file\n");
        fclose(ifp);
        return -1;
    }
    
    /* Fill out here by invoking retrieve_data() */
    
    fprintf(ofp,"%s\n","[Accessed Data]");
    
    char str[50];
    while (fgets(str, sizeof(str), ifp) != NULL ) {
        char adrstr[10];
        int i=0;
        for(;i<15;i++){
            if(str[i]=='\0'||str[i]==' '){adrstr[i]='\0';break;}
            else{
                adrstr[i]=str[i];
            }
        }
        for(;i<10;i++){
            if(str[i]!=' '){
                access_type=str[i];
                break;
            }
        }
        fprintf(ofp,"%-4s\t%c\t",adrstr,access_type);
        if(access_type=='b')num_bytes+=1;
        else if(access_type=='h')num_bytes+=2;
        else {num_bytes+=4;}
        int val=retrieve_data(adrstr,access_type);
        fprintf(ofp,"0x%x\n",val); //문자열 입력
        
    }
    
    num_access_cycles=num_cache_misses*(CACHE_ACCESS_CYCLE+MEMORY_ACCESS_CYCLE)+num_cache_hits*CACHE_ACCESS_CYCLE;
    
    fprintf(ofp,"%s\n","-----------------------------------------");
    if(DEFAULT_CACHE_ASSOC ==1)fprintf(ofp,"%s","[Direct mapped cache performance]\n");
    else if(DEFAULT_CACHE_ASSOC ==2)fprintf(ofp,"%s","[2-way set associative cache performance]\n");
    else {fprintf(ofp,"%s","[Fully associative cache performance]\n");}

    fprintf(ofp,"Hit ratio = %.2f (%d/%d)\n",(double)((double)num_cache_hits/(double)(num_cache_hits+num_cache_misses)),num_cache_hits,num_cache_hits+num_cache_misses);
    fprintf(ofp,"Bandwidth = %.2f (%d/%d)\n",(double)((double)num_bytes/(double)num_access_cycles),num_bytes,num_access_cycles);
    


    fclose(ifp);
    fclose(ofp);
    
    print_cache_entries();
    return 0;
}




