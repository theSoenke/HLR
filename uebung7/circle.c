#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <string.h>

static int cur_data_width = 0;

// number of elements for a process
int proc_length(int N, int rank, int num_procs, int steps)
{
	int base = N / num_procs;
	int rest = N % num_procs;
	int tmp_rank = (rank + steps) % num_procs;

	if(tmp_rank < rest)
	{
		base++;
	}
	
	return base;
}

int*
init (int N, int rank, int num_procs)
{
	cur_data_width = proc_length(N, rank, num_procs, 0);
	int length = (N / num_procs) + 1;
  	int* buf = malloc(sizeof(int) * length);

  	srand(time(NULL) + rank);

	for (int i = 0; i < length; i++)
	{
		buf[i] = rand() % 25; //do not modify %25
	}

	return buf;
}

void circle (int *buf,int rank, int N, int num_procs)
{
	int up_rank, down_rank;

	int buf_size = (N / num_procs) + 1;
	int tmp_buf[buf_size];
 	memcpy(tmp_buf, buf, buf_size * sizeof(int));
	int termination;

	if(rank == 0)
	{
		MPI_Send(buf, 1, MPI_INT, num_procs-1, 1, MPI_COMM_WORLD);
	}

	if(rank == num_procs-1)
	{
		MPI_Recv(&termination, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, NULL);
	}

	MPI_Barrier(MPI_COMM_WORLD);

	int steps = 0;
	int running = 1;
	int i = 0;
	while(running && i < num_procs-1)
	{	
		up_rank = ((rank + 1) % num_procs);
		down_rank = ((rank - 1) % num_procs);

		MPI_Send(tmp_buf, buf_size, MPI_INT, up_rank, 0, MPI_COMM_WORLD);
		MPI_Recv(tmp_buf, buf_size, MPI_INT, down_rank, 0, MPI_COMM_WORLD, NULL);

		if(rank == num_procs-1 && tmp_buf[0] == termination)
		{
			running = 0;			
		}

        	MPI_Bcast(&running, 1, MPI_INT, num_procs-1, MPI_COMM_WORLD);
		steps++;
	}
  
	cur_data_width = proc_length(N, rank, num_procs, steps);
 	memcpy(buf, tmp_buf, buf_size * sizeof(int));
}

void print_data(int *buf, int rank)
{
	printf("rank %d: ", rank);
	for (int i = 0; i < cur_data_width; i++)
	{
		printf (" %d ", buf[i]);
	}
	printf("\n");
}

// every process prints data in order
void print_ordered(int *buf, int rank, int num_procs)
{
	for(int i = 0; i < num_procs; i++) 
	{
	    MPI_Barrier(MPI_COMM_WORLD);
	    if (i == rank) 
	    {
		 print_data(buf, rank);
	    }
	}	
}

// rank 0 gathers and prints all data
void print_ordered2(int *buf,int N, int rank, int num_procs)
{
	int *buffer;

	if(rank == 0)
	{	
		buffer = malloc(sizeof(int) * N);
	}

	int data_width = proc_length(N, rank, num_procs, 0);
	MPI_Gather(buf, data_width, MPI_INT, 
	            buffer, data_width, MPI_INT, 0, MPI_COMM_WORLD);

	if(rank == 0)
	{
		for(int i = 0; i < N; i++)
		{
			printf("%d ", *(buffer + i));
		}
		
		printf("\n");
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
	//print_ordered(buf, rank, num_procs);
	print_ordered2(buf, N, rank, num_procs);
	
	MPI_Barrier(MPI_COMM_WORLD);

	
	circle(buf, rank, N, num_procs);

	
	MPI_Barrier(MPI_COMM_WORLD);
	
	if(rank == 0)
	{
		printf("\nAFTER\n");
	}
	//print_ordered(buf, rank, num_procs);
	print_ordered2(buf, N, rank, num_procs);
	
	
	MPI_Barrier(MPI_COMM_WORLD);


	
	MPI_Finalize();
	return EXIT_SUCCESS;
}
