# Informe Trabajo Práctico 3 | Programación en pasaje de mensajes / Programación híbrida

_Por Franco Kumichel_

### Introducción

Se propone realizar un informe que describa brevemente las soluciones planteadas, análisis de resultados y conclusiones. El presente se divide en dos partes principales: en la primera se resuelven los ejercicios 2 y 3 correspondientes a la práctica 4 para los cuales se ejecutan los algoritmos provistos por la cátedra y se responden las preguntas planteadas en dichos ejercicios, y luego en la segunda parte se resuelve la expresión dada desarrollando un algortimo paralelo empleando mpi y otro algoritmo paralelo híbrido empleando MPI + OpenMP. De cada caso se analiza su rendimiento y escalabilidad tanto individual como comparativamente y se incluyen tablas con los tiempos de ejecución, valores de Speedup, Eficiencia y Overhead de comunicación para cada caso.

#### Características del hardware y software utilizados:

Cluster provisto por la cátedra de Sistemas Paralelos de la Facultad de Informatica UNLP, el cual posee las siguientes características:

- 8GB de RAM
- Dos procesadores Intel XeonE5405

El procesador Intel XeonE5405 posee las siguientes características:

- Frecuencia básica de 2.0GHz
- Caché: 12MB L2 Caché
- Número de cores físicos: 4
- Hilos por core: 4
- Velocidad del bus: 1333MHz

### Parte 1
#### Ejercicio 2
Los códigos blocking.c y non-blocking.c siguen el patrón master-worker, donde los procesos worker le envían un mensaje de texto al master empleando operaciones de comunicación bloqueantes y no bloqueantes, respectivamente.

- Compile y ejecute ambos códigos usando P={4,8,16} (no importa que el número de núcleos sea menor que la cantidad de procesos). ¿Cuál de los dos retorna antes el control? 
<br>

    A continuación se muestran las salidas para _blocking.c_ y _non-blocking.c_

    ```bash
    Tiempo transcurrido 0.000002 (s):       proceso 0, llamando a MPI_Recv() [bloqueante] (fuente rank 1)
    Tiempo transcurrido 2.000265 (s):       proceso 0, MPI_Recv() devolvio control con mensaje: Hola Mundo! Soy el proceso 1
    Tiempo transcurrido 2.000277 (s):       proceso 0, llamando a MPI_Recv() [bloqueante] (fuente rank 2)
    Tiempo transcurrido 4.000113 (s):       proceso 0, MPI_Recv() devolvio control con mensaje: Hola Mundo! Soy el proceso 2
    Tiempo transcurrido 4.000128 (s):       proceso 0, llamando a MPI_Recv() [bloqueante] (fuente rank 3)
    Tiempo transcurrido 6.000145 (s):       proceso 0, MPI_Recv() devolvio control con mensaje: Hola Mundo! Soy el proceso 3

    Tiempo total = 0.000000 (s)
    ```

    ```bash
    Tiempo transcurrido 0.000001 (s):       proceso 0, llamando a MPI_IRecv() [no bloqueante] (fuente rank 1)
    Tiempo transcurrido 0.000048 (s):       proceso 0, MPI_IRecv() devolvio el control..
                                            ..pero el mensaje no fue aun recibido..
    Tiempo transcurrido 1.999974 (s):       proceso 0, operacion receive completa con mensaje: Hola Mundo! Soy el proceso 1
    Tiempo transcurrido 1.999987 (s):       proceso 0, llamando a MPI_IRecv() [no bloqueante] (fuente rank 2)
    Tiempo transcurrido 1.999995 (s):       proceso 0, MPI_IRecv() devolvio el control..
                                            ..pero el mensaje no fue aun recibido..
    Tiempo transcurrido 3.999921 (s):       proceso 0, operacion receive completa con mensaje: Hola Mundo! Soy el proceso 2
    Tiempo transcurrido 3.999933 (s):       proceso 0, llamando a MPI_IRecv() [no bloqueante] (fuente rank 3)
    Tiempo transcurrido 3.999940 (s):       proceso 0, MPI_IRecv() devolvio el control..
                                            ..pero el mensaje no fue aun recibido..
    Tiempo transcurrido 6.000016 (s):       proceso 0, operacion receive completa con mensaje: Hola Mundo! Soy el proceso 3

    Tiempo total = 0.000000 (s)

    ```

