#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <semaphore.h>

#include "io.h"



// Candy struct declaration. Holds the type of candy 
// and contains a macro call from sys/queue.h that declares
// a structure that allows Candies to be part of a queue.
struct Candy 
{
  ProductType candyType;
  STAILQ_ENTRY(Candy) Conveyor;
};


// Defines the head of our conveyor belt queue.
STAILQ_HEAD(ConveyorHead, Candy);


// Conveyor Belt declaration. Our conveyor belt is a queue
// whose head is defined as beltHead. The OnBelt array 
// holds info about what candy types are currently on the belt
struct ConveyorBelt
{
  struct ConveyorHead beltHead;
  int OnBelt[ProductTypeN];
  int Produced[ProductTypeN];
  int** Consumed;
  int ProducerSpeeds[ProductTypeN];
  int ConsumerSpeeds[ConsumerTypeN];
};


//Structure for communicating between producer and consumer threads
typedef struct {
  struct ConveyorBelt* Belt;    // Pointer to shared conveyor belt
  int *ConsAssignedPtr;     // Consumers currently assigned
  int *ProdsAssignedPtr;    // Producers currently assigned
  sem_t *MutexPtr;       // pointer to critical reg semaphore
  sem_t *ProdLimitPtr;   // pointer to barrier semaphore
  sem_t *ConsLimitPtr;
  sem_t *BeltEmptyPtr;   // pointer to belt capacity semaphore
  sem_t *BeltFullPtr;
  sem_t *FrogLimitPtr;   // pointer to FrogBite limit semaphore
} THREAD_DATA;





// Constructor-like function for a conveyor belt. Takes a 
// pointer to a conveyor belt and initializes its internal
// array(s) to 0.
void InitConveyorBelt(struct ConveyorBelt*);

// Add Candy
// Function that allows producer to add a candy to the 
// conveyor belt. Takes a pointer to the conveyor belt
// as well as a pointer to the Candy
void AddCandyToBelt(struct ConveyorBelt*, struct Candy*);

// Remove Candy
// Function that allows consumer to remove a candy from
// the conveyor belt. Takes a pointer to the conveyor belt
// and returns a pointer to the Candy at the head of the
// queue, if there is one.
struct Candy* RemoveCandyFromBelt(struct ConveyorBelt*);

// Test function to make sure our conveyor belt is 
// accurately storing candy quantities
void PrintCurrentBeltInventory(struct ConveyorBelt*);

// Destructor function to free up any dynamically allocated memory 
void DeleteConveyorBelt(struct ConveyorBelt*);
