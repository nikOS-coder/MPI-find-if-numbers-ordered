#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

void check_order(int, int, MPI_Status);

int main(int argc, char **argv) {
    int my_rank, p, size, group_size, spare, choice;
    int offset = 0;
    int *data, *group_data;
    int flag_tag = 0;
    MPI_Status status;

    int rc = MPI_Init(&argc, &argv);
    if(rc != 0) {
        fprintf(stderr, "Problem in initialization of mpi!\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    do {
        if(my_rank == 0) {
            fprintf(stdout, "Welcome to menu start.\n");
            fprintf(stdout, "[1]... Give numbers.\n");
            fprintf(stdout, "[2]... exit the menu\n");

            scanf("%d", &choice);
            for(int i = 1; i < p; i++) {
                MPI_Send(&choice, 1, MPI_INT, i, flag_tag, MPI_COMM_WORLD);
            } 
            switch(choice) {
                case 1: check_order(my_rank, p, status); break;
                case 2: break;
                default: fprintf(stdout, "Please give a valid option, try again.\n");
            }
            fprintf(stdout, "Exiting menu...\n");
        } 
        else {
            MPI_Recv(&choice, 1, MPI_INT, 0, flag_tag, MPI_COMM_WORLD, &status);
            switch(choice) {
                case 1: check_order(my_rank, p, status); break;
                case 2: break;
                default: break;
            }
        }
    }while(choice != 2);

    MPI_Finalize();
    return 0;
}

void check_order(int my_rank, int p, MPI_Status status) {
    int size, group_size, spare;
    int offset = 0;
    int *data, *group_data;
    int flag_tag = 0;

    if(my_rank == 0) {
        fprintf(stdout, "Give size of your number array\n");
        scanf("%d", &size);
        data = (int*)malloc(size * sizeof(int));
        fprintf(stdout, "Give your number string entering a number at a time\n");
        for(int i = 0; i < size; i++) {
            fprintf(stdout, "number %d -> ", i + 1);
            fflush(stdout);
            scanf("%d", data + i);
        }
        spare = size % p;
        if(spare > my_rank) {
            group_size = size / p + 1;
        }
        else {
            group_size = size / p;
        }
        group_data = (int*)malloc(group_size * sizeof(int));
        for(int i = 0; i < group_size; i++) {
            group_data[i] = data[i];
        }
        offset += group_size;
        for(int i = 1; i < p; i++) {
            if(spare > i) {
                group_size = size / p + 1;
            }
            else {
                group_size = size / p;
            }

            MPI_Send(&offset, 1, MPI_INT, i, flag_tag, MPI_COMM_WORLD);
            MPI_Send(&spare, 1, MPI_INT, i, flag_tag, MPI_COMM_WORLD);
            MPI_Send(&group_size, 1, MPI_INT, i, flag_tag, MPI_COMM_WORLD);
            MPI_Send(&data[offset], group_size, MPI_INT, i, flag_tag, MPI_COMM_WORLD);

            offset += group_size;
        }
    }else {
        MPI_Recv(&offset, 1, MPI_INT, 0, flag_tag, MPI_COMM_WORLD, &status);
        MPI_Recv(&spare, 1, MPI_INT, 0, flag_tag, MPI_COMM_WORLD, &status);
        MPI_Recv(&group_size, 1, MPI_INT, 0, flag_tag, MPI_COMM_WORLD, &status);
 
        group_data = (int*)malloc(group_size * sizeof(int));
        MPI_Recv(&group_data[0], group_size, MPI_INT, 0, flag_tag, MPI_COMM_WORLD, &status);
    }

    int err_loc = -1;
    for(int j = 0; j < group_size - 1; j++) {
        if(group_data[j] > group_data[j + 1]) {
            err_loc = offset + j + 1; 
            break;
        }
    }
    if(my_rank > 0) {
        MPI_Send(&group_data[0], 1, MPI_INT, my_rank - 1, flag_tag, MPI_COMM_WORLD);
    }
    if(my_rank < p - 1) {
        int next_first; 
        MPI_Recv(&next_first, 1, MPI_INT, my_rank + 1, flag_tag, MPI_COMM_WORLD, &status);
        if(group_data[group_size - 1] > next_first) {
            err_loc = offset + group_size;
        }
    }
    if(my_rank > 0) {
        MPI_Send(&err_loc, 1, MPI_INT, 0, flag_tag, MPI_COMM_WORLD);
    }
    else {
        int first_err_loc = err_loc;

        for(int i = 1; i < p; i++) {
            int worker_err;
            MPI_Recv(&worker_err, 1, MPI_INT, i, flag_tag, MPI_COMM_WORLD, &status);
            if(worker_err != -1) {
                if(first_err_loc == -1 || worker_err < first_err_loc) {
                    first_err_loc = worker_err;
                }
            }
        }

        printf("\n----------------------------------------\n");
        if(first_err_loc == -1) {
            printf("RESULT: YES (The array is sorted in ascending order)\n");
        } else {
            printf("RESULT: NO (Sorting fails at element index %d)\n", first_err_loc);
        }
        printf("----------------------------------------\n");
    }

    if(my_rank == 0) {
        free(data);
        free(group_data);
    }
    else {
        free(group_data);
    }
}