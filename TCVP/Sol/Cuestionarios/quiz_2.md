# TCVP - CLASE 2: Quiz sobre la Jerarquía de la Computabilidad
1. ¿Cuál de las siguientes afirmaciones define correctamente un lenguaje recursivo (R)?
- (a) Es un lenguaje para el cual existe una Máquina de Turing (MT) que lo acepta.
- (b) Es un lenguaje para el cual existe una MT que lo acepta y siempre se detiene para cualquier entrada.
- (c) Es un lenguaje que puede ser generado por una gramática formal.
- (d) Es el complemento de un lenguaje recursivamente enumerable.
2. ¿Qué significa que un lenguaje $L$ pertenezca al conjunto de los lenguajes recursivamente enumerables (RE)?
- (a) Que existe una MT que siempre se detiene y decide si una cadena pertenece o no a $L$.
- (b) Que existe una MT que se detiene y acepta si una cadena pertenece a $L$, pero puede no detenerse si no pertenece.
- (c) Que tanto $L$ como su complemento $L^C$ son aceptados por alguna MT.
- (d) Que $L$ es un subconjunto de todos los lenguajes posibles.
3. ¿Cuál es la relación fundamental entre los conjuntos R (recursivos), RE (recursivamente enumerables) y CO-RE (complementos de lenguajes recursivamente enumerables)?
- (a) $RE \subset R \subset CO-RE$
- (b) $CO-RE \subset R \subset RE$
- (c) $R = RE \cup CO-RE$
- (d) $R = RE \cap CO-RE$ 
4. Si un lenguaje $L$ es recursivo ($L \in R$), ¿qué se puede decir de su complemento $L^C$?
- (a) $L^C$ no es necesariamente recursivo.
- (b) $L^C$ pertenece al conjunto RE pero no necesariamente a R.
- (c) $L^C$ también es recursivo ($L^C \in R$).
- (d) $L^C$ pertenece al conjunto CO-RE pero no necesariamente a R.
5. Si un lenguaje $L$ pertenece tanto a RE como a CO-RE, entonces:
- (a) $L$ es necesariamente no recursivo.
- (b) $L$ es necesariamente recursivo. 
- (c) $L$ no puede existir.
- (d) No se puede determinar si $L$ es recursivo o no.
6. ¿Cuál de las siguientes afirmaciones sobre la clausura de los conjuntos de lenguajes es verdadera?
- (a) El conjunto RE no es cerrado bajo la operación de intersección.
- (b) El conjunto R no es cerrado bajo la operación de unión.
- (c) El conjunto RE es cerrado bajo la operación de unión.
- (d) El conjunto R no es cerrado bajo la operación de complemento.
7. El conjunto CO-RE contiene todos los lenguajes cuyos:
- (a) Elementos pueden ser contados por una Máquina de Turing.
- (b) Complementos son recursivos.
- (c) Complementos son recursivamente enumerables. 
- (d) Son decididos por una Máquina de Turing no determinística.