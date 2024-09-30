//------------------------------------------------------------------------------------------------------------------
// AUTHOR NAME - Chandan Srinivas
// DATE        - 30 August 2024
// NAME        - Cache.h
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//*************************************************************************************************************** */


#ifndef CACHE_H
#define CACHE_H

#include <stdio.h>
#include <stdint.h>
#include "Queue.h"
#include "Prefetch.h"
#include <stdlib.h>
#include <math.h>
#include "LinkedList.h"
#include "CacheOperation.h"


//------------------------------------------------------------------------------------------------------------------
// Global Macros

#define ADDRESSING_WIDTH 32U                    // 32 bit addressing in hex
#define LOG2(X)  ceil(log(X) / log(2))          // logarithmic value to the base 2.

#define BLOCKOFFSETEXTRACT(address, bitField)   (memAddress & ( ( 1 << bitField) - 1UL))
                                                // Extract BlockOffset value from Memory address
                                                // bitField is number of bits used to represent bytes.

#define INDEXEXTRACT(address, offSet, bitField) ((memAddress >> offSet) & ( ( 1 << bitField) - 1UL))
                                                // Extract Index value form Memory Address.
                                                // here offset is number of block Offset Bits

#define TAGEXTRACT(address, offSet, bitField)   ((memAddress >> offSet) & ( ( 1 << bitField) - 1UL))
                                                // Extract Tag Value from Memory Address
                                                // Here offset value is number fo BlockOffset bits + number of index bits.


#define INDEXRETREIVE(indexAddress, offSet)       (indexAddress << offSet);
                                                // Extract Index value form Memory Address.
                                                // here offset is number of block Offset Bits

#define TAGRETREIVE(tagAddress, offSet)   (tagAddress << offSet);
                                                // Extract Tag Value from Memory Address
                                                // Here offset value is number fo BlockOffset bits + number of index bits.

//------------------------------------------------------------------------------------------------------------------
// Structure type declaraion


typedef enum _prefetchAvailableTag{

    ePrefetchPresent = 0x1,
    ePrefetchAbsent  = 0x2


}TprefetchAvailableStatus;

typedef struct _prefetchStats{

    uint32_t readCount;
    uint32_t missCount;
    uint32_t prefetchCount;
    uint32_t hitCount;

}TprefetchStats;


//------------------------------------------------------------------------------------------------------------------
//Global enums
// Enum to store current status of the cache Controller - Read/Write.
typedef enum _cacheOperationTag{

    eRead  = 0xFF,
    eWrite = 0xFE
}TcacheOperation;

typedef enum _cacheReplacementPolicyTag{

    eDM  = 0xFF,
    eLRU = 0xFE
}TcacheReplacementPolicy;

// Enum to select between various cacheReplacement policies.
typedef enum _cacheWritePolicy{

    eWBWA  = 0xFF
}TcacheWritePolicy;


//------------------------------------------------------------------------------------------------------------------
//Global typedefs

// DataStore of tag attributes of a set i.e Tag bit to perform replacement of blocks
// valid bit to monitor the validity of the block, dirty bit to know the write status of the block
// and counter to monitor how far behind the particular block is and implement the replacement policy.
typedef struct _cacheTag{

    uint32_t tag;                   // Store tag value of a particular address.
    bool     valid;                 // Store Valid bit of a particular Cache set.
    bool     dirty;                 // Store current status/state of the cache block.
    uint32_t counter;               // Store the counter value to know the reference i.e if
                                    // the counter value of a paticular block is greater than others in the set , it will be evicted.
    uint32_t data;
}TCacheTagDS;


// DataStore of Cache set attributes i.e
// cacheTagDS to store details such as tag, valid, dirty and counter values.
// This also has index value to decode the Cache set where data is present.
typedef struct _cacheSet{
    struct _cacheTag *cacheTagDS;  // Pointer to strcuture to hold details of tag, valid,
                                      // dirty bit and counter value to monitor least recently used cache block.
                                      // Number of structures instantiated per cache Set depends on associativity required.
}TCacheSetDS;





typedef struct _cacheStats{

    uint32_t readCount;
    uint32_t readMissCount;
    uint32_t writeCount;
    uint32_t writeMissCount;
    uint32_t writeBackCount;
    uint32_t hitCount;
    uint32_t missCount;
    float_t missRate;

} TCacheStats;


// Datastore to store a cache in hierarchy of memory.
typedef struct _cacheDS{

    // Basic Data required per level of memory.
    char*               name;        // Name of the cache i.e Cache level.
    uint32_t            numOfSets;   // Number fo sets per cache.
    uint32_t            size;        // Total size fo the cache.
    uint32_t            assoc;       // Associativity of the cache.
    uint32_t            blockSize;   // BlockSize.

    uint8_t             numOfIndexBits; // Bits required to represent number of sets.
    uint8_t             numOfBlockOffsetBits;  // bits required to represent each byte in a Block.
    uint8_t             numOfTagBits;   // Bits required to represent/identify a block.

    TprefetchAvailableStatus     prefetchAvailable;
    uint8_t             numOfStreams;   // Number of prefetch streams in a particular cache level.
    uint8_t             numOfBlocksPerStream;   // Number of blocks to be stored per stream, i.e depth.
    struct _cacheSet    *cacheSetDS;
                        // Dynamically allocated cache set depending on number of cache sets required per cahce level.
    TCacheStats         cacheStatistics;
    TprefetchStats       prefetchStatistics;
    struct _queue_tag   *prefetchQueue;
    uint32_t             totalMemoryTraffic;
}TCacheDS;


//------------------------------------------------------------------------------------------------------------------
// Global functions

//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheDSInit
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------
void CacheDSInit(TCacheDS* cacheDSPtr, char* name, uint32_t blockSize, uint32_t totalCacheSize, uint32_t cacheAssoc,
                 uint8_t numofStreams, uint8_t numOfBlocksPerStreams);

//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheInit
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------

void CacheInit(TLinkedListNode *headPtr);

//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheSetAndTagInit
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------
void CacheSetAndTagInit(TLinkedListNode *headPtr);

//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheDeallocateMemory
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------
void CacheDeallocateMemory(TLinkedListNode *headPtr);

//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheAllocateMemory
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------
void CacheAllocateMemory(TLinkedListNode * headPtr);

void UpdateLRUCounters(TLinkedListNode *headPtr, uint32_t index, uint32_t tagFoundIndex);
#endif
