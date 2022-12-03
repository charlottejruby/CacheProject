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

extern int num_cache_hits;                                          //# of hits
extern int num_cache_misses;                                        //# of misses

extern int num_bytes;                                               //# of bytes
extern int num_access_cycles;                                       //# of access cycles

extern int global_timestamp;                                        //most recent access time

cache_entry_t cache_array[CACHE_SET_SIZE][DEFAULT_CACHE_ASSOC];     //data in cache with size
int memory_array[DEFAULT_MEMORY_SIZE_WORD];                         //data in memory with size of 128 words


/* DO NOT CHANGE THE FOLLOWING FUNCTION */
void init_memory_content() {                                        //function for initializing the memory content  
    unsigned char sample_upward[16] = {0x001, 0x012, 0x023, 0x034, 0x045, 0x056, 0x067, 0x078, 0x089, 0x09a, 0x0ab, 0x0bc, 0x0cd, 0x0de, 0x0ef};    //arbitrary data set to construct memory
    unsigned char sample_downward[16] = {0x0fe, 0x0ed, 0x0dc, 0x0cb, 0x0ba, 0x0a9, 0x098, 0x087, 0x076, 0x065, 0x054, 0x043, 0x032, 0x021, 0x010};  //arbitrary data set to construct memory
    int index, i=0, j=1, gap = 1;                                   //set up a variable to be used in a loop statement
    
    /*loop to construct actual values in memory*/
    for (index=0; index < DEFAULT_MEMORY_SIZE_WORD; index++) {      //loop for memory setup
        memory_array[index] = (sample_upward[i] << 24) | (sample_upward[j] << 16) | (sample_downward[i] << 8) | (sample_downward[j]);   //shifting bits are performed for memory set up
        if (++i >= 16)                                              //add i for cycle
            i = 0;                                                  //cycle
        if (++j >= 16)                                              //add j for cycle
            j = 0;                                                  //cycle
        
        if (i == 0 && j == i+gap)                                   //difference of i and j == gap
            j = i + (++gap);                                        //increases 1 gap and new j for each value
            
        printf("mem[%d] = %#x\n", index, memory_array[index]);      //print to check the value in memory
    }
}   

/* DO NOT CHANGE THE FOLLOWING FUNCTION */
void init_cache_content() {                                         //function for initializing the cache contents
    int i, j;                                                       //variable for loop 
    
    for (i=0; i<CACHE_SET_SIZE; i++) {                              //loop statement for cache initializing
        for (j=0; j < DEFAULT_CACHE_ASSOC; j++) {                   //loop statement for cache initializing
            cache_entry_t *pEntry = &cache_array[i][j];             //Set a pointer of type cache_entry_t to point the cache array.
            pEntry->valid = 0;                                      //Since there is nothing in initial, set the valid bit as 0.
            pEntry->tag = -1;                                       //Likewise, since there is no tag, set it to -1.
            pEntry->timestamp = 0;                                  //Since there was no access trial, so set the timestampe as 0.
        }
    }
}

/* DO NOT CHANGE THE FOLLOWING FUNCTION */
/* This function is a utility function to print all the cache entries. It will be useful for your debugging */
void print_cache_entries() {                                        //function for printing cache entries
    int i, j, k;                                                    //set variable to use in the loop statement
    
    for (i=0; i<CACHE_SET_SIZE; i++) {                              //for each set
        printf("[Set %d] ", i);                                     //print the set number
        for (j=0; j <DEFAULT_CACHE_ASSOC; j++) {                    //for each entry in  a set
            cache_entry_t *pEntry = &cache_array[i][j];             //set a pointer of type cache_entry_t to point the cache array to print cache entries
            printf("V: %d Tag: %#x Time: %d Data: ", pEntry->valid, pEntry->tag, pEntry->timestamp);    //print valid bit, tag, and timestamp
            for (k=0; k<DEFAULT_CACHE_BLOCK_SIZE_BYTE; k++) {       //for each block in a entry
                printf("%#x(%d) ", pEntry->data[k], k);             //print data
            }
            printf("\t");                                           //print tab
        }
        printf("\n");                                               //print the line
    }
}

