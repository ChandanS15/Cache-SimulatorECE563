//------------------------------------------------------------------------------------------------------------------
// AUTHOR NAME - Chandan Srinivas
// DATE        - 30 August 2024
// NAME        - Cache.c
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//*************************************************************************************************************** */
#include "../include/Prefetch.h"

void ExtractPrefetchAddress(TCacheDS *cacheDSPtr, uint32_t memAddress, uint32_t *tag, uint32_t *index);
void RetrievePrefetchAddress(TCacheDS *cacheDSPtr, uint32_t index, uint32_t tag, uint32_t *memAddress);
//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheDSInit
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------
TprefetchDS *SearchTagInPrefetch(TLinkedListNode *headPtr, uint32_t tag, bool *prefetchTagSearchStatus, uint32_t *retrievedPrefetchIndex, uint32_t *retrievedPrefetchStream) {

    TLinkedListNode *cursorPtr = headPtr;

    if(cursorPtr->nextPtr != NULL) {
        cursorPtr = headPtr->nextPtr;
    }
    else {
        cursorPtr = headPtr;
    }
    bool queuePeekStatus;
    TprefetchDS *retrievePrefetchData = (TprefetchDS*)malloc(sizeof(TprefetchDS));
    bool queueRemoveStatus = false;

    // Search for the block availability in all the queue streams.
    for(uint16_t searchQueueIndex = 0 ; searchQueueIndex < cursorPtr->cacheLevelPtr->numOfStreams; searchQueueIndex++) {
        for(uint16_t searchTagIndex = 0; searchTagIndex < cursorPtr->cacheLevelPtr->numOfBlocksPerStream; searchTagIndex++) {

            // Retrieve the data from a particular index of the prefetch stream and check if the tag we are searching for is available.
            // if available return that particular prefetch block back.
            queuePeekStatus = QueuePeekByIndex(&cursorPtr->cacheLevelPtr->prefetchQueue[searchQueueIndex], (uint16_t*)retrievePrefetchData, searchTagIndex);
            if(queuePeekStatus == true && retrievePrefetchData->tag == tag){
                *prefetchTagSearchStatus = true;
                *retrievedPrefetchIndex = searchTagIndex;
                *retrievedPrefetchStream = searchQueueIndex;
                //RemoveTagsFromPrefetch(&cursorPtr->cacheLevelPtr->prefetchQueue[searchQueueIndex], searchTagIndex);
                return retrievePrefetchData;
            }
        }
    }
    *prefetchTagSearchStatus = false;
    return NULL;
}

void FillPrefetchBuffer(TLinkedListNode *headPtr, uint32_t index, uint32_t tag) {
    TLinkedListNode *cursorPtr = headPtr;
    uint16_t prefetchIndex = 0;
    uint16_t numOfStreams = 0;
    TprefetchDS prefetchData;
    uint16_t lruPrefetchStreamIndex = 0;



    uint32_t reqIndex = 0;  uint32_t reqTag = 0;  uint32_t reqBlockOffset = 0;
    uint32_t returnedAddress = 0;
    RetrievePrefetchAddress(cursorPtr->cacheLevelPtr, index, tag, &returnedAddress);

    reqTag = returnedAddress >> cursorPtr->cacheLevelPtr->numOfBlockOffsetBits;
    prefetchData.tag = ++reqTag;
    prefetchData.data = 0;

    lruPrefetchStreamIndex = FindLRUPrefetch(cursorPtr);
    cursorPtr->cacheLevelPtr->prefetchQueue[lruPrefetchStreamIndex].validBit = true;
    for(numOfStreams = 0; numOfStreams < cursorPtr->cacheLevelPtr->numOfStreams; numOfStreams++) {
        for(prefetchIndex = 0; prefetchIndex < cursorPtr->cacheLevelPtr->numOfBlocksPerStream; prefetchIndex++) {

            QueueAppend(&cursorPtr->cacheLevelPtr->prefetchQueue[lruPrefetchStreamIndex], (uint8_t*)&prefetchData);

            prefetchData.tag++;
        }
        cursorPtr->cacheLevelPtr->prefetchQueue[lruPrefetchStreamIndex].count = 4;
    }
}


uint32_t FindLRUPrefetch(TLinkedListNode *headPtr) {
    TLinkedListNode *cursorPtr = headPtr;
    uint32_t prefetchStreamIndex = 0;
    uint32_t lruIndex = 0;

    lruIndex = cursorPtr->cacheLevelPtr->prefetchQueue[0].lruIndex;

    for(prefetchStreamIndex = 0; prefetchStreamIndex < cursorPtr->cacheLevelPtr->numOfStreams; prefetchStreamIndex++) {

        if(cursorPtr->cacheLevelPtr->prefetchQueue[prefetchStreamIndex].lruIndex > lruIndex) {
            lruIndex = cursorPtr->cacheLevelPtr->prefetchQueue[prefetchStreamIndex].lruIndex;
        }
    }
    return lruIndex;
}

void ExtractPrefetchAddress(TCacheDS *cacheDSPtr, uint32_t memAddress, uint32_t *tag, uint32_t *index) {

    if(cacheDSPtr != NULL) {

        // Extratc tag value from given memory Address.
        *tag         = (uint32_t)TAGEXTRACT(memAddress, (cacheDSPtr->numOfBlockOffsetBits + cacheDSPtr->numOfIndexBits),
                                 cacheDSPtr->numOfTagBits);

        // Extract index values from the given memory address.

        *index       = (uint32_t)INDEXEXTRACT(memAddress, (cacheDSPtr->numOfBlockOffsetBits), cacheDSPtr->numOfIndexBits);
    }


}

void RetrievePrefetchAddress(TCacheDS *cacheDSPtr, uint32_t index, uint32_t tag, uint32_t *memAddress) {

    uint32_t retreivedIndex = 0; uint32_t retreivedTag = 0;
    if(cacheDSPtr != NULL) {

        // Extract tag value from given memory Address.
        retreivedTag        = (uint32_t)TAGRETREIVE(tag, (cacheDSPtr->numOfBlockOffsetBits + cacheDSPtr->numOfIndexBits));

        // Extract index values from the given memory address.

        retreivedIndex       = (uint32_t)INDEXRETREIVE(index, (cacheDSPtr->numOfBlockOffsetBits));

        *memAddress = retreivedIndex | retreivedTag;
    }


}