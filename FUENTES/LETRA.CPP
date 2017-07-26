//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//
//      Compilador de LETRA, Solo Programadores (Agosto 1996)                //
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alloc.h>
#include <dos.h>

#include "lexico.h"

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//
//      Definiciones y constantes                                            //
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//

#define byte unsigned char
#define word unsigned int

#define TRUE 1
#define FALSE 0

#define indice_hash     256
#define max_num_objetos 1024
#define longitud_vnom   12288

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//
//      Prototipos de las funciones de an lisis sint ctico                   //
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//

void an_PROGRAMA(void);
void an_SENTENCIA(void);
void an_ETIQUETA(void);
void an_DECLARACION_DATOS(void);
void an_ASIGNACION(void);
void an_SENT_SI(void);
void an_SENT_MIENTRAS(void);
void an_SENT_REPETIR(void);
void an_SENT_IR(void);
void an_SENT_HACER(void);
void an_SENT_VOLVER(void);
void an_INICIALIZACION_DATO(void);
void an_INICIALIZACION_TABLA(int);
void an_CONSTANTE(void);
int  an_DATO(int);
void an_EXPRESION(void);
void an_COMPUTO(void);
void an_VALOR(void);
void an_TERMINO(void);
void an_FACTOR(void);
void an_FUNCION_INTERNA(void);
void an_BORRAR_PANTALLA(void);
void an_ESCRIBIR(void);
void an_LEER_TECLA(void);
void an_MOVER_CURSOR(void);
void an_COLOR(void);
void an_ALEATORIO(void);
void an_ESPERAR(void);
void an_LEER_CARACTER(void);
void an_PONER_CARACTER(void);

void error(int);

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//
//      Prototipos de las funciones de an lisis sem ntico                    //
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//

char hash(void);                // Devuelve el valor Hash de un identificador
void inicializar(int,char**);   // Inicializaciขn del proceso de an lisis
void finalizar(void);           // Finalizaciขn del proceso de compilaciขn

int acceder(char*,int tipo);    // Acceso dentro del programa a un objeto

int declarar(char *,int,int);   // Declaraciขn de un objeto
int usar(char *,int tipo);      // Se usa un objeto

void listado_objetos(void);     // Listado de los objetos compilados

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//
//      Prototipos de las funciones de generaciขn de cขdigo                  //
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//

void inicializar_generador(void); 	// Inicializa el generador de cขdigo
void finalizar_generador(void);		// Finaliza el generador de cขdigo
void generar(int,int parametro=0); 	// Genera una instrucciขn
void separar_eml(void);			// Separaciขn dentro del listado EML
void generar_ejecutable(void);		// Graba el cขdigo m quina a un fich.
void generar_acceso(int,int);           // Genera instr. de acceso a un dato

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//
//      Variables globales del compilador                                    //
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//

int vhash[indice_hash];         // Vector hash

char vnom[longitud_vnom];       // Vector de nombres
int ivnom;                      // Indice del vector de nombres

enum { _nul,_dat,_tab,_eti };	// Diferentes tipos de objetos

struct tobjeto {                // Registro de la tabla de objetos

  int tipo;                     // Tipo de objeto
  int nombre;                   // Indice del vector de nombres
  int enlace;                   // Enlace hash con otros objetos
  char declarado;               // Indica si este objeto ha sido declarado
  char usado;                   // Indica si el objeto ha sido accedido
  int linea;                    // Lกnea del programa fuente en la que apareciข
  int direccion;		// Direcciขn del objeto
  int longitud;			// Nง de posiciones de las tablas

};

struct tobjeto tobj[max_num_objetos];   // Tabla de objetos

int itobj;                              // Indice de la tabla de objetos

int linea_error=0;              // Si no se especifica ser  la linea actual

int status_error=0;             // Indica si se ha producido un error

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//
//      Variables relacionadas con la generaciขn de cขdigo                   //
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//

// Vector de memoria destino

byte * mem;             // Se inicializar  como un vector de 64k posiciones

// define para acceder a un word dentro del vector mem[]

#define memw(x) *(word*)(mem+x)

int ip;                 // Puntero dentro de mem[], direcciขn actual

// Cขdigos asociados a cada instrucciขn EML

enum { _loc, _loe, _lof, _ste, _stf,            // Acceso a datos
       _mul, _div, _mod, _sum, _res,            // Aritmtica binaria
       _neg, _not,                              // Operadores unarios
       _and, _or,                               // Operadores lขgicos
       _dis, _igu, _may, _mai, _men, _mei,      // Operadores de comparaciขn
       _ir, _irf, _lla, _ret,                   // Control de flujo
       _asp };                                  // Funciones internas

// Instrucciones del cขdigo EML

struct t_eml {

