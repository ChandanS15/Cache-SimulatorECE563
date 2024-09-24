#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdio.h>
#include <stdint.h>


typedef struct node{
    struct _cacheDS* cacheLevelPtr;       // points to the Data
    struct node* nextPtr;   // points to the next node (Linked List)
}TLinkedListNode; 

//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - CacheDSInit
// Note          - 
// Return        - 
//------------------------------------------------------------------------------------------------------------------
uint32_t LLCountNodes(TLinkedListNode *headPtr);


// *****************************************************************************
//
// FUNCTION NAME: LLAppend

// Notes: Adds a new node at the end of the linked list
//
// *****************************************************************************
void LLAppend(TLinkedListNode *headPtr, TLinkedListNode* newNodePtr);

// *****************************************************************************
//
// FUNCTION NAME: LLlink

// Notes: Adds a new node at the end of the linked list
//
// *****************************************************************************

//void LLlink(TLinkedListNode *headPtr, TCacheDS *cacheNode1, TCacheDS *cacheNode2);
#endif