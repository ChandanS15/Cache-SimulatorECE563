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
#include "../include/Cache.h"

//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheDSInit
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------
void CacheDSInit(TCacheDS* cacheDSPtr, char* name, uint32_t blockSize, uint32_t totalCacheSize, uint32_t cacheAssoc,
                 uint8_t numofBufPerPrefetchStream, uint8_t numOfBlocksPerStreams) {

    cacheDSPtr->name                 = name;
    cacheDSPtr->blockSize            = blockSize;
    cacheDSPtr->size                 = totalCacheSize;
    cacheDSPtr->assoc                = cacheAssoc;
    cacheDSPtr->numOfStreams         = numofBufPerPrefetchStream;
    cacheDSPtr->numOfBlocksPerStream = numOfBlocksPerStreams;

    // Calcaulte number of sets depending ont he associativity, blockSize and Total size of the cache.
    cacheDSPtr->numOfSets = (cacheDSPtr->size) / ( cacheDSPtr->assoc * cacheDSPtr->blockSize);

    // Compute number of bits required to represent a set in cache and blockoffset to get  a particular byte, word, half-word.
    cacheDSPtr->numOfBlockOffsetBits = (uint8_t)LOG2(cacheDSPtr->blockSize);
    cacheDSPtr->numOfIndexBits       = (uint8_t)LOG2(cacheDSPtr->numOfSets);
    cacheDSPtr->numOfTagBits         = (uint8_t)ADDRESSING_WIDTH - cacheDSPtr->numOfIndexBits - cacheDSPtr->numOfBlockOffsetBits;
    cacheDSPtr->prefetchAvailable = ePrefetchAbsent;

    if(cacheDSPtr->numOfStreams > 0) {
        cacheDSPtr->prefetchAvailable = ePrefetchPresent;
    }

    cacheDSPtr->cacheStatistics.hitCount = 0;
    cacheDSPtr->cacheStatistics.missCount = 0;
    cacheDSPtr->cacheStatistics.readCount = 0;
    cacheDSPtr->cacheStatistics.writeCount = 0;
    cacheDSPtr->cacheStatistics.readMissCount = 0;
    cacheDSPtr->cacheStatistics.writeMissCount = 0;
    cacheDSPtr->cacheStatistics.writeBackCount = 0;

    cacheDSPtr->prefetchStatistics.missCount = 0;
    cacheDSPtr->prefetchStatistics.prefetchCount = 0;
    cacheDSPtr->prefetchStatistics.readCount = 0;
    cacheDSPtr->prefetchStatistics.hitCount = 0;
    cacheDSPtr->totalMemoryTraffic = 0;

                 }

//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheInit
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------

void CacheInit(TLinkedListNode *headPtr) {

    CacheSetAndTagInit(headPtr);


}