En las salidas mostradas anteriormente se puede observar la diferencia entre _MPI_Recv()_ y _MPI_IRecv()_, ya que el primero no retorna el control hasta que no se haya recibido y leido el mensaje solicitado completamente al buffer. Sin embargo el segundo retorna el control inmediatamente, requiriendo mas adelante llamar a _MPI_Wait()_ sobre la petición realizada _MPI_IRecv()_ para esperar a que se termine de leer el mensaje.

- En el caso de la versión no bloqueante, ¿qué sucede si se elimina la operación MPI_Wait() (línea 52)? ¿Se imprimen correctamente los mensajes enviados? ¿Por qué?
<br>

    Salida de _non-blocking.c_ eliminando la operación _MPI_Wait()_ (linea 52):
    <br>
    ```bash
    Tiempo transcurrido 0.000001 (s):       proceso 0, llamando a MPI_IRecv() [no bloqueante] (fuente rank 1)
    Tiempo transcurrido 0.000044 (s):       proceso 0, MPI_IRecv() devolvio el control..
                                            ..pero el mensaje no fue aun recibido..
    Tiempo transcurrido 0.000060 (s):       proceso 0, operacion receive completa con mensaje: No deberia estar leyendo esta frase.
    Tiempo transcurrido 0.000067 (s):       proceso 0, llamando a MPI_IRecv() [no bloqueante] (fuente rank 2)
    Tiempo transcurrido 0.000077 (s):       proceso 0, MPI_IRecv() devolvio el control..
                                            ..pero el mensaje no fue aun recibido..
    Tiempo transcurrido 0.000094 (s):       proceso 0, operacion receive completa con mensaje: No deberia estar leyendo esta frase.
    Tiempo transcurrido 0.000103 (s):       proceso 0, llamando a MPI_IRecv() [no bloqueante] (fuente rank 3)
    Tiempo transcurrido 0.000113 (s):       proceso 0, MPI_IRecv() devolvio el control..
                                            ..pero el mensaje no fue aun recibido..
    Tiempo transcurrido 0.000125 (s):       proceso 0, operacion receive completa con mensaje: No deberia estar leyendo esta frase.

    Tiempo total = 0.000000 (s)

    ```

    Los mensajes no se imprimen correctamente. En vez de imprimir los mensajes enviados por los procesos, se puede lee en la salida _no debería estar leyendo esta frase_, el cual es el valor inicial asignado al buffer. Esto se debe a que al no esperar la llegada del mensaje mediante _MPI_Wait()_, el proceso _MASTER_ imprime inmediatamente el contenido del buffer que no se actualizó con el mensaje esperado.

#### Ejercicio 3
Los códigos blocking-ring.c y non-blocking-ring.c comunican a los procesos en forma de anillo empleando operaciones bloqueantes y no bloqueantes, respectivamente. Compile y ejecute ambos códigos empleando P={4,8,16} (no importa que el número de núcleos sea menor que la cantidad de procesos) y N={10000000, 20000000, 40000000, …}. ¿Cuál de los dos algoritmos requiere menos tiempo de comunicación? ¿Por qué?

Se han compilado y ejecutado ambos algoritmos variando el tamaño de la matriz N y la cantidad de procesos P, obteniendo los siguientes resultados:

**blocking-ring:**

|P/N  |10000000|20000000|40000000|
|:---:|  :---: | :---:  | :---:  |
|  4  |0.248836|0.495090|0.985015|
|  8  |0.570116|1.137427|2.308255|
|  16 |2.369974|4.720051|9.579285|

**non-blocking-ring:**

|P/N  |10000000|20000000|40000000|
|:---:|  :---: | :---:  | :---:  |
|  4  |0.220926|0.442457|0.878886|
|  8  |0.300627|0.592935|1.179835|
|  16 |0.966698|1.931252|3.907735|

