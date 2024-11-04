# Práctica 5 - Rendezvous

## Explicación práctica

> Es un único programa con un cuerpo principal.
> Se desarrollan TASK dentro de ese programa que pueden ejecutar independientemente y contienen primitivas de sincronización/comunicación. 
> TASK TYPE -> varias instancias iguales de ese tipo. 
> Cada TASK tiene operaciones exportadas -entrys- y la implementación de las mismas. 
> La comunicación entre dos tareas se realiza por medio de un **Entry Call** y un **Accept**. Ambos procesos se mantienen sincronizados durante el rendezvous. 
> Los entry call a un entry se almacenan en la cola implícita del mismo de acuerdo al orden de llegada.
> Comunicación bidireccional.
> Entry Call simple
> Entry Call Condicional (select - else): no espera que le acepten el pedido, si la otra tarea no está lista para realizar el accept a su pedido inmediatamente, entonces lo cancela y realiza otra cosa.
> Entry Call Temporal (select - or delay): espera a lo sumo un tiempo a que la otra tarea realice el accept a su pedido, pasado el tiempo cancela el entrycall y realiza otra cosa. 

### Ejercicio 1 
Se debe modelar la atención en un banco por medio de un único empleado. Los clientes llegan y son atendidos de acuerdo al orden de llegada.
Task -> empleado.
Task type -> clientes. 

```go 
procedure Banco1 is 
    //Declaración de los datos
    Task Empleado is 
        //Declaración de las entrycalls
        Entry Pedido(Datos: IN texto);
    End empleado;

    Task Type Cliente is
        Entry Respuesta(Res: IN texto)
    End cliente;

    arr Clientes: array(1..N) of Cliente;

    //--Acá definimos los subprogramadas
    Task Body cliente is
        Resultado: texto;
    Begin
        Empleado.pedido("Datos", Resultado) //Llamado al entry Empleado y en la variable Resultado el empleado nos retorna la respuesta
    End cliente;

    Task Body Empleado is
        R, D: texto;
    Begin
        loop
            Accept Pedido(Datos: IN texto, Res: OUT text) do
                Res = procesarPedido(Datos)
            end Pedido;
        end loop;
    end empleado;

    begin //Este es el programa principal
        null;
    end Banco1;
```

### Ejercicio 2
Se debe modelar la atención en un banco por medio de un único empleado. Los clientes llegan y esperan a lo sumo 10 minutos a ser atendidos de acuerdo al orden de llegada. Pasado ese tiempo se retira sin ser atendido. 

```go
procedure Banco2 is
    Task Empleado is 
        Entry Pedido(Datos: IN texto, Resultado: OUT texto)
    end Empleado;

    Task Type Cliente is 
    end Cliente;

    arrClientes: array (1..N) of Cliente

    Task Body Empleado is
        Data: texto
    begin
        loop
            ACCEPT Pedido (Data: IN texto, Resultado: OUT texto) do
                Resultado = analizarPedido(Data);
            end Pedido;
        end loop;
    end Empleado;


    Task Body Cliente is 
        Respuesta = texto
    Begin
        SELECT 
            Empleado.pedido("Datoss", Respuesta);
        OR DELAY 600.0 //Si no tenemos espera del empleado en 10 minutos entonces no hace nada
            Null;
        END SELECT;

    end Cliente;

begin

end Banco2;
```

### Ejercicio 3 
Se debe modelar la atención en un banco por medio de un único empleado. Los clientes llegan y esperan a ser atendidos; pueden ser Regulares o Prioritarios. El empleado los atiende de acuerdo al orden de llegada pero dando prioridad a los clientes Prioritarios.

```go
procedure Banco3 is 
    Task Empleado is 
        Entry pedidoRegular(Datos: IN texto; Resultado: OUT texto);
        Entry pedidoPrioritario(Datos: IN texto; Resultado: OUT texto)
    end Empleado; 

    Task type Cliente;

    arrCliente = array (1..N) of Cliente

    Task body Cliente is 
        bool prioritario;
        text respuesta
    begin
        prioritario = ...
        if prioritario then Empleado.pedidoPrioritario("data", respuesta)
        else Empleado.pedidoRegular("data", respuesta)
    end Cliente;

    Task body Empleado is 
    begin
        loop
            SELECT 
                accept pedidoPrioritario(Datos: IN Texto; Resultado: OUT texto) do 
                    Resultado = analizarPedido(Datos)
                end pedidoPrioritario
            OR 
                when (pedidoPrioritario´count = 0) -> { //Revisa que en la fila de los pedidos prioritarios esté vacía para recién atender a los regulares
                    accept pedidoRegular(Datos: IN texto; Resultado: OUT texto) do
                        Resultado = analizarPedido(Datos)
                    end pedidoRegular;
                }
            end Select;
        end loop;
    end Empleado;

begin
    null
end Banco3;
```

### Ejercicio 4 
Se debe modelar la atención en un banco por medio de DOS empleados. Los clientes llegan y son atendidos de acuerdo al orden de llegada por cualquiera de los dos. 

