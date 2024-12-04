# Direccionamiento - Capa de red
## Apuntes 
Máscara -> indicará la cantidad de host que podrá aguantar el host. 
(2^32-mask - 2) -> se descarta loas masscaras 31 y 32 (no se usan). Se resta -2 por la dirección del broadcast.
Entonces para la cantidad que requiere la red, tenemos que elegir la mascara que sobre pero con el que sea suficiente - mínimo posible. 
La máscara va aindicar la parte de la dirección ip que quedará inmovilizado. 
Una vez que se vvan diviendo por los bloques el primero hasta llegar a la dirección deseada se asigna a la red y la segunda se utiliza para la siguiente red. En caso de no alcanzar el proceso se repite pero se tiene que bajar la primera dirección obtenida.
Las direcciones libres se enceuntran en la segunda columna. Seiempre que se llega a la máscara deseada se la asigna la primera. 
9