#include <iostream>
#include <string>
#include <string.h>

#include "include/library/cppBL.hpp"
#include "HuffmanSetup.hpp"

// Retorna la cantidad de bytes de un archivo

int fileBytes(FILE* f) {
   return fileSize<unsigned char>(f);
}

// init function

HuffmanTable huffmanTable(unsigned int n, string cod) { // Inicializa la tabla de huffman

   HuffmanTable hT; 
   hT.n = n;
   hT.cod = cod;

   return hT;
}

int cmpHuffmanTreeInfoOrder(HuffmanTreeInfo* a,HuffmanTreeInfo* b) {    // Un comparador de nodos del arbol

    int frst = a->n-b->n;
    int scnd = a->c-b->c;

    if(frst == 0) {
        return scnd;
    }

    return frst;
}

int cmpHuffmanTreeInfoFind(HuffmanTreeInfo* a, unsigned int b) {
    return a->c-b;
}

int cmpHuffmanTableFind(HuffmanTable a, unsigned char b) {
    return a.n-b;
}

int cmpHuffmanTableFind2(HuffmanTable a, string cod) {
    return cmpString(a.cod,cod);
}

int cmpHuffmanTableOrder(HuffmanTable a, HuffmanTable b) {
    return length(a.cod)-length(b.cod);
}

void byteDiscover(unsigned int c,List<HuffmanTreeInfo*>& lByte) {   // Cuenta las ocurrencias de un byte o los agrega a la lista

    HuffmanTreeInfo** hti = listFind <HuffmanTreeInfo*, unsigned int> (lByte, c, cmpHuffmanTreeInfoFind); // Se busca el nodo de la lista que contenga el byte

    if (hti == NULL) {  // En caso de encontrarse el nodo

        HuffmanTreeInfo* x = huffmanTreeInfo(c, 0, NULL, NULL); // Crear un nodo del arbol de huffman
        hti = listAdd<HuffmanTreeInfo*>(lByte, x);  // Agregar a la lista el byte
    }

    (*hti) -> n++; // Se aumenta la cantidad de ocurrencias
}

List<HuffmanTreeInfo*> createByteList(FILE* f) {    // Generar la lista de bytes con sus ocurrencias a partir de un archivo

    List<HuffmanTreeInfo*> lByte = list <HuffmanTreeInfo*>();   // Se inicializa la lista de ocurrencias
    unsigned int c = (unsigned int) read <unsigned char>(f);    // Se lee un byte del archivo

    while(!feof(f)) {   // Mientras no se haya leido todo el archivo

        byteDiscover(c, lByte);     //Se agrega un byte a la lista de ocurrencias o se cuenta el byte
        c = (unsigned int) read<unsigned char>(f);      // Se lee el siguiente byte
    }

    return lByte;   // Se retorna la lista de ocurrencias
}

HuffmanTreeInfo* byteListToHuffmanTree(List<HuffmanTreeInfo*> lByte) {   // Convierte la lista de bytes y sus ocurrencias en un arbol de Huffman

    listSort<HuffmanTreeInfo*>(lByte, cmpHuffmanTreeInfoOrder); // Se ordena la lista por ocurrencias

    while(listSize(lByte) > 1) {    // Mientras el tamaño de la lista sea mayor a uno

        HuffmanTreeInfo* lft = listRemoveFirst<HuffmanTreeInfo*>(lByte);   // Se van removiendo elementos de la lista y se asignan a izq y drch
        HuffmanTreeInfo* rgt = listRemoveFirst<HuffmanTreeInfo*>(lByte);

        HuffmanTreeInfo* ast = huffmanTreeInfo(256, rgt->n + lft->n, lft,rgt);  // Se crea un nodo el cual no es un byte si no la representacion de la suma de las frecuencias y los punteros a los nodos hijos
        listOrderedInsert<HuffmanTreeInfo*>(lByte, ast, cmpHuffmanTreeInfoOrder);  //  Inserta el nuevo nodo ast de vuelta en la lista lByte de manera ordenada
    }

    // Esto se para crear el arbol de abajo hacia arriba

    HuffmanTreeInfo* root = lByte.p->info;  // Asigna al nodo restante en la lista como la raíz del arbol de Huffman
    listFree<HuffmanTreeInfo*>(lByte);  // Se elimina la lista

    return root;    // Se devuelve la raiz
}