Se puede observar en los resultados obtenidos que el algoritmo no bloqueante (_non-blocking-ring.c_) requiere menos tiempo de comunicación. Observando el código podemos inferir que la diferencia está en el uso de _MPI_Recv()_ y _MPI_Send()_, las cuales son bloqueantes, contra _MPI_IRecv()_ y _MPI_Isend()_, que son no bloqueantes, para enviar los vectores _sendbuff_ y _recvbuff_. En _blocking-ring.c_ todos los procesos menos el último realizan en primer lugar _MPI_Recv()_ y luego _MPI_Send()_. Al ser _MPI_Recv()_ bloqueante, estos procesos no pueden comenzar a enviar sus vectores _sendbuff_ hasta que no reciban completamente el vector _recvbuff_. Por otro lado, el algortimo _non-blocking-ring.c_ utiliza versiones no bloqueantes para el envio y recepción de mensajes, por lo que no tienen ete problema, pueden enviar y recibir ambos vectores en simultáneo y luego esperar que ambas operaciones finalicen.

### Parte 2

Dada la siguiente expresión:

$𝑅 = \dfrac{(𝑀𝑎𝑥𝐴 × 𝑀𝑎𝑥𝐵 − 𝑀𝑖𝑛𝐴 × 𝑀𝑖𝑛𝐵)}{𝑃𝑟𝑜𝑚𝐴 × 𝑃𝑟𝑜𝑚𝐵} × [𝐴 × 𝐵] + [𝐶 × 𝐷]$

- Donde $A, B, C, D$ y $R$ son matrices cuadradas de $NxN$ con elementos de tipo double.
- $MaxA, MinA$ y $PromA$ son los valores máximo, mínimo y promedio de la matriz A, respectivamente.
- $MaxB, MinB$ y $PromB$ son los valores máximo, mínimo y promedio de la matriz $B$, respectivamente.

Desarrolle 2 algoritmos que computen la expresión dada:
1. Algoritmo paralelo empleando MPI
2. Algoritmo paralelo híbrido empleando MPI+OpenMP

Mida el tiempo de ejecución de los algoritmos en el cluster remoto. Las prueban deben considerar la variación del tamaño de problema (N={512, 1024, 2048, 4096}) y, en el caso de los algoritmos paralelos, también la cantidad de núcleos (P={8,16,32} para MPI, es decir, 1, 2 y 4 nodos, respectivamente; P={16,32} para híbrido, es decir, 2 y 4 nodos, respectivamente).

Para la implementación de los algoritmos solicitados, se tuvieron en cuenta los siguientes aspectos:

#### Algoritmo paralelo utilizando MPI
Esta solución se basa en la librería **OpenMPI**, la cual utiliza el estandar MPI (Message Passing Interface). En este caso las operaciones utilizadas son:

- *MPI_Barrier()*: se utiliza para sincronizar los procesos para poder medir los tiempos correctamente.

- *MPI_Scatter()*: se utiliza para enviar las matrices que se ordenan por filas. Esto se debe a que cada proceso debe tener una parte de la matriz para aplicarle las operaciones que le corresponda.

- *MPI_Bcast()*: se utiliza para enviar las matrices que se ordenan por columnas. Esto se debe a que los procesos necesitan la matriz entera, para poder acceder a todas sus columnas al realizar la multiplicación de matrices.

- *MPI_Allreduce()*: se utiliza para poder recibir el máximo, mínimo y suma que encontró cada proceso de las matrices $A$ y $B$, con el fin de obtener el valor correspondiente.

- *MPI_Gather()*: se utiliza para que luego que todos los procesos hayan ejecutado las operaciones necesarias, el proceso coordinador obtenga todas las partes de la matriz resultante.

#### Algoritmo paralelo híbrido utilizando MPI + OpenMP
Esta solución híbrida combina la memoria compartida con el pasaje de mensajes aprovechando las potencialidades de cada uno. La comunicación entre hilos que pertenecen a la misma maquina física se realiza utilizando memoria compartida mientras que la comunicación entre ellas se lleva a cabo por medio de pasaje de mensajes. 

El algoritmo implementa un enfoque de interacción master/worker, donde el master asume tanto el rol de coordinador como de trabajador. Se utilizan directivas de OMP (OpenMP) para controlar el paralelismo a nivel de hilo dentro de cada proceso, mientras que las comunicaciones de MPI se utilizan para sincronizar y compartir datos entre los distintos procesos.

Al inicio, el coordinador distribuye las matrices a los distintos procesos utilizando comunicaciones colectivas. Como todos los procesadores tienen la misma capacidad de cómputo y los bloques son del mismo tamaño, los procesos trabajan a la misma velocidad, repartiéndose equitativamente las filas de las matrices.

