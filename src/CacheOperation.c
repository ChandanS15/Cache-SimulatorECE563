//------------------------------------------------------------------------------------------------------------------
// AUTHOR NAME - Chandan Srinivas
// DATE        - 30 August 2024
// NAME        - CacheOoperation.c
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//*************************************************************************************************************** */
#include "../include/CacheOperation.h"




//------------------------------------------------------------------------------------------------------------------
// Local function prototypes.
void ExtractAddress(TCacheDS *cacheDSPtr, uint32_t memAddress, uint32_t *tag, uint32_t *index, uint32_t *blockOffset);
void RetrieveAddress(TCacheDS *cacheDSPtr, uint32_t index, uint32_t tag, uint32_t *memAddress);
//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheLoadData
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------
bool CacheLoadData(TLinkedListNode *headPtr, uint32_t memAddress, uint32_t *loadedData) {

    // Data and memAddress to be stored.
    uint32_t index = 0;  uint32_t tag = 0;  uint32_t blockOffset = 0;
    // CursorPtr pointing to the head of LL.
    TLinkedListNode *cursorPtr = headPtr;
    // Variables to point to respective Indexes used later.
    uint16_t tagFoundIndex = 0;  uint16_t lRUIndex = 0; uint16_t tagFoundInPrefetchIndex = 0; uint16_t tagFoundInPrefetchStream = 0;
    bool isCacheSetFull = false;
    TPrefetchSearchStatus tagFoundInPrefetchStatus = eNone;
    // Default searchStatus is Miss.
    TCacheSearchStatus searchStatus = eNone;

    // Extract BlockOffset value from a given memory address.
    ExtractAddress(cursorPtr->cacheLevelPtr, memAddress, &tag, &index, &blockOffset);

    // Search if the memAddress is currently present in the cache
    // and return the status and the tag where the block is found.
    cursorPtr->cacheLevelPtr->cacheStatistics.readCount += 1;
    searchStatus = SearchTag(cursorPtr, memAddress, &tagFoundIndex, &tagFoundInPrefetchStatus, &tagFoundInPrefetchIndex, &tagFoundInPrefetchStream);

    if(searchStatus == eCacheHit) {
        // Because cache is hit Just update the LRU counter wrt to the tagFoundIndex

        if(tagFoundInPrefetchStatus == eCacheHitInPrefetch && (cursorPtr->cacheLevelPtr->prefetchAvailable == ePrefetchPresent)) {
            TprefetchDS prefetchLastTag;
            TprefetchDS newPrefetch;
            QueueRemoveUntilIndex(&cursorPtr->cacheLevelPtr->prefetchQueue[tagFoundInPrefetchStream],  tagFoundInPrefetchIndex);
            QueuePeekByIndex(&cursorPtr->cacheLevelPtr->prefetchQueue[tagFoundInPrefetchStream], (uint16_t *) &prefetchLastTag, tagFoundInPrefetchIndex);
            bool peekStatus = QueuePeek(&cursorPtr->cacheLevelPtr->prefetchQueue[tagFoundInPrefetchStream],(uint8_t*)&prefetchLastTag);
            if(peekStatus == true)
                FillPrefetchBuffer(cursorPtr, index, prefetchLastTag.tag);
        }

        cursorPtr->cacheLevelPtr->cacheStatistics.hitCount += 1;
        UpdateLRUCounters(cursorPtr, index, tagFoundIndex);
    }
    else if (searchStatus == eCacheMiss) {

        // This handles a single level of memory.

        if(cursorPtr->nextPtr == NULL) {
            cursorPtr->cacheLevelPtr->totalMemoryTraffic += 1;
        }

        // If we face cache miss follow this flow.
        cursorPtr->cacheLevelPtr->cacheStatistics.readMissCount += 1;
        uint32_t emptyTagIndex = 0;

        isCacheSetFull = IsCacheSetFull(cursorPtr, index, &emptyTagIndex);

        if(isCacheSetFull == true) {
            // If the cache is full LRU replacement
            // find the LRU block
            lRUIndex = FindLRUBlockIndex(cursorPtr, index);
            if(cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].dirty == true) {
                // IF the block to be replaced is dirty it has to be written back before replacing it with another block.
                uint32_t writeBackAddress = 0; uint32_t requestedAddress = 0;
                // Extract the index adn tag from the retrieved block address.
                uint32_t reqIndex = 0;  uint32_t reqTag = 0;  uint32_t reqBlockOffset = 0;
                RetrieveAddress(cursorPtr->cacheLevelPtr, index, cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].tag,&writeBackAddress);
                if(cursorPtr->nextPtr != NULL) {
                    WriteBack(cursorPtr, writeBackAddress);
                    requestedAddress = CacheBlockRequest(cursorPtr, memAddress);
                    ExtractAddress(cursorPtr->cacheLevelPtr, requestedAddress, &reqTag, &reqIndex, &reqBlockOffset);
                    cursorPtr->cacheLevelPtr->cacheStatistics.writeBackCount += 1;
                    if(cursorPtr->nextPtr == NULL) {
                        cursorPtr->cacheLevelPtr->totalMemoryTraffic += 1;
                    }

                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].tag = reqTag;
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].dirty = false;

                    UpdateLRUCounters(cursorPtr, index, lRUIndex);
                } else {
                    if(tagFoundInPrefetchStatus == eCacheHitInPrefetch && (cursorPtr->cacheLevelPtr->prefetchAvailable == ePrefetchPresent))  {

                        TprefetchDS retreivedTag;
                        RetrieveTagFromPrefetch(cursorPtr, tagFoundInPrefetchStream, tagFoundInPrefetchIndex, index, &retreivedTag);
                        cursorPtr->cacheLevelPtr->prefetchStatistics.hitCount += 1;
                        cursorPtr->cacheLevelPtr->cacheStatistics.hitCount += 1;
                        //cursorPtr->cacheLevelPtr->cacheStatistics.readMissCount -= 1;
                        cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].tag = retreivedTag.tag >> cursorPtr->cacheLevelPtr->numOfIndexBits;
                        //cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].dirty = true;
                        cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].dirty = false;
                        return true;

                    } else if (tagFoundInPrefetchStatus == eCacheMissInPrefetch && (cursorPtr->cacheLevelPtr->prefetchAvailable == ePrefetchPresent)){
                        requestedAddress = memAddress;
                        ExtractAddress(cursorPtr->cacheLevelPtr, requestedAddress, &reqTag, &reqIndex, &reqBlockOffset);
                        //cursorPtr->cacheLevelPtr->cacheStatistics.writeBackCount += 1;
                        if(cursorPtr->nextPtr == NULL) {
                            cursorPtr->cacheLevelPtr->totalMemoryTraffic += 1;
                        }

                        cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].tag = reqTag;
                        cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].dirty = false;
                        FillPrefetchBuffer(cursorPtr, index, reqTag);
                        cursorPtr->cacheLevelPtr->prefetchStatistics.prefetchCount += (cursorPtr->cacheLevelPtr->numOfBlocksPerStream+ 1);
                        UpdateLRUCounters(cursorPtr, index, lRUIndex);
                    }

                    requestedAddress = memAddress;
                    ExtractAddress(cursorPtr->cacheLevelPtr, requestedAddress, &reqTag, &reqIndex, &reqBlockOffset);
                    cursorPtr->cacheLevelPtr->cacheStatistics.writeBackCount += 1;
                    if(cursorPtr->nextPtr == NULL) {
                         cursorPtr->cacheLevelPtr->totalMemoryTraffic += 1;
                    }

                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].tag = reqTag;
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].dirty = false;

                    UpdateLRUCounters(cursorPtr, index, lRUIndex);
                    }
                } else { // If not dirty
                // Might have to check this out.
                // Because the block is dirty we have written it back to next level and now installing the new block at the LRU index.
                uint32_t reqIndex = 0;  uint32_t reqTag = 0;  uint32_t reqBlockOffset = 0;
                uint32_t requestedAddress = 0;
                if(cursorPtr->nextPtr != NULL) {
                    requestedAddress = CacheBlockRequest(cursorPtr, memAddress);
                    ExtractAddress(cursorPtr->cacheLevelPtr, requestedAddress, &reqTag, &reqIndex, &reqBlockOffset);
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].tag = reqTag;
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].valid = true;
                    UpdateLRUCounters(cursorPtr, index, lRUIndex);
                }else {
                    if(tagFoundInPrefetchStatus == eCacheHitInPrefetch && (cursorPtr->cacheLevelPtr->prefetchAvailable == ePrefetchPresent))  {
                        TprefetchDS retreivedTag;
                        RetrieveTagFromPrefetch(cursorPtr, tagFoundInPrefetchStream, tagFoundInPrefetchIndex, index, &retreivedTag);
                        cursorPtr->cacheLevelPtr->prefetchStatistics.hitCount += 1;
                        //cursorPtr->cacheLevelPtr->cacheStatistics.readMissCount -= 1;
                        cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].tag = retreivedTag.tag >> cursorPtr->cacheLevelPtr->numOfIndexBits;
                        cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].dirty = false;

                    } else if (tagFoundInPrefetchStatus == eCacheMissInPrefetch && (cursorPtr->cacheLevelPtr->prefetchAvailable == ePrefetchPresent)) {
                        requestedAddress = memAddress;
                        //cursorPtr->cacheLevelPtr->cacheStatistics.readMissCount += 1;
                        ExtractAddress(cursorPtr->cacheLevelPtr, requestedAddress, &reqTag, &reqIndex, &reqBlockOffset);
                        cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].tag = reqTag;
                        cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].valid = true;
                        UpdateLRUCounters(cursorPtr, index, lRUIndex);

                        FillPrefetchBuffer(cursorPtr, index, reqTag);
                        cursorPtr->cacheLevelPtr->prefetchStatistics.prefetchCount += (cursorPtr->cacheLevelPtr->numOfBlocksPerStream+ 1);

                    }

                    requestedAddress = memAddress;
                    ExtractAddress(cursorPtr->cacheLevelPtr, requestedAddress, &reqTag, &reqIndex, &reqBlockOffset);
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].tag = reqTag;
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].valid = true;
                    UpdateLRUCounters(cursorPtr, index, lRUIndex);

                }
            }

        } else if(isCacheSetFull == false) {
            // if the cache is not full just install the block into the empty tag after requesting the data from the next level.
            uint32_t requestedAddress = 0;
            // Extract the index adn tag from the retrieved block address.
            uint32_t reqIndex = 0;  uint32_t reqTag = 0;  uint32_t reqBlockOffset = 0;

            // Request the mem address present in the next level of hierarchy.
            if(cursorPtr->nextPtr != NULL) {
                requestedAddress = CacheBlockRequest(cursorPtr, memAddress);
                ExtractAddress(cursorPtr->cacheLevelPtr, requestedAddress, &reqTag, &reqIndex, &reqBlockOffset);
                cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[emptyTagIndex].tag = reqTag;
                cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[emptyTagIndex].valid = true;
                UpdateLRUCounters(cursorPtr, index, emptyTagIndex);
            } else {
                if(tagFoundInPrefetchStatus == eCacheHitInPrefetch && (cursorPtr->cacheLevelPtr->prefetchAvailable == ePrefetchPresent))  {
                    TprefetchDS retreivedTag;
                    RetrieveTagFromPrefetch(cursorPtr, tagFoundInPrefetchStream, tagFoundInPrefetchIndex, index, &retreivedTag);
                    cursorPtr->cacheLevelPtr->prefetchStatistics.hitCount += 1;
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].tag = retreivedTag.tag >> cursorPtr->cacheLevelPtr->numOfIndexBits;
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].dirty = true;

                } else if (tagFoundInPrefetchStatus == eCacheMissInPrefetch && (cursorPtr->cacheLevelPtr->prefetchAvailable == ePrefetchPresent)) {
                    requestedAddress = memAddress;
                    cursorPtr->cacheLevelPtr->prefetchStatistics.prefetchCount += 1;
                    ExtractAddress(cursorPtr->cacheLevelPtr, requestedAddress, &reqTag, &reqIndex, &reqBlockOffset);
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[emptyTagIndex].tag = reqTag;
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[emptyTagIndex].valid = true;
                    UpdateLRUCounters(cursorPtr, index, emptyTagIndex);
                    FillPrefetchBuffer(cursorPtr, index, reqTag);
                    cursorPtr->cacheLevelPtr->prefetchStatistics.prefetchCount += (cursorPtr->cacheLevelPtr->numOfBlocksPerStream+ 1);
                }
                requestedAddress = memAddress;
                ExtractAddress(cursorPtr->cacheLevelPtr, requestedAddress, &reqTag, &reqIndex, &reqBlockOffset);
                cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[emptyTagIndex].tag = reqTag;
                cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[emptyTagIndex].valid = true;
                UpdateLRUCounters(cursorPtr, index, emptyTagIndex);

            }
        }
    }
}
//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheStoreData
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------
bool CacheStoreData(TLinkedListNode *headPtr, uint32_t memAddress, uint32_t dataToBeStored) {

    // Data and memAddress to be stored.
    uint32_t index = 0;  uint32_t tag = 0;  uint32_t blockOffset = 0;
    // CursorPtr pointing to the head of LL.
    TLinkedListNode *cursorPtr = headPtr; uint16_t lRUIndex = 0;
    // Variables to point to respective Indexes used later.
    uint16_t tagFoundIndex = 0;  uint16_t victimTagIndex = 0; uint16_t tagFoundInPrefetchIndex = 0; uint16_t tagFoundInPrefetchStream = 0;

    // Default searchStatus is Miss.
    TCacheSearchStatus searchStatus = eNone;
    bool isCacheSetFull = false;
    TPrefetchSearchStatus tagFoundInPrefetchStatus = eNone;

    // Extract BlockOffset value from a given memory address.
    ExtractAddress(cursorPtr->cacheLevelPtr, memAddress, &tag, &index, &blockOffset);

    // Search if the memAddress is currently present in the cache
    // and return the status and the tag where the block is found.
    searchStatus = SearchTag(cursorPtr, memAddress, &tagFoundIndex, &tagFoundInPrefetchStatus, &tagFoundInPrefetchIndex, &tagFoundInPrefetchStream);

    cursorPtr->cacheLevelPtr->cacheStatistics.writeCount += 1;

    if(searchStatus == eCacheHit) {

        if(tagFoundInPrefetchStatus == eCacheHitInPrefetch && (cursorPtr->cacheLevelPtr->prefetchAvailable == ePrefetchPresent)) {
            TprefetchDS prefetchLastTag;
            TprefetchDS newPrefetch;
            QueueRemoveUntilIndex(&cursorPtr->cacheLevelPtr->prefetchQueue[tagFoundInPrefetchStream],  tagFoundInPrefetchIndex);
            QueuePeekByIndex(&cursorPtr->cacheLevelPtr->prefetchQueue[tagFoundInPrefetchStream], (uint16_t *) &prefetchLastTag, tagFoundInPrefetchIndex);
            bool peekStatus = QueuePeek(&cursorPtr->cacheLevelPtr->prefetchQueue[tagFoundInPrefetchStream],(uint8_t*)&prefetchLastTag);
            if(peekStatus == true)
                FillPrefetchBuffer(cursorPtr, index, (prefetchLastTag.tag>> cursorPtr->cacheLevelPtr->numOfIndexBits));
        }
        cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[tagFoundIndex].tag = tag;
        cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[tagFoundIndex].valid = true;
        cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[tagFoundIndex].dirty = true;
        UpdateLRUCounters(cursorPtr, index, tagFoundIndex);
        cursorPtr->cacheLevelPtr->cacheStatistics.hitCount += 1;
    } else if(searchStatus == eCacheMiss) {

        if(cursorPtr->nextPtr == NULL) {
            cursorPtr->cacheLevelPtr->totalMemoryTraffic += 1;
        }
        // would have to move this or subtract one in the future to acocunt for read in prefetch

        cursorPtr->cacheLevelPtr->cacheStatistics.writeMissCount += 1;
        uint32_t emptyTagIndex = 0;
        isCacheSetFull = IsCacheSetFull(cursorPtr, index, &emptyTagIndex);

        if(isCacheSetFull == true) {
            // If the set is completely filled, search for the lru INdex to be replaced.
            lRUIndex = FindLRUBlockIndex(cursorPtr, index);
            if(cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].dirty == true) {
                uint32_t writeBackAddress = 0; uint32_t requestedAddress = 0;
                // Extract the index adn tag from the retrieved block address.
                RetrieveAddress(cursorPtr->cacheLevelPtr, index, cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].tag,&writeBackAddress);
                if(cursorPtr->nextPtr != NULL) {
                    WriteBack(cursorPtr, writeBackAddress);
                    requestedAddress = CacheBlockRequest(cursorPtr, memAddress);
                    cursorPtr->cacheLevelPtr->cacheStatistics.writeBackCount += 1;
                    cursorPtr->cacheLevelPtr->totalMemoryTraffic += 1;
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].tag = tag;
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].dirty = true;
                    UpdateLRUCounters(cursorPtr, index, lRUIndex);
                }
                // If this is the last block
                if(tagFoundInPrefetchStatus == eCacheHitInPrefetch && (cursorPtr->cacheLevelPtr->prefetchAvailable == ePrefetchPresent))  {
                    TprefetchDS retreivedTag;
                    RetrieveTagFromPrefetch(cursorPtr, tagFoundInPrefetchStream, tagFoundInPrefetchIndex, index, &retreivedTag);
                    cursorPtr->cacheLevelPtr->prefetchStatistics.hitCount += 1;
                    cursorPtr->cacheLevelPtr->cacheStatistics.hitCount += 1;
                    //cursorPtr->cacheLevelPtr->cacheStatistics.readMissCount -= 1;
                    cursorPtr->cacheLevelPtr->cacheStatistics.writeMissCount -= 1;
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].tag = retreivedTag.tag >> cursorPtr->cacheLevelPtr->numOfIndexBits;
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].dirty = true;

                } else if (tagFoundInPrefetchStatus == eCacheMissInPrefetch && (cursorPtr->cacheLevelPtr->prefetchAvailable == ePrefetchPresent)){

                    FillPrefetchBuffer(cursorPtr, index, tag);
                    cursorPtr->cacheLevelPtr->prefetchStatistics.prefetchCount += (cursorPtr->cacheLevelPtr->numOfBlocksPerStream+ 1);
                    cursorPtr->cacheLevelPtr->prefetchStatistics.prefetchCount += cursorPtr->cacheLevelPtr->numOfBlocksPerStream;
                    // Check if I have to remove this or the above one
                    if(cursorPtr->nextPtr == NULL)
                        cursorPtr->cacheLevelPtr->cacheStatistics.writeBackCount += 1;
                    cursorPtr->cacheLevelPtr->totalMemoryTraffic += 1;
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].tag = tag;
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].dirty = true;
                    UpdateLRUCounters(cursorPtr, index, lRUIndex);
                }
                //cursorPtr->cacheLevelPtr->cacheStatistics.writeBackCount += 1;
                cursorPtr->cacheLevelPtr->totalMemoryTraffic += 1;
                cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].tag = tag;
                cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].dirty = true;
                UpdateLRUCounters(cursorPtr, index, lRUIndex);

            } else {
                // dirty bit not set just request blopck form the next level and install
                uint32_t reqIndex = 0;  uint32_t reqTag = 0;  uint32_t reqBlockOffset = 0;
                uint32_t requestedAddress = 0;
                if(cursorPtr->nextPtr != NULL) {
                    requestedAddress = CacheBlockRequest(cursorPtr, memAddress);
                    ExtractAddress(cursorPtr->cacheLevelPtr, requestedAddress, &reqTag, &reqIndex, &reqBlockOffset);
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].tag = reqTag;
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].valid = true;
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].dirty = true;
                    UpdateLRUCounters(cursorPtr, index, lRUIndex);
                }else{

                    if(tagFoundInPrefetchStatus == eCacheHitInPrefetch && (cursorPtr->cacheLevelPtr->prefetchAvailable == ePrefetchPresent))  {
                        TprefetchDS retreivedTag;
                        RetrieveTagFromPrefetch(cursorPtr, tagFoundInPrefetchStream, tagFoundInPrefetchIndex, index, &retreivedTag);
                        cursorPtr->cacheLevelPtr->prefetchStatistics.hitCount += 1;

                        cursorPtr->cacheLevelPtr->cacheStatistics.writeMissCount -= 1;
                        cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].tag = retreivedTag.tag >> cursorPtr->cacheLevelPtr->numOfIndexBits;
                        cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].dirty = true;

                    } else if (tagFoundInPrefetchStatus == eCacheMissInPrefetch && (cursorPtr->cacheLevelPtr->prefetchAvailable == ePrefetchPresent)) {

                        requestedAddress = memAddress;
                        cursorPtr->cacheLevelPtr->prefetchStatistics.prefetchCount += 1;
                        ExtractAddress(cursorPtr->cacheLevelPtr, requestedAddress, &reqTag, &reqIndex, &reqBlockOffset);
                        cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].tag = reqTag;
                        cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].valid = true;
                        cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].dirty = true;
                        FillPrefetchBuffer(cursorPtr, index, reqTag);
                        UpdateLRUCounters(cursorPtr, index, lRUIndex);
                    }
                    requestedAddress = memAddress;
                    ExtractAddress(cursorPtr->cacheLevelPtr, requestedAddress, &reqTag, &reqIndex, &reqBlockOffset);
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].tag = reqTag;
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].valid = true;
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].dirty = true;
                    UpdateLRUCounters(cursorPtr, index, lRUIndex);

                }
            }
        } else if(isCacheSetFull == false) {
            // if the cache is not full just install the block into the empty tag after requesting the data from the next level.
            uint32_t requestedAddress = 0;
            // Extract the index adn tag from the retrieved block address.
            uint32_t reqIndex = 0;  uint32_t reqTag = 0;  uint32_t reqBlockOffset = 0;
            // Request the mem address present in the next level of hierarchy.
            lRUIndex = FindLRUBlockIndex(cursorPtr, index);
            if(cursorPtr->nextPtr != NULL) {
                requestedAddress = CacheBlockRequest(cursorPtr, memAddress);
                ExtractAddress(cursorPtr->cacheLevelPtr, requestedAddress, &reqTag, &reqIndex, &reqBlockOffset);
                cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].tag = reqTag;
                cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].dirty = true;
                cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].valid = true;
                UpdateLRUCounters(cursorPtr, index, lRUIndex);
            } else {

                if(tagFoundInPrefetchStatus == eCacheHitInPrefetch && (cursorPtr->cacheLevelPtr->prefetchAvailable == ePrefetchPresent))  {
                    TprefetchDS retreivedTag;
                    RetrieveTagFromPrefetch(cursorPtr, tagFoundInPrefetchStream, tagFoundInPrefetchIndex, index, &retreivedTag);
                    cursorPtr->cacheLevelPtr->prefetchStatistics.hitCount += 1;

                    cursorPtr->cacheLevelPtr->cacheStatistics.writeMissCount -= 1;
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].tag = retreivedTag.tag >> cursorPtr->cacheLevelPtr->numOfIndexBits;
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].dirty = true;

                } else if (tagFoundInPrefetchStatus == eCacheMissInPrefetch && (cursorPtr->cacheLevelPtr->prefetchAvailable == ePrefetchPresent)) {
                    requestedAddress = memAddress;
                    cursorPtr->cacheLevelPtr->prefetchStatistics.prefetchCount += 1;
                    ExtractAddress(cursorPtr->cacheLevelPtr, requestedAddress, &reqTag, &reqIndex, &reqBlockOffset);
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].tag = reqTag;
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].dirty = true;
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].valid = true;
                    FillPrefetchBuffer(cursorPtr, index, reqTag);
                    UpdateLRUCounters(cursorPtr, index, lRUIndex);
                }
                requestedAddress = memAddress;
                ExtractAddress(cursorPtr->cacheLevelPtr, requestedAddress, &reqTag, &reqIndex, &reqBlockOffset);
                cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].tag = reqTag;
                cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].dirty = true;
                cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].valid = true;
                UpdateLRUCounters(cursorPtr, index, lRUIndex);

            }
        }
    }
}


