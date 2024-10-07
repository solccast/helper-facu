## Semáforos 
- P(s) -> decrementa -> `<await (s>0) s = s-1>` 
- V(s) -> incrementa -> `<s = s+1>`

### Explicación - ejercicio 1
Hay C chicos y hay una bolsa con caramelos que nunca se vacía. Los chicos de a UNO van sacando de a UN caramelo y lo comen. Los chicos deben llevar la cuenta de cuantos caramelos se han tomado de la bolsa. 

```c
sem mutex = 1
int cant = 0

process Chico[id: 0..C-1]
{
    while (true)
    {
        P(mutex) //Inicio sección crítica
        tomarCaramelo()
        cant := cant + 1
        V(mutex) //Fin sección crítica
        comerCaramelo()
    }
}
``` 

### Explicación - ejercicio 2
Hay C chicos y hay una bolsa con caramelos limitada a N caramelos. Los chico de a UNO van sacando de a UN caramelo y lo comen. Los chicos deben llevar la cuenta de cuantos caramelos se han tomado de la bolsa. 

```c
int cant = 0
sem mutex = 1


process Chico[id: 0..C-1]
{
    P(mutex) 
    while(cant<N) //Mientras haya caramelos en la bolsa 
    {
        tomarCaramelo()
        cant = cant + 1
        V(mutex)
        comerCaramelo() //Concurrencia
        P(mutex) // Asegura la exclusion mutua porque vuelve a consultar la condición del while
    }
    V(mutex) //Libera el while
}
```

### Explicación - ejercicio 3
Hay C chicos y hay una bolsa con caramelos limitada a N caramelos administrada por UNA abuela. Cuando todos los chicos han llegado llaman a la abuela, y a partir de ese momento ella N veces selecciona un chico aleatoriamente y lo deja pasar a tomar un caramelo.

```c
int barrera = C
int contador = 0
sem barrera = 0
sem mutex = 1
sem espera_abuela = 0
sem espera_chicos[c-1] = ([C] 0)
bool hay_caramelos = true

process Chico[id: 0..c-1]
{
    P(mutex)
    contador := contador + 1
    if (contador == C)
    {
        for (i = 1..C)
        {
            V(barrera) //Incrementa la barrera para despertar a los procesos que se encuentran esperando
        }
        V(espera_abuela) //Despierta a la abuela
    } 
    V(mutex)
    P(barrera) //ESPERA a decrementar el valor. Se queda acá realizando busy waiting
    
    P(espera_chicos[id])
    while(hay_caramelos) //Haya caramelos 
    {
        tomarCaramelo()
        V(listo) //Incrementa para avisar a la abuela que ya agarró el caramelo
        comerCaramelo()
        P(espera_chicos[id]) //Espera a que la a abuela incremente este valor para avisarles a los chicos y corroboren el valor de hay_caramelos. Como es falso, se cierra el while.
    }
}


process Abuela
{
    P(espera_abuela)
    for (i: 1..N) 
    {
        id_random := int.obtenerNúmeroRandom(C) //Número random entre 0 y C-1
        V(espera_chicos[id_random])
        P(listo) //Espera a que el proceso lo tome  
    }
    hay_caramelos = false
    for (i: 0..C-1){
        V(espera_chicos[i]) //Avisa a todos para que no se queden esperando y chequeen la variable hay_caramelos 
    } 
}

```

### Explicación - ejercicio 4
En una empresa de genética hay N clientes que envían secuencias de ADN para que sean analizadas y esperan los resultados para poder continuar. Para resolver estos análisis la empresa cuenta con 1 servidores que resuelve los pedidos de acuerdo al orden de llegada de los mismos.

