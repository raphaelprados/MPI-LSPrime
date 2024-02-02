
    #include "mpi.h"
    #include <ctime>
    #include <cmath>
    #include <chrono>
    #include <cstdlib>
    #include <cstring>
    #include <iostream>

    bool testePrimalidade(int number) {
        static signed char historico[50000000];
        static bool primeira_execucao = true;
        int endereco = number / 2 + 1;

        if(primeira_execucao) {
            memset(&historico, 'f', sizeof(unsigned char));
            primeira_execucao = false;
        }

        if(((number & 1) == 0 && number != 2) || number <= 1)
            return false;

        if(historico[endereco] == 'p' || historico[endereco] == 'n')
            return (historico[endereco] == 'p');

        for(int i = 3; i < (int)sqrt(number) + 1; i += 2)
            if(number % i == 0) {
                historico[endereco] == 'n';
                return false;
            }

        historico[endereco] = 'p';

        return true;
    }

    int** createMatrix(int r, int c) {
        int **matrix;
        matrix = new int*[r];
        for(int i = 0; i < c; i++)
            matrix[i] = new int[c];
        return matrix;
    }

    void fillMatrix(int **mat, int r, int c) {
        srand(time(NULL));
        for(int i = 0; i < r; i++) {
            for(int j = 0; j < c; j++) {
                mat[i][j] = rand() % 10;
            }
        }
    }

    void deleteMatrix(int **matrix, int r, int c) {
        for(int i = 0; i < r; i++)
            free(matrix[i]);
        free(matrix);
    }

    int main(int argc, char const *argv[]) {

        const int ROWS = 10000;
        const int COLUMNS = ROWS;

        int size, rank, prime_count = 0, prime_total = 0;
        int ** mat, ** submat;

        auto start = std::chrono::system_clock::now();

        MPI_Init(NULL, NULL);
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        const int SUBROWS = ROWS / size;

        if(rank == 0) {
            mat = createMatrix(ROWS, COLUMNS);
            // fillMatrix(mat, ROWS, COLUMNS);
            std::cout << "Starting Prime Calculation for a " << ROWS << "x" << COLUMNS << ", with 4 process nodes." << std::endl;
        }

        std::cout << mat[100][100] << std::endl;

        submat = createMatrix(SUBROWS, COLUMNS);

        Espalha a matriz
        MPI_Scatter(&mat, SUBROWS * COLUMNS, MPI_INT, &submat, SUBROWS * COLUMNS, MPI_INT, 0, MPI_COMM_WORLD);

        std::cout << mat[100][100] << std::endl;

        for(int i = 0; i < SUBROWS; i++) {
            for(int j = 0; j < SUBCOLUMNS; j++) {
                prime_count += (testePrimalidade(submat[i][j]));
            }
        }

        MPI_Reduce(&prime_count, &prime_total, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

        auto end = std::chrono::system_clock::now();

        if(rank == 0)
            std::cout << "Total elapsed time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms\n" <<
                         "Primes found: " << prime_total << std::endl; 

        deleteMatrix(submat, SUBROWS, COLUMNS);
        if(rank == 0)
            deleteMatrix(mat, ROWS, COLUMNS);

        MPI_Finalize();   

        return 0;
    }