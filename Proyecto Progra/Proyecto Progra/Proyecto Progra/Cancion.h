#ifndef CANCION_H
#define CANCION_H

#include <string>

class Cancion {
public:
    Cancion(const std::string& nombre, const std::string& artista, int duracion);

    std::string getNombre() const;
    std::string getArtista() const;
    int getDuracion() const;

private:
    std::string nombre;
    std::string artista;
    int duracion;
};

#endif  // CANCION_H
