// Bren Garcia Marti
// 2024sp ECE 4480 002
// Lab3: Branch Predictor

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define LENGTH_TRACE_DATA 16416280

// Last 16 bits
#define SIZE 16
#define NUM_ENTRIES 65536 //2^16

/* #define SIZE 19
#define NUM_ENTRIES 524288 // 2^19 */

#define TWO_LEVEL_SIZE 18
#define TWO_NUM_ENTRIES 262144 // 2^18
#define GHR_SIZE_GLOBAL 10
#define GHR_SIZE_LOCAL 13
#define BTH_NUM_ENTRIES 8912 //2^13


void init_trace();
void read_file();
void print_trace();

// ADDED CODE HERE
void pht_init();
void always_taken();
void not_taken();
bool one_level_bp(unsigned int addr, char outcome);
bool two_level_gbp(unsigned int addr, char outcome, unsigned int* _ghr);
bool two_level_lbp(unsigned int addr, char outcome, unsigned int* _ghr);
bool two_level_gshare(unsigned int addr, char outcome, unsigned int* _ghr);
bool two_level_gskewed(bool globalpred, bool localpred, bool gsharepred);

unsigned int BRANCH_ADDRESS[LENGTH_TRACE_DATA];
char         BRANCH_OUTCOME[LENGTH_TRACE_DATA];

unsigned int PHT1L[NUM_ENTRIES];
unsigned int PHT2G[TWO_NUM_ENTRIES];
unsigned int PHT2L[TWO_NUM_ENTRIES];
unsigned int BHT[BTH_NUM_ENTRIES];
unsigned int votingtable[NUM_ENTRIES];

int main(void)
{
  int bphit = 0;
  int gbphit = 0;
  int lbphit = 0;
  int gsharehit = 0;
  int gskewedhit = 0;
  unsigned int GHR = 0; // Global history register index


  init_trace();
  read_file();
  // print_trace();

// ADDED CODE HERE
  pht_init();

  always_taken();
  not_taken();
  
  for(int i = 0; i < LENGTH_TRACE_DATA; i++){
    if( one_level_bp(BRANCH_ADDRESS[i], BRANCH_OUTCOME[i]) ) {
      bphit++;
    }
  if( two_level_gbp(BRANCH_ADDRESS[i], BRANCH_OUTCOME[i], &GHR) ) {
      gbphit++;
    }
  if( two_level_lbp(BRANCH_ADDRESS[i], BRANCH_OUTCOME[i], &GHR) ) {
      lbphit++;
    }
  if( two_level_gshare(BRANCH_ADDRESS[i], BRANCH_OUTCOME[i], &GHR) ) {
      gsharehit++;
    }
   /* two_level_gskewed(BRANCH_ADDRESS[i], BRANCH_OUTCOME[i], &GHR) */
  if (two_level_gskewed(  two_level_gbp(BRANCH_ADDRESS[i], BRANCH_OUTCOME[i], &GHR),
                          two_level_lbp(BRANCH_ADDRESS[i], BRANCH_OUTCOME[i], &GHR),
                          two_level_gshare(BRANCH_ADDRESS[i], BRANCH_OUTCOME[i], &GHR)  ) ) {
    gskewedhit++;
  };
  }


// print results to terminal
printf("\n*** One-level BP \n");
//printf("%d \n", bphit);
  float sillyResults = (  (float)bphit * 100.0)/ (float)LENGTH_TRACE_DATA;
  printf("%9.6f\n", sillyResults);
  sillyResults = 0; //clean up

printf("\n*** Two-level global BP \n");
//printf("%d \n", gbphit);
  sillyResults = (  (float)gbphit  * 100.0)/ (float)LENGTH_TRACE_DATA;
  printf("%9.6f\n", sillyResults);
  sillyResults = 0; //clean up

printf("\n*** Two-level local BP \n");
//printf("%d \n", lbphit);
  sillyResults = (  (float)lbphit  * 100.0)/ (float)LENGTH_TRACE_DATA;
  printf("%9.6f\n", sillyResults);
  sillyResults = 0; //clean up

printf("\n*** Two-level gshare BP \n");
//printf("%d \n", gsharehit);
  sillyResults = (  (float)gsharehit  * 100.0)/ (float)LENGTH_TRACE_DATA;
  printf("%9.6f\n", sillyResults);
  sillyResults = 0; //clean up

printf("\n*** Two-level gskewed BP \n");
//printf("%d \n", gskewedhit);
  sillyResults = (  (float)gskewedhit  * 100.0)/ (float)LENGTH_TRACE_DATA;
  printf("%9.6f\n", sillyResults);
} // end of main


void init_trace()
{
  int i;
  for(i = 0; i < LENGTH_TRACE_DATA; i++){
	  BRANCH_ADDRESS[i] = 0;	
    BRANCH_OUTCOME[i] = '0'; 
  }
  printf("\n*** Initialization Done!\n");
}

