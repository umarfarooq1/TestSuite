#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#define ALT_MPI_SENDRECV 0

int Alt_MPI_Sendrecv(void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag,
                     void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status){
#if ALT_MPI_SENDRECV
    MPI_Request irequests[2];
    MPI_Status istatuses[2];    
    MPI_Irecv( recvbuf, recvcount, recvtype, source, recvtag, MPI_COMM_WORLD, &irequests[0] );
    MPI_Isend( sendbuf, sendcount, sendtype, dest, sendtag, MPI_COMM_WORLD, &irequests[1] );
    MPI_Waitall(2, irequests, istatuses);
#else
    return MPI_Sendrecv(sendbuf, sendcount, sendtype, dest, sendtag,
                        recvbuf, recvcount, recvtype, source, recvtag, MPI_COMM_WORLD, status);
#endif
    
}


/*
 * get_index
 *
 */
int get_index(int row, int col, int sp)
{
   return ((row+sp)%sp)*sp + (col+sp)%sp;
}





/*
 * random_A_B
 *
 */
void random_A_B(float** A, float** B, float** C, int dg)
{
   int i,j;

    srand((unsigned int)time(NULL));     

    for(i=0; i<dg ; i++)
      for(j=0; j<dg ; j++)
	  {
	    A[i][j] = rand();
        B[i][j] = rand();
        C[i][j] = 0.0;
	  }
}






/* scatter_A_B
 * 
 */
void scatter_A_B(int p, int sp, int dl, float* tmp_a, float** A, float* tmp_b, float **B, float* a, int dl2, float* b)
{
   int i,j,k,l;
   int p_imin,p_imax,p_jmin,p_jmax;

   for(k=0; k<p; k++)

   {

	  p_jmin = (k % sp    ) * dl;
  	  p_jmax = (k % sp + 1) * dl-1;
	  p_imin = (k - (k % sp))/sp * dl;
	  p_imax = ((k - (k % sp))/sp +1) *dl -1;
          l = 0;

      /*rank=0*/
      for(i=p_imin; i<=p_imax; i++)
      {
      	  for(j=p_jmin; j<=p_jmax; j++)
      	  {
              tmp_a[l] = A[i][j];
	      tmp_b[l] = B[i][j];
	      l++;
          }
      }

      /*rank=0*/
      if(k==0)
      {
         memcpy(a, tmp_a, dl2 * sizeof(float));
	 memcpy(b, tmp_b, dl2 * sizeof(float));
      } else   /*rank=0*/
      {
          MPI_Send(tmp_a, dl2, MPI_FLOAT, k, 1, MPI_COMM_WORLD);
	  MPI_Send(tmp_b, dl2, MPI_FLOAT, k, 2, MPI_COMM_WORLD);
      }
   }
}






/*
 * init_alignment
 *
 */  
void init_alignment(float* a, int dl2, int my_row, int my_col, int sp, float* tmp_a, MPI_Status status, float* b, float*tmp_b)
{
   Alt_MPI_Sendrecv(a, dl2, MPI_FLOAT, get_index(my_row,my_col-my_row,sp), 1,
            tmp_a, dl2, MPI_FLOAT, get_index(my_row,my_col+my_row,sp), 1, MPI_COMM_WORLD, &status);
   memcpy(a, tmp_a, dl2 * sizeof(float) );


   Alt_MPI_Sendrecv(b, dl2, MPI_FLOAT, get_index(my_row-my_col,my_col,sp), 1,
            tmp_b, dl2, MPI_FLOAT, get_index(my_row+my_col,my_col,sp), 1, MPI_COMM_WORLD, &status);
   memcpy(b, tmp_b, dl2 * sizeof(float) );
}





/*
 * main_shift
 *
 */ 
void main_shift(int sp, int dl, float* c, float* a, float* b, int dl2, int my_row, int my_col, float* tmp_a, MPI_Status status, float* tmp_b)
{
   int i,j,k,l;

   for(l=0; l<sp; l++)
   {

     for(i=0; i<dl; i++)
       for(j=0; j<dl; j++)
         for(k=0; k<dl; k++)
           c[i*dl+j] += a[i*dl+k]*b[k*dl+j];


      Alt_MPI_Sendrecv(a , dl2, MPI_FLOAT, get_index(my_row, my_col-1, sp), 1, 
             tmp_a, dl2, MPI_FLOAT, get_index(my_row, my_col+1, sp), 1, MPI_COMM_WORLD, &status);

      Alt_MPI_Sendrecv(b , dl2, MPI_FLOAT, get_index(my_row-1, my_col, sp), 1, 
     tmp_b , dl2, MPI_FLOAT, get_index(my_row+1, my_col, sp), 1, MPI_COMM_WORLD, &status);
   }
}





/*
 * collect_c
 */
void collect_C(int dl, float** C, float* c, int p, int dl2, MPI_Status status, int sp)
{
   int i,j,i2,j2,k;
   int p_imin,p_imax,p_jmin,p_jmax; 


   for (i=0;i<dl;i++)
	 for(j=0;j<dl;j++)
	   C[i][j]=c[i*dl+j];

   for (k=1;k<p;k++)
   {

       MPI_Recv(c, dl2, MPI_FLOAT, k, 1, MPI_COMM_WORLD, &status);

       p_jmin = (k % sp    ) *dl;
       p_jmax = (k % sp + 1) *dl-1;
       p_imin =  (k - (k % sp))/sp     *dl;
       p_imax = ((k - (k % sp))/sp +1) *dl -1;

       i2=0;

       for(i=p_imin; i<=p_imax; i++)
       {
           j2=0;
           for(j=p_jmin; j<=p_jmax; j++)
           {
               C[i][j]=c[i2*dl+j2];
               j2++;
           }
           i2++;
       }
   }
}






