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

//------------------------------------------------------------------------------------------------------------------
//Global enums

typedef enum _searchStatus {

    eCacheHit = 0x01,
    eCacheMiss = 0x02,
    eCacheHitInPrefetch = 0x04,
    eCacheMissInPrefetch = 0x08,
    eNone = 0xFF
}TCacheSearchStatus;

typedef enum _cacheRequestType {

    eCacheRead = 0x01,
    eCacheWrite = 0x02,
    eCachePrefetchRead = 0x03
}TCacheRequestType;



//------------------------------------------------------------------------------------------------------------------
// Global Function prototypes.

//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheDSInit
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------
bool CacheStoreData(TLinkedListNode *headPtr, uint32_t memAddress, uint32_t dataToBeStored);

//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheDSInit
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------

bool CacheLoadData(TLinkedListNode *headPtr, uint32_t memAddress, uint32_t *loadedData) ;

TCacheSearchStatus SearchTag(TLinkedListNode *headPtr, uint32_t memAddress, uint16_t *tagFoundIndex,
                            TCacheSearchStatus *tagFoundInPrefetchStatus, uint16_t *tagFoundInPrefetchIndex, uint16_t *tagFoundInPrefetchStream)  ;

uint32_t CacheBlockRequest(TLinkedListNode *headPtr, uint32_t memAddress);//, TCacheRequestType cacheOperation);

uint32_t FindLRUBlockIndex(TLinkedListNode *headPtr, uint32_t index) ;

void WriteBack(TLinkedListNode *headPtr, uint32_t memAddress);
bool IsCacheSetFull(TLinkedListNode *headPtr, uint32_t index, uint32_t *tagEmptyIndex);


#endif