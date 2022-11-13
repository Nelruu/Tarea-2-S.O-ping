#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <thread>
#include <unistd.h>
#include <memory>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <array>

using namespace std;

/*
    los datos de los pings se guarda  en la estructura

*/
class Ping
{
public:

    string Estado;
    string ip;
    int ipEnviada;
    int ipRecibida;
    int ipPerdida;

};

/*
se crean los pings
*/
void createPing(Ping *ping, string cant)
{
    // se crean el comando de ping
    string command = "ping " + ping->ip + " -c " + cant + " -q";

    /*
        se generan los pings y recuperar el output del comando ping y se guarda en result

    */
    std::array<char, 128> buffer;
    std::string Resultado;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        Resultado += buffer.data();
    }
    // si falla el ping el result saldra vacio
    if (Resultado == "")
    {
        throw std::runtime_error("El ping fallo");
    }


    /*
        la funcion recorre buscando los espacios,
    */
    char *aux;
    char *CantidadRecibida;

    // de String a char * para para evitar errores
    char *output = new char[Resultado.length() + 1];

    /*
        trtok copia los valores de result en output
    */
    strcpy(output, Resultado.c_str());
    aux = strtok(output, " ");
    do
    {
        // strtok guarda los caracteres del output
        aux = strtok(NULL, " ");
        // compruebar los paquetes recibidos
        if (strcmp(aux, "received,") == 0)
        {
            // Si se encuenta se guarda
            ping->ipRecibida = atoi(CantidadRecibida);
        }
        else
        {
            // guarda el valor de strtok
            CantidadRecibida = aux;
        }
    } while (strcmp(aux, "received,") != 0);

    // guarda el numero paquetes enviados
    ping->ipEnviada = atoi(cant.c_str());

    // guarda el numero paquetes enviados
    ping->ipPerdida = ping->ipEnviada - ping->ipRecibida;

    if (ping->ipRecibida >= 1)
    {
        ping->Estado = "UP";
    }
    else
    {
        ping->Estado = "DOWN";
    }

    // imprime
    if (ping->ip.length() >= 15)
    {
        cout << ping->ip << " \t" << ping->ipEnviada << " \t" << ping->ipRecibida << " \t" << ping->ipPerdida << " \t" << ping->Estado << "\n";
    }
    else
    {
        cout << ping->ip << " \t\t" << ping->ipEnviada << " \t" << ping->ipRecibida << " \t" << ping->ipPerdida << " \t" << ping->Estado << "\n";
    }
}

int main(int argc, char *argv[])
{
    // comprueba que se haya ingresado
    if (argc < 2)
    {
        cout << "argumentos bajos \n";
        return 1;
    }
    string nombreArchivo = argv[1];
    if (nombreArchivo.find(".txt") == std::string::npos)
    {
        cout << "archivo valido (.txt)" << endl;
        return 1;
    }

    try
    {
        int numero = stoi(argv[2]);
    }
    catch (std::exception const & e)
    {
        cout << "numero invalido" << '\n';
        return 1;
    }

    // se transforma en string
    string cantPackages = argv[2];
    
    // se habre el archivo
    std::ifstream ipFile;
    ipFile.open(argv[1]);

    // manejo de strings
    std::string ip;
    int count = 0;
    // if si el archivo esta abierto
    if (ipFile.is_open())
    {
        // se guardan las ip
        while (ipFile.good())
        {
            // se guardan las ip
            ipFile >> ip;
            count++;
        }
    }

    // se guardan las ips en el array de pings
    ipFile.close();
    ipFile.open(argv[1]);

    // creacion de array de pings
    Ping pings[count];
    count = 0;
    if (ipFile.is_open())
    {
        while (ipFile.good())
        {
            ipFile >> ip;
            pings[count].ip = ip;
            count++;
        }
    }
    /* hilos */
    //  Se crean los hilos
    thread threads[count];

    // se imprime
    cout << "IP \t\t   Trans. \tRec. \t Perd. \t Estado \n";
    cout << "--------------------------------------------------------\n";

    // Cantidad restante
    for (int i = 0; i < count; i++)
    {
        threads[i] = std::thread(&createPing, &pings[i], cantPackages);
        // restantes--;
    }

    // pasan los hilos al main
    for (int i = 0; i < count; i++)
    {
        threads[i].join();
    }

    return 0;
}