//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - SearchTag
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------
TCacheSearchStatus SearchTag(TLinkedListNode *headPtr, uint32_t memAddress, uint16_t *tagFoundIndex,
                            TPrefetchSearchStatus *tagFoundInPrefetchStatus, uint16_t *tagFoundInPrefetchIndex, uint16_t *tagFoundInPrefetchStream) {

    TLinkedListNode * cursorPtr; TCacheSearchStatus currentSearchStatus = eNone;  uint32_t index = 0; uint32_t tag = 0;
    uint32_t blockOffset = 0;  cursorPtr = headPtr; *tagFoundInPrefetchIndex = 0; *tagFoundInPrefetchStream = 0;
if(cursorPtr != NULL) {
    // Extract BlockOffset value for given memory address.
    ExtractAddress(cursorPtr->cacheLevelPtr, memAddress, &tag, &index, &blockOffset);

    // Search for Cache hit by going through all the ways in the set.
    for(uint8_t searchTagIndex = 0; searchTagIndex < cursorPtr->cacheLevelPtr->assoc; searchTagIndex++) {

        // Search every way(associativity) of every set to determine if the memory address is present.

        // Check for Cache hit.
        if((cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[searchTagIndex].tag == tag) &&
            cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[searchTagIndex].valid == true ) {

            // Store the index of way where tag is found, so that the counter of that way can be used to implement LRU.
            *tagFoundIndex = searchTagIndex;

            // Current status is cache hit.
            currentSearchStatus = eCacheHit;
            break;
            }
        else{

            // This is currently a demand miss as we are yet to search the prefetch buffer.
            currentSearchStatus = eCacheMiss;
        }
    }

    if(cursorPtr->cacheLevelPtr->prefetchAvailable == ePrefetchPresent) {
        // When encountered with Cache Miss search in the prefetchBuffer
        TprefetchDS *retrievedPrefetchTag;
        bool prefetchTagSearchStatus;
        uint32_t retrievedPrefetchIndex;
        uint32_t retrievedPrefetchStream;

        retrievedPrefetchTag = SearchTagInPrefetch(cursorPtr, index, tag, &prefetchTagSearchStatus, &retrievedPrefetchIndex, &retrievedPrefetchStream);

        if(prefetchTagSearchStatus == false) {
            *tagFoundInPrefetchStatus = eCacheMissInPrefetch ;
            //currentSearchStatus = eCacheMiss;
        } else {
            *tagFoundInPrefetchIndex = retrievedPrefetchIndex;
            *tagFoundInPrefetchStream = retrievedPrefetchStream;
            *tagFoundInPrefetchStatus = eCacheHitInPrefetch;
            //currentSearchStatus = eCacheHit;
        }
    }

}
    return currentSearchStatus;
}

