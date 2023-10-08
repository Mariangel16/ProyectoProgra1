#ifndef CD_H
#define CD_H

#include <string>
#include <list>
#include "Cancion.h"

class CD {
public:
    CD(const std::string& nombre);

    void agregarCancion(const Cancion& cancion);
    void listarCanciones() const;
    const std::list<Cancion>& getCanciones() const;
    std::string getNombre() const;
    int getCantidadCanciones() const; // obtener la cantidad de canciones.

private:
    std::string nombre;
    std::list<Cancion> canciones;
    int cantidadCanciones; // Variable para almacenar la cantidad de canciones en el CD.
};

#endif  // CD_H