  char * nombre;	// Mnemขnico del la instrucciขn
  int long_codigo;	// Nฃmero de bytes que tiene la cadena de cขdigo
  int parametro;	// Posiciขn del par metro si lo tiene, 0 si no
  byte * codigo;	// Cadena de cขdigo x86 equivalente a la instrucciขn

} eml[_asp+1] = {

  "LOC", 4,1,"\xb8\x00\x00\x50",
  "LOE", 4,2,"\xff\x36\x00\x00",
  "LOF", 7,5,"\x5b\xd1\xe3\xff\xb7\x00\x00",
  "STE", 4,2,"\x8f\x06\x00\x00",
  "STF", 8,6,"\x58\x5b\xd1\xe3\x89\x87\x00\x00",
  "MUL", 5,0,"\x5b\x58\xf7\xeb\x50",
  "DIV", 7,0,"\x31\xd2\x5b\x58\xf7\xfb\x50",
  "MOD", 7,0,"\x31\xd2\x5b\x58\xf7\xfb\x52",
  "SUM", 5,0,"\x58\x89\xe3\x01\x07",
  "RES", 5,0,"\x58\x89\xe3\x29\x07",
  "NEG", 4,0,"\x89\xe3\xf7\x1F",
  "NOT", 4,0,"\x89\xe3\xf7\x17",
  "AND", 5,0,"\x58\x89\xe3\x21\x07",
  "OR",  5,0,"\x58\x89\xe3\x09\x07",
  "DIS",10,0,"\x31\xd2\x5b\x58\x39\xd8\x74\x01\x42\x52",
  "IGU",10,0,"\x31\xd2\x5b\x58\x39\xd8\x75\x01\x42\x52",
  "MAY",10,0,"\x31\xd2\x5b\x58\x39\xd8\x7E\x01\x42\x52",
  "MAI",10,0,"\x31\xd2\x5b\x58\x39\xd8\x7C\x01\x42\x52",
  "MEN",10,0,"\x31\xd2\x5b\x58\x39\xd8\x7D\x01\x42\x52",
  "MEI",10,0,"\x31\xd2\x5b\x58\x39\xd8\x7F\x01\x42\x52",
  "IR",  5,1,"\xbb\x00\x00\xff\xe3",
  "IRF",10,6,"\x58\xa8\x01\x75\x05\xbb\x00\x00\xff\xe3",
  "LLA", 5,1,"\xbb\x00\x00\xff\xd3",
  "RET", 1,0,"\xc3",
  "ASP", 4,2,"\x81\xc4\x00\x00"

};

// Fichero de salida (para emitir el listado con el cขdigo EML)

FILE * f_eml;

// Fichero de ordenes para que DEBUG desensamble el cขdigo m quina.

FILE * f_debug;

// Acceso a la direcciขn de la tabla de funciones

word far * tabla_funciones = (word far *) MK_FP(0,0x45E);

word * funciones;

// Acceso a la direcciขn de carga del programa

word far * direccion_carga = (word far *) MK_FP(0,0x45C);

// Nombres de ficheros

char fichero_lt[128];
char fichero_com[128];
char fichero_lst[128];
char fichero_eml[128];
char fichero_obj[128];

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//
//      Programa principal                                                   //
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//

