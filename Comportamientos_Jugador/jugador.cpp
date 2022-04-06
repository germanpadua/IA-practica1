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
	cout << "Hay Objetivo: " << hay_objetivo << endl;
	cout << "AccionesPendientes Size: " << accionesPendientes.size() << endl;
	cout << "Porcentaje Cerca: " << porcentajeCerca(sensores) << endl;
	cout << endl;

	// Actualizar variables de estado
	int cont = -1;
	no_puede_ir = false;
	huir = false;

	if (sensores.reset)
	{
		ultimaAccion = actIDLE;
		accionesPendientes.clear();
		for (int i = 0; i < mapaSinSensor.size(); i++)
		{
			for (int j = 0; j < mapaSinSensor.size(); j++)
			{
				mapaSinSensor[i][j] = '?';
				mapaContadorAuxiliar[i][j] = 0;
			}
		}

		if (sensores.nivel != 0)
		{
			bien_situado = false;
			fil = col = 99;
		}
		if (sensores.nivel >= 2)
			brujula = NORTE;

		bikini = false;
		zapatillas = false;
	}

	if (sensores.colision)
	{
		accionesPendientes.push_back(ultimaAccion);
		iteraciones_chocandose++;
		ultimaAccion = actIDLE;
	}

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
		break;

	case actTURN_R:
		brujula = (brujula + 1) % 4;
		break;
	case actIDLE:
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

	if (ultimaAccion == actTURN_L || ultimaAccion == actTURN_R)
	{
		iteraciones_girando++;
	}
	else
	{
		iteraciones_girando = 0;
	}

	if (iteraciones_girando > 2)
	{
		huir = true;
	}

	if (iteraciones_chocandose > 4)
	{
		accionesPendientes.clear();
		if (rand() % 2 == 0)
		{
			accionesPendientes.push_back(actTURN_L);
		}
		else
		{
			accionesPendientes.push_back(actTURN_R);
		}
		iteraciones_chocandose = 0;
	}

	if (sensores.nivel == 0)
	{
		bien_situado = true;
		fil = sensores.posF;
		col = sensores.posC;
	}

	switch (sensores.terreno[0])
	{
	case 'D':
		zapatillas = true;
		break;
	case 'K':
		bikini = true;
		break;
	default:
		break;
	}

	if (sensores.terreno[0] == 'G' && !bien_situado)
	{
		bien_situado = true;

		// Rellenar mapaResultado con mapaSinSensor
		for (int i = 0; i < mapaResultado.size(); i++)
		{
			for (int j = 0; j < mapaResultado.size(); j++)
			{
				if (mapaSinSensor[fil - sensores.posF + i][col - sensores.posC + j] != '?' && mapaResultado[i][j] == '?')
				{
					mapaResultado[i][j] = mapaSinSensor[fil - sensores.posF + i][col - sensores.posC + j];
				}

				mapaContadorResultado[i][j] += mapaContadorAuxiliar[fil - sensores.posF + i][col - sensores.posC + j];
			}
		}

		fil = sensores.posF;
		col = sensores.posC;
	}

	rellenarMapa(bien_situado, sensores);

	poca_bateria = sensores.bateria <= 2500;

	if (sensores.vida > 4000)
		poca_bateria = sensores.bateria <= 3500;
	else
		poca_bateria = sensores.bateria <= (sensores.vida + 200);

	// Decidir qué acción tomar
	if (sensores.terreno[0] == 'X' && poca_bateria)
	{
		hay_objetivo = true;
	}

	if (porcentajeCerca(sensores) >= 95)
	{
		// se puede morir
		if (accionesPendientes.size() == 0)
		{
			int obj = buscarMuerte(sensores);

			if (obj != -1)
			{
				if (puedeIrMuerte(sensores, obj))
				{
					cout << "VA HACIA MUERTE" << endl;
					calcularCamino(sensores, obj);
				}
				else
				{
					if (sensores.terreno[2] != 'M')
					{
						accion = actFORWARD;
					}
					else
					{
						if (rand() % 2 == 0)
						{
							accion = actTURN_L;
						}
						else
						{
							accion = actTURN_R;
						}
					}
				}
			}
			else
			{
				int obj = buscarObjetivo(sensores);
				cout << "E" << endl;

				if (sensores.terreno[0] == 'X' && poca_bateria)
				{
					accion = actIDLE;
				}
				else if (obj != -1)
				{
					if (huir && puedeIr(sensores, obj) || !huir && puedeIrSinEquipar(sensores, obj))
					{
						cout << "F" << endl;
						calcularCamino(sensores, obj);
						cout << "G" << endl;
						accion = accionesPendientes[accionesPendientes.size() - 1];
						accionesPendientes.pop_back();
					}
				}
				else
				{
					cout << "NO HAY OBJETIVO" << endl;
					if (rand() % 2 == 0)
					{
						accion = actTURN_L;
					}
					else
					{
						accion = actTURN_R;
					}
				}

				if (!(huir && puedeIr(sensores, obj) || !huir && puedeIrSinEquipar(sensores, obj)))
				{
					cout << "NO PUEDE IR" << endl;
					int obj = buscarObjetivoCerca(sensores);

					if (obj != -1)
					{

						if (huir && puedeIr(sensores, obj) || !huir && puedeIrSinEquipar(sensores, obj))
						{
							calcularCamino(sensores, obj);
							accion = accionesPendientes[accionesPendientes.size() - 1];
							accionesPendientes.pop_back();
						}
						else
						{

							if (rand() % 2 == 0)
							{
								accion = actTURN_L;
							}
							else
							{
								accion = actTURN_R;
							}
						}
					}
					else
					{
						if (rand() % 2 == 0)
						{
							accion = actTURN_L;
						}
						else
						{
							accion = actTURN_R;
						}
					}
				}
			}
		}
		else
		{
			accion = accionesPendientes[accionesPendientes.size() - 1];
			accionesPendientes.pop_back();
		}
	}
	else if (accionesPendientes.size() >= 1)
	{
		cout << "A" << endl;
		int obj = buscarObjetivo(sensores);
		cout << "B" << endl;

		if (!va_por_minimo)
		{
			if (huir && puedeIr(sensores, obj) || !huir && puedeIrSinEquipar(sensores, obj))
			{
				accionesPendientes.clear();
				accion = actIDLE;
				cout << "C" << endl;
				calcularCamino(sensores, obj);
				cout << "D" << endl;

				accion = accionesPendientes[accionesPendientes.size() - 1];
				accionesPendientes.pop_back();
			}
			else
			{
				accion = accionesPendientes[accionesPendientes.size() - 1];
				accionesPendientes.pop_back();
			}
		}
		else
		{
			accion = accionesPendientes[accionesPendientes.size() - 1];
			accionesPendientes.pop_back();
		}

		if(loboCerca(sensores)){
			accionesPendientes.clear();
			cout << "LOBO CERQUITA :O" << endl;
			if (rand() % 2 == 0)
			{
				accion = actTURN_L;
			}
			else
			{
				accion = actTURN_R;
			}
		}
	}
	else
	{
		int obj = buscarObjetivo(sensores);
		cout << "E" << endl;

		if (sensores.terreno[0] == 'X' && poca_bateria)
		{
			accion = actIDLE;
		}
		else if (obj != -1)
		{
			if (huir && puedeIr(sensores, obj) || !huir && puedeIrSinEquipar(sensores, obj))
			{
				cout << "F" << endl;
				calcularCamino(sensores, obj);
				cout << "G" << endl;
				accion = accionesPendientes[accionesPendientes.size() - 1];
				accionesPendientes.pop_back();
			}
		}
		else
		{
			cout << "NO HAY OBJETIVO" << endl;
			if (rand() % 2 == 0)
			{
				accion = actTURN_L;
			}
			else
			{
				accion = actTURN_R;
			}
		}

		if (!(huir && puedeIr(sensores, obj) || !huir && puedeIrSinEquipar(sensores, obj)))
		{
			cout << "NO PUEDE IR" << endl;
			int obj = buscarObjetivoCerca(sensores);

			if (obj != -1)
			{

				if (huir && puedeIr(sensores, obj) || !huir && puedeIrSinEquipar(sensores, obj))
				{
					calcularCamino(sensores, obj);
					accion = accionesPendientes[accionesPendientes.size() - 1];
					accionesPendientes.pop_back();
				}
				else
				{

					if (rand() % 2 == 0)
					{
						accion = actTURN_L;
					}
					else
					{
						accion = actTURN_R;
					}
				}
			}
			else
			{
				if (rand() % 2 == 0)
				{
					accion = actTURN_L;
				}
				else
				{
					accion = actTURN_R;
				}
			}
		}

		if(loboCerca(sensores)){
			accionesPendientes.clear();
			cout << "LOBO CERQUITA :O" << endl;
			if (rand() % 2 == 0)
			{
				accion = actTURN_L;
			}
			else
			{
				accion = actTURN_R;
			}
		}
	}

	/*
		if (sensores.terreno[2] == 'P')
		{
			accionesPendientes.clear();
			accion = actTURN_L;
		}
	*/
	iteracion++;

	ultimaAccion = accion;

	cout << "HUIR " << huir << endl;
	// Determinar el efecto de la ultima accion enviada
	return accion;
}

