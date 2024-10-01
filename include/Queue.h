#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "Cache.h"


typedef struct _queue_tag
{
    bool validBit;
    uint16_t lruIndex;
    uint32_t head;              ///< Queue's head index (next write position)
    uint32_t tail;              ///< Queue's tail index (next read position)
    uint32_t count;             ///< Queue's count - the number of items in the queue
    uint32_t size;              ///< Queue's size - the maximum number of items the queue can hold
    uint32_t itemSize;          ///< Queue's item size
    struct _prefetchTag  *dataPtr;          ///< Pointer to the queue's data storage
} TQueueRecord;

//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheDSInit
// Note          - 
// Return        - 
//------------------------------------------------------------------------------------------------------------------
bool QueueInit(TQueueRecord* queuePtr, uint32_t itemSize, uint32_t queueSize, uint16_t prefetchStreamIndex);

//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheDSInit
// Note          - 
// Return        - 
//------------------------------------------------------------------------------------------------------------------
bool QueueReset(TQueueRecord* queuePtr);

//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheDSInit
// Note          - 
// Return        - 
//------------------------------------------------------------------------------------------------------------------
bool QueueAppend(TQueueRecord* queuePtr, uint8_t *itemPtr);

//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheDSInit
// Note          - 
// Return        - 
//------------------------------------------------------------------------------------------------------------------
bool QueueRead(TQueueRecord* queuePtr, uint8_t *itemPtr);

//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheDSInit
// Note          - 
// Return        - 
//------------------------------------------------------------------------------------------------------------------
bool QueuePeek(TQueueRecord* queuePtr, uint8_t *itemPtr);

//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheDSInit
// Note          - 
// Return        - 
//------------------------------------------------------------------------------------------------------------------
bool QueueIsFull(TQueueRecord* queuePtr);

//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheDSInit
// Note          - 
// Return        - 
//------------------------------------------------------------------------------------------------------------------
bool QueueIsEmpty(TQueueRecord* queuePtr);

//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheDSInit
// Note          - 
// Return        - 
//------------------------------------------------------------------------------------------------------------------
uint32_t QueueGetCount(TQueueRecord* queuePtr);
bool QueueReadHead(TQueueRecord* queuePtr, uint8_t *itemPtr);
bool QueueAppendTail(TQueueRecord* queuePtr, uint8_t *itemPtr);
bool QueuePeekByIndex(TQueueRecord* queuePtr, uint16_t *itemPtr, uint16_t peekIndex);
bool QueueRemoveUntilIndex(TQueueRecord* queuePtr, uint32_t index);
bool QueueAppendByIndex(TQueueRecord* queuePtr, uint16_t *itemPtr, uint16_t appendIndex);
#endif