//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheSetAndTagInit
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------
void CacheSetAndTagInit(TLinkedListNode *headPtr) {

    // Allocation of Cache Set, number of set depends on the calculation
    // #sets = (totalSizeOfCache/(associativity * blockSize))
    CacheAllocateMemory(headPtr);

}
//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheDeallocateMemory
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------
void CacheDeallocateMemory(TLinkedListNode *headPtr) {

    TLinkedListNode *cursorPtr;

    cursorPtr = headPtr;

    while(cursorPtr != NULL) {

        for(int currentSetIndex = 0; currentSetIndex < cursorPtr->cacheLevelPtr->numOfSets; currentSetIndex++) {
            free(cursorPtr->cacheLevelPtr->cacheSetDS[currentSetIndex].cacheTagDS);
        }

    free(cursorPtr->cacheLevelPtr->cacheSetDS);
        cursorPtr = cursorPtr->nextPtr;
    }

    //printf("All the allocated memory has been deallocated.\n");
}
//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheAllocateMemory
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------
void CacheAllocateMemory(TLinkedListNode *headPtr) {
    TLinkedListNode *cursorPtr;
    bool queueInitReturnStatus = false;

    cursorPtr  = headPtr;
    while(cursorPtr != NULL) {
        // Dyanmic Allocation of memory depends on number of sets which again depends on
        // blockSize, associativty and total size of the cache to be designed.

        cursorPtr->cacheLevelPtr->cacheSetDS = (struct _cacheSet*)calloc(cursorPtr->cacheLevelPtr->numOfSets, sizeof(struct _cacheSet));

        // Check if the memory has been allocated to the sets i.e.the actual cache memory.

        if(cursorPtr->cacheLevelPtr->cacheSetDS == NULL) {
            printf("Memory allocation of cache sets failed\n");
            exit(0);
        }
        else {
#ifdef DEBUG_AVAILABLE
            printf("Memory allocation of Cache Sets was Successful\n");
#endif
            for(uint16_t currentSetIndex = 0; currentSetIndex < cursorPtr->cacheLevelPtr->numOfSets; currentSetIndex++) {
                cursorPtr->cacheLevelPtr->cacheSetDS[currentSetIndex].cacheTagDS = (struct _cacheTag*)calloc(cursorPtr->cacheLevelPtr->assoc, sizeof(struct _cacheTag));

                // Check if memory is allocated to Cache Tags.
                if(cursorPtr->cacheLevelPtr->cacheSetDS[currentSetIndex].cacheTagDS == NULL) {
                    printf("Memory allocation of cache tag strcuture block failed\n");
                    exit(0);
                }
#ifdef DEBUG_AVAILABLE
                else {
                    printf("Memory allocation of %s Cache Tags was Successful\n", cursorPtr->cacheLevelPtr->name);
                }
#endif
                // Initialise the values of counter in ascending order to make sure the implementation of LRU is simplified.
                for(uint16_t currentWayIndex = 0; currentWayIndex < cursorPtr->cacheLevelPtr->assoc; currentWayIndex++) {

                    // Initialising counter values in ascending order just to make sure LRU implementation is
                    // easier and applied when a block is replaced.
                    cursorPtr->cacheLevelPtr->cacheSetDS[currentSetIndex].cacheTagDS[currentWayIndex].counter = currentWayIndex  ;
                    // This initialisation of memory helps in perfroming LRU later.

                }
            }

            // If prefetch is availbal for the particular cache level, memory has to be allocated for number of streams and the depth of each stream
            // respectively.
            if(cursorPtr->cacheLevelPtr->prefetchAvailable == ePrefetchPresent) {

                // Allocate queue memory based on required streams.
                cursorPtr->cacheLevelPtr->prefetchQueue = (struct _queue_tag*)malloc(cursorPtr->cacheLevelPtr->numOfStreams *sizeof(struct _queue_tag));
                // Check if the memory allocated is pointing to NULL.
                for(uint16_t prefetchStreamIndex = 0 ; prefetchStreamIndex < cursorPtr->cacheLevelPtr->numOfStreams; prefetchStreamIndex++) {
                    if(cursorPtr->cacheLevelPtr->prefetchQueue == NULL) {
                        printf("Memory allocation of Prefetch Stream %d failed.\n", prefetchStreamIndex);
                        exit(0);
                    }
                }
#ifdef DEBUG_AVAILABLE
                else {
                    printf("Memory allocation of %s Prefetch Streams was Successful\n", cursorPtr->cacheLevelPtr->name);
                }
#endif
                // Initialise the queue with default values and the data to be stored in queue is a structure with memebers
                // data, tag and valid bit.
                TprefetchDS tempPrefetchDS;
                tempPrefetchDS.data =0;
                tempPrefetchDS.tag =0;
                for(uint16_t prefetchStreamIndex = 0; prefetchStreamIndex < cursorPtr->cacheLevelPtr->numOfStreams; prefetchStreamIndex++ ) {
                    // Initialising each queue and the number of queues depends on the prefetch streams required.
                    queueInitReturnStatus = QueueInit(&cursorPtr->cacheLevelPtr->prefetchQueue[prefetchStreamIndex] , sizeof(struct _prefetchTag), cursorPtr->cacheLevelPtr->numOfBlocksPerStream, prefetchStreamIndex);
                    if(!queueInitReturnStatus) {
                        printf("Memory Allocation of queue failed.\n");
                        exit(0);
                        // } else {
                        //     for(uint16_t prefetchBlockindex =0; prefetchBlockindex < cursorPtr->cacheLevelPtr->numOfBlocksPerStream; prefetchBlockindex++) {
                        //         tempPrefetchDS.data= prefetchBlockindex + 1;
                        //         tempPrefetchDS.tag = prefetchBlockindex + 1;
                        //         bool queueAppend = QueueAppend(&cursorPtr->cacheLevelPtr->prefetchQueue[prefetchStreamIndex], (uint8_t*)&tempPrefetchDS);
                        //         if(queueAppend == false) {
                        //             printf("Memory Allocation of queue %d Stream, %d block failed.\n",prefetchStreamIndex, prefetchBlockindex);
                        //         }
                        //     }
                        // }
#ifdef DEBUG_AVAILABLE
                    else {
                        printf("Memory allocation of %s Prefetch Streams and Buffer was Successful\n",cursorPtr->cacheLevelPtr->name);
                    }
#endif
                    }
                }
#ifdef DEBUG_AVAILABLE
                printf("Memory Allocation of Cache %s is successful.\n", cursorPtr->cacheLevelPtr->name);
#endif

            }
            cursorPtr = cursorPtr->nextPtr;
        }

    }
}
