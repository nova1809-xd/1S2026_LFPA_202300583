#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <set>
#include <map>

using namespace std;

// estructura para guardar los datos de un estudiante
struct Estudiante {
    int carnet;
    string nombre;
    string apellido;
    string carrera;
    int semestre;
};

// estructura para guardar los datos de un curso
struct Curso {
    int codigo;
    string nombre;
    int creditos;
    int semestre;
    string carrera;
};

// estructura para guardar las notas de un estudiante en un curso
struct Nota {
    int carnet;
    int codigo_curso;
    double nota;
    string ciclo;
    int año;
};

// vectores globales para almacenar todos los datos
vector<Estudiante> estudiantes;
vector<Curso> cursos;
vector<Nota> notas;

// función auxiliar para separar un string por un delimitador (como el split de python)
vector<string> split(const string& texto, char delimitador) {
    // acá voy guardando cada pedacito que salga del texto
    vector<string> resultado;

    // convierto el texto a stream para leerlo por partes con getline
    stringstream ss(texto);

    // esta variable temporal me guarda cada parte antes de meterla al vector
    string item;
    
    // mientras getline encuentre otro pedazo separado por el delimitador, sigo
    // ejemplo rápido: "a,b,c" con ',' -> primero "a", luego "b", luego "c"
    while (getline(ss, item, delimitador)) {
        // aquí solo lo meto al vector final
        resultado.push_back(item);
    }
    
    // al final regreso todo ya separado
    return resultado;
}

// función para sacar el promedio de una lista de notas
double calcularPromedio(const vector<double>& valores) {
    // si no hay valores, regreso 0 para evitar división entre 0
    if (valores.empty()) return 0.0;

    // voy sumando todo para luego dividir entre la cantidad
    double suma = 0.0;
    for (double valor : valores) {
        suma += valor;
    }

    return suma / valores.size();
}

// función para sacar la nota máxima de una lista
double calcularMaximo(const vector<double>& valores) {
    // si está vacío, no hay máximo real, entonces regreso 0
    if (valores.empty()) return 0.0;

    return *max_element(valores.begin(), valores.end());
}

// función para sacar la nota mínima de una lista
double calcularMinimo(const vector<double>& valores) {
    // si está vacío, no hay mínimo real, entonces regreso 0
    if (valores.empty()) return 0.0;

    return *min_element(valores.begin(), valores.end());
}

// función para sacar la mediana de una lista
double calcularMediana(vector<double> valores) {
    // si no hay datos, regreso 0
    if (valores.empty()) return 0.0;

    // ordeno para poder encontrar el centro
    sort(valores.begin(), valores.end());

    int n = static_cast<int>(valores.size());

    // si la cantidad es impar, la mediana es el dato del centro
    if (n % 2 != 0) {
        return valores[n / 2];
    }

    // si es par, saco el promedio de los dos del centro
    return (valores[(n / 2) - 1] + valores[n / 2]) / 2.0;
}

// función para sacar la desviación estándar (poblacional)
double calcularDesviacionEstandar(const vector<double>& valores) {
    // con lista vacía no hay desviación
    if (valores.empty()) return 0.0;

    double promedio = calcularPromedio(valores);
    double sumaDiferencias = 0.0;

    // aquí voy acumulando (xi - promedio)^2
    for (double valor : valores) {
        double diferencia = valor - promedio;
        sumaDiferencias += (diferencia * diferencia);
    }

    // varianza poblacional = suma / n, luego raíz para desviación
    double varianza = sumaDiferencias / valores.size();
    return sqrt(varianza);
}

