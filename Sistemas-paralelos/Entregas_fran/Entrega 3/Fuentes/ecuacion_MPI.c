#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#define COORDINATOR 0

static inline void encontrar_valoresA(double *A, int N, int stripSize ,double *mins,double *maxs,double *sumas);
static inline void encontrar_valoresB(double *B, int N, int rank, int cantProcesos,double *mins,double *maxs,double *sumas);
static inline void mult_matrices(double *A, double *B, double *C, int stripSize, int tam_bloque, int N);
static inline void mult_bloques(double *ablk, double *bblk, double *cblk, int tam_bloque, int N);
static inline void sumar_AB_CD(double *AB, double *CD, double *R, double RP, int stripSize, int N);

int main(int argc, char* argv[]){
	int i,j,k,cantProcesos,rank,N,stripSize,tam_bloque,check=1;
    double promedioA, promedioB,RP;
	double *A,*B,*C,*D,*CD,*AB,*R,*resultados;
	MPI_Status status;
	double commTimes[6], maxCommTimes[6], minCommTimes[6], commTime, totalTime;

    MPI_Init(&argc,&argv);

    if(argc != 3){
	    printf("Param1: tamanio matriz - Param2: tamanio bloque: \n");
		exit(1);
	}
    N=atoi(argv[1]);
	tam_bloque=atoi(argv[2]);
	MPI_Comm_size(MPI_COMM_WORLD,&cantProcesos);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    /*  PASOS
    1)  MinA = mínimo de A
        MaxA = máximo de A
        PromA = promedio de A
        MinB = mínimo de B
        MaxB = máximo de B
        PromB = promedio de B 
    2)  AB = A x B
    3)  CD = C x D2 
    4)  RP = (MaxA x MaxB - MinA x MinB) / (PromA x PromB) 
    5)  R = AB + CD
    
    Orden matrices:
    -A: por filas
    -B: por columnas
    -C: por filas
    -D: por columnas
    -AB: por filas
    -CD: por filas
    -R: por filas */


    //Calcular porcion de cada worker
	stripSize = N / cantProcesos;

    // Reservar memoria
	if (rank == COORDINATOR) {
		A = (double*) malloc(sizeof(double)*N*N);
		C = (double*) malloc(sizeof(double)*N*N);
        R = (double*) malloc(sizeof(double)*N*N);
	} else {
		A = (double*) malloc(sizeof(double)*N*stripSize);
		C = (double*) malloc(sizeof(double)*N*stripSize);
        R = (double*) malloc(sizeof(double)*N*stripSize);
	}
    B = (double*) malloc(sizeof(double)*N*N);
    D = (double*) malloc(sizeof(double)*N*N); 
    AB = (double*) malloc(sizeof(double)*N*stripSize); 
    CD = (double*) malloc(sizeof(double)*N*stripSize);


    //Inicializar datos
	if(rank == COORDINATOR){
	    for(i=0;i<N;i++){
            for (j=0;j<N;j++){
				A[i*N+j]=1;
                B[j*N+i]=1;
                C[i*N+j]=1;
                D[j*N+i]=1;
            }
        }
	}

    //Espero a que el coordinador inicialice
    MPI_Barrier(MPI_COMM_WORLD);

    //Pos 0 -> min, max y suma de A
    //Pos 1 -> min, max y suma de B
    double mins[2],maxs[2],sumas[2];
    double minsR[2],maxsR[2],sumasR[2];

    commTimes[0] = MPI_Wtime();

    //Distribuyo los datos
	MPI_Scatter(A,stripSize*N,MPI_DOUBLE,A,stripSize*N,MPI_DOUBLE,COORDINATOR,MPI_COMM_WORLD);
    MPI_Scatter(C,stripSize*N,MPI_DOUBLE,C,stripSize*N,MPI_DOUBLE,COORDINATOR,MPI_COMM_WORLD);
	MPI_Bcast(B,N*N,MPI_DOUBLE,COORDINATOR,MPI_COMM_WORLD);
    MPI_Bcast(D,N*N,MPI_DOUBLE,COORDINATOR,MPI_COMM_WORLD);

    commTimes[1] = MPI_Wtime();

    //Debo inicializar aca -> porque ya recibi las matrices    
    mins[0]=A[0]; maxs[0]=A[0]; sumas[0]=0;
    mins[1]=B[0]; maxs[1]=B[0]; sumas[1]=0;

    encontrar_valoresA(A,N,stripSize,mins,maxs,sumas);
    encontrar_valoresB(B,N,rank,cantProcesos,mins,maxs,sumas);

    commTimes[2] = MPI_Wtime();

    MPI_Allreduce(mins,minsR,2,MPI_DOUBLE,MPI_MIN,MPI_COMM_WORLD);
    MPI_Allreduce(maxs,maxsR,2,MPI_DOUBLE,MPI_MAX,MPI_COMM_WORLD);
    MPI_Allreduce(sumas,sumasR,2,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);

    commTimes[3] = MPI_Wtime();

    promedioA=sumasR[0]/(N*N);
    promedioB=sumasR[1]/(N*N);
    RP = ((maxsR[0] * maxsR[1] - minsR[0] * minsR[1]) / (promedioA * promedioB));

    mult_matrices(A,B,AB,stripSize,tam_bloque,N);
    mult_matrices(C,D,CD,stripSize,tam_bloque,N);
    sumar_AB_CD(AB,CD,R,RP,stripSize,N);

    commTimes[4] = MPI_Wtime();

    MPI_Gather(R,stripSize*N,MPI_DOUBLE,R,stripSize*N,MPI_DOUBLE,COORDINATOR,MPI_COMM_WORLD);

    commTimes[5] = MPI_Wtime();

    MPI_Reduce(commTimes, minCommTimes, 6, MPI_DOUBLE, MPI_MIN, COORDINATOR, MPI_COMM_WORLD);
	MPI_Reduce(commTimes, maxCommTimes, 6, MPI_DOUBLE, MPI_MAX, COORDINATOR, MPI_COMM_WORLD);

    if(rank==COORDINATOR) { 
		for(i=0;i<N;i++){
			for(j=0;j<N;j++){
				check=check&&(R[i*N+j]==N);
			}
		}
        if(check){
            printf("Multiplicacion de matrices resultado correcto\n");
        }else{
            printf("Multiplicacion de matrices resultado erroneo\n");
        }
        totalTime = (maxCommTimes[5] - minCommTimes[0]);
	    commTime = (maxCommTimes[1] - minCommTimes[0]) + (maxCommTimes[3] - minCommTimes[2]) + (maxCommTimes[5] - minCommTimes[4]);
        printf("Tiempo total=%f, Tiempo comunicacion=%f, Tiempo computo=%f\n",totalTime,commTime,totalTime-commTime);
    }

    if(rank == COORDINATOR){
        free(A);
        free(B);
        free(C);
        free(D);
        free(R);
    }else{
        free(A);
        free(B);
        free(C);
        free(D);
        free(R);
        free(AB);
        free(CD);
    }
    MPI_Finalize();
    return 0;
}

