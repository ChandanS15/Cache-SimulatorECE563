
#include "../include/Queue.h"
#include <stdlib.h>
#include <string.h>


//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheDSInit
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------

bool QueueInit(TQueueRecord* queuePtr, uint32_t itemSize, uint32_t queueSize, uint16_t prefetchStreamIndex)
{
    bool returnStatus;
    //Check for NULL pointers
    //if((queuePtr == NULL) || (dataPtr == NULL))
    if((queuePtr == NULL))
    {
        returnStatus = false;
    }
    else
    {
        //Initialize queue members
        queuePtr->itemSize = itemSize;
        queuePtr->size = queueSize;
        queuePtr->dataPtr = (TprefetchDS*)malloc(sizeof(struct _prefetchTag) * queuePtr->size);
        queuePtr->head = 0u;
        queuePtr->tail = 0u;
        queuePtr->count = 0u;
        queuePtr->validBit = false;
        queuePtr->lruIndex = prefetchStreamIndex;

        returnStatus = true;
    }
    //TO DO: Check for invalid parameters & update return value accordingly
    return returnStatus;

} // End of QueueInit()

//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheDSInit
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------
bool QueueReset(TQueueRecord* queuePtr)
{
    bool returnStatus;

    //Check for NULL pointer
    if(queuePtr == NULL)
    {
        returnStatus = false;
    }
    else
    {
        //Reset the queue members
        queuePtr->head = 0u;
        queuePtr->tail = 0u;
        queuePtr->count = 0u;


        returnStatus = true;
    }

    return returnStatus;

} // End of QueueReset()

//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - QueueAppend
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------
bool QueueAppend(TQueueRecord* queuePtr, uint8_t *itemPtr)
{
    bool returnStatus, queueFull;

    //Check for NULL pointers
    if((queuePtr == NULL) || (itemPtr == NULL))
    {
        returnStatus = false;
    }
    else {
        //Check if queue is full
        queueFull = QueueIsFull(queuePtr);
        if(queueFull == true)
        {
            returnStatus = false;
        }
        else if(queueFull == false)
        {
            //Append the item onto the queue
            memcpy((void*)&(queuePtr->dataPtr[queuePtr->head]), (void*)itemPtr, queuePtr->itemSize);

            //memcpy((void*)(queuePtr->dataPtr[queuePtr->head]), (void*)itemPtr, queuePtr->itemSize);

            //Increment head index i.e items in the queue
            queuePtr->head++;
            //Wrap head index if head is equal to size
            if(queuePtr->head == queuePtr->size)
            {
                queuePtr->head = 0;
            }

            //Increment count
            queuePtr->count++;
            returnStatus = true;
        }
        //}
    }
    return returnStatus;

} // End of QueueAppend()


//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - QueueRead
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------
bool QueueRead(TQueueRecord* queuePtr, uint8_t *itemPtr)
{
    bool returnStatus, queueEmpty;
    uint32_t queueOffset;

    //Check for NULL pointers
    if((queuePtr == NULL) || (itemPtr == NULL))
    {
        returnStatus = false;
    }
    else
    {
        //Check if queue is empty
        queueEmpty = QueueIsEmpty(queuePtr);
        if(queueEmpty == true)
        {
            returnStatus = false;
        }
        else
        {

            //Compute the offset where data must be read from queue
            queueOffset = queuePtr->tail * queuePtr->itemSize;



            //Move the queue entry onto the required memory location
            memcpy((void*)itemPtr, (void*)((queuePtr->dataPtr) + queueOffset), queuePtr->itemSize);

            //Increment tail index
            queuePtr->tail++;
            //Wrap tail index if tail is equal to size
            if(queuePtr->tail == queuePtr->size)
            {
                queuePtr->tail = 0;
            }

            //Decrement queue count
            queuePtr->count--;

            returnStatus = true;
        }
    }

    return returnStatus;
} // End of QueueRead()


//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheDSInit
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------
bool QueuePeek(TQueueRecord* queuePtr, uint8_t *itemPtr)
{
    bool returnStatus, queueEmpty;
    uint32_t queueOffset;

    //Check for NULL pointers
    if((queuePtr == NULL) || (itemPtr == NULL))
    {
        returnStatus = false;
    }
    else
    {
        //Check if queue is empty
        queueEmpty = QueueIsEmpty(queuePtr);
        if(queueEmpty == true)
        {
            returnStatus = false;
        }
        else
        {
            //Compute the offset where data must be read from queue
            queueOffset = queuePtr->tail ;//* queuePtr->itemSize;

            //Move the queue entry onto the required memory location
            memcpy((void*)itemPtr, (void*)(&queuePtr->dataPtr[queueOffset]), queuePtr->itemSize);

            returnStatus = true;
        }
    }

    return returnStatus;
} // End of QueuePeek()

