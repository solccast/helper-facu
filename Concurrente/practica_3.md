## Monitores 

- No existen las variables compartidas: Monitores por un lado, procesos para el otro. 
- Las variables permanentes del monitor sólo pueden usar dentro del monitor
- Si el monitor tiene un código de inicialización, hasta que no termine su ejecución no atiende llamados a los procedures.
- Los procesos interactúan entre ellos y con los recursos compartidos por medio de los monitores haciendo llamados a los procedures de estos
- Cuando el monitor está  libre TODOS los procesos que están haciendo llamados a sus procedimientos compiten por acceder al monitor. 
- Los monitores son pasivos e independientes.
- Se pueden realizar llamados entre monitores.
- La exclusión mutua es implícita
- Sincronización por condición -> usa variables _Conditions_: variables permanentes del monitor.
    - _wait (vc)_: duerme al proceso en la cola asociada a la variable condicion. Duerme al proceso que hizo el llamado. 
    - _signal(vc)_: despierta al primer proceso dormido en _vc_ para que **compita** nuevamente para acceder al monitor y continuar con la instrucción siguiente.  
    - __signal_all()__: despierta a todos los procesos (para realizar algoritmos del tipo barrera) para que todos pasen a competir por acceder nuevamente al monitor. 

---

## Explicación práctica - Ejercicio 1 
Existen N personas que desean utilizar un cajero automático. En este primer caso no se debe tener en cuenta el orden de llegada de las personas (cuando está libre cualquiera lo puede usar). Suponga que hay una función UsarCajero() que simula el uso del cajero. 

> Notar que solo se utiliza exclusión mutua, entonces en este caso el monitor actúa como el recurso compartido (cajero automático)


```c

Monitor Cajero
{
    procedure pasarAlCajero()
    {
        UsarCajero() //Hasta que esta función no termine de ejecutarse no libera el monitor
    }
}


process Persona[id: 0..N-1]
{
    Cajero.PasarAlCajero() //Como compiten por el monitor se quedan esperando hasta que quede libre. Cualquiera puede pasar. 
}

```

Existen N personas que desean utilizar un cajero automático.. En este segundo caso se debe tener en cuenta el orden de llegada de las personas. Suponga que hay una función UsarCajero() que simula el uso del cajero. 

> En este caso ya no nos sirve que el monitor sea el recurso compartido, sino que sea el administrador del recurso compartido porque mientras una persona ocupa el cajero el monitor debe permitir que se encolen el resto de forma ordenada


```c

Monitor AdministradorCajero
{
    bool libre = true; //Esta es la variable que nos ayudará con el estado del recurso compartido. Si está libre no hace fila y usa el recurso directamente, caso contrario deberá encolarse 
    cond cola; //Esto me dará el orden de llegada
    int cantidad_esperando = 0

    procedure PasarAlCajero()
    {
        if (not libre) //El cajero está ocupado
        {
            cantidad_esperando:= cantidad_esperando + 1   
            wait(cola) //Duerme al proceso si el cajero no está libre
        } else //El cajero está libre
        {
            libre = false //Esto cambia cuando despertó el proceso y ocupa el cajero
        }
    }


    procedure SalirDelCajero()
    {
        if (cantidad_esperando == 0) -> libre = true //Esto es lo que servirá para respetar la exclusión mutua 
        else {
            cantidad_esperando:= cantidad_esperando - 1
            signal(cola) //Como despierta al primer dormido respeta el orden, no es necesario usar otro tipo de estructura
        }
    }
}


process Persona[id: 0..N-1]
{
    Cajero.PasarAlCajero()
    UsarCajero()
    Cajero.SalirDelCajero()
}

```

¿Qué ocurre si llega una persona anciana que tiene prioridad?
> Será necesario una nueva estructura que permita mantener el orden (cola ordenada) para mantener el id e información de la edad