int ComportamientoJugador::buscarObjetivoCerca(Sensores sensores)
{
	va_por_minimo = false;
	int cont;
	pos_a_la_vista = bikini_a_la_vista = zapatillas_a_la_vista = recarga_a_la_vista = -1;

	int k_objetivo = -1;

	for (int k = 3; k > 0; k--)
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
	else if (sensores.terreno[0] == 'X' && !poca_bateria)
	{
		huir = true;

		for (int i = 3; i > 0; i--)
		{
			if (sensores.terreno[i] != 'M' && sensores.terreno[i] != 'P')
			{
				k_objetivo = i;
				hay_objetivo = true;
				huir = true;
			}
		}
	}
	else if (sensores.terreno[0] == 'B' && !zapatillas)
	{
		huir = true;
		for (int i = 3; i > 0; i--)
		{
			if (sensores.terreno[i] == 'A' && bikini)
			{
				k_objetivo = i;
				hay_objetivo = true;
				huir = true;
			}
			else if (sensores.terreno[i] != 'B' && sensores.terreno[i] != 'M' && sensores.terreno[i] != 'P')
			{
				k_objetivo = i;
				hay_objetivo = true;
				huir = true;
			}
		}

		if (k_objetivo != -1 && sensores.terreno[2] != 'P' && sensores.terreno[2] != 'M')
			k_objetivo = 2;
	}
	else if (sensores.terreno[0] == 'A' && !bikini)
	{
		huir = true;
		for (int i = 3; i > 0; i--)
		{
			if (sensores.terreno[i] == 'B' && zapatillas)
			{
				k_objetivo = i;
				hay_objetivo = true;
				huir = true;
			}
			else if (sensores.terreno[i] != 'A' && sensores.terreno[i] != 'M' && sensores.terreno[i] != 'P')
			{
				k_objetivo = i;
				hay_objetivo = true;
				huir = true;
			}
		}

		if (k_objetivo != -1 && sensores.terreno[2] != 'P' && sensores.terreno[2] != 'M')
			k_objetivo = 2;
	}

	if (k_objetivo == -1)
	{
		int min = 99999999;
		int k_min = -1;

		if (bien_situado)
		{
			switch (brujula)
			{
			case NORTE:
				cont = -1;

				for (int k = 1; k < 4; k++)
				{
					if (mapaContadorResultado[fil - 1][col + cont] < min && sensores.terreno[k] != 'P' && sensores.terreno[k] != 'M')
					{
						min = mapaContadorResultado[fil - 1][col + cont];
						k_min = k;
					}
					cont++;
				}

				if (min == mapaContadorResultado[fil - 1][col])
					k_min = 2;

				break;

			case SUR:
				cont = 1;
				for (int k = 1; k < 4; k++)
				{
					if (mapaContadorResultado[fil + 1][col + cont] < min && sensores.terreno[k] != 'P' && sensores.terreno[k] != 'M')
					{
						min = mapaContadorResultado[fil + 1][col + cont];
						k_min = k;
					}
					cont--;
				}

				if (min == mapaContadorResultado[fil + 1][col])
					k_min = 2;

				break;

			case ESTE:
				cont = -1;
				for (int k = 1; k < 4; k++)
				{
					if (mapaContadorResultado[fil + cont][col + 1] < min && sensores.terreno[k] != 'P' && sensores.terreno[k] != 'M')
					{
						min = mapaContadorResultado[fil + cont][col + 1];
						k_min = k;
					}
					cont++;
				}

				if (min == mapaContadorResultado[fil][col + 1])
					k_min = 2;

				break;

			case OESTE:
				cont = 1;

				for (int k = 1; k < 4; k++)
				{
					if (mapaContadorResultado[fil + cont][col - 1] < min && sensores.terreno[k] != 'P' && sensores.terreno[k] != 'M')
					{
						min = mapaContadorResultado[fil + cont][col - 1];
						k_min = k;
					}
					cont--;
				}

				if (min == mapaContadorResultado[fil][col - 1])
					k_min = 2;
				break;
			}
		}
		else
		{
			switch (brujula)
			{
			case NORTE:
				cont = -1;

				for (int k = 1; k < 4; k++)
				{
					if (mapaContadorAuxiliar[fil - 1][col + cont] < min && sensores.terreno[k] != 'P' && sensores.terreno[k] != 'M')
					{
						min = mapaContadorAuxiliar[fil - 1][col + cont];
						k_min = k;
					}
					cont++;
				}

				if (min == mapaContadorAuxiliar[fil - 1][col])
					k_min = 2;

				break;

			case SUR:
				cont = 1;
				for (int k = 1; k < 4; k++)
				{
					if (mapaContadorAuxiliar[fil + 1][col + cont] < min && sensores.terreno[k] != 'P' && sensores.terreno[k] != 'M')
					{
						min = mapaContadorAuxiliar[fil + 1][col + cont];
						k_min = k;
					}
					cont--;
				}

				if (min == mapaContadorAuxiliar[fil + 1][col])
					k_min = 2;

				break;

			case ESTE:
				cont = -1;
				for (int k = 1; k < 4; k++)
				{
					if (mapaContadorAuxiliar[fil + cont][col + 1] < min && sensores.terreno[k] != 'P' && sensores.terreno[k] != 'M')
					{
						min = mapaContadorAuxiliar[fil + cont][col + 1];
						k_min = k;
					}
					cont++;
				}

				if (min == mapaContadorAuxiliar[fil][col + 1])
					k_min = 2;

				break;

			case OESTE:
				cont = 1;

				for (int k = 1; k < 4; k++)
				{
					if (mapaContadorAuxiliar[fil + cont][col - 1] < min && sensores.terreno[k] != 'P' && sensores.terreno[k] != 'M')
					{
						min = mapaContadorAuxiliar[fil + cont][col - 1];
						k_min = k;
					}
					cont--;
				}

				if (min == mapaContadorAuxiliar[fil][col - 1])
					k_min = 2;

				break;
			}
		}

		/*
		if (((sensores.terreno[k_objetivo] == 'A' && bikini) || (sensores.terreno[k_objetivo] == 'B' && zapatillas)))
			k_objetivo = k_min;
		else if (sensores.terreno[k_objetivo] == 'A' || sensores.terreno[k_objetivo] == 'B')
		{
		}
		else if (sensores.terreno[k_objetivo] == 'P' || sensores.terreno[k_objetivo] == 'M')
		{
		}
		else
		{
			k_objetivo = k_min;
		}
		*/
		va_por_minimo = true;
		k_objetivo = k_min;
	}

	if (k_objetivo != -1)
	{
		cout << "OBJETIVO CERCA " << k_objetivo << endl;
	}
	else
	{
		if (sensores.terreno[2] != 'P' && sensores.terreno[2] != 'M')
			k_objetivo = 2;
	}

	/*
	if (sensores.terreno[2] == 'M' || sensores.terreno[2] == 'P')
	{
		accionesPendientes.clear();
		accionesPendientes.push_back(actTURN_R);
	}
*/
	return k_objetivo;
}

bool ComportamientoJugador::calcularCamino(Sensores sensores, int objetivo)
{
	vector<Action> accionesPendientes_aux;

	cout << "OBJETIVO " << objetivo;

	switch (objetivo)
	{
	case 1:
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actTURN_L);
		accionesPendientes_aux.push_back(actFORWARD);
		break;
	case 2:
		accionesPendientes_aux.push_back(actFORWARD);
		break;
	case 3:
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actTURN_R);
		accionesPendientes_aux.push_back(actFORWARD);
		break;
	case 4:
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actTURN_L);
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actFORWARD);
		break;
	case 5:
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actTURN_L);
		accionesPendientes_aux.push_back(actFORWARD);
		break;
	case 6:
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actFORWARD);
		break;
	case 7:
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actTURN_R);
		accionesPendientes_aux.push_back(actFORWARD);
		break;
	case 8:
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actTURN_R);
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actFORWARD);
		break;
	case 9:
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actTURN_L);
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actFORWARD);
		break;
	case 10:
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actTURN_L);
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actFORWARD);
		break;
	case 11:
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actTURN_L);
		accionesPendientes_aux.push_back(actFORWARD);
		break;
	case 12:
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actFORWARD);
		break;
	case 13:
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actTURN_R);
		accionesPendientes_aux.push_back(actFORWARD);
		break;
	case 14:
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actTURN_R);
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actFORWARD);
		break;
	case 15:
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actTURN_R);
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actFORWARD);
		accionesPendientes_aux.push_back(actFORWARD);
		break;
	default:
		break;
	}

	for (int i = accionesPendientes_aux.size(); i > 0; i--)
	{
		accionesPendientes.push_back(accionesPendientes_aux[accionesPendientes_aux.size() - 1]);
		accionesPendientes_aux.pop_back();
	}

	return accionesPendientes.size() > 0;
}

