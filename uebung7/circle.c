#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <string.h>


// number of elements for a process
int proc_length(int N, int rank, int num_procs)
{
	int base = N / num_procs;
	int rest = N % num_procs;

	if(rank < rest)
	{
		base++;
	}
	
	return base;
}

int*
init (int N, int rank, int num_procs)
{
	int length = (N / num_procs) + 1;
  	int* buf = malloc(sizeof(int) * length);

  	srand(time(NULL) + rank);

	for (int i = 0; i < length; i++)
	{
		buf[i] = rand() % 25; //do not modify %25
	}

	return buf;
}

void circle (int *buf, int rank, int N, int num_procs)
{
	int up_rank, down_rank;

	int buf_size = (N / num_procs) + 1;
	int tmp_buf[buf_size];
 	memcpy(tmp_buf, buf, buf_size * sizeof(int));

	for(int i = 0; i < num_procs-1; i++)
	{	
		up_rank = ((rank + 1) % num_procs);
		down_rank = ((rank - 1) % num_procs);
		
		MPI_Send(tmp_buf, buf_size, MPI_INT, up_rank, 0, MPI_COMM_WORLD);
		MPI_Recv(tmp_buf, buf_size, MPI_INT, down_rank, 0, MPI_COMM_WORLD, NULL);
	}
  
 	memcpy(buf, tmp_buf, buf_size * sizeof(int));
}

void print_array(int *buf, int rank, int length)
{
	printf("rank %d: ", rank);
	for (int i = 0; i < length; i++)
	{
		printf (" %d ", buf[i]);
	}
	printf("\n");
}

void print_ordered(int *buf, int rank, int N, int num_procs)
{
	// length nach circle aufruf nicht immer korrekt	
	int length = proc_length(N, rank, num_procs);
	int mesg = 0;
	
	if(rank == 0)
	{
		print_array(buf, rank, length);

		if(rank < (num_procs - 1))
		{
			mesg = 1;
			MPI_Send(&mesg, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);		
		}
	}
	else
	{
		MPI_Recv(&mesg, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, NULL);
		print_array(buf, rank, length);
		
		if(rank < (num_procs - 1))
		{
			int mesg = 1;
			MPI_Send(&mesg, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
		}
	}	
}

int
main (int argc, char** argv)
{
	int rank, num_procs;
  
	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);


	char arg[256];

	if (argc < 2)
	{
		printf("Arguments error\n");
		MPI_Finalize();
		return EXIT_FAILURE;
	}

	sscanf(argv[1], "%s", arg);

	//array length
	int N = atoi(arg);
	int *buf = init(N, rank, num_procs);
	
	MPI_Barrier(MPI_COMM_WORLD);
	

	if(rank == 0)
	{
		printf("\nBEFORE\n");
	}
	print_ordered(buf, rank, N, num_procs);
	
	MPI_Barrier(MPI_COMM_WORLD);

	
	circle(buf, rank, N, num_procs);

	
	MPI_Barrier(MPI_COMM_WORLD);
	
	if(rank == 0)
	{
		printf("\nAFTER\n");
	}
	print_ordered(buf, rank, N, num_procs);

	
	MPI_Barrier(MPI_COMM_WORLD);


	
	MPI_Finalize();
	return EXIT_SUCCESS;
}