// reporte 1: estadísticas generales por curso (en html)
void generarReporteEstadisticasPorCursoHTML() {
    // validación rápida para no generar html vacío
    if (cursos.empty() || notas.empty()) {
        cout << "primero carga cursos y notas para generar este reporte\n";
        return;
    }

    // ejecutado desde src
    ofstream archivo("../docs/reporte1_estadisticas_por_curso.html");

    // ejecutado desde raiz
    if (!archivo.is_open()) {
        archivo.open("docs/reporte1_estadisticas_por_curso.html");
    }

    if (!archivo.is_open()) {
        cout << "error: no se pudo crear el archivo html del reporte 1\n";
        return;
    }

    // aquí escribo la estructura básica del html
    archivo << "<!DOCTYPE html>\n";
    archivo << "<html lang=\"es\">\n";
    archivo << "<head>\n";
    archivo << "  <meta charset=\"UTF-8\">\n";
    archivo << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    archivo << "  <title>Reporte 1 - Estadisticas por Curso</title>\n";
    archivo << "  <style>\n";
    archivo << "    body { font-family: Arial, sans-serif; margin: 24px; background: #f8f9fb; }\n";
    archivo << "    h1 { color: #1f2937; }\n";
    archivo << "    table { width: 100%; border-collapse: collapse; background: white; }\n";
    archivo << "    th, td { border: 1px solid #d1d5db; padding: 8px; text-align: left; }\n";
    archivo << "    th { background: #e5e7eb; }\n";
    archivo << "    tr:nth-child(even) { background: #f9fafb; }\n";
    archivo << "  </style>\n";
    archivo << "</head>\n";
    archivo << "<body>\n";
    archivo << "  <h1>Reporte 1: Estadisticas Generales por Curso</h1>\n";
    archivo << "  <table>\n";
    archivo << "    <thead>\n";
    archivo << "      <tr>\n";
    archivo << "        <th>Codigo</th>\n";
    archivo << "        <th>Curso</th>\n";
    archivo << "        <th>Cantidad</th>\n";
    archivo << "        <th>Promedio</th>\n";
    archivo << "        <th>Maxima</th>\n";
    archivo << "        <th>Minima</th>\n";
    archivo << "        <th>Desv. Estandar</th>\n";
    archivo << "        <th>Mediana</th>\n";
    archivo << "      </tr>\n";
    archivo << "    </thead>\n";
    archivo << "    <tbody>\n";

    // recorro los cursos y voy metiendo una fila por cada uno
    for (const Curso& curso : cursos) {
        vector<double> notasCurso;

        // filtro las notas de este curso
        for (const Nota& n : notas) {
            if (n.codigo_curso == curso.codigo) {
                notasCurso.push_back(n.nota);
            }
        }

        double promedio = calcularPromedio(notasCurso);
        double maximo = calcularMaximo(notasCurso);
        double minimo = calcularMinimo(notasCurso);
        double desviacion = calcularDesviacionEstandar(notasCurso);
        double mediana = calcularMediana(notasCurso);

        archivo << "      <tr>\n";
        archivo << "        <td>" << curso.codigo << "</td>\n";
        archivo << "        <td>" << curso.nombre << "</td>\n";
        archivo << "        <td>" << notasCurso.size() << "</td>\n";
        archivo << "        <td>" << fixed << setprecision(2) << promedio << "</td>\n";
        archivo << "        <td>" << fixed << setprecision(2) << maximo << "</td>\n";
        archivo << "        <td>" << fixed << setprecision(2) << minimo << "</td>\n";
        archivo << "        <td>" << fixed << setprecision(2) << desviacion << "</td>\n";
        archivo << "        <td>" << fixed << setprecision(2) << mediana << "</td>\n";
        archivo << "      </tr>\n";
    }

    archivo << "    </tbody>\n";
    archivo << "  </table>\n";
    archivo << "</body>\n";
    archivo << "</html>\n";

    archivo.close();
    cout << "reporte html generado: reporte1_estadisticas_por_curso.html\n";
}

// función auxiliar para buscar créditos de un curso por código
int obtenerCreditosCurso(int codigoCurso) {
    // recorro la lista de cursos hasta encontrar el código
    for (const Curso& curso : cursos) {
        if (curso.codigo == codigoCurso) {
            return curso.creditos;
        }
    }

    // si no lo encontré, regreso 0 para no romper cálculos
    return 0;
}

