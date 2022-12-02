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

int num_cache_hits = 0;         //# of hits(use to get the value of hit ration)
int num_cache_misses = 0;       //# of misses(use to get the value of hit ration)

int num_bytes = 0;              //# of accessed bytes(use to get the value of bandwidth)
int num_access_cycles = 0;      //# of clock cycle(use to get the value of bandwidth)

int global_timestamp = 0;       //# of data access trials

//this function is for extracting datas
int retrieve_data(void *addr, char data_type) {
    /*the initial value should be 1, because if the cache hit happens, 
    then the variable will be change as 1 by check_cache_data_hit function*/
    int value_returned = -1; /* accessed data */

    /* Invoke check_cache_data_hit() */
    value_returned=check_cache_data_hit(addr,data_type);    //if cache_hit --> return 1, cache miss --> return -1

    /* In case of the cache miss event, access the main memory by invoking access_memory() */
    /*If cache miss event happens, value_returned will be -1.
    through the conditional statement, when a cache miss event occurs, value_returned is received through memory access. 
    Furthermore, since a cache miss event has occurred, add count of num_cache_misses.*/

    if(value_returned ==-1){                            //when cache miss event occurs
        value_returned = access_memory(addr,data_type); //the memory is accessed through the function access_memory
        num_cache_misses++;                             //since, this conditional statement is for cache miss event, count the num_cache_misses by adding one by one
    }
    else{                                               //this is because the only number of cases in which value_returned may not be -1 in the code up until now is when the cache_hit event occurs
        num_cache_hits++;                               //count the num_cache_hits by adding one by one to count the number of cache hit events                               
    }
    return value_returned;                              //the value returned through cache or memory access is returned to the retrieve_data function
}

//main function
int main(void) {                                        //start of main function
    FILE *ifp = NULL, *ofp = NULL;                      //declare the necessary pointers to prepare for opening and closing the file. 
    unsigned long int access_addr;                      /* byte address (located at 1st column) in "access_input.txt" */
    char access_type;                                   /* 'b'(byte), 'h'(halfword), or 'w'(word) (located at 2nd column) in "access_input.txt" */
    int accessed_data;                                  /* This is the data that you want to retrieve first from cache, and then from memory */ 
    
    init_memory_content();                              //initialize the memory through the previously created function
    init_cache_content();                               //initialize the cache throught the previously created function

    ifp = fopen("access_input.txt", "r");               //open the input file to be read, using r (read) mode
    if (ifp == NULL) {                                  //conditional statement for when the pointer does not point to any file because the file is not open
        printf("Can't open input file\n");              //print to notify that the program cannot open the input file
        return -1;                                      //return -1 to indicate a non-normal termination
    }
    ofp = fopen("access_output.txt", "w");              //open the output file to write, using w (write) mode
    if (ofp == NULL) {                                  //conditional statement for when the pointer does not point to any file because the file is not open
        printf("Can't open output file\n");             //print to notify that the program cannot open the output file
        fclose(ifp);                                    //close the input file
        return -1;                                      //return -1 to indicate a non-normal termination
    }
    
    /* Fill out here by invoking retrieve_data() */
    
    fprintf(ofp,"%s\n","[Accessed Data]");              //file print to fit the provided output format
    
    char str[50];                                       //
    while (fgets(str, sizeof(str), ifp) != NULL ) {     //
        char adrstr[10];                                //
        int i=0;                                        //a variable to be used in the loop statement is set into 0 in advance
        for(;i<15;i++){                                 //
            if(str[i]=='\0'||str[i]==' '){adrstr[i]='\0';break;}    //
            else{                                       //
                adrstr[i]=str[i];                       //
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

    //
    fprintf(ofp,"Hit ratio = %.2f (%d/%d)\n",(double)((double)num_cache_hits/(double)(num_cache_hits+num_cache_misses)),num_cache_hits,num_cache_hits+num_cache_misses);
    //
    fprintf(ofp,"Bandwidth = %.2f (%d/%d)\n",(double)((double)num_bytes/(double)num_access_cycles),num_bytes,num_access_cycles);
    


    fclose(ifp);                                        //close the input file
    fclose(ofp);                                        //close the output file
    
    print_cache_entries();                              //
    return 0;                                           //return 1 to indicate a normal termination
}




