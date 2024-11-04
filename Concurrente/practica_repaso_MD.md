# Ejercicios de repaso - Memoria compartida

## Pasaje de mensajes

### Ejercicio 1
En una oficina existen 100 empleados que envían documentos para imprimir en 5 impresoras compartidas. Los pedidos de impresión son procesados por orden de llegada y se asignan a la primera impresora que encuentre libre.

**A. Implemente usando PMA**

```c
chan usarImpresora(int);
chan nroImpresora(nroImpr)

process Empleado[id: 0..99]{
    text documento = generarDocumento()
    send avisoImpresora(id, documento)
    receive aviso[id](nroImpr)    
    receive impresion[id](doc_impreso)
}

process Administrador{
    while (true){
        receive libre(nroImpresora)
        receive usarImpresora(idEmpleado, documento)    
        send impresoraAsignada[idEmpleado](nroImpresora)
        send documento[nroImpresora](idEmpleado, documento)
    }
}

process Impresora[id: 0..4]{
    while(true){
        send libre(id)
        receive documento(idEmpleado, documento)
        impresion = imprimir(documento)
        send impresion[idEmpleado](impresion)
    }
}

```

**B. Resuelva el mismo problema anterior pero ahora usando PMS**
```c

process Empleado[id: 0..99]{
    documento = generarDocumento()
    Buffer!pedido(id, documento)
    Impresora[*]?impresion(doc_impreso)
}

process Buffer{
    cola pedidos;

    do Empleado[*]!pedido(id, documento) -> push(pedidos, (idE, documento))
    - not empty(pedidos); Impresora[*]?libre(idImpr) -> {
        pop(cola, (idE, documento))
        Impresora[idImpr]!enviarPedido(idE, documento)
    }
}

process Impresora[id: 0..4]{
    while(true){
        Buffer!libre(id)
        Buffer?enviarPedido(idEmpleado, documento)
        doc_impreso = generarImpresion(documento);
        Empleado[idEmpleado]!impresion(doc_impreso)
    }
}
```

### Ejercicio 2
Resolver el siguiente problema con PMS. En la estacion de trenes hay una terminal de SUBE que debe ser usada por P personas de acuerdo con el orden de llegada. Cuando la persona accede a la terminal, la usa y luego se retira para dejar al siguiente. 

```c

process Persona[id: 0..P-1]{
    Coordinador!avisoLlegada(id)
    Coordinador?maquinaDisponible()
    usarMáquina("SUBE")
    Coordinador!maquinaLibre()
    retirarse()
}

process Coordinador{
    bool maquina_libre = true;
    Cola cola;

    do Persona[*]?avisoLlegada(id) -> {
        if maquina_libre {
            maquina_libre = false
            Persona[id]!maquinaDisponible()
        } else push(cola, id)
    }
    - not maquina_libre; Persona[*]?maquinaLibre() ->{
        if empty(cola) maquina_libre = true
        else{
            pop(cola, idSiguiente)
            Persona[idSiguiente]!maquinaDisponible()
        }        
    }
}
```

### Ejercicio 3 
Resolver el siguiente problema con PMA. En un negocio de cobros digitales hay P personas que deben pasar por la única caja de cobros para realizar el pago de sus boletas. Las personas son atendidas de acuerdo con el orden de llegada, teniendo prioridad aquellos que deben pagar menos de 5 boletas de los que pagan más. Adicionalmente, las personas embarazadas tienen prioridad sobre los dos casos anteriores. Las personas entregan sus boletas al cajero y el dinero de pago; el cajero les devuelve el vuelto y los recibos de pago. 

```c 
chan fila_embarazada(int) //Fila prioritaria embarazadas
chan fila_menos_boletas(int) //Fila prioritaria menor boletas
chan file_normal(int) //fila normal 
chan[P] turno(bool) 
chan[P] cajero(text[], double)


process Persona[id: 0..P-1]{
    int cantidadBoletas = ...;
    boletas = boletas[cantidadBoletas]
    double dinero = $$
    bool persona_embarazada = ...; 

    if persona_embarazada {
        send fila_embarazada(id)
    } else if cantidadBoletas < 5 {
        send fila_menos_boletas(id)
    } else send fila_normal(id)

    receive turno[id]()
    send cajero[id](boletas, dinero)
    receive respuesta[id](recibos, vuelto)

}

process Coordinador{
    while(true){
        if not empty(fila_embarazada) ->{
            receive fila_embarazada(idP)
        } 
        - not empty(fila_menos_boletas) and empty(fila_embarazada) -> {
            receive fila_menos_boletas(idP)
        }
        - not empty(fila_normal) and empty(fila_embarazada) and empty(fila_menos_boletas) -> {
            receive fila_normal(idP)
        }

        send turno[idP](true);
        receive cajero[idP](boletas, dinero)
        costoTotal = calcularPagoTotal(boletas)
        vuelto = cobrar(costoTotal, dinero)
        recibos = generarRecibos(boletas)

        send respuesta[idP](recibos, vuelto)

    }
        
}

```

# ADA 

## Ejercicio 1
La página web del Banco Central exhibe las diferentes cotizaciones del dólar oficiales de 20 bancos del país, tanto para la compra como para la venta. Existe una tarea programada que se ocupa de actualizar la página en forma periódica y para ello consulta la cotización de cada uno de los 20 bancos. Cada banco dispone de una API cuya única función es procesar las solicitudes de aplicaciones externas. La tarea programada consulta de a una API por vez esperando a lo sumo 5 segundos por su respuesta. Si pasado ese tiempo no respondió, entonces se mostrará vacía la información de ese banco.