int check_cache_data_hit(void *addr, char type) {                   //a function that check whether the cache hit event happens

    /* Fill out here */
    /*Calculate set in the beginning.
    And then caculate a tag and check whether the value of the tags are same.
    If tags are same, hit event happens and set the return value.
    Else if tag values are different, miss event happnes and set the return value as -1.*/

    char buf[100];                                                  //set a string buffer
    sprintf(buf, "%s", (char*)addr);                                //store the address vaule in the buffer

    
    int address=0;                                                  //an arbitrary variable to convert the address value in the buffer into an integer
    for(int i=0;i<10;i++){                                          //
        if(buf[i]=='\0')break;                                      //if '\0' appears in the buffer value, exit the loop
        address=address*10+buf[i]-'0';                              //put the buffer value into the address, meanwhile multiply 10 to the original address value
    }                                                               //since the buffer is a char type, '\0' is subtracted


    int val=-1;                                                     //val variable is the value to be returned by the check_cache_data_hit function
                                                                    //if the hit event did not occur, -1 should be returned, so the initial value is -1

    int set=(int)(address/DEFAULT_CACHE_BLOCK_SIZE_BYTE)%(CACHE_SET_SIZE) ; //calculate the set
                                                                            //set of the address is the remainder of dividing the (address/cache block size) by the cache set size
    int tag=(int)(address/DEFAULT_CACHE_BLOCK_SIZE_BYTE)/(CACHE_SET_SIZE) ; //calculate the tag
                                                                            //tag of the address is the quotient of dividing the (address/cache block size) by the cache set size
    for(int i=0;i<DEFAULT_CACHE_ASSOC;i++){                         //the number of times the loop is repeated depends on the cache associativity
        if(cache_array[set][i].tag == tag){                         //when tags are same(hit event occurs)
            int size;                                               //a variable named size is used to distinguish bytes according to access types
            if(type=='b')size=1;                                    //if the type is byte, the access type is one byte so the size is 1
            else if (type=='h')size=2;                              //if the type is halfword, the access type is two bytes so the size is 2
            else{ size=4; }                                         //else means that the wordk is the access type, so the size should be 4
            int startbyte=address%DEFAULT_CACHE_BLOCK_SIZE_BYTE;    //Since the cache store two words(=8byte), accessed data start at 'address modulo 8' in cache 
            val=0;                                                  //set val zero
            for(int j=startbyte+size-1;j>=startbyte;j--){           //access data from 'startbyte' to 'startbyte+size-1'(reverse). subtract 1 because index start at 0
                int v=cache_array[set][i].data[j];                  //get data from cache to varibale v
                if(v>=0)val=val*256+v;                              //if the data is larger or equal to 0 
                                                                    //since data is a hexadecimal number, two digits must be shifted left, so multiply by the power of 16 then add
                else{                                               //else if the data is less than 0 (if first bit is '1', v is considered as negative number because of sign extension)
                    char outputData[10];                            //set another string buffer
                                                                    //if v<0, v is 8 bit data. necessary data is in 6th, 7th bit and others(0~5bits) are filled with 'f' by sign extension
                    sprintf(outputData, "%x", cache_array[set][i].data[j]); //store the data in the outputData buffer
                                                                    //beacause data is stored as 'char', hexadecimal number A~F cannot recognized by 10~15.
                    if(outputData[6]>='a'&&outputData[6]<='f')v=outputData[6]-'a'+10;   //if data is 'a~f', subtract ascii value of a.
                                                                                        //then 'a~f' become '0~5'. add 10 beacuse 'a' means '10' in hexadecimal
                    else{ v=outputData[6]-'0'; }                    //else means data is '0~9' in char. make it '0~9' in integer by subtracting ascii value of 'char zero'. 
                    v*=16;                                          //since data is a hexadecimal number, so multiply by the power of 16
                    if(outputData[7]>='a'&&outputData[7]<='f')v=v+outputData[7]-'a'+10; //change heximal number 'a~f' to Integer '10~15' 
                    else{ v=v+outputData[7]-'0'; }                  //change character '0~9' to integer '0~9'
                    val=val*256+v;                                  //since data is a hexadecimal number, two digits must be shifted left, so multiply by the power of 16 then add
                }
            }
            cache_array[set][i].timestamp=global_timestamp++;       //increase the timestamp to indicate the recent access time
            break;                                                  //exit the loop
        }
    }
    return val;                                                     //return the value (if cache hit events happen, return the data and if cache miss event happens, return -1)



    /* Return the data */
    //return 0;    
}