// reporte 2: rendimiento por estudiante (en html)
void generarReporteRendimientoPorEstudianteHTML() {
    // para este reporte sí necesito estudiantes, cursos y notas
    if (estudiantes.empty() || cursos.empty() || notas.empty()) {
        cout << "primero carga estudiantes, cursos y notas para generar este reporte\n";
        return;
    }

    // desde src
    ofstream archivo("../docs/reporte2_rendimiento_por_estudiante.html");

    // por si se ejecuta desde la raiz
    if (!archivo.is_open()) {
        archivo.open("docs/reporte2_rendimiento_por_estudiante.html");
    }

    if (!archivo.is_open()) {
        cout << "error: no se pudo crear el archivo html del reporte 2\n";
        return;
    }

    // escribo estructura base del html
    archivo << "<!DOCTYPE html>\n";
    archivo << "<html lang=\"es\">\n";
    archivo << "<head>\n";
    archivo << "  <meta charset=\"UTF-8\">\n";
    archivo << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    archivo << "  <title>Reporte 2 - Rendimiento por Estudiante</title>\n";
    archivo << "  <style>\n";
    archivo << "    body { font-family: Arial, sans-serif; margin: 24px; background: #f8f9fb; }\n";
    archivo << "    h1 { color: #1f2937; }\n";
    archivo << "    table { width: 100%; border-collapse: collapse; background: white; }\n";
    archivo << "    th, td { border: 1px solid #d1d5db; padding: 8px; text-align: left; }\n";
    archivo << "    th { background: #e5e7eb; }\n";
    archivo << "    tr:nth-child(even) { background: #f9fafb; }\n";
    archivo << "  </style>\n";
    archivo << "</head>\n";
    archivo << "<body>\n";
    archivo << "  <h1>Reporte 2: Rendimiento por Estudiante</h1>\n";
    archivo << "  <table>\n";
    archivo << "    <thead>\n";
    archivo << "      <tr>\n";
    archivo << "        <th>Carnet</th>\n";
    archivo << "        <th>Nombre</th>\n";
    archivo << "        <th>Carrera</th>\n";
    archivo << "        <th>Semestre</th>\n";
    archivo << "        <th>Promedio</th>\n";
    archivo << "        <th>Aprobados</th>\n";
    archivo << "        <th>Reprobados</th>\n";
    archivo << "        <th>Creditos Acumulados</th>\n";
    archivo << "      </tr>\n";
    archivo << "    </thead>\n";
    archivo << "    <tbody>\n";

    // recorro cada estudiante y calculo sus datos
    for (const Estudiante& estudiante : estudiantes) {
        vector<double> notasEstudiante;
        int cursosAprobados = 0;
        int cursosReprobados = 0;
        int creditosAcumulados = 0;

        for (const Nota& n : notas) {
            if (n.carnet == estudiante.carnet) {
                notasEstudiante.push_back(n.nota);

                if (n.nota >= 61.0) {
                    cursosAprobados++;
                    creditosAcumulados += obtenerCreditosCurso(n.codigo_curso);
                } else {
                    cursosReprobados++;
                }
            }
        }

        double promedio = calcularPromedio(notasEstudiante);
        string nombreCompleto = estudiante.nombre + " " + estudiante.apellido;

        archivo << "      <tr>\n";
        archivo << "        <td>" << estudiante.carnet << "</td>\n";
        archivo << "        <td>" << nombreCompleto << "</td>\n";
        archivo << "        <td>" << estudiante.carrera << "</td>\n";
        archivo << "        <td>" << estudiante.semestre << "</td>\n";
        archivo << "        <td>" << fixed << setprecision(2) << promedio << "</td>\n";
        archivo << "        <td>" << cursosAprobados << "</td>\n";
        archivo << "        <td>" << cursosReprobados << "</td>\n";
        archivo << "        <td>" << creditosAcumulados << "</td>\n";
        archivo << "      </tr>\n";
    }

    archivo << "    </tbody>\n";
    archivo << "  </table>\n";
    archivo << "</body>\n";
    archivo << "</html>\n";

    archivo.close();
    cout << "reporte html generado: reporte2_rendimiento_por_estudiante.html\n";
}

