#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void bubble_sort_local(int *arr, int n)
{
    int temp;
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

int main(int argc, char **argv)
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double start_time = 0.0;
    double end_time = 0.0;

    long N = 400000; // tamanho do vetor (pode ajustar)
    int *data = NULL;
    int local_n = N / size;
    int *local_data = (int *)malloc(local_n * sizeof(int));

    // Processo 0 inicializa os dados
    if (rank == 0)
    {
        data = (int *)malloc(N * sizeof(int));
        srand(42);
        for (int i = 0; i < N; i++)
            data[i] = rand() % 100000;

        // Marca o início do tempo
    }

    start_time = MPI_Wtime();
    // Envia blocos para cada processo
    MPI_Scatter(data, local_n, MPI_INT,
                local_data, local_n, MPI_INT,
                0, MPI_COMM_WORLD);

    // Ordena localmente
    bubble_sort_local(local_data, local_n);

    for (int phase = 0; phase < size; phase++)
    {
        int parceiro;

        if (phase % 2 == 0)
        {
            if (rank % 2 == 0)
                parceiro = rank + 1;
            else
                parceiro = rank - 1;
        }
        else
        {
            if (rank % 2 == 0)
                parceiro = rank - 1;
            else
                parceiro = rank + 1;
        }

        
        if (parceiro >= 0 && parceiro < size)
        {

            int *recv_data = (int *)malloc(local_n * sizeof(int));

            MPI_Sendrecv(
                local_data, local_n, MPI_INT,
                parceiro, 0,
                recv_data, local_n, MPI_INT,
                parceiro, 0,
                MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // mescla dos menores ou maiores
            if (rank < parceiro)
            {
                int i = 0, j = 0, k = 0;
                int *temp = (int *)malloc(2 * local_n * sizeof(int));
                while (k < 2 * local_n)
                {
                    if (i < local_n && (j >= local_n || local_data[i] < recv_data[j]))
                        temp[k++] = local_data[i++];
                    else
                        temp[k++] = recv_data[j++];
                }
                for (int c = 0; c < local_n; c++)
                    local_data[c] = temp[c];
                free(temp);
            }
            else
            {
                int i = local_n - 1, j = local_n - 1, k = 2 * local_n - 1;
                int *temp = (int *)malloc(2 * local_n * sizeof(int));
                while (k >= 0)
                {
                    if (i >= 0 && (j < 0 || local_data[i] > recv_data[j]))
                        temp[k--] = local_data[i--];
                    else
                        temp[k--] = recv_data[j--];
                }
                for (int c = 0; c < local_n; c++)
                    local_data[c] = temp[c + local_n];
                free(temp);
            }

            free(recv_data);
        }
    }

    // Coleta dados no processo 0
    MPI_Gather(local_data, local_n, MPI_INT,
               data, local_n, MPI_INT,
               0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        end_time = MPI_Wtime();

        printf("Ordenação concluída com %d processo(s).\n", size);
        printf("Tempo total: %f segundos\n", end_time - start_time);

        /*
        for (int i = 0; i < N; i++){
            printf("%d ", data[i]);
        }
        printf("\n");
        */
        free(data);
    }

    free(local_data);
    MPI_Finalize();
    return 0;
}