void read_file()
{
 FILE *btrace;

 btrace = fopen("branch-trace-5.trace", "r");
 printf("\n*** Reading the trace!\n");

 if(btrace == NULL)
 {
    printf("Error: Can't open file\n");
 }
 else
 { 
    int i = 0;
    unsigned int address;
    char         outcome;

    while(fscanf(btrace, "%u %c\n", &address, &outcome) != EOF){
    	BRANCH_ADDRESS[i] = address;
	    BRANCH_OUTCOME[i] = outcome;

      i++;
    }
 }
 
 fclose(btrace);
}

void print_trace()
{
  printf("\n*** Printing the first 5 traces!\n");

  for(int i = 0; i < 5; i++){
  	printf("%u %c\n", BRANCH_ADDRESS[i], BRANCH_OUTCOME[i]);
  }
}

void always_taken()
{
  printf("\n*** AlwaysTaken BP \n");
  int hit = 0;
  for(int i = 0; i < LENGTH_TRACE_DATA; i++)
  {
    if (BRANCH_OUTCOME[i] == 'T')
    {
      hit++;
    }
  } // end of for-i

  // print to terminal to check the results
  //printf("%d \n", hit);
  float sillyResults = (  (float)hit * 100.0) / (float)LENGTH_TRACE_DATA;
  printf("%9.6f\n", sillyResults);
}
void not_taken()
{
  printf("\n*** NotTaken BP \n");
  int hit = 0;
  for(int i = 0; i < LENGTH_TRACE_DATA; i++)
  {
    if (BRANCH_OUTCOME[i] == 'N')
    {
      hit++;
    }
  } // end of for-i

  // print to terminal to check the results
  //printf("%d \n", hit);
  float sillyResults = (  (float)hit * 100.0 )/ (float)LENGTH_TRACE_DATA;
  printf("%9.6f\n", sillyResults);
}

void pht_init()
{
  int i;

  for (i = 0; i < NUM_ENTRIES; i++) {
    PHT1L[i] = 0;
  }
  for (i = 0; i < TWO_NUM_ENTRIES; i++) {
    PHT2G[i] = 0;
  }
  for (i = 0; i < TWO_NUM_ENTRIES; i++) {
    PHT2L[i] = 0;
  }
  for (i = 0; i < BTH_NUM_ENTRIES; i++) {
    BHT[i] = 0;
  }
  for (i = 0; i < NUM_ENTRIES; i++) {
    votingtable[i] = 0;
  }

}

bool one_level_bp(unsigned int addr, char outcome)
{
  int funIndex;
  bool hit = false;
  char portent = '0';

  // Looking at 16 LSBs
  unsigned int mask = (1 << SIZE) - 1;

    // funIndex = (addr >> 2) % numEntries; // #define ONE_LEVEL_SIZE 16
    funIndex = addr & mask;


      if (PHT1L[funIndex] == 1 ) {
        portent = 'T'; //taken
      }
      else {
        portent = 'N'; //not taken
      }

      // update the PHT
      if (outcome == portent) {
        hit = true;
      }
      else if (outcome == 'T') {
        PHT1L[funIndex] = 1;
      }
      else if (outcome == 'N') {
        PHT1L[funIndex] = 0;
      }
  return hit;
}

bool two_level_gbp(unsigned int addr, char outcome, unsigned int* _ghr)
{
  int  funIndex = 0; // reset the PHT index
  bool hit = false;
  char portent = '0';
  
  // Looking at 16 LSBs
  unsigned int mask = (1 << TWO_LEVEL_SIZE) - 1;
  unsigned int GHR_mask = (1 << GHR_SIZE_GLOBAL) - 1;

    funIndex = (  (addr << GHR_SIZE_GLOBAL) | (*_ghr & GHR_mask)  )  & mask;

  switch (outcome){
    case 'T':
      if (  (PHT2G[funIndex] == 0) || (PHT2G[funIndex] == 1)  ) { // correct prediction
        PHT2G[funIndex] = 0;   // update PHT by going to state 0
        hit = true;
        *_ghr = *_ghr | 1; // set last bit of GHR
      }
      else if (PHT2G[funIndex] == 2){ //incorrect prediction
        PHT2G[funIndex] = 1; // update PHT state
      }
      else if (PHT2G[funIndex] == 3){ //incorrect prediction
        PHT2G[funIndex] = 2; // update PHT state
      }
    break;
    case 'N':
      if (  (PHT2G[funIndex] == 2) || (PHT2G[funIndex] == 3)  ) { //correct prediction
        PHT2G[funIndex] = 3;   // update PHT state
        hit = true;
      }
      else if (PHT2G[funIndex] == 1){ //incorrect prediction
        PHT2G[funIndex] = 2; // update PHT state
      }
      else if (PHT2G[funIndex] == 0){ //incorrect prediction
        PHT2G[funIndex] = 1; // update PHT state
      }
  }
  *_ghr = *_ghr << 1; // shift left to make room for new GHR data

  return hit;
}

