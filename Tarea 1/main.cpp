#include <iostream>
#include <string>

using namespace std;

class Videojuego {
private:
    // atributos :)
    string titulo;
    string genero;
    int horasJugadas;

public:
    // constructor 
    Videojuego(string t, string g, int h) {
        titulo = t;
        genero = g;
        horasJugadas = h;
    }

    // simulamos jugar
    void jugar(int horas) {
        horasJugadas += horas;
        cout << "Jugando " << titulo << "... jugaste " << horas << " horas bro" << endl;
    }

    // mostrar detalles
    void mostrarDetalles() {
        cout << "Detalles del Juego" << endl;
        cout << "Titulo: " << titulo << endl;
        cout << "Genero: " << genero << endl;
        cout << "Total horas jugadas: " << horasJugadas << endl;
        cout << "" << endl;
    }
};

int main() {
    // creamos dos juegos
    Videojuego juego1("Honkai: Star Rail", "Gacha RPG por turnos", 100000);
    Videojuego juego2("Minecraft", "Sandbox", 400000);

    // testeamos metodos
    juego1.mostrarDetalles();
    juego1.jugar(2); // jugamos dos horas
    juego1.mostrarDetalles(); // mostramos los detalles

    cout << endl; // enter

    juego2.mostrarDetalles();
    juego2.jugar(5);
    juego2.mostrarDetalles();
    
    return 0;
}