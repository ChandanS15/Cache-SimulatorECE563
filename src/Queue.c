
#include "../include/Queue.h"
#include <stdlib.h>
#include <string.h>


//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheDSInit
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------

bool QueueInit(TQueueRecord* queuePtr, uint32_t itemSize, uint32_t queueSize)
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
    else
    {
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
            /*if(queuePtr->head == queuePtr->size)
            {
                queuePtr->head = 0;
            }*/

            //Increment count
            queuePtr->count++;
            returnStatus = true;
        }
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
            queueOffset = queuePtr->tail * queuePtr->itemSize;

            //Move the queue entry onto the required memory location
            memcpy((void*)itemPtr, (void*)((queuePtr->dataPtr) + queueOffset), queuePtr->itemSize);

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
                memcpy((void*)itemPtr, (void*)&(queuePtr->dataPtr[queueOffset]), queuePtr->itemSize);

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
bool QueueRemoveUntilIndex(TQueueRecord* queuePtr,  uint32_t index)
{
    bool returnStatus = true, queueEmpty;

    // Check for NULL pointers
    if (queuePtr == NULL )
    {
        returnStatus = false;
    }
    else
    {
        // Check if queue is empty
        queueEmpty = QueueIsEmpty(queuePtr);
        if (queueEmpty == true)
        {
            returnStatus = false;
        }
        else
        {
            // Ensure the index is valid
            if (index >= queuePtr->count)
            {
                returnStatus = false;  // Invalid index, can't remove more than current count
            }
            else
            {
                // Remove all elements up to the specified index
                uint32_t itemsToRemove = index ; // Including the element at the specified index

                for (uint32_t i = 0; i < itemsToRemove; i++)
                {
                    // Remove the element from the current head position

                    // Move the tail pointer forward
                    queuePtr->tail++;

                    // Wrap the tail index if necessary
                    if (queuePtr->tail == queuePtr->size)
                    {
                        queuePtr->tail = 0;
                    }

                    // Decrement count
                    queuePtr->count--;
                }

                returnStatus = true;
            }
        }
    }

    return returnStatus;
}

