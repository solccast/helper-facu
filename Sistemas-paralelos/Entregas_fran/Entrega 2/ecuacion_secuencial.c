#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>

//Para calcular tiempo
double dwalltime();
void mult_matrices(double *a, double *b, double *c, int n, int bs); //multiplicar matriz
void mult_bloques(double *ablk, double *bblk, double *cblk, int n, int bs); //multiplicar bloque

int main(int argc, char *argv[]){
    double *A,*B,*C,*D,*R; 
    double *CD,*AB;
    double timeTotal,tick,maxA,minA,maxB,minB;
    int i,j,k,l,N,tam_bloque,check=1;

    // para los promedios
    double sumaA = 0; 
    double sumaB = 0;
    double promedioA = 0; 
    double promedioB = 0;
    double cantElementos;
    

    //Verificar parametro 
    if ((argc != 3)){
        printf("Param1:512|1024|2048|4096 - Param2:tambloque \n");
        exit(1);
    }
    N = atoi(argv[1]);
    tam_bloque = atoi(argv[2]);
    cantElementos = N*N;


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
            A[i*N+j]=1; //Ordenada por Filas
            B[j*N+i]=1; //Ordenada por Columnas
            C[i*N+j]=1; //Ordenada Por Filas
            D[j*N+i]=1; //Ordenada por Columnas 1..40
        }
    }

    
    //------------EMPIEZA A CONTAR EL TIEMPO-----------------------
    tick = dwalltime();

    //1) calcula max de A y B , min de A y B y promedio de A y B
    maxA=A[0];
    minA=A[0];
    maxB=B[0];
    minB=B[0];
    sumaA=0;
    sumaB=0;
    tick = dwalltime();
    for(int i = 0; i < N*N; i++){
        if(A[i] > maxA) 
            maxA = A[i];
        if(A[i] < minA) 
            minA = A[i];
        sumaA += A[i];
    }
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            int pos=j*N+i;
            if(B[pos] > maxB) maxB=B[pos];
            else if(B[pos] < minB) minB=B[pos];
            sumaB+=B[pos];
        }
    }
    promedioA=sumaA/cantElementos;
    promedioB=sumaB/cantElementos;

    //2) A x B = AB(ordenado x filas)
    mult_matrices(A,B,AB,N,tam_bloque);

    //4) C x D2 = CD(ordenado x filas)
    mult_matrices(C,D,CD,N,tam_bloque);
    
    //5) Calculo de la primera parte de la ecuacion
    double RP = ((maxA * maxB - minA * minB) / (promedioA * promedioB)); //RP es un solo numero

    //6) AB = AB x RP 
    for (int i = 0; i < N*N; i++) {
        AB[i] = AB[i]*RP;
    }

    //7) AB + CD = R
    for (int i = 0; i < N*N; i++) {
        R[i] = AB[i] + CD[i];
    }

    timeTotal = dwalltime() - tick;

    //printf("resultados parciales maxA:%0.0f minA:%0.0f promA:%0.0f maxB:%0.0f minB:%0.1f promB:%0.1f \n",maxA,minA,promedioA,maxB,minB,promedioB);
    printf("matriz: %dx%d\n",N,N);
    printf("tam_bloque: %d\n",tam_bloque);
    printf("Tiempo requerido total de la ecuacion: %f\n",timeTotal);

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
    free(R);
    free(D);
    free(AB);
    free(CD);
    return 0;
}

void mult_matrices(double *A, double *B, double *C, int N, int bs){
    int i, j, k;
    for(i = 0; i < N; i += bs){
        int valori=i*N;
        for(j = 0; j < N; j += bs){
            int valorj=j*N;
            for(k = 0; k < N; k += bs){
                mult_bloques(&A[valori+k], &B[valorj+k], &C[valori+j],N,bs);
            }
        }
    }
}

void mult_bloques(double *ablk, double *bblk, double *cblk, int N, int bs){
    int i, j, k; 
    for(i = 0; i < bs; i++){
        int valori=i*N;
        for(j = 0; j < bs; j++){
            double temp=0;
            int valorj=j*N;
            for(k = 0; k < bs; k++){
                temp += ablk[valori+k] * bblk[valorj+k];
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