//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - SearchTag
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------
uint32_t CacheBlockRequest(TLinkedListNode *headPtr, uint32_t memAddress) {
    TLinkedListNode *cursorPtr = headPtr->nextPtr; TCacheSearchStatus searchStatus = eNone;
    // Variables to point to respective Indexes used later.
    uint16_t tagFoundIndex = 0;  uint32_t index = 0;  uint32_t tag = 0;  uint32_t blockOffset = 0;
    uint16_t lRUIndex = 0; uint32_t requestedBlock;uint16_t tagFoundInPrefetchIndex = 0; uint16_t tagFoundInPrefetchStream = 0;
    bool isCacheSetFull = false;
    TCacheSearchStatus tagFoundInPrefetchStatus = false;

    while (cursorPtr != NULL) {
        cursorPtr->cacheLevelPtr->cacheStatistics.readCount += 1;
        // Default searchStatus is Miss.
        TCacheSearchStatus searchStatus = eNone;

        // Extract BlockOffset value from a given memory address.
        ExtractAddress(cursorPtr->cacheLevelPtr, memAddress, &tag, &index, &blockOffset);

        searchStatus = SearchTag(cursorPtr, memAddress, &tagFoundIndex ,&tagFoundInPrefetchStatus, &tagFoundInPrefetchIndex, &tagFoundInPrefetchStream);
        // return the tags from the next level.
        if(searchStatus == eCacheHit) {

            cursorPtr->cacheLevelPtr->cacheStatistics.hitCount += 1;
            UpdateLRUCounters(cursorPtr,index,tagFoundIndex);
            RetrieveAddress(cursorPtr->cacheLevelPtr, index, cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[tagFoundIndex].tag, &requestedBlock);
            return requestedBlock;

        } else if(searchStatus == eCacheMiss) {
            // If the block to be returned is not in this level
            // install here and search for ir in the next level.
            cursorPtr->cacheLevelPtr->cacheStatistics.readMissCount += 1;
            cursorPtr->cacheLevelPtr->totalMemoryTraffic += 1;
            cursorPtr->cacheLevelPtr->cacheStatistics.missCount += 1;

            uint32_t emptyTagIndex = 0;

            isCacheSetFull = IsCacheSetFull(cursorPtr, index, &emptyTagIndex);

                if(isCacheSetFull == true) {
                    // If the cache is full LRU replacement
                    // find the LRU block
                    lRUIndex = FindLRUBlockIndex(cursorPtr, index);
                    if(cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].dirty == true) {
                        // Writeback because the dirty bit is set.

                        // Perform these operations with multiple levels of heirarchy.
                        if(cursorPtr->nextPtr != NULL) {
                            uint32_t writeBackAddress = 0; uint32_t requestedAddress = 0;
                            RetrieveAddress(cursorPtr->cacheLevelPtr, index, cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].tag,&writeBackAddress);
                            WriteBack(cursorPtr, writeBackAddress);
                            requestedAddress = CacheBlockRequest(cursorPtr, memAddress);
                        }


                        // If the block is not found in the L2 go to the next level to allocate and write data.
                        cursorPtr->cacheLevelPtr->cacheStatistics.writeBackCount += 1;
                        cursorPtr->cacheLevelPtr->totalMemoryTraffic += 1;
                        cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].tag = tag;
                        UpdateLRUCounters(cursorPtr, index, lRUIndex);
                        cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].dirty = false;
                        cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].valid = true;
                        RetrieveAddress(cursorPtr->cacheLevelPtr, index, cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].tag, &requestedBlock);
                        return requestedBlock;

                    } else {

                        cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].tag = tag;
                        UpdateLRUCounters(cursorPtr, index, lRUIndex);
                        RetrieveAddress(cursorPtr->cacheLevelPtr, index, cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].tag, &requestedBlock);
                        return requestedBlock;
                    }



                }else if(isCacheSetFull==false) {
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[emptyTagIndex].tag = tag;
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[emptyTagIndex].valid = true;
                    UpdateLRUCounters(cursorPtr, index, emptyTagIndex);
                    RetrieveAddress(cursorPtr->cacheLevelPtr, index, cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[emptyTagIndex].tag, &requestedBlock);
                    return requestedBlock;
                }

        }
        cursorPtr = cursorPtr->nextPtr;
    }
}