int ComportamientoJugador::buscarObjetivo(Sensores sensores)
{
	va_por_minimo = false;
	int cont;
	pos_a_la_vista = bikini_a_la_vista = zapatillas_a_la_vista = recarga_a_la_vista = -1;

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
		huir = true;
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
	else if (sensores.terreno[0] == 'A' && !bikini)
	{
		huir = true;

		for (int i = 15; i > 0; i--)
		{
			if (sensores.terreno[i] == 'B' && zapatillas)
			{
				k_objetivo = i;
				hay_objetivo = true;
				huir = true;
			}
			else if (sensores.terreno[i] != 'A' && sensores.terreno[i] != 'M' && sensores.terreno[i] != 'P')
			{
				k_objetivo = i;
				hay_objetivo = true;
				huir = true;
			}
		}

		if (k_objetivo != -1 && sensores.terreno[2] != 'P' && sensores.terreno[2] != 'M')
			k_objetivo = 2;
	}
	else if (sensores.terreno[0] == 'X' && !poca_bateria)
	{
		huir = true;

		for (int i = 15; i > 0; i--)
		{
			if (sensores.terreno[i] != 'M' && sensores.terreno[i] != 'P')
			{
				k_objetivo = i;
				hay_objetivo = true;
				huir = true;
			}
		}
	}
	else if (sensores.terreno[0] == 'B' && !zapatillas)
	{
		huir = true;
		for (int i = 15; i > 0; i--)
		{
			if (sensores.terreno[i] == 'A' && bikini)
			{
				k_objetivo = i;
				hay_objetivo = true;
				huir = true;
			}
			else if (sensores.terreno[i] != 'B' && sensores.terreno[i] != 'M' && sensores.terreno[i] != 'P')
			{
				k_objetivo = i;
				hay_objetivo = true;
				huir = true;
			}
		}

		if (k_objetivo != -1 && sensores.terreno[2] != 'P' && sensores.terreno[2] != 'M')
			k_objetivo = 2;
	}
	else
	{
		int min = 99999999;
		int k_min = -1;
		if (bien_situado)
		{
			switch (brujula)
			{
			case NORTE:
				cont = -1;
				for (int k = 1; k < 4; k++)
				{
					if (mapaContadorResultado[sensores.posF - 1][sensores.posC + cont] < min && sensores.terreno[k] != 'P' && sensores.terreno[k] != 'M')
					{
						min = mapaContadorResultado[sensores.posF - 1][sensores.posC + cont];
						k_min = k;
					}
					cont++;
				}

				cont = -2;
				for (int k = 4; k < 9; k++)
				{
					if (mapaContadorResultado[sensores.posF - 2][sensores.posC + cont] < min && sensores.terreno[k] != 'P' && sensores.terreno[k] != 'M')
					{
						min = mapaContadorResultado[sensores.posF - 2][sensores.posC + cont];
						k_min = k;
					}
					cont++;
				}

				cont = -3;

				for (int k = 9; k < 16; k++)
				{
					if (mapaContadorResultado[sensores.posF - 3][sensores.posC + cont] < min && sensores.terreno[k] != 'P' && sensores.terreno[k] != 'M')
					{
						min = mapaContadorResultado[sensores.posF - 3][sensores.posC + cont];
						k_min = k;
					}
					cont++;
				}

				if (min == mapaContadorResultado[sensores.posF - 3][sensores.posC])
				{
					k_min = 12;
				}

				break;

			case SUR:
				cont = 1;
				// min = mapaContador[sensores.posF + 3][sensores.posC];

				for (int k = 1; k < 4; k++)
				{
					if (mapaContadorResultado[sensores.posF + 1][sensores.posC + cont] < min && sensores.terreno[k] != 'P' && sensores.terreno[k] != 'M')
					{
						min = mapaContadorResultado[sensores.posF + 1][sensores.posC + cont];
						k_min = k;
					}
					cont--;
				}

				cont = 2;

				for (int k = 4; k < 9; k++)
				{
					if (mapaContadorResultado[sensores.posF + 2][sensores.posC + cont] < min && sensores.terreno[k] != 'P' && sensores.terreno[k] != 'M')
					{
						min = mapaContadorResultado[sensores.posF + 2][sensores.posC + cont];
						k_min = k;
					}
					cont--;
				}

				cont = 3;

				for (int k = 9; k < 16; k++)
				{
					if (mapaContadorResultado[sensores.posF + 3][sensores.posC + cont] < min && sensores.terreno[k] != 'P' && sensores.terreno[k] != 'M')
					{
						min = mapaContadorResultado[sensores.posF + 3][sensores.posC + cont];
						k_min = k;
					}
					cont--;
				}

				if (min == mapaContadorResultado[sensores.posF + 3][sensores.posC])
				{
					k_min = 12;
				}

				break;

			case ESTE:
				cont = -1;
				// min = mapaContador[sensores.posF][sensores.posC + 3];

				for (int k = 1; k < 4; k++)
				{
					if (mapaContadorResultado[sensores.posF + cont][sensores.posC + 1] < min && sensores.terreno[k] != 'P' && sensores.terreno[k] != 'M')
					{
						min = mapaContadorResultado[sensores.posF + cont][sensores.posC + 1];
						k_min = k;
					}
					cont++;
				}

				cont = -2;

				for (int k = 4; k < 9; k++)
				{
					if (mapaContadorResultado[sensores.posF + cont][sensores.posC + 2] < min && sensores.terreno[k] != 'P' && sensores.terreno[k] != 'M')
					{
						min = mapaContadorResultado[sensores.posF + cont][sensores.posC + 2];
						k_min = k;
					}
					cont++;
				}

				cont = -3;

				for (int k = 9; k < 16; k++)
				{
					if (mapaContadorResultado[sensores.posF + cont][sensores.posC + 3] < min && sensores.terreno[k] != 'P' && sensores.terreno[k] != 'M')
					{
						min = mapaContadorResultado[sensores.posF + cont][sensores.posC + 3];
						k_min = k;
					}
					cont++;
				}

				if (min == mapaContadorResultado[sensores.posF][sensores.posC + 3])
				{
					k_min = 12;
				}

				break;

			case OESTE:
				cont = 1;
				// min = mapaContador[sensores.posF][sensores.posC - 3];
				for (int k = 1; k < 4; k++)
				{
					if (mapaContadorResultado[sensores.posF + cont][sensores.posC - 1] < min && sensores.terreno[k] != 'P' && sensores.terreno[k] != 'M')
					{
						min = mapaContadorResultado[sensores.posF + cont][sensores.posC - 1];
						k_min = k;
					}
					cont--;
				}

				cont = 2;

				for (int k = 4; k < 9; k++)
				{
					if (mapaContadorResultado[sensores.posF + cont][sensores.posC - 2] < min && sensores.terreno[k] != 'P' && sensores.terreno[k] != 'M')
					{
						min = mapaContadorResultado[sensores.posF + cont][sensores.posC - 2];
						k_min = k;
					}

					cont--;
				}

				cont = 3;

				for (int k = 9; k < 16; k++)
				{
					if (mapaContadorResultado[sensores.posF + cont][sensores.posC - 3] < min && sensores.terreno[k] != 'P' && sensores.terreno[k] != 'M')
					{
						min = mapaContadorResultado[sensores.posF + cont][sensores.posC - 3];
						k_min = k;
					}
					cont--;
				}

				if (min == mapaContadorResultado[sensores.posF][sensores.posC - 3])
				{
					k_min = 12;
				}

				break;
			}
		}
		else
		{
			switch (brujula)
			{
			case NORTE:
				cont = -1;
				for (int k = 1; k < 4; k++)
				{
					if (mapaContadorAuxiliar[fil - 1][col + cont] < min && sensores.terreno[k] != 'P' && sensores.terreno[k] != 'M')
					{
						min = mapaContadorAuxiliar[fil - 1][col + cont];
						k_min = k;
					}
					cont++;
				}

				cont = -2;

				for (int k = 4; k < 9; k++)
				{
					if (mapaContadorAuxiliar[fil - 2][col + cont] < min && sensores.terreno[k] != 'P' && sensores.terreno[k] != 'M')
					{
						min = mapaContadorAuxiliar[fil - 2][col + cont];
						k_min = k;
					}
					cont++;
				}

				cont = -3;

				for (int k = 9; k < 16; k++)
				{
					if (mapaContadorAuxiliar[fil - 3][col + cont] < min && sensores.terreno[k] != 'P' && sensores.terreno[k] != 'M')
					{
						min = mapaContadorAuxiliar[fil - 3][col + cont];
						k_min = k;
					}
					cont++;
				}

				if (min == mapaContadorAuxiliar[fil - 3][col])
				{
					k_min = 12;
				}

				break;

			case SUR:
				cont = 1;
				// min = mapaContador[fil + 3][col];

				for (int k = 1; k < 4; k++)
				{
					if (mapaContadorAuxiliar[fil + 1][col + cont] < min && sensores.terreno[k] != 'P' && sensores.terreno[k] != 'M')
					{
						min = mapaContadorAuxiliar[fil + 1][col + cont];
						k_min = k;
					}
					cont--;
				}

				cont = 2;

				for (int k = 4; k < 9; k++)
				{
					if (mapaContadorAuxiliar[fil + 2][col + cont] < min && sensores.terreno[k] != 'P' && sensores.terreno[k] != 'M')
					{
						min = mapaContadorAuxiliar[fil + 2][col + cont];
						k_min = k;
					}
					cont--;
				}

				cont = 3;

				for (int k = 9; k < 16; k++)
				{
					if (mapaContadorAuxiliar[fil + 3][col + cont] < min && sensores.terreno[k] != 'P' && sensores.terreno[k] != 'M')
					{
						min = mapaContadorAuxiliar[fil + 3][col + cont];
						k_min = k;
					}
					cont--;
				}

				if (min == mapaContadorAuxiliar[fil + 3][col])
				{
					k_min = 12;
				}

				break;

			case ESTE:
				cont = -1;
				// min = mapaContador[fil][col + 3];

				for (int k = 1; k < 4; k++)
				{
					if (mapaContadorAuxiliar[fil + cont][col + 1] < min && sensores.terreno[k] != 'P' && sensores.terreno[k] != 'M')
					{
						min = mapaContadorAuxiliar[fil + cont][col + 1];
						k_min = k;
					}
					cont++;
				}

				cont = -2;

				for (int k = 4; k < 9; k++)
				{
					if (mapaContadorAuxiliar[fil + cont][col + 2] < min && sensores.terreno[k] != 'P' && sensores.terreno[k] != 'M')
					{
						min = mapaContadorAuxiliar[fil + cont][col + 2];
						k_min = k;
					}
					cont++;
				}

				cont = -3;

				for (int k = 9; k < 16; k++)
				{
					if (mapaContadorAuxiliar[fil + cont][col + 3] < min && sensores.terreno[k] != 'P' && sensores.terreno[k] != 'M')
					{
						min = mapaContadorAuxiliar[fil + cont][col + 3];
						k_min = k;
					}
					cont++;
				}

				if (min == mapaContadorAuxiliar[fil][col + 3])
				{
					k_min = 12;
				}

				break;

			case OESTE:
				cont = 1;
				// min = mapaContador[fil][col - 3];
				for (int k = 1; k < 4; k++)
				{
					if (mapaContadorAuxiliar[fil + cont][col - 1] < min && sensores.terreno[k] != 'P' && sensores.terreno[k] != 'M')
					{
						min = mapaContadorAuxiliar[fil + cont][col - 1];
						k_min = k;
					}
					cont--;
				}

				cont = 2;

				for (int k = 4; k < 9; k++)
				{
					if (mapaContadorAuxiliar[fil + cont][col - 2] < min && sensores.terreno[k] != 'P' && sensores.terreno[k] != 'M')
					{
						min = mapaContadorAuxiliar[fil + cont][col - 2];
						k_min = k;
					}

					cont--;
				}

				cont = 3;

				for (int k = 9; k < 16; k++)
				{
					if (mapaContadorAuxiliar[fil + cont][col - 3] < min && sensores.terreno[k] != 'P' && sensores.terreno[k] != 'M')
					{
						min = mapaContadorAuxiliar[fil + cont][col - 3];
						k_min = k;
					}
					cont--;
				}

				if (min == mapaContadorAuxiliar[fil][col - 3])
				{
					k_min = 12;
				}

				break;
			}
		}

		k_objetivo = k_min;
		va_por_minimo = true;
	}

	if (sensores.terreno[2] == 'M' || sensores.terreno[2] == 'P')
	{
		k_objetivo = -1;
	}

	return k_objetivo;
}