//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheDSInit
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------
bool QueueIsFull(TQueueRecord* queuePtr)
{
    bool returnStatus;

    //Check if queue is full
    if(queuePtr->count == queuePtr->size)
    {
        returnStatus = true;
    }
    else
    {
        returnStatus = false;
    }

    return returnStatus;

} // End of QueueIsFull()


//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheDSInit
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------
bool QueueIsEmpty(TQueueRecord* queuePtr)
{
    bool returnStatus;

    //Check if queue is empty
    if(queuePtr->count == 0)
    {
        returnStatus = true;
    }
    else
    {
        returnStatus = false;
    }

    return returnStatus;

} // End of QueueIsEmpty()


//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheDSInit
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------
uint32_t QueueGetCount(TQueueRecord* queuePtr)
{
    uint32_t numOfItemsInQueue;

    //Check for NULL pointers
    if(queuePtr == NULL)
    {
        numOfItemsInQueue = 0;
    }
    //Return number of items present in queue
    else
    {
        numOfItemsInQueue = queuePtr->count;
    }

    return numOfItemsInQueue;

} // End of QueueGetCount()
//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - QueuePeekByIndex
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------
bool QueuePeekByIndex(TQueueRecord* queuePtr, uint16_t *itemPtr, uint16_t peekIndex)
{
    bool returnStatus = false, queueEmpty;
    uint32_t queueOffset =  0;
    //Check for NULL pointers
    if((queuePtr == NULL) || (itemPtr == NULL))
    {
        returnStatus = false;
    }
    else
    {
        //Check if queue is empty
        queueEmpty = QueueIsEmpty(queuePtr);
        if(queueEmpty == true)
        {
            returnStatus = false;
        }
        else
        {
            queueOffset = queuePtr->tail + peekIndex;
            if(peekIndex < queuePtr->count ) {

                //Move the queue entry onto the required memory location
                memcpy((void*)itemPtr, (void*)(&queuePtr->dataPtr[queueOffset]), queuePtr->itemSize);

                returnStatus = true;
            }
            else if(peekIndex > queuePtr->count ) {
               queuePtr->dataPtr[queueOffset].data = 0;
                queuePtr->dataPtr[queueOffset].tag = 0;
                memcpy((void*)itemPtr, (void*)&(queuePtr->dataPtr[queueOffset]), queuePtr->itemSize);
                returnStatus = true;
            }
        }
    }

    return returnStatus;
} // End of QueuePeekByIndex()
//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - QueuePeekByIndex
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------
bool QueueRemoveUntilIndex(TQueueRecord* queuePtr, uint32_t index)
{
    TprefetchDS * itemPtr;

    if (queuePtr == NULL || index >= queuePtr->count)
    {
        return false; // Return false if pointers are NULL or index is out of bounds
    }

    // Copy the last element to be removed to the itemPtr (element at index)
    uint32_t removeOffset = ((queuePtr->tail + index) % queuePtr->size) * queuePtr->itemSize;
    memcpy((void*)itemPtr, (void*)((queuePtr->dataPtr) + removeOffset), queuePtr->itemSize);


    // Calculate how many elements remain after the removed elements
    uint32_t remainingElements = queuePtr->count - (index + 1);

    // If there are elements to shift, proceed with shifting
    if (remainingElements > 0)
    {
        // Shift remaining elements from the current tail + index + 1 to the new position
        for (uint32_t i = 0; i < remainingElements; i++)
        {
            uint32_t currentIndex = (queuePtr->tail + index + 1 + i) % queuePtr->size;
            uint32_t newIndex = (queuePtr->tail + i) % queuePtr->size;

            // Shift each element to the new position
            memcpy((void*)&(queuePtr->dataPtr[newIndex]),
                   (void*)&(queuePtr->dataPtr[currentIndex]),
                   queuePtr->itemSize);
        }
    }


    // Update the tail pointer and count to reflect the removal
    //queuePtr->tail = (queuePtr->tail + index + 1) % queuePtr->size; // Move tail to the new position
    queuePtr->count = remainingElements; // Update the count to the number of remaining elements
    queuePtr->head = remainingElements;
    TprefetchDS itemPeekLast;
    QueuePeekByIndex(queuePtr, (uint16_t*)&itemPeekLast, (queuePtr->head -1));
    itemPeekLast.tag += 1;
    QueueAppend(queuePtr, (uint8_t*)&itemPeekLast);

    return true; // Indicate successful removal
}


