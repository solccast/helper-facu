#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <sys/time.h>

double dwalltime();
static inline void mult_bloques(double *ablk, double *bblk, double *cblk);

//variables compartidas
double maxA,minA,maxB,minB,sumaA,sumaB,promedioA,promedioB,RP;
int numThreads,tam_bloque,N;


int main(int argc, char *argv[]){
    double time,tick;
    int i,j,k,cantElementos,check=1;
    double *A,*B,*C,*D,*CD,*AB,*R;

    //Verificar parametro 
    if ((argc != 4)){
        printf("Param1: N - Param2: tamanio bloque - Param3: cantidad threads \n");
        exit(1);
    }
    N=atoi(argv[1]);
    tam_bloque = atoi(argv[2]);
    numThreads = atoi(argv[3]);
    omp_set_num_threads(numThreads);
    cantElementos=N*N;

    //Aloca memoria para las matrices
    A=(double*) malloc(N*N*sizeof(double));
    B=(double*) malloc(N*N*sizeof(double));
    C=(double*) malloc(N*N*sizeof(double));
    D=(double*) malloc(N*N*sizeof(double));
    R=(double*) malloc(N*N*sizeof(double));
    CD=(double*) malloc(N*N*sizeof(double));
    AB=(double*) malloc(N*N*sizeof(double));


    //Inicializar matrices
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            A[i*N+j]=1.0; //Ordenada por Filas
            B[j*N+i]=1.0; //Ordenada por Columnas
            C[i*N+j]=1.0; //Ordenada Por Filas
            D[j*N+i]=1.0; //Ordenada por Columnas;
        }
    }

    // para los promedios
    sumaA = 0; 
    sumaB = 0;
    maxA=A[0];
    minA=A[0];
    maxB=B[0];
    minB=B[0];


    //-------------------EMPIEZA A CONTAR EL TIEMPO----------------------------
    tick = dwalltime();

    #pragma omp parallel shared(A,B,C,D,AB,CD,R) 
    {
        //1) Buscar max,min y suma de A y B
        #pragma omp for nowait reduction(+:sumaA) reduction(min:minA) reduction(max:maxA)
        for(i=0;i<cantElementos;i++){
            sumaA+=A[i];
            if(A[i]>maxA) maxA=A[i]; 
            else if(A[i]<minA) minA=A[i];
        }

        #pragma omp for reduction(+:sumaB) reduction(min:minB) reduction(max:maxB)
        for(i=0;i<cantElementos;i++){
            sumaB+=B[i];
            if(B[i]>maxB) maxB=B[i]; 
            else if(B[i]<minB) minB=B[i];
        }

        //2) calcular RP
        #pragma omp single
        {
            promedioA=sumaA/cantElementos;
            promedioB=sumaB/cantElementos;
            RP = ((maxA * maxB - minA * minB) / (promedioA * promedioB)); //RP es un solo numero
        }


        //3) AB = A x B
        #pragma omp for nowait
        for(i=0;i<N;i+=tam_bloque){
            int valori=i*N;
            for(j=0;j<N;j+=tam_bloque){
                int valorj=j*N;
                for(k=0;k<N;k+=tam_bloque){
                    mult_bloques(&A[valori+k], &B[valorj+k], &AB[valori+j]);
                }
            }  
        }

        //5) CD = C x D
        #pragma omp for nowait
        for(i=0;i<N;i+=tam_bloque){
            int valori=i*N;
            for(j=0;j<N;j+=tam_bloque){
                int valorj=j*N;
                for(k=0;k<N;k+=tam_bloque){
                    mult_bloques(&C[valori+k], &D[valorj+k], &CD[valori+j]);
                }
            }  
        }


        //6) AB = AB * RP
        #pragma omp for nowait
        for (i=0;i<N*N;i++) {
            AB[i] = AB[i]*RP;
        }

        //7) R = AB + CD
        #pragma omp for nowait
        for (i=0;i<N*N;i++) {
            R[i] = AB[i] + CD[i];
        }
    
    }
    

    time = dwalltime() - tick;
    //-----------------------TERMINA CONTAR TIEMPO--------------------------
    printf("Algoritmo con OpenMP\n");
    printf("El tiempo total de la ecuacion con N:%i y %i threads es: %f \n",N,numThreads,time);
    //Corrobar resultado
    for(i=0;i<N*N;i++){
	    check=check&&(R[i]==N);
    }   

    if(check){
        printf("Calculo de la ecuacion correcto \n");
    }else{
        printf("Calculo de la ecuacion erroneo \n");
    }
    
    free(A);
    free(B);
    free(C);
    free(D);
    free(R);
    free(CD);
    free(AB);
}

//Multiplicacion de cada bloque
static inline void mult_bloques(double *ablk, double *bblk, double *cblk){
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

double dwalltime(){
	double sec;
	struct timeval tv;
	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}
