#ifndef _TARR_BITWRITER_
#define _TARR_BITWRITER_

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "strings.hpp"
#include "files.hpp"

using namespace std;

struct BitWriter
{
   FILE* f;
   string s;
   int pos;
};

BitWriter bitWriter(FILE* f)
{
   return {f,"",0};
}

void bitWriterWrite(BitWriter& bw,int bit)
{
   bw.s += intToString(bit);
   bw.pos++;
}

void bitWriterWrite(BitWriter& bw,string sbit)
{
   bw.s += sbit;
   bw.pos += length(sbit);
}

void bitWriterFlush(BitWriter& bw)
{
   seek<char>(bw.f,fileSize<char>(bw.f));   // Se mueve el puntero al final del archivo
   while( bw.pos%8 != 0 )   //Inicia un bucle que continuara mientras bw.pos no sea un multiplo de 8
   {
      bitWriterWrite(bw,0); // para escribir ceros adicionales hasta que bw.pos este alineado a un byte completo.
   }
   while(bw.s != "")    // Inicia un segundo bucle que continuara mientras bw.s no este vacio. bw.s es la cadena de bits que se han acumulado pero no se han escrito aun al archivo
   {
      char a = stringToInt(substring(bw.s,0,8),2); // Obtiene una subcadena de los primeros 8 bits, convierte la subcadena binaria a un entero en base 2
      bw.s = substring(bw.s,8,length(bw.s)); // Elimina los primeros 8 bits de bw.s, actualizando bw.s a la subcadena que comienza desde el noveno bit en adelante 
      write<char>(bw.f,a); // Escribe los caracteres en el archivo
   }
   bw.pos = 0;  // Se reinicia la posición bw.pos a 0, ya que se han escrito todos los bits acumulados y el BitWriter está listo para empezar de nuevo
}

#endif
