## Ejercicio 1

1. 
E = {a, b, c}
L = {a^n b^n c^n|n>= 0}

=>  Ʃ*⋂ L = L  , pues L se encuentra contenido en  Ʃ*
=>  Ʃ* ⋃ L = Ʃ* , pues todo elemento de L se encuentra en Ʃ*
=> L^c = Ʃ*- L  , como L se encuentra contenido en Ʃ* entonces todo los elementos que pertenecen a L pertenecen también a Ʃ* su intersección serán todos los elementos que pertenecen a Ʃ* y no a L.

2. Problema de satisfactibilidad de las fórmulas booleanas 
- Visión de MT calculadora -> ante la entrada de fórmula booleana la salida será una asignación de valores de verdad, si no existe la máquina rechazará. 
- Visión de MT reconocedora -> ante la entrada de fórmula booleana la salida será _sí_ si existe una asignación de valores que la expresión se evalúe como verdadera, por el contrario rechazará. 
- Visión de MT generadora -> ante la entrada de fórmula booleana la salida será un conjunto de asignaciones de valores de verdad 

3. Tesis de Church-Turing: Todo dispositivo de computación físicamente realizable puede ser simulado por una MT.

4. 
- MTs equivalentes -> aceptan el mismo lenguaje, o sea, ante la misma cadena de entrada ambas máquinas tendrán el mismo comportamiento. M1 y M2 pueden tener mecanismos internos y formas diferentes en procesar la entrada pero se comportan igual.
- Modelos de MT equivalentes -> si dada una MT de un modelo existe una MT equivalente del otro. Ejemplo: MT con una sola cinta y otro modelo con varias cintas ya que cualquier lenguaje que pueda ser reconocido por la máquina de dos cintas entonces es también reconocible por el modelo de una cinta. 

5. 
- Lenguajes R (recursivos): que ante la entrada la máquina siempre se detendrá (rechace o acepte)
- Lenguajes RE (recursivos enumerables): 