//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - ExtractAddress
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------

void ExtractAddress(TCacheDS *cacheDSPtr, uint32_t memAddress, uint32_t *tag, uint32_t *index, uint32_t *blockOffset) {

    if(cacheDSPtr != NULL) {
        *blockOffset = (uint32_t)BLOCKOFFSETEXTRACT(memAddress, cacheDSPtr->numOfBlockOffsetBits);

        // Extratc tag value from given memory Address.
        *tag         = (uint32_t)TAGEXTRACT(memAddress, (cacheDSPtr->numOfBlockOffsetBits + cacheDSPtr->numOfIndexBits),
                                 cacheDSPtr->numOfTagBits);

        // Extract index values from the given memory address.

        *index       = (uint32_t)INDEXEXTRACT(memAddress, (cacheDSPtr->numOfBlockOffsetBits), cacheDSPtr->numOfIndexBits);
    }


}

void RetrieveAddress(TCacheDS *cacheDSPtr, uint32_t index, uint32_t tag, uint32_t *memAddress) {

    uint32_t retreivedIndex = 0; uint32_t retreivedTag = 0;
    if(cacheDSPtr != NULL) {

        // Extract tag value from given memory Address.
        retreivedTag        = (uint32_t)TAGRETREIVE(tag, (cacheDSPtr->numOfBlockOffsetBits + cacheDSPtr->numOfIndexBits));

        // Extract index values from the given memory address.

        retreivedIndex       = (uint32_t)INDEXRETREIVE(index, (cacheDSPtr->numOfBlockOffsetBits));

        *memAddress = retreivedIndex | retreivedTag;
    }


}