bool two_level_lbp(unsigned int addr, char outcome, unsigned int* _ghr)
{
  int funIndex = 0;   // reset the PHT index
  int sillyIndex = 0; // BHT index
  bool hit = false;
  char portent = '0';
  
  // Looking at 16 LSBs
  unsigned int mask = (1 << TWO_LEVEL_SIZE) - 1;
  unsigned int GHR_mask = (1 << GHR_SIZE_LOCAL) - 1; //FEED ME SEYMOR

    sillyIndex = (addr) % NUM_ENTRIES; // 2^16
    funIndex = (  (addr << GHR_SIZE_LOCAL) | (BHT[sillyIndex] & GHR_mask)  )  & mask;

  switch (outcome){
    case 'T':
      if (  (PHT2G[funIndex] == 0) || (PHT2G[funIndex] == 1)  ) { // correct prediction
        PHT2G[funIndex] = 0;   // update PHT by going to state 0
        hit = true;
        BHT[sillyIndex] = BHT[sillyIndex] | 1; // set last bit of GHR
      }
      else if (PHT2G[funIndex] == 2){ //incorrect prediction
        PHT2G[funIndex] = 1; // update PHT state
      }
      else if (PHT2G[funIndex] == 3){ //incorrect prediction
        PHT2G[funIndex] = 2; // update PHT state
      }
    break;
    case 'N':
      if (  (PHT2G[funIndex] == 2) || (PHT2G[funIndex] == 3)  ) { //correct prediction
        PHT2G[funIndex] = 3;   // update PHT state
        hit = true;
      }
      else if (PHT2G[funIndex] == 1){ //incorrect prediction
        PHT2G[funIndex] = 2; // update PHT state
      }
      else if (PHT2G[funIndex] == 0){ //incorrect prediction
        PHT2G[funIndex] = 1; // update PHT state
      }
  }
  BHT[sillyIndex] = BHT[sillyIndex] << 1; // shift left to make room for new GHR data

  return hit;
}

bool two_level_gshare(unsigned int addr, char outcome, unsigned int* _ghr)
{  
  int  funIndex = 0; // reset the PHT index
  bool hit = false;
  char portent = '0';
  
  // Looking at 16 LSBs
  unsigned int mask = (1 << TWO_LEVEL_SIZE) - 1;
  unsigned int GHR_mask = (1 << GHR_SIZE_GLOBAL) - 1;

  //  The algorithm of gshare is identical to two_level_global() with the exception
  //  of the fun Index below:
  //  funIndex = (  (addr & mask) ^ (GHR_SIZE_GLOBAL & mask)  );
  funIndex = 0xFFFF & (*_ghr ^ ((addr) & 0xFFFF));

  switch (outcome){
    case 'T':
      if (  (PHT2G[funIndex] == 0) || (PHT2G[funIndex] == 1)  ) { // correct prediction
        PHT2G[funIndex] = 0;   // update PHT by going to state 0
        hit = true;
        *_ghr = *_ghr | 1; // set last bit of GHR
      }
      else if (PHT2G[funIndex] == 2){ //incorrect prediction
        PHT2G[funIndex] = 1; // update PHT state
      }
      else if (PHT2G[funIndex] == 3){ //incorrect prediction
        PHT2G[funIndex] = 2; // update PHT state
      }
    break;
    case 'N':
      if (  (PHT2G[funIndex] == 2) || (PHT2G[funIndex] == 3)  ) { //correct prediction
        PHT2G[funIndex] = 3;   // update PHT state
        hit = true;
      }
      else if (PHT2G[funIndex] == 1){ //incorrect prediction
        PHT2G[funIndex] = 2; // update PHT state
      }
      else if (PHT2G[funIndex] == 0){ //incorrect prediction
        PHT2G[funIndex] = 1; // update PHT state
      }
  }
  *_ghr = *_ghr << 1; // shift left to make room for new GHR data

  return hit;
}

bool two_level_gskewed(bool globalpred, bool localpred, bool gsharepred)
{
  /*  The calling of this function calls functions that each fill their respective PHT as follows
    if (two_level_gskewed(two_level_gbp(BRANCH_ADDRESS[i], BRANCH_OUTCOME[i], &GHR),
                          two_level_lbp(BRANCH_ADDRESS[i], BRANCH_OUTCOME[i], &GHR),
                          two_level_gshare(BRANCH_ADDRESS[i], BRANCH_OUTCOME[i], &GHR)  ) ) {
    gskewed++;
  }; */
  bool hit = false;
  int majority = 0;

  if (globalpred) { majority++;  }
  else { majority--;  }
  if (localpred) { majority++;  }
  else { majority--;  }
  if (gsharepred) { majority++;  }
  else { majority--;  } 

  if (majority >= 2){
    hit = true;
  }
  return hit;
}