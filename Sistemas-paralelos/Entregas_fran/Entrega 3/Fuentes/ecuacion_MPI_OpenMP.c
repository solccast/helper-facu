#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>
#include <time.h>
#define COORDINATOR 0

static inline void mult_bloques(double *ablk, double *bblk, double *cblk, int tam_bloque, int N);

int main(int argc, char* argv[]){
	int i,j,k,cantProcesos,rank,N,stripSize,tam_bloque,check=1,nivel_provisto, cantThreads;
    double promedioA, promedioB, RP,maxA,minA,sumaA,maxB,minB,sumaB;
	double *A,*B,*C,*D,*CD,*AB,*R;
	MPI_Status status;
	double commTimes[6], maxCommTimes[6], minCommTimes[6], commTime, totalTime;

    MPI_Init_thread(&argc,&argv,MPI_THREAD_MULTIPLE,&nivel_provisto);

    if(argc != 4){
	    printf("Param1: tamanio matriz - Param2: tamanio bloque - Param3: cantidad threads \n");
		exit(1);
	}
    N=atoi(argv[1]);
	tam_bloque=atoi(argv[2]);
    cantThreads=atoi(argv[3]);
    omp_set_num_threads(cantThreads);
	MPI_Comm_size(MPI_COMM_WORLD,&cantProcesos);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);

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
    sumaA = 0; sumaB = 0;

    commTimes[0] = MPI_Wtime();            
    //Distribuyo los datos
    MPI_Scatter(A,stripSize*N,MPI_DOUBLE,A,stripSize*N,MPI_DOUBLE,COORDINATOR,MPI_COMM_WORLD);
    MPI_Scatter(C,stripSize*N,MPI_DOUBLE,C,stripSize*N,MPI_DOUBLE,COORDINATOR,MPI_COMM_WORLD);
    MPI_Bcast(B,N*N,MPI_DOUBLE,COORDINATOR,MPI_COMM_WORLD);
    MPI_Bcast(D,N*N,MPI_DOUBLE,COORDINATOR,MPI_COMM_WORLD);

    maxA=A[0];
    minA=A[0];
    maxB=B[0];
    minB=B[0];

    commTimes[1] = MPI_Wtime();

    int primera=rank*stripSize;
    int ultima=primera+stripSize;

    //Empieza region paralela 
    #pragma omp parallel shared(A,B,C,D,AB,CD,R) 
    {

        //1) Buscar max,min y suma de A y B
        #pragma omp for reduction(+:sumaA) reduction(min:minA) reduction(max:maxA)
        for(i=0;i<stripSize*N;i++){
            sumaA+=A[i];
            if(A[i]>maxA) maxA=A[i]; 
            else if(A[i]<minA) minA=A[i];
        }

        #pragma omp for reduction(+:sumaB) reduction(min:minB) reduction(max:maxB)
        for(i=primera;i<ultima;i++){
            for(j=0;j<N;j++){
                sumaB+=B[i*N+j];
                if(B[i*N+j]>maxB) maxB=B[i*N+j]; 
                else if(B[i*N+j]<minB) minB=B[i*N+j];
            }
        }

        #pragma omp single
        {
            mins[0]=minA; maxs[0]=maxA; sumas[0]=sumaA;
            mins[1]=minB; maxs[1]=maxB; sumas[1]=sumaB;

            commTimes[2] = MPI_Wtime();

            MPI_Allreduce(mins,minsR,2,MPI_DOUBLE,MPI_MIN,MPI_COMM_WORLD);
            MPI_Allreduce(maxs,maxsR,2,MPI_DOUBLE,MPI_MAX,MPI_COMM_WORLD);
            MPI_Allreduce(sumas,sumasR,2,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);

            commTimes[3] = MPI_Wtime();

            if(rank==COORDINATOR){
                printf("minA=%f, maxA=%f, sumaA=%f \n",minsR[0],maxsR[0],sumasR[0]);
                printf("minB=%f, maxB=%f, sumaB=%f \n",minsR[1],maxsR[1],sumasR[1]);
            }

            promedioA=sumasR[0]/(N*N);
            promedioB=sumasR[1]/(N*N);
            RP = ((maxsR[0] * maxsR[1] - minsR[0] * minsR[1]) / (promedioA * promedioB));
        }

        //3) AB = A x B
        #pragma omp for nowait
        for(i=0;i<stripSize;i+=tam_bloque){
            int valori=i*N;
            for(j=0;j<N;j+=tam_bloque){
                int valorj=j*N;
                for(k=0;k<N;k+=tam_bloque){
                    mult_bloques(&A[valori+k], &B[valorj+k], &AB[valori+j],tam_bloque,N);
                }
            }  
        }

        //4) CD = C x D
        #pragma omp for nowait
        for(i=0;i<stripSize;i+=tam_bloque){
            int valori=i*N;
            for(j=0;j<N;j+=tam_bloque){
                int valorj=j*N;
                for(k=0;k<N;k+=tam_bloque){
                    mult_bloques(&C[valori+k], &D[valorj+k], &CD[valori+j],tam_bloque,N);
                }
            }  
        }

        //5) R = AB + CD
        #pragma omp for nowait
        for (i=0;i<stripSize*N;i++) {
            R[i] = AB[i]*RP + CD[i];
        }
        
    }
        
    commTimes[4] = MPI_Wtime();

    MPI_Gather(R,stripSize*N,MPI_DOUBLE,R,stripSize*N,MPI_DOUBLE,COORDINATOR,MPI_COMM_WORLD);

    commTimes[5] = MPI_Wtime();
    
    MPI_Reduce(commTimes, minCommTimes, 6, MPI_DOUBLE, MPI_MIN, COORDINATOR, MPI_COMM_WORLD);
    MPI_Reduce(commTimes, maxCommTimes, 6, MPI_DOUBLE, MPI_MAX, COORDINATOR, MPI_COMM_WORLD);

    if(rank==COORDINATOR){
        printf("imprimo R \n");
        for(i=0;i<8;i++){
            for(j=0;j<8;j++){
            printf(" [%i][%i]= %0.0f ",i,j,R[i*N+j]);
        }
            printf("\n");
        }
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
