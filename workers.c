#include "workers.h"



void * Producer(void * VoidPtr)
{
  struct timespec SleepTime;
  int DelayMS = 0;
  struct Candy* candy;
  int i;
  int ProducerID;

  // type cast void pointer into THREAD_DATA
  THREAD_DATA *DataPtr = (THREAD_DATA *) VoidPtr;

  // Initialize Producer
  ProducerID = InitializeProducer(VoidPtr, &DelayMS);
  SetSleepTime(&SleepTime, DelayMS);

  // Get to work
  while(1)
    {
      // Allocate memory for new candy and set type
      candy = malloc(sizeof(struct Candy));
      candy->candyType = ProducerID;

      // Check for empty slot on belt
      sem_wait(DataPtr->BeltEmptyPtr);   

      // Check for available production slot per daily quota
      sem_wait(DataPtr->ProdLimitPtr);

      // Check against frog limit of 3 on belt at a time
      if(candy->candyType == FrogBite)
	{
	  sem_wait(DataPtr->FrogLimitPtr);
	}

      // Candy Production/////////////////////////////////////////////
      sem_wait(DataPtr->MutexPtr);     // Enter Critical Region

      AddCandyToBelt(DataPtr->Belt,  candy);
      // Update daily production log
      (DataPtr->Belt->Produced[candy->candyType])++;

      // Output belt status after change
      io_add_type(candy->candyType, DataPtr->Belt->OnBelt, DataPtr->Belt->Produced);
      sem_post(DataPtr->MutexPtr);    // Exit Critical Region
      ////////////////////////////////////////////////////////////////

      // Mark new candy's slot on belt as full
      sem_post(DataPtr->BeltFullPtr);

      nanosleep(&SleepTime, NULL);    // Delay
    }
      
  return NULL;
  


}



void * Consumer(void *VoidPtr)
{
  struct timespec SleepTime;
  int DelayMS = 0;
  struct Candy* candy;
  int ConsumerID;

  // Typecast void pointer into THREAD_DATA
  THREAD_DATA *DataPtr = (THREAD_DATA *) VoidPtr;

  // Initialize Consumer 
  ConsumerID = InitializeConsumer(VoidPtr, &DelayMS);
  SetSleepTime(&SleepTime, DelayMS);

  // Get to work
  while(1)
    {
      // Check for a full slot on belt (ie candy created)
      sem_wait(DataPtr->BeltFullPtr);


      // Candy Consumption/////////////////////////////////////////
      sem_wait(DataPtr->MutexPtr);    // Enter critical region
     
      candy = RemoveCandyFromBelt(DataPtr->Belt);  // Grab next candy
      
      if(candy)  // verify a candy was grabbed
	{
	  // Update consumption log
	  DataPtr->Belt->Consumed[ConsumerID][candy->candyType]++;
	  
	  // Output belt status after change
	  io_remove_type(ConsumerID, candy->candyType, DataPtr->Belt->OnBelt, DataPtr->Belt->Consumed[ConsumerID]);
	}

      sem_post(DataPtr->MutexPtr);    // Exit critical region
      /////////////////////////////////////////////////////////////

      if(candy)   
	{
	  if(candy->candyType == FrogBite)
	    {
	      // Free up a slot for a frogbite
	      sem_post(DataPtr->FrogLimitPtr); 
	    }

	  sem_post(DataPtr->BeltEmptyPtr);   // mark slot on belt as empty
	  sem_post(DataPtr->ConsLimitPtr);   // progress toward consumption quota
	  
	  free(candy);    // deallocate candy's memory
	  
	  nanosleep(&SleepTime, NULL);    // delay
	}
    }
  return NULL;
}


int InitializeProducer(void* VoidPtr, int* DelayMS)
{
  int ID;

  THREAD_DATA *DataPtr = (THREAD_DATA *) VoidPtr;

  ///////////////////////////////////////////////////////
  sem_wait(DataPtr->MutexPtr);  // Enter Critical Region

  // Grab latest producer ID from conveyor and increment
  ID = *(DataPtr->ProdsAssignedPtr);   
  (*(DataPtr->ProdsAssignedPtr))++;

  // Grab producer delay data from conveyor
  *DelayMS = DataPtr->Belt->ProducerSpeeds[ID];
  
  sem_post(DataPtr->MutexPtr);   // Exit Critical Region
  ////////////////////////////////////////////////////////

  return ID; 
}



int InitializeConsumer(void* VoidPtr, int* DelayMS)
{
  int ID;

  THREAD_DATA *DataPtr = (THREAD_DATA *) VoidPtr;

  ///////////////////////////////////////////////////////
  sem_wait(DataPtr->MutexPtr);  // Enter Critical Region

  // Grab latest cosumer ID from conveyor and increment
  ID = *(DataPtr->ConsAssignedPtr);   
  (*(DataPtr->ConsAssignedPtr))++;

  // Grab consumer delay data from conveyor
  *DelayMS = DataPtr->Belt->ConsumerSpeeds[ID];
  
  sem_post(DataPtr->MutexPtr);   // Exit Critical Region
  ////////////////////////////////////////////////////////

  return ID; 
}




// Helper function to set the sleep time of a worker
// Inputs: pointer to worker's timespec and the desired delay in ms
void SetSleepTime(struct timespec* tspec, int delayMS)
{
  tspec->tv_sec = delayMS / MSPERSEC;
  tspec->tv_nsec = (delayMS % MSPERSEC) * NSPERMS;
}
