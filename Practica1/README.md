# Sistema de Análisis de Rendimiento Académico

## Descripción General

Sistema desarrollado en **C++** para procesar y analizar datos académicos de estudiantes. El programa permite cargar información de estudiantes, cursos y notas desde archivos .lfp, para generar 5 reportes detallados en formato **HTML** con estadísticas, análisis de rendimiento y métricas de desempeño.

---

## Requisitos

- **Compilador**: g++ (MinGW/msys64 ucrt64)
- **Sistema Operativo**: Windows/Linux/macOS
- **Librerías**: Solo STL (vector, string, algorithm, cmath, fstream, sstream, iomanip)

---

## Estructura del Proyecto

```
Practica1/
├── README.md                          # Este archivo
├── src/
│   └── main.cpp                       # Programa principal (~900 líneas)
├── data/
│   ├── estudiantes.lfp               # Datos de estudiantes (15 registros)
│   ├── cursos.lfp                    # Datos de cursos (15 registros)
│   └── notas.lfp                     # Calificaciones (45 registros)
└── docs/
    ├── reporte1_estadisticas_por_curso.html (creados por usuario)
    ├── reporte2_rendimiento_por_estudiante.html
    ├── reporte3_top10_estudiantes.html
    ├── reporte4_cursos_reprobacion.html
    └── reporte5_analisis_carrera.html
```

---

## Cómo Compilar

### Opción 1: Desde el terminal (recomendado)

```bash
cd c:\Users\angel\OneDrive\Escritorio\LFP_A_1S2026\Practica1\src
g++ .\main.cpp -o .\main.exe
```

### Opción 2: Desde VS Code

1. Abre el archivo `src/main.cpp`
2. Presiona `Ctrl + Shift + B` (ejecuta la tarea de build)
3. El compilador generará `main.exe`

---

## Cómo Ejecutar

### Desde la carpeta `src/`

```bash
cd .\src
.\main.exe
```

### Desde la raíz del proyecto

```bash
.\src\main.exe
```

---

## Menú de Opciones

Una vez ejecutado el programa, verás el siguiente menú:

```
===============================================
    SISTEMA DE ANÁLISIS ACADÉMICO
===============================================
1. Cargar estudiantes
2. Cargar cursos
3. Cargar notas
4. Generar reporte 1: Estadísticas por curso
5. Generar reporte 2: Rendimiento por estudiante
6. Generar reporte 3: Top 10 mejores estudiantes
7. Generar reporte 4: Cursos con reprobación
8. Generar reporte 5: Análisis por carrera
9. Salir
Selecciona una opción: 
```

---

## Flujo de Uso Recomendado

1. **Opción 1**: Cargar estudiantes (se cargará `data/estudiantes.lfp`)
2. **Opción 2**: Cargar cursos (se cargará `data/cursos.lfp`)
3. **Opción 3**: Cargar notas (se cargará `data/notas.lfp`)
4. **Opciones 4-8**: Generar los reportes deseados
5. **Opción 9**: Salir del programa

---

## Descripción de Reportes

### Reporte 1: Estadísticas por Curso
**Archivo generado**: `docs/reporte1_estadisticas_por_curso.html`

Tabla con estadísticas por cada curso:
- Código y nombre del curso
- Cantidad de estudiantes evaluados
- Promedio, máximo, mínimo
- Desviación estándar y mediana

### Reporte 2: Rendimiento por Estudiante
**Archivo generado**: `docs/reporte2_rendimiento_por_estudiante.html`

Tabla que muestra para cada estudiante:
- Carnet, nombre completo, carrera y semestre
- Promedio general
- Cursos aprobados y reprobados
- Créditos acumulados

### Reporte 3: Top 10 Mejores Estudiantes
**Archivo generado**: `docs/reporte3_top10_estudiantes.html`

Ranking de los 10 estudiantes con mejor promedio:
- Posición, carnet, nombre
- Carrera, semestre y promedio final

### Reporte 4: Cursos con Mayor Reprobación
**Archivo generado**: `docs/reporte4_cursos_reprobacion.html`

Análisis de reprobación ordenado descendente:
- Código y nombre del curso
- Total de estudiantes, aprobados y reprobados
- Porcentaje de reprobación

### Reporte 5: Análisis por Carrera
**Archivo generado**: `docs/reporte5_analisis_carrera.html`

Resumen estadístico por cada carrera:
- Total de estudiantes
- Promedio general de la carrera
- Cursos disponibles
- Distribución de estudiantes por semestre

---

## Descripción de Archivos de Datos

### `data/estudiantes.lfp`
Formato: `carnet,nombre,apellido,carrera,semestre`

Ejemplo:
```
2023001,Juan,Pérez,Sistemas,1
2024005,María,González,Civil,3
2025010,Carlos,López,Industrial,2
```

### `data/cursos.lfp`
Formato: `codigo,nombre,creditos,semestre,carrera`

Ejemplo:
```
250,Programación 1,4,1,Sistemas
300,Estructuras de Datos,4,2,Sistemas
841,Química General,3,1,Química
```

### `data/notas.lfp`
Formato: `carnet,codigo_curso,nota,ciclo,anio`

Ejemplo:
```
2023001,250,78.5,S1,2024
2024005,300,65.0,S1,2024
2025010,250,92.3,S2,2024
```

---

## Notas Requeridas

- **Aprobado**: Nota >= 61
- **Reprobado**: Nota < 61

---

## Archivos Generados

Todos los reportes se guardan en `docs/` en formato HTML con:
- Tablas formateadas
- Estilos CSS integrados
- Colores y tipografía profesional
- Encabezados descriptivos
- Fácil de abrir en cualquier navegador

---

## Ejemplo de Ejecución Completa

```
===============================================
    SISTEMA DE ANÁLISIS ACADÉMICO
===============================================
1. Cargar estudiantes
2. Cargar cursos
3. Cargar notas
4. Generar reporte 1: Estadísticas por curso
5. Generar reporte 2: Rendimiento por estudiante
6. Generar reporte 3: Top 10 mejores estudiantes
7. Generar reporte 4: Cursos con reprobación
8. Generar reporte 5: Análisis por carrera
9. Salir
Selecciona una opción: 1

se cargaron 15 estudiantes exitosamente!!

Selecciona una opción: 2

se cargaron 15 cursos exitosamente!!

Selecciona una opción: 3

se cargaron 45 notas exitosamente!!

Selecciona una opción: 4

reporte html generado: reporte1_estadisticas_por_curso.html

Selecciona una opción: 9

cerrando, hasta luego! :)
```

---

## Datos técnicos

- El programa usa **struct** para modelar datos (no clases)
- Utiliza **vectores dinámicos** de STL para almacenar registros
- Implementa **búsqueda lineal** para filtrar datos
- Los reportes se generan en **HTML con CSS inline**
- Maneja **rutas relativas** para archivos de datos
- Usa **set** para contar elementos únicos

---

## Información

**Curso**: Laboratorio de Lenguajes Formales y de Programación A (LFP)
**Nombre**: Angel Sebastian Mazariegos Pérez, 202300583
**Universidad**: Universidad de San Carlos de Guatemala

