# Tarea 2 - Sistemas Operativos 2025-2

## Descripción

Código relacionado con la tarea 2 del curso **Sistemas Operativos 2025-2**.

Corresponde a la implementación de _Sincronización con Barrera Reutilizable_ y a un _Simulador de
Traducción de Direcciones_.

## Uso

### Requisitos

Programas requeridos por el repositorio:

- gcc
- git
- GNU make

> [!NOTE]
> Comandos para instalar dependencias:
> - Ubuntu/Debian: `sudo apt install gcc git make`
> - Fedora/RHEL: `sudo dnf install gcc git make`

### Comandos

Utiliza estos comandos para obtener el código, compilarlo y ejecutarlo:

```sh
# Clonar repo
git clone https://github.com/krsconnie/SO2

# Acceder a repo y compilar
cd SO2
make

# Testear barrier y simulador*
./barrier 5 10
./run_sim_tests.sh
```

\* El script que testea el simulador retorna el archivo `resultados_generales.csv`, donde se puede
comprobar el rendimiento de este.
