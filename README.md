# Parallel Array Sorting Checker in C using MPI

A robust parallel computing application implemented in **C** using **MPI (Message Passing Interface)**. The program checks whether a sequence of integers of length $N$ provided by the user is sorted in non-decreasing order ($T_i \le T_{i+1}$) across $p$ parallel processors using **Point-to-Point communication**, featuring a dynamic interactive terminal menu.

## Problem Description & Requirements

1. **Input Generation & Distribution:** Process `0` reads the sequence length $N$ and the sequence of integers from the user, then distributes the data evenly among all $p$ processors without redundancy.
2. **Parallel Processing & Load Balancing:** The computational workload (comparisons) and input data are partitioned equally across all available processors. Support is included for arbitrary combinations of $N$ and $p$ (handling non-multiples gracefully).
3. **Validation & Error Detection:** Processors check their local chunks and handle boundary communications. If the sequence is not sorted, the program identifies and reports the exact index $i$ where the sorting condition first fails ($T_i > T_{i+1}$).
4. **Interactive Loop:** Features a repetitive menu interface (`1. Continue`, `2. Exit`) allowing the user to run multiple checks in a single session.

---

## Parallel Architecture & Communication Strategy

- **Data Partitioning:** The array of size $N$ is divided into chunks of size $local\_N = \lfloor N / p \rfloor$, with remaining elements distributed one-by-one to the first few processors.
- **Boundary Overlap:** To check sorting continuity across processor boundaries, adjacent processors exchange boundary elements using MPI Point-to-Point routines (`MPI_Send` / `MPI_Recv`).
- **Global Reduction:** Local validation flags and minimum failing indices are aggregated using MPI reduction operations to determine the global state.

---

## Tech Stack

- **Language:** C (C99)
- **Parallel Paradigm:** **MPI** (OpenMPI / MPICH)
- **Build System:** Makefile / GCC

---

## Prerequisites & Dependencies

To compile and run this program, ensure you have an MPI implementation installed on your system:

```bash
# Ubuntu / Debian
sudo apt-get install openmpi-bin libopenmpi-dev

# Fedora / RHEL
sudo dnf install openmpi openmpi-devel
Compilation & Execution1. Compile the ProgramYou can compile using mpicc:Bashmpicc -o mpi_sort_checker main.c
(or via a Makefile if configured).2. Run the Parallel ApplicationExecute the program using mpirun or mpiexec specifying the number of processors $p$:Bashmpirun -np 4 ./mpi_sort_checker
Interactive Menu OptionsWhen you run the program, a terminal menu will appear:Continue: Prompts for a new sequence length $N$, the sequence elements, and evaluates whether it is sorted.Exit: Terminates the MPI execution gracefully.Project Structuremain.c — Core implementation containing user interaction menus, data distribution logic, local sorting validation, boundary point-to-point communication, and error reporting.Makefile — Optional build automation script.
Author
Christopoulos Nikolas