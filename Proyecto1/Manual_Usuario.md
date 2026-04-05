# Manual de Usuario - MedLexer

## 1. Que hace este programa
MedLexer sirve para cargar archivos `.med`, analizarlos y generar reportes HTML.
En resumen: tu escribes datos de hospital (pacientes, medicos, citas, diagnosticos) y la app te devuelve tablas ya ordenadas.

## 2. Que necesitas para usarlo
- Windows 10/11
- Qt6 + entorno MSYS2 UCRT64 (si lo vas a compilar tu)
- CMake y compilador g++

Si solo vas a ejecutar el `.exe` en tu maquina ya configurada, no necesitas tocar mucho mas.

## 3. Como compilar y ejecutar

### Opcion A: compilar y correr
En PowerShell, dentro de `Proyecto1`:

1. `cmake -S . -B build`
2. `cmake --build build --config Release`
3. `./medlexer.exe`

Si te da problema de librerias Qt, corre antes:

`$env:PATH = "C:\msys64\ucrt64\bin;$env:PATH"`

### Opcion B: solo ejecutar
Si ya esta compilado:

`./medlexer.exe`

## 4. Como usar la interfaz
1. Click en **Cargar Archivo** y selecciona un `.med`.
2. Click en **Analizar** para ver tokens y errores.
3. Click en **Generar Reportes** para crear los HTML en carpeta `docs`.

La tabla de abajo muestra errores lexicos y tambien errores semanticos (por ejemplo fecha/hora invalida dentro de comillas).

## 5. Formato esperado del archivo
Ejemplo base:

HOSPITAL {
}

PACIENTES
Paciente {
   codigo: "PAC-001"
   nombre: "Ana Lopez"
   nacimiento: "1990-05-15"
   sangre: "O+"
}

MEDICOS
Medico {
   codigo: "MED-001"
   nombre: "Dr. Ruiz"
   especialidad: "CARDIOLOGIA"
}

CITAS
Cita {
   codigo: "CIT-001"
   paciente: "PAC-001"
   medico: "MED-001"
   fecha: "2026-04-10"
   hora: "09:30"
}

DIAGNOSTICOS
Diagnostico {
   cita: "CIT-001"
   detalle: "Control general"
}

## 6. Errores comunes (rapido)
- Si no abre el exe: revisa PATH de Qt/MSYS2.
- Si un dato no aparece en reportes: revisa formato y rangos de fecha/hora.
- Si hay errores en tabla inferior: corrige eso primero y vuelve a generar reportes.