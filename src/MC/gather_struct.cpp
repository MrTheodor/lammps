#include <mpi.h>
#include <vector>
#include <iostream>

struct data {
  int a, b;
  double c;
};

void DeclareMPIType(MPI_Datatype *newType) {
  struct data tmp[2];
  MPI_Aint extent = &tmp[1] - &tmp[0];
  MPI_Type_create_resized(MPI_2INT, 0, extent, newType);
  MPI_Type_commit(newType);
}


int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  int size, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  std::cout << rank << "/" << size << std::endl;

  int num = 1+rank*2;
  data *d = new data[num];
  for (int a = 0; a < num; a++) {
    d[a].a = 100+a*2+num;
    d[a].b = 100+a*2+num;
    d[a].c = 99+a*3+num;
    std::cout << rank << ":" << "a=" << d[a].a << " b=" << d[a].b << " c=" << d[a].c << std::endl;
  }

  MPI_Datatype Data_type;
  DeclareMPIType(&Data_type);

  int revcount[size];
  int displ[size];
  if (rank == 0) {
    revcount[0] = 1;
    displ[0] = 0;
    for (int i = 1; i < size; i++) {
      revcount[i] = i+1;
      displ[i] = revcount[i-1] + displ[i-1];
    }
  }

  for (int i = 0; i < size; i++) {
    revcount[i] = size;
  }

  data *recv = NULL;

  MPI_Gatherv(d, num, MPI_BYTE, recv, revcount, displ, MPI_BYTE, 0, MPI_COMM_WORLD);

  MPI_Type_free(&Data_type);

  MPI_Finalize();
}
