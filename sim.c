#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "include/sim.h"
#include "include/Cache.h"
#include "include/LinkedList.h"
#include "include/Queue.h"



// Instances of linkedList to create heirarchy of Cache.
TLinkedListNode *cacheNode1, *cacheNode2, *headLL;
// Instantiation of Caches.
TCacheDS *cacheL1, *cacheL2;

// Temporary variable to load value;
uint32_t loadedValue;

bool loadStoreStatus = false;
void PrefetchTest(TLinkedListNode * node);






int main (int argc, char *argv[]) {
   FILE *fp;			                     // File pointer.
   char *trace_file;		                  // This variable holds the trace file name.
   cache_params_t params;	               // Look at the sim.h header file for the definition of struct cache_params_t.
   char rw;			                        // This variable holds the request's type (read or write) obtained from the trace.
   uint32_t addr;		                     // This variable holds the request's address obtained from the trace.
				                              // The header file <inttypes.h> above defines signed and unsigned integers of various sizes in a machine-agnostic way.  "uint32_t" is an unsigned integer of 32 bits.

   // Exit with an error if the number of command-line arguments is incorrect.
   if (argc != 9) {
      printf("Error: Expected 8 command-line arguments but was provided %d.\n", (argc - 1));
      exit(EXIT_FAILURE);
   }

   // Head of the linkedList to connect multiple heirarchies of cache memories.
   headLL = NULL;

   // "atoi()" (included by <stdlib.h>) converts a string (char *) to an integer (int).
   params.BLOCKSIZE = (uint32_t) atoi(argv[1]);
   params.L1_SIZE   = (uint32_t) atoi(argv[2]);
   params.L1_ASSOC  = (uint32_t) atoi(argv[3]);
   params.L2_SIZE   = (uint32_t) atoi(argv[4]);
   params.L2_ASSOC  = (uint32_t) atoi(argv[5]);
   params.PREF_N    = (uint32_t) atoi(argv[6]);
   params.PREF_M    = (uint32_t) atoi(argv[7]);
   trace_file       = argv[8];

   // Open the trace file for reading.
   fp = fopen(trace_file, "r");
   if (fp == (FILE *)NULL) { //(FILE *)
      // Exit with an error if file open failed.
      printf("Error: Unable to open file %s\n", trace_file);
      exit(EXIT_FAILURE);
   }

   // Print simulator configuration.
   printf("===== Simulator configuration =====\n");
   printf("BLOCKSIZE:  %u\n", params.BLOCKSIZE);
   printf("L1_SIZE:    %u\n", params.L1_SIZE);
   printf("L1_ASSOC:   %u\n", params.L1_ASSOC);
   printf("L2_SIZE:    %u\n", params.L2_SIZE);
   printf("L2_ASSOC:   %u\n", params.L2_ASSOC);
   printf("PREF_N:     %u\n", params.PREF_N);
   printf("PREF_M:     %u\n", params.PREF_M);
   printf("trace_file: %s\n", trace_file);

   // Initialise cache Data store which has attributes of cache to be designed.
   if(params.L2_SIZE == 0) {

      // Allocate memory for LinkedLists.
      cacheNode1 = (TLinkedListNode*)malloc(sizeof(TLinkedListNode));

      // Allocate memory for Cache which has all the attributes of a cache, including Tag, valid, dirty, counter, blocks.
      cacheL1 = (TCacheDS*)malloc(sizeof(TCacheDS));

      // Initialise the DataStore of L1 Cache.
      CacheDSInit(cacheL1, "L1", params.BLOCKSIZE, params.L1_SIZE, params.L1_ASSOC, params.PREF_N, params.PREF_M);

      // Data pointer of Linkedlist node 1 points to L1 Cache and
      // the next node is NULL.
      cacheNode1->cacheLevelPtr = cacheL1;
      cacheNode1->nextPtr = NULL;
      headLL = cacheNode1;
   }
   else if (params.L2_SIZE > 0){

      // Initialise the DataStore of L1 Cache and L2 Cache.

      // Allocate memory for LinkedLists.
      cacheNode1 = (TLinkedListNode*)malloc(sizeof(TLinkedListNode));
      cacheNode2 = (TLinkedListNode*)malloc(sizeof(TLinkedListNode));

      // Allocate memory for Cache which has all the attributes of a cache, including Tag, valid, dirty, counter, blocks,

      cacheL1 = (TCacheDS*)malloc(sizeof(TCacheDS));
      cacheL2 = (TCacheDS*)malloc(sizeof(TCacheDS));

      CacheDSInit(cacheL1, "L1", params.BLOCKSIZE, params.L1_SIZE, params.L1_ASSOC, 0, 0);
      CacheDSInit(cacheL2, "L2", params.BLOCKSIZE, params.L2_SIZE, params.L2_ASSOC, params.PREF_N, params.PREF_M);

      // Data pointer of Linkedlist Node 1 points to L1 Cache and
      // the next node pointer points to LL of L2 cache.

      // Initialising data to the LinkedList Node.
      cacheNode1->cacheLevelPtr = cacheL1;
      cacheNode2->cacheLevelPtr = cacheL2;

      // Connect LinkedList nodes.
      cacheNode1->nextPtr = cacheNode2;
      cacheNode2->nextPtr = NULL;

      // Assign head to the first node
      headLL = cacheNode1;
   }

   // Allocate memory for the various members of CacheDS and initialise some of the important membrs such as counter
   // to make LRU implementation easier.
   CacheInit(headLL);

   //exit(0);
   uint32_t breakPoint = 135;
   uint32_t count = 0;
   uint32_t refTag = 0;
   // Read requests from the trace file and echo them back.
#ifdef GENERATE_DEBUG_FILE
   const char *debugfileName="debugCacheContents.txt";
   FILE *debugFile = fopen( debugfileName, "w" );
   if( (FILE*) debugFile == NULL) {
      printf("Failed to open file %s\n", debugfileName);
      return;
   }

#endif

   while (fscanf(fp, "%c %x\n", &rw, &addr) == 2) {	// Stay in the loop if fscanf() successfully parsed two tokens as specified.
      count++;


      if(count == 41926) {
         count++;
         count--;
      }

      if(count == 99391) {
         count++;
         count--;
      }

      if(count == 99452) {
         count++;
         count--;
      }

      if(count == 99995) {
         count++;
         count--;
      }

      if(count == breakPoint) {
         count++;
         count--;
         breakPoint  += 1;
      }

      //    refTag = cacheL2->prefetchQueue[0].dataPtr[0].tag;
      // for(uint8_t i=0; i< cacheL2->numOfStreams; i++) {
      //
      //    if(refTag == cacheL2->prefetchQueue[i].dataPtr[0].tag) {
      //       count++;
      //       count--;
      //    }
      // }

      if (rw == 'r') {
#ifdef GENERATE_DEBUG_FILE
         loadStoreStatus = CacheLoadData(headLL,addr,&loadedValue,debugFile);
#elif
            loadStoreStatus = CacheLoadData(headLL,addr,&loadedValue);
#endif
#ifdef DEBUG_LOWPRIORITY_AVAILABLE
             if(loadStoreStatus) {
                printf("The LoadedValue from the address %x is : %d\n", addr, loadedValue);
                printf("Cache Statistics Hit Count  = %d.\n", headLL->cacheLevelPtr->cacheStatistics.hitCount);
                printf("Cache Statistics Miss Count = %d.\n", headLL->cacheLevelPtr->cacheStatistics.missCount);
             }
             else {
                printf("Load operation failed.\n");
             }
#endif

         }
         else if (rw == 'w') {
            //printf("w %x\n", addr);
#ifdef GENERATE_DEBUG_FILE
            loadStoreStatus = CacheStoreData(headLL,addr,0, debugFile);
#elif
            loadStoreStatus = CacheStoreData(headLL,addr,0);
#endif
#ifdef DEBUG_LOWPRIORITY_AVAILABLE
             if(loadStoreStatus) {
                printf("The Stored Value to the address %x. \n", addr);
             }
             else {
                printf("Store operation failed.\n");
             }
#endif
         }
         else {
            printf("Error: Unknown request type %c.\n", rw);
            exit(EXIT_FAILURE);
         }
   }


   // Printing the Contents of each Cache
#ifdef GENERATE_FILE
   PrintCacheContents(headLL, "cacheContents.txt");
#else
   PrintCacheContents(headLL);
#endif
   //if(headLL->cacheLevelPtr->prefetchAvailable)
   //   PrefetchTest(headLL);
   CacheDeallocateMemory(headLL);


    return(0);
}

