//------------------------------------------------------------------------------------------------------------------
// AUTHOR NAME - Chandan Srinivas
// DATE        - 30 August 2024
// NAME        - CacheOperation.h
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//*************************************************************************************************************** */

#ifndef CACHE_OPERATION_H
#define CACHE_OPERATION_H

#include <stdio.h>
#include <stdint.h>
#include "Queue.h"
#include "Cache.h"
#include "LinkedList.h"
#define DEBUGDISPLAYL1

#define DEBUGDISPLAYL2

//------------------------------------------------------------------------------------------------------------------
//Global enums

typedef enum _cacheSearchStatus {

    eCacheHit = 0x01,
    eCacheMiss = 0x02,
    eNone = 0xFF
}TCacheSearchStatus;

typedef enum _prefetchSearchStatus {

    eCacheHitInPrefetch = 0x01,
    eCacheMissInPrefetch = 0x02,
    eNoOp = 0xFF
}TPrefetchSearchStatus;



typedef enum _cacheRequestType {

    eCacheRead = 0x01,
    eCacheWrite = 0x02,
    eCachePrefetchRead = 0x03
}TCacheRequestType;

#define GENERATE_DEBUG_FILE

//#define DEBUG_PRINTDATA

//------------------------------------------------------------------------------------------------------------------
// Global Function prototypes.

//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheDSInit
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------
#ifdef GENERATE_DEBUG_FILE
bool CacheStoreData(TLinkedListNode *headPtr, uint32_t memAddress, uint32_t dataToBeStored,FILE *debugFile);
#elif
bool CacheStoreData(TLinkedListNode *headPtr, uint32_t memAddress, uint32_t dataToBeStored);
#endif

//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheDSInit
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------
#ifdef GENERATE_DEBUG_FILE
bool CacheLoadData(TLinkedListNode *headPtr, uint32_t memAddress, uint32_t *loadedData, FILE *debugFile) ;
#elif
bool CacheLoadData(TLinkedListNode *headPtr, uint32_t memAddress, uint32_t *loadedData) ;
#endif
TCacheSearchStatus SearchTag(TLinkedListNode *headPtr, uint32_t memAddress, uint16_t *tagFoundIndex,
                            TPrefetchSearchStatus *tagFoundInPrefetchStatus, uint16_t *tagFoundInPrefetchIndex, uint16_t *tagFoundInPrefetchStream)  ;
#ifdef GENERATE_DEBUG_FILE
uint32_t CacheBlockRequest(TLinkedListNode *headPtr, uint32_t memAddress, FILE *debugFile);//, TCacheRequestType cacheOperation);
#elif
uint32_t CacheBlockRequest(TLinkedListNode *headPtr, uint32_t memAddress);
#endif
uint32_t FindLRUBlockIndex(TLinkedListNode *headPtr, uint32_t index) ;
#ifdef GENERATE_DEBUG_FILE
void WriteBack(TLinkedListNode *headPtr, uint32_t memAddress, FILE *debugFile);
#elif
void WriteBack(TLinkedListNode *headPtr, uint32_t memAddress);
#endif
bool IsCacheSetFull(TLinkedListNode *headPtr, uint32_t index, uint32_t *tagEmptyIndex);


#endif