import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from sklearn.linear_model import LinearRegression
from sklearn.metrics import mean_squared_error

# Cargar datos
df = pd.read_excel('META.xlsx')

# Convertir la columna 'fecha' a formato datetime
df['Date'] = pd.to_datetime(df['Date'])

# Crear una columna 'Dias' que represente el número de Dias desde la primera fecha
df['Dias'] = (df['Date'] - df['Date'].min()).dt.days

# Periodo de actualización
inicio_actualizaciones = pd.to_datetime('2024-02-02')
fin_actualizaciones = pd.to_datetime('2024-04-24')

# Análisis General
X = df[['Dias']].values
y = df['Precio U$D'].values

# Regresión lineal
modelo_general = LinearRegression()
modelo_general.fit(X, y)
predicciones_general = modelo_general.predict(X)
mse_general = mean_squared_error(y, predicciones_general)

# Análisis Sin Datos Durante la Actualización
df_sin_actualizaciones = df[~((df['Date'] >= inicio_actualizaciones) & (df['Date'] <= fin_actualizaciones))]
X_sin_actualizaciones = df_sin_actualizaciones[['Dias']].values
y_sin_actualizaciones = df_sin_actualizaciones['Precio U$D'].values

modelo_sin_actualizaciones = LinearRegression()
modelo_sin_actualizaciones.fit(X_sin_actualizaciones, y_sin_actualizaciones)
predicciones_sin_actualizaciones = modelo_sin_actualizaciones.predict(X_sin_actualizaciones)
mse_sin_actualizaciones = mean_squared_error(y_sin_actualizaciones, predicciones_sin_actualizaciones)

# Cálculo del impacto de las actualizaciones en el ajuste
diferencia_mse = mse_general - mse_sin_actualizaciones
porcentaje_cambio_mse = (diferencia_mse / mse_general) * 100

# Resultados
print("Modelo General:")
print(f"Pendiente: {modelo_general.coef_[0]}")
print(f"Intercepto: {modelo_general.intercept_}")
print(f"MSE (Error Cuadrático Medio): {mse_general}")

print("\nModelo Sin Datos Durante Actualizaciones:")
print(f"Pendiente: {modelo_sin_actualizaciones.coef_[0]}")
print(f"Intercepto: {modelo_sin_actualizaciones.intercept_}")
print(f"MSE (Error Cuadrático Medio): {mse_sin_actualizaciones}")

print(f"\nPorcentaje de Cambio en el MSE debido a las actualizaciones: {porcentaje_cambio_mse:.2f}%")

# Graficar
plt.figure(figsize=(12, 6))

# Datos originales
plt.scatter(df['Dias'], df['Precio U$D'], color='blue', label='Datos Originales')

# Línea de regresión general
plt.plot(df['Dias'], predicciones_general, color='red', label='Regresión General')

# Línea de regresión sin datos del periodo de actualización
plt.plot(df_sin_actualizaciones['Dias'], predicciones_sin_actualizaciones, color='green', linestyle='--', label='Regresión Sin Actualizaciones')

# Detalles del gráfico
plt.title('Regresión Lineal con y Sin Datos Durante Actualizaciones')
plt.xlabel('Días')
plt.ylabel('Precio')
plt.legend()
plt.grid(True)

# Añadir el porcentaje de cambio en el MSE al gráfico
plt.text(100, y=df['Precio U$D'].min(), 
         s=f'Porcentaje de Cambio en el MSE: {porcentaje_cambio_mse:.2f}%',
         fontsize=12, color='black', va='bottom', ha='left')

plt.show()
