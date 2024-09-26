import os
import pandas as pd
import statsmodels.api as sm
import numpy as np
import math


estimated_variance = 0
estimated_covariance = 0
beta_one = 0
sxx = 0
level_significance = 0

def calculate_T():
    if sxx == 0:
        print("vo ta loco vite")
    x = estimated_variance / sxx
    return beta_one / (math.sqrt(x))


def get_df_clean():
    archivo_csv = f'{os.path.dirname(os.path.abspath(__file__))}{os.sep}players_21.csv'
    try:
        df = pd.read_csv(archivo_csv)
    except FileNotFoundError:
        print("No se encontró el archivo deseado.")
        return None
    return df.dropna(axis=1, how='any')

def generate_values(m, ss_res, independiente):
    
        global estimated_variance
        global estimated_covariance
        global beta_one
        global sxx
        global level_significance
        
        x_total_fields = len(independiente) - 2
        estimated_variance = math.sqrt(ss_res / x_total_fields)
        estimated_covariance = ss_res / x_total_fields
        beta_one = m
        sxx = np.sum((independiente - np.mean(independiente)) ** 2)
        level_significance = math.sqrt(x_total_fields)


def calcular_r_cuadrado(dependiente, independiente):
    """
    Calcula el coeficiente de determinación (R²) para una variable dependiente e independiente.
    """
    # Calcular la media de la variable dependiente
    y_mean = np.mean(dependiente)

    # Calcular la pendiente (m) y la intersección (b) de la línea de regresión
    m = np.cov(independiente, dependiente)[0, 1] / np.var(independiente)
    b = y_mean - m * np.mean(independiente)
    # Predicciones
    y_pred = m * independiente + b
    # Cálculo de SS_res y SS_tot
    ss_res = np.sum((dependiente - y_pred) ** 2)
    ss_tot = np.sum((dependiente - y_mean) ** 2)
    if independiente.name == 'overall':
        global x_avg
        global y_avg
        y_avg = y_mean
        x_avg = np.mean(independiente)
        generate_values(m, ss_res, independiente)
    # Calcular R²
    return 1 - (ss_res / ss_tot)

def get_five_best():
    df = get_df_clean().select_dtypes(include=['number'])
    # Elegimos columna a comparar
    columna_referencia = 'value_eur'
    # Obtenemos desde la matriz de correlación  los valores de R**2 respecto del atributo que nos interesa
    correlaciones = df.corr()[columna_referencia].drop(index=columna_referencia)

    # Obtenemos los 5 atributos con mayor correlación
    top_5_correlaciones = correlaciones.abs().nlargest(5)

    print("Los 5 atributos con mayor correlación lineal con '{}' son:".format(columna_referencia))
    for atributo in top_5_correlaciones.index:
        valor_correlacion = top_5_correlaciones[atributo]
        valor_r_cuadrado = calcular_r_cuadrado(df[columna_referencia], df[atributo])
        print(f"{atributo}: Correlación: {valor_correlacion:.3f}, R²: {valor_r_cuadrado:.3f}")
    t_value = calculate_T()
    print(f"El estadístico T que valida nuestra Hipótesis alternativa es:{t_value:.4f}")
    print(f"Valor del nivel de significancia:{level_significance:.4f}, Beta 1: {beta_one:.4f}")
    

# Ejemplo de uso
if __name__ == "__main__":
    get_five_best()
    df = get_df_clean().select_dtypes(include=['number'])
    # Elegimos columna a comparar
    # Obtenemos desde la matriz de correlación  los valores de R**2 respecto del atributo que nos interesa
    Y = df['value_eur']
    X = df['overall']

    print("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%5555")
    print(f"varianza:{estimated_variance}, promedio(x):{x_avg} ")
    
    X = sm.add_constant(X)
    
    model = sm.OLS(Y, X).fit()
    summary = model.summary()

    rsquared = model.rsquared
    print(f"Resumen :{summary},\n \t raiz cuadrada:{rsquared}")
    result = estimated_variance * ( (1/18946) + (x_avg/sxx) )
    print(f"toda la raizz y cuenterio :{math.sqrt(result):.4f}")
    result_of_beta_0_estimated = y_avg + beta_one * x_avg
    print(f"Beta 0 :{result_of_beta_0_estimated:.4f}")
    x = estimated_variance / sxx
    print(f"Beta 1 :{beta_one}")
        
    
    
    
    