// estructura para estudiantes y promedios juntos
struct EstudianteConPromedio {
    Estudiante estudiante;
    double promedio;
};

// ordenadora de promedios descendente para el reporte 3
bool compararPorPromedio(const EstudianteConPromedio& a, const EstudianteConPromedio& b) {
    return a.promedio > b.promedio;
}

// reporte 3: top 10 mejores estudiantes (en html)
void generarReporteTop10EstudiantesHTML() {
    // validaciones
    if (estudiantes.empty() || notas.empty()) {
        cout << "primero carga estudiantes y notas para generar este reporte\n";
        return;
    }

    // creo vector de estudiantes con promedios
    vector<EstudianteConPromedio> top;

    for (const Estudiante& estudiante : estudiantes) {
        vector<double> notasEstudiante;

        for (const Nota& n : notas) {
            if (n.carnet == estudiante.carnet) {
                notasEstudiante.push_back(n.nota);
            }
        }

        double promedio = calcularPromedio(notasEstudiante);
        top.push_back({estudiante, promedio});
    }

    // ordeno por promedio descendente
    sort(top.begin(), top.end(), compararPorPromedio);

    // tomo los primeros 10
    int cantidad = min(10, static_cast<int>(top.size()));

    // abro archivo html
    ofstream archivo("../docs/reporte3_top10_estudiantes.html");

    if (!archivo.is_open()) {
        archivo.open("docs/reporte3_top10_estudiantes.html");
    }

    if (!archivo.is_open()) {
        cout << "error: no se pudo crear el archivo html del reporte 3\n";
        return;
    }

    // escribo el html
    archivo << "<!DOCTYPE html>\n";
    archivo << "<html lang=\"es\">\n";
    archivo << "<head>\n";
    archivo << "  <meta charset=\"UTF-8\">\n";
    archivo << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    archivo << "  <title>Reporte 3 - Top 10 Mejores Estudiantes</title>\n";
    archivo << "  <style>\n";
    archivo << "    body { font-family: Arial, sans-serif; margin: 24px; background: #f8f9fb; }\n";
    archivo << "    h1 { color: #1f2937; }\n";
    archivo << "    table { width: 100%; border-collapse: collapse; background: white; }\n";
    archivo << "    th, td { border: 1px solid #d1d5db; padding: 8px; text-align: left; }\n";
    archivo << "    th { background: #e5e7eb; }\n";
    archivo << "    tr:nth-child(even) { background: #f9fafb; }\n";
    archivo << "  </style>\n";
    archivo << "</head>\n";
    archivo << "<body>\n";
    archivo << "  <h1>Reporte 3: Top 10 Mejores Estudiantes</h1>\n";
    archivo << "  <table>\n";
    archivo << "    <thead>\n";
    archivo << "      <tr>\n";
    archivo << "        <th>Posicion</th>\n";
    archivo << "        <th>Carnet</th>\n";
    archivo << "        <th>Nombre Completo</th>\n";
    archivo << "        <th>Carrera</th>\n";
    archivo << "        <th>Semestre</th>\n";
    archivo << "        <th>Promedio</th>\n";
    archivo << "      </tr>\n";
    archivo << "    </thead>\n";
    archivo << "    <tbody>\n";

    // agrego una fila por cada estudiante del top 10
    for (int i = 0; i < cantidad; i++) {
        string nombreCompleto = top[i].estudiante.nombre + " " + top[i].estudiante.apellido;

        archivo << "      <tr>\n";
        archivo << "        <td>" << (i + 1) << "</td>\n";
        archivo << "        <td>" << top[i].estudiante.carnet << "</td>\n";
        archivo << "        <td>" << nombreCompleto << "</td>\n";
        archivo << "        <td>" << top[i].estudiante.carrera << "</td>\n";
        archivo << "        <td>" << top[i].estudiante.semestre << "</td>\n";
        archivo << "        <td>" << fixed << setprecision(2) << top[i].promedio << "</td>\n";
        archivo << "      </tr>\n";
    }

    archivo << "    </tbody>\n";
    archivo << "  </table>\n";
    archivo << "</body>\n";
    archivo << "</html>\n";

    archivo.close();
    cout << "reporte html generado: reporte3_top10_estudiantes.html\n";
}

