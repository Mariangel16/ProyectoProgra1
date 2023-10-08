#include "CD.h"
#include<iostream>

CD::CD(const std::string& nombre) : nombre(nombre), cantidadCanciones(0) {
}

std::string CD::getNombre() const {
    return nombre;
}

int CD::getCantidadCanciones() const {
    return cantidadCanciones; // Retorna la cantidad de canciones.
}

void CD::agregarCancion(const Cancion& cancion) {
    canciones.push_back(cancion);
    cantidadCanciones++; // Incrementa la cantidad de canciones
}

void CD::listarCanciones() const {
    std::cout << "Canciones en el CD '" << nombre << "':" << std::endl;
    for (const auto& cancion : canciones) {
        std::cout << "Nombre: " << cancion.getNombre() << " | Artista: " << cancion.getArtista()
            << " | Duracion: " << cancion.getDuracion() << " Minutos y segundos" << std::endl;
    }
}

const std::list<Cancion>& CD::getCanciones() const {
    return canciones;
}
