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
      vector<unsigned int> aux3(199, 0);

      for(int i=0; i < 199; i++){
        mapaSinSensor.push_back(aux);
        mapaContador.push_back(aux3);
      }
      
      vector<unsigned char> aux2(16, '?');

      for(int i=0; i < 16; i++){
        distancias.push_back(aux2);
      }

      hay_objetivo = zapatillas = bikini = false;
      pos_a_la_vista = bikini_a_la_vista = zapatillas_a_la_vista=  recarga_a_la_vista = -1;
      poca_bateria = false;
      posX_objetivo = posY_objetivo = -1;
      pos_objetivo = -1;
      cont_mov = -1;

      iteracion = 0;
      no_puede_ir = huir = va_por_minimo = false;
    }

    void rellenarMapa(bool sensor_posicion, Sensores sensores);
    int buscarObjetivo(Sensores sensores);
    int buscarObjetivoCerca(Sensores sensores);

    bool calcularCamino(Sensores sensores, int objetivo);

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
  int pos_a_la_vista, bikini_a_la_vista, zapatillas_a_la_vista, recarga_a_la_vista;
  bool poca_bateria;
  int posX_objetivo, posY_objetivo, pos_objetivo;
  int cont_mov;


  vector< vector< unsigned char> > distancias;
  vector< Action> accionesPendientes;
  Action ultimaAccion;
  int contador;

  bool no_puede_ir;
  bool huir;
  vector< vector< unsigned int> > mapaContador;

  bool va_por_minimo;

  int iteracion;

};

#endif
