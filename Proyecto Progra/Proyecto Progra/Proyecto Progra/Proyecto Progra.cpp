#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <queue>
#include <algorithm>
#include <functional>
#include <set>
#include <sys/stat.h>
#include "Cancion.h"
#include "CD.h"
#include <dirent.h>

void mostrarArchivosEnCarpeta(const std::string& carpeta) {
    DIR* dir;
    struct dirent* entry;

    std::cout << "Archivos en la carpeta:" << std::endl;

    if ((dir = opendir(carpeta.c_str())) != nullptr) {
        while ((entry = readdir(dir)) != nullptr) {
            std::string nombreArchivo = entry->d_name;
            if (nombreArchivo.size() > 4 && nombreArchivo.substr(nombreArchivo.size() - 4) == ".txt") {
                std::cout << nombreArchivo << std::endl;
            }
        }
        closedir(dir);
    }
}

void cargarRespaldos(const std::string& carpeta, std::list<CD>& listaCDs, std::queue<Cancion>& colaReproduccion, Cancion& cancionActual) {
    // Limpia la lista de CDs existente
    listaCDs.clear();

    DIR* dir;
    struct dirent* entry;

    if ((dir = opendir(carpeta.c_str())) != nullptr) {
        while ((entry = readdir(dir)) != nullptr) {
            std::string nombreArchivo = entry->d_name;
            // Verifica si el archivo tiene extensión .txt
            if (nombreArchivo.size() > 4 && nombreArchivo.substr(nombreArchivo.size() - 4) == ".txt") {
                std::ifstream archivoRespaldo(carpeta + "/" + nombreArchivo);
                if (archivoRespaldo.is_open()) {
                    CD cd(nombreArchivo.substr(0, nombreArchivo.size() - 4)); // Crea un nuevo CD con el nombre del archivo (sin extensión)
                    bool errorEnArchivo = false;
                    std::set<std::string> cancionesUnicas; // Conjunto para canciones únicas

                    std::string linea;
                    while (std::getline(archivoRespaldo, linea)) {
                        // Analiza la línea para obtener nombre, artista y duración
                        std::string nombre, artista;
                        int duracion = 0;
                        size_t coma1 = linea.find("||");
                        size_t coma2 = linea.find("||", coma1 + 2);

                        if (coma1 != std::string::npos && coma2 != std::string::npos) {
                            nombre = linea.substr(0, coma1);
                            artista = linea.substr(coma1 + 2, coma2 - coma1 - 2);
                            duracion = std::stoi(linea.substr(coma2 + 2));

                            // Verificar si la canción ya existe
                            if (cancionesUnicas.find(nombre) != cancionesUnicas.end()) {
                                std::cerr << "Error en el archivo '" << nombreArchivo << "': Cancion duplicada." << std::endl;
                                errorEnArchivo = true;
                            }
                            else {
                                cancionesUnicas.insert(nombre); // Agregar la canción al conjunto
                                cd.agregarCancion(Cancion(nombre, artista, duracion));
                            }
                        }
                        else {
                            std::cerr << "Error en el archivo '" << nombreArchivo << "': linea con formato incorrecto." << std::endl;
                            errorEnArchivo = true;
                        }
                    }

                    if (!errorEnArchivo) {
                        listaCDs.push_back(cd); // Agrega el CD a la lista si no hubo errores
                        std::cout << "CD '" << cd.getNombre() << "' cargado correctamente." << std::endl;
                    }

                    archivoRespaldo.close();
                }
                else {
                    std::cerr << "Error al abrir el archivo '" << nombreArchivo << "'." << std::endl;
                }
            }
        }
        closedir(dir);
    }
    else {
        std::cerr << "Error al abrir la carpeta '" << carpeta << "'." << std::endl;
    }
    std::cout << "CDs cargados correctamente desde archivos .txt en la carpeta." << std::endl;
}

// Función para agregar una canción a la cola de reproducción
void agregarCancionAReproduccion(const CD& cd, std::queue<Cancion>& colaReproduccion) {
    std::list<Cancion> cancionesCD = cd.getCanciones();

    if (!cancionesCD.empty()) {
        std::cout << "Selecciona una cancion del CD '" << cd.getNombre() << "':" << std::endl;

        int i = 1;
        for (const auto& cancion : cancionesCD) {
            std::cout << i << ". " << cancion.getNombre() << " - " << cancion.getArtista() << std::endl;
            i++;
        }

        int opcion;
        std::cin >> opcion;

        if (opcion >= 1 && opcion <= cancionesCD.size()) {
            auto it = cancionesCD.begin();
            std::advance(it, opcion - 1);
            colaReproduccion.push(*it);
            std::cout << "Cancion agregada a la cola de reproduccion." << std::endl;
        }
        else {
            std::cout << "Opción no valida." << std::endl;
        }
    }
    else {
        std::cout << "El CD no tiene canciones." << std::endl;
    }
}


void agregarCancionAReproduccion(const Cancion& cancion, std::queue<Cancion>& colaReproduccion) {
    //  agregar la canción a la cola de reproducción
    colaReproduccion.push(cancion);
    std::cout << "Cancion agregada a la cola de reproduccion." << std::endl;
}


