#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <mpi.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char** argv)
{
	int length = 400;

	MPI_Init(&argc, &argv);

	int num_processes;
	MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);


	char output[length];
	char *buffer;

	if(rank == 0)
	{	
		buffer = malloc(sizeof(char) * length * num_processes);
	}
	
	
	// Get Hostname and time
	struct timeval tv;
	gettimeofday(&tv, NULL);
	
	char time[40];
	strftime(time, 40, "%Y-%m-%d %T", localtime(&tv.tv_sec));
	char hostname[200];
	gethostname(hostname, sizeof(hostname));
	snprintf(output, sizeof(output), "%s: %s.%ld", hostname, time, tv.tv_usec);

    // Gather hostname and time into buffer
	MPI_Gather(&output, length, MPI_CHAR, 
	            buffer, length, MPI_CHAR, 0, MPI_COMM_WORLD);
	            
    int min_micro;
    MPI_Reduce(&tv.tv_usec, &min_micro, 1, MPI_INT, MPI_MIN,
	       0, MPI_COMM_WORLD);
	       
	if(rank == 0 && num_processes >= 1)
	{
		for(int i = 0; i < num_processes; i++)
		{
			printf("%s\n", buffer + (length * i));
		}
		printf("%d\n", min_micro);
	}

	MPI_Barrier(MPI_COMM_WORLD);

	printf("Rang %d beendet jetzt!\n", rank);

	MPI_Finalize();
	return 0;
}
