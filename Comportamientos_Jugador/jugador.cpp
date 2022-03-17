#include "../Comportamientos_Jugador/jugador.hpp"
#include <iostream>
using namespace std;

Action ComportamientoJugador::think(Sensores sensores)
{

	Action accion = actIDLE;

	cout << "Posicion: fila " << sensores.posF << " columna " << sensores.posC << " ";
	switch (sensores.sentido)
	{
	case 0:
		cout << "Norte" << endl;
		break;
	case 1:
		cout << "Este" << endl;
		break;
	case 2:
		cout << "Sur " << endl;
		break;
	case 3:
		cout << "Oeste" << endl;
		break;
	}
	cout << "Terreno: ";
	for (int i = 0; i < sensores.terreno.size(); i++)
		cout << sensores.terreno[i];
	cout << endl;

	cout << "Superficie: ";
	for (int i = 0; i < sensores.superficie.size(); i++)
		cout << sensores.superficie[i];
	cout << endl;

	cout << "Colisión: " << sensores.colision << endl;
	cout << "Reset: " << sensores.reset << endl;
	cout << "Vida: " << sensores.vida << endl;
	cout << "Objetivo: " << pos_objetivo<< endl;
	cout << "POS OBJETIVO: " << pos_a_la_vista << endl;
	cout << endl;

	//Actualizar variables de estado
	int cont = -1;

	switch (ultimaAccion)
	{
	case actFORWARD:
		switch (brujula)
		{
		case NORTE:
			fil--;
			break;
		case ESTE:
			col++;
			break;
		case SUR:
			fil++;
			break;
		case OESTE:
			col--;
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
	default:
	break;
	}

	if (sensores.nivel == 0 || sensores.nivel == 1)
	{
		switch (sensores.sentido)
		{
		case norte:
			brujula = NORTE;
			break;

		case sur:
			brujula = SUR;
			break;

		case este:
			brujula = ESTE;
			break;

		case oeste:
			brujula = OESTE;
			break;
		}
	}

	if (sensores.nivel == 0)
	{
		bien_situado = true;
	}

	if (sensores.terreno[0] == 'G' && !bien_situado)
	{
		bien_situado = true;

		//Rellenar mapaResultado con mapaSinSensor
		for (int i = 0; i < mapaResultado.size(); i++)
		{
			for (int j = 0; j < mapaResultado.size(); j++)
			{
				if (mapaSinSensor[fil - sensores.posF + i][col - sensores.posC + j] != '?')
				{
					mapaResultado[i][j] = mapaSinSensor[fil - sensores.posF + i][col - sensores.posC + j];
				}
			}
		}

		fil = sensores.posF;
		col = sensores.posC;
	}

	rellenarMapa(bien_situado, sensores);

	poca_bateria = sensores.bateria <= 500;

	//Decidir qué acción tomar

	if (!hay_objetivo)
	{
		/*
		posicionesCerca[3][3] = sensores.terreno[0];
		switch (brujula)
		{
			int cont;
		case NORTE:
			cont = 2;
			for (int k = 1; k < 4; k++)
			{
				posicionesCerca[2][cont] = sensores.terreno[k];
				cont++;
			}

			cont = 1;
			for (int k = 4; k < 9; k++)
			{
				posicionesCerca[1][cont] = sensores.terreno[k];
				cont++;
			}

			cont = 0;
			for (int k = 9; k < 16; k++)
			{
				posicionesCerca[0][cont] = sensores.terreno[k];
				cont++;
			}
			break;
		case SUR:

			for (int k = 1; k < 4; k++)
			{
			}
			break;
		case ESTE:
			break;
		case OESTE:
			break;
		}
		*/

		pos_a_la_vista = bikini_a_la_vista = zapatillas_a_la_vista = recarga_a_la_vista = -1;
		posX_objetivo = posY_objetivo = -1;

		int k_objetivo = -1;

		for (int k = 15; k > 0; k--)
		{
			switch (sensores.terreno[k])
			{
			case 'G':
				pos_a_la_vista = k;
				break;

			case 'K':
				bikini_a_la_vista = k;
				break;

			case 'D':
				zapatillas_a_la_vista = k;
				break;

			case 'X':
				recarga_a_la_vista = k;
				break;
			default:
			break;
			}
			
		}

		if (pos_a_la_vista != -1 && !bien_situado)
		{
			k_objetivo = pos_a_la_vista;
			hay_objetivo = true;
		}
		else if (recarga_a_la_vista != -1 && poca_bateria)
		{
			k_objetivo = recarga_a_la_vista;
			hay_objetivo = true;
		}
		else if (bikini_a_la_vista != -1 && !bikini)
		{
			k_objetivo = bikini_a_la_vista;
			hay_objetivo = true;
		}
		else if (zapatillas_a_la_vista != -1 && !zapatillas)
		{
			k_objetivo = zapatillas_a_la_vista;
			hay_objetivo = true;
		}
		/*
			switch (brujula)
			{
			case NORTE:
				switch (k_objetivo)
				{
				case 1:
				case 2:
				case 3:
					posX_objetivo = fil - 1;
					posY_objetivo = col - 2 + k_objetivo;
					break;

				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
					posX_objetivo = fil - 2;
					posY_objetivo = col - 6 + k_objetivo;
					break;

				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
				case 14:
				case 15:
					posX_objetivo = fil - 3;
					posY_objetivo = col - 12 + k_objetivo;
					break;
				}
				break;
			case SUR:
				switch (k_objetivo)
				{
				case 1:
				case 2:
				case 3:
					posX_objetivo = fil + 1;
					posY_objetivo = col + 2 - k_objetivo;
					break;

				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
					posX_objetivo = fil + 2;
					posY_objetivo = col + 6 - k_objetivo;
					break;

				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
				case 14:
				case 15:
					posX_objetivo = fil + 3;
					posY_objetivo = col + 12 - k_objetivo;
					break;
				}
				break;
			case ESTE:
				switch (k_objetivo)
				{
				case 1:
				case 2:
				case 3:
					posX_objetivo = fil - 2 + k_objetivo;
					posY_objetivo = col + 1;
					break;

				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
					posX_objetivo = fil - 6 + k_objetivo;
					posY_objetivo = col + 2;
					break;

				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
				case 14:
				case 15:
					posX_objetivo = fil - 12 + k_objetivo;
					posY_objetivo = col + 3;
					break;
				}

			case OESTE:
				switch (k_objetivo)
				{
				case 1:
				case 2:
				case 3:
					posX_objetivo = fil + 2 - k_objetivo;
					posY_objetivo = col - 1;
					break;

				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
					posX_objetivo = fil + 6 - k_objetivo;
					posY_objetivo = col - 2;
					break;

				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
				case 14:
				case 15:
					posX_objetivo = fil + 12 - k_objetivo;
					posY_objetivo = col - 3;
					break;
				}
				break;
			}
			*/
		pos_objetivo = k_objetivo;
		switch(pos_a_la_vista){
			case 1:
				hay_objetivo = true;
				accionesPendientes.push_back(actFORWARD);
				accionesPendientes.push_back(actTURN_L);
				accionesPendientes.push_back(actFORWARD);
			break;
			case 2:
				accionesPendientes.push_back(actFORWARD);
			break;
			case 3:
				hay_objetivo = true;
				accionesPendientes.push_back(actFORWARD);
				accionesPendientes.push_back(actTURN_R);
				accionesPendientes.push_back(actFORWARD);
			break;
			case 4:
			break;
			case 5:
			break;
			case 6:
			break;
			case 7:
			break;
			case 8:
			break;
			case 9:
			break;
			case 10:
			break;
			case 11:
			break;
			case 12:
			break;
			case 13:
			break;
			case 14:
			break;
			case 15:
			break;
		}
		accion = actIDLE;

	}else{
		if(accionesPendientes.size() > 1){
			accion = accionesPendientes[accionesPendientes.size() - 1];
			accionesPendientes.pop_back();
		}else{
			hay_objetivo = false;
			accion = actFORWARD;
		}
	}
/*
	if ((sensores.terreno[2] == 'T' || sensores.terreno[2] == 'S' || sensores.terreno[2] == 'G') && sensores.superficie[2] == '_')
	{
		accion = actFORWARD;
	}
	else
	{
		if (girar_derecha)
			accion = actTURN_R;
		else
			accion = actTURN_L;
	}
*/
	ultimaAccion = accion;

	// Determinar el efecto de la ultima accion enviada
	return accion;
}

void ComportamientoJugador::rellenarMapa(bool sensor_posicion, Sensores sensores)
{
	int cont;

	if (!sensor_posicion)
	{
		if (mapaSinSensor[fil][col] == '?')
			mapaSinSensor[fil][col] = sensores.terreno[0];

		switch (brujula)
		{
		case NORTE:
			cont = -1;

			for (int k = 1; k < 4; k++)
			{
				if (mapaSinSensor[fil - 1][col + cont] == '?')
					mapaSinSensor[fil - 1][col + cont] = sensores.terreno[k];
				cont++;
			}

			cont = -2;

			for (int k = 4; k < 9; k++)
			{
				if (mapaSinSensor[fil - 2][col + cont] == '?')
					mapaSinSensor[fil - 2][col + cont] = sensores.terreno[k];
				cont++;
			}

			cont = -3;

			for (int k = 9; k < 16; k++)
			{
				if (mapaSinSensor[fil - 3][col + cont] == '?')
					mapaSinSensor[fil - 3][col + cont] = sensores.terreno[k];
				cont++;
			}
			break;

		case SUR:
			cont = 1;
			for (int k = 1; k < 4; k++)
			{
				if (mapaSinSensor[fil + 1][col + cont] == '?')
					mapaSinSensor[fil + 1][col + cont] = sensores.terreno[k];
				cont--;
			}

			cont = 2;

			for (int k = 4; k < 9; k++)
			{
				if (mapaSinSensor[fil + 2][col + cont] == '?')
					mapaSinSensor[fil + 2][col + cont] = sensores.terreno[k];
				cont--;
			}

			cont = 3;

			for (int k = 9; k < 16; k++)
			{
				if (mapaSinSensor[fil + 3][col + cont] == '?')
					mapaSinSensor[fil + 3][col + cont] = sensores.terreno[k];
				cont--;
			}
			break;

		case ESTE:
			cont = -1;
			for (int k = 1; k < 4; k++)
			{
				if (mapaSinSensor[fil + cont][col + 1] == '?')
					mapaSinSensor[fil + cont][col + 1] = sensores.terreno[k];
				cont++;
			}

			cont = -2;

			for (int k = 4; k < 9; k++)
			{
				if (mapaSinSensor[fil + cont][col + 2] == '?')
					mapaSinSensor[fil + cont][col + 2] = sensores.terreno[k];
				cont++;
			}

			cont = -3;

			for (int k = 9; k < 16; k++)
			{
				if (mapaSinSensor[fil + cont][col + 3] == '?')
					mapaSinSensor[fil + cont][col + 3] = sensores.terreno[k];
				cont++;
			}

			break;

		case OESTE:
			cont = 1;

			for (int k = 1; k < 4; k++)
			{
				if (mapaSinSensor[fil + cont][col - 1] == '?')
					mapaSinSensor[fil + cont][col - 1] = sensores.terreno[k];
				cont--;
			}

			cont = 2;

			for (int k = 4; k < 9; k++)
			{
				if (mapaSinSensor[fil + cont][col - 2] == '?')
					mapaSinSensor[fil + cont][col - 2] = sensores.terreno[k];
				cont--;
			}

			cont = 3;

			for (int k = 9; k < 16; k++)
			{
				if (mapaSinSensor[fil + cont][col - 3] == '?')
					mapaSinSensor[fil + cont][col - 3] = sensores.terreno[k];
				cont--;
			}
			break;
		}
	}
	else
	{
		if (mapaResultado[sensores.posF][sensores.posC] == '?')
			mapaResultado[sensores.posF][sensores.posC] = sensores.terreno[0];

		switch (brujula)
		{
		case NORTE:
			cont = -1;

			for (int k = 1; k < 4; k++)
			{
				if (mapaResultado[sensores.posF - 1][sensores.posC + cont] == '?')
					mapaResultado[sensores.posF - 1][sensores.posC + cont] = sensores.terreno[k];
				cont++;
			}

			cont = -2;

			for (int k = 4; k < 9; k++)
			{
				if (mapaResultado[sensores.posF - 2][sensores.posC + cont] == '?')
					mapaResultado[sensores.posF - 2][sensores.posC + cont] = sensores.terreno[k];
				cont++;
			}

			cont = -3;

			for (int k = 9; k < 16; k++)
			{
				if (mapaResultado[sensores.posF - 3][sensores.posC + cont] == '?')
					mapaResultado[sensores.posF - 3][sensores.posC + cont] = sensores.terreno[k];
				cont++;
			}
			break;

		case SUR:
			cont = 1;
			for (int k = 1; k < 4; k++)
			{
				if (mapaResultado[sensores.posF + 1][sensores.posC + cont] == '?')
					mapaResultado[sensores.posF + 1][sensores.posC + cont] = sensores.terreno[k];
				cont--;
			}

			cont = 2;

			for (int k = 4; k < 9; k++)
			{
				if (mapaResultado[sensores.posF + 2][sensores.posC + cont] == '?')
					mapaResultado[sensores.posF + 2][sensores.posC + cont] = sensores.terreno[k];
				cont--;
			}

			cont = 3;

			for (int k = 9; k < 16; k++)
			{
				if (mapaResultado[sensores.posF + 3][sensores.posC + cont] == '?')
					mapaResultado[sensores.posF + 3][sensores.posC + cont] = sensores.terreno[k];
				cont--;
			}
			break;

		case ESTE:
			cont = -1;
			for (int k = 1; k < 4; k++)
			{
				if (mapaResultado[sensores.posF + cont][sensores.posC + 1] == '?')
					mapaResultado[sensores.posF + cont][sensores.posC + 1] = sensores.terreno[k];
				cont++;
			}

			cont = -2;

			for (int k = 4; k < 9; k++)
			{
				if (mapaResultado[sensores.posF + cont][sensores.posC + 2] == '?')
					mapaResultado[sensores.posF + cont][sensores.posC + 2] = sensores.terreno[k];
				cont++;
			}

			cont = -3;

			for (int k = 9; k < 16; k++)
			{
				if (mapaResultado[sensores.posF + cont][sensores.posC + 3] == '?')
					mapaResultado[sensores.posF + cont][sensores.posC + 3] = sensores.terreno[k];
				cont++;
			}

			break;

		case OESTE:
			cont = 1;

			for (int k = 1; k < 4; k++)
			{
				if (mapaResultado[sensores.posF + cont][sensores.posC - 1] == '?')
					mapaResultado[sensores.posF + cont][sensores.posC - 1] = sensores.terreno[k];
				cont--;
			}

			cont = 2;

			for (int k = 4; k < 9; k++)
			{
				if (mapaResultado[sensores.posF + cont][sensores.posC - 2] == '?')
					mapaResultado[sensores.posF + cont][sensores.posC - 2] = sensores.terreno[k];
				cont--;
			}

			cont = 3;

			for (int k = 9; k < 16; k++)
			{
				if (mapaResultado[sensores.posF + cont][sensores.posC - 3] == '?')
					mapaResultado[sensores.posF + cont][sensores.posC - 3] = sensores.terreno[k];
				cont--;
			}
			break;
		}
	}
}

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}
