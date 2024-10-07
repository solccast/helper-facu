# Parciales antiguos 

## 2023 - Primera fecha 

### Semáforos
En una estación de trenes, asisten P personas que deben realizar una carga de su tarjeta SUBE en la terminal disponible. La terminal es utilizada en forma exclusiva por cada persona de acuerdo con el orden de llegada. 
Implemente una solución utilizando procesos Persona. Nota: UsarTerminal() le permite cargar la SUBE en la terminal disponible.

```c
cola C;
sem mutex = 1 
sem siguiente_persona[P] = ([P] 0)
bool libre = true

process Persona[id: 0..P-1]{
    int id_sig;

    P(mutex)
    if not(libre){ //Si la máquina no está libre se encola 
        push(C, id)
        V(mutex) //Libera la SC
        P(siguiente_persona[id]) //Se queda esperando su turno
    } else {
        libre=false //Cambia el estado
        V(mutex)
    }
    
    usarTerminal()

    P(mutex)
    if not(isEmpty(C)){ //Si hay alguien en la fila le da el turno
        pop(C,id_sig)
        V(siguiente_persona[id_sig])
    } else{
        libre = true //Si no hay nadie libera la máquina
    }
    V(mutex)

}

```


Resuelva el mismo problema anterior pero ahora considerando que hay T terminales disponibles. Las personas realizan una única fila y la carga la realizan en la primera terminal que se libera. Recuerde que sólo debe emplear procesos Persona. Nota: la función UsarTerminal(t) le permite cargar la Sube en la terminal t.

```c
cola C;
sem mutex = 1 
sem siguiente_persona[P] = ([P] 0)
cola terminales; //Cargado con las terminales dispos
sem terminal=1;
int terminales_libres = T

process Persona[id: 0..P-1]{
    P(mutex)
    if (terminales_libres==0){
        push(C, id)
        V(mutex)
        P(siguiente_persona[id])
    } else {
        terminales_libres--
        V(mutex)
    }
    
    P(terminal)
    pop(terminales,t)
    V(terminal)
    usarTerminal(t)
    P(terminal)
    push(terminales, t)
    V(terminal)

    P(mutex)
    if not(isEmpty(C)){
        pop(C, id_sig)
        V(siguiente_persona[id_sig])    
    } else terminales_libres++
    V(mutex)
}

```

---

## 2022 - Primera fecha

### Semáforos
En una planta verificadora de vehículos existen 7 estaciones donde se dirigen 150 vehículos para ser verificados. Cuando un vehículo llega a la planta, el coordinador de la planta le indica a qué estación debe dirigirse. El coordinador selecciona la estación que tenga menos vehículos asignados en ese momento. Una vez que el vehículo sabe qué estación le fue asignada, se dirige a la misma y espera a que lo llamen para verificar. Luego de la revisión, la estación le entrega un comprobante que indica si pasó la revisión o no. Más allá del resultado, el vehículo se retira de la plata. Nota: maximizar la concurrencia.

```c
sem s_entrada = 1
cola en_espera;
sem s_entrada_coordinador = 0
int estacion[7] = ([0..6] 0)
int estacion_asignada[150];
sem aviso_asignacion[150] = ([0...149] 0)
cola fila_estacion[0..6];
sem sem_estacion[7] = ([7]1)
sem sem_asignacion = 1
sem sem_atencion_est[7] = ([7] 0)
string resultados[150];
sem espera_resultado[150] = ([150] 0)


process Vehiculo[id: 0..149]{
    int nro_estacion;

    P(s_entrada)
    push(en_espera, id)
    V(s_entrada)

    V(s_entrada_coordinador) //Avisa al coordinador que está el vehiculo

    P(aviso_asignacion[id]) 

    P(sem_asignacion)
    nro_estacion = estacion_asignada[id]    
    V(sem_asignacion) 

    P(sem_estacion[nro_estacion])
    push(fila_estacion[nro_estacion], id)
    V(sem_estacion[nro_estacion])
    V(sem_atencion_est[nro_estaacion])

    P(espera_resultado[id])
    P(sem_asignacion)
    estacion[nro_estacion]--
    V(sem_asignacion) 

    resultado_final = resultado[id]
}


process Estacion[id: 0..6]{
    while(true){
        P(sem_atencion_est[id])
        P(sem_estacion[id])
        pop(fila_estacion[id], id_vehiculo)
        V(sem_estacion[id])

        resul_vehiculo = generarResultado(id_vehiculo)
        
        resultado[id_vehiculo] = resul_vehiculo
        V(espera_resultado[id_vehiculo])

    }
}


process Coordinador{
    while(true){
        P(s_entrada_coordinador)
        P(s_entrada)
        pop(en_espera, id_vehiculo)
        V(s_entrada)

        P(sem_asignacion)
        estacion_min = minimo(estacion) //Retorna la posición que tiene menos autos 
        estacion[estacion_min]++ //Incrementa el contador de autos en la estación
        estacion_asignada[id_vehiculo] = estacion_min
        V(sem_asignacion)
        V(aviso_asignacion[id_vehiculo])

    }
}

```

### Monitores 
En un sistema operativo se ejecutan 20 procesos que periodicamente realizan cierto cómputo mediante la función Procesar(). Los resultados de dicha función son persistidos en un archivo, para lo que se requiere de acceso al subsistema de E/S. Sólo un proceso a la vez puede hacer uso del subsistema de E/S, y el acceso al mismo se define por la prioridad del proceso (menor valor indica mayor prioridad)

```c
Monitor Subsistema{

    bool libre = true
    cola C;
    cond vcProcesos[20];

    procedure acceso(id: in int, prioridad: in int){
        if not(libre){
            insertar(C, id, prioridad)
            wait(vcProcesos[id])
        } else libre = false
    }


    procedure salida(){
        if(isEmpty(C)){
            libre = true
        } else {
            sacar(C, id_siguiente)
            signal(vcProcesos[id_siguiente])
        }
    }
}


process Proceso[id: 0..19]{
    int prioridad = ...; 

    while(true){
        resultados = Procesar()
        Subsistema.acceso(id, prioridad)
        persistirResultados(resultados)
        Subsistema.salida()
    }
}

```

---

## 2021 - Recuperatorio
### Semáforos

> Corregir

En un restorán trabajan C cocineros y M mozos. De forma repetida, los cocineros preparan un plato y lo dejan listo en la bandeja de platos terminados, mientras que los mozos toman los platos de esta bandeja para repartirlos entre los comensales. Tanto los cocineros como los mozos trabajan de a un plato por vez. Modele el funcionamiento del restorán considerando que la bandeja de platos listos puede almacenar hasta P platos. No es necesario modelar a los comensales ni que los procesos terminen.

```c
sem bandeja = 1
sem aviso = 0
platos bandeja_terminados[P]; 
int libre = P; 
sem platos = 1

process Cocinero[id: 0..C-1]{
    while (true){
        plato = cocinar()

        P(bandeja)
        P(platos)
        platos_listos++
        push(bandeja_terminados, plato)
        V(platos)
        V(bandeja)
        P(aviso)

    }
}


process Mozo[id: 0..M-1]{
    while(true){
        P(aviso)
        P(bandeja)
        P(platos)
        pop(bandeja_terminados, plato_terminado)
        platos_listos--
        V(platos)
        V(bandeja)

        repartirAlComensal(plato_terminado)
    }
}

```