Luego, cada proceso entra en una región paralela con la macro *#pragma omp parallel* para realizar los cálculos con múltiples hilos. La macro *#pragma omp single* asegura que una sección del código se ejecute una sola vez por proceso, facilitando que solo un proceso por nodo se comunique con los demás.

Finalmente, tras completar las operaciones en paralelo, los resultados se reúnen en el proceso coordinador mediante la comunicación colectiva *MPI Gather*.

#### Resultados de ejecución:

Los algoritmos se ejecutaron con las siguiente características:

- Se utilizo la optimización O3 ya que fue la que dio resultados más favorables en cuanto a performance y la precisión matemática que brinda.

- En la medida de lo posible, se utilizo como tamaño de bloque 128 que fue el que mejores tiempos de ejecución brindo según se vio en las anteriores entregas. En los casos donde no fue posible utilizar este valor de bloque, se opto por utilizar el segundo que dio mejores resultados, sino el tercero y asi prosiguiendo.

**Algoritmo secuencial**

Tiempos de ejecución:

| N   |   512    |   1024   |  2048   |   4096   |
|:---:|  :---:   |  :---:   |  :---:  |  :---:   |
| Tiempo ejecución    | 0.427138 | 3.405070 |27.118076 |216.650521 |

**Algoritmo paralelo con OpenMP**

Tiempos de ejecución:

|P/N  |   512    |   1024   |  2048   |   4096   |
|:---:|  :---:   |  :---:   |  :---:  |  :---:   |
|  2  | 0.230495 | 1.783829 |14.155644|113.043257|
|  4  | 0.116238 | 0.894773 |7.100621 |56.878447 |
|  8  | 0.064789 | 0.462704 |3.638109 |29.648739 |

**Algoritmo paralelo con MPI:**

Tiempos de ejecución total:

|P/N  |   512    |   1024   |  2048   |   4096   |
|:---:|  :---:   |  :---:   |  :---:  |  :---:   |
|  8  | 0.091785 | 0.553571 |4.103143 |32.336629 |
|  16 | 0.107200 | 0.569465 |3.028158 |20.361223 |
|  32 | 0.119979 | 0.570909 |2.601665 |13.568427 |

Tiempos de comunicación:

|P/N  |   512    |   1024   |  2048   |   4096   |
|:---:|  :---:   |  :---:   |  :---:  |  :---:   |
|  8  | 0.031544 | 0.091479 |0.387143 |1.471631  |
|  16 | 0.087693 | 0.341555 |1.212650 |5.133080  |
|  32 | 0.121441 | 0.379243 |1.515286 |5.889797  |


**Algoritmo paralelo híbrido (MPI + OpenMP):**

Tiempos de ejecución total:

|P/N  |   512    |   1024   |  2048   |   4096   |
|:---:|  :---:   |  :---:   |  :---:  |  :---:   |
|  16 | 0.107698 | 0.525190 |2.949862 |19.267635 |
|  32 | 0.111222 | 0.494151 |2.357711 |13.296873 |

Tiempos de comunicación:

|P/N  |   512    |   1024   |  2048   |   4096   |
|:---:|  :---:   |  :---:   |  :---:  |  :---:   |
|  16 | 0.073313 | 0.283945 |1.119810 |4.447214  |
|  32 | 0.095832 | 0.352190 |1.380991 |5.905809  |

#### Speedup, Eficiencia y Overhead de comunicación

El **Speedup** es la relación entre el tiempo de ejecución de una tarea en un sistema secuencial y el tiempo de ejecución de la misma tarea en un sistema paralelo. Para calcular el mismo se utiliza la siguiente relación:

$S_{p}(n) = \dfrac{T_{s}(n)}{T_{p}(n)}$

La **eficiencia** mide la relación entre el tiempo empleado y la cantidad de recursos utilizados para realizar una tarea específica en un entorno paralelo. Una alta eficiencia indica que el sistema está aprovechando de manera óptima sus recursos para completar las tareas. Este se calcula de la siguiente manera:

$E_{p}(n) = \dfrac{S_{p}(n)}{S_{optimo}}$

El overhead de comunicación la relación entre el tiempo requerido por las comunicaciones de nuestra solución y el tiempo total que esta requiera. Se calcula con la siguiente formula:

$OC_{p}(n) = \dfrac{Tcomm_{p}(n)}{T_{p}(n)} \times 100$

**Algoritmo paralelo con OpenMP**