```go
procedure Banco4 is 
    task type Empleado is 
        entry pedido(Datos: in texto; resultado: out texto)
    end empleado;

    task Administrador is 
        entry recepcion(idEmpleado: out int)
    end Administrador; 

    task type Cliente is 
        entry respuesta(resp: in texto); 
        entry identidad(identificador: in int)
    end Cliente;

    arrClientes: array (1..N) of Cliente
    EmpleadoA: Empleado
    EmpleadoB: Empleado

    Task body Cliente is 
        int id;
        int idEmpleado;
        text resultado; 
    begin
        Accept identidad(identificador: in int) do 
            id = identificador
        end identidad; 

        Administrador.recepcion("Data", id)

        accept respuesta (resp: in texto) do
            resultado = resp
        end respuesta;

    endCliente;

    Task body Administrador is 
        int idCl;
        texto data;
    begin
        
        loop
            Accept recepcion(datos: in text; idCliente: in int) do
                data = datos;
                idCl = idCliente; 
                accept empleadoLibre(idCliente: out int, datos: out text) do
                    idCliente = idCl;
                    datos = data;
                end empleadoLibre;
            end recepcion;
        end loop;
    end;

    task body Empleado is 
        int idCl;
        texto data;
        texto resp;
    begin
        loop
            Administrador.empleadoLibre(idCl, data)

            resp = analizarDatos(data);

            arrClientes[idCl].respuesta(resp)

        end loop;
    end;

begin
    for i in 1..N loop
        arrClientes(i).ident(i)
    end loop;
end banco4;

```

### Ejercicio 5
Se debe modelar un buscador para contar la cantidad de veces que aparece un número dentro de un vector distribuido entre las N tareas contador. Además, existe un administrador que decide el número se desea buscar y se lo envía a los N contadores para que lo busquen en la parte del vector que poseen y calcula la cantidad total. 

```go
procedure ContadorOcurrencias is 
    Task Admin is 
        entry respuesta(parcial: in int)
    end Admin;

    task type Contador is
        entry buscar(num: in int;)
        entry identidad(identificador: in int)
    end Contador;

    arrContador: array [1..N] of Contador


    Task body admin is 
        int numero, i;
        int res, resultado;
        int total;
    begin 
        numero = ...; 
        for i in [1..N] loop
            arrContador[i].buscar(numero)
        end loop;

        total=0;
        for i in [1..N] loop
            Accept respuesta(res: in int) do
                total = total + res;
            end respuesta;
        end loop


    end admin;


    task body Contador is 
        int id, numero, ocurrencias;
        int[] vector;
    begin
        Accept identidad(identificador: in int) do
            id = identificador
        end identidad; 

        Accept buscar(num: in int) do
            numero = num;
        end buscar

        ocurrencias = buscar(vector, numero)

        Admin.respuesta(ocurrencias)
    end Contador

begin
    for i in [1..N] loop
        arrContador[i].identidad(i)
    end loop;

end ContadorOcurrencias;
```

---

## Ejercicio 1 
Se requiere modelar un puente de un único sentido que sorporta hasta 5 unidades de peso. El peso de los vehículos depende del tipo: cada auto pesa 1 unidad, cada camioneta pesa 2 unidades y cada camión 3 unidades. Suponga que hay una cantidad innumerable de vehículos (A autos, B camionetas y C camiones). 
A. Relice la solución suponiendo que todos los vehículos tienen la misma prioridad. 

```ada
procedure Puente is 
    Task Administrador is
        entry llegada(tipo: in string)
    end Administrador;

    Task type Vehiculo is 

    end Vehiculo;

    arrVehiculos: array [1..A+B+C] of Vehiculo

    Task body vehiculo is 
        String tipo; 
        int peso, id;
    begin
        tipo = "auto" | "camioneta" | "camion"
        
        Accept identidad(identidad: in int) do 
            id = identidad;
        end identidad; 

        Administrador.llegada(id, peso)
        
        Accept aviso_pase() do 
            cruzarPuente()
        end aviso_pase; 

    end vehiculo;

    Task body Administrador is 
        int peso_actual, id_actual, p;
        
    begin
        peso_actual = 0 
        
        Accept llegada(id: in int, peso: in int) do 
            id_actual = id
            p= peso
        end llegada; 

        if peso_actual <= 5 {
            arrVehiculos[id_actual].aviso_pase();
        } else 
            push 

    end Administrador;

begin
    for i in [1 ... A+B+C] loop
        arrVehiculos[i].identidad(i)
    end loop;
end Puente;
```

---

## Parcial 2022

### Ejercicio 1 - PMA 
Se debe modelar el funcionamiento de una casa de venta de repuestos automotores, en la que trabajan V vendedores y que debe atender a C clientes. El modelado debe considerar que 
- Cada cliente realiza un pedido y luego espera a que se lo entreguen,
- Los pedidos que hacen los clientes son tomados por cualquiera de los vendedores. 
- Cuando no hay pedidos para atender, los vendedores aprovechan para controlar el stock de los repuestos (tardan entre 2 y 4 minutos para hacer esto).

