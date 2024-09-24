
#include "../include/LinkedList.h"
// *****************************************************************************
//
// FUNCTION NAME: LLCountNodes

// Notes: Counts the number of Linked List Structure instances(nodes) created
//        starting from the head that is passed as an argument
//
// *****************************************************************************
uint32_t LLCountNodes(TLinkedListNode *headPtr)
{
    // Local Ptr of the LinkedList type
    TLinkedListNode *cursorPtr;
    // LocalPtr points to the head node that is being passed
    cursorPtr = headPtr;
    // has the count value of the number of LinkedList nodes created
    uint32_t cnt = 0u;
    // Checks whether the localPtr is pointing to NULL
    while(cursorPtr != NULL)
    {
        // If the LocalPtr(cursorPtr) is not pointing to NULL then the count value of the number of nodes in the LinkedList is incremented
        cnt++;
        //The cursor now points to the next LinkedList node
        cursorPtr = cursorPtr->nextPtr;
    }
    return cnt;
} // End of LLCountNodes()

// *****************************************************************************
//
// FUNCTION NAME: LLAppend

// Notes: Adds a new node at the end of the linked list
//
// *****************************************************************************
void LLAppend(TLinkedListNode *headPtr, TLinkedListNode* newNodePtr)
{
    /* go to the last node */
    // Local Ptr of the LinkedList type
    TLinkedListNode *cursorPtr;
    // LocalPtr points to the head node that is being passed
    cursorPtr = headPtr;
    // checks whether the nextPtr of the cursor is pointing to NULL
    while(cursorPtr->nextPtr != NULL)
        // If the nextPtr of the CUrsor is not pointing to NULL, the cursor traverses until the last node
            cursorPtr = cursorPtr->nextPtr;
    //Appends the new node to the Linked List
    cursorPtr->nextPtr = newNodePtr;
} // End of LLAppend()
