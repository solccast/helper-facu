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
chan canalEmpleado[0..2](texto)
chan pedido(int)

process Persona[id: 0..N-1]{
    texto rep;
    while (true){
        rep = generarReporte()
        send reportes(rep)
    }
}

process Coordinador{
    texto rep;
    int empleado;
    while (true){
        receive Pedido(empleado) //Recibe el aviso del empleado
        if empty(reportes){ //Si la lista de reportes está vacía entonces envía al empleado que no tiene tareas para realizar, caso contrario, le envía la tarea
            send canalEmpleado[empleado]('vacio')
        } else {
            receive reportes(rep)
            send canalEmpleado[empleado](rep)
        }
    }
}

process Empleado[id: 0..2]{
    text rep, res, texto;
    while (true){
        send Pedido(id) // Avisa que está disponible 
        receive canalEmpleado[id](texto) // Sí o sí recibe una respuesta 
        if (texto <> "vacio"){ //Consulta si es que el coordinador envió tarea 
            res = generarSolución(texto)           
        } else{
            delay(600)
        } 
    }
}

```


---
## Práctica

## Ejercicio 2 
Se desea modelar el funcionamiento de un banco en el cual existen 5 cajas para realizar pagos. Existen P clientes que desean hacer un pago. Para esto, cada una selecciona la caja donde hay menos personas esperando; una vez seleccionada, espera a ser atendido. En cada caja, los clientes son atendidos por orden de llegada por los cajeros. Luego del pago, se les entrega un comprobante. 

```c
chan filaCaja[0..4](int) //Fila de los procesos 
chan siguiente[0..P-1](int) //El cajero avisa al proceso que puede pasar
chan llegadaCliente(int) //El cliente informa al Coordinador que llegó
chan caja[0..4](texto) //Canal privado del cajero y el cliente
chan aviso[0..4]

process Cliente[id: 0..P-1]{
    texto pago, comprobante;
    send llegadaCliente(id) //Avisa al coordinador que llegó 
    //Esperar en la fila 
    receive siguiente[idCliente](idCaja) //Espera que lo llame el cajero

    //Proceso de pago y recibo
    send caja[idCaja](pago)
    receive caja[idCaja](comprobante) 
    retirarse()
}

process Coordinador{
    int idCliente
    int[0..4] cantidadFila = ([5] 0) // Inicio vector contador
    while(true){
        receive llegadaCliente(idCliente)
        filaOptima = minimo(cantidadFila) //Trae la posición del vector con menos cantidad de gente

        send filaCaja[filaOptima](idCliente)
        cantidadFila[filaOptima]++ //Incrementa vector contador

        if (!empty(liberar)){ //Pregunta si el canal liberar tiene algún mensaje
            receive liberar(idAux)
            cantidadFila[idAux]--
        }
    }
}

process Caja[id: 0..4]{
    int idCliente;
    texto comprobante, pagoCliente
    while(true){

        receive filaCaja[id](idCliente) 
        send siguiente[idCliente](id) //Envía el id de la caja a la que debe acercarse 

        receive caja[id](pagoCliente)
        generarPago(pagoCliente)
        comprobante = generarComprobante()

        send caja[idCliente](comprobante)
        send liberar(id) //Se libera 
    }
}

```

## Ejercicio 3
Se debe modelar el funcionamiento de una casa de comida rápida, en la cual trabajan 2 cocineros y 3 vendedores y que debe atender a C clientes. 
Se debe considerar: 
- Cada cliente realiza un pedido y luego espera a que se lo entreguen.
- Los pedidos que hacen los clientes son tomados por cualquiera de los vendedores y se lo pasan a los cocineros para que realicen el plato. Cuando no hay pedidos para atender, los vendedores aprovechan para reponer un pack de bebidas de la heladera. 
- Cada cocinero toma un pedido peniente dejado por los vendedores, lo cocina y se lo entrega directamente al cliente correspondiente.

```c
chan pedidosClientes(int,texto)
chan entregaCliente[C](comida)


process Cliente[id: 0..C-1]{
    //Realizar pedido
    text pedido = generarPedido()
    send pedidosClientes(id, pedido)
    receive entregaCliente[id](comida)
    retirarse()
}


process Vendedor[id: 0..2]{
    //Toma un pedido del canal pedidos.
    //Si es que no hay pedidos toma unos minutos.
    int idCliente;
    text pedidoCliente;
    while(true){

        receive pedido(idCliente, pedidoCliente)
    }
}


process Cocinero[id: 0..1]{
    //Toma un pedido dejado por los vendedores
    //Cocina y se lo entrega al cliente
}

```


## Ejercicio 4
Simular la atención en un locutorio con 10 cabinas telefónicas, el cual tiene un empleado que se encarga de atender a N clientes. Al llegar, cada cliente espera hasta que el empleado le indique a qué cabina ir, la usa y luego se dirige al empleado para pagarle. El empleado atiende a los clientes en el orden en que hacen los pedidos, pero siempre dando prioridad a los que terminaron de usar la cabina. A cada cliente se le entrega un ticket factura. Suponga que hay una función Cobrar() llamada por el empleado que simula que el empleado le cobra al cliente. 

```c
chan pedido(int)
chan asignaciónCabina[N](int)
chan prioridadFin(int)
chan factura[N](texto)

process Cliente [id: 0..N-1]{
    send pedido(id) //Avisa al empleado que tiene un pedido

    receive asignacionCabina[id](nroCabina); //Espera a que el empleado le asigne una cabina
    usarCabina(nroCabina)

    send prioridadFin(id, pago)
    receive factura[id](factura)
    salir()

}


process Empleado{
    int cabina[10] = ([10] false)

    while (true){
        //No determinístico
        if not empty(prioridadFin){
            cabina[10] = False
            receive prioridadFin(idCliente, pago)
            cobrar(pago)
            factura = generarFactura(pago);
            send factura[idCliente](factura)
        } else empty(prioridadFin) and not empty(pedido){
            //Acá atiende a los pedidos de quienes llegan 
            receive pedido(id) //El empleado recibe el pedido del cliente 
            
            //Armar una cola circular
            cabinaLibre = i mod 10 ...
            cabina[cabinaLibre] = True
            send asignacionCabina[id](cabinaLibre)
        }
    }
}

```
