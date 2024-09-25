//------------------------------------------------------------------------------------------------------------------
// AUTHOR NAME - Chandan Srinivas
// DATE        - 30 August 2024
// NAME        - Prefetch.h
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
//*************************************************************************************************************** */


#ifndef PREFETCH_H
#define PREFETCH_H


#include "LinkedList.h"
#include "Queue.h"




typedef struct _prefetchTag TprefetchDS;

struct _prefetchTag{
    uint32_t      tag;
    uint32_t      data;
};

typedef struct _prefetchStream TPrefetchStreamDS;

struct _prefetchStream {
    struct _prefetchTag prefetchTag;
};




TprefetchDS *SearchTagInPrefetch(TLinkedListNode *headPtr, uint32_t tag, bool *prefetchTagSearchStatus, uint32_t *retrievedPrefetchIndex, uint32_t *retrievedPrefetchStream);

void PrefetchTagLoad(TLinkedListNode *headPtr);
void FillPrefetchBuffer(TLinkedListNode *headPtr, uint32_t index, uint32_t tag);
uint32_t FindLRUPrefetch(TLinkedListNode *headPtr);

#endif