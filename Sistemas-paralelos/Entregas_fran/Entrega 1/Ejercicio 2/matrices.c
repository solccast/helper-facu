#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

//Para calcular tiempo
double dwalltime(){
    double sec;
    struct timeval tv;
    gettimeofday(&tv,NULL);
    sec = tv.tv_sec + tv.tv_usec/1000000.0;
    return sec;
}

int main(int argc, char *argv[]){
    double *A,*B,*C,*D,*CD,*AB,*R; 
    double timeTotal,tick,maxA,minA,maxB,minB;
    int i,j,offI,offJ,k,l,N,tam_bloque;
    int ii, jj, kk;
    int check=1;

    // para los promedios
    double promedioA = 0.0; 
    double promedioB = 0.0;
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
            A[i*N+j]=1.0; //Ordenada por Filas
            B[j*N+i]=1.0; //Ordenada por Columnas
            C[i*N+j]=1.0; //Ordenada Por Filas
            D[j*N+i]=1.0; //Ordenada por Columnas
        }
    }
    
    /*  PASOS

    1)  Calculamos:
            MinA = mínimo de A
            MaxA = máximo de A
            PromA = promedio de A
            MinB = mínimo de B
            MaxB = máximo de B
            PromB = promedio de B 
    2)  A x B = AB
    3)  C x D = CD
    4)  (MaxA x MaxB - MinA x MinB) / (PromA x PromB) = RP 
    5)  AB + CD = R

    */

    //------------Comienza conteo de tiempo-----------------------
    tick = dwalltime();
    //1) calcula max de A y B , min de A y B y promedio de A y B
    
    maxA = minA = A[0];
    for(i=0;i<N;i++){
    		offI = i*N;
        for(j=0;j<N;j++){
            int valor=A[i*N+j];
            //buscamos max de A
            if(valor > maxA)
            		maxA=valor;
            if(valor < minA)
            		minA=valor;
            promedioA += valor;
        }
    }
    promedioA=promedioA/cantElementos;
    
    maxB = minB = B[0];
    for(j=0;j<N;j++){
    		offJ = j*N;
        for(i=0;i<N;i++){
            int valor=B[offJ+i];
            //buscamos max de A
            if(valor > maxB)
            		maxB=valor;
            if(valor < minB)
            		minB=valor;
            promedioB += valor;
        }
    }    
    promedioB=promedioB/cantElementos;

    // 2) calculo de A x B = AB (ordenado x filas)
    for (i = 0; i < N; i += tam_bloque) {
        for (j = 0; j < N; j += tam_bloque) {
            for  (k = 0; k < N; k += tam_bloque) {
                for (ii = i; ii < i + tam_bloque; ii++) {
                    offI=ii*N;
                    for (jj = j; jj < j + tam_bloque; jj++){
                        offJ=jj*N;
                        double temp = 0.0;
                        for (kk = k; kk < k + tam_bloque; kk++) {
                            temp += A[offI+kk] * B[offJ+kk];
                        }
                        AB[offI+jj] += temp;
                    }
                }
            }
        }
    } 

    // 3) C x D = CD (ordenado x filas)
    for (i = 0; i < N; i += tam_bloque) {
        for (j = 0; j < N; j += tam_bloque) {
            for  (k = 0; k < N; k += tam_bloque) {
                for (ii = i; ii < i + tam_bloque; ii++) {
                    offI=ii*N;
                    for (jj = j; jj < j + tam_bloque; jj++){
                        offJ=jj*N;
                        double temp = 0.0;
                        for (kk = k; kk < k + tam_bloque; kk++) {
                            temp += C[offI+kk] * D[offJ+kk];
                        }
                        CD[offI+jj] += temp;
                    }
                }
            }
        }
    }
    
    // 4) calculo de la primera parte de la ecuacion
    double RP = ((maxA * maxB - minA * minB) / (promedioA * promedioB)); //RP es un solo numero

    // 5) AB * RP + CD = R
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            R[i*N+j] = AB[i*N+j] * RP + CD[j*N+i];
        }
    }

    timeTotal = dwalltime() - tick;
    //------------FIN DE  CONTAR EL TIEMPO-----------------------

    printf("matriz: %dx%d\n",N,N);
    printf("tam_bloque: %d\n",tam_bloque);
    printf("Tiempo requerido total de la ecuacion: %f\n",timeTotal);
    
    //Verifica el resultado
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
    
    free(A);
    free(B);
    free(C);
    free(R);
    free(D);
    free(AB);
    free(CD);
    return 0;
}
