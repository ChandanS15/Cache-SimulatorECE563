#ifndef SIM_CACHE_H
#define SIM_CACHE_H

#include "LinkedList.h"


#define DEBUG_AVAILABLE 1U

#define GENERATE_FILE 1U
typedef
struct {
    uint32_t BLOCKSIZE;
    uint32_t L1_SIZE;
    uint32_t L1_ASSOC;
    uint32_t L2_SIZE;
    uint32_t L2_ASSOC;
    uint32_t PREF_N;
    uint32_t PREF_M;
} cache_params_t;
//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - PrintCacheContents
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------
#ifdef GENERATE_FILE
void PrintCacheContents( TLinkedListNode *headPtr, const char *fileName);
#else
void PrintCacheContents( TLinkedListNode *headPtr);
#endif
//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - PrintMeasurements
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------
#ifdef GENERATE_FILE
void PrintMeasurements(TLinkedListNode *headPtr, FILE *fileName);
#else
void PrintMeasurements(TLinkedListNode *headPtr);
#endif
void CalculatePerformanceParameters(TLinkedListNode *headPtr);
#endif