uint32_t FindLRUBlockIndex(TLinkedListNode *headPtr, uint32_t index) {

    TLinkedListNode * cursorPtr = headPtr;  uint32_t victimTagIndex = 0; uint16_t maxCounterValue = 0;

    maxCounterValue = cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[0].counter;
    // Find the way which has highest counter value which would eventually be the victim and evicted.
    for(uint16_t searchMaxCounterIndex = 0; searchMaxCounterIndex < cursorPtr->cacheLevelPtr->assoc; searchMaxCounterIndex++) {

        // Assume counter of way 0 of current set to be the highest value and find the way whose counter has the highest value by travesring
        // across the the counters in the set and when the highest is found(this would be the LRU) and becomes the victim  block that
        // must be evicted.
        if(cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[searchMaxCounterIndex].counter > maxCounterValue) {

            maxCounterValue = cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[searchMaxCounterIndex].counter;
            // Get the index of LRU block
            victimTagIndex = searchMaxCounterIndex;
        }
    }

    return victimTagIndex;
}

void UpdateLRUCounters(TLinkedListNode *headPtr, uint32_t index, uint32_t tagFoundIndex) {

    TLinkedListNode *cursorPtr = headPtr;

    // When Cache hit occurs make sure the Index where the tag value was found
    // must be reset and other values lesser than that in terms of usefulness must be incremented.
    for(uint8_t searchLRU = 0; searchLRU < cursorPtr->cacheLevelPtr->assoc; searchLRU++) {

        // Increment the counter values of other ways(counter) to monitor LRU.
        if(cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[searchLRU].counter <
            cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[tagFoundIndex].counter) {

            cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[searchLRU].counter += 1;

            }
    }
    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[tagFoundIndex].counter = 0;
}