List<HuffmanTable> rootToHuffmanTable(HuffmanTreeInfo* root) {  // Convierte la raíz de un arbol de Huffman en una lista de tablas de Huffman

    HuffmanTree hTree = huffmanTree(root);  // Se inicializa el arbol con el nodo raiz
    List<HuffmanTable> lHTable = list<HuffmanTable>();  // Se inicializa una tabla de nodos
    string cod; // Usado para almacenar los codigos generados

    while(huffmanTreeHasNext(hTree)) {// Mientras haya nodos sin procesar 

        HuffmanTreeInfo* hTINext = huffmanTreeNext(hTree, cod); // Se obtiene el siguiente nodo y se actualiza el codigo cod 
        HuffmanTable hTable = huffmanTable(hTINext->c, cod);    // Crea un nuevo elemento con el caracter, y el codigo

        listAdd<HuffmanTable>(lHTable, hTable); //Se agrega el elemento a la tabla 
    }

    listSort<HuffmanTable>(lHTable, cmpHuffmanTableOrder);  // Se ordena la tabla por longitud de los codigos
    return lHTable; // Se retorna la tabla
}

unsigned char codSize(HuffmanTable* x) {    // Retorna el tamaño de un codigo Huffman.

    int a = length(x->cod);

    return (unsigned char)a;
}

void generateMetadata(FILE* f, FILE* fHuf, List<HuffmanTable> lHTable) { // Genera los metadatos para el archivo comprimido

    unsigned short leafCount = listSize<HuffmanTable>(lHTable);   // Se obtiene el numero de elementos
    write<unsigned short>(fHuf, leafCount); // Se escribe el numero de elementos
    listReset<HuffmanTable>(lHTable);   // Resetea el iterador de la lista para recorrerla desde el principio

    while(listHasNext<HuffmanTable>(lHTable)) { // Iterar sobre la lista

        HuffmanTable* x = listNext<HuffmanTable>(lHTable); // Siguiente elemento de la lista
        unsigned short cS = codSize(x); // El tamaño del codigo 
        unsigned int codNum = stringToInt(x->cod, 2); // Convierte el codigo a binario 
        write<unsigned char>(fHuf, x->n); // Se escribe el byte
        write<unsigned short>(fHuf, cS); // Se escribe el tamaño del codigo
        write<unsigned int>(fHuf, codNum);  // Se escribe el codigo en biario
    }

    unsigned int fSize = fileBytes(f);  // Calcula el tamaño del archivo original del archivo
    write<unsigned int>(fHuf, fSize);   // Escribe el tamaño del archivo original
}

string compressByte(unsigned char c, List<HuffmanTable> lHTable) {  // Comprime un byte utilizando las tablas de Huffman

    HuffmanTable* hTable = listFind<HuffmanTable,unsigned char>(lHTable, c, cmpHuffmanTableFind); // Se obtiene el dono del byte de la tabla

    return hTable->cod; // Se retorna el codigo
}

void loadCompressedData(FILE* f, FILE* fHuf, List<HuffmanTable> lHTable) {  // Carga los datos comprimidos en un archivo
    
    seek<int>(f, 0);        // Mueve el puntero del archivo de entrada al inicio
    BitWriter bW = bitWriter(fHuf); // Inicializa BitWriter que permite escribir bits en el archivo
    unsigned char c = read<unsigned char>(f);   // Se lee un byte

    // Obtener el tamaño total del archivo
    unsigned int fileSize = fileBytes(f);   

    while (!feof(f)) {  // Mientras no se alcanze el final del archivo

        string cod = compressByte(c, lHTable);  // Se obtine el codigo del byte de la tabla
        bitWriterWrite(bW, cod);    // Se escribe el codigo de Huffman

        if (length(bW.s) % 8 == 0) {    // Si el tamaño del codigo es multiplo de 8 se usa bitWriterFlush
            bitWriterFlush(bW); // Se utiliza para escribir los bits acumulados
        }
        c = read<unsigned char>(f); // Se lee el siguiente byte
    }

    bitWriterFlush(bW); // Se utiliza para escribir los bits acumulados
}

