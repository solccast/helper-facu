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
    send avisoImpresora(id)
    receive aviso[id](nroImpr)
    usarImpresora(nroImpr, documento)
    send liberar(nroImpr)
}

process Administrador{
    bool impresoraLibre[5] = ([5] true)

    while (true){
        receive usarImpresora(idEmpleado)    

        
        send aviso[idEmpleado](nroImpresoraLibre)
        
        if not empty(liberar){
            receive liberar(nroImpresora)
            impresoraLibre[nroImpresora] = true
        }
    }
}

```

