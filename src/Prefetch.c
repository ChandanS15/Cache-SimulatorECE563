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
TprefetchDS *SearchTagInPrefetch(TLinkedListNode *headPtr,uint32_t index, uint32_t tag,
                                bool *prefetchTagSearchStatus, uint32_t *retrievedPrefetchIndex, uint32_t *retrievedPrefetchStream) {

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

    uint32_t tagToSearchInPrefetch = (tag << cursorPtr->cacheLevelPtr->numOfIndexBits) | index;
    uint8_t mruArray[cursorPtr->cacheLevelPtr->numOfStreams];
    FindMRUPrefetch(cursorPtr, mruArray);
    uint32_t searchTill = cursorPtr->cacheLevelPtr->numOfStreams;
    uint32_t searchInPrefetchQueue = 0;
    // Search for the block availability in all the queue streams.
    for(uint32_t searchQueueIndex = 0 ; searchQueueIndex < searchTill; searchQueueIndex++) {
        searchInPrefetchQueue = mruArray[searchQueueIndex];
        for(uint32_t searchTagIndex = 0; searchTagIndex < cursorPtr->cacheLevelPtr->numOfBlocksPerStream; searchTagIndex++) {

            // Retrieve the data from a particular index of the prefetch stream and check if the tag we are searching for is available.
            // if available return that particular prefetch block back.
            queuePeekStatus = QueuePeekByIndex(&cursorPtr->cacheLevelPtr->prefetchQueue[searchInPrefetchQueue], (uint16_t*)retrievePrefetchData, searchTagIndex);
            if(queuePeekStatus == true &&
                (retrievePrefetchData->tag ) == (tagToSearchInPrefetch)){
                *prefetchTagSearchStatus = true;
                *retrievedPrefetchIndex = searchTagIndex;
                *retrievedPrefetchStream = searchInPrefetchQueue;
                //RemoveTagsFromPrefetch(&cursorPtr->cacheLevelPtr->prefetchQueue[searchQueueIndex], searchTagIndex);
                return retrievePrefetchData;
            }
            else {
                *prefetchTagSearchStatus = false;
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
    TprefetchDS prefetchDataRemoved;
    uint16_t lruPrefetchStreamIndex = 0;



    uint32_t tagToInsert = (tag << cursorPtr->cacheLevelPtr->numOfIndexBits) | (index + 1);

    prefetchData.tag = tagToInsert;
    prefetchData.data = 0;
    if(cursorPtr->cacheLevelPtr->prefetchAvailable == ePrefetchPresent) {
        lruPrefetchStreamIndex = FindLRUPrefetch(cursorPtr);
        cursorPtr->cacheLevelPtr->prefetchQueue[lruPrefetchStreamIndex].validBit = true;
        for(prefetchIndex = 0; prefetchIndex < cursorPtr->cacheLevelPtr->numOfBlocksPerStream; prefetchIndex++) {

            QueueRead(&cursorPtr->cacheLevelPtr->prefetchQueue[lruPrefetchStreamIndex], (uint8_t*)&prefetchDataRemoved);
        }
        cursorPtr->cacheLevelPtr->prefetchQueue[lruPrefetchStreamIndex].head = 0;
        for(prefetchIndex = 0; prefetchIndex < cursorPtr->cacheLevelPtr->numOfBlocksPerStream; prefetchIndex++) {

            QueueAppend(&cursorPtr->cacheLevelPtr->prefetchQueue[lruPrefetchStreamIndex], (uint8_t*)&prefetchData);

            prefetchData.tag++;
        }
        UpdatePrefetchLRU(cursorPtr, lruPrefetchStreamIndex);
        if(cursorPtr->cacheLevelPtr->prefetchQueue[lruPrefetchStreamIndex].dataPtr[1].tag == 0) {
            uint32_t count = 0;
        }
    }
}



void UpdatePrefetchLRU(TLinkedListNode *headPtr, uint32_t lruPrefetchStreamIndex) {

    TLinkedListNode *cursorPtr = headPtr;

    // When Cache hit occurs make sure the Index where the tag value was found
    // must be reset and other values lesser than that in terms of usefulness must be incremented.
    for(uint8_t searchLRU = 0; searchLRU < cursorPtr->cacheLevelPtr->numOfStreams; searchLRU++) {

        // Increment the counter values of other ways(counter) to monitor LRU.
        if(cursorPtr->cacheLevelPtr->prefetchQueue[searchLRU].lruIndex  <
            cursorPtr->cacheLevelPtr->prefetchQueue[lruPrefetchStreamIndex].lruIndex) {

            cursorPtr->cacheLevelPtr->prefetchQueue[searchLRU].lruIndex  += 1;

            }
    }
    cursorPtr->cacheLevelPtr->prefetchQueue[lruPrefetchStreamIndex].lruIndex = 0;
}

uint32_t FindLRUPrefetch(TLinkedListNode *headPtr) {
    TLinkedListNode *cursorPtr = headPtr;
    uint32_t prefetchStreamIndex = 0;
    uint32_t lruIndex = 0;
    uint32_t maxlruIndexValue = 0;
    if(cursorPtr->cacheLevelPtr->prefetchAvailable == ePrefetchPresent) {
    maxlruIndexValue = cursorPtr->cacheLevelPtr->prefetchQueue[0].lruIndex;

    for(prefetchStreamIndex = 0; prefetchStreamIndex < cursorPtr->cacheLevelPtr->numOfStreams; prefetchStreamIndex++) {

        if(cursorPtr->cacheLevelPtr->prefetchQueue[prefetchStreamIndex].lruIndex > maxlruIndexValue) {
            lruIndex = prefetchStreamIndex;
            maxlruIndexValue = cursorPtr->cacheLevelPtr->prefetchQueue[0].lruIndex;
        }
    }
    return lruIndex;
    }
}
void FindMRUPrefetch(TLinkedListNode *headPtr, uint8_t *mruArray) {
    TLinkedListNode *cursorPtr = headPtr;

    //uint8_t *mruIndexArray = (uint8_t*)malloc(cursorPtr->cacheLevelPtr->numOfStreams * (sizeof(uint8_t)));
    uint32_t prefetchStreamIndex1 = 0;
    uint32_t prefetchStreamIndex2 = 0;
    if(cursorPtr->cacheLevelPtr->prefetchAvailable == ePrefetchPresent) {
        uint32_t streamSearchIndex = 0;
        for(prefetchStreamIndex1 = 0; prefetchStreamIndex1 < cursorPtr->cacheLevelPtr->numOfStreams; prefetchStreamIndex1++) {

            for(prefetchStreamIndex2 = 0; prefetchStreamIndex2 < cursorPtr->cacheLevelPtr->numOfStreams; prefetchStreamIndex2++) {

                if(cursorPtr->cacheLevelPtr->prefetchQueue[prefetchStreamIndex2].lruIndex == streamSearchIndex) {
                    mruArray[prefetchStreamIndex1] = prefetchStreamIndex2;

                }
            }streamSearchIndex++;

        }
    }
    //return &mruIndexArray;
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

void RetrieveTagFromPrefetch(TLinkedListNode *headPtr, uint32_t tagFoundInPrefetchStream, uint32_t tagFoundInPrefetchIndex, uint32_t index , TprefetchDS *retrievedTag) {
    TLinkedListNode *cursorPtr = headPtr;

    bool peekByIndexStatus = QueuePeekByIndex(&cursorPtr->cacheLevelPtr->prefetchQueue[tagFoundInPrefetchStream], (uint16_t *)retrievedTag,
                                tagFoundInPrefetchIndex);

    TprefetchDS prefetchStreamLastTagValue;
    bool peekLastBlockStatus = QueuePeek(&cursorPtr->cacheLevelPtr->prefetchQueue[tagFoundInPrefetchStream],
                                        (uint8_t *)&prefetchStreamLastTagValue);
    *retrievedTag = prefetchStreamLastTagValue;
    QueueRemoveUntilIndex(&cursorPtr->cacheLevelPtr->prefetchQueue[tagFoundInPrefetchStream],  tagFoundInPrefetchIndex);
    UpdatePrefetchLRU(cursorPtr,tagFoundInPrefetchStream);


}