```c
sem mutex = 1
cola C;
sem pedidos = 0
sem analisis_listo[N] = ([N] 0)
int resultados[N]

process Cliente[id: 0..N-1]
{
    sec = generarSecuencia()
    P(mutex)
    push(C, (sec, id))
    V(mutex)
    V(pedidos) //Usado como contador de recursos se incrementa

    P(analisis_listo[id])
    resultado = resultados[id] //Obtengo los resultados
}


process Servidor
{
    while (true){

        P(mutex)
        P(pedidos) //Contador de recursos se decrementa
        pop(C,(sec, id_cliente))
        V(mutex)

        secuencia_lista = analizarSecuencia(sec)

        resultados[id_cliente] = secuencia_lista
        V(analisis_listo[id_cliente]) //Aviso al cliente que está listo

    }

}
```

### Explicación - ejercicio 6
En una montaña hay 30 escaladores que en una parte de la subida deben utilizar un único paso de a uno a la vez y de acuerdo al orden de llegada al mismo. 

```c
sem mutex = 1
cola C;
bool libre = true
sem consultar = 1
sem espera[30] = ([30] 0)

process escalador[id: 1..30]
{
    subirMontaña()

    P(mutex) //Protege la variable libre y la cola
    if (libre)
    {
        libre:= false
        V(mutex) //Libera en caso de que no haya nadie 
    }
    else 
    {
        push(C, id) //Se encola el escalador
        V(mutex) //Libera
        P(espera[id]) //Espera su turno
    }

    P(mutex_paso)
    cruzarElPaso()
    V(mutex_paso)

    P(mutex) //El mismo semáforo que se uso al inciio, porque son las mismas variables 
    if (isEmpty(C)) libre = true //Libera el recurso
    else
    {
        pop(C, id_siguiente)
        V(espera[id_siguiente]) //Le dice al siguiente que pase 
    }
    V(mutex)

}
```

---

# Práctica 2 

## Ejercicio 3 
Un sistema operativo mantiene 5 instancias de un recurso almacenadas en una cola. Además, existen P procesos que necesitan usar una instancia del recurso. Para eso, deben sacar la instancia de la cola antes de usarla. Una vez usada, la instancia debe ser encolada nuevamente para su reúso.  

```c

sem cant_recurso = 5
cola C;
sem mutex = 1
sem acceso = 1

process Proceso[id: 0..P-1]
{
    while(true)
    {
        P(cant_recurso) //Decrementa la cantidad de recursos. Si es 0 se queda esperando 
        P(mutex) //exclusión mutua para acceder al recurso
        pop(C, recurso)
        V(mutex)

        usarRecurso(recurso)

        P(mutex)
        push(C, recurso)
        V(cant_recurso) //Incremento la cantidad del recurso disponible
        V(mutex)
    }
}

```

## Ejercicio 11
En un vacunatorio hay un empleado de salud para vacunar a 50 personas. El empleado de salud atiende a las personas de acuerdo con el orden de llegada y de a 5 personas a la vez. Es decir, que cuando está libre debe esperar a que haya al menos 5 personas esperando, luego vacuna a las 5 primeras personas, y al terminar las deja ir para esperar por otras 5. Cuando ha atendido a las 50 personas el empleado de salud se retira. Nota: todos los procesos deben terminar su ejecución; suponga que el empleado tienen una función VacunarPersona() que simula que el empleado está vacunando a UNA persona.   

```c

cola C;
sem mutex = 1;
sem acceso[50] = ([50] 0)
sem personas = 0
int contador = 0
cola personas_vacunada;

process Persona[id: 1..50]
{
    irAlVacunatorio()

    P(mutex)
    contador = contador + 1 
    push(C, id)
    if (contador == 5)
    {
        contador == 0
        V(cumple_cantidad) //Avisa al personal de salud que ya se encuentran 5 personas en la fila 
    }
    V(mutex)


    P(salidaVacunatorio[id]) //Espera a que el personal de salud lo deje
    salirDelVacunatorio()
}


process PersonalDeSalud
{
    for (i: 1..10)
    {
        P(cumple_cantidad) //Espera a que sean 5 personas en la fila 
            for (i: 1..5)
            {
                P(mutex)
                pop(C, id_paciente)
                V(mutex)

                vacunarPaciente(id_paciente)
                push(personas_vacunadas, id_paciente)
                
            }
            for (i: 1..5){
                V(salidaVacunatorio[pop(personas_vacunadas, id_paciente)]) //Avisa a los 5 pacientes que entraron que ya se pueden ir 
            }
    }
}

```

