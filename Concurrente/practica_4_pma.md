# Pasaje de mensajes asincrónicas 
- Los canales son compartidos por todos los procesos.
- Cada canal es una cola de mensajes; por lo tanto, el primer mensaje encolado es el primero en ser atendido. 
- El `send` no bloquea al emisor. 
- `empty` como sentencia para saber si hay algún mensaje en el canal. No indica cantidad. 

---

## Explicación práctica 
### Ejercicio 2
En una empresa de software hay N personas que prueban un nuevo producto para encontrar errores, cuando encuentran uno generan un reporte para que un empleado corrija el error y esperan la respuesta del mismo. El empleado toma los reportes de acuerdo al orden de llegada, los evalúan, hace las correcciones necesarias y le responde a la persona que hizo el reporte.

```c
chan reportes(texto);
chan[0..N-1] respuestas(texto); 

process Persona [id: 0..N-1]{
    while (true){
        R = generarReporte()
        send reportes(R, id)
        receive respuestas[id](res)
    }
}


process Empleado{
    texto Resp, Rept
    while (true){
        receive reportes(Rept, id)
        Resp = generarRespuesta(Rept)
        send respuestas[id](Resp)
    }
}
```


### Ejercicio 3
En una empresa de software hay N personas que prueban un nuevo producto para encontrar errores, cuando encuentran uno generar un reporte para que alguno de los 3 empleados corrija el error y esperan la respuesta del mismo. Los empleados toman los reportes de acuerdo al orden de llegada, los evalúan, hacen las correcciones necesarias y le reponden a la persona que hizo el reporte. 

```c
chan reportes(texto);
chan respuesta(texto)[0..N-1]

process Persona[id: 0..N-1]{
    texto Res, Rep; 
    while (true){
        Rep = generarReporte();
        send reportes(id, Rep);
        receive respuesta[id](res)
    }
}


process Empleado[id: 0..2]{
    texto Res, Rep;
    int idP;
    while(true){
        receive reportes(idP, Rep);
        Res = generarSolución(Rep);
        send respuesta[id](res)
    }
}
```

### Ejercicio 4 
En una empresa de software hay N personas que prueban un nuevo producto para encontrar errores, cuando enceuntran uno generan un reporte para que un empleado corrija el error (las personas no deben recibir ninguna respuesta). El empleado toma los reportes de acuerdo al orden de llegada, los evalúan y hace las correcciones necesarias; cuando no hay reportes para atender el empleado se dedica a leer durante 10 minutos. 

```c

chan reportes(texto);
chan respuestas(texto)[0..N.1]

process Persona(id: 0..N-1){
    texto rep;
    while (true){
        rep = generarReporte();
        send reportes(rep);
    }
}

process Empleado{
    texto res;
    while (true){
        if(empty(reportes)){
            delay(10minutos)
        } else{
            receive reportes(rep);
            resolverReporte(rep)
        }
    }
}

```

### Ejercicio 5

En una empresa de software hay N personas que prueban un nuevo producto para encontrar errores, cuando encuentran uno generan un reporte para que uno de los 3 empleados corrija el error (las personas no deben recibir ninguna respuesta). Los empleados toman los reportes de acuerdo al orden de llegada, los evalúan y hacen las correcciones necesarias; cuando no hay reportes para atender los empleados se dedican a leer durante 10 minutos. 

```c
chan reportes(texto);
chan Pedido(int)


process Persona[id: 0..N-1]{
    texto rep;
    while (true){
        rep = generarReporte()
        send reportes(rep)
    }
}

process Coordinador{

}

process Empleado[id: 0..2]{
    text rep;
    while (true){

    }
}

```


---

## Ejercicio 1
