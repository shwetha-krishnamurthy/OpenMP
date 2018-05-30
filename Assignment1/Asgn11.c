#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define n 10

int main()
{
	int A[n][n], B[n][n], C[n][n];
	int chunk = 10;
	int i, j, k, sum;	

	for (i = 0; i < n; i++)
	{
		for(j = 0; j < n; j++)
		{
			C[i][j] = 0;
		}
	}

	FILE *fpa, *fpb, *fpc;
	fpa = fopen("A.txt", "r");
	fpb = fopen("B.txt", "r");
	fpc = fopen("Output11.txt", "w");

	for (i = 0; i<n; i++){
		for (j = 0; j<n; j++)
		{
			fscanf(fpa, "%d", &A[i][j]);
			fscanf(fpb, "%d", &B[i][j]);
		}
	}

	double t1_r = omp_get_wtime();

	#pragma omp parallel for schedule(static) 
		for (i = 0; i < n; i++)
		{
			#pragma omp parallel for private(sum) 
				for (j = 0; j < n; j++)
			{
				sum = 0;
				#pragma omp parallel for reduction(+ : sum) 
					for (k = 0; k < n; k++)
					{
							sum += A[i][k]*B[k][j];
					}
				C[i][j] = sum;
			}
		}

	double t2_r = omp_get_wtime();
	double elapsed_r = t2_r - t1_r;

	fprintf(fpc, "Using reduction\n");

	for (i = 0; i<n; i++)
	{
			for (j = 0; j<n; j++)
				{
			fprintf(fpc, "%d ", C[i][j]);
		}
		fprintf(fpc, "\n");
	}


	for (i = 0; i < n; i++)
	{
		for(j = 0; j < n; j++)
		{
			C[i][j] = 0;
		}
	}

	double t1_c = omp_get_wtime();
	#pragma omp parallel for schedule(static) //collapse(2)  
		for (i = 0; i < n; i++)
		{
			#pragma omp parallel for 
				for (j = 0; j < n; j++)
			{
				#pragma omp parallel for 
					for (k = 0; k < n; k++)
					{
						#pragma omp critical
							C[i][j] += A[i][k]*B[k][j];
					}
			}
		}
	double t2_c = omp_get_wtime();
	double elapsed_c = t2_r - t1_r;

	fprintf(fpc, "Using critical\n");

	for (i = 0; i<n; i++)
	{
			for (j = 0; j<n; j++)
				{
			fprintf(fpc, "%d ", C[i][j]);
		}
		fprintf(fpc, "\n");
	}

	fprintf(fpc, "Execution time using critical: %f \nExecution time using reduction: %f \n", elapsed_c, elapsed_r);
}