void WriteBack(TLinkedListNode *headPtr, uint32_t memAddress) {
    TLinkedListNode *cursorPtr = headPtr->nextPtr;
    // Data and memAddress to be stored.
    uint32_t index = 0;  uint32_t tag = 0;  uint32_t blockOffset = 0; uint16_t tagFoundIndex = 0; uint16_t tagFoundInPrefetchIndex = 0; uint16_t tagFoundInPrefetchStream = 0;
    // Default searchStatus is Miss.
    TCacheSearchStatus searchStatus = eNone;
    TPrefetchSearchStatus tagFoundInPrefetchStatus = eNone;
    bool isCacheSetFull = false; uint32_t lRUIndex = 0; uint32_t emptyTagIndex = 0;

    if(cursorPtr != NULL) {
        cursorPtr->cacheLevelPtr->cacheStatistics.writeCount += 1;
        searchStatus = SearchTag(cursorPtr, memAddress, &tagFoundIndex ,&tagFoundInPrefetchStatus, &tagFoundInPrefetchIndex, &tagFoundInPrefetchStream);

        ExtractAddress(cursorPtr->cacheLevelPtr, memAddress, &tag, &index, &blockOffset);
        if(searchStatus == eCacheHit) {
            if(tagFoundInPrefetchStatus == eCacheHitInPrefetch && (cursorPtr->cacheLevelPtr->prefetchAvailable == ePrefetchPresent)) {
                TprefetchDS prefetchLastTag;

                TprefetchDS newPrefetch;
                QueueRemoveUntilIndex(&cursorPtr->cacheLevelPtr->prefetchQueue[tagFoundInPrefetchStream],  tagFoundInPrefetchIndex);


                bool peekStatus = QueuePeek(&cursorPtr->cacheLevelPtr->prefetchQueue[tagFoundInPrefetchStream],(uint8_t*)&prefetchLastTag);

                if(peekStatus == true)
                    FillPrefetchBuffer(cursorPtr, index, (prefetchLastTag.tag>> cursorPtr->cacheLevelPtr->numOfIndexBits));
            }
            // If hit update the pre-existing block and set dirty bit
            cursorPtr->cacheLevelPtr->cacheStatistics.hitCount += 1;
            cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[tagFoundIndex].dirty   = true;
            cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[tagFoundIndex].valid   = true;
            cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[tagFoundIndex].tag   = tag;
            UpdateLRUCounters(cursorPtr, index, tagFoundIndex);
        }
        else if (searchStatus == eCacheMiss) {

            lRUIndex = FindLRUBlockIndex(cursorPtr, index);
            isCacheSetFull = IsCacheSetFull(cursorPtr, index, &emptyTagIndex);
            if(cursorPtr != NULL ) {
                if(isCacheSetFull == true) {
                    if (cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].dirty == true) {
                        cursorPtr->cacheLevelPtr->cacheStatistics.writeBackCount += 1;
                        cursorPtr->cacheLevelPtr->totalMemoryTraffic += 1;
                        if(cursorPtr->nextPtr != NULL)
                            WriteBack(cursorPtr, memAddress);
                    }
                    cursorPtr->cacheLevelPtr->cacheStatistics.writeMissCount += 1;
                    cursorPtr->cacheLevelPtr->totalMemoryTraffic += 1;
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].tag = tag;
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].valid   = true;
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[lRUIndex].dirty = true;
                    UpdateLRUCounters(cursorPtr, index, lRUIndex);
                    //cursorPtr->cacheLevelPtr->totalMemoryTraffic += 2;

                } else if(isCacheSetFull == false) {
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[emptyTagIndex].tag = tag;
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[emptyTagIndex].valid   = true;
                    cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[emptyTagIndex].dirty = true;
                    UpdateLRUCounters(cursorPtr, index, emptyTagIndex);
                }
            }
        }
    }
}


bool IsCacheSetFull(TLinkedListNode *headPtr, uint32_t index, uint32_t *tagEmptyIndex) {

    TLinkedListNode *cursorPtr = headPtr;
    bool returnStatus = true;
    uint32_t cacheTagIndex = 0;

    for(cacheTagIndex = 0; cacheTagIndex < cursorPtr->cacheLevelPtr->assoc; cacheTagIndex++) {

        if(cursorPtr->cacheLevelPtr->cacheSetDS[index].cacheTagDS[cacheTagIndex].valid == false) {
            returnStatus = false;
            *tagEmptyIndex = cacheTagIndex;
            break;
        }
    }
    return returnStatus;

}
