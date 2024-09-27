import os
import pandas as pd
import numpy as np
import math
from scipy.stats import t as dist_student

# Función para cargar y limpiar el DataFrame
def get_df_clean():
    """
    Carga el archivo CSV de los jugadores y limpia el DataFrame eliminando columnas con valores nulos.
    """
    archivo_csv = f'{os.path.dirname(os.path.abspath(__file__))}{os.sep}players_21.csv'
    try:
        df = pd.read_csv(archivo_csv)
    except FileNotFoundError:
        print("No se encontró el archivo deseado.")
        return None
    return df.dropna(axis=1, how='any')


def significance_level(n):
    """
    Calculamos nivel de significancia y analizamos Hipótesis en función de T
    """
    alpha = 0.05
    t_critico = dist_student.ppf(1 - alpha/2, n - 2)
    return t_critico


def calculate_confidence_interval(beta, t, variance):
    """
    Calcula el intervalo de confianza para un coeficiente beta dado.
    Parámetros:
    - beta: Coeficiente de regresión (b0 o b1)
    - t: Valor crítico de la distribución t-Student
    - variance: Varianza del coeficiente
    
    Retorna:
    - (ini_conf, end_conf): Tupla con los límites inferior y superior del intervalo de confianza
    """
    margin_of_error = t * np.sqrt(variance)
    return beta - margin_of_error, beta + margin_of_error

def estimate_fluctuations(beta_zero, beta_one, significative_level, x_mean, sigma_2, sxx, n):
    """
    Estima los intervalos de confianza para los coeficientes de regresión b0 y b1.
    Imprime:
    - Intervalos de confianza para b0 y b1
    """
    # Calcular varianzas de b1 y b0
    var_beta1 = sigma_2 / sxx
    var_beta0 = sigma_2 * (1 / n + (x_mean ** 2) / sxx)
    
    # Calcular intervalos de confianza usando la función auxiliar
    ini_conf_b1, end_conf_b1 = calculate_confidence_interval(beta_one, significative_level, var_beta1)
    ini_conf_b0, end_conf_b0 = calculate_confidence_interval(beta_zero, significative_level, var_beta0)
    
    # Imprimir resultados
    print(f"Intervalo de confianza para b1: ({ini_conf_b1}, {end_conf_b1})")
    print(f"Intervalo de confianza para b0: ({ini_conf_b0}, {end_conf_b0})")


# Función para generar valores estadísticos requeridos para el análisis
def generate_statistical_values(ss_res, independiente):
    """
    Genera los valores estadísticos necesarios para el análisis de regresión:
    varianza estimada, covarianza estimada, y pendiente beta.
    
    Datos utilizados:
    - ss_res: suma de los residuos al cuadrado (error residual)
    - independiente: datos de la característica independiente (como 'overall', 'age', etc.)
    
    Retorna:
    - estimated_variance: Varianza estimada
    - sxx: Suma de los cuadrados de la desviación de x respecto a su media
    - level_significance: Nivel de significancia
    - beta_one: Pendiente de la recta de regresión (beta_one)
    """
    n = len(independiente)
    x_total_fields = n - 2
    
    estimated_variance = math.sqrt(ss_res / x_total_fields)
    estimated_covariance = ss_res / x_total_fields
    sxx = np.sum((independiente - np.mean(independiente)) ** 2)
    
    return estimated_variance, sxx, estimated_covariance

# Función para calcular el estadístico T
def calculate_t_stat(beta_one, estimated_variance, sxx):
    """
    Calcula el valor del estadístico T para validar la hipótesis de regresión.
    
    Datos utilizados:
    - beta_one: Pendiente de la regresión (m)
    - estimated_variance: Varianza estimada
    - sxx: Suma de los cuadrados de la desviación de x respecto a su media
    
    Retorna:
    - t_value: Estadístico T para la prueba de significancia
    """
    if sxx == 0:
        raise ValueError("Sxx es cero. No se puede continuar con el cálculo.")
    
    x = estimated_variance / sxx
    return beta_one / (math.sqrt(x))

# Función para obtener la proporción de veces que el valor de mercado supera la incertidumbre de predicción
def get_estimated_proportion(df, x_avg, sxx, estimated_variance):
    """
    Calcula cuántas veces el valor de mercado supera la incertidumbre de predicción.
    
    Datos utilizados:
    - df: DataFrame que contiene los datos de los jugadores
    - x_avg: Media de la característica independiente
    - sxx: Suma de los cuadrados de la desviación de x respecto a su media
    - estimated_variance: Varianza estimada
    
    Retorna:
    - Proporción escalada de veces que el valor de mercado supera la incertidumbre de predicción.
    """
    result = estimated_variance * ((1 / len(df)) + (x_avg / sxx))
    exceed_count = sum(df['value_eur'] > result)
    total_count = len(df['value_eur'])
    
    proportion = exceed_count / total_count
    proportion_scaled = proportion * math.sqrt(total_count - 2)

    print(f"Proporción de veces que el valor de mercado supera la incertidumbre de predicción: {proportion:.4f}")
    print(f"Proporción escalada por sqrt(n-2): {proportion_scaled:.4f}")

