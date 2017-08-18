/* File:       
 *    mpi_hello.c
 *
 * Purpose:    
 *    A "hello,world" program that uses MPI
 *
 * Compile:    
 *    mpicc -g -Wall -std=C99 -o mpi_hello mpi_hello.c
 * Usage:        
 *    mpiexec -n<number of processes> ./mpi_hello
 *
 * Input:      
 *    None
 * Output:     
 *    A greeting from each process
 *
 * Algorithm:  
 *    Each process sends a message to process 0, which prints 
 *    the messages it has received, as well as its own message.
 *
 * IPP:  Section 3.1 (pp. 84 and ff.)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  /* For strlen             */
#include <mpi.h>     /* For MPI functions, etc */ 
#include <assert.h>

const int MAX_STRING = 100;

static MPI_T_pvar_session session;
static MPI_T_pvar_handle handle;
static MPI_T_pvar_handle handle1;

int MPI_Init( int *argc, char ***argv );
int MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag,MPI_Comm comm);
int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status);
int MPI_Finalize( void );

int MPI_Init( int *argc, char ***argv )
{
  int err, num, i, index, namelen, verbosity,index1;//,index2; 
  int var_class, bind, threadsup; 
  int readonly, continuous, atomic, count,count1;//,count2; 
  char name[32]; 
  MPI_Comm comm,comm1;//,comm2; 
  MPI_Datatype datatype; 
  MPI_T_enum enumtype; 
 
  err=PMPI_Init(argc,argv); 
  if (err!=MPI_SUCCESS) return err; 
 
  err=PMPI_T_init_thread(MPI_THREAD_SINGLE,&threadsup); 
  if (err!=MPI_SUCCESS) return err; 
 
  err=PMPI_T_pvar_get_num(&num); 
  if (err!=MPI_SUCCESS) return err;  
  index=-1; 
  i=0; 
  while ((i<num) && (index<0) && (err==MPI_SUCCESS)) { 
    /* Pass a buffer that is at least one character longer than */ 
    /* the name of the variable being searched for to avoid */ 
    /* finding variables that have a name that has a prefix */ 
    /* equal to the name of the variable being searched. */ 
    namelen=32; 
    err=PMPI_T_pvar_get_info(i, name, &namelen, &verbosity,&var_class, &datatype, &enumtype, NULL, NULL, &bind,&readonly, &continuous, &atomic); 
    if (strcmp(name,"pml_ob1_unexpected_msgq_length")==0) index=i;     
    i++; } 
  if (err!=MPI_SUCCESS) return err;  
 
  /* this could be handled in a more flexible way for a generic tool */ 
  assert(index>=0); 
  index1=-1;
  i=0;
  while ((i<num) && (index1<0) && (err==MPI_SUCCESS)) {
    /* Pass a buffer that is at least one character longer than */
    /* the name of the variable being searched for to avoid */
    /* finding variables that have a name that has a prefix */
    /* equal to the name of the variable being searched. */
    namelen=32;
    err=PMPI_T_pvar_get_info(i, name, &namelen, &verbosity,&var_class, &datatype, &enumtype, NULL, NULL, &bind,&readonly, &continuous, &atomic);
    if (strcmp(name,"pml_ob1_posted_recvq_length")==0) index1=i;
    i++; }
  if (err!=MPI_SUCCESS) return err;
 
  /* this could be handled in a more flexible way for a generic tool */
  assert(index1>=0);

 /* Create a session */ 
 err=PMPI_T_pvar_session_create(&session); 
 if (err!=MPI_SUCCESS) return err; 
 /*Get a handle and bind to MPI_COMM_WORLD */ 
 comm=MPI_COMM_WORLD;  
 err=PMPI_T_pvar_handle_alloc(session, index, &comm, &handle, &count); 
 if (err!=MPI_SUCCESS) return err; 
 comm1=MPI_COMM_WORLD;
 err=PMPI_T_pvar_handle_alloc(session, index1, &comm1, &handle1, &count1);
 if (err!=MPI_SUCCESS) return err;

 /* Start variable */ 
 err=PMPI_T_pvar_start(session, handle); 
 if (err!=MPI_SUCCESS) return err;  
 err=PMPI_T_pvar_start(session, handle1);
 if (err!=MPI_SUCCESS) return err;
 return MPI_SUCCESS; 
}

int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag,MPI_Comm comm, MPI_Status *status)
{
  printf("Before Recv: performance related routines here\n");  
  int value,value1, err; 
  if (comm==MPI_COMM_WORLD) { 
  	err=PMPI_T_pvar_read(session, handle, &value);
	printf("This is the value of the variable %d\n",value);
	err=PMPI_T_pvar_read(session, handle1, &value1);
        printf("This is the value of the variable 1 %d\n",value1);

  }
  return PMPI_Recv(buf, count, datatype, source, tag, comm, status);
}

int MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag,MPI_Comm comm)
{
  printf("Before Send: performance related routines here\n");  
  return PMPI_Send(buf, count, datatype, dest, tag, comm);
}

int main(int argc, char *argv[]) {
   char       greeting[MAX_STRING];  /* String storing message */
   int        comm_sz;               /* Number of processes    */
   int        my_rank;               /* My process rank        */

   /* Start up MPI */
   MPI_Init(&argc, &argv); 

   /* Get the number of processes */
   MPI_Comm_size(MPI_COMM_WORLD, &comm_sz); 

   /* Get my rank among all the processes */
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 

   if (my_rank != 0) { 
      /* Create message */
      sprintf(greeting, "Greetings from process %d of %d!", 
            my_rank, comm_sz);
      /* Send message to process 0 */
      MPI_Send(greeting, strlen(greeting)+1, MPI_CHAR, 0, 0,
            MPI_COMM_WORLD); 
   } else {  
      /* Print my message */
      printf("Greetings from process %d of %d!\n", my_rank, comm_sz);
      for (int q = 1; q < comm_sz; q++) {
         /* Receive message from process q */
         MPI_Recv(greeting, MAX_STRING, MPI_CHAR, q,
            0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
         /* Print message from process q */
         printf("%s\n", greeting);
      } 
   }

   /* Shut down MPI */
   MPI_Finalize(); 

   return 0;
}  /* main */