void ComportamientoJugador::rellenarMapa(bool sensor_posicion, Sensores sensores)
{
	int cont;
	cout << "H" << endl;
	if (!sensor_posicion)
	{
		if (mapaSinSensor[fil][col] == '?')
			mapaSinSensor[fil][col] = sensores.terreno[0];

		mapaContadorAuxiliar[fil][col] = mapaContadorAuxiliar[fil][col] + 8;

		switch (brujula)
		{
		case NORTE:
			cont = -1;

			for (int k = 1; k < 4; k++)
			{
				if (mapaSinSensor[fil - 1][col + cont] == '?')
					mapaSinSensor[fil - 1][col + cont] = sensores.terreno[k];

				if (sensores.terreno[2] != 'M')
					mapaContadorAuxiliar[fil - 1][col + cont]++;

				cont++;
			}

			cont = -2;

			for (int k = 4; k < 9; k++)
			{
				if (mapaSinSensor[fil - 2][col + cont] == '?')
					mapaSinSensor[fil - 2][col + cont] = sensores.terreno[k];

				if (sensores.terreno[2] != 'M')
					mapaContadorAuxiliar[fil - 2][col + cont]++;

				cont++;
			}

			cont = -3;

			for (int k = 9; k < 16; k++)
			{
				if (mapaSinSensor[fil - 3][col + cont] == '?')
					mapaSinSensor[fil - 3][col + cont] = sensores.terreno[k];

				if (sensores.terreno[2] != 'M')
					mapaContadorAuxiliar[fil - 3][col + cont]++;

				cont++;
			}
			break;

		case SUR:
			cont = 1;
			for (int k = 1; k < 4; k++)
			{
				if (mapaSinSensor[fil + 1][col + cont] == '?')
					mapaSinSensor[fil + 1][col + cont] = sensores.terreno[k];

				if (sensores.terreno[2] != 'M')
					mapaContadorAuxiliar[fil + 1][col + cont]++;

				cont--;
			}

			cont = 2;

			for (int k = 4; k < 9; k++)
			{
				if (mapaSinSensor[fil + 2][col + cont] == '?')
					mapaSinSensor[fil + 2][col + cont] = sensores.terreno[k];

				if (sensores.terreno[2] != 'M')
					mapaContadorAuxiliar[fil + 2][col + cont]++;

				cont--;
			}

			cont = 3;

			for (int k = 9; k < 16; k++)
			{
				if (mapaSinSensor[fil + 3][col + cont] == '?')
					mapaSinSensor[fil + 3][col + cont] = sensores.terreno[k];

				if (sensores.terreno[2] != 'M')
					mapaContadorAuxiliar[fil + 3][col + cont]++;

				cont--;
			}
			break;

		case ESTE:
			cont = -1;
			for (int k = 1; k < 4; k++)
			{
				if (mapaSinSensor[fil + cont][col + 1] == '?')
					mapaSinSensor[fil + cont][col + 1] = sensores.terreno[k];

				if (sensores.terreno[2] != 'M')
					mapaContadorAuxiliar[fil + cont][col + 1]++;

				cont++;
			}

			cont = -2;

			for (int k = 4; k < 9; k++)
			{
				if (mapaSinSensor[fil + cont][col + 2] == '?')
					mapaSinSensor[fil + cont][col + 2] = sensores.terreno[k];

				if (sensores.terreno[2] != 'M')
					mapaContadorAuxiliar[fil + cont][col + 2]++;

				cont++;
			}

			cont = -3;

			for (int k = 9; k < 16; k++)
			{
				if (mapaSinSensor[fil + cont][col + 3] == '?')
					mapaSinSensor[fil + cont][col + 3] = sensores.terreno[k];

				if (sensores.terreno[2] != 'M')
					mapaContadorAuxiliar[fil + cont][col + 3]++;

				cont++;
			}

			break;

		case OESTE:
			cont = 1;

			for (int k = 1; k < 4; k++)
			{
				if (mapaSinSensor[fil + cont][col - 1] == '?')
					mapaSinSensor[fil + cont][col - 1] = sensores.terreno[k];

				if (sensores.terreno[2] != 'M')
					mapaContadorAuxiliar[fil + cont][col - 1]++;

				cont--;
			}

			cont = 2;

			for (int k = 4; k < 9; k++)
			{
				if (mapaSinSensor[fil + cont][col - 2] == '?')
					mapaSinSensor[fil + cont][col - 2] = sensores.terreno[k];

				if (sensores.terreno[2] != 'M')
					mapaContadorAuxiliar[fil + cont][col - 2]++;

				cont--;
			}

			cont = 3;

			for (int k = 9; k < 16; k++)
			{
				if (mapaSinSensor[fil + cont][col - 3] == '?')
					mapaSinSensor[fil + cont][col - 3] = sensores.terreno[k];

				if (sensores.terreno[2] != 'M')
					mapaContadorAuxiliar[fil + cont][col - 3]++;

				cont--;
			}
			break;
		}
	}
	else
	{
		cout << "I" << endl;
		cout << sensores.posF << "   " << sensores.posC << endl;

		if (mapaResultado[sensores.posF][sensores.posC] == '?')
			mapaResultado[sensores.posF][sensores.posC] = sensores.terreno[0];
		cout << "fnapf" << endl;
		mapaContadorResultado[sensores.posF][sensores.posC] = mapaContadorResultado[sensores.posF][sensores.posC] + 8;
		cout << "ainainf" << endl;
		switch (brujula)
		{
		case NORTE:
			cont = -1;
			cout << "PRIMERO" << endl;
			for (int k = 1; k < 4; k++)
			{
				if (mapaResultado[sensores.posF - 1][sensores.posC + cont] == '?')
					mapaResultado[sensores.posF - 1][sensores.posC + cont] = sensores.terreno[k];

				if (sensores.terreno[2] != 'M')
					mapaContadorResultado[sensores.posF - 1][sensores.posC + cont]++;

				cont++;
			}

			cont = -2;
			cout << "SEGUNDO" << endl;

			for (int k = 4; k < 9; k++)
			{
				if (mapaResultado[sensores.posF - 2][sensores.posC + cont] == '?')
					mapaResultado[sensores.posF - 2][sensores.posC + cont] = sensores.terreno[k];

				if (sensores.terreno[2] != 'M')
					mapaContadorResultado[sensores.posF - 2][sensores.posC + cont]++;

				cont++;
			}

			cont = -3;
			cout << "TERCERO" << endl;

			for (int k = 9; k < 16; k++)
			{
				if (mapaResultado[sensores.posF - 3][sensores.posC + cont] == '?')
					mapaResultado[sensores.posF - 3][sensores.posC + cont] = sensores.terreno[k];

				if (sensores.terreno[2] != 'M')
					mapaContadorResultado[sensores.posF - 3][sensores.posC + cont]++;

				cont++;
			}
			break;

		case SUR:
			cont = 1;
			cout << "PRIMERO" << endl;

			for (int k = 1; k < 4; k++)
			{
				if (mapaResultado[sensores.posF + 1][sensores.posC + cont] == '?')
					mapaResultado[sensores.posF + 1][sensores.posC + cont] = sensores.terreno[k];

				if (sensores.terreno[2] != 'M')
					mapaContadorResultado[sensores.posF + 1][sensores.posC + cont]++;

				cont--;
			}

			cont = 2;
			cout << "SEGUNDO" << endl;

			for (int k = 4; k < 9; k++)
			{
				if (mapaResultado[sensores.posF + 2][sensores.posC + cont] == '?')
					mapaResultado[sensores.posF + 2][sensores.posC + cont] = sensores.terreno[k];

				if (sensores.terreno[2] != 'M')
					mapaContadorResultado[sensores.posF + 2][sensores.posC + cont]++;

				cont--;
			}

			cont = 3;
			cout << "TERCERO" << endl;

			for (int k = 9; k < 16; k++)
			{
				if (mapaResultado[sensores.posF + 3][sensores.posC + cont] == '?')
					mapaResultado[sensores.posF + 3][sensores.posC + cont] = sensores.terreno[k];
				if (sensores.terreno[2] != 'M')
					mapaContadorResultado[sensores.posF + 3][sensores.posC + cont]++;

				cont--;
			}
			break;

		case ESTE:
			cont = -1;
			cout << "PRIMERO" << endl;

			for (int k = 1; k < 4; k++)
			{
				if (mapaResultado[sensores.posF + cont][sensores.posC + 1] == '?')
					mapaResultado[sensores.posF + cont][sensores.posC + 1] = sensores.terreno[k];

				if (sensores.terreno[2] != 'M')
					mapaContadorResultado[sensores.posF + cont][sensores.posC + 1]++;

				cont++;
			}

			cont = -2;
			cout << "SEGUNDO" << endl;

			for (int k = 4; k < 9; k++)
			{
				if (mapaResultado[sensores.posF + cont][sensores.posC + 2] == '?')
					mapaResultado[sensores.posF + cont][sensores.posC + 2] = sensores.terreno[k];

				if (sensores.terreno[2] != 'M')
					mapaContadorResultado[sensores.posF + cont][sensores.posC + 2]++;

				cont++;
			}

			cont = -3;
			cout << "TERCERO" << endl;

			for (int k = 9; k < 16; k++)
			{
				if (mapaResultado[sensores.posF + cont][sensores.posC + 3] == '?')
					mapaResultado[sensores.posF + cont][sensores.posC + 3] = sensores.terreno[k];

				if (sensores.terreno[2] != 'M')
					mapaContadorResultado[sensores.posF + cont][sensores.posC + 3]++;

				cont++;
			}

			break;

		case OESTE:
			cont = 1;
			cout << "PRIMERO" << endl;

			for (int k = 1; k < 4; k++)
			{
				if (mapaResultado[sensores.posF + cont][sensores.posC - 1] == '?')
					mapaResultado[sensores.posF + cont][sensores.posC - 1] = sensores.terreno[k];

				if (sensores.terreno[2] != 'M')
					mapaContadorResultado[sensores.posF + cont][sensores.posC - 1]++;

				cont--;
			}

			cont = 2;
			cout << "SEGUNDO" << endl;

			for (int k = 4; k < 9; k++)
			{
				if (mapaResultado[sensores.posF + cont][sensores.posC - 2] == '?')
					mapaResultado[sensores.posF + cont][sensores.posC - 2] = sensores.terreno[k];

				if (sensores.terreno[2] != 'M')
					mapaContadorResultado[sensores.posF + cont][sensores.posC - 2]++;

				cont--;
			}

			cont = 3;
			cout << "TERCERO" << endl;

			for (int k = 9; k < 16; k++)
			{
				if (mapaResultado[sensores.posF + cont][sensores.posC - 3] == '?')
					mapaResultado[sensores.posF + cont][sensores.posC - 3] = sensores.terreno[k];

				if (sensores.terreno[2] != 'M')
					mapaContadorResultado[sensores.posF + cont][sensores.posC - 3]++;

				cont--;
			}
			break;
		}
	}
}