void generateHuffmanFile(FILE* f, List<HuffmanTable> lHTable,string fName) {  //Genera un archivo comprimido

    fName += ".huf";  // Se le agrega la extencion ".huf" al archivo 

    FILE* fHuf = fopen(fName.c_str(),"w+b"); // Se crea el archivo

    // Se generan y escriben los datos del arbol

    generateMetadata(f,fHuf,lHTable); 

    // Se carga la informacion recodificada

    cout << "Comprimiendo archivo..." << endl;

    loadCompressedData(f,fHuf,lHTable);

    fclose(fHuf);

    cout << "Archivo comprimido con exito en: " << fName << endl;

}

void huffmanCompressor(string fName) {  // Metodo para comprimir un archivo

    // Se abre el archivo

    FILE* f = fopen(fName.c_str(),"r+b");

    if (f == nullptr) {
        cout << "Error al abrir el archivo" << std::endl;
        return;
    }

    cout << endl << "Leyendo archivo..." << endl;

    // Se crea la lista de bytes con sus ocurrencias

    List<HuffmanTreeInfo*> lByte = createByteList(f);

    // Se obtiene, a partir de la lista, la raíz del arbol de Huffman

    HuffmanTreeInfo* root = byteListToHuffmanTree(lByte);

    // Se convierte a la raiz en una tabla con los datos de las hojas

    List<HuffmanTable> lHTable = rootToHuffmanTable(root);

    // Se genera el archivo comprimido

    generateHuffmanFile(f,lHTable,fName);

    fclose(f);
}

string regToCod(unsigned short cS,unsigned int codNum) {  // Convierte un registro de codigo en un codigo Huffman

    string cod = _binToString<unsigned int>(codNum); // Convertir de binario a un string
    return substring(cod, (int)(length(cod)-cS));   // Se devuelve el string del codigo
}

List<HuffmanTable> readHuffmanTable(FILE* f) {  // Lee las tablas de Huffman de un archivo comprimido

    List<HuffmanTable> lHTable = list<HuffmanTable>(); // Se inicializa una tabla
    int leafCount = (int)read<unsigned short>(f);   // Se lee la cantidad de elementos del archivo

    for(int i=0; i<leafCount; i++) {    // Se recorren los elementos por el tamaño de la tabla
    
        unsigned char byte = read<unsigned char>(f); // Se lee el byte
        unsigned short cS = read<unsigned short>(f); // Se lee el tamaño del codigo
        unsigned int codNum = read<unsigned int>(f); // Se lee el codigo
        string cod = regToCod(cS, codNum);  // Se convierte el codigo de binario

        listAdd<HuffmanTable>(lHTable,huffmanTable(byte,cod)); // Se agrega al la lista el nodo 
    }

    listSort<HuffmanTable>(lHTable,cmpHuffmanTableOrder); // Se ordena la tabla por logitud de codigos

    return lHTable; // Se retorna la tabla
}

bool isLeaf(HuffmanTreeInfo* aux) {  // Verifica si un nodo en el arbol de Huffman es una hoja

    if(aux->left==NULL && aux->right==NULL) {
        return true;
    }

    return false;
}

unsigned char decode(HuffmanTreeInfo* root,BitReader& bR) {  // Decodifica un byte utilizando un árbol de Huffman

    string provCod;  
    HuffmanTreeInfo* aux = root; // Auxiliar = raiz

    while(!isLeaf(aux)) {  // Ciclo mientras el nodo aux no sea una hoja
        provCod += intToString(bitReaderRead(bR));  // Lee un bit del BitReader, lo convierte en una cadena utilizando intToString y lo agrega a la cadena provCod
        aux = provCod[length(provCod)-1] == '1' ? aux->right : aux->left; // ctualiza el puntero aux para que apunte al hijo derecho (right) o izquierdo (left) del nodo actual, dependiendo del último bit leido en provCod
    }

    return aux->c; // se devuelve el byte representado por esa hoja en el arbol de Huffman.
}