//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - PrintCacheContents
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------
#ifdef GENERATE_FILE
void PrintCacheContents( TLinkedListNode *headPtr, const char *fileName) {
#else
void PrintCacheContents( TLinkedListNode *headPtr) {
#endif

   CalculatePerformanceParameters(headPtr);
   TLinkedListNode *cursorPtr = headPtr;
   TprefetchDS *retrievePrefetchData = (TprefetchDS*)malloc(sizeof(TprefetchDS));

   uint32_t indexValue =0;
   uint32_t tagValue = 0;

   uint32_t prefetchStreamIndex = 0;
   uint32_t prefetchBlockIndex = 0;

#ifdef GENERATE_FILE
    FILE *file = fopen( fileName, "w" );
    if(file == NULL) {
        printf("Failed to open file %s\n", fileName);
        return;
    }
#endif

    while ( cursorPtr != NULL ) {
       // Print the Contents of the respective Cache
#ifdef DEBUG_AVAILABLE
       printf("\n");
       printf( "===== %s contents =====",cursorPtr->cacheLevelPtr->name);
       printf( "\n");
#endif
#ifdef GENERATE_FILE
       fprintf(file, "\n");
       fprintf(file,"===== %s contents =====",cursorPtr->cacheLevelPtr->name);
       fprintf(file, "\n");
#endif

uint32_t printSet = 0;
       uint32_t printSet2 = 0;
       uint32_t printNextLine = 0;
       for(indexValue =0; indexValue < cursorPtr->cacheLevelPtr->numOfSets; indexValue++) {
          printSet2 = 0;


          uint32_t tagIndex = 0;
          uint32_t countIndex = 0;

          while(tagIndex < cursorPtr->cacheLevelPtr->assoc) {
             for(tagValue =0; tagValue < cursorPtr->cacheLevelPtr->assoc; tagValue++) {
                if(cursorPtr->cacheLevelPtr->cacheSetDS[indexValue].cacheTagDS[tagValue].counter == countIndex  ) {
                   if(cursorPtr->cacheLevelPtr->cacheSetDS[indexValue].cacheTagDS[tagValue].valid == true){
                      printSet = 1;
                      printNextLine = 1;
                      if(printSet == 1 && printSet2 == 0) {
if(cursorPtr->cacheLevelPtr->assoc > 1)
   printSet2 = 1;
#ifdef DEBUG_AVAILABLE
                         printf("set    %d:",indexValue);
#endif
#ifdef GENERATE_FILE
                         fprintf(file,"set    %d:",indexValue);
#endif
#ifdef DEBUG_AVAILABLE
                      }
                      printSet = 0;
                      printf("     %x %s", cursorPtr->cacheLevelPtr->cacheSetDS[indexValue].cacheTagDS[tagValue].tag, (cursorPtr->cacheLevelPtr->cacheSetDS[indexValue].cacheTagDS[tagValue].dirty == true) ? "D" : " " );
#endif
#ifdef GENERATE_FILE
                      fprintf(file,"    %x %s ", cursorPtr->cacheLevelPtr->cacheSetDS[indexValue].cacheTagDS[tagValue].tag, (cursorPtr->cacheLevelPtr->cacheSetDS[indexValue].cacheTagDS[tagValue].dirty == true) ? "D" : " ");
#endif
                   }
                }
             }
             countIndex++;
             tagIndex ++;
          }

          if(printNextLine == 1) {
#ifdef DEBUG_AVAILABLE
             printf("\n");
#endif
#ifdef GENERATE_FILE
             fprintf(file,"\n");
#endif
          }
          printNextLine = 0;
       }

       // Print prefetch Stream buffers content if available
       if(cursorPtr->cacheLevelPtr->prefetchAvailable == ePrefetchPresent) {
          uint32_t printStreamBuffer = 0;
          uint32_t printNextLine = 0;
#ifdef DEBUG_AVAILABLE
          printf("\n");
          printf("===== Stream Buffer (s) contents =====\n");
#endif
#ifdef GENERATE_FILE
          fprintf(file,"\n");
          fprintf(file,"===== Stream Buffer (s) contents =====\n");
#endif
          uint32_t tagIndex = 0;
          uint32_t countIndex = 0;
          uint8_t mruArray[cursorPtr->cacheLevelPtr->numOfStreams];
          FindMRUPrefetch(cursorPtr, mruArray);

          uint32_t maxCounterValue = cursorPtr->cacheLevelPtr->prefetchQueue[0].lruIndex;

          for(uint32_t searchQueueIndex = 0 ; searchQueueIndex < cursorPtr->cacheLevelPtr->numOfStreams; searchQueueIndex++) {
             uint32_t printInPrefetchQueue = mruArray[searchQueueIndex];
             if(cursorPtr->cacheLevelPtr->prefetchQueue[searchQueueIndex].validBit == true){
                for(prefetchBlockIndex = 0; prefetchBlockIndex < cursorPtr->cacheLevelPtr->numOfBlocksPerStream; prefetchBlockIndex++) {
                   bool queuePeekStatus = QueuePeekByIndex(&cursorPtr->cacheLevelPtr->prefetchQueue[printInPrefetchQueue],
                                                           (uint16_t *) retrievePrefetchData, prefetchBlockIndex);
                   // Print the contents of the prefetch Buffer
                   if(queuePeekStatus == true) {
#ifdef DEBUG_AVAILABLE
                      printf("%x  ", retrievePrefetchData->tag);
#endif
#ifdef GENERATE_FILE
                      fprintf(file,"%x  ", retrievePrefetchData->tag);
#endif
                   }

                }

#ifdef DEBUG_AVAILABLE
                   printf("\n");
#endif
#ifdef GENERATE_FILE
                   fprintf(file,"\n");
#endif

             }
          }

             }cursorPtr = cursorPtr->nextPtr;
       }


   // After printing the contents of the cache print the Measurments.
#ifdef GENERATE_FILE
   PrintMeasurements(headPtr, file);
#else
   PrintMeasurements(headPtr);
#endif
}
//*************************************************************************************************************** */
//------------------------------------------------------------------------------------------------------------------
// Function Name - PrintMeasurements
// Note          -
// Return        -
//------------------------------------------------------------------------------------------------------------------
#ifdef GENERATE_FILE
void PrintMeasurements(TLinkedListNode *headPtr, FILE *fileName) {

#else
void PrintMeasurements(TLinkedListNode *headPtr) {
#endif
   uint32_t defaultValue = 0;
    TLinkedListNode *cursorPtr = headPtr;
#ifdef GENERATE_FILE
   FILE *file = fileName;
#endif
#ifdef DEBUG_AVAILABLE
      printf("\n");
      printf("===== Measurements =====\n");
      printf("a. L1 reads:    %d\n",cursorPtr->cacheLevelPtr->cacheStatistics.readCount);
      printf("b. L1 read misses:    %d\n",cursorPtr->cacheLevelPtr->cacheStatistics.readMissCount);
      printf("c. L1 writes:    %d\n",cursorPtr->cacheLevelPtr->cacheStatistics.writeCount);
      printf("d. L1 write misses:    %d\n",cursorPtr->cacheLevelPtr->cacheStatistics.writeMissCount);
      printf("e. L1 miss rate:    %.4f\n",cursorPtr->cacheLevelPtr->cacheStatistics.missRate);
      printf("f. L1 writebacks:    %d\n",cursorPtr->cacheLevelPtr->cacheStatistics.writeBackCount);
      printf("g. L1 prefetches:    %d\n",cursorPtr->cacheLevelPtr->prefetchStatistics.prefetchCount);



      // Print statistics of L2 cache.
   if(cursorPtr->nextPtr != NULL) {
      printf("h. L2 reads (demand):    %d\n",cursorPtr->nextPtr->cacheLevelPtr->cacheStatistics.readCount);
      printf("i. L2 read misses (demand):    %d\n",cursorPtr->nextPtr->cacheLevelPtr->cacheStatistics.readMissCount);
      printf("j. L2 reads (prefetch):    %d\n",cursorPtr->nextPtr->cacheLevelPtr->prefetchStatistics.readCount);
      printf("k. L2 read misses (prefetch):    %d\n",cursorPtr->nextPtr->cacheLevelPtr->prefetchStatistics.missCount);
      printf("l. L2 writes:    %d\n",cursorPtr->nextPtr->cacheLevelPtr->cacheStatistics.writeCount);
      printf("m. L2 write misses:    %d\n",cursorPtr->nextPtr->cacheLevelPtr->cacheStatistics.writeMissCount);
      printf("n. L2 miss rate:    %.4lf\n",cursorPtr->nextPtr->cacheLevelPtr->cacheStatistics.missRate);
      printf("o. L2 writebacks:    %d\n",cursorPtr->nextPtr->cacheLevelPtr->cacheStatistics.writeBackCount);
      printf("p. L2 prefetches:    %d\n",cursorPtr->nextPtr->cacheLevelPtr->prefetchStatistics.prefetchCount);
      printf("q. memory traffic:    %d\n",cursorPtr->nextPtr->cacheLevelPtr->totalMemoryTraffic);
   } else {

      printf("h. L2 reads (demand):    %d\n", defaultValue);
      printf("i. L2 read misses (demand):    %d\n",defaultValue);
      printf("j. L2 reads (prefetch):    %d\n",defaultValue);
      printf("k. L2 read misses (prefetch):    %d\n",defaultValue);
      printf("l. L2 writes:    %d\n",defaultValue);
      printf("m. L2 write misses:    %d\n",defaultValue);
      printf("n. L2 miss rate:    %.4lf\n",(double)defaultValue);
      printf("o. L2 writebacks:    %d\n",defaultValue);
      printf("p. L2 prefetches:    %d\n",defaultValue);
      printf("q. memory traffic:    %d\n",cursorPtr->cacheLevelPtr->totalMemoryTraffic);
   }
#endif
#ifdef GENERATE_FILE
      fprintf(file,"\n");
      fprintf(file,"===== Measurements =====\n");
      fprintf(file,"a. L1 reads:    %d\n",cursorPtr->cacheLevelPtr->cacheStatistics.readCount);
      fprintf(file,"b. L1 read misses:    %d\n",cursorPtr->cacheLevelPtr->cacheStatistics.readMissCount);
      fprintf(file,"c. L1 writes:    %d\n",cursorPtr->cacheLevelPtr->cacheStatistics.writeCount);
      fprintf(file,"d. L1 write misses:    %d\n",cursorPtr->cacheLevelPtr->cacheStatistics.writeMissCount);
      fprintf(file,"e. L1 miss rate:    %.4lf\n",cursorPtr->cacheLevelPtr->cacheStatistics.missRate);
      fprintf(file,"f. L1 writebacks:    %d\n",cursorPtr->cacheLevelPtr->cacheStatistics.writeBackCount);
      fprintf(file,"g. L1 prefetches:    %d\n",cursorPtr->cacheLevelPtr->prefetchStatistics.prefetchCount);

   if(cursorPtr->nextPtr != NULL) {
      // Print statistics of L2 cache.
      fprintf(file,"h. L2 reads (demand):    %d\n",cursorPtr->nextPtr->cacheLevelPtr->cacheStatistics.readCount);
      fprintf(file,"i. L2 read misses (demand):    %d\n",cursorPtr->nextPtr->cacheLevelPtr->cacheStatistics.readMissCount);
      fprintf(file,"j. L2 reads (prefetch):    %d\n",cursorPtr->nextPtr->cacheLevelPtr->prefetchStatistics.readCount);
      fprintf(file,"k. L2 read misses (prefetch):    %d\n",cursorPtr->nextPtr->cacheLevelPtr->prefetchStatistics.missCount);
      fprintf(file,"l. L2 writes:    %d\n",cursorPtr->nextPtr->cacheLevelPtr->cacheStatistics.writeCount);
      fprintf(file,"m. L2 write misses:    %d\n",cursorPtr->nextPtr->cacheLevelPtr->cacheStatistics.writeMissCount);
      fprintf(file,"n. L2 miss rate:    %.4lf\n",cursorPtr->nextPtr->cacheLevelPtr->cacheStatistics.missRate);
      fprintf(file,"o. L2 writebacks:    %d\n",cursorPtr->nextPtr->cacheLevelPtr->cacheStatistics.writeBackCount);
      fprintf(file,"p. L2 prefetches:    %d\n",cursorPtr->nextPtr->cacheLevelPtr->prefetchStatistics.prefetchCount);
      fprintf(file,"q. memory traffic:    %d\n",cursorPtr->nextPtr->cacheLevelPtr->totalMemoryTraffic);
   } else {
      fprintf(file,"h. L2 reads (demand):    %d\n",defaultValue);
      fprintf(file,"i. L2 read misses (demand):    %d\n",defaultValue);
      fprintf(file,"j. L2 reads (prefetch):    %d\n",defaultValue);
      fprintf(file,"k. L2 read misses (prefetch):    %d\n",defaultValue);
      fprintf(file,"l. L2 writes:    %d\n",defaultValue);
      fprintf(file,"m. L2 write misses:    %d\n",defaultValue);
      fprintf(file,"n. L2 miss rate:    %.4lf\n",(double)defaultValue);
      fprintf(file,"o. L2 writebacks:    %d\n",defaultValue);
      fprintf(file,"p. L2 prefetches:    %d\n",defaultValue);
      fprintf(file,"q. memory traffic:    %d\n",cursorPtr->cacheLevelPtr->totalMemoryTraffic);
   }


   fclose(file);
#endif

}

void CalculatePerformanceParameters(TLinkedListNode *headPtr) {
   TLinkedListNode *cursorPtr = headPtr;


   uint32_t missRate = 0;
   if(cursorPtr !=NULL) {

      //if(cursorPtr->cacheLevelPtr->prefetchAvailable == ePrefetchAbsent) {
         cursorPtr->cacheLevelPtr->cacheStatistics.missRate = (float_t)(cursorPtr->cacheLevelPtr->cacheStatistics.readMissCount + cursorPtr->cacheLevelPtr->cacheStatistics.writeMissCount)/
           (cursorPtr->cacheLevelPtr->cacheStatistics.readCount + cursorPtr->cacheLevelPtr->cacheStatistics.writeCount);
      //}

      if(cursorPtr->nextPtr != NULL) {
         //if(cursorPtr->cacheLevelPtr->prefetchAvailable == ePrefetchAbsent) {
         cursorPtr->nextPtr->cacheLevelPtr->cacheStatistics.missRate = (float_t)(cursorPtr->nextPtr->cacheLevelPtr->cacheStatistics.readMissCount)/
                                       (cursorPtr->cacheLevelPtr->cacheStatistics.readMissCount + cursorPtr->cacheLevelPtr->cacheStatistics.writeMissCount);
         //}
      }

   }
}