float ComportamientoJugador::porcentajeCerca(Sensores sensores)
{
	float resultado = 0;
	float total = 0;
	if (bien_situado)
	{

		for (int i = -10; i < 11 && (i + fil) < mapaResultado.size(); i++)
		{
			for (int j = -10; j < 11 && (j + col) < mapaResultado.size(); j++)
			{
				if ((i + fil) >= 0 && (j + col) >= 0)
				{
					if (mapaResultado[fil + i][col + j] != '?')
					{
						resultado++;
					}
					total++;
				}
			}
		}
	}
	else
	{
		return 0;
	}

	if (total <= 333)
	{
		return 0;
	}
	else
		return 100 * (resultado / total);
}

int ComportamientoJugador::buscarMuerte(Sensores sensores)
{
	bool muerte_a_la_vista = false;

	for (int i = 1; i < 16; i++)
	{
		if (sensores.terreno[i] == 'P' || sensores.superficie[i] == 'l')
		{
			muerte_a_la_vista = true;
			return i;
		}
	}
	return -1;
}

bool ComportamientoJugador::puedeIrMuerte(Sensores sensores, int casilla)
{
	bool puede = true;
	switch (casilla)
	{
	case 1:
		if ((sensores.terreno[2] == 'M' || (sensores.terreno[2] == 'A' && !bikini) || (sensores.terreno[2] == 'B' && !zapatillas) || sensores.terreno[1] == 'M' || (sensores.terreno[1] == 'A' && !bikini) || (sensores.terreno[1] == 'B' && !zapatillas)))
			puede = false;
		break;
	case 2:
		if (!huir && (sensores.terreno[2] == 'M' || (sensores.terreno[2] == 'A' && !bikini) || (sensores.terreno[2] == 'B' && !zapatillas)))
			puede = false;

		break;
	case 3:
		if (!huir && (sensores.terreno[2] == 'M' || (sensores.terreno[2] == 'A' && !bikini) || (sensores.terreno[2] == 'B' && !zapatillas) || sensores.terreno[3] == 'M' || (sensores.terreno[3] == 'A' && !bikini) || (sensores.terreno[3] == 'B' && !zapatillas)))
			puede = false;

		break;
	case 4:
		if (!huir && ((sensores.terreno[2] == 'M') || (sensores.terreno[2] == 'A' && !bikini) || (sensores.terreno[2] == 'B' && !zapatillas) || (sensores.terreno[6] == 'M') || (sensores.terreno[6] == 'A' && !bikini) || (sensores.terreno[6] == 'B' && !zapatillas) || (sensores.terreno[5] == 'M') || (sensores.terreno[5] == 'A' && !bikini) || (sensores.terreno[5] == 'B' && !zapatillas) || (sensores.terreno[4] == 'A' && !bikini) || (sensores.terreno[4] == 'B' && !zapatillas)))
			puede = false;

		break;
	case 5:
		if (!huir && ((sensores.terreno[2] == 'M') || (sensores.terreno[2] == 'A' && !bikini) || (sensores.terreno[2] == 'B' && !zapatillas) || (sensores.terreno[6] == 'M') || (sensores.terreno[6] == 'A' && !bikini) || (sensores.terreno[6] == 'B' && !zapatillas) || sensores.terreno[5] == 'M' || (sensores.terreno[5] == 'A' && !bikini) || (sensores.terreno[5] == 'B' && !zapatillas)))
			puede = false;
		else

			break;
	case 6:
		if (!huir && (sensores.terreno[2] == 'M' || (sensores.terreno[2] == 'A' && !bikini) || (sensores.terreno[2] == 'B' && !zapatillas) || sensores.terreno[6] == 'M' || (sensores.terreno[6] == 'A' && !bikini) || (sensores.terreno[6] == 'B' && !zapatillas)))
			puede = false;
		else

			break;
	case 7:
		if (!huir && ((sensores.terreno[2] == 'M') || (sensores.terreno[2] == 'A' && !bikini) || (sensores.terreno[2] == 'B' && !zapatillas) || (sensores.terreno[6] == 'M') || (sensores.terreno[6] == 'A' && !bikini) || (sensores.terreno[6] == 'B' && !zapatillas) || sensores.terreno[7] == 'M' || (sensores.terreno[7] == 'A' && !bikini) || (sensores.terreno[7] == 'B' && !zapatillas)))
			puede = false;
		else

			break;
	case 8:
		if (!huir && ((sensores.terreno[2] == 'M') || (sensores.terreno[2] == 'A' && !bikini) || (sensores.terreno[2] == 'B' && !zapatillas) || (sensores.terreno[6] == 'M') || (sensores.terreno[6] == 'A' && !bikini) || (sensores.terreno[6] == 'B' && !zapatillas) || (sensores.terreno[7] == 'M') || (sensores.terreno[7] == 'A' && !bikini) || (sensores.terreno[7] == 'B' && !zapatillas) || sensores.terreno[8] == 'M' || (sensores.terreno[8] == 'A' && !bikini) || (sensores.terreno[8] == 'B' && !zapatillas)))
			puede = false;
		else

			break;
	case 9:
		if (!huir && ((sensores.terreno[2] == 'M') || (sensores.terreno[2] == 'A' && !bikini) || (sensores.terreno[2] == 'B' && !zapatillas) || (sensores.terreno[6] == 'M') || (sensores.terreno[6] == 'A' && !bikini) || (sensores.terreno[6] == 'B' && !zapatillas) || (sensores.terreno[12] == 'M') || (sensores.terreno[12] == 'A' && !bikini) || (sensores.terreno[12] == 'B' && !zapatillas) || (sensores.terreno[11] == 'M') || (sensores.terreno[11] == 'A' && !bikini) || (sensores.terreno[11] == 'B' && !zapatillas) || (sensores.terreno[10] == 'M') || (sensores.terreno[10] == 'A' && !bikini) || (sensores.terreno[10] == 'B' && !zapatillas) || sensores.terreno[9] == 'M' || (sensores.terreno[9] == 'A' && !bikini) || (sensores.terreno[9] == 'B' && !zapatillas)))
			puede = false;

		break;
	case 10:
		if (!huir && ((sensores.terreno[2] == 'M') || (sensores.terreno[2] == 'A' && !bikini) || (sensores.terreno[2] == 'B' && !zapatillas) || (sensores.terreno[6] == 'M') || (sensores.terreno[6] == 'A' && !bikini) || (sensores.terreno[6] == 'B' && !zapatillas) || (sensores.terreno[12] == 'M') || (sensores.terreno[12] == 'A' && !bikini) || (sensores.terreno[12] == 'B' && !zapatillas) || (sensores.terreno[11] == 'M') || (sensores.terreno[11] == 'A' && !bikini) || (sensores.terreno[11] == 'B' && !zapatillas) || sensores.terreno[10] == 'M' || (sensores.terreno[10] == 'A' && !bikini) || (sensores.terreno[10] == 'B' && !zapatillas)))
			puede = false;

		break;
	case 11:
		if (!huir && ((sensores.terreno[2] == 'M') || (sensores.terreno[2] == 'A' && !bikini) || (sensores.terreno[2] == 'B' && !zapatillas) || (sensores.terreno[6] == 'M') || (sensores.terreno[6] == 'A' && !bikini) || (sensores.terreno[6] == 'B' && !zapatillas) || (sensores.terreno[12] == 'M') || (sensores.terreno[12] == 'A' && !bikini) || (sensores.terreno[12] == 'B' && !zapatillas) || sensores.terreno[11] == 'M' || (sensores.terreno[11] == 'A' && !bikini) || (sensores.terreno[11] == 'B' && !zapatillas)))
			puede = false;

		break;
	case 12:
		if (!huir && ((sensores.terreno[2] == 'M') || (sensores.terreno[2] == 'A' && !bikini) || (sensores.terreno[2] == 'B' && !zapatillas) || (sensores.terreno[6] == 'M') || (sensores.terreno[6] == 'A' && !bikini) || (sensores.terreno[6] == 'B' && !zapatillas) || sensores.terreno[12] == 'M' || (sensores.terreno[12] == 'A' && !bikini) || (sensores.terreno[12] == 'B' && !zapatillas)))
			puede = false;

		break;
	case 13:
		if (!huir && ((sensores.terreno[2] == 'M') || (sensores.terreno[2] == 'A' && !bikini) || (sensores.terreno[2] == 'B' && !zapatillas) || (sensores.terreno[6] == 'M') || (sensores.terreno[6] == 'A' && !bikini) || (sensores.terreno[6] == 'B' && !zapatillas) || (sensores.terreno[12] == 'M') || (sensores.terreno[12] == 'A' && !bikini) || (sensores.terreno[12] == 'B' && !zapatillas) || sensores.terreno[13] == 'M' || (sensores.terreno[13] == 'A' && !bikini) || (sensores.terreno[13] == 'B' && !zapatillas)))
			puede = false;

		break;
	case 14:
		if (!huir && ((sensores.terreno[2] == 'M') || (sensores.terreno[2] == 'A' && !bikini) || (sensores.terreno[2] == 'B' && !zapatillas) || (sensores.terreno[6] == 'M') || (sensores.terreno[6] == 'A' && !bikini) || (sensores.terreno[6] == 'B' && !zapatillas) || (sensores.terreno[12] == 'M') || (sensores.terreno[12] == 'A' && !bikini) || (sensores.terreno[12] == 'B' && !zapatillas) || (sensores.terreno[13] == 'M') || (sensores.terreno[13] == 'A' && !bikini) || (sensores.terreno[13] == 'B' && !zapatillas) || sensores.terreno[14] == 'M' || (sensores.terreno[14] == 'A' && !bikini) || (sensores.terreno[14] == 'B' && !zapatillas)))
			puede = false;

		break;
	case 15:
		if (!huir && ((sensores.terreno[2] == 'M') || (sensores.terreno[2] == 'A' && !bikini) || (sensores.terreno[2] == 'B' && !zapatillas) || (sensores.terreno[6] == 'M') || (sensores.terreno[6] == 'A' && !bikini) || (sensores.terreno[6] == 'B' && !zapatillas) || (sensores.terreno[12] == 'M') || (sensores.terreno[12] == 'A' && !bikini) || (sensores.terreno[12] == 'B' && !zapatillas) || (sensores.terreno[13] == 'M') || (sensores.terreno[13] == 'A' && !bikini) || (sensores.terreno[13] == 'B' && !zapatillas) || (sensores.terreno[14] == 'M') || (sensores.terreno[14] == 'A' && !bikini) || (sensores.terreno[14] == 'B' && !zapatillas) || sensores.terreno[15] == 'M' || (sensores.terreno[15] == 'A' && !bikini) || (sensores.terreno[15] == 'B' && !zapatillas)))
			puede = false;
		break;
	default:
		break;
	}

	return puede;
}

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}