// reporte 4: cursos con mayor porcentaje de reprobación (en html)
void generarReporteReprobacionHTML() {
    if (cursos.empty() || notas.empty()) {
        cout << "primero carga cursos y notas para generar este reporte\n";
        return;
    }

    struct ReprobacionCurso {
        Curso curso;
        int totalEstudiantes;
        int aprobados;
        int reprobados;
        double porcentajeReprobacion;
    };

    vector<ReprobacionCurso> cursoReprobacion;

    for (const Curso& curso : cursos) {
        set<int> estudiantesUnicos;
        int aprobados = 0;
        int reprobados = 0;

        for (const Nota& n : notas) {
            if (n.codigo_curso == curso.codigo) {
                estudiantesUnicos.insert(n.carnet);

                if (n.nota >= 61.0) {
                    aprobados++;
                } else {
                    reprobados++;
                }
            }
        }

        int totalEstudiantes = estudiantesUnicos.size();
        double porcentaje = 0.0;

        if (totalEstudiantes > 0) {
            porcentaje = (static_cast<double>(reprobados) / (aprobados + reprobados)) * 100.0;
        }

        cursoReprobacion.push_back({curso, totalEstudiantes, aprobados, reprobados, porcentaje});
    }

    sort(cursoReprobacion.begin(), cursoReprobacion.end(), 
         [](const ReprobacionCurso& a, const ReprobacionCurso& b) {
             return a.porcentajeReprobacion > b.porcentajeReprobacion;
         });

    ofstream archivo("../docs/reporte4_cursos_reprobacion.html");

    if (!archivo.is_open()) {
        archivo.open("docs/reporte4_cursos_reprobacion.html");
    }

    if (!archivo.is_open()) {
        cout << "error: no se pudo crear el archivo html del reporte 4\n";
        return;
    }

    archivo << "<!DOCTYPE html>\n";
    archivo << "<html lang=\"es\">\n";
    archivo << "<head>\n";
    archivo << "  <meta charset=\"UTF-8\">\n";
    archivo << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    archivo << "  <title>Reporte 4 - Cursos con Mayor Reprobacion</title>\n";
    archivo << "  <style>\n";
    archivo << "    body { font-family: Arial, sans-serif; margin: 24px; background: #f8f9fb; }\n";
    archivo << "    h1 { color: #1f2937; }\n";
    archivo << "    table { width: 100%; border-collapse: collapse; background: white; }\n";
    archivo << "    th, td { border: 1px solid #d1d5db; padding: 8px; text-align: left; }\n";
    archivo << "    th { background: #e5e7eb; }\n";
    archivo << "    tr:nth-child(even) { background: #f9fafb; }\n";
    archivo << "  </style>\n";
    archivo << "</head>\n";
    archivo << "<body>\n";
    archivo << "  <h1>Reporte 4: Cursos con Mayor Indice de Reprobacion</h1>\n";
    archivo << "  <table>\n";
    archivo << "    <thead>\n";
    archivo << "      <tr>\n";
    archivo << "        <th>Codigo</th>\n";
    archivo << "        <th>Curso</th>\n";
    archivo << "        <th>Total Estudiantes</th>\n";
    archivo << "        <th>Aprobados</th>\n";
    archivo << "        <th>Reprobados</th>\n";
    archivo << "        <th>Porcentaje Reprobacion</th>\n";
    archivo << "      </tr>\n";
    archivo << "    </thead>\n";
    archivo << "    <tbody>\n";

    for (const auto& cr : cursoReprobacion) {
        archivo << "      <tr>\n";
        archivo << "        <td>" << cr.curso.codigo << "</td>\n";
        archivo << "        <td>" << cr.curso.nombre << "</td>\n";
        archivo << "        <td>" << cr.totalEstudiantes << "</td>\n";
        archivo << "        <td>" << cr.aprobados << "</td>\n";
        archivo << "        <td>" << cr.reprobados << "</td>\n";
        archivo << "        <td>" << fixed << setprecision(2) << cr.porcentajeReprobacion << "%</td>\n";
        archivo << "      </tr>\n";
    }

    archivo << "    </tbody>\n";
    archivo << "  </table>\n";
    archivo << "</body>\n";
    archivo << "</html>\n";

    archivo.close();
    cout << "reporte html generado: reporte4_cursos_reprobacion.html\n";
}

