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
    
    fprintf(ofp,"%s\n","[Accessed Data]");              //print into file to fit the provided output format
    
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
        fprintf(ofp,"%-4s\t%c\t",adrstr,access_type);   //file print to fit the provided output format
        if(access_type=='b')num_bytes+=1;               //if access_type is byte, add 1 in num_bytes since, a byte is one byte
        else if(access_type=='h')num_bytes+=2;          //if access_type is halfword, add 2 in num_bytes since, a halfword is two bytes
        else {num_bytes+=4;}                            //else means that the access_type is word, add 4, since a word is fout bytes
        int val=retrieve_data(adrstr,access_type);      //the value obtained through memory or cache access is returned through the retrieve_data function, and it is put into a variable nameed val to print in output file
        fprintf(ofp,"0x%x\n",val);                      //write a data obtained through memory or cache access to the output file, according to the format (0x and hexadecimal expression)
    }
    
    /*In the calculation method of num_access_cycle, data obtained through cache access is obtained only through cache access cycle, and data obtained through memory, memory access is perfomed after the cache access.
    The number of num_cache_misses must be multiplied by (memory access cycle + cache access cycle), and num_cache_hits should be multiplied by cache access cycle.*/
    num_access_cycles=num_cache_misses*(CACHE_ACCESS_CYCLE+MEMORY_ACCESS_CYCLE)+num_cache_hits*CACHE_ACCESS_CYCLE;
    
    fprintf(ofp,"%s\n","-----------------------------------------");    //print into file to fit the provided output format
    if(DEFAULT_CACHE_ASSOC ==1)fprintf(ofp,"%s","[Direct mapped cache performance]\n"); //If cache associativity is 1, the sentence is printed according to the provided output format.
    else if(DEFAULT_CACHE_ASSOC ==2)fprintf(ofp,"%s","[2-way set associative cache performance]\n");    //If cache associativity is 2, the sentence is printed according to the provided output format.
    else {fprintf(ofp,"%s","[Fully associative cache performance]\n");} //else case is for fully associative cache, so print the sentence according to the provided output format.

    //The hit ratio must divide num_cache_hits by (num_cache_hits + num_cache_misses), and we set the result to be printed until two decimal point.
    fprintf(ofp,"Hit ratio = %.2f (%d/%d)\n",(double)((double)num_cache_hits/(double)(num_cache_hits+num_cache_misses)),num_cache_hits,num_cache_hits+num_cache_misses);
    //For bandwidth, num_bytes should be divided by num_access_cycle, and this value is also printed until two decimal point.
    fprintf(ofp,"Bandwidth = %.2f (%d/%d)\n",(double)((double)num_bytes/(double)num_access_cycles),num_bytes,num_access_cycles);
    


    fclose(ifp);                                        //close the input file
    fclose(ofp);                                        //close the output file
    
    print_cache_entries();                              //print the final cache entries by invoking print_cache_entries()
    return 0;                                           //return 1 to indicate a normal termination
}




