/****************************************************************************/
/****************************************************************************/
/**                                                                        **/
/**                 TU München - Institut für Informatik                   **/
/**                                                                        **/
/** Copyright: Prof. Dr. Thomas Ludwig                                     **/
/**            Andreas C. Schmidt                                          **/
/**                                                                        **/
/** File:      jacobi_mpi.c                                                **/
/**                                                                        **/
/** Purpose:   Partial differential equation solver for Jacobi method.     **/
/**                                                                        **/
/****************************************************************************/
/****************************************************************************/

/* ************************************************************************ */
/* Include standard header file.                                            */
/* ************************************************************************ */
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include <malloc.h>
#include <sys/time.h>
#include <mpi.h>

#include "partdiff-par.h"

int rank;
int num_procs;
int m_size;
int m_from;
int m_to;

struct calculation_arguments
{
    uint64_t  N;              /* number of spaces between lines (lines=N+1)     */
    uint64_t  num_matrices;   /* number of matrices                             */
    double    h;              /* length of a space between two lines            */
    double    ***Matrix;      /* index matrix used for addressing M             */
    double    *M;             /* two matrices with real values                  */
};

struct calculation_results
{
    uint64_t  m;
    uint64_t  stat_iteration; /* number of current iteration                    */
    double    stat_precision; /* actual precision of all slaves in iteration    */
};

/* ************************************************************************ */
/* Global variables                                                         */
/* ************************************************************************ */

/* time measurement variables */
struct timeval start_time;       /* time when program started                      */
struct timeval comp_time;        /* time when calculation completed                */


/* ************************************************************************ */
/* Hilfsfunktionen                                                          */
/* ************************************************************************ */
static
int
min(int a, int b) {
    if(a < b) return a;
    return b;
}

/* ************************************************************************ */
/* initVariables: Initializes some global variables                         */
/* ************************************************************************ */
static
void
initVariables (struct calculation_arguments* arguments, struct calculation_results* results, struct options const* options)
{
    arguments->N = (options->interlines * 8) + 9 - 1;
    arguments->num_matrices = 2;
    arguments->h = 1.0 / arguments->N;

    results->m = 0;
    results->stat_iteration = 0;
    results->stat_precision = 0;


    uint64_t  N = arguments->N;
    m_size = ceil((float)(N-1) / num_procs);
    m_from = min((m_size * rank + 1), N);
    m_to = min((m_size * (rank+1)), N-1);
    if(m_from > m_to) {
        m_from = N;
        m_to = N-1;
    }
    m_size = m_to - m_from + 1;
}

/* ************************************************************************ */
/* freeMatrices: frees memory for matrices                                  */
/* ************************************************************************ */
static
void
freeMatrices (struct calculation_arguments* arguments)
{
    uint64_t i;

    for (i = 0; i < arguments->num_matrices; i++)
    {
        free(arguments->Matrix[i]);
    }

    free(arguments->Matrix);
    free(arguments->M);
}

/* ************************************************************************ */
/* allocateMemory ()                                                        */
/* allocates memory and quits if there was a memory allocation problem      */
/* ************************************************************************ */
static
void*
allocateMemory (size_t size)
{
    void *p;

    if ((p = malloc(size)) == NULL)
    {
        printf("Speicherprobleme! (%" PRIu64 " Bytes)\n", size);
        /* exit program */
        exit(1);
    }

    return p;
}

/* ************************************************************************ */
/* allocateMatrices: allocates memory for matrices                          */
/* ************************************************************************ */
static
void
allocateMatrices (struct calculation_arguments* arguments)
{
    uint64_t i, j;

    uint64_t const N = arguments->N;

    arguments->M = allocateMemory(arguments->num_matrices * (N + 1) * (m_size + 2) * sizeof(double));
    arguments->Matrix = allocateMemory(arguments->num_matrices * sizeof(double**));

    for (i = 0; i < arguments->num_matrices; i++)
    {
        arguments->Matrix[i] = allocateMemory((N + 1) * sizeof(double*));

        for (j = 0; j <= N; j++)
        {
            arguments->Matrix[i][j] = arguments->M + (i * (m_size + 2) * (N + 1)) + (j * (N + 1));
        }
    }
}

