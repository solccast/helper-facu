#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <float.h>

//Para calcular el tiempo de ejecución
double dwalltime(){
	double sec;
	struct timeval tv;
	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}

//Para imprimir el resultado
void printMatrix(double *matrix, int n) {
    for (int i = 0; i < n; i++) {        
        for (int j = 0; j < n; j++) {    
            printf("%8.2lf ", matrix[i * n + j]);  
        }
        printf("\n");  // Nueva línea al final de cada fila
    }
    printf("\n");  // Espaciado adicional entre matrices si se imprimen varias
}


/************** MAIN *************/
int main(int argc, char *argv[]){
    double *A, *B, *BT, *C, *R, *AB, *CBT;
    double maxA, minA, maxB, minB, promedioA, promedioB;
    int N, tam_bloque, cant_elementos;
    int i, j, k, ii, jj, kk;
    int offI,offJ;
    double timetick;
  

    //Verificar parametro 
    if ( (argc != 3) || ((N = atoi(argv[1])) <= 0) || ((tam_bloque = atoi(argv[2])) <= 0) || ((N % tam_bloque) != 0)){
        printf("Error en los parámetros - N debe ser multiplo de tamaño de bloque");
        exit(1);
    }
    
    
  
    cant_elementos = N*N;

    A=(double*) malloc(N*N*sizeof(double));
    B=(double*) malloc(N*N*sizeof(double));
    BT=(double*) malloc(N*N*sizeof(double));
    C=(double*) malloc(N*N*sizeof(double));
    R=(double*) malloc(N*N*sizeof(double));
    CBT=(double*) malloc(N*N*sizeof(double));
    AB=(double*) malloc(N*N*sizeof(double));

    //Inicializar matrices
    double value = 0;
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            value += 1;
            A[i*N+j]=1.0; //Ordenada por Filas
            B[j*N+i]=value; //Ordenada por Columnas
            C[i*N+j]=2.0; //Ordenada Por Filas
            AB[i*N+j]=0.0; //Ordenada Por Filas
            CBT[i*N+j]=0.0; //Ordenada Por Filas
        }
    }
    
    //Inicializar variables
    maxA = -DBL_MAX;
    minA= DBL_MAX;
    maxB = -DBL_MAX;
    minB = DBL_MAX;
    promedioA = 0.0;
    promedioB = 0.0;
 
    //------------Comienza a correr el tiempo-----------------------
    timetick = dwalltime();
  
  
    //1) Calculo de los promedios, min, max 
    for (i = 0; i < N; i++){
      int offset_i = i*N;
      for (j = 0; j < N; j++){
        double value_a = A[offset_i +j];
        if (value_a > maxA){
          maxA = value_a;
        }
        if (value_a < minA){
          minA = value_a;
        }
        promedioA += value_a;
      }
    }
  
    promedioA = promedioA / cant_elementos;
  
    for (i = 0; i < N; i++){
      int offset_i = i*N;
      for (j = 0; j < N; j++){
         double value_b = B[j*N +i];
         if (value_b > maxB){
          maxB = value_b;
        }
        if (value_b < minB){
          minB = value_b;
        }
        promedioB += value_b;
      
      }
    }
  
    promedioB = promedioB / cant_elementos;
  
  
    //2) Calculo de A X B, el resultado va a estar ordenado por filas 
    for (i = 0; i < N; i += tam_bloque) {
        for (j = 0; j < N; j += tam_bloque) {
            for  (k = 0; k < N; k += tam_bloque) {
                for (ii = i; ii < i + tam_bloque; ii++) {
                    offI=ii*N;
                    for (jj = j; jj < j + tam_bloque; jj++){
                        double resultado_parcial = 0.0;
                        for (kk = k; kk < k + tam_bloque; kk++) {
                            resultado_parcial += A[offI+kk] * B[kk*N+jj];
                        }
                        AB[offI+jj] += resultado_parcial;
                    }
                }
            }
        }
    } 
    
    // 3) Calculo BT
    for (i = 0; i < N; i++){
      offI = i*N;
      for (j = 0; j < N; j++){
        BT[j*N + i] = B[offI + j];
      }
    } 

    // 4) Calculo de C x BT, el resultado va a estar ordenado por filas
    for (i = 0; i < N; i += tam_bloque) {
        for (j = 0; j < N; j += tam_bloque) {
            for  (k = 0; k < N; k += tam_bloque) {
                for (ii = i; ii < i + tam_bloque; ii++) {
                    offI=ii*N;
                    for (jj = j; jj < j + tam_bloque; jj++){
                        double resultado_parcial = 0.0;
                        offJ = jj*N;
                        for (kk = k; kk < k + tam_bloque; kk++) {
                            resultado_parcial += C[offI+kk] * BT[kk*N+jj];
                        }
                        CBT[offI+jj] += resultado_parcial;
                    }
                }
            }
        }
    }
  
  
    // 5) Calculo el coeficiente 
    double coeficiente = (maxA*maxB - minA*minB)/(promedioA*promedioB);
  
  
    //6) Obtengo R = coeficiente* AB + CBT
    for (i = 0; i < N; i++){
      int offI = i*N;
      for (j = 0; j < N; j++){
        R[offI + j] = AB[offI + j] * coeficiente + CBT[offI + j];
      }
    }
  
  
    double workTime = dwalltime() - timetick;

    /*
    printf("MMBLK-SEC\n");
    printf("Tamaño de la matriz (n): %d\n", N);
    printf("Tamaño del bloque (bs): %d\n", tam_bloque);
    printf("Tiempo de ejecución (segundos): %lf\n", workTime);
    printf("Rendimiento (GFLOPS): %lf\n", ((double)2*N*N*N)/(workTime*1000000000));
    */
    
    printf("%f;",workTime);
    printf("%lf;", ((double)2*N*N*N)/(workTime*1000000000));
    
    /*
    //Imprimo la matriz resultado
    printf("Matriz B:\n");
    printMatrix(B, N);
  
    printf("Matriz BT:\n");
    printMatrix(BT, N);

    printf("Matriz A:\n");
    printMatrix(A, N);
    
    printf("Matriz C:\n");
    printMatrix(C, N);

    
    printf("Matriz AB:\n");
    printMatrix(AB, N);
    
    printf("Matriz CBT:\n");
    printMatrix(CBT, N);
    
    printf("Matriz R:\n");
    printMatrix(R, N);
    

    printf("%.5f\n", coeficiente);
    */

   
    free(A);
    free(B);
    free(BT);
    free(C);
    free(R);
    free(AB);
    free(CBT);
  

    return 0;
}



