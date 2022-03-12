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
	}

	switch(sensores.sentido){
		case norte:
			brujula = 0;
		break;

		case sur:
			brujula = 2;
		break;

		case este:
			brujula = 1;
		break;

		case oeste:
			brujula = 3;
		break;
	}

	if (sensores.terreno[0] == 'G' && !bien_situado)
	{
		bien_situado = true;
		fil = sensores.posF;
		col = sensores.posC;

		//Rellenar mapaResultado con mapaSinSensor
	}

	rellenarMapa(true, sensores);

	//Decidir qué acción tomar
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
				if (mapaSinSensor[fil + cont][col + 1] == '?')
					mapaSinSensor[fil + cont][col + 1] = sensores.terreno[k];
				cont--;
			}

			cont = 2;

			for (int k = 4; k < 9; k++)
			{
				if (mapaSinSensor[fil + cont][col + 2] == '?')
					mapaSinSensor[fil + cont][col + 2] = sensores.terreno[k];
				cont--;
			}

			cont = 3;

			for (int k = 9; k < 16; k++)
			{
				if (mapaSinSensor[fil + cont][col + 2] == '?')
					mapaSinSensor[fil + cont][col + 2] = sensores.terreno[k];
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
				cont --;
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