/*print
 */
void print(float **m,char *str, int dg)
{
   int i,j;
   printf("%s",str);

   for(i=0;i<dg;i++)
   {
       for(j=0;j<dg;j++)
           printf("%15.0f    ",m[i][j]);
       printf("\n");
   }
   printf("\n");
}




/*
 * main
 */


int main(int argc,  char *argv[])
{
   int i;

//   int ret;
//   int err, numPvar;
//   int provided;   

   float **A, **B, **C;              /* C = A * B */
   float *a, *b, *c, *tmp_a, *tmp_b; 
   int dg, dl, dl2,p, sp;            
   double starttime;
   double time1,time2;
   int my_rank, my_row, my_col;
   MPI_Status status;
   MPI_Init(&argc, &argv);                  
   MPI_Comm_size(MPI_COMM_WORLD, &p);       
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 

   sp = sqrt(p);

//   ret =  MPI_T_init_thread(MPI_THREAD_SINGLE, &provided);

//	MPI_Init_thread(MPI_THREAD_SINGLE,&provided);
//        err = MPI_T_init_thread(MPI_THREAD_SINGLE,&provided);
//        if (err) MPI_Abort(MPI_COMM_WORLD,0);
//        err = MPI_T_cvar_get_num(&numPvar);
//        if (err) MPI_Abort(MPI_COMM_WORLD,0);
//        printf("%d MPI Performance Variables\n",numPvar);


   if (sp*sp != p)
   {
       printf("Number of processors is not a quadratic number!\n");       
       exit(-1);
   }

   if (argc != 2)
   {
       printf("please input the dimension of the matrix e.g. 2048, 4096, 8192, 16384, 32768\n");
       exit(-1);
   }
   starttime = MPI_Wtime();
   dg  =  atoi(argv[1]);    
   dl  = dg / sp;          
   dl2 = dl * dl;


   my_col =  my_rank % sp ;
   my_row = (my_rank-my_col) / sp ;


   a = (float *)malloc( dl2 * sizeof(float) );
   b = (float *)malloc( dl2 * sizeof(float) );
   c = (float *)malloc( dl2 * sizeof(float) );


   for(i=0; i<dl2 ; i++)
     c[i] = 0.0;


   tmp_a = (float *)malloc( dl2 * sizeof(float) );
   tmp_b = (float *)malloc( dl2 * sizeof(float) );

   if (my_rank == 0)
   {

      A = (float **)malloc( dg * sizeof(float*) );
      B = (float **)malloc( dg * sizeof(float*) );
      C = (float **)malloc( dg * sizeof(float*) );

      for(i=0; i<dg; i++)
      {
         A[i] = (float *)malloc( dg * sizeof(float) );
         B[i] = (float *)malloc( dg * sizeof(float) );
         C[i] = (float *)malloc( dg * sizeof(float) );
      }
      random_A_B(A,B,C,dg);     
      scatter_A_B(p,sp,dl,tmp_a,A,tmp_b,B,a,dl2,b);    
   } else               
   {
       MPI_Recv(a, dl2, MPI_FLOAT, 0 , 1, MPI_COMM_WORLD, &status);
       MPI_Recv(b, dl2, MPI_FLOAT, 0 , 2, MPI_COMM_WORLD, &status);
   }
   time1=MPI_Wtime();
   init_alignment(a,dl2,my_row,my_col,sp,tmp_a,status,b,tmp_b);    

   main_shift(sp, dl, c, a, b, dl2, my_row, my_col, tmp_a, status, tmp_b);       
   if(my_rank == 0)
   {
     collect_C(dl, C, c, p, dl2, status, sp);       
     //     print(A,"random matrix A : \n", dg);  
     //     print(B,"random matrix B : \n", dg);  
     //     print(C,"Matrix C = A * B : \n", dg);     

   } else
   {
      MPI_Send(c,dl2,MPI_FLOAT,0,1,MPI_COMM_WORLD); 
   }
   time2=MPI_Wtime();
   if (my_rank == 0)
    {
      printf("mpiexec -n %d %s %s : Total running time = %f second\n",  p, argv[0], argv[1], time2-starttime);
      printf("mpiexec -n %d %s %s : Distribute data time = %f second\n", p, argv[0], argv[1], time1-starttime);
      printf("mpiexec -n %d %s %s : Parallel compute time = %f second\n", p, argv[0], argv[1], time2-time1);

//      printf("MPI_T_init_thread() ret=%d, provided=%d\n", ret, provided);
//      ret = MPI_T_cvar_get_num(&num_cvar);
//      printf("MPI_T_cvar_get_num ret=%d, num_cvar=%d\n", ret, num_cvar);
//      ret = MPI_T_pvar_get_num(&num_pvar);
//      printf("MPI_T_pvar_get_num ret=%d, num_pvar=%d\n", ret, num_pvar);

//      MPI_T_finalize();
    }

   MPI_Finalize();                     

   return 0;
}