void main (int argc, char ** argv) {

  int i;

  inicializar(argc,argv);
  an_PROGRAMA(); // Llamamos al procedimiento para analizar un programa

  i=itobj; // Comprobamos si se ha quedado algฃn objeto por declarar
  while (--i>=0) if (!tobj[i].declarado) {
    linea_error=tobj[i].linea;
    error(2005);
  }

  finalizar();

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//
//      Inicializaciขn                                                       //
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//

void inicializar(int argc, char ** argv) {

  int n;
  char *p;

  printf("Compilador de Letra 1.0 (c) 1996 Daniel Navarro Medrano - Freeware\n\n");

  if (argc<2) {
    printf("Error: Indicar el nombre del fichero (*.LT) como par metro.");
    exit(0);
  }

  p=fichero_com;
  strcpy(p,argv[1]);
  while (strchr(p,'\\')!=NULL) p=strchr(p,'\\')+1;
  if (strchr(p,'.')==NULL) strcat(p,".lt");

  strcpy(fichero_lt,fichero_com);
  *(strchr(p,'.'))='\0';
  strcpy(fichero_lst,fichero_com);
  strcpy(fichero_eml,fichero_com);
  strcpy(fichero_obj,fichero_com);

  strcat(fichero_com,".com"); // Ejecutable
  strcat(fichero_lst,".lst"); // Desensamble
  strcat(fichero_eml,".eml"); // EML
  strcat(fichero_obj,".obj"); // Objetos

  inicializar_generador();

  for (n=0;n<indice_hash;n++) vhash[n]=-1; // Inicializa enlaces hash

  ivnom=0; // Inicializa el indice global del vector de nombres

  itobj=0; // Inicializa el indice global de la tabla de objetos

  inicializacion_lexica(fichero_lt); // Inicializamos el analizador lxico

  obtener_pieza(); // Obtenemos en pieza_actual la primera pieza
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//
//      Finalizaciขn                                                         //
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//

void finalizar(void) {

  if (!status_error)
    printf("El programa %s cumple todas las reglas del lenguaje.\n",
           fichero_lt);

  finalizacion_lexica();

  finalizar_generador();

  if (!status_error) listado_objetos();

  generar_ejecutable();

  exit(0);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//
//      Funcion hash                                                         //
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//

char hash (char * p) {

  char n=0;

  while (*p) {
    n<<=1;
    n^=*p++;
  }

  return(n);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//
//      Declaraciขn de un nuevo objeto (puede que haya sido usado antes)     //
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//

int declarar(char * nombre, int tipo, int ip) {

  int iobj=acceder(nombre,tipo);
  int n,m;

  if (tobj[iobj].declarado) error(2000);

  tobj[iobj].declarado=TRUE;

  if (tobj[iobj].usado) {

    // Deshacer la cadena

    n=tobj[iobj].direccion;

    while (n) {
      m=memw(n);
      memw(n)=ip;
      n=m;
    }

    tobj[iobj].direccion=ip;

  } else tobj[iobj].direccion=ip;

  return(iobj);

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//
//      Se usa un objeto (que podia estar o no definido anteriormente)       //
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//

int usar(char * nombre, int tipo) {

  int iobj=acceder(nombre,tipo);

  tobj[iobj].usado=TRUE;

  return(iobj);

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//
//      Acceso a un objeto                                                   //
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//

int acceder(char * nombre, int tipo) {

  unsigned char ivhash; // Indice del vector hash
  char encontrado;      // Indica si ya existกa ese objeto
  int iobj;             // En caso afirmativo, iobj indica su posiciขn

  ivhash=hash(nombre);

  encontrado=FALSE;

  if (vhash[ivhash]>=0) {

    iobj=vhash[ivhash];

    do {

      if (!strcmp(nombre,&vnom[tobj[iobj].nombre])) {
	encontrado=TRUE;
	break;
      }

      iobj=tobj[iobj].enlace;

    } while (iobj>=0);

  }

  if (!encontrado) {

    if (itobj==max_num_objetos) error(2002);

    iobj=itobj++;
    tobj[iobj].tipo=tipo;
    tobj[iobj].direccion=0;

    if (ivnom+strlen(nombre)+1>longitud_vnom) error(2003);

    strcpy(&vnom[ivnom],nombre);
    tobj[iobj].nombre=ivnom;
    ivnom+=strlen(nombre)+1;

    tobj[iobj].enlace=vhash[ivhash];
    vhash[ivhash]=iobj;

    tobj[iobj].declarado=FALSE;
    tobj[iobj].usado=FALSE;
    tobj[iobj].linea=linea;
    tobj[iobj].longitud=0;

  } else {

    if (tobj[iobj].tipo!=tipo) error(2001);

  }

  return(iobj);

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//
//      Procedimientos de an lisis sint ctico de Letra                       //
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//

/******************************************************************************
<PROGRAMA> ::= { <SENTENCIA> } p_eof
******************************************************************************/

void an_PROGRAMA (void) {

  while (pieza_actual!=p_eof) {
    an_SENTENCIA();
  }
}

/******************************************************************************
<SENTENCIA> ::=
  <ETIQUETA> | <DECLARACION_DATOS> | <ASIGNACION> | <SENT_SI> |
  <SENT_MIENTRAS> | <SENT_REPETIR> | <SENT_IR> | <SENT_HACER> |
  <SENT_VOLVER> | <FUNCION_INTERNA>
******************************************************************************/

void an_SENTENCIA (void) {

  switch(pieza_actual) {
    case p_etiqueta: an_ETIQUETA(); break;
    case p_datos: an_DECLARACION_DATOS(); break;
    case p_identificador: an_ASIGNACION(); break;
    case p_si: an_SENT_SI(); break;
    case p_mientras: an_SENT_MIENTRAS(); break;
    case p_repetir: an_SENT_REPETIR(); break;
    case p_ir: an_SENT_IR(); break;
    case p_hacer: an_SENT_HACER(); break;
    case p_volver: an_SENT_VOLVER(); break;
    default: an_FUNCION_INTERNA(); break;
  }
  separar_eml();
}

/******************************************************************************
<ETIQUETA> ::= p_etiqueta
******************************************************************************/

void an_ETIQUETA(void) {

  declarar(nombre,_eti,ip);
  fprintf(f_eml,"%u\t%s\n",ip,nombre);
  obtener_pieza();
}

/******************************************************************************
<DECLARACION_DATOS> ::=
  p_datos { p_identificador [ <INICIALIZACION_DATO> |
  <INICIALIZACION_TABLA> ] } p_fin
******************************************************************************/

void an_DECLARACION_DATOS(void) {

  char ident[long_linea];
  int ip_salto=ip,ip_final;

  ip+=eml[_ir].long_codigo; // Reservamos espacio para una sentencia _ir

  obtener_pieza();
  while (pieza_actual==p_identificador) {
    strcpy(ident,nombre);
    obtener_pieza();
    if (pieza_actual==p_abrir_corchete) {

      fprintf(f_eml,"%u\t\%s[]\n",ip,ident);

      an_INICIALIZACION_TABLA(declarar(ident,_tab,ip));

    } else {

      fprintf(f_eml,"%u\t\%s\n",ip,ident);

      declarar(ident,_dat,ip);
      if (pieza_actual==p_igual) an_INICIALIZACION_DATO();
      else { memw(ip)=0; ip+=2; }
    }
  }
  if (pieza_actual!=p_fin) error(p_fin);

  ip_final=ip; // Puntero a mem[] tras la declaraciขn de datos
  ip=ip_salto; // Nos vamos al comienzo de la zona de datos para ...
  generar(_ir,ip_final); // ... generar el salto al final de la zona de datos

  // Da instrucciones a DEBUG para que no desensamble la zona de datos
  fprintf(f_debug,"%x\n D %x %x\n U %x ",ip-1,ip,ip_final-1,ip_final);

  ip=ip_final; // Recuperamos el puntero actualizado
  obtener_pieza();
}

/******************************************************************************
<ASIGNACION> ::= <DATO> p_igual <EXPRESION>
******************************************************************************/

void an_ASIGNACION(void) {

  int obj;

  obj=an_DATO(0);
  if (pieza_actual!=p_igual) error(p_igual);
  obtener_pieza();
  an_EXPRESION();

  if (tobj[obj].tipo==_dat) generar_acceso(_ste,obj);
  else generar_acceso(_stf,obj);
}

/******************************************************************************
<SENT_SI> ::= p_si <EXPRESION> { <SENTENCIA> } [ p_sino { <SENTENCIA> } ] p_fin
******************************************************************************/

void an_SENT_SI(void) {

  int ip_salto_sino,ip_salto_fin,ip_final;

  obtener_pieza();
  an_EXPRESION();

  ip_salto_sino=ip; // Reserva espacio para saltar al sino/fin cuando ...
  ip+=eml[_irf].long_codigo; // ... la expresiขn no sea cierta

  while (pieza_actual!=p_sino && pieza_actual!=p_fin) {
    an_SENTENCIA();
  }

  if (pieza_actual==p_sino) { // Sentencia tipo si ... sino ... fin

    ip_salto_fin=ip; // Tras ejecutarse el si ... se debe saltar a fin para ...
    ip+=eml[_ir].long_codigo; // ... que no se ejecute la parte sino ...

    ip_final=ip; // Genera el salto a la parte sino ... cuando la primera ...
    ip=ip_salto_sino; // ... condiciขn es falsa
    generar(_irf,ip_final);
    ip=ip_final;

    obtener_pieza();
    while (pieza_actual!=p_fin) {
      an_SENTENCIA();
    }

    ip_final=ip; // Genera el salto al final tras ejecutarse una parte si ...
    ip=ip_salto_fin;
    generar(_ir,ip_final);
    ip=ip_final;

  } else { // Sentencia tipo si ... fin

    ip_final=ip; // Si la primera condiciขn es falsa ...
    ip=ip_salto_sino; // ... se salta directamente al final de la sentencia
    generar(_irf,ip_final);
    ip=ip_final;

  }

  obtener_pieza();
}

/******************************************************************************
<SENT_MIENTRAS> ::= p_mientras <EXPRESION> { <SENTENCIA> } p_fin
******************************************************************************/

void an_SENT_MIENTRAS(void) {

  int ip_inicio=ip,ip_salto,ip_final;

  obtener_pieza();
  an_EXPRESION();

  ip_salto=ip; // Reserva espacio para realizar un salto al final del bucle ...
  ip+=eml[_irf].long_codigo; // ... cuando la expresiขn sea falsa

  while (pieza_actual!=p_fin) {
    an_SENTENCIA();
  }

  generar(_ir,ip_inicio); // Repite el bucle

  ip_final=ip; // Guardamos la direcciขn final de mem[] tras la sentencia
  ip=ip_salto; // Generamos el salto de ruptura del bucle ahora que sabemos ...
  generar(_irf,ip_final); // ... donde tiene que saltar
  ip=ip_final; // Recuperamos ip

  obtener_pieza();
}

/******************************************************************************
<SENT_REPETIR> ::= p_repetir { <SENTENCIA> } p_hasta <EXPRESION>
******************************************************************************/

void an_SENT_REPETIR(void) {

  int ip_inicio=ip;

  obtener_pieza();
  while (pieza_actual!=p_hasta) {
    an_SENTENCIA();
  }
  obtener_pieza();
  an_EXPRESION(); // Evalฃa la condiciขn y ...

  generar(_irf,ip_inicio); // ... si es falsa repite el bucle
}

/******************************************************************************
<SENT_IR> ::= p_ir p_etiqueta
******************************************************************************/

void an_SENT_IR(void) {

  obtener_pieza();
  if (pieza_actual!=p_etiqueta) error(p_etiqueta);

  generar_acceso(_ir,usar(nombre,_eti));

  obtener_pieza();
}

/******************************************************************************
<SENT_HACER> ::= p_hacer p_etiqueta
******************************************************************************/

void an_SENT_HACER(void) {

  obtener_pieza();
  if (pieza_actual!=p_etiqueta) error(p_etiqueta);

  generar_acceso(_lla,usar(nombre,_eti));

  obtener_pieza();
}

/******************************************************************************
<SENT_VOLVER> ::= p_volver
******************************************************************************/

void an_SENT_VOLVER(void) {

  generar(_ret);

  obtener_pieza();
}

/******************************************************************************
<INICIALIZACION_DATO> ::= p_igual <CONSTANTE>
******************************************************************************/

void an_INICIALIZACION_DATO(void) {
  int viejo_ip=ip;

  obtener_pieza();
  an_CONSTANTE();

  if (ip!=viejo_ip+2) error(2006);
}

/******************************************************************************
<INICIALIZACION_TABLA> ::=
  p_abrir_corchete
    (p_entero p_cerrar_corchete
    [ p_igual <CONSTANTE> { p_dos_puntos <CONSTANTE> } ] |
    p_cerrar_corchete p_igual <CONSTANTE> { p_dos_puntos <CONSTANTE> } )
******************************************************************************/

void an_INICIALIZACION_TABLA (int iobj) {

  int n=0;

  obtener_pieza();
  switch(pieza_actual) {

    case p_entero:
      tobj[iobj].longitud=atol(nombre);
      obtener_pieza();
      if (pieza_actual!=p_cerrar_corchete) error(p_cerrar_corchete);
      obtener_pieza();
      if (pieza_actual==p_igual) {
      	obtener_pieza();
	an_CONSTANTE();
	while (pieza_actual==p_dos_puntos) {
      	  obtener_pieza();
	  an_CONSTANTE();
	}
        n=(ip-tobj[iobj].direccion)/2; // Nง de objetos declarados
	if (n-1>tobj[iobj].longitud) error(2004);
      }
      for (;n<=tobj[iobj].longitud;n++) { memw(ip)=0; ip+=2; }
      break;

    case p_cerrar_corchete:
      obtener_pieza();
      if (pieza_actual!=p_igual) error(p_igual);
      obtener_pieza();
      an_CONSTANTE();
      while (pieza_actual==p_dos_puntos) {
	obtener_pieza();
	an_CONSTANTE();
      }
      n=(ip-tobj[iobj].direccion)/2; // Nง de objetos declarados
      tobj[iobj].longitud=n-1;
      break;

    default: error(p_entero);
  }
}

/******************************************************************************
<CONSTANTE> ::= p_cierto | p_falso | [ p_mas | p_menos ] p_entero | p_literal
******************************************************************************/

void an_CONSTANTE (void) { // Genera en mem[ip] la constante analizada

int n;

  switch(pieza_actual) {

    case p_cierto:
      memw(ip)=1; ip+=2;
      obtener_pieza();
      break;

    case p_falso:
      memw(ip)=0; ip+=2;
      obtener_pieza();
      break;

    case p_menos:
      obtener_pieza();
      if (pieza_actual!=p_entero) error(p_entero);
      memw(ip)=-atol(nombre); ip+=2;
      obtener_pieza();
      break;

    case p_mas:
      obtener_pieza();
      if (pieza_actual!=p_entero) error(p_entero);
    case p_entero:
      memw(ip)=atol(nombre); ip+=2;
      obtener_pieza();
      break;

    case p_literal:
      n=0; while (nombre[n]) { memw(ip)=nombre[n++]; ip+=2; }
      obtener_pieza();
      break;

    default: error(1000);
  }
}

/******************************************************************************
<DATO> ::= p_identificador [ p_abrir_corchete <EXPRESION> p_cerrar_corchete ]
******************************************************************************/

int an_DATO(int generar_codigo) {

  char ident[long_linea];
  int obj;

  strcpy(ident,nombre);

  obtener_pieza();
  if (pieza_actual==p_abrir_corchete) {
    obj=usar(ident,_tab);
    obtener_pieza();
    an_EXPRESION();
    if (pieza_actual!=p_cerrar_corchete) error(p_cerrar_corchete);
    obtener_pieza();
    if (generar_codigo) generar_acceso(_lof,obj);
  } else {
    obj=usar(ident,_dat);
    if (generar_codigo) generar_acceso(_loe,obj);
  }

  return(obj); // Devuelve el nฃmero de objeto (de la tabla de objetos)
}

/******************************************************************************
<EXPRESION> ::= <COMPUTO> { <OP_LOGICO> <COMPUTO> }
<OP_LOGICO> ::= p_y | p_o
******************************************************************************/

void an_EXPRESION (void) {

  int operador;

  an_COMPUTO();
  while (pieza_actual==p_y || pieza_actual==p_o) {
    operador=pieza_actual-p_y+_and;
    obtener_pieza();
    an_COMPUTO();
    generar(operador);
  }
}

/******************************************************************************
<COMPUTO> ::= <VALOR> { <OP_COMPARACION> <VALOR> }
<OP_COMPARACION> ::=
  p_mayor | p_mayor_igual | p_menor | p_menor_igual | p_igual | p_distinto
******************************************************************************/

void an_COMPUTO (void) {

  int operador;

  an_VALOR();
  while (pieza_actual==p_distinto || pieza_actual==p_igual ||
	 pieza_actual==p_mayor || pieza_actual==p_mayor_igual ||
	 pieza_actual==p_menor || pieza_actual==p_menor_igual) {
    operador=pieza_actual-p_distinto+_dis;
    obtener_pieza();
    an_VALOR();
    generar(operador);
  }
}

/******************************************************************************
<VALOR> ::= <TERMINO> { <OP_ADITIVO> <TERMINO> }
<OP_ADITIVO> ::= p_mas | p_menos
******************************************************************************/

void an_VALOR (void) {

  int operador;

  an_TERMINO();
  while (pieza_actual==p_mas || pieza_actual==p_menos) {
    operador=pieza_actual-p_mas+_sum;
    obtener_pieza();
    an_TERMINO();
    generar(operador);
  }
}

/******************************************************************************
<TERMINO> ::= <FACTOR> { <OP_MULTIPLICATIVO> <FACTOR> }
<OP_MULTIPLICATIVO> ::= p_multiplicar | p_dividir | p_modulo
******************************************************************************/

void an_TERMINO (void) {

  int operador;

  an_FACTOR();
  while (pieza_actual==p_multiplicar || pieza_actual==p_dividir ||
	 pieza_actual==p_modulo) {
    operador=pieza_actual-p_multiplicar+_mul;
    obtener_pieza();
    an_FACTOR();
    generar(operador);
  }
}

/******************************************************************************
<FACTOR> ::=
  <OP_UNARIO> <FACTOR> | p_abrir_parentesis <EXPRESION> p_cerrar_parentesis |
  <DATO> | p_cierto | p_falso | p_entero | p_literal
<OP_UNARIO> ::= p_mas | p_menos | p_no
******************************************************************************/

void an_FACTOR(void) {

  switch(pieza_actual){

    case p_mas:
      obtener_pieza();
      an_FACTOR();
      break;

    case p_menos:
      obtener_pieza();
      an_FACTOR();
      generar(_neg);
      break;

    case p_no:
      obtener_pieza();
      an_FACTOR();
      generar(_not);
      break;

    case p_abrir_parentesis:
      obtener_pieza();
      an_EXPRESION();
      if (pieza_actual!=p_cerrar_parentesis) error(p_cerrar_parentesis);
      obtener_pieza();
      break;

    case p_identificador:
      an_DATO(1);
      break;

    case p_cierto:
      generar(_loc,1);
      obtener_pieza();
      break;

    case p_falso:
      generar(_loc,0);
      obtener_pieza();
      break;

    case p_entero:
      generar(_loc,atol(nombre));
      obtener_pieza();
      break;

    case p_literal:
      if (strlen(nombre)!=1) error(2006);
      generar(_loc,*nombre);
      obtener_pieza();
      break;

    default: error(1001);
  }
}

/******************************************************************************
<FUNCION_INTERNA> ::=
  <BORRAR_PANTALLA> | <ESCRIBIR> | <LEER_TECLA> | <MOVER_CURSOR> |
  <COLOR> | <ALEATORIO> | <ESPERAR> | <LEER_CARACTER> | <PONER_CARACTER>
******************************************************************************/

void an_FUNCION_INTERNA(void) {

  switch(pieza_actual) {
    case p_borrar_pantalla: an_BORRAR_PANTALLA(); break;
    case p_escribir: an_ESCRIBIR(); break;
    case p_leer_tecla: an_LEER_TECLA(); break;
    case p_mover_cursor: an_MOVER_CURSOR(); break;
    case p_color: an_COLOR(); break;
    case p_aleatorio: an_ALEATORIO(); break;
    case p_esperar: an_ESPERAR(); break;
    case p_leer_caracter: an_LEER_CARACTER(); break;
    case p_poner_caracter: an_PONER_CARACTER(); break;
    default: error(1002); break;
  }
}

/******************************************************************************
<BORRAR_PANTALLA> ::= p_borrar_pantalla p_abrir_parentesis p_cerrar_parentesis
******************************************************************************/

void an_BORRAR_PANTALLA(void) {

  obtener_pieza();
  if (pieza_actual!=p_abrir_parentesis) error(p_abrir_parentesis);
  obtener_pieza();

  generar(_lla,funciones[1]); // Llama a la funciขn interna nง 1

  if (pieza_actual!=p_cerrar_parentesis) error(p_cerrar_parentesis);
  obtener_pieza();
}

/******************************************************************************
<ESCRIBIR> ::= p_escribir p_abrir_parentesis p_literal p_cerrar_parentesis
******************************************************************************/

void an_ESCRIBIR(void) {

  int inicio_cadena;

  obtener_pieza();
  if (pieza_actual!=p_abrir_parentesis) error(p_abrir_parentesis);
  obtener_pieza();
  if (pieza_actual!=p_literal) error(p_literal);
  obtener_pieza();

  inicio_cadena=ip+eml[_ir].long_codigo;
  generar(_ir,inicio_cadena+strlen(nombre)+1);
  strcpy(mem+ip,nombre);
  ip+=strlen(nombre)+1;

  // Da instrucciones a DEBUG para que no desensamble la cadena
  fprintf(f_debug,"%x\n D %x %x\n U %x ",inicio_cadena-1,inicio_cadena,ip-1,ip);

  generar(_loc,inicio_cadena);
  generar(_lla,funciones[2]); // Llama a la funciขn interna nง 2 ...
  generar(_asp,2);            // ... y despus desapila el par metro

  if (pieza_actual!=p_cerrar_parentesis) error(p_cerrar_parentesis);
  obtener_pieza();
}

/******************************************************************************
<LEER_TECLA> ::= p_leer_tecla p_abrir_parentesis <DATO> p_cerrar_parentesis
******************************************************************************/

void an_LEER_TECLA(void) {

  int obj;

  obtener_pieza();
  if (pieza_actual!=p_abrir_parentesis) error(p_abrir_parentesis);
  obtener_pieza();

  obj=an_DATO(0);
  if (tobj[obj].tipo==_dat) generar_acceso(_loc,obj);
  else {
    generar(_loc,2);
    generar(_mul);
    generar_acceso(_loc,obj);
    generar(_sum);
  }
  generar(_lla,funciones[3]); // Llama a la funciขn interna nง 3 ...
  generar(_asp,2);	      // ... y despus desapila el par metro

  if (pieza_actual!=p_cerrar_parentesis) error(p_cerrar_parentesis);
  obtener_pieza();
}

/******************************************************************************
<MOVER_CURSOR> ::=
  p_mover_cursor p_abrir_parentesis <EXPRESION> p_coma
  <EXPRESION> p_cerrar_parentesis
******************************************************************************/

void an_MOVER_CURSOR(void) {

  obtener_pieza();
  if (pieza_actual!=p_abrir_parentesis) error(p_abrir_parentesis);
  obtener_pieza();
  an_EXPRESION();
  if (pieza_actual!=p_coma) error(p_coma);
  obtener_pieza();
  an_EXPRESION();

  generar(_lla,funciones[4]); // Llama a la funciขn interna nง 4 ...
  generar(_asp,4);            // ... y despus desapila el par metro

  if (pieza_actual!=p_cerrar_parentesis) error(p_cerrar_parentesis);
  obtener_pieza();
}

/******************************************************************************
<COLOR> ::= p_color p_abrir_parentesis <EXPRESION> p_cerrar_parentesis
******************************************************************************/

void an_COLOR(void) {

  obtener_pieza();
  if (pieza_actual!=p_abrir_parentesis) error(p_abrir_parentesis);
  obtener_pieza();
  an_EXPRESION();

  generar(_lla,funciones[5]); // Llama a la funciขn interna nง 5 ...
  generar(_asp,2);            // ... y despus desapila el par metro

  if (pieza_actual!=p_cerrar_parentesis) error(p_cerrar_parentesis);
  obtener_pieza();
}

/******************************************************************************
<ALEATORIO> ::= p_aleatorio p_abrir_parentesis <DATO> p_cerrar_parentesis
******************************************************************************/

void an_ALEATORIO(void) {

  int obj;

  obtener_pieza();
  if (pieza_actual!=p_abrir_parentesis) error(p_abrir_parentesis);
  obtener_pieza();

  obj=an_DATO(0);
  if (tobj[obj].tipo==_dat) generar_acceso(_loc,obj);
  else {
    generar(_loc,2);
    generar(_mul);
    generar_acceso(_loc,obj);
    generar(_sum);
  }
  generar(_lla,funciones[6]); // Llama a la funciขn interna nง 6 ...
  generar(_asp,2);	      // ... y despus desapila el par metro

  if (pieza_actual!=p_cerrar_parentesis) error(p_cerrar_parentesis);
  obtener_pieza();
}

/******************************************************************************
<ESPERAR> ::= p_esperar p_abrir_parentesis <EXPRESION> p_cerrar_parentesis
******************************************************************************/

void an_ESPERAR(void) {

  obtener_pieza();
  if (pieza_actual!=p_abrir_parentesis) error(p_abrir_parentesis);
  obtener_pieza();
  an_EXPRESION();

  generar(_lla,funciones[7]); // Llama a la funciขn interna nง 7 ...
  generar(_asp,2);            // ... y despus desapila el par metro

  if (pieza_actual!=p_cerrar_parentesis) error(p_cerrar_parentesis);
  obtener_pieza();
}

/******************************************************************************
<LEER_CARACTER> ::=
  p_leer_car cter p_abrir_parentesis <DATO> p_cerrar_parentesis
******************************************************************************/

void an_LEER_CARACTER(void) {

  int obj;

  obtener_pieza();
  if (pieza_actual!=p_abrir_parentesis) error(p_abrir_parentesis);
  obtener_pieza();

  obj=an_DATO(0);
  if (tobj[obj].tipo==_dat) generar_acceso(_loc,obj);
  else {
    generar(_loc,2);
    generar(_mul);
    generar_acceso(_loc,obj);
    generar(_sum);
  }
  generar(_lla,funciones[8]); // Llama a la funciขn interna nง 8 ...
  generar(_asp,2);		    // ... y despus desapila el par metro

  if (pieza_actual!=p_cerrar_parentesis) error(p_cerrar_parentesis);
  obtener_pieza();
}

/******************************************************************************
<PONER_CARACTER> ::=
  p_poner_car cter p_abrir_parentesis <EXPRESION> p_cerrar_parentesis
******************************************************************************/

void an_PONER_CARACTER(void) {

  obtener_pieza();
  if (pieza_actual!=p_abrir_parentesis) error(p_abrir_parentesis);
  obtener_pieza();
  an_EXPRESION();

  generar(_lla,funciones[9]); // Llama a la funciขn interna nง 9 ...
  generar(_asp,2);                   // ... y despus desapila el par metro

  if (pieza_actual!=p_cerrar_parentesis) error(p_cerrar_parentesis);
  obtener_pieza();
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//
//      Funciขn de error                                                     //
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//

void error(int e) {

  int l;

  if (linea_error) l=linea_error; else l=linea;

  if (pieza_actual==p_error) {                  // *** Errores lxicos ***

    printf("Error lxico en lกnea %u, ",l);
    switch(error_lexico) {
      case 0: printf("car cter no esperado."); break;
      case 1: printf("nฃmero fuera de rango."); break;
      case 2: printf("literal sin cerrar."); break;
    }

  } else if (e<2000) {                          // *** Errores sint cticos ***

    printf("Error sint ctico en lกnea %u, ",l);
    switch(e) {
      case 1000:
	printf("esperando una constante en lugar de %s.",piezas[pieza_actual]);
	break;
      case 1001:
	printf("esperando un factor en lugar de %s.",piezas[pieza_actual]);
	break;
      case 1002:
	printf("esperando una sentencia en lugar de %s.",piezas[pieza_actual]);
	break;
      default:
	printf("se esperaba %s en lugar de %s.",piezas[e],piezas[pieza_actual]);
	break;
    }

  } else {                                    // *** Errores sem nticos ***

    printf("Error sem ntico en lกnea %u, ",l);
    switch(e) {
      case 2000: printf("objeto declarado m s de una vez."); break;
      case 2001: printf("objeto usado con diferentes tipos."); break;
      case 2002: printf("demasiados objetos definidos."); break;
      case 2003: printf("desbordado el vector de nombres."); break;
      case 2004: printf("demasiados valores para la tabla.");break;
      case 2005: printf("objeto no declarado."); break;
      case 2006: printf("se esperaba un solo car cter."); break;
    }
  }

  status_error=1; // No se grabar  el cขdigo a disco si hay un error

  finalizar();

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//
//      Listado de la tabla de objetos                                       //
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//

void listado_objetos(void) {

  int n;
  FILE * f;

  int dat=0,tab=0,eti=0;

  if ((f=fopen(fichero_obj,"wt"))==NULL) {
    printf("\nError: No se pudo crear %s",fichero_obj);
    exit(0);
  }

  fprintf(f,"Resumen de los objetos analizados en el programa\n\n");

  fprintf(f,"Ocupaciขn de la tabla de objetos: %u de %u\n",itobj,max_num_objetos);
  fprintf(f,"Ocupaciขn del vector de nombres:  %u de %u\n\n",ivnom,longitud_vnom);

  fprintf(f,"Listado de los objetos\n\n");

  n=0; while(n<itobj) {

    fprintf(f,"<%u>\t",n);

    switch(tobj[n].tipo) {
      case _nul: fprintf(f,"Sin tipo"); break;
      case _dat: fprintf(f,"Dato"); dat++; break;
      case _tab: fprintf(f,"Tabla de datos"); tab++; break;
      case _eti: fprintf(f,"Etiqueta"); eti++; break;
    }

    fprintf(f," \"%s\"\n",&vnom[tobj[n].nombre]);

    fprintf(f,"\tHash %u, ",(unsigned char)hash(&vnom[tobj[n].nombre]));

    if (tobj[n].enlace>=0) fprintf(f,"enlace con <%u>\n",tobj[n].enlace);
    else fprintf(f,"sin enlace\n");

    fprintf(f,"\tDeclarado=%u, Usado=%u\n",tobj[n].declarado,tobj[n].usado);

    fprintf(f,"\tDirecciขn=%u\n",tobj[n].direccion);

    if (tobj[n].tipo==_tab) fprintf(f,"\tRango 0..%u\n",tobj[n].longitud);

    fprintf(f,"\tLกnea %u\n",tobj[n].linea);

    fprintf(f,"\n");

    n++;
  }

  fprintf(f,"Nฃmero de Datos     = %u\n",dat);
  fprintf(f,"Nฃmero de Tablas    = %u\n",tab);
  fprintf(f,"Nฃmero de Etiquetas = %u\n",eti);

  printf("\nInfo: Listado de objetos generado en el fichero %s",fichero_obj);

  fclose(f);                   // Finalmente cerramos el fichero destino
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//
//      Inicializa el proceso de generaciขn de cขdigo                        //
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//

void inicializar_generador(void) {

  FILE * f;
  int n;

  if ((f_eml=fopen(fichero_eml,"wt"))==NULL) {
    printf("Error: No se pudo crear %s",fichero_eml);
    exit(0);
  }

  fprintf(f_eml,";Traducciขn del programa a cขdigo EML\n\n");

  if ((mem=(byte*)farmalloc(65536))==NULL) {
    printf("Error: No se pudo reservar memoria para mem[]");
    exit(0);
  }

  if ((f=fopen("startup.com","rb"))==NULL) {
    printf("Error: No se pudo cargar el fichero STARTUP.COM");
    exit(0);
  }

  fseek(f,0,SEEK_END);
  n=ftell(f);
  fseek(f,0,SEEK_SET);
  fread(mem+256,1,n,f);
  fclose(f);

  system("startup.com");
  if (!(ip=*direccion_carga)) ip=256+n;
  funciones=(word*)(mem+*tabla_funciones);
  funciones[0]=ip; // Indica al startup el inicio del programa

  if ((f_debug=fopen("debug.tmp","wt"))==NULL) {
    printf("Error: No se pudo crear el fichero debug.tmp");
    exit(0);
  }

  fprintf(f_debug," \\* DESENSAMBLE DE %s *\x1b\n U %x ",fichero_com,ip);

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//
//      Finaliza el proceso de generaciขn de cขdigo                          //
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//

void finalizar_generador(void) {

  generar(_ret);

  fclose(f_eml);

  printf("\nInfo: Traducciขn en cขdigo EML generada en el fichero %s",fichero_eml);

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//
//      Graba el vector mem[] a un fichero 				     //
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//

void generar_ejecutable (void) {

  FILE * f;
  char orden_debug[256];

  // Termina el fichero de ordenes para DEBUG

  fprintf(f_debug,"%x\n Q\xd",ip-1);
  fclose(f_debug);

  if (!status_error) {

    // Graba el cขdigo m quina a disco

    if ((f=fopen(fichero_com,"wb"))==NULL) {
      printf("\nError: No se pudo crear el fichero %s",fichero_com);
      exit(0);
    }

    fwrite(&mem[256],1,ip-256,f);

    fclose(f);

    printf("\nInfo: Cขdigo m quina generado en el fichero %s",fichero_com);

    // Genera un listado con las instrucciones en ensamblador usando DEBUG

    sprintf(orden_debug,"debug %s <debug.tmp >%s",fichero_com,fichero_lst);

    if (!system(orden_debug))
      printf("\nInfo: Desensamble generado en el fichero %s",fichero_lst);

  }

  system("del debug.tmp");

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//
//      Genera una instrucciขn EML		                             //
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//

void generar(int instruccion, int parametro) {

  // Genera el mnemขnico en el listado EML

  fprintf(f_eml,"%u\t%s",ip,eml[instruccion].nombre);

  if (eml[instruccion].parametro)
    fprintf(f_eml," %d",parametro);

  fprintf(f_eml,"\n");

  // Genera el cขdigo m quina en el vector mem[]

  memcpy(&mem[ip],eml[instruccion].codigo,eml[instruccion].long_codigo);

  if (eml[instruccion].parametro)
    memw(ip+eml[instruccion].parametro)=parametro;

  ip+=eml[instruccion].long_codigo;

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//
//      Generar una instrucciขn que accede a un dato (etiqueta/dato/tabla)   //
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//

void generar_acceso(int instruccion, int objeto) {

  int ip_inicio=ip;

  generar(instruccion,tobj[objeto].direccion);

  if (!tobj[objeto].declarado) {
    tobj[objeto].direccion=ip_inicio+eml[instruccion].parametro;
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//
//      Emite una separaciขn dentro del listado EML		             //
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ//

void separar_eml(void) {

  fprintf(f_eml,";\n");
}