/* ************************************************************************ */
/* initMatrices: Initialize matrix/matrices and some global variables       */
/* ************************************************************************ */
static
void
initMatrices (struct calculation_arguments* arguments, struct options const* options)
{
    uint64_t g, i, j;                                /*  local variables for loops   */

    uint64_t const N = arguments->N;
    double const h = arguments->h;
    double*** Matrix = arguments->Matrix;

    uint64_t const m_height = m_size + 2;


    /* initialize matrix/matrices with zeros */
    for (g = 0; g < arguments->num_matrices; g++)
    {
        for (i = 0; i < m_height; i++)
        {
            for (j = 0; j < N; j++)
            {
                Matrix[g][i][j] = 0.0;
            }
        }
    }

    /* initialize borders, depending on function (function 2: nothing to do) */
    if (options->inf_func == FUNC_F0)
    {
        for (g = 0; g < arguments->num_matrices; g++)
        {
            /* Links und Rechts */
            for (i = 0; i < m_height; i++)
            {
                Matrix[g][i][0] = 1.0 - (h * (i + m_from - 1));
                Matrix[g][i][N] = h * (i + m_from - 1);
            }
            /* Oberste Zeile */
            if(m_from == 1) {
                for (i = 0; i < N; i++)
                {
                    Matrix[g][0][i] = 1.0 - (h * i);
                }
                Matrix[g][0][N] = 0.0;
            }
            /* Unterste Zeile */
            if((uint64_t)m_to >= N-1) {
                for (i = 0; i < N; i++)
                {
                    Matrix[g][m_size+1][i] = h * i;
                }
                Matrix[g][m_size+1][0] = 0.0;
            }
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
}


/* ************************************************************************ */
/* Tauscht die doppelten Zeilen unter den Prozessen aus  (Jacobi)           */
/* ************************************************************************ */
static
void
shareValues(struct calculation_arguments const* arguments, struct options const* options, int m_in, int iteration) {
    int const elements = 8 * options->interlines + 9;

    MPI_Status status;
    MPI_Request send_request;

    int rank_bef, rank_aft;
    rank_bef = rank - 1;
    rank_aft = rank + 1;

    double** Matrix = arguments->Matrix[m_in];

    if(rank_bef >= 0) {
        MPI_Isend(Matrix[1], elements, MPI_DOUBLE, rank_bef, iteration*num_procs + rank, MPI_COMM_WORLD, &send_request);
        MPI_Recv(Matrix[0], elements, MPI_DOUBLE, rank_bef, iteration*num_procs + rank_bef, MPI_COMM_WORLD, &status);
    }

    if(rank_aft < num_procs) {
        MPI_Isend(Matrix[m_size], elements, MPI_DOUBLE, rank_aft, iteration*num_procs + rank, MPI_COMM_WORLD, &send_request);
        MPI_Recv(Matrix[m_size + 1], elements, MPI_DOUBLE, rank_aft, iteration*num_procs + rank_aft, MPI_COMM_WORLD, &status);
    }
}


/* ************************************************************************ */
/* Empfängt die Daten die zur nächsten Berechnung benötigt werden           */
/* Iteration (0 ... n) ist ein aufsteigender Wert. Nicht verbl. Iteration   */
/* ************************************************************************ */
static
void
recvValues(struct calculation_arguments const* arguments, struct options const* options, int m_in, int iteration) {
    int const elements = 8 * options->interlines + 9;

    MPI_Status status;

    int rank_bef, rank_aft;
    rank_bef = rank - 1;
    rank_aft = rank + 1;

    double** Matrix = arguments->Matrix[m_in];

    /* Vom vorherigen Rang empfangen.*/
    if(rank_bef >= 0) {
        MPI_Recv(Matrix[0], elements, MPI_DOUBLE, rank_bef, iteration, MPI_COMM_WORLD, &status);
    }

    /* Vom nachfolgenden Rang empfangen. Erst nach erster Iteration */
    if(rank_aft < num_procs && iteration > 0) {
        MPI_Recv(Matrix[m_size + 1], elements, MPI_DOUBLE, rank_aft, iteration, MPI_COMM_WORLD, &status);
    }
}

/* ************************************************************************ */
/* Sendet die Daten der letzten Berechnung                                  */
/* Iteration (0 ... n) ist ein aufsteigender Wert. Nicht verbl. Iteration   */
/* ************************************************************************ */
static
void
sendValues(struct calculation_arguments const* arguments, struct options const* options, int m_in, int iteration) {
    int const elements = 8 * options->interlines + 9;

    int rank_bef, rank_aft;
    rank_bef = rank - 1;
    rank_aft = rank + 1;

    double** Matrix = arguments->Matrix[m_in];

    if(rank_bef >= 0) {
        MPI_Request request;
        MPI_Isend(Matrix[1], elements, MPI_DOUBLE, rank_bef, iteration+1, MPI_COMM_WORLD, &request);
        MPI_Request_free(&request);
    }

    if(rank_aft < num_procs) {
        MPI_Request request;
        MPI_Isend(Matrix[m_size], elements, MPI_DOUBLE, rank_aft, iteration, MPI_COMM_WORLD, &request);
        MPI_Request_free(&request);
    }
}

/* ************************************************************************ */
/* calculate: solves the equation                                           */
/* ************************************************************************ */
static
void
calculate (struct calculation_arguments const* arguments, struct calculation_results *results, struct options const* options)
{
    int i, j;                                   /* local variables for loops  */
    int m1, m2;                                 /* used as indices for old and new matrices       */
    double star;                                /* four times center value minus 4 neigh.b values */
    double residuum;                            /* residuum of current iteration                  */
    double maxresiduum;                         /* maximum residuum value of a slave in iteration */

    int const N = arguments->N;
    double const h = arguments->h;

    double pih = 0.0;
    double fpisin = 0.0;

    int iterations_nr = 0;

    int term_iteration = options->term_iteration;

    /* Will be changed for precision, to finish the calculation */
    uint64_t local_termination = options->termination;

    /* initialize m1 and m2 depending on algorithm */
	if (options->method == METH_JACOBI)
	{
		m1 = 0;
		m2 = 1;
	}
	else
	{
		m1 = 0;
		m2 = 0;
	}

    if (options->inf_func == FUNC_FPISIN)
    {
        pih = PI * h;
        fpisin = 0.25 * TWO_PI_SQUARE * h * h;
    }

    while (term_iteration > 0)
    {

        double** Matrix_Out = arguments->Matrix[m1];
        double** Matrix_In  = arguments->Matrix[m2];

        if (options->method == METH_GAUSS_SEIDEL)
    	{
            /* Empfangen der zusätzlichen zeilen */
            recvValues(arguments, options, m1, iterations_nr);
        }

        maxresiduum = 0;

        /* over all rows */
        for (i = 1; i < (m_size + 1); i++)
        {
            double fpisin_i = 0.0;

            if (options->inf_func == FUNC_FPISIN)
            {
                fpisin_i = fpisin * sin(pih * (double)(i + m_from - 1));
            }

            /* over all columns */
            for (j = 1; j < N; j++)
            {
                star = 0.25 * (Matrix_In[i-1][j] + Matrix_In[i][j-1] + Matrix_In[i][j+1] + Matrix_In[i+1][j]);

                if (options->inf_func == FUNC_FPISIN)
                {
                    star += fpisin_i * sin(pih * (double)j);
                }

                if (options->termination == TERM_PREC || term_iteration == 1)
                {
                    residuum = Matrix_In[i][j] - star;
                    residuum = (residuum < 0) ? -residuum : residuum;
                    maxresiduum = (residuum < maxresiduum) ? maxresiduum : residuum;
                }

                Matrix_Out[i][j] = star;
            }
        }

        if (options->method == METH_JACOBI)
    	{
            MPI_Allreduce(MPI_IN_PLACE, &maxresiduum, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

            /* Austauschen der zusätzlichen zeilen */
            shareValues(arguments, options, m1, term_iteration);
        }
        else if (options->method == METH_GAUSS_SEIDEL)
    	{
            /* Senden der zusätzlichen zeilen */
            sendValues(arguments, options, m1, iterations_nr);

            /* Allreduce nur solange möglich */
            if(iterations_nr + rank >= num_procs) {
                if((local_termination == TERM_ITER && rank < term_iteration)
                    /* Der erste Prozess ist noch nicht fertig */
                    || local_termination == TERM_PREC
                    /* Es steht noch auf PREC also auch erster Rang nicht fertig. */)
                {
                    MPI_Allreduce(MPI_IN_PLACE, &maxresiduum, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
                }
            }
        }

        results->stat_iteration++;
        results->stat_precision = maxresiduum;

        /* exchange m1 and m2 */
        i = m1;
        m1 = m2;
        m2 = i;


        /* check for stopping calculation, depending on termination method */
        if (local_termination == TERM_PREC)
        {
            if (maxresiduum < options->term_precision
                && iterations_nr + rank >= num_procs
                /* Jeder Rang hat mindestens eine iteration ausgeführt */)
            {
                /* Restliche Iterationen ausführen */
                local_termination = TERM_ITER;
                term_iteration = rank;
            }
        }
        else if (local_termination == TERM_ITER)
        {
            term_iteration--;
        }

        iterations_nr++;
    }

    if (options->method == METH_GAUSS_SEIDEL)
    {
        MPI_Barrier(MPI_COMM_WORLD); /* k.a. ob benötigt */

        MPI_Allreduce(MPI_IN_PLACE, &maxresiduum, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
        results->stat_precision = maxresiduum;
    }

    results->m = m2;
}

/* ************************************************************************ */
/*  displayStatistics: displays some statistics about the calculation       */
/* ************************************************************************ */
static
void
displayStatistics (struct calculation_arguments const* arguments, struct calculation_results const* results, struct options const* options)
{
    int N = arguments->N;
    double time = (comp_time.tv_sec - start_time.tv_sec) + (comp_time.tv_usec - start_time.tv_usec) * 1e-6;

    printf("Berechnungszeit:    %f s \n", time);
    printf("Speicherbedarf:     %f MiB\n", (N + 1) * (N + 1) * sizeof(double) * arguments->num_matrices / 1024.0 / 1024.0);

    printf("\n");
    printf("Interlines:         %" PRIu64 "\n",options->interlines);
    printf("Stoerfunktion:      ");

    if (options->inf_func == FUNC_F0)
    {
        printf("f(x,y) = 0");
    }
    else if (options->inf_func == FUNC_FPISIN)
    {
        printf("f(x,y) = 2pi^2*sin(pi*x)sin(pi*y)");
    }

    printf("\n");
    printf("Terminierung:       ");

    if (options->termination == TERM_PREC)
    {
        printf("Hinreichende Genaugkeit");
    }
    else if (options->termination == TERM_ITER)
    {
        printf("Anzahl der Iterationen");
    }

    printf("\n");
    printf("Anzahl Iterationen: %" PRIu64 "\n", results->stat_iteration);
    printf("Norm des Fehlers:   %e\n", results->stat_precision);
    printf("\n");
}


/**
 * rank and size are the MPI rank and size, respectively.
 * from and to denote the global(!) range of lines that this process is responsible for.
 *
 * Example with 9 matrix lines and 4 processes:
 * - rank 0 is responsible for 1-2, rank 1 for 3-4, rank 2 for 5-6 and rank 3 for 7.
 *   Lines 0 and 8 are not included because they are not calculated.
 * - Each process stores two halo lines in its matrix (except for ranks 0 and 3 that only store one).
 * - For instance: Rank 2 has four lines 0-3 but only calculates 1-2 because 0 and 3 are halo lines for other processes. It is responsible for (global) lines 5-6.
 */
static
void
DisplayMatrix (struct calculation_arguments* arguments, struct calculation_results* results, struct options* options, int rank, int size, int from, int to)
{
  int const elements = 8 * options->interlines + 9;

  int x, y;
  double** Matrix = arguments->Matrix[results->m];
  MPI_Status status;

  /* first line belongs to rank 0 */
  if (rank == 0)
    from--;

  /* last line belongs to rank size - 1 */
  if (rank + 1 == size)
    to++;

  if (rank == 0)
    printf("Matrix:\n");

  for (y = 0; y < 9; y++)
  {
    int line = y * (options->interlines + 1);

    if (rank == 0)
    {
      /* check whether this line belongs to rank 0 */
      if (line < from || line > to)
      {
        /* use the tag to receive the lines in the correct order
         * the line is stored in Matrix[0], because we do not need it anymore */
        MPI_Recv(Matrix[0], elements, MPI_DOUBLE, MPI_ANY_SOURCE, 42 + y, MPI_COMM_WORLD, &status);
      }
    }
    else
    {
      if (line >= from && line <= to)
      {
        /* if the line belongs to this process, send it to rank 0
         * (line - from + 1) is used to calculate the correct local address */
        MPI_Send(Matrix[line - from + 1], elements, MPI_DOUBLE, 0, 42 + y, MPI_COMM_WORLD);
      }
    }

    if (rank == 0)
    {
      for (x = 0; x < 9; x++)
      {

        int col = x * (options->interlines + 1);

        if (line >= from && line <= to)
        {
          /* this line belongs to rank 0 */
          printf("%7.4f", Matrix[line][col]);
        }
        else
        {
          /* this line belongs to another rank and was received above */
          printf("%7.4f", Matrix[0][col]);
        }
      }
      printf("\n");
    }
  }
  fflush(stdout);
}

/* ************************************************************************ */
/*  main                                                                    */
/* ************************************************************************ */
int
main (int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    struct options options;
    struct calculation_arguments arguments;
    struct calculation_results results;

    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* get parameters */
    AskParams(&options, argc, argv, rank);              /* ************************* */

    initVariables(&arguments, &results, &options);           /* ******************************************* */

    allocateMatrices(&arguments);        /*  get and initialize variables and matrices  */
    initMatrices(&arguments, &options);            /* ******************************************* */

    gettimeofday(&start_time, NULL);                   /*  start timer         */
    calculate(&arguments, &results, &options);         /*  solve the equation  */
    gettimeofday(&comp_time, NULL);                    /*  stop timer          */

    if(rank == 0) {
        displayStatistics(&arguments, &results, &options);
    }
    DisplayMatrix(&arguments, &results, &options, rank, num_procs, m_from, m_to);

    freeMatrices(&arguments);                                       /*  free memory     */
    MPI_Finalize();
    return EXIT_SUCCESS;
}
