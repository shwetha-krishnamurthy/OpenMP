#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	int n = 10;
	int *a, *b, *ne, *x;
	int i, j, k;

	float sum = 0.0;

	a = (int*)malloc(n*n*sizeof(int));
	b = (int*)malloc(n*sizeof(int));
	ne = (int*)malloc(n*sizeof(int));
	x = (int*)malloc(n*sizeof(int));

	printf("Matrix a:\n");
	#pragma omp parallel for private(j)
		for (i = 0; i<n; i++)
		{
			b[i] = (int)RAND_MAX/(int)rand();
			#pragma omp parallel for
				for (j = 0; j < n; ++j)
				{
					a[i*n + j] = (int)RAND_MAX/(int)rand();
				}
		}

	for (i = 0; i<n; i++)
	{
		for (j = 0; j < n; ++j)
		{
			printf("%d\t", a[i*n+j]);
		}
		printf("\n");
	}

	printf("Vector b:\n");
	for(i =0; i<n; i++)
		printf("%d\n", b[i]);
	#pragma omp parallel for
		for(i = 0; i<n; i++)
			x[i] = 0;

	#pragma omp parallel for private(sum, k)
		for (j = 0; j < n; ++j)
		{
			sum = 0; 
			#pragma omp parallel for reduction(+:sum)
				for (k = 0; k < n; ++k)
				{
					sum = sum + a[j*n + k]*x[k];
				}
			ne[j] = (1/a[j*n + j])*(b[j] - sum);
		}

	#pragma omp parallel for
		for( j = 0; j<n; j++)
		{
			x[j] = ne[j];
			printf("%d\n", x[i]);
		}
		
}