```c

Monitor administradorCajero
{ 
    bool libre = true
    int esperando = 0
    colaOrdenada fila;
    cond espera[N]; //Vector de procesos dormidos

    Procedure Pasar(id, edad: int)
    {
        if(not libre)
        {
            insertar(fila, id, edad) //Se insertaen una estrctura que se encarga de eso 
            esperando++
            wait(espera[id]) //Duerme al proceso que llegó 
        }
    }

    
    Procedure Salir()
    {
        if (esperando > 0) //Si hay alguno esperando
        {
            esperando--
            sacar(fila, id_siguiente) //La estructura me retorna el id del proceso que sigue
            signal(espera[id_siguiente]) //Despierta al proceso que estaba dormido, al proceso específico
        } else 
        {
            libre = true //Da pase libre en caso de que no haya nadie esperando
        }
    }


}


Process Persona[id: 0..N-1]
{
    int edad = ...; 

    Cajero.pasar(id, edad)
    UsarCajero()
    Cajero.salir()
}

```

## Explicación práctica - ejercicio 4
En un banco hay 3 empleados y hay N clientes que deben ser atendidos por uno de ellos (cualquiera) de acuerdo al orden de llegada. Cuando uno de los empleados lo atiende el cliente le entrega los papeles y espera el resultado. 

> Será necesario tener un monitor por cada uno de los empleados 

```c

Monitor Banco
{
    int esperando = 0
    cond esperaClientes;
    cola empleados_libres;
    int cant_empl_libres = 0 //Para los empleados que no están haciendo nada, que no están analizando al siguiente y que no están atendiendo a nadie

    procedure Llegada(id_empl: int)
    {
        if (cantLibres==0) //Si no hay ningun empleado libre
        { //El cliente se duerme
            esperando++
            wait(esperaClientes) //El proceso se queda dormido 
        } else cantLibres-- //Si hay un empleado libre lo resto
        
        pop(empleados_libres, id_empl) //Ejecuta si el proceso despertó o la condición era falsa
    }

    procedure Siguiente(idE: int)
    {
        push(empleados_libres, idE)
        if(esperando>0)
        {
            esperando--
            signal(esperaC)
        } else cantLibres++
    }
}


Monitor Escritorio[id: 0..2]n //Un monitor por cada empleado
{
    cond vcEmpleado;
    cond vcCliente;
    text Datos;
    test resultados;
    bool listo = false

    //La variable booleana 
    procedure atencion(documentacion: in text , resultado: out text)
    {
        datos = documentacion //Setea la variable datos con la doucmentacion
        listo = true //Avisa que la data está lista
        signal(vcEmpleado);  //Despierta al empleado de que ya están los datos
        wait(vcCliente); //El cliente se queda esperando el analisis
        resultado = resultados
        signal(vcEmpleado) //Le avisa al empleado que ya tomó el dato
    }  

    procedure esperarDatos(D: out text)
    {
        if(listo = false){ //En caso de que todavía no haya llegado el cliente se queda esperando
            wait(vcEmpleado)//Se queda esperando los datos ya que el cliente lo despierta
        } //Si llegó el cliente toma el dato
        D = Datos   //Recupera la información y la pasa por parámetro
    }

    procedure entregarResultado(result: in text)
    {
        resultados = result
        signal(vcCliente) //Avisa al cliente que ya están los datos listos
        wait(vcEmpleado) //El empleado se queda esperando que el cliente confirme que ya tomó el dato
        listo = false //Para la próxima iteración
    }

}


process Empleado[id: 0..2]
{
    while(true)
    {
        Banco.siguiente(id) //Envio el id del empleado que quedó libre
        Escritorio.esperarDatos(datos)
        result = analizarDatos(datos)
        Escritorio[id].entregarResultado(result)
    }
}


process Cliente[id: 0..N-1]
{   int idEmpleado
    Banco.llegada(idEmpleado)
    Escritorio[idEmpleado].atencion(documentacion, resultado)
}

```

## Explicación práctica - ejercicio 5
Se debe simular un partido de fútbol 11. Cuando los 22 jugadores llegaron a la cancha juegan durante 90 minutos y luego todos se retiran

> Es necesario implementar una barrera hasta que lleguen todos los jugadores y luego todos juntos al mismo timepo deben jugar durante 90 minutos. 

