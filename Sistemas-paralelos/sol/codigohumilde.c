#include<stdio.h>
#include<stdlib.h>

//Para calcular tiempo
double dwalltime(){
    double sec;
    struct timeval tv;

    gettimeofday(&tv,NULL);
    sec = tv.tv_sec + tv.tv_usec/1000000.0;
    return sec;
}

void verificarResultado(double *M, int N);

int main (int argc,char*argv[]){

    double *A, *B, *C; //Matrices cuadradas de NxN con elementos de tipo double
    double *R; //Matriz resultado de la multiplicacion de todas las matrices 
    double *RES_PARCIAL; //Matriz resultado de la multiplicacion de A y B
    double *RES_PRIMERA_PARTE_FORM;
    double *RES_PARCIAL_T; //Matriz resultado de la multiplicacion de C y B transpuesta
    int i, j, k, sub_i, sub_j, sub_k;
    double timetick;
    double minA, minB, maxA, maxB, promA, promB;  
    double den, num;
    double escalar = 0.0; 
    double valorActual; 
    int cantidad_elementos_totales;


    if (argc < 3) {
        printf("Uso: %s <n> <tam_bloque>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int tam_bloque = atoi(argv[2]);

    if (n <= 0 || tam_bloque <= 0) {
        printf("Error: Los valores de n y bs deben ser mayores que 0\n");
        return 1;
    }

    // Verifica que bs sea divisor de n
    if (n % tam_bloque != 0) {
        printf("Error: bs debe ser un divisor exacto de n\n");
        return 1;
    }

    printf("Parámetros recibidos correctamente: n=%d, bs=%d\n", n, tam_bloque);

    // Reservamos memoria para las matrices
    A = (double *) malloc(n*n*sizeof(double)); //Reservamos memoria para A
    B = (double *) malloc(n*n*sizeof(double)); //Reservamos memoria para B
    C = (double *) malloc(n*n*sizeof(double)); //Reservamos memoria para C
    R = (double *) malloc(n*n*sizeof(double)); //Reservamos memoria para R
    RES_PARCIAL = (double *) malloc(n*n*sizeof(double)); //Reservamos memoria para RES_PARCIAL
    RES_PARCIAL_T = (double *) malloc(n*n*sizeof(double)); //Reservamos memoria para RES_PARCIAL_T
    RES_PRIMERA_PARTE_FORM = (double *) malloc(n*n*sizeof(double)); //Reservamos memoria para RES_PRIMERA_PARTE_FORM

    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            A[i*n + j] = 1.0; // Inicializamos A por fila
            B[j*n + i] = 1.0; // Inicializamos B por columna
            C[i*n + j] = 1.0; // Inicializamos C por fila
            RES_PARCIAL[i*n + j]= 0.0;
            RES_PARCIAL_T[i*n + j]= 0.0;
            
        }
    }

    cantidad_elementos_totales = n * n; 

    //Imprimir vector A
    printf("Vector A:\n");
    for (int m = 0; m < cantidad_elementos_totales; m++){
        printf("%.1f ", A[m]);
    }
    printf("\n\nVector B:\n");
    for (int q = 0; q < cantidad_elementos_totales; q++){
        printf("%.1f ", B[q]);
    }
    printf("\n\nVector C:\n");
    for (int r = 0; r < cantidad_elementos_totales; r++){
        printf("%.1f ", C[r]);
    }
    printf("\n -------------------------------\n");

    timetick = dwalltime(); //Inicio del conteo 

    //---- Calculo del conteo del tiempo de ejecución ----- //

    // Recorrido de la matriz A
    minA = A[0]; // Inicializamos el minimo en el primer elemento de la matriz
    maxA = B[0]; // Inicializamos el maximo en el primer elemento de la matriz
    promA = 0; 

    //Posteriormente usaremos valorActual para evaluar la eficiencia 

    for (i = 0; i < n; i++){
        for (j = 0; j < n; j++){
            promA += A[i*n+j]; 
            if (A[i*n + j] < minA){
                minA = A[i*n + j]; // Si encontramos un nuevo minimo, lo actualizamos
            }
            if (A[i*n + j] > maxA){
                maxA = A[i*n + j]; // Si encontramos un nuevo maximo, lo actualizamos
            }
        }
    }

    // Calculo del promedio final de la matriz A
    promA = promA / (n * n); 


    // Recorrido de la matriz B
    minB = B[0]; // Inicializamos el minimo en el primer elemento de la matriz
    maxB = B[0]; // Inicializamos el maximo en el primer elemento de la matriz
    promB = 0.0;

    for (i = 0; i < n; i++){
        for (j = 0; j < n; j++){
            promB += B[i*n +j]; //Recorre de forma contigua el vector y no da saltos 
            if (B[i*n+j] < minB){
                minB = B[i*n+j]; // Si encontramos un nuevo minimo, lo actualizamos
            }
            if (B[i*n+j] > maxB){
                maxB = B[i*n+j]; // Si encontramos un nuevo maximo, lo actualizamos
            }
        }
    }

    promB = promB / (n * n); // Calculo del promedio final de la matriz B

    // Obtenemos el escalar parcial
    num = (maxA * maxB) - (minA * minB);
    den = promA * promB; 
    escalar = num / den; //Calculamos el escalar 

    
    printf("\nmaxA: %.2f, maxB: %.2f, minA: %.2f, minB: %.2f\n", maxA, maxB, minA, minB);
    printf("promA: %.2f, promB: %.2f\n", promA, promB); //Imprimimos los valores de maximo, minimo y promedio de A y B
    printf("Escalar parcial: %.2f\n", escalar); //Imprimimos el escalar parcial

    // Multiplicación de las matrices A y B 
    printf("Multiplicación de matrices A y B:\n");
    for (i = 0; i < n; i+= tam_bloque){
        for (j = 0; j < n; j+= tam_bloque){
            for (k = 0; k < n; k+= tam_bloque){
                //Recorrido submatrices 
                for (sub_i = i; sub_i < i + tam_bloque; sub_i++){
                    for (sub_j = j; sub_j < j + tam_bloque; sub_j++){
                        double sumaParcial = 0.0; // Inicializamos el valor actual en 0.0
                        for (sub_k = k; sub_k < k + tam_bloque; sub_k++){
                            sumaParcial += A[sub_i*n + sub_k] * B[sub_k*n + sub_j]; 
                            //printf("Estamos multiplicando: %.1f * %.1f => Resultado: %.1f\n", A[sub_i*n + sub_k], B[sub_j*n + sub_k], sumaParcial); //Imprimimos el valor de la multiplicacion
                        }
                        RES_PARCIAL[sub_i*n + sub_j] += sumaParcial * escalar; // Guardamos el resultado en la matriz RES_PARCIAL y le multiplicamos el resultado de la primera parte de la fórmula
                        //printf("Se almacena: %.1f\n", RES_PARCIAL[sub_i*n + sub_j]); //Mostrar qué almacena y en qué posición 
                    }
                }
            } 
        }
    }    

    // Multiplicación de matrices: C * B transpuesta
    for (i = 0; i < n; i+= tam_bloque){
        for (j = 0; j < n; j+= tam_bloque){
            for (k = 0; k < n; k+= tam_bloque){
                //Recorrido submatrices 
                for (sub_i = i; sub_i < i + tam_bloque; sub_i++){
                    for (sub_j = j; sub_j < j + tam_bloque; sub_j++){
                        double sumaParcial = 0.0; // Inicializamos el valor actual en 0.0
                        for (sub_k = k; sub_k < k + tam_bloque; sub_k++){
                            sumaParcial += C[sub_i*n + sub_k] * B[sub_j*n + sub_k]; 
                            //printf("Estamos multiplicando: %.1f * %.1f => Resultado: %.1f\n", A[sub_i*n + sub_k], B[sub_j*n + sub_k], sumaParcial); //Imprimimos el valor de la multiplicacion
                        }
                        RES_PARCIAL_T[sub_i*n + sub_j] += sumaParcial; // Almacenamos el resultado más la suma de la primera parte 
                        //printf("Se almacena: %.1f\n", sumaParcial); //Mostrar qué almacena y en qué posición 
                    }
                }
            } 
        }
    }
    for (int m = 0; m <n*n; m++){
        RES_PARCIAL_T[m] += RES_PARCIAL[m]; //Sumamos la matriz resultado de la primera parte con la segunda parte
    }
    timetick = dwalltime() - timetick; //Guardamos el tiempo de fin 

    for (int m = 0; m < cantidad_elementos_totales; m++){
        printf("%.1f ", RES_PARCIAL_T[m]);
    } 

    printf("Multiplicacion de matrices de %dx%d. Tiempo en segundos %f\n",n,n, timetick); //Imprimimos el tiempo de la multiplicacion de matrices

    verificarResultado(RES_PARCIAL_T, n); //Verificamos el resultado de la multiplicacion de matrices

    //Liberar memoria
    free(A);
    free(B);
    free(C);
    free(R);
    free(RES_PARCIAL);
    free(RES_PARCIAL_T);
    free(RES_PRIMERA_PARTE_FORM);
    return (0);
}

void verificarResultado(double *result, int N) {
    double correct_value = N;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (result[i * N + j] != correct_value) {
                printf("Incorrect result at (%d, %d): Expected %1.f, Got %1.f\n", i, j, correct_value, result[i * N + j]);
                return;
            }
        }
    }
    printf("Result is correct.\n");
}
// Fin del programa