Speedup

|P/N  |   512    |   1024   |  2048   |   4096   |
|:---:|  :---:   |  :---:   |  :---:  |  :---:   |
|  2  | 1.853133 | 1.908854 |1.915707 |1.916527  |
|  4  | 3.674684 | 3.805512 |3.819113 |3.809009  |
|  8  | 6.592754 | 7.359067 |7.453893 |7.307242  |

Eficiencia

|P/N  |   512    |   1024   |  2048   |   4096   |
|:---:|  :---:   |  :---:   |  :---:  |  :---:   |
|  2  | 0.926566 | 0.954427 |0.957853 |0.958263  |
|  4  | 0.918671 | 0.951378 |0.954778 |0.952252  |
|  8  | 0.824094 | 0.919883 |0.931736 |0.913405  |

**Algoritmo paralelo con MPI**

Speedup

|P/N  |   512    |   1024   |  2048   |   4096   |
|:---:|  :---:   |  :---:   |  :---:  |  :---:   |
|  8  | 4.653679 | 6.151098 |6.609098 |6.699848  |
|  16 | 3.984496 | 5.979419 |8.955304 |10.640349 |
|  32 | 3.560106 | 5.964295 |10.423354|15.967254 |

Eficiencia

|P/N  |   512    |   1024   |  2048   |   4096   |
|:---:|  :---:   |  :---:   |  :---:  |  :---:   |
|  8  | 0.581709 | 0.768887 |0.826137 |0.837481  |
|  16 | 0.249031 | 0.373713 |0.559706 |0.665021  |
|  32 | 0.111253 | 0.186384 |0.325729 |0.498997  |

Overhead por comunicación

|P/N  |   512    |   1024   |  2048   |   4096   |
|:---:|  :---:   |  :---:   |  :---:  |  :---:   |
|  8  | 0.343672 | 0.165252 |0.094352 |0.045509  |
|  16 | 0.818031 | 0.599782 |0.400457 |0.252100  |
|  32 | 1.000000 | 0.664279 |0.582429 |0.434081  |

**Algoritmo paralelo híbrido (MPI + OpenMP)**

Speedup

|P/N  |   512    |   1024   |  2048   |   4096   |
|:---:|  :---:   |  :---:   |  :---:  |  :---:   |
|  16 | 3.966071 | 6.483501 |9.192998 |11.244271 |
|  32 | 3.840409 | 6.890747 |11.501866|16.293343 |

Eficiencia

|P/N  |   512    |   1024   |  2048   |   4096   |
|:---:|  :---:   |  :---:   |  :---:  |  :---:   |
|  16 | 0.247879 | 0.405218 |0.574562 |0.702766  |
|  32 | 0.120012 | 0.215335 |0.359433 |0.509166  |

Overhead por comunicación

|P/N  |   512    |   1024   |  2048   |   4096   |
|:---:|  :---:   |  :---:   |  :---:  |  :---:   |
|  16 | 0.680727 | 0.540651 |0.379614 |0.230812  |
|  32 | 0.861628 | 0.712717 |0.585733 |0.444150  |

#### Análisis de los resultados

**MPI vs OpenMP**

En las tablas de los tiempos se observa que el algoritmo que utiliza OpenMP es mas rápido. Como consecuencia de esto, el SpeedUp y por último la eficiencia de este es mejor en comparación del algoritmo con MPI.

**MPI vs Híbrido (MPI+OpenMP)**

Al analizar las tablas, se puede ver que los tiempos de comunicación son mayores en MPI. Esto sucede porque se tienen mas procesos que deben comunicarse a través del pasaje de mensajes. Por ejemplo para P=32, en MPI se tienen 32 procesos que deben comunicarse entre si usando pasaje de mensajes.
En cambio en el híbrido, solamente se crean 4 procesos, que luego cada uno crea 8 threads, que utilizan memoria compartida, así disminuyendo el pasaje de mensajes. Como consecuencia a que los tiempos del híbrido son mejores, el speedUp y la eficiencia de este también tienen resultados mas óptimos.

En los gráficos de eficiencia y overhead de comunicación tanto de MPI como del híbrido se puede ver que mas grande son las barras que indican el overhead, más se achica el de la eficiencia. Que esto suceda es muy coherente, ya que a mayor tiempo se gaste realizando las comunicaciones, menor tiempo
se van a estar empleando las unidades de procesamiento para realizar computo. Se puede constatar que al incrementar el N, al tener que realizar mas computo, el overhead se decrementa y la eficiencia aumenta.

