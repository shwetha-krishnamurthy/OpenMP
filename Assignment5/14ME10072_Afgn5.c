#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

int main(int argc, char* argv[])
{
    int rank;
    int size;
    int arr[9999][9999];
    int sub_arr[3333][9999];
    int sub_arr1[3333][250];
    int a[250];

    int i;
    int k;
    int m;
    int flag=0;

    MPI_Status status;
    
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&p);

    srand(time(NULL));

    for(i=0; i<250; i++)
        a[i] = 0;

    if(rank == 0)
    {

        for(i=0; i<9999; i++)
            for(k=0; k<99999; k++)
            {
                arr[i][k] = (i+k)/80;
            }
        
        for(i=0; i<3333; i++)
            MPI_Send(&(arr[i][0]),9999,MPI_INT,1,flag,MPI_COMM_WORLD); 
        
        for(i=3333; i<6666; i++)
            MPI_Send(&(arr[i][0]),9999,MPI_INT,2,flag,MPI_COMM_WORLD); 
        
        for(i=6666; i<9999; i++)
            MPI_Send(&(arr[i][0]),9999,MPI_INT,3,flag,MPI_COMM_WORLD); 

        for (i=1; i<=3; i++)
            MPI_Recv(&(sub_arr1[i-1][0]), 250, MPI_INT, i, 0, MPI_COMM_WORLD, &status); 
    }


    if(rank != 0)
    {
        for(i=0; i<3333 ; i++)
        {
            MPI_Recv(&(sub_arr[i][0]),9999,MPI_INT,0,flag,MPI_COMM_WORLD,&status);

            for(k=0; k<9999 ; k++)
            {
                for(m=0 ; m<250 ; m++)
                {
                    if(sub_arr[i][k] == m)
                        a[m]++;
                }
            }
        } 
        MPI_Send(&(a[0]),250,MPI_INT,0,flag,MPI_COMM_WORLD);
    }

    printf("\nHistogram : ");
    if(rank == 0)
    {
        for (k = 0; k < 250; ++k)
        {
            printf("%d ", (sub_arr1[0][k] + sub_arr1[1][k] + sub_arr1[2][k]) );
        } 
    }

    MPI_Finalize();
}