```ada
procedure BancoCentral is 
    Task Cotizacion 

    Task type Banco is 
        entry api_dolar(cotizacion_compra: OUT float; cotizacion_venta: OUT float)
    end Banco; 

    arrBancos: array [1..20] of Banco

    task body Cotizacion is 
        int i;
        float val_compra, val_venta;

    begin
        loop
            for i in [1..20] loop
                SELECT 
                    arrBancos[i].api_dolar(val_compra, val_venta);
                    actualizarInformacionCotizacion(i, val_compra, val_venta)
                OR DELAY (5seg)
                    actualizarInformacionCotizacion(i, "", "")

            end loop

            delay(tiempo)
        end loop
    end Cotización;

    task body Banco is 
        float valor_venta, valor_compra;
    begin
        Accept api_dolar(cotizacion_compra: out float; cotizacion_venta: out float) do
            cotizacion_compra = valor_compra;
            cotizacion_venta = valor_venta;
        end api_dolar;
    end Banco;

begin

end BancoCentral;
```

## Ejercicio 2
En un negocio de cobros digitales hay P personas que deben pasar por la única caja de cobros para realizar el pago de sus boletas. Las personas son atendidas de acuerdo con el orden de llegada, teniendo prioridad aquellos que deben pagar menos de 5 boletas de los que pagan más. Adicionalmente, las personas ancianas tienen prioridad sobre los dos casos anteriores. Las personas Entregan sus boletas al cajero y el dinero de pago; el cajero les devuelve el vuelto y los recibos de pago. 

```ada
procedure Negocio is 
    Task Caja is 
        entry prioridad_ancianos(boletas: in text; dinero: in double; vuelto: out double)
        entry prioridad_menos_boletas(boletas: in texto; dinero: in double; vuelto out double)
        entry caja_normal(boletas: in texti; dinero: in double; vuelto: out double)
    end Caja;   

    Task type Persona; 

    arrPersonas: array [1..P] of Persona; 

    TASK BODY Caja is 

    begin
        SELECT 
            Accept prioridad_ancianos(boletas: in text; dinero: in double; vuelto: out double) do
                //Toda la lógica de cobro 
            end prioridad_ancianos;
        OR 
            WHEN (prioridad_ancianos'count = 0 and prioridad_menos_boletas'count = 0) => ACCEPT caja_normal(...) do
                //Toda la lógica de cobro 
                end caja_normal;
        OR 
            WHEN (prioridad_ancianos'count = 0) => ACCEPT prioridad_menos_boletas(...)
                //Toda la lógica de cobro
            end prioridad_menos_boletas; 
        END SELECT;  
    end CAJA; 

    
    TASK BODY Persona IS 

    begin
        anciano = ... // (true | false)
        cantidad_boletas = ... 

        if (anciano) {
            Caja.prioridad_ancianos(boletas, dinero, vuelto)
        } else if (cantidad_boletas < 5) {
            Caja.prioridad_menos_boletas(boletas,dinero, vuelto)
        } else Caja.caja_normal(boletas, dinero, vuelto)
    end Persona;

begin

end Negocio;

```

## Ejercicio 3
La oficina central de una empresa de venta de indumentaria debe calcular cuántas veces fue vendido cada uno de los artículos de su catálogo. La empresa se compone de 100 sucursales y cada una de ellas maneja su propia base de datos de ventas. La oficina central cuenta con una herramienta que funciona de la siguiente manera: ante la consulta realizada para un artículo determinado, la herramienta envía el identificador del artículo a las sucursales, para que cada una calcule cuantas veces fue vendido en ella. Al final del procesamiento, la herramienta debe conocer cuántas veces fue vendido en total, considerando todas las sucursales Cuando ha terminado de procesar un artículo comienza con el siguiente (suponga que la herramienta tiene una función generarArtículo() que retorna el siguiente ID a consultar)

```ADA 
PROCEDURE Empresa is 
    Task Central is 
    end Central; 

    Task type Sucursal is 
        entry nroSucursal(identidad: IN int)
    end Sucursal;

    arrSucursales: array of [1..100] of Sucursal;

    Task body Central is 
        int idArt;
        int total, parcial;
    begin
        LOOP
            totalProducto = 0;
            idProducto = generarArticulo();
            FOR i IN (1..200) LOOP
                SELECT
                    ACCEPT listoParaProcesar (id: OUT int) DO -- Recibe el aviso desde las sucursales y le pasamos el id del articulo
                        id = idProducto;
                    END listoParaProcesar;
                OR
                    ACCEPT recibirCantidadVendida(cantidadVendida: INT int) DO --Puede que hayan terminados unos el cómputo así que mientras tanto va calculando el total 
                        totalProducto = totalProducto + cantidadVendida;
                    END recibirCantidadVendida;
                END SELECT;
            END LOOP;
            FOR i IN (1..100) LOOP --Para indicar que ya todos los procesos enviaron su reporte 
                ACCEPT terminoCiclo;
            END LOOP;
            PUTS ("El artículo " + idArticulo + " fue vendido: " + totalProducto + " veces.");
        END LOOP 
    end Central; 


    Task body Sucursal is 
        int id, articulo, cantVendidas;

    begin
        LOOP
            Central.listoParaProcesar (idArticulo);
            Central.recibirCantidadVendida (db.ObtenerVentas(idArticulo));
            Central.terminoCiclo; 
        END LOOP

    end Sucursal

begin
null;
end Empresa; 
```