// Función para limpiar la Lista de CDs, la cola de reproducción y la reproducción actual
void limpiarTodo(std::list<CD>& listaCDs, std::queue<Cancion>& colaReproduccion, Cancion& cancionActual) {
    listaCDs.clear();  // Limpia la lista de CDs
    while (!colaReproduccion.empty()) {
        colaReproduccion.pop();  // Vacíar la cola de reproducción
    }
    cancionActual = Cancion("", "", 0);// Limpia la reproducción actual
}

// Función para convertir la duración de segundos a un formato legible (minutos y segundos)
std::string formatoDuracion(int duracion) {
    int minutos = duracion / 60;
    int segundos = duracion % 60;

    std::string formatoMinutos = std::to_string(minutos);
    std::string formatoSegundos = segundos < 10 ? "0" + std::to_string(segundos) : std::to_string(segundos);


    return formatoMinutos + ":" + formatoSegundos;
}

// Función para ver la cola de reproducción
void verColaReproduccion(const std::queue<Cancion>& colaReproduccion) {
    if (colaReproduccion.empty()) {
        std::cout << "La cola de reproduccion esta vacia." << std::endl;
    }
    else {
        std::cout << "Cola de reproduccion:" << std::endl;
        std::queue<Cancion> copiaCola = colaReproduccion; // Copia la cola para no vaciarla
        while (!copiaCola.empty()) {
            const Cancion& cancion = copiaCola.front();
            std::cout << "Nombre: " << cancion.getNombre() << " | Artista: " << cancion.getArtista()
                << " | Duracion: " << formatoDuracion(cancion.getDuracion()) << std::endl; // Utiliza formatoDuracion
            copiaCola.pop();
        }
    }
}


void reproduccionActual(const Cancion& cancionActual) {
    if (cancionActual.getNombre() != "") {
        std::cout << "Reproduciendo:" << std::endl;
        std::cout << "Nombre: " << cancionActual.getNombre() << " | Artista: " << cancionActual.getArtista()
            << " | Duracion: " << formatoDuracion(cancionActual.getDuracion()) << std::endl;
    }
    else {
        std::cout << "Reproduccion en pausa." << std::endl;
    }
}

// Función para reproducir la siguiente canción en la cola
void reproducirSiguiente(std::queue<Cancion>& colaReproduccion, Cancion& cancionActual) {
    if (!colaReproduccion.empty()) {
        cancionActual = colaReproduccion.front();
        colaReproduccion.pop();
        std::cout << "Reproduciendo siguiente cancion..." << std::endl;
    }
    else {
        std::cout << "La cola de reproduccion esta vacia." << std::endl;
    }
}

// Función para ordenar la cola de reproducción
void ordenarCola(std::queue<Cancion>& colaReproduccion, const std::string& criterio, bool ascendente) {
    std::vector<Cancion> cancionesOrdenadas;

    // Vaciar la cola 
    while (!colaReproduccion.empty()) {
        cancionesOrdenadas.push_back(colaReproduccion.front());
        colaReproduccion.pop();
    }

    // Definir la función de comparación en función del criterio seleccionado
    std::function<bool(const Cancion&, const Cancion&)> comparador;

    if (criterio == "nombre") {
        comparador = [](const Cancion& a, const Cancion& b) {
            return a.getNombre() < b.getNombre();
            };
    }
    else if (criterio == "artista") {
        comparador = [](const Cancion& a, const Cancion& b) {
            return a.getArtista() < b.getArtista();
            };
    }
    else if (criterio == "duracion") {
        comparador = [](const Cancion& a, const Cancion& b) {
            return a.getDuracion() < b.getDuracion();
            };
    }
    else {
        std::cout << "Criterio de orden no valido." << std::endl;
        return;
    }

    // Ordenar el vector de canciones
    if (ascendente) {
        std::sort(cancionesOrdenadas.begin(), cancionesOrdenadas.end(), comparador);
    }
    else {
        // Usar una función lambda inversa para el orden descendente
        std::sort(cancionesOrdenadas.begin(), cancionesOrdenadas.end(), [&](const Cancion& a, const Cancion& b) {
            return !comparador(a, b);
            });
    }


    // Llenar la cola ordenada
    for (const Cancion& cancion : cancionesOrdenadas) {
        colaReproduccion.push(cancion);
    }
}

