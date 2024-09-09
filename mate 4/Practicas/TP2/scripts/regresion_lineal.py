import csv
from pathlib import Path

# Obtener el directorio actual del script
directorio_actual = Path(__file__).parent

# Ir un nivel arriba y luego acceder a la carpeta 'datos'
directorio_datos = directorio_actual.parent / 'datos'

# Crear la ruta del archivo
ruta_archivo = directorio_datos / 'EJ4.csv'

# Abrir y leer el archivo CSV con encabezados
with open(ruta_archivo, mode='r', encoding="utf-8") as archivo:
    lector_csv = csv.DictReader(archivo)
    
    # Leer e imprimir cada fila del archivo CSV como diccionario
    for fila in lector_csv:
        print(fila)

