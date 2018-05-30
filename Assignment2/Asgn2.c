#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define n 2000

int main()
{
	FILE* img=fopen("Julia_IIM_6_circle.pgm","rb");
	FILE* fpc=fopen("Histogram.txt", "wb");

	int i, j;
 
    int *im[n];
    for (i=0; i<n; i++)
         im[i] = (int *)malloc(n * sizeof(int));
 
    int Xsize,Ysize,maxg;

	fscanf(img,"%d",&Xsize);
	fscanf(img,"%d",&Ysize);
	fscanf(img,"%d",&maxg);
	printf("%d %d %d\n",Xsize,Ysize,maxg);

    for (i = 0; i < n; i++)
      for (j = 0; j < n; j++)
         im[i][j] = fgetc(img); 

    int* histogram;
    histogram = (int *)malloc(256 * sizeof(int));
   // int histogram[256];

    for(i=0; i<256; i++) histogram[i] = 0;

	#pragma omp parallel
	{
	    int* histogram_private;
	    histogram_private = (int *)malloc(256 * sizeof(int));
	    for(i=0; i<256; i++) histogram_private[i] = 0;
	    printf("Tee\n");
	    #pragma omp for nowait
	    for(i=0; i<n; i++) {
	       for(j=0; j<n; j++) {    
	           histogram_private[im[i][j]]++;
	       }
	    }      
	    #pragma omp critical 
	    {
	        for(i=0; i<256; i++)
	        {
	        	histogram[i] += histogram_private[i];
	        	fprintf(fpc, "%d , %d\n", i, histogram[i]);
	        }
	    }
	}

   
   // for (i = 0; i < n; i++)
   // {
   //    for (j = 0; j < n; j++)
   //       printf("%d ", im[i][j]);
   //   printf("\n");
   // }
 
    /* Code for further processing and free the 
      dynamically allocated memory */
 
   return 0;
}