int main() {
    std::list<CD> listaCDs;
    std::queue<Cancion> colaReproduccion;
    Cancion cancionActual("", "", 0);
    int opcion;
    std::string carpeta;
    bool carpetaCargada = false;

    do {

        // Mostrar el menú
        std::cout << "Menu:" << std::endl;
        std::cout << "1. Cargar respaldos desde carpeta" << std::endl;
        std::cout << "2. Agregar Cancion" << std::endl;
        std::cout << "3. Ver Cola de Reproduccion" << std::endl;
        std::cout << "4. Reproduccion Actual" << std::endl;
        std::cout << "5. Reproducir Siguiente" << std::endl;
        std::cout << "6. Ordenar Cola de Reproduccion" << std::endl;
        std::cout << "7. Salir" << std::endl;
        std::cout << "Seleccione una opcion: ";

        // Leer la opción del usuario
        std::cin >> opcion;

        // Validar la opción ingresada
        if (opcion < 1 || opcion > 7) {
            std::cout << "Opcion no valida. Intente de nuevo." << std::endl;
        }
        else {
            switch (opcion) {
            case 1:
                // Cargar respaldos desde carpeta
                if (!carpetaCargada) {
                    std::cout << "Ingrese la ruta de la carpeta: ";
                    std::cin >> carpeta;

                    // Validar si la carpeta existe
                    struct stat info;
                    if (stat(carpeta.c_str(), &info) == 0 && S_ISDIR(info.st_mode)) {
                        mostrarArchivosEnCarpeta(carpeta); // Mostrar archivos .txt en la carpeta
                        cargarRespaldos(carpeta, listaCDs, colaReproduccion, cancionActual);
                        carpetaCargada = true;
                    }
                    else {
                        std::cerr << "La carpeta ingresada no existe o no es valida." << std::endl;
                    }
                }
                else {
                    std::cout << "La carpeta ya ha sido cargada." << std::endl;
                }
            case 2:
                // Agregar Cancion
                if (carpetaCargada) { 
                    // Mostrar lista de CDs disponibles
                    int i = 1;
                    for (const CD& cd : listaCDs) {
                        std::cout << i << ". " << cd.getNombre() << std::endl;
                        i++;
                    }
                    int opcionCD;
                    std::cout << "Seleccione un CD: ";
                    std::cin >> opcionCD;
                    if (opcionCD >= 1 && opcionCD <= listaCDs.size()) {
                        CD& cdSeleccionado = *std::next(listaCDs.begin(), opcionCD - 1);

                        // Mostrar canciones del CD seleccionado
                        cdSeleccionado.listarCanciones();

                        int opcionCancion;
                        std::cout << "Seleccione una cancion: ";
                        std::cin >> opcionCancion;
                        if (opcionCancion >= 1 && opcionCancion <= cdSeleccionado.getCanciones().size()) {
                            Cancion cancion = *std::next(cdSeleccionado.getCanciones().begin(), opcionCancion - 1);

                            // Llamada a la función para agregar la canción a la cola
                            agregarCancionAReproduccion(cancion, colaReproduccion);
                        }
                        else {
                            std::cout << "Opcion no valida." << std::endl;
                        }
                    }
                    else {
                        std::cout << "Opcion no valida." << std::endl;
                    }
                }
                else {
                    std::cout << "Primero cargue respaldos desde una carpeta." << std::endl;
                }
                break;
            case 3:
                // Ver Cola de Reproduccion
                if (carpetaCargada) { // Solo permite ver la cola si la carpeta ha sido cargada
                    verColaReproduccion(colaReproduccion);
                }
                else {
                    std::cout << "Primero cargue respaldos desde una carpeta." << std::endl;
                }
                break;
            case 4:
                // Reproduccion Actual
                reproduccionActual(cancionActual);
                break;
            case 5:
                // Reproducir Siguiente
                reproducirSiguiente(colaReproduccion, cancionActual);
                break;
            case 6:
                // Ordenar Cola de Reproduccion
                if (carpetaCargada) { // Solo permite ordenar la cola si la carpeta ha sido cargada
                    if (!colaReproduccion.empty()) {
                        std::cout << "Seleccione un criterio de ordenamiento:" << std::endl;
                        std::cout << "1. Nombre del artista (ascendente)" << std::endl;
                        std::cout << "2. Nombre del artista (descendente)" << std::endl;
                        std::cout << "3. Nombre de la cancion (ascendente)" << std::endl;
                        std::cout << "4. Nombre de la cancion (descendente)" << std::endl;
                        std::cout << "5. Duracion (ascendente)" << std::endl;
                        std::cout << "6. Duracion (descendente)" << std::endl;

                        int opcionOrden;
                        std::cin >> opcionOrden;

                        switch (opcionOrden) {
                        case 1:
                            ordenarCola(colaReproduccion, "artista", true);
                            break;
                        case 2:
                            ordenarCola(colaReproduccion, "artista", false);
                            break;
                        case 3:
                            ordenarCola(colaReproduccion, "nombre", true);
                            break;
                        case 4:
                            ordenarCola(colaReproduccion, "nombre", false);
                            break;
                        case 5:
                            ordenarCola(colaReproduccion, "duracion", true);
                            break;
                        case 6:
                            ordenarCola(colaReproduccion, "duracion", false);
                            break;
                        default:
                            std::cout << "Opcion no valida." << std::endl;
                            break;
                        }
                    }
                    else {
                        std::cout << "La cola de reproduccion esta vacia." << std::endl;
                    }
                }
                else {
                    std::cout << "Primero cargue respaldos desde una carpeta." << std::endl;
                }
                break;
            case 7:
                // Salir
                limpiarTodo(listaCDs, colaReproduccion, cancionActual);
                std::cout << "Saliendo del programa." << std::endl;
                break;
            }
        }
    } while (opcion != 7);

    return 0;
}



