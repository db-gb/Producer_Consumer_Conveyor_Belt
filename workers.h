#include <time.h>
#include "conveyor.h"

#define NSPERMS 1000000     // 1 mil ns per ms
#define MSPERSEC 1000       // 1 thousand ms per sec


// Function that initializes and runs a producer
void * Producer(void * VoidPtr);

// Function that initializes and runs a consumer
void * Consumer(void *VoidPtr);

// Initialize Producer
// Input: THREAD_DATA pointer from conveyor
//        Int: desired delay in ms
// Output: Int: ProducerID for newly created producer
int InitializeProducer(void* voidPtr, int* delayMS);


// Initialize Consumer
// Input: THREAD_DATA pointer from conveyor
//        Int: desired delay in ms
// Output: Int: ConsumerID for newly created consumer
int InitializeConsumer(void* voidPtr, int* delayMS);


// Sets the worker's sleep time based on desired delay in 
// milliseconds
void SetSleepTime(struct timespec*, int delayMS);
