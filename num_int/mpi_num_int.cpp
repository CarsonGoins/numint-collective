#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <mpi.h>

#ifdef __cplusplus
extern "C" {
#endif

    float f1(float x, int intensity);
    float f2(float x, int intensity);
    float f3(float x, int intensity);
    float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif

float (*var) (float,int);

int main (int argc, char* argv[])
{
    if (argc < 6) 
    {
        std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
        return -1;
    }

    int a = atoi(argv[2]);
    int n = atoi(argv[4]);
    float m = (atoi(argv[3]) - a) / (float)n;
    int intensity = atoi(argv[5]);

    int rank, size;
    double global = 0.0;

    switch(atoi(argv[1]))
    {
        case 1:
            var = &f1;
            break;
        case 2:
            var = &f2;
            break;
        case 3:
            var = &f3;
            break;
        case 4:
            var = &f4;
            break;
        default:
            std::cerr<<"nope";
            exit(-1);
    }
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

    MPI_Init (&argc, &argv);

    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int temp1 = n / size;
    int temp2 = rank * temp1;
    int last = (rank + 1) * temp1;
    if (rank == size - 1)
        arrEnd = n;

    double temp3 = 0.0;
    for (int x = temp2; x < last; ++x)
    {
        temp3 += (double)var(a + (x + 0.5) * m, intensity) * m;
    }
    if (rank != 0)
    {
        MPI_Send(&temp3, 1, MPI_DOUBLE_PRECISION, 0, 100+rank, MPI_COMM_WORLD);
    }
    else
    {
        global = temp3;
        for (int x = 1; x < size; ++x)
        {
            MPI_Status status;
            MPI_Recv(&temp3, 1, MPI_DOUBLE_PRECISION, x, 100+x, MPI_COMM_WORLD, &status);
            global += temp3;
        }
    }

    MPI_Finalize();
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;

    if(rank == 0)
    {
        std::cout<<global<<std::endl;
        std::cerr<<elapsed_seconds.count()<<std::endl;
    }

    return 0;
}