// reporte 5: análisis por carrera (en html)
void generarReporteAnalisisCarreraHTML() {
    if (estudiantes.empty() || cursos.empty()) {
        cout << "primero carga estudiantes y cursos para generar este reporte\n";
        return;
    }

    set<string> carreras;
    for (const Estudiante& est : estudiantes) {
        carreras.insert(est.carrera);
    }

    ofstream archivo("../docs/reporte5_analisis_carrera.html");

    if (!archivo.is_open()) {
        archivo.open("docs/reporte5_analisis_carrera.html");
    }

    if (!archivo.is_open()) {
        cout << "error: no se pudo crear el archivo html del reporte 5\n";
        return;
    }

    archivo << "<!DOCTYPE html>\n";
    archivo << "<html lang=\"es\">\n";
    archivo << "<head>\n";
    archivo << "  <meta charset=\"UTF-8\">\n";
    archivo << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    archivo << "  <title>Reporte 5 - Analisis por Carrera</title>\n";
    archivo << "  <style>\n";
    archivo << "    body { font-family: Arial, sans-serif; margin: 24px; background: #f8f9fb; }\n";
    archivo << "    h1 { color: #1f2937; }\n";
    archivo << "    .carrera-section { margin-bottom: 30px; padding: 16px; background: white; border: 1px solid #d1d5db; }\n";
    archivo << "    .carrera-title { font-size: 18px; font-weight: bold; color: #374151; margin-bottom: 12px; }\n";
    archivo << "    .carrera-info { margin-bottom: 10px; }\n";
    archivo << "    table { width: 100%; border-collapse: collapse; }\n";
    archivo << "    th, td { border: 1px solid #d1d5db; padding: 8px; text-align: left; }\n";
    archivo << "    th { background: #e5e7eb; }\n";
    archivo << "  </style>\n";
    archivo << "</head>\n";
    archivo << "<body>\n";
    archivo << "  <h1>Reporte 5: Analisis por Carrera</h1>\n";

    for (const string& carrera : carreras) {
        int totalEstudiantes = 0;
        vector<double> promedios;
        map<int, int> estudiantesPorSemestre;

        for (const Estudiante& est : estudiantes) {
            if (est.carrera == carrera) {
                totalEstudiantes++;
                estudiantesPorSemestre[est.semestre]++;

                vector<double> notasEst;
                for (const Nota& n : notas) {
                    if (n.carnet == est.carnet) {
                        notasEst.push_back(n.nota);
                    }
                }
                double prom = calcularPromedio(notasEst);
                promedios.push_back(prom);
            }
        }

        int cursosPorCarrera = 0;
        for (const Curso& curso : cursos) {
            if (curso.carrera == carrera) {
                cursosPorCarrera++;
            }
        }

        double promedioCarrera = calcularPromedio(promedios);

        archivo << "  <div class=\"carrera-section\">\n";
        archivo << "    <div class=\"carrera-title\">" << carrera << "</div>\n";
        archivo << "    <div class=\"carrera-info\">Estudiantes totales: " << totalEstudiantes << "</div>\n";
        archivo << "    <div class=\"carrera-info\">Promedio general: " << fixed << setprecision(2) << promedioCarrera << "</div>\n";
        archivo << "    <div class=\"carrera-info\">Cursos disponibles: " << cursosPorCarrera << "</div>\n";
        archivo << "    <div class=\"carrera-info\">Distribucion por semestre:</div>\n";
        archivo << "    <table>\n";
        archivo << "      <thead>\n";
        archivo << "        <tr>\n";
        archivo << "          <th>Semestre</th>\n";
        archivo << "          <th>Cantidad de Estudiantes</th>\n";
        archivo << "        </tr>\n";
        archivo << "      </thead>\n";
        archivo << "      <tbody>\n";

        for (const auto& semestre : estudiantesPorSemestre) {
            archivo << "        <tr>\n";
            archivo << "          <td>" << semestre.first << "</td>\n";
            archivo << "          <td>" << semestre.second << "</td>\n";
            archivo << "        </tr>\n";
        }

        archivo << "      </tbody>\n";
        archivo << "    </table>\n";
        archivo << "  </div>\n";
    }

    archivo << "</body>\n";
    archivo << "</html>\n";

    archivo.close();
    cout << "reporte html generado: reporte5_analisis_carrera.html\n";
}