---

# Práctica repaso - Memoria Compartida 
## Sección semáforos

### 1. a
En una estación de trenes, asisten P personas que deben realizar una carga de su tarjeta SUBE en la terminal disponible. La terminal es utilizada en forma exclusiva por cada persona de acuerdo con el orden de llegada. Implemente una solución utilizando únicamente procesos Persona. Nota: la función UsarTerminal() le permite cargar la SUBE en la terminal disponible.  

```c
cola C;
boolean libre = true
sem mutex = 1
int avisar_siguiente[P] = ([P] 0)

process Persona[id: 0..P-1]
{
    P(mutex)
    if (libre)
    {
        libre=false
        V(mutex)
    } else 
    {
        push(C, id)
        V(mutex)
        P(avisar_siguiente(id)) //Se queda acá esperando hasta que el que use la terminal sube 
    } 
    
    
    usarTerminalSUBE()
    P(mutex)
    if(isEmpty(C))
    {
        libre=true
    } else
    {
        id_siguiente = pop(c)
        V(avisar_siguiente[id_siguiente]) //Acá avisa al siguiente que se quedó esperando
    }
    V(mutex)

}
```

Resuelva el mismo problema anterior pero ahora considerando que hay T terminales disponibles. Las personas realizan una única fila y la carga la realizan en la primera terminal que se libera. Recuerde que sólo debe emplear procesos Persona. Nota: la función UsarTerminal(t) le permite cargar la SUBE en la terminal t. 

```c
sem terminales = T
cola C;
sem mutex = 1
int libres = T

process Persona[id: 0..P-1]
{

    P(mutex)
    if not(isEmpty(C)) //Si hay gente en la fila me uno
    {
        push(C, id) //La persona se suma a la fila 
        V(mutex)
        P(avisar_siguiente[pop(C)]) //Se queda esperando que sea su turno
    } else
    {
        V(mutex)
    }


    P(terminales) //decrementa el disponible
    usarTerminal(t) //Usa la terminal disponible
    P(mutex) 
    if not(isEmpty(C))
    {
        V(avisar_siguiente[pop(C)])
    }
    V(mutex)
    V(terminales)
}


```

### 3.
Implemente una solución para el siguiente problema. Se debe simular el uso de una máquina expendedora de gaseosas con capacidad para 100 latas por parte de U usuarios. Además, existe un repositor encargado de reponer las latas de la máquina. Los usuarios usan la máquina según el orden de llegada. Cuando les toca usarla, sacan una lata y luego se retiran. En el caso de que la máquina se quede sin latas, entonces le debe avisar al repositor para que cargue nuevamente la máquina en forma completa. Luego de la recarga, saca una botella y se retira. Nota: maximizar la concurrencia; mientras se reponen las latas se debe permitir que otros usuarios puedan agregarse a la fila. 

```c
cola C;
int latas = 100
sem siguiente_fila[U] = ([U] 0)
bool libre = true
sem mutex = 1
sem máquina = 1

process Usuario[id: 0..U-1]
{
    P(mutex)
    if not(libre)
    {    
        push(C, id)
        V(mutex)
        P(siguiente_fila[id])
    } else 
    {
        V(mutex)
    }
    
    P(máquina)
    latas:= latas - 1
    usarMáquina()
    if (latas == 0) V(máquina_vacia)
    V(máquina)
    P(mutex)
    if (isEmpty(C)) libre = true 
    else 
        P(siguiente_fila[pop(C)]) //Le aviso al otro que puede pasar
    V(mutex)
}


process Repositor
{
    while(true)
    {
        P(maquina_vacia)         
        P(máquina)
        recargarMáquinaConLatas(100)
        latas:= 100
        usarMáquina()
        V(máquina)

    }
}

```