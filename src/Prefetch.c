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


//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheDSInit
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------
TprefetchDS *SearchTagInPrefetch(TLinkedListNode *headPtr, uint32_t index, uint32_t tag, bool *prefetchTagSearchStatus, uint32_t *retrievedPrefetchIndex) {

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
                //RemoveTagsFromPrefetch(&cursorPtr->cacheLevelPtr->prefetchQueue[searchQueueIndex], searchTagIndex);
                return retrievePrefetchData;
            }
        }
    }
    *prefetchTagSearchStatus = false;
    return NULL;
}

//void FillPrefetechbuffer(TLinkedListNode *headPtr, )
