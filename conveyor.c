#include "conveyor.h"


void InitConveyorBelt(struct ConveyorBelt* belt)
{
  int i, j;
  
  // Initialize the queue pointed to by convoyer belt head
  STAILQ_INIT(&belt->beltHead);
  
  // Initialize the values of OnBelt and Produced arrays to 0
  for(i = 0; i < ProductTypeN; i++)
    {
      *(&belt->OnBelt[i]) = 0;
      *(&belt->Produced[i]) = 0;
      *(&belt->ProducerSpeeds[i]) = 0;   // default: 0ms production time
    }

  
  // Initialize the values of Consumed and Consumer speeds arrays to 0
  //Consumed = malloc (ConsumerTypeN * sizeof(int *));
  belt->Consumed = malloc(ConsumerTypeN * sizeof(int*));
  for(i = 0; i < ConsumerTypeN; i++)
    {
      *(&belt->ConsumerSpeeds[i]) = 0;     // default: 0ms pack time (no delay)
      
      belt->Consumed[i] = malloc(ProductTypeN * sizeof(int));
      for(j = 0; j < ProductTypeN; j++)
	{
	  belt->Consumed[i][j] = 0;
	}
    }

}


void AddCandyToBelt(struct ConveyorBelt* belt, struct Candy* candy)
{
  // Inserts a candy at the tail of the queue
  STAILQ_INSERT_TAIL(&belt->beltHead, candy, Conveyor);

  // Increment the OnBelt array for that candy type
  *(&belt->OnBelt[candy->candyType]) += 1;
}


struct Candy* RemoveCandyFromBelt(struct ConveyorBelt* belt)
{
  struct Candy* candy;

  // Returns first item in queue or NULL if queue empty
  candy = STAILQ_FIRST(&belt->beltHead);
  if(!candy)
    {
      printf("No candies on conveyor belt!\n");
      return NULL; 
    }

  // Remove later
  if(candy->candyType == FrogBite){
    printf("Candy type: FrogBite\n");
  }
  if(candy->candyType == Escargot)
    {
      printf("Candy type: Escargot\n");
    }
  
  // Remove the first item from the queue
  STAILQ_REMOVE_HEAD(&belt->beltHead, Conveyor);

  // Decrement OnBelt array for that candy type
  *(&belt->OnBelt[candy->candyType]) -= 1;

  // Return a poitner to the candy
  return candy;
  
}


// Destructor
void DeleteConveyorBelt(struct ConveyorBelt* belt)
{
  int i;
  // Free up Consumed array 
  for(i = 0; i < ConsumerTypeN; i++)
    {
      free(belt->Consumed[i]);
    }
  free(belt->Consumed);
  
}
