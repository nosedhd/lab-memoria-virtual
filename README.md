# Laboratorio de Memoria Virtual

Simulador de gestión de memoria virtual basado en paginación de dos niveles y
política de reemplazo FIFO.

## Requisitos

- `g++` con soporte para C++17.
- `make` o `mingw32-make`.
- Windows: MSYS2/MinGW-w64 o una instalación equivalente de MinGW.

## Comandos del Makefile

En Windows, abrir PowerShell o una terminal con MinGW y ejecutar los comandos
desde la raíz del proyecto.

### Compilar

```powershell
mingw32-make
```

Compila el simulador y genera `simulador.exe`.

También puede utilizarse el target explícito:

```powershell
mingw32-make all
```

### Ejecutar el simulador

```powershell
mingw32-make run ARGS=tests/test1_basico.txt
```

Otros archivos de prueba disponibles:

```powershell
mingw32-make run ARGS=tests/test2_fallos.txt
mingw32-make run ARGS=tests/test3_reemplazo.txt
```

El target `run` compila el proyecto si es necesario y luego ejecuta el
simulador con el archivo indicado en `ARGS`.

### Ejecutar las pruebas unitarias

```powershell
mingw32-make test
```

Este comando compila y ejecuta las pruebas unitarias de:

- `tests/unit/test_memoryconfig.cpp`
- `tests/unit/test_virtualaddress.cpp`

Si todas pasan, se muestra:

```text
Todos los tests pasaron.
```

El archivo `tests/unit/test_mmu.cpp` todavía no se incluye porque está en
desarrollo.

### Limpiar archivos compilados

```powershell
mingw32-make clean
```

Elimina los objetos compilados, el directorio `build/` y `simulador.exe`.

## Estructura principal

```text
include/   Headers del proyecto
src/       Código fuente
tests/     Archivos de entrada y pruebas unitarias
build/     Objetos compilados
bin/       Ejecutables de las pruebas unitarias
```

## Compilación manual de una prueba

Si no se dispone de `make`, las pruebas pueden compilarse directamente:

```powershell
g++ -std=c++17 -Wall -Wextra -Werror -Iinclude `
	tests/unit/test_memoryconfig.cpp `
	src/domain/config/MemoryConfig.cpp `
	src/domain/paging/VirtualAdress.cpp `
	-o test_memoryconfig.exe

.\test_memoryconfig.exe
```