void decompress(FILE* f, HuffmanTreeInfo* root, string fName) {     //Metodo para descomprimir un archivo

    // Eliminar el .huf del nombre del archivo

    fName = substring(fName, 0, length(fName) - 4); 

    //Abrir el archivo

    FILE* fD = fopen(fName.c_str(), "w+b"); 

    if (fD == nullptr) {
        cout << "Error al abrir el archivo" << std::endl;
        return;
    }

    // Lee el tamaño del archivo original que se almaceno en los metadatos del archivo comprimido 
    unsigned int fSize = read<unsigned int>(f); 

    //Inicializa un BitReader para leer los bits del archivo comprimido
    BitReader bR = bitReader(f);

    cout << endl << "Descomprimiendo archivo..." << endl;

    for (int i = 0; i < (int)fSize; i++) { // Un ciclo que se ejecutara por el tamaño del archivo
        unsigned char leafByte = decode(root, bR); // Se decodifica un byte del archivo comprimido utilizando el arbol de Huffman
        write<unsigned char>(fD, leafByte); // Se escribe el byte decodificado en el archivo
    }

    cout << "Archivo descomprimido con exito en: " << fName << endl;

    fclose(fD);
}

void leafPut(HuffmanTreeInfo* aux,HuffmanTable* hT,string cod) { // Coloca una hoja en el arbol de Huffman

    // Si el codigo es 1, la hoja se coloca en el hijo derecho del nodo actual

    if(cod=="1") {
        aux->right = huffmanTreeInfo(hT->n, 0, NULL, NULL);
    }

    //Si el codigo es "0", la hoja se coloca en el hijo izquierdo del nodo actual

    else {
        aux->left = huffmanTreeInfo(hT->n, 0, NULL, NULL);
    }
}

void leafDiscover(HuffmanTable* hT, HuffmanTreeInfo* root) {  // Descubre una hoja en el arbol de Huffman

    string cod = hT->cod;           // Se almacena el codigo del nodo
    HuffmanTreeInfo* aux = root;    // Aux = raiz
    HuffmanTreeInfo* ant = NULL;    // Anterior = NULL

    while(length(cod)>1) {  // Mientras la longitud del codigo sea mayor a 1

        char num = cod[0];  // Extrae el primer caracter del codigo
        cod = removeAt(cod,0);  // Elimina el primer caracter

        ant = aux;  // Se ctualiza ant para que apunte al nodo actual
        aux = num == '1' ? aux->right : aux->left; // Actualiza aux para que apunte al hijo derecho si num es 1, o al hijo izquierdo si num es 0

        if(aux == NULL) { 
            aux = huffmanTreeInfo(256, 0, NULL, NULL); // Si aux es nulo , crea un nuevo nodo con valor 256 y sin hijos, y asigna aux a este nuevo nodo
        }

        if(num=='1') { // Se signa el nuevo nodo aux al hijo derecho de ant si num es 1, o al hijo izquierdo si num es 0
            ant->right = aux;
        }

        else {
            ant->left = aux;
        }
    }

    leafPut(aux,hT,cod); // Se llama a la funcion leafPut para insertar la hoja
}

HuffmanTreeInfo* huffmanTableToRoot(List<HuffmanTable> lHTable) {  // Convierte una lista de tablas de Huffman en la raiz de un arbol de Huffman

    HuffmanTreeInfo* root = huffmanTreeInfo(256, 0, NULL, NULL);  // Se incializa la raiz
    listReset<HuffmanTable>(lHTable);   // Se resetea el iterador de la lista

    while (listHasNext<HuffmanTable>(lHTable)) {    // Mientras haya un siguiente elemento en la tabla

        HuffmanTable* hT = listNext<HuffmanTable>(lHTable); // Se obtiene el siguiente elemento
        leafDiscover(hT, root); // Se insertar el nodo correspondiente en el arbol
    }

    return root; // Se retorna la raiz del arbol
}

void huffmanDecompressor(string fName) {

    // Se abre el archivo

    FILE* f = fopen(fName.c_str(),"r+b");

    // Se interpretan los primeros registros del archivo y se cargan en una tabla

    List<HuffmanTable> lHTable = readHuffmanTable(f);

    // Se convierte a la tabla en raíz del arbol de Huffman

    HuffmanTreeInfo* root = huffmanTableToRoot(lHTable);

    // Se descomprimen y escriben los datos
    
    decompress(f,root,fName);

    fclose(f);
}