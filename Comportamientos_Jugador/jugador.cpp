#include "../Comportamientos_Jugador/jugador.hpp"
#include <iostream>
using namespace std;



Action ComportamientoJugador::think(Sensores sensores){

	Action accion = actIDLE;

	cout << "Posicion: fila " << sensores.posF << " columna " << sensores.posC << " ";
	switch(sensores.sentido){
		case 0: cout << "Norte" << endl; break;
		case 1: cout << "Este" << endl; break;
		case 2: cout << "Sur " << endl; break;
		case 3: cout << "Oeste" << endl; break;
	}
	cout << "Terreno: ";
	for (int i=0; i<sensores.terreno.size(); i++)
		cout << sensores.terreno[i];
	cout << endl;

	cout << "Superficie: ";
	for (int i=0; i<sensores.superficie.size(); i++)
		cout << sensores.superficie[i];
	cout << endl;

	cout << "Colisión: " << sensores.colision << endl;
	cout << "Reset: " << sensores.reset << endl;
	cout << "Vida: " << sensores.vida << endl;
	cout << endl;

	
	//Actualizar variables de estado
	switch(ultimaAccion){
		case actFORWARD:
			switch(brujula){
				case 0:
					fil --;
				break;
				case 1:
					col++;
				break;
				case 2:
					fil ++;
				break;
				case 3:
					col --;
				break;
			}

		break;

		case actTURN_L:
			brujula = (brujula + 3) % 4;
			girar_derecha = (rand() % 2) == 0;
		break;

		case actTURN_R:
			brujula = (brujula + 1) % 4;
			girar_derecha = (rand() % 2) == 0;
		break;
	}

	if(sensores.terreno[0] == 'G' && !bien_situado){
		bien_situado = true;
		fil = sensores.posF;
		col = sensores.posC;
	}
	
	if(bien_situado){
		mapaResultado[sensores.posF][sensores.posC] = sensores.terreno[0];
	}

	//Decidir qué acción tomar
	if((sensores.terreno[2] == 'T' || sensores.terreno[2] == 'S' || sensores.terreno[2] == 'G') && sensores.superficie[2] == '_') {
		accion = actFORWARD;
	}else{
		if(girar_derecha)
			accion = actTURN_R;
		else
			accion = actTURN_L;
	}

	ultimaAccion = accion;


	// Determinar el efecto de la ultima accion enviada
	return accion;
}

int ComportamientoJugador::interact(Action accion, int valor){
  return false;
}
