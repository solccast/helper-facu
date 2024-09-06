import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# Definición de la función f(x, y)
def f(x, y):
    return np.where(x**2 + y**2 != 0, x * y**2 / (x**2 + y**2), 0)

# Crear un espacio de valores para x e y
x = np.linspace(-2, 2, 400)
y = np.linspace(-2, 2, 400)
X, Y = np.meshgrid(x, y)
Z = f(X, Y)

# Menú para seleccionar el tipo de gráfico
def mostrar_menu():
    print("Seleccione el tipo de gráfico que desea visualizar:")
    print("1. Superficie 3D")
    print("2. Contorno")
    print("3. Mapa de calor")

# Funciones para los distintos gráficos
def grafico_superficie():
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    ax.plot_surface(X, Y, Z, cmap='viridis')
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('f(x, y)')
    plt.show()

def grafico_contorno():
    plt.contour(X, Y, Z, 50, cmap='viridis')
    plt.xlabel('X')
    plt.ylabel('Y')
    plt.title('Gráfico de Contorno')
    plt.colorbar()
    plt.show()

def grafico_mapa_calor():
    plt.imshow(Z, extent=(-2, 2, -2, 2), origin='lower', cmap='viridis')
    plt.colorbar()
    plt.xlabel('X')
    plt.ylabel('Y')
    plt.title('Mapa de Calor')
    plt.show()

# Ejecutar el menú
def ejecutar_menu():
    while True:
        mostrar_menu()
        opcion = input("Ingrese el número de la opción seleccionada: ")
        
        if opcion == '1':
            grafico_superficie()
        elif opcion == '2':
            grafico_contorno()
        elif opcion == '3':
            grafico_mapa_calor()
        else:
            print("Opción no válida. Inténtelo de nuevo.")
            continue
        
        # Preguntar si desea continuar o salir
        continuar = input("¿Desea seleccionar otro gráfico? (s/n): ").lower()
        if continuar != 's':
            break

# Ejecutar el programa
ejecutar_menu()
