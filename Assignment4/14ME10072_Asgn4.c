#include <stdio.h>
#include <mpi.h>

void transpose (float *arr, int size)
{
  int i, j;
  int ij, ji, k;
  float temp;
  ij = 0;
  k = -1;
  for (i = 0; i < size; i++)
  	{
    	k += size + 1;
    	ji = k;
    	ij += i + 1;
    	for (j = i+1; j < size; j++)
		{
	  		temp = arr[ij];
	  		arr[ij] = arr[ji];
	  		arr[ji] = temp;
	  		ij++;
	  		ji += size;
		}
    }
}

int main (int argc, char *argv[])
{
  const int p = 10000;
  const int q = 2500;
  float a[p][q];
  float b[q][p];

  int i, j, nprocs, rank;

  MPI_Init (&argc, &argv);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);

  if(rank == 0)
  {
  	printf("Transposing a 10000x10000 matrix, divided among 4 processors\n");
  }
  
  if (nprocs != 4)
  {
    if (rank == 0)
		printf ("Error, number of processes must be 4\n");
    MPI_Finalize ();
    return 1;
  }

  for (i = 0; i < r; i++)
    for (j = 0; j < c; j++)
      a[i][j] = (float)(i*(j + rank*q))/(float)p;   

  MPI_Alltoall (&a[0][0], q * q, MPI_DOUBLE, &b[0][0], q * q, MPI_DOUBLE, MPI_COMM_WORLD);

  for (i = 0; i < 4; i++)
      trans (&b[i * q][0], q);

  for (i = 0; i < p; i++)
    for (j = 0; j < q; j++)
    {
		if (b[i][j] != (double)(i*(j + rank*c))/(double)r  )
		  {
		    printf ("process %d found b[%d][%d] = %f, but %f was expected\n", rank, i, j, b[i][j], (float)(i*(j + rank*c))/(float)r );
	    	printf("\nError in transpose!");
		    MPI_Abort (MPI_COMM_WORLD,1);
		    return 1;
		  }
	}

  if (rank == 0)
    printf ("Correct transpose obtained!\n");

  MPI_Finalize ();
  return 0;
}