#include <unistd.h>
#include <pthread.h>

#include "workers.h"

// program exit codes
#define EXT_NORMAL 0
#define EXT_INPUT_ERR 5
#define EXT_SEMAPHORE 10
#define EXT_THREAD 15

#define NUM_CANDIES_TO_PRODUCE 100
#define MAX_CANDIES_ON_BELT 10
#define MAX_FROGBITES_ON_BELT 3


// Helper function to create our semaphores
void CreateSemaphores(sem_t *Mutex, sem_t *ProductionLimit, 
		      sem_t *ConsumptionLimit, sem_t *BeltEmptySlots, 
		      sem_t *BeltFullSlots, sem_t *BeltFrogSlots)
{
 // Create semaphores
  if(sem_init(Mutex, 0, 1) == -1)
    {
      fprintf(stderr, "Unable to initialize Mutex semaphore\n");
      exit(EXT_SEMAPHORE);
    }

  if(sem_init(ProductionLimit, 0, NUM_CANDIES_TO_PRODUCE) == -1)
    {
      fprintf(stderr, "Unable to initialize Production limit barrier\n");
      exit(EXT_SEMAPHORE);
    }

  if(sem_init(ConsumptionLimit, 0, 0) == -1)
    {
      fprintf(stderr, "Unable to initilize Consumption limit barrier\n");
      exit(EXT_SEMAPHORE);
    }

  if(sem_init(BeltEmptySlots, 0, MAX_CANDIES_ON_BELT) == -1)
    {
      fprintf(stderr, "Unable to initialize belt Empty semaphore\n");
      exit(EXT_SEMAPHORE);
    }

  if(sem_init(BeltFullSlots, 0, 0) == -1)
    {
      fprintf(stderr, "Unable to initialize belt Full semaphore\n");
      exit(EXT_SEMAPHORE);
    }

  if(sem_init(BeltFrogSlots, 0, MAX_FROGBITES_ON_BELT) == -1)
    {
      fprintf(stderr, "Unable to init frog semaphore\n");
      exit(EXT_SEMAPHORE);
    } 
}




// I the undersigned promise that the attached assignment is my own work.
// While I was free to discuss ideas with others, the work contained is
// my own. I recognize that should this not be the case, I will be
// subject to penalties as outlined in the course syllabus.
// Name: Daniel C. Barnas
// RedID: 811754860
  int main(int argc, char *argv[])
{
  THREAD_DATA ConveyorBeltData;   // Data to share amongst threads
  int ConsumersAssigned = 0;
  int ProducersAssigned = 0;
  sem_t Mutex;            // Critical region semaphore
  sem_t ProductionLimit;  // Control production via barrier
  sem_t ConsumptionLimit; // Control consumption via barrier
  sem_t BeltEmpty;        // Empty slots on belt
  sem_t BeltFull;         // Full slots on belt
  sem_t FrogLimit;        // Control FrogBites on belt

  pthread_t ProducerThread_0;     // FrogBite producer
  pthread_t ProducerThread_1;     // Escargot producer
  pthread_t ConsumerThread_0;     // Lucy
  pthread_t ConsumerThread_1;     // Ethel

  int option;
  int lucyDelay = 0;
  int ethelDelay = 0;
  int frogBiteDelay = 0;
  int escargotDelay = 0;

  void *ThreadResultPtr;

  int i;

  struct ConveyorBelt belt;
 
  // Handle optional arguments
  while((option = getopt(argc, argv, "E:L:f:e:")) != -1)
    {
      switch(option)
	{
	case 'E':
	  // Grab Ethel's delay time
	  ethelDelay = atoi(optarg);
	  break;
	case 'L':
	  // Grab Lucy's delay time
	  lucyDelay = atoi(optarg);
	  break;
	case 'f':
	  // Grab frogBite delay
	  frogBiteDelay = atoi(optarg);
	  break;
	case 'e': 
	  // Grab escargot delay
	  escargotDelay = atoi(optarg);
	  break;
	default:
	  printf("invalid flag!\n");
	  exit(EXT_INPUT_ERR);
	}
    }



  // Initialize belt info data structure
  ConveyorBeltData.Belt = &belt; 
  ConveyorBeltData.ConsAssignedPtr = &ConsumersAssigned;
  ConveyorBeltData.ProdsAssignedPtr = &ProducersAssigned;
  ConveyorBeltData.MutexPtr = &Mutex;
  ConveyorBeltData.ProdLimitPtr = &ProductionLimit;
  ConveyorBeltData.ConsLimitPtr = &ConsumptionLimit;
  ConveyorBeltData.BeltEmptyPtr = &BeltEmpty;
  ConveyorBeltData.BeltFullPtr = &BeltFull;
  ConveyorBeltData.FrogLimitPtr = &FrogLimit;

  // Initialize Conveyor belt
  InitConveyorBelt(&belt);      // sets consumption and prod logs to 0

  // Set worker delay times
  belt.ConsumerSpeeds[Ethel] = ethelDelay;
  belt.ConsumerSpeeds[Lucy] = lucyDelay;
  belt.ProducerSpeeds[FrogBite] = frogBiteDelay;
  belt.ProducerSpeeds[Escargot] = escargotDelay;

 
  CreateSemaphores(&Mutex, &ProductionLimit, &ConsumptionLimit, 
		   &BeltEmpty, &BeltFull, &FrogLimit);


  // Create worker threads and off to work!
  pthread_create(&ProducerThread_0, NULL, Producer, &ConveyorBeltData);
  pthread_create(&ProducerThread_1, NULL, Producer, &ConveyorBeltData);
  pthread_create(&ConsumerThread_0, NULL, Consumer, &ConveyorBeltData);
  pthread_create(&ConsumerThread_1, NULL, Consumer, &ConveyorBeltData);

  // Barrier to make sure work stops when quotas met
  for(i = 0; i < NUM_CANDIES_TO_PRODUCE; i++)
    {
      // Make sure all candies consumed
      sem_wait(ConveyorBeltData.ConsLimitPtr);   
    }

  // Work done - end all threads
  pthread_kill(&ProducerThread_0, 0);
  pthread_kill(&ProducerThread_1, 0);
  pthread_kill(&ConsumerThread_0, 0);
  pthread_kill(&ConsumerThread_1, 0);
  
  
  // Print production report
  io_production_report(ConveyorBeltData.Belt->Produced, ConveyorBeltData.Belt->Consumed);

  DeleteConveyorBelt(ConveyorBeltData.Belt);

  exit(EXIT_SUCCESS);
}