```c

Monitor Cancha
{
    int cant = 0
    cond espera, inicio

    procedure llegada()
    {
        cant++
        if (cant == 22)
        {
            signal(inicio) //Si soy el último jugador en llegar aviso y despierto al proceso partido 
        } 
        wait(espera) //Si faltan jugadores se espera 
    }


    procedure iniciar() //Acá trabajan todos juntos, por eso lo tienen que hacer en un solo lugar
    {
        if(cant < 22 ){
            wait(inicio) //El proceso partido se queda dormido
        } 
    }

    procedure terminar()
    {
        signal_all(espera) //Despierta a los jugadores que estaban esperando para irse 
    }

}


process Jugador[id: 0..21]
{
    Cancha.llegada()

}

process Partido
{
    Cancha.iniciar()
    delay(90minutos)
    Cancha.terminar()
}

```

---

# Práctica 3 - Monitores

## Ejercicio 4
Existen N vehículos que deben pasar por un puente de acuerdo con el orden de llegada.  Considere que el puente no soporta más de 50000kg y que cada vehículo cuenta con su propio peso (ningún vehículo supera el peso soportado por el puente).  

```c

Monitor Puente
{
    int peso_actual = 0
    cond vehiculos; 
    bool libre = true
    cond disponiblePasar 
    int vehi_esperando = 0
    cond peso_disp

    procedure llegada(p: in int)
    {
        if not(libre) //El puente se encuentra ocupado
        {
            vehi_esperando++ //Se queda a esperear
            wait(vehiculos)
        }
        //Una vez que despiertan al proceso para pasar evalúan su peso
        while (peso_actual + p >= 50000)
        {
            wait(peso_disp) //Si supera el peso debe esperar
        } 

        libre=false // Una vez que se cumple condición, se ocupa el puente
        peso_actual+=p 
        
        if(vehi_esperando > 0) //Si hay alguien esperando avisa al proceso q este dormido
        {
            vehi_esperando--
            signal(vehiculos)
        } else 
        {
            libre=true //Si no hay nadie esperando da el pase libre 
        }
    }


    procedure salirDelPuente(p: in int)
    {
        peso_actual-= p
        libre=true
        signal(peso_disp)
    }

}


process Vehiculo[id: 0..N-1]
{
    int peso = ...

    Puente.llegada(peso)
    cruzarPuente()
    Puente.salirDelPuente(peso)
}

```

## Ejercicio 7 
Se debe simular una maratón con C corredores donde en la llegada hay UNA máquina expendedoras de agua con capacidad para 20 botellas. Además, existe un repositor encargado de reponer las botellas de la máquina. Cuando los C corredores han llegado al inicio comienza la carrera. Cuando un corredor termina la carrera se dirigen a la máquina expendedora, espera su turno (respetando el orden de llegada), saca una botella y se retira. Si encuentra la máquina sin botellas, le avisa al repositor para que cargue nuevamente la máquina con 20 botellas; espera a que se haga la recarga; saca una botella y se retira. Nota: mientras se reponen las botellas se debe permitir que otros corredores se encolen.  

```C

Monitor Maraton
{
    int esperando = 0
    cond vcCorredores;
    

    procedure llegada()
    {
        esperando++

        if (esperando == C){
            signal_all(vcCorredores)
        } else {
            wait(vcCorredores)
        }
    }
}


Monitor máquina
{
    cond maquina;
    int botellas = 20
    bool libre = true
    cond reponer; 
    cond listo;
    int esperando = 0


    procedure dirigirHaciaMaquina(){
        if not(libre){
            esperando++
            wait(maquina)
        }
        
        if (botellas == 0){
            signal(reponer)
            wait(listo)
        }

    }

    procedure tomarBebida(){
        esperando--
        botellas-- 

        if(esperando == 0){
            libre = true
        } else signal(maquina)
    }

    procedure reponerMaquina(){
        wait(reponer)
        botellas = 20
        signal(listo)
    }
}

process Corredor[id: 1..C-1]
{
    Maraton.llegada()
    correr()
    Maquina.dirigirHaciaMáquina()
    Maquina.tomarBebida()
}

process repositor
{
    while(true){
        Maquina.reponerMaquina()
    }
}

```