bool ComportamientoJugador::puedeIr(Sensores sensores, int dest)
{
	bool puede = false;
	switch (dest)
	{
	case 1:
		if (sensores.terreno[2] != 'M' && sensores.terreno[2] != 'P' && sensores.terreno[1] != 'M' && sensores.terreno[1] != 'P')
			puede = true;
		break;
	case 2:
		if (sensores.terreno[2] != 'M' && sensores.terreno[2] != 'P')
			puede = true;
		break;
	case 3:
		if (sensores.terreno[2] != 'M' && sensores.terreno[2] != 'P' && sensores.terreno[3] != 'M' && sensores.terreno[3] != 'P')
			puede = true;
		break;
	case 4:
		if (sensores.terreno[2] != 'M' && sensores.terreno[2] != 'P' && sensores.terreno[6] != 'M' && sensores.terreno[6] != 'P' && sensores.terreno[5] != 'M' && sensores.terreno[5] != 'P' && sensores.terreno[4] != 'M' && sensores.terreno[4] != 'P')
			puede = true;
		break;
	case 5:
		if (sensores.terreno[2] != 'M' && sensores.terreno[2] != 'P' && sensores.terreno[6] != 'M' && sensores.terreno[6] != 'P' && sensores.terreno[5] != 'M' && sensores.terreno[5] != 'P')
			puede = true;
		break;
	case 6:
		if (sensores.terreno[2] != 'M' && sensores.terreno[2] != 'P' && sensores.terreno[6] != 'M' && sensores.terreno[6] != 'P')
			puede = true;
		break;
	case 7:
		if (sensores.terreno[2] != 'M' && sensores.terreno[2] != 'P' && sensores.terreno[6] != 'M' && sensores.terreno[6] != 'P' && sensores.terreno[7] != 'M' && sensores.terreno[7] != 'P')
			puede = true;
		break;
	case 8:
		if (sensores.terreno[2] != 'M' && sensores.terreno[2] != 'P' && sensores.terreno[6] != 'M' && sensores.terreno[6] != 'P' && sensores.terreno[7] != 'M' && sensores.terreno[7] != 'P' && sensores.terreno[8] != 'M' && sensores.terreno[8] != 'P')
			puede = true;
		break;
	case 9:
		if (sensores.terreno[2] != 'M' && sensores.terreno[2] != 'P' && sensores.terreno[6] != 'M' && sensores.terreno[6] != 'P' && sensores.terreno[12] != 'M' && sensores.terreno[12] != 'P' && sensores.terreno[11] != 'M' && sensores.terreno[11] != 'P' && sensores.terreno[10] != 'M' && sensores.terreno[10] != 'P' && sensores.terreno[9] != 'M' && sensores.terreno[9] != 'P')
			puede = true;
		break;
	case 10:
		if (sensores.terreno[2] != 'M' && sensores.terreno[2] != 'P' && sensores.terreno[6] != 'M' && sensores.terreno[6] != 'P' && sensores.terreno[12] != 'M' && sensores.terreno[12] != 'P' && sensores.terreno[11] != 'M' && sensores.terreno[11] != 'P' && sensores.terreno[10] != 'M' && sensores.terreno[10] != 'P')
			puede = true;
		break;
	case 11:
		if (sensores.terreno[2] != 'M' && sensores.terreno[2] != 'P' && sensores.terreno[6] != 'M' && sensores.terreno[6] != 'P' && sensores.terreno[12] != 'M' && sensores.terreno[12] != 'P' && sensores.terreno[11] != 'M' && sensores.terreno[11] != 'P')
			puede = true;
		break;
	case 12:
		if (sensores.terreno[2] != 'M' && sensores.terreno[2] != 'P' && sensores.terreno[6] != 'M' && sensores.terreno[6] != 'P' && sensores.terreno[12] != 'M' && sensores.terreno[12] != 'P')
			puede = true;
		break;
	case 13:
		if (sensores.terreno[2] != 'M' && sensores.terreno[2] != 'P' && sensores.terreno[6] != 'M' && sensores.terreno[6] != 'P' && sensores.terreno[12] != 'M' && sensores.terreno[12] != 'P' && sensores.terreno[13] != 'M' && sensores.terreno[13] != 'P')
			puede = true;
		break;
	case 14:
		if (sensores.terreno[2] != 'M' && sensores.terreno[2] != 'P' && sensores.terreno[6] != 'M' && sensores.terreno[6] != 'P' && sensores.terreno[12] != 'M' && sensores.terreno[12] != 'P' && sensores.terreno[13] != 'M' && sensores.terreno[13] != 'P' && sensores.terreno[14] != 'M' && sensores.terreno[14] != 'P')
			puede = true;
		break;
	case 15:
		if (sensores.terreno[2] != 'M' && sensores.terreno[2] != 'P' && sensores.terreno[6] != 'M' && sensores.terreno[6] != 'P' && sensores.terreno[12] != 'M' && sensores.terreno[12] != 'P' && sensores.terreno[13] != 'M' && sensores.terreno[13] != 'P' && sensores.terreno[14] != 'M' && sensores.terreno[14] != 'P' && sensores.terreno[15] != 'M' && sensores.terreno[15] != 'P')
			puede = true;
		break;

	default:
		break;
	}
	return puede;
}