static inline void encontrar_valoresA(double *A, int N, int stripSize ,double *mins,double *maxs,double *sumas){
    int i;
    for(i=0;i<stripSize*N;i++){
        if(A[i] > maxs[0]) 
            maxs[0] = A[i];
        else if(A[i] < mins[0]) 
            mins[0] = A[i];
        sumas[0] += A[i];
    }
}

static inline void encontrar_valoresB(double *B, int N, int rank, int cantProcesos,double *mins,double *maxs,double *sumas){
    int i,j;
    int primera=rank*(N/cantProcesos);
    int ultima=primera+(N/cantProcesos)-1;

    for(i=primera;i<=ultima;i++){
        for(j=0;j<N;j++){
            int pos=j*N+i;
            if(B[pos] > maxs[1]) 
                maxs[1] = B[pos];
            else if(B[pos] < mins[1]) 
                mins[1] = B[pos];
            sumas[1] += B[pos];
        }
    }
}

static inline void mult_matrices(double *A, double *B, double *C, int stripSize,int tam_bloque, int N){
    int i,j,k;
	for (i=0;i<stripSize;i+=tam_bloque){
		int valori=i*N;
		for (j=0;j<N;j+=tam_bloque){
		    int valorj=j*N;
			for (k=0;k<N;k+=tam_bloque) { 
                mult_bloques(&A[valori+k], &B[valorj+k], &C[valori+j],tam_bloque,N);
			}
		}
	}
}

static inline void mult_bloques(double *ablk, double *bblk, double *cblk, int tam_bloque, int N){
    int i,j,k; 
    for(i=0;i<tam_bloque;i++){
        int valori=i*N;
        for(j=0;j<tam_bloque;j++){
            double temp=0;
            int valorj=j*N;
            for(k=0;k<tam_bloque;k++){
                temp+=ablk[valori+k]*bblk[valorj+k];
            }
            cblk[valori+j]+=temp;
        }
    }
}

static inline void sumar_AB_CD(double *AB, double *CD, double *R, double RP, int stripSize, int N){
	int i;
	for(i=0;i<stripSize*N;i++){
		R[i]=AB[i]*RP + CD[i];
	}
}