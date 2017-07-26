//-------------------------------------------------------------------------//
// 	Analizador l‚xico de LETRA, Solo Programadores (Enero 1996)        //
//-------------------------------------------------------------------------//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lexico.h"

//-------------------------------------------------------------------------//
//      Constantes                                                         //
//-------------------------------------------------------------------------//

#define _tab 9
#define _cr  13
#define _lf  10

//-------------------------------------------------------------------------//
//	Prototipos de funciones						   //
//-------------------------------------------------------------------------//

void traer_caracter (void);

void leer_palabra (void);
void leer_entero (void);
void leer_etiqueta (void);
void leer_literal (char);

int es_alfanumerico (char);
int es_alfabetico (char);
int es_numerico (char);

//---------------------------------------------------------------------------//
// Ahora definimos las diferentes piezas en el mismo orden, las palabras     //
// reservadas se definen para poder identificarlas, el resto solo se definen //
// con el fin de informar de su presencia en el listado generado.            //
//---------------------------------------------------------------------------//

char * piezas[]= { "datos","cierto","falso","si","sino","fin","mientras",
		   "repetir","hasta","ir","hacer","volver","y","o","no",

		   "borrar_pantalla","escribir","leer_tecla","mover_cursor",
		   "color","aleatorio","esperar","leer_caracter",
		   "poner_caracter",

		   "distinto","igual","mayor","mayor o igual","menor",
		   "menor o igual","m s","menos","multiplicar","dividir",
		   "m¢dulo","abrir corchete","cerrar corchete","abrir par‚ntesis",
		   "cerrar par‚ntesis","dos puntos","coma",

		   "identificador","etiqueta","entero","literal",
		   "--- fin del fichero ---","*** PIEZA ERROREA ***" };

//-------------------------------------------------------------------------//
//	Variables globales   						   //
//-------------------------------------------------------------------------//

long linea;                     // N£mero de l¡nea que est  siendo analizada

int pieza_actual;               // Ultima pieza sint ctica analizada

char nombre[long_linea];        // Para el paso de informaci¢n adicional

char * fuente;			// Puntero al fichero fuente cargado

long long_fuente;               // Longitud del fichero fuente

char * indice;                  // Indice de lectura del fichero

int error_lexico;               // Indica el tipo de error lexico producido

//-------------------------------------------------------------------------//
//      Inicializaci¢n del analizador l‚xico                               //
//-------------------------------------------------------------------------//

void inicializacion_lexica (char * fichero) {

FILE * ff;			// Fichero fuente (*.LT)

  // Abrimos el fichero, o emitimos un error si no pudimos

  if ((ff=fopen(fichero,"rb"))==NULL) {
    printf("Error: No se pudo abrir el fichero indicado (%s).",fichero);
    exit(0);
  }

  // Pedimos memoria para cargar el fichero fuente

  fseek(ff,0,SEEK_END);

  long_fuente=ftell(ff);

  if ((fuente=(char *)malloc(long_fuente+1))==NULL) {
    printf("Error: Memoria insuficiente.");
    exit(0);
  }

  fseek(ff,0,SEEK_SET);

  // Cargamos el fichero fuente

  long_fuente=fread(fuente,1,long_fuente,ff);

  fclose(ff);			// Cerramos el fichero

  *(fuente+long_fuente)=_cr;    // A¤adimos un retorno de carro al final

  indice=fuente; 		// Fijamos el ¡ndice de lectura del fuente

  linea=1; 			// Fijamos el n£mero de l¡nea inicial

}

//-------------------------------------------------------------------------//
//      Finalizaci¢n del analisis l‚xico (libera la memoria)               //
//-------------------------------------------------------------------------//

void finalizacion_lexica (void) {

  free(fuente);
}

//-------------------------------------------------------------------------//
//	Analizador l‚xico, obtiene la siguiente pieza del fichero   	   //
//-------------------------------------------------------------------------//