# Función para calcular el coeficiente de determinación R²
def calcular_r_cuadrado(dependiente, independiente):
    """
    Calcula el coeficiente de determinación (R²) para una variable dependiente e independiente.
    
    Datos utilizados:
    - dependiente: 'value_eur' (valor de mercado de un jugador)
    - independiente: Cualquier característica del jugador (como 'overall', 'age', 'potential', etc.)
    
    Este cálculo nos da:
    - La pendiente (m o beta_one) para la regresión
    - R² para medir la calidad del ajuste
    - Intercepto (b o beta_zero) para la regresión
    """
    y_mean = np.mean(dependiente)  # Media de la variable dependiente (valor de mercado)
    m = np.cov(independiente, dependiente)[0, 1] / np.var(independiente)  # Pendiente de la recta de regresión (beta_one)
    b = y_mean - m * np.mean(independiente)  # Intercepto de la recta (beta_0)

    y_pred = m * independiente + b  # Valores predichos por la recta de regresión
    ss_res = np.sum((dependiente - y_pred) ** 2)  # Suma de los residuos al cuadrado
    ss_tot = np.sum((dependiente - y_mean) ** 2)  # Suma total de cuadrados
    
    return m, 1 - (ss_res / ss_tot), ss_res, b

# Función principal para obtener las 5 características más correlacionadas
def get_five_best():
    """
    Obtiene las 5 características más correlacionadas con el valor de mercado
    y calcula la regresión lineal para cada una.(Intervalos de Confianza únicamente hechos para overall, aunque borrando el if se obtienen el resto con los niveles dados.)
    Datos utilizados:
    - Característica objetivo: 'value_eur' (valor de mercado de un jugador)
    - Variables independientes: 5 características del jugador con mayor correlación con 'value_eur'
    
    Paso 1: Se calculan las correlaciones y se seleccionan las 5 características más relevantes.
    Paso 2: Para cada una, se calcula R², la pendiente (beta_one) y se realiza una prueba de significancia con el estadístico T.
    """
    df = get_df_clean().select_dtypes(include=['number'])
    columna_referencia = 'value_eur'  # Esta es la variable dependiente que queremos predecir

    # Calculamos la matriz de correlación y seleccionamos los 5 atributos más relevantes
    correlaciones = df.corr()[columna_referencia].drop(index=columna_referencia)
    top_5_correlaciones = correlaciones.abs().nlargest(5)

    print(f"Los 5 atributos con mayor correlación lineal con '{columna_referencia}' son:")
    for atributo in top_5_correlaciones.index:
        # Paso 1: Cálculo de la recta de regresión
        beta_one, r_cuadrado, ss_res, beta_zero = calcular_r_cuadrado(df[columna_referencia], df[atributo])
        print(f"{atributo}: Correlación: {top_5_correlaciones[atributo]:.3f}, R²: {r_cuadrado:.3f}")

        # Paso 2: Generar valores estadísticos y calcular T
        estimated_variance, sxx, estimated_covariance = generate_statistical_values(ss_res, df[atributo])
        t_value = calculate_t_stat(beta_one, estimated_variance, sxx)
        significative_level = significance_level(len(df[columna_referencia]))
        print(f"Estadístico T: {t_value:.4f}, Nivel de Significancia: (t):{significative_level:.4f},\n \
            Varianza: {estimated_variance:.4f}, Covarianza: {estimated_covariance:.4f},\n \
            Beta 1: {beta_one:.4f}, Beta 0: {beta_zero:.4f}")

        # Obtenemos Intervalos de Confianza para los parámetros del atr. a analizar
        if atributo == "overall":
            x_mean = np.mean(df[atributo])
            sigma_2 = estimated_variance**2
            estimate_fluctuations(beta_zero, beta_one, significative_level, x_mean, sigma_2, sxx,len(df[columna_referencia]))

            # Proporción de veces que el valor de mercado supera la incertidumbre de predicción
            get_estimated_proportion(df, np.mean(df[atributo]), sxx, estimated_variance)
            
            x = estimated_variance * ( (1/len(df[columna_referencia]))+ (((np.mean(df[atributo])**2)/sxx)) )
# Ejecución del análisis
if __name__ == "__main__":
    get_five_best()