```C
chan pedido(int, string);
chan[0..C-1] entregaPedido(Repuesto)

process Cliente [id: 0..C-1]{
    string nombreRepuesto = "..."
    send pedido(id, nombreRepuesto)
    receive entregaPedido[id](repuesto)
}

process Administrador{
    bool libre; 
    int idV;
    string nombreRepuesto;

    while (true){
        receive vendedorLibre(idV)
        
        if (empty(pedido)) {
            send armarPedido[idV](false)
        } else {
            send armarPedido[idV](true)
            receive pedido(idCliente, nombreRepuesto);
            send generarPedido[idV](idCliente, nombreRepuesto)
        }

        
    }
}

process Vendedor [id: 0..V-1]{
    Repuesto rep;
    int idCliente;
    bool hay_pedido;

    while (true){
        send vendedorLibre(id)
        receive armarPedido(hay_pedido)
        if not hay_pedido{
            delay controlarStock();
        } else {
            receive generarPedido[idV](idCliente, nombreRepuesto)
            rep = obtenerRepuesto(nombreRepuesto)
            send entregaPedido[idCliente](rep)
        }
    }
}

```

### Ejercicio 2 - ADA 
Se quiere modelar el funcionamiento de un banco, al cual llegna clientes que deben realizar un pago y llevarse su comprobante. Los clientes se dividen entre los regulares y los premium, habiendo R clientes regulares y P clietnes premium. Existe un único empleado en el banco, el cual atiende de acuerdo al orden de llegada, pero dando prioridad a los premium por sobre los regulares. Si a los 30 minutos de llegar un cliente regular no fue atendido, entonces se retira sin realizar el pago. Los clientes premium siempre esperan hasta ser atendidos. 

```ada 
procedure Banco is 
    Task Cajero is 
        entry atencionRegulares(pago: in float; comprobante: out string)
        entry atencionPremiums(pago: in float; comprobante: out string)
    end Cajero;

    Task type ClientePremium is

    end ClientePremium;

    Task type ClienteRegular is 

    end ClienteRegular; 

    arrPremiums: array [1..P] of ClientePremium;
    arrRegulares: array [1..R] of ClienteRegular; 


    Task body Cajero is 
        float pago_rec; 
        string comprob;
    begin

        loop 
            SELECT 
                Accept atencionPremium(dinero: in double; comprobante: out string) do
                    pago_rec = dinero;
                    comprobante = generarPago(pago_rec)
                end atencionPremium;
            ELSE 
                when (atencionPremium'count = 0) -> Accept atencionRegular(dinero: in double; comprobante: out string) do
                    pago_rec = dinero;
                    comprobante = generarPago(pago_rec)
                end atencionRegular;
        end loop;
    end Cajero;  


    Task body ClientePremium is 
        float dinero;
        string comprobante;
    begin
        dinero = ...
        Cajero.atencionPremium(dinero, comprobante)

    end ClientePremium;

    
    Task body ClienteRegular is 
        float dinero;
        string comprobante;
    begin
        dinero = ...
        SELECT 
            Cajero.atencionRegular(dinero, comprobante)
        OR DELAY (60 seg * 30 )
            null;

    end ClienteRegular;

begin

end Banco;

```

---

## Parcial 2023

### Ejercicio 1 - PMA
En una oficina existen 100 empleados que envían documentos para imprimir en 5 impresoras compartidas. Los pedidos de impresión son procesados por orden de llegada y se asignan a la primera impresora que se encuentre libre. 

```c

process Empleado[id: 0..99]{
    Impresion impr;
    string documento = generarDocumento()
    send pedidoImpresion(id, documento)
    receive docImpreso[id](impr)
}


process Administrador{  
    int idImpr, idE;
    string documento; 
    while(true){
        receive avisoDisponible(idImpr)
        
        receive pedidoImpresion(idE, documento)
        send imprimir[idImpr](idE, documento)
    }

}


process Impresora[id: 0..4]{
    Impresion impr
    while(true){
        send avisoDisponible(id)
        receive imprimir[id](idE, documento)
        impr = generarImpresion(documento)
        send docImpreso[idE](impr)
    }
}

```

### Ejercicio 2 - PMS 
El mismo problema anterior. 

```c

process Empleado[id: 0..99]{
    string documento = generarDoc();
    Impresion impresion;
    Administrador!imprimir(id, documento)
    Administrador?impresoraAsignada(numImpresora)
    Impresora[numImpresora]?resultado(impresion)
}

process Administrador{
    int idE, idImpr;
    string doc;
    Cola cola;
    while (true){

        if Empleado[*]?imprimir(idE, doc) -> push(cola, (idE, doc))
        [] not empty(cola); Impresora[*]?libre(idImpr) -> {
            Empleado[idE]!impresoraAsignada(idImpr)
            Impresora[idImpr]!asignarImpresion(pop(cola,(idE, doc)))
        }

    }
}

process Impresora[id: 0..4]{
    Impresion impr;
    int idE;
    while (true){
        Administrador!libre(id)
        Administrador?asignarImpresion(idE, doc)
        impr = imprimir(doc)
        Empleado[idE]!resultado(impr)
    }

}

```