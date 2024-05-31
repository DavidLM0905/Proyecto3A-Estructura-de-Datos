#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#include "HuffmanDeCompressor.hpp"

using namespace std;

int main(int argc, char* argv[]) {

    cout << "=================================================" << endl;
    cout << "Bienvenido al compresor y decompresor de archivos" << endl;
    cout << "=================================================" << endl << endl;

    while (true) {
        cout << "Porfavor digite la opcion que desea utilizar: " << endl << endl;
        cout << "1. Compresor de archivos " << endl;
        cout << "2. Decompresor de archivos " << endl;
        cout << "3. Salir " << endl;
        cout << "Opcion: ";

        int opcion;
        cin >> opcion;

        if (opcion == 3) {
            break;
        }

        string archivo;

        switch (opcion) {

        case 1:

            cout << "Porfavor ingrese el nombre del archivo junto con la extencion del archivo, por ejemplo: MiArchivo.txt" << endl;
            cout << "Nombre del archivo: ";

            cin >> archivo;

            huffmanCompressor(archivo);

            system("pause");
            system("cls");

            break;

        case 2:
            cout << "Porfavor ingrese el nombre del archivo junto con la extencion del archivo, por ejemplo: MiArchivo.txt.huf" << endl;
            cout << "Nombre del archivo: ";

            cin >> archivo;

            huffmanDecompressor(archivo);

            system("pause");
            system("cls");

            break;

        default:
            cout << "Opcion no valida. Por favor intente de nuevo." << endl;
            break;

        }
    }

    return 0;
}