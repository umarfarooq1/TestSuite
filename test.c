#include <stdio.h>
#include "mpi.h"

#define MAX_NAME_LEN 128
#define MAX_DESC_LEN 1024

static MPI_T_pvar_session session;
static MPI_T_pvar_handle *pvar_handles;
static int *pvar_index;
static int *pvar_count;
static unsigned long long int **pvar_value_buffer; // cumulative values
static void *read_value_buffer; // values are read into this buffer.
static int pvar_num_watched;
static int total_num_of_var;
static int max_num_of_state_per_pvar = -1; //  max_num_of_state_per_pvar = max(pvar_count[performance_variable]) for all performance_variables
static int num_mpi_tasks;
int main(int argc, char *argv[])
{
	int i,num_cvar,nameLen,verbosity,descLen,binding;
	int required=MPI_THREAD_SINGLE,provided,err,scope;
	char name[MAX_NAME_LEN],desc[MAX_DESC_LEN];
	MPI_T_enum enumtype;
	MPI_Datatype datatype;
	MPI_Init_thread(0,0,required,&provided);
	MPI_T_init_thread(required,&provided);
	MPI_T_cvar_get_num(&num_cvar);
	printf("%d MPI CONTROL variables\n",num_cvar);
	for(i = 0;i<num_cvar;i++){
                nameLen = sizeof(name);
                descLen = sizeof(desc);
                err = MPI_T_cvar_get_info(i,name,&nameLen,&verbosity,
                                        &datatype,&enumtype,desc,
                                        &descLen,&binding,&scope);
                printf("\t%-32s\t%s\n",name,desc);
        }
        MPI_T_finalize();
        MPI_Finalize();
        return 0;
}