void obtener_pieza (void) {

  traer_caracter();

  if (indice==fuente+long_fuente)
    pieza_actual=p_eof;

  else if (es_alfabetico(tolower(*indice)))
    leer_palabra();

  else if (es_numerico(*indice))
    leer_entero();

  else switch (*indice) {

    case '.': leer_etiqueta(); break;

    case '"': leer_literal('"'); break;

    case '\'': leer_literal('\''); break;

    case '<': if (*++indice=='=') {
		pieza_actual=p_menor_igual; indice++;
	      } else if (*indice=='>') {
		pieza_actual=p_distinto; indice++;
	      } else pieza_actual=p_menor;
	      break;

    case '>': if (*++indice=='=') {
		pieza_actual=p_mayor_igual; indice++;
	      } else pieza_actual=p_mayor;
	      break;

    case '=': pieza_actual=p_igual; indice++; break;

    case '+': pieza_actual=p_mas; indice++; break;

    case '-': pieza_actual=p_menos; indice++; break;

    case '*': pieza_actual=p_multiplicar; indice++; break;

    case '/': pieza_actual=p_dividir; indice++; break;

    case '%': pieza_actual=p_modulo; indice++; break;

    case '[': pieza_actual=p_abrir_corchete; indice++; break;

    case ']': pieza_actual=p_cerrar_corchete; indice++; break;

    case '(': pieza_actual=p_abrir_parentesis; indice++; break;

    case ')': pieza_actual=p_cerrar_parentesis; indice++; break;

    case ':': pieza_actual=p_dos_puntos; indice++; break;

    case ',': pieza_actual=p_coma; indice++; break;

    default: error_lexico=0; pieza_actual=p_error; indice++; break;

  }
}

//-------------------------------------------------------------------------//
//	Avanza espacios, tabulaciones, saltos de linea y comentarios       //
//-------------------------------------------------------------------------//

void traer_caracter (void) {

  while ((*indice==' ' || *indice==_tab || *indice==_cr || *indice==';')
	&& indice<fuente+long_fuente) {

    if (*indice++==';') while (*indice!=_cr) indice++;  // Salta un comentario

    if (*indice==_lf) { indice++; linea++; }            // Salto de l¡nea

  }
}

//-------------------------------------------------------------------------//
//      Lee una palabra, diferencia palabras reservadas e identificadores  //
//-------------------------------------------------------------------------//

void leer_palabra (void) {

  int n=0;

  while (es_alfanumerico(nombre[n]=tolower(*indice))) {
    n++; indice++;
  } nombre[n]=0;

  pieza_actual=p_identificador;

  for (n=p_datos;n<p_distinto;n++)
    if (!strcmp(nombre,piezas[n])) pieza_actual=n;
}

//-------------------------------------------------------------------------//
//	Lee una constante num‚rica entera                                  //
//-------------------------------------------------------------------------//

void leer_entero (void) {

  int n=0;

  while (es_numerico(nombre[n]=*indice)) {
    n++; indice++;
  } nombre[n]=0;

  if (atol(nombre)<=32768) pieza_actual=p_entero;
  else { error_lexico=1; pieza_actual=p_error; }
}

//-------------------------------------------------------------------------//
//	Lee una etiqueta						   //
//-------------------------------------------------------------------------//

void leer_etiqueta (void) {

  int n=0;

  nombre[n++]='.'; indice++;

  while (es_alfanumerico(nombre[n]=tolower(*indice))) {
    n++; indice++;
  } nombre[n]=0;

  pieza_actual=p_etiqueta;
}

//-------------------------------------------------------------------------//
//	Lee un literal, delimitado entre el car cter indicado              //
//-------------------------------------------------------------------------//

void leer_literal (char delimitador) {

  int n=0;

  indice++;

  while ((nombre[n]=*indice)!=delimitador && *indice!=_cr) {
    n++; indice++;
  } nombre[n]=0;

  if (*indice==delimitador) {
    pieza_actual=p_literal; indice++;
  } else { error_lexico=2; pieza_actual=p_error; }
}

//-------------------------------------------------------------------------//
//	Funci¢n para determinar si un car cter es alfanum‚rico             //
//-------------------------------------------------------------------------//

int es_alfanumerico (char c) {

  return( (c>='a' && c<='z') || (c>='0' && c<='9') || c=='_' );

}

//-------------------------------------------------------------------------//
//      Funci¢n para determinar si un car cter es alfab‚tico               //
//-------------------------------------------------------------------------//

int es_alfabetico (char c) {

  return( c>='a' && c<='z' );

}

//-------------------------------------------------------------------------//
//	Funci¢n para determinar si un car cter es num‚rico                 //
//-------------------------------------------------------------------------//

int es_numerico (char c) {

  return( c>='0' && c<='9' );

}