int find_entry_index_in_set(int cache_index) {
    int entry_index=-1;                                             //value to check there is empty space and to return proper index in cache

    /* Check if there exists any empty cache space by checking 'valid' */
    for(int i=0;i<DEFAULT_CACHE_ASSOC;i++){                         //for each entry in set
        if(cache_array[cache_index][i].valid==0){                   //if valid bit is 0. 'valid==0' means that pointing space is empty
            entry_index=i; break;                                   //store index of empty space and break the loop
        }
    }
    if(entry_index==-1){                                            //if there is no empty space, 'entry_index' is still '-1'
                                                                    //find entry with lowset timestamp because timestamp continuously increases
            int min=100;                                                       
            for(int i=0;i<DEFAULT_CACHE_ASSOC;i++){                 //for each entry in set
                if(cache_array[cache_index][i].timestamp<min){  //if timestamp of entry is lower than entry_index, 
                    entry_index=i;                                  //set entry_index to i
                    min=cache_array[cache_index][i].timestamp;
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
    
    char buf[100];                                                      //set a string buffer
    sprintf(buf, "%s", (char*)addr);                                    //store string of which adress is addr(parameter) 
                                                                        //byte address in decimal number is store in 'buf' by character.
    int address=0;                                                      //variable to store adress
    for(int i=0;i<10;i++){                                              //add i for cycle
        if(buf[i]=='\0')break;                                          //break if string 'buf' has null(if string is done)
        address=address*10+buf[i]-'0';                                  //since address is a decimal number, multiply by 10. then add accessed data
                                                                        //since buf[i] is character type, make integer type by subtracting ascii value of 'char zero'
    }
                                                                        //now, Byte Adress is stored in 'address', integer variable

    int byteAddress=((int)(address/DEFAULT_CACHE_BLOCK_SIZE_BYTE))*DEFAULT_CACHE_BLOCK_SIZE_BYTE/WORD_SIZE_BYTE;    
                                                                        //calculate byte adress 
    
    unsigned int memoryData=memory_array[byteAddress];                  //access memory by calculated byte adress
                                                                        //one word is stored in memoryData
    char data[DEFAULT_CACHE_BLOCK_SIZE_BYTE];                           //string for data that has to store in cache
                                                                        //char holds 1 byte. 1 byte holds two hexadecimal number because one hexadecimal number hold 4 bits

    for(int i=0;i<4;i++){                                               //store memorry_array[byteAdress] in data[0]~data[3]
                                                                        //since Little endian is used, LSB is stored first, at the lowest address.
        data[i]=memoryData%(256);                                       //store LSB in 'data[i]' 
                                                                        //since char store 2 hexadecimal number, use 16*16=256
        memoryData/=256;                                                //erase least two bits
    }

    memoryData=memory_array[byteAddress+1];                             //since cache store 2 words(8 byte) access memory of which adress is 'byteAddress+1'
                                                                        //one word is stored in memoryData
    for(int i=4;i<8;i++){                                               //store memorry_array[byteAdress] in data[4]~data[8]
        data[i]=memoryData%256;                                         //store 2 least bits in 'data[i]'
        memoryData/=256;                                                //erase least two bits
    }

    int set=(int)(address/DEFAULT_CACHE_BLOCK_SIZE_BYTE)%(CACHE_SET_SIZE) ;                              //calculate set
    int tag=(int)(address/DEFAULT_CACHE_BLOCK_SIZE_BYTE)/(CACHE_SET_SIZE) ;                              //calculate tag

    int index=find_entry_index_in_set(set);                             //invoke find_entry_index_in_set() for copying to the cache
    cache_array[set][index].valid=1;                                    //set valid bit 1 to indicate this array is used
    cache_array[set][index].tag=tag;                                    //write caculated 'tag value to tag of cache
    cache_array[set][index].timestamp=global_timestamp++;               //write timestamp and then add 1 to indicate the recent access time
    for(int i=0;i<8;i++){                                               
        cache_array[set][index].data[i]=data[i];                        //write 8bits of data in cache
    }                                                                   

   
    int size;                                                           //a variable named size is used to distinguish bytes according to access types
    if(type=='b')size=1;                                                //if the type is byte, the access type is one byte so the size is 1
    else if (type=='h')size=2;                                          //if the type is halfword, the access type is two bytes so the size is 2
    else{ size=4; }                                                     //else means that the wordk is the access type, so the size should be 4
    int startbyte=address%DEFAULT_CACHE_BLOCK_SIZE_BYTE;                //Since the cache store two words(=8byte), accessed data start at 'address modulo 8' in cache 
    unsigned int val=0;                                                 //set val zero
    for(int j=startbyte+size-1;j>=startbyte;j--){                       //access data from 'startbyte' to 'startbyte+size-1'(reverse). subtract 1 because index start at 0
        int v=cache_array[set][index].data[j];                          //get data from cache to varibale v
        if(v>=0)val=val*256+v;                                          //if the data is larger or equal to 0 
                                                                        //since data is a hexadecimal number, two digits must be shifted left, so multiply by the power of 16 then add
        else{                                                           //else if the data is less than 0 (if first bit is '1', v is considered as negative number because of sign extension)
            char outputData[10];                                        //set another string buffer
                                                                        //if v<0, v is 8 bit data. necessary data is in 6th, 7th bit and others(0~5bits) are filled with 'f' by sign extension
            sprintf(outputData, "%x", cache_array[set][index].data[j]); //store the data in the outputData buffer
                                                                        //beacause data is stored as 'char', hexadecimal number A~F cannot recognized by 10~15
            if(outputData[6]>='a'&&outputData[6]<='f')v=outputData[6]-'a'+10;   //if data is 'a~f', subtract ascii value of a.
                                                                                //then 'a~f' become '0~5'. add 10 beacuse 'a' means '10' in hexadecimal
            else{ v=outputData[6]-'0'; }                                //else means data is '0~9' in char. make it '0~9' in integer by subtracting ascii value of 'char zero'
            v*=16;                                                      //since data is a hexadecimal number, so multiply by the power of 16
            if(outputData[7]>='a'&&outputData[7]<='f')v=v+outputData[7]-'a'+10;     //change heximal number 'a~f' to Integer '10~15' 
            else{ v=v+outputData[7]-'0'; }                              //change character '0~9' to integer '0~9'
            val=val*256+v;                                              //since data is a hexadecimal number, two digits must be shifted left, so multiply by the power of 16 then add
        }
    }
    /* Return the accessed data with a suitable type */    

    return val;                                                         //return the value
}