## Ejercicio 8 
En un entrenamiento de fútbol hay 20 jugadores que forman 4 equipos (cada jugador conoce el equipo al cual pertenece llamando a la función DarEquipo()). Cuando un equipo está listo (han llegado los 5 jugadores que lo componen), debe enfrentarse a otro equipo que también esté listo (los dos primeros equipos en juntarse juegan en la cancha 1, y los otros dos equipos juegan en la cancha 2). Una vez que el equipo conoce la cancha en la que juega, sus jugadores se dirigen a ella. Cuando los 10 jugadores del partido llegaron a la cancha comienza el partido, juegan durante 50 minutos, y al terminar todos los jugadores del partido se retiran (no es necesario que se esperen para salir). 

```c

Monitor Equipo[id: 1..4]{
    cond vcJugadores 
    int jugadores = 0 

    procedure avisarLlegada(id_cancha: out int){
        jugadores++
        
        if(jugadores == 5){
            signal_all(vcJugadores)
            AdministradorCancha.esperarContrincante(id_cancha)
        } else wait(vcJugadores)
    }
}

Monitor Cancha[id: 1..2]{
    cant jugadores_esperando= 0
    cond jugadores, inicio

    procedure ir(){
        jugadores_esperando++

        if(jugadores_esperando == 10){
            signal(inicio) //Se da inicio
        }
        wait(jugadores) //Se queda en el partido 
    }

    procedure iniciar(){ //Si llegan los 10 jugadores antes que el proceso Partido entonces es necesario verificar queda en deadlock
        if (jugadores_esperando == 10){
            wait(inicio)
        }
    }

    procedure finalizarPartido(){
        signal_all(jugadores)
    }

}   


Monitor AdministradorCancha{
    int equipos = 4

    procedure esperarContrincante(cancha: out int){
        equipos--
        
        if (equipos >= 2){
            cancha = 1        
        } else {
            cancha = 2
        }
    }
}

process Jugador[id: 0..19]{
    int id_cancha;
    int equipo = DarEquipo()
    Equipo[equipo].avisarLlegada(id_cancha)
    Cancha[id_cancha].ir()
}


process Partido[id: 1..2]{
    Cancha.iniciar() 
    delay(50minutos)
    Cancha.finalizarPartido()
}

``` 

---

# Práctica Repaso - Monitores
## Ejercicio 1
Resolver el siguiente problema. En una elección estudiantil, se utiliza una máquina para voto electrónico. Existen N Personas que votan y una Autoridad de Mesa que les da acceso a la máquina de acuerdo con el orden de llegada, aunque ancianos y embarazadas tienen prioridad sobre el resto. La máquina de voto sólo puede ser usada por una persona a la vez. Nota: la función Votar() permite usar la máquina

```c

Monitor Maquina{

    int esperando = 0;
    cond personas[N]; 
    cola C; 
    cond fin, aviso;
    

    procedure formarFila(id: in int, prioridad: in bool){
        insertar(c,id, prioridad)
        signal(aviso)
        wait(personas[id])
    }

    procedure salirDeLaMesa(){
        signal(fin)
    }

    procedure siguiente(){
        if (isEmpty(c))  wait(aviso) //Esto es para que no haya deadlock

        sacar(c, id_sig)
        signal(personas[id_sig])

        wait(fin)

        }
    }

}


process Persona[id: 0..N-1]{
    bool prioridad = ...; 
    Maquina.formarFila(id, prioridad)
    votar()
    Maquina.salirDeLaMesa()
}

process Autoridad{
    while(true){
        Máquina.siguiente()
    }
}

```

## Ejercicio 3
En una montaña hay 30 escaladores que en una parte de la subida deben utilizar un único paso de a uno a la vez y de acuerdo con el orden de llegada al mismo. Nota: sólo se pueden utilizar procesos que representen a los escaladores; cada escalador usa sólo una vez el paso. 

```

```
