#include <stdio.h>
#include <string.h>
#include "mpi.h"

static MPI_T_pvar_session session; 
static MPI_T_pvar_handle handle; 
//create more handles and sessions. Read up on sessions to see how they need to proceed.
int main(int argc, char *argv[])
{
	int check1,check2,check3;
	int provided,err;
	int numPvar,nameLen,descLen,verbosity,varClass;
	int binding, isReadonly,isContinuous,isAtomic,i;
	char name[128],desc[1024];
	int len = 3;
	char *test[] = {"mpool_hugepage_bytes_allocated","pml_ob1_unexpected_msgq_length","pml_ob1_posted_recvq_length"};
	MPI_Comm comm;
	MPI_T_enum enumtype;
	MPI_Datatype datatype;

/*	err=MPI_Init(argc,argv); 
      	if (err!=MPI_SUCCESS) return err; 
        err=MPI_T_init_thread(MPI_THREAD_SINGLE,&provided); 
        if (err!=MPI_SUCCESS) return err;*/ 
	MPI_Init_thread(0,0,MPI_THREAD_SINGLE,&provided);
	err = MPI_T_init_thread(MPI_THREAD_SINGLE,&provided);
	if (err) MPI_Abort(MPI_COMM_WORLD,0);
	err = MPI_T_pvar_get_num(&numPvar);
	if (err) MPI_Abort(MPI_COMM_WORLD,0);
	printf("%d MPI Performance Variables\n",numPvar);
	for(i = 0;i<numPvar;i++){
		int index,count,value; //new vars
		nameLen = sizeof(name);
		descLen = sizeof(desc);
		err = MPI_T_pvar_get_info(i,name,&nameLen,&verbosity,
					&varClass,&datatype,&enumtype,desc,
					&descLen,&binding,&isReadonly,
					&isContinuous,&isAtomic);
		if (err) MPI_Abort(MPI_COMM_WORLD,0);
		err = MPI_T_pvar_session_create(&session);
		printf("i came till here\n");
		printf("err1 %d\n",err);
		if(err) return err;
		printf("haha\n");	
		comm = MPI_COMM_WORLD;
//		err=PMPI_T_pvar_handle_alloc(session, i, &comm, &handle, &count); 
// 		printf("err2 %d\n",err);
//		if (err) return err;
	//	assert(count==1);
		printf("count = %d\n",count);
//		err=MPI_T_pvar_start(session, handle);
//		printf("err3 %d\n",err);
//      		if (err) return err;
//		printf("hahahah2\n");
//		err=PMPI_T_pvar_read(session, handle, &value);
		printf("%s\n",test[i%3]);
		printf("Current value for variable is %d\n",value);

		printf("finalizing..\n");
		err=PMPI_T_pvar_handle_free(session, &handle);
	        err=PMPI_T_pvar_session_free(&session);

	//	check1 = strncmp(name,test1,15);
	//	check2 = strncmp(name,test2,15);
	//	check3 = strncmp(name,test3,15);
		int *pvar_index;
		//MPI_T_pvar_get_index(test[i],2,&pvar_index); 
		//printf("%s %d\n",test[i],pvar_index);
//		if(check1 == 0 || check2 == 0 || check3 == 0){
//			printf("\t%s\tClass=%d\tReadonly=%s\tContinuous=%s\tAtomic=%s\t%s\n",
//				name,varClass,isReadonly ? "T" : "F",
//				isContinuous ? "T" : "F",isAtomic?"T" : "F",desc);
//			printf("%d\n",i);
//		}
	}
	
	//err = 
	MPI_T_finalize();
	MPI_Finalize();
	return 0;
}