// función para cargar el archivo de estudiantes
void cargarEstudiantes(const string& ruta) {
    ifstream archivo(ruta);
    
    // verificamos si el archivo se abrió correctamente
    if (!archivo.is_open()) {
        cout << "error: no se pudo abrir el archivo de estudiantes" << endl;
        return;
    }
    
    string linea;
    int contador = 0;
    
    // leemos línea por línea del archivo
    while (getline(archivo, linea)) {
        // saltamos líneas vacías
        if (linea.empty()) continue;
        
        // separamos la línea por comas
        vector<string> datos = split(linea, ',');
        
        // verificamos que tengamos todos los campos
        if (datos.size() != 5) {
            cout << "advertencia: línea con formato incorrecto: " << linea << endl;
            continue;
        }
        
        // creamos un nuevo estudiante y llenamos sus datos
        Estudiante est;
        est.carnet = stoi(datos[0]);
        est.nombre = datos[1];
        est.apellido = datos[2];
        est.carrera = datos[3];
        est.semestre = stoi(datos[4]);
        
        // lo agregamos al vector
        estudiantes.push_back(est);
        contador++;
    }
    
    archivo.close();
    cout << "se cargaron " << contador << " estudiantes exitosamente!!" << endl;
}

// función para cargar el archivo de cursos
void cargarCursos(const string& ruta) {
    ifstream archivo(ruta);
    
    // verificamos si el archivo se abrió correctamente
    if (!archivo.is_open()) {
        cout << "error: no se pudo abrir el archivo de cursos" << endl;
        return;
    }
    
    string linea;
    int contador = 0;
    
    // leemos línea por línea del archivo
    while (getline(archivo, linea)) {
        // saltamos líneas vacías
        if (linea.empty()) continue;
        
        // separamos la línea por comas
        vector<string> datos = split(linea, ',');
        
        // verificamos que tengamos todos los campos
        if (datos.size() != 5) {
            cout << "advertencia: línea con formato incorrecto: " << linea << endl;
            continue;
        }
        
        // creamos un nuevo curso y llenamos sus datos
        Curso cur;
        cur.codigo = stoi(datos[0]);
        cur.nombre = datos[1];
        cur.creditos = stoi(datos[2]);
        cur.semestre = stoi(datos[3]);
        cur.carrera = datos[4];
        
        // lo agregamos al vector
        cursos.push_back(cur);
        contador++;
    }
    
    archivo.close();
    cout << "se cargaron " << contador << " cursos exitosamente!!" << endl;
}

