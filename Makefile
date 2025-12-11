# Nome do executável
TARGET = sort_mpi

# Compilador MPI
MPICC = mpicc

# Flags de compilação
CFLAGS = -O2

# Arquivos fonte
SRC = bubblesort_mpi.c

# Regra padrão
all: $(TARGET)

# Compilação
$(TARGET): $(SRC)
	$(MPICC) $(CFLAGS) -o $(TARGET) $(SRC)

# Execuções com diferentes números de processos
run1:
	mpirun -np 1 ./$(TARGET)

run2:
	mpirun -np 2 ./$(TARGET)

run4:
	mpirun -np 4 ./$(TARGET)

run6:
	mpirun -np 6 ./$(TARGET)

run8:
	mpirun -np 8 ./$(TARGET)

run16:
	mpirun -np 16 ./$(TARGET)

# Rodar tudo em sequência
run_all: run1 run2 run4 run6 run8 run16

# Limpeza
clean:
	rm -f $(TARGET)
