#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"
using namespace std;

const int NORTE = 0;
const int SUR = 2;
const int ESTE = 1;
const int OESTE = 3;

class ComportamientoJugador : public Comportamiento{

  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size){
      // Constructor de la clase
      // Dar el valor inicial a las variables de estado
      fil = col = 99;
      
      brujula = 0;
      ultimaAccion = actIDLE;
      girar_derecha = false;
      bien_situado = false;
      
      vector<unsigned char> aux(199, '?');

      for(int i=0; i < 199; i++){
        mapaSinSensor.push_back(aux);
      }

      hay_objetivo = zapatillas = bikini = false;
    }

    void rellenarMapa(bool sensor_posicion, Sensores sensores);

    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);

  private:
  
  // Declarar aquí las variables de estado
  int fil, col, brujula;
  bool girar_derecha, bien_situado;
  vector< vector< unsigned char> > mapaSinSensor;
  bool hay_objetivo, zapatillas, bikini;

  vector< vector< int> > posicionesCerca;

  Action ultimaAccion;
};

#endif