**Rendimiento y escalabilidad**

**Rendimiento**: esta característica de los programas paralelos se puede evaluar, analizando el SpeedUp y teniendo en cuenta las dos leyes:
- *Ley de Amdahl:* Permite estimar el Speedup alcanzable en aquellos programas paralelos que
contienen partes secuenciales.
- *Ley de Gustafson:* Reescribió la ecuación que planteó Amdahl para estimar el SpeedUp máximo
alcanzable, pero teniendo en cuenta el tamaño del problema (N).

**Escalabilidad:** esta característica de los programas paralelos hace referencia a la capacidad que tiene un sistema de mantener un nivel de eficiencia fijo al incrementar tanto el numero de unidades de procesamiento (P), como el tamaño del problema a resolver (N).

**Algoritmo paralelo MPI**

*Rendimiento:*
- Ley de Amdahl: en la tabla se puede ver que al incrementar el P, y mantener fijo el N, el speedUp mejora (excepto en valores con N chico, ya que se produce un gran overhead de comunicación).
Esto es porque al utilizar una mayor cantidad de unidades de procesamiento la solución al problema se obtiene en un tiempo menor.

- Ley de Gustafson: al aumentar el tamaño del problema (N) y mantener el P también mejora el speedUp. Esto sucede porque la proporción de código que se ejecuta en paralelo, crece mayormente a la sección secuencial.

**Escalabilidad**:

- No es fuertemente escalable: debido a que si incrementamos el valor de P, y mantenemos fijo el valor de N, la eficiencia no se mantiene en un valor fijo.

- Para analizar si es débilmente escalable podemos mirar en diagonal los valores de la eficiencia. Si quisiéramos hacer una comparación equitativa entre MPI y el híbrido deberíamos mirar la tabla solo cuando P=16 y P=32. Mirando en estos valores podemos concluir que es débilmente escalable, ya que en una de las diagonales vemos que la eficiencia se mantiene entre el 55% y 49%. Se concluye entonces que el algoritmo realizado con MPI, es débilmente escalable.

**Algoritmo Híbrido (MPI + OpenMP)**
*Rendimiento*:
- Ley de Amdahl: en la tabla se puede ver que al incrementar el P, y mantener fijo el N, el speedUp mejora. Esto es porque al utilizar una mayor cantidad de unidades de procesamiento la solución al problema se obtiene en un tiempo menor.

- Ley de Gustafson: al aumentar el tamaño del problema (N) y mantener el P también mejora el speedUp. Esto sucede porque la proporción de código que se ejecuta en paralelo, crece mayormente a la sección secuencial.

**Escalabilidad:**

- No es fuertemente escalable: debido a que si incrementamos el valor de P, y mantenemos fijo el valor de N, la eficiencia no se mantiene en un valor fijo.

- Si es débilmente escalable: si incrementamos el valor de P y además incrementamos el N (como por ejemplo con “P=16 y N=1024” con “P=32 y N=2048”), se puede ver que la eficiencia se mantiene aproximadamente en un 40-35%.

#### Conclusión

En este trabajo se compararon tres soluciones paralelas (MPI, OpenMP y MPI+OpenMP) para resolver la ecuación propuesta, contrastándolas también con una solución secuencial. 

Uno de los aspectos en común del algoritmo híbrido y el que utiliza MPI es el impacto del overhead de las comunicaciones. Cuando se trabaja con matrices pequeñas y un P elevado se observa una reducción considerable en el SpeedUp debido a este factor. En el caso de OpenMP esto sucede debido al costo de
sincronización entre los hilos.

La solución híbrida que combina MPI y OpenMP resulta la más prometedora, ya que logra aprovechar las ventajas de ambos enfoques (memoria compartida junto con comunicación por mensajes) y muestra una mejor escalabilidad.

La elección de la solución depende del tamaño de las matrices y la necesidad de escalabilidad. Para matrices pequeñas, OpenMP es más eficiente debido a su mejor aprovechamiento de la memoria compartida. Para matrices grandes y escenarios que requieran alta escalabilidad y uso de múltiples nodos, la solución híbrida es probablemente la más adecuada.