bool ComportamientoJugador::loboCerca(Sensores sensores){
	bool lobo = false;
	for(int i=1; i<4; i++){
		if(sensores.superficie[i] == 'l')
			lobo = true;
	}
	return lobo;
}

bool ComportamientoJugador::puedeIrSinEquipar(Sensores sensores, int dest)
{
	bool puede = false;
	switch (dest)
	{
	case 1:
		if (sensores.terreno[2] != 'M' && sensores.terreno[2] != 'P' && sensores.terreno[1] != 'M' && sensores.terreno[1] != 'P')
		{
			puede = true;
			if (sensores.terreno[2] == 'A' && !bikini || sensores.terreno[2] == 'B' && !zapatillas || sensores.terreno[1] == 'A' && !bikini || sensores.terreno[1] == 'B' && !zapatillas)
				puede = false;
		}
		break;
	case 2:
		if (sensores.terreno[2] != 'M' && sensores.terreno[2] != 'P')
		{
			puede = true;
			if (sensores.terreno[2] == 'A' && !bikini || sensores.terreno[2] == 'B' && !zapatillas)
				puede = false;
		}
		break;
	case 3:
		if (sensores.terreno[2] != 'M' && sensores.terreno[2] != 'P' && sensores.terreno[3] != 'M' && sensores.terreno[3] != 'P')
		{
			puede = true;
			if (sensores.terreno[2] == 'A' && !bikini || sensores.terreno[2] == 'B' && !zapatillas || sensores.terreno[3] == 'A' && !bikini || sensores.terreno[3] == 'B' && !zapatillas)
				puede = false;
		}
		break;
	case 4:
		if (sensores.terreno[2] != 'M' && sensores.terreno[2] != 'P' && sensores.terreno[6] != 'M' && sensores.terreno[6] != 'P' && sensores.terreno[5] != 'M' && sensores.terreno[5] != 'P' && sensores.terreno[4] != 'M' && sensores.terreno[4] != 'P')
		{
			puede = true;
			if (sensores.terreno[2] == 'A' && !bikini || sensores.terreno[2] == 'B' && !zapatillas || sensores.terreno[6] == 'A' && !bikini || sensores.terreno[6] == 'B' && !zapatillas || sensores.terreno[5] == 'A' && !bikini || sensores.terreno[5] == 'B' && !zapatillas || sensores.terreno[4] == 'A' && !bikini || sensores.terreno[4] == 'B' && !zapatillas)
				puede = false;
		}
		break;
	case 5:
		if (sensores.terreno[2] != 'M' && sensores.terreno[2] != 'P' && sensores.terreno[6] != 'M' && sensores.terreno[6] != 'P' && sensores.terreno[5] != 'M' && sensores.terreno[5] != 'P')
		{
			puede = true;
			if (sensores.terreno[2] == 'A' && !bikini || sensores.terreno[2] == 'B' && !zapatillas || sensores.terreno[6] == 'A' && !bikini || sensores.terreno[6] == 'B' && !zapatillas || sensores.terreno[5] == 'A' && !bikini || sensores.terreno[5] == 'B' && !zapatillas)
				puede = false;
		}
		break;
	case 6:
		if (sensores.terreno[2] != 'M' && sensores.terreno[2] != 'P' && sensores.terreno[6] != 'M' && sensores.terreno[6] != 'P')
		{
			puede = true;
			if (sensores.terreno[2] == 'A' && !bikini || sensores.terreno[2] == 'B' && !zapatillas || sensores.terreno[6] == 'A' && !bikini || sensores.terreno[6] == 'B' && !zapatillas)
				puede = false;
		}
		break;
	case 7:
		if (sensores.terreno[2] != 'M' && sensores.terreno[2] != 'P' && sensores.terreno[6] != 'M' && sensores.terreno[6] != 'P' && sensores.terreno[7] != 'M' && sensores.terreno[7] != 'P')
		{
			puede = true;
			if (sensores.terreno[2] == 'A' && !bikini || sensores.terreno[2] == 'B' && !zapatillas || sensores.terreno[6] == 'A' && !bikini || sensores.terreno[6] == 'B' && !zapatillas || sensores.terreno[7] == 'A' && !bikini || sensores.terreno[7] == 'B' && !zapatillas)
				puede = false;
		}
		break;
	case 8:
		if (sensores.terreno[2] != 'M' && sensores.terreno[2] != 'P' && sensores.terreno[6] != 'M' && sensores.terreno[6] != 'P' && sensores.terreno[7] != 'M' && sensores.terreno[7] != 'P' && sensores.terreno[8] != 'M' && sensores.terreno[8] != 'P')
		{
			puede = true;
			if (sensores.terreno[2] == 'A' && !bikini || sensores.terreno[2] == 'B' && !zapatillas || sensores.terreno[6] == 'A' && !bikini || sensores.terreno[6] == 'B' && !zapatillas || sensores.terreno[7] == 'A' && !bikini || sensores.terreno[7] == 'B' && !zapatillas || sensores.terreno[8] == 'A' && !bikini || sensores.terreno[8] == 'B' && !zapatillas)
				puede = false;
		}
		break;
	case 9:
		if (sensores.terreno[2] != 'M' && sensores.terreno[2] != 'P' && sensores.terreno[6] != 'M' && sensores.terreno[6] != 'P' && sensores.terreno[12] != 'M' && sensores.terreno[12] != 'P' && sensores.terreno[11] != 'M' && sensores.terreno[11] != 'P' && sensores.terreno[10] != 'M' && sensores.terreno[10] != 'P' && sensores.terreno[9] != 'M' && sensores.terreno[9] != 'P')
		{
			puede = true;
			if (sensores.terreno[2] == 'A' && !bikini || sensores.terreno[2] == 'B' && !zapatillas || sensores.terreno[6] == 'A' && !bikini || sensores.terreno[6] == 'B' && !zapatillas || sensores.terreno[12] == 'A' && !bikini || sensores.terreno[12] == 'B' && !zapatillas || sensores.terreno[11] == 'A' && !bikini || sensores.terreno[11] == 'B' && !zapatillas || sensores.terreno[10] == 'A' && !bikini || sensores.terreno[10] == 'B' && !zapatillas || sensores.terreno[9] == 'A' && !bikini || sensores.terreno[9] == 'B' && !zapatillas)
				puede = false;
		}
		break;
	case 10:
		if (sensores.terreno[2] != 'M' && sensores.terreno[2] != 'P' && sensores.terreno[6] != 'M' && sensores.terreno[6] != 'P' && sensores.terreno[12] != 'M' && sensores.terreno[12] != 'P' && sensores.terreno[11] != 'M' && sensores.terreno[11] != 'P' && sensores.terreno[10] != 'M' && sensores.terreno[10] != 'P')
		{
			puede = true;
			if (sensores.terreno[2] == 'A' && !bikini || sensores.terreno[2] == 'B' && !zapatillas || sensores.terreno[6] == 'A' && !bikini || sensores.terreno[6] == 'B' && !zapatillas || sensores.terreno[12] == 'A' && !bikini || sensores.terreno[12] == 'B' && !zapatillas || sensores.terreno[11] == 'A' && !bikini || sensores.terreno[11] == 'B' && !zapatillas || sensores.terreno[10] == 'A' && !bikini || sensores.terreno[10] == 'B' && !zapatillas)
				puede = false;
		}
		break;
	case 11:
		if (sensores.terreno[2] != 'M' && sensores.terreno[2] != 'P' && sensores.terreno[6] != 'M' && sensores.terreno[6] != 'P' && sensores.terreno[12] != 'M' && sensores.terreno[12] != 'P' && sensores.terreno[11] != 'M' && sensores.terreno[11] != 'P')
		{
			puede = true;
			if (sensores.terreno[2] == 'A' && !bikini || sensores.terreno[2] == 'B' && !zapatillas || sensores.terreno[6] == 'A' && !bikini || sensores.terreno[6] == 'B' && !zapatillas || sensores.terreno[12] == 'A' && !bikini || sensores.terreno[12] == 'B' && !zapatillas || sensores.terreno[11] == 'A' && !bikini || sensores.terreno[11] == 'B' && !zapatillas)
				puede = false;
		}
		break;
	case 12:
		if (sensores.terreno[2] != 'M' && sensores.terreno[2] != 'P' && sensores.terreno[6] != 'M' && sensores.terreno[6] != 'P' && sensores.terreno[12] != 'M' && sensores.terreno[12] != 'P')
		{
			puede = true;
			if (sensores.terreno[2] == 'A' && !bikini || sensores.terreno[2] == 'B' && !zapatillas || sensores.terreno[6] == 'A' && !bikini || sensores.terreno[6] == 'B' && !zapatillas || sensores.terreno[12] == 'A' && !bikini || sensores.terreno[12] == 'B' && !zapatillas)
				puede = false;
		}
		break;
	case 13:
		if (sensores.terreno[2] != 'M' && sensores.terreno[2] != 'P' && sensores.terreno[6] != 'M' && sensores.terreno[6] != 'P' && sensores.terreno[12] != 'M' && sensores.terreno[12] != 'P' && sensores.terreno[13] != 'M' && sensores.terreno[13] != 'P')
		{
			puede = true;
			if (sensores.terreno[2] == 'A' && !bikini || sensores.terreno[2] == 'B' && !zapatillas || sensores.terreno[6] == 'A' && !bikini || sensores.terreno[6] == 'B' && !zapatillas || sensores.terreno[12] == 'A' && !bikini || sensores.terreno[12] == 'B' && !zapatillas || sensores.terreno[13] == 'A' && !bikini || sensores.terreno[13] == 'B' && !zapatillas)
				puede = false;
		}
		break;
	case 14:
		if (sensores.terreno[2] != 'M' && sensores.terreno[2] != 'P' && sensores.terreno[6] != 'M' && sensores.terreno[6] != 'P' && sensores.terreno[12] != 'M' && sensores.terreno[12] != 'P' && sensores.terreno[13] != 'M' && sensores.terreno[13] != 'P' && sensores.terreno[14] != 'M' && sensores.terreno[14] != 'P')
		{
			puede = true;
			if (sensores.terreno[2] == 'A' && !bikini || sensores.terreno[2] == 'B' && !zapatillas || sensores.terreno[6] == 'A' && !bikini || sensores.terreno[6] == 'B' && !zapatillas || sensores.terreno[12] == 'A' && !bikini || sensores.terreno[12] == 'B' && !zapatillas || sensores.terreno[13] == 'A' && !bikini || sensores.terreno[13] == 'B' && !zapatillas || sensores.terreno[14] == 'A' && !bikini || sensores.terreno[14] == 'B' && !zapatillas)
				puede = false;
		}
		break;
	case 15:
		if (sensores.terreno[2] != 'M' && sensores.terreno[2] != 'P' && sensores.terreno[6] != 'M' && sensores.terreno[6] != 'P' && sensores.terreno[12] != 'M' && sensores.terreno[12] != 'P' && sensores.terreno[13] != 'M' && sensores.terreno[13] != 'P' && sensores.terreno[14] != 'M' && sensores.terreno[14] != 'P' && sensores.terreno[15] != 'M' && sensores.terreno[15] != 'P')
		{
			puede = true;
			if (sensores.terreno[2] == 'A' && !bikini || sensores.terreno[2] == 'B' && !zapatillas || sensores.terreno[6] == 'A' && !bikini || sensores.terreno[6] == 'B' && !zapatillas || sensores.terreno[12] == 'A' && !bikini || sensores.terreno[12] == 'B' && !zapatillas || sensores.terreno[13] == 'A' && !bikini || sensores.terreno[13] == 'B' && !zapatillas || sensores.terreno[14] == 'A' && !bikini || sensores.terreno[14] == 'B' && !zapatillas || sensores.terreno[15] == 'A' && !bikini || sensores.terreno[15] == 'B' && !zapatillas)
				puede = false;
		}
		break;

	default:
		break;
	}
	return puede;
}