// función para cargar el archivo de notas
void cargarNotas(const string& ruta) {
    ifstream archivo(ruta);
    
    // verificamos si el archivo se abrió correctamente
    if (!archivo.is_open()) {
        cout << "error: no se pudo abrir el archivo de notas" << endl;
        return;
    }
    
    string linea;
    int contador = 0;
    
    // leemos línea por línea del archivo
    while (getline(archivo, linea)) {
        // saltamos líneas vacías
        if (linea.empty()) continue;
        
        // separamos la línea por comas
        vector<string> datos = split(linea, ',');
        
        // verificamos que tengamos todos los campos
        if (datos.size() != 5) {
            cout << "advertencia: línea con formato incorrecto: " << linea << endl;
            continue;
        }
        
        // creamos una nueva nota y llenamos sus datos
        Nota n;
        n.carnet = stoi(datos[0]);
        n.codigo_curso = stoi(datos[1]);
        n.nota = stod(datos[2]);
        n.ciclo = datos[3];
        n.año = stoi(datos[4]);
        
        // lo agregamos al vector
        notas.push_back(n);
        contador++;
    }
    
    archivo.close();
    cout << "se cargaron " << contador << " notas exitosamente!!" << endl;
}

// función para mostrar el menú principal
void mostrarMenu() {
    cout << "\n==========================================\n";
    cout << "   SISTEMA DE ANÁLISIS ACADÉMICO\n";
    cout << "==========================================\n";
    cout << "1. Cargar archivo de estudiantes\n";
    cout << "2. Cargar archivo de cursos\n";
    cout << "3. Cargar archivo de notas\n";
    cout << "4. Generar Reporte: Estadísticas por Curso\n";
    cout << "5. Generar Reporte: Rendimiento por Estudiante\n";
    cout << "6. Generar Reporte: Top 10 Mejores Estudiantes\n";
    cout << "7. Generar Reporte: Cursos con Mayor Reprobación\n";
    cout << "8. Generar Reporte: Análisis por Carrera\n";
    cout << "9. Salir\n";
    cout << "==========================================\n";
    cout << "Seleccione una opción: ";
}

// función principal
int main() {
    cout << "Sistema de Análisis de Rendimiento Académico\n\n";
    
    int opcion = 0;
    
    // el menú se va a repetir hasta que el usuario presione 9 para salir
    do {
        mostrarMenu();
        cin >> opcion;
        cin.ignore(); // limpiamos el buffer del enter
        
        switch(opcion) {
            case 1:
                // cargamos el archivo de estudiantes desde la carpeta data
                cargarEstudiantes("../data/estudiantes.lfp");
                break;
            
            case 2:
                // cargamos el archivo de cursos desde la carpeta data
                cargarCursos("../data/cursos.lfp");
                break;
            
            case 3:
                // cargamos el archivo de notas desde la carpeta data
                cargarNotas("../data/notas.lfp");
                break;

            case 4:
                generarReporteEstadisticasPorCursoHTML();
                break;
            
            case 5:
                generarReporteRendimientoPorEstudianteHTML();
                break;
            
            case 6:
                generarReporteTop10EstudiantesHTML();
                break;
            
            case 7:
                generarReporteReprobacionHTML();
                break;
            
            case 8:
                generarReporteAnalisisCarreraHTML();
                break;
            
            case 9:
                cout << "cerrando, hasta luego! :)\n";
                break;
            
            default:
                cout << "opción no válida, intenta de nuevo\n";
        }
        
    } while(opcion != 9); // mientras no sea 9, seguimos en el menú
    
    return 0;
}