
//---------------------------------------------------------------------------
// 	C¢digo de inicializaci¢n/finalizaci¢n en Letra
//---------------------------------------------------------------------------

#include "stdio.h"
#include "stdlib.h"
#include "dos.h"
#include "conio.h"
#include "mem.h"

//---------------------------------------------------------------------------
//	Constantes
//---------------------------------------------------------------------------

#ifdef __cplusplus
    #define __CPPARGS ...
#else
    #define __CPPARGS
#endif

#define byte unsigned char
#define word unsigned int

//---------------------------------------------------------------------------
//      Prototipos de funciones
//---------------------------------------------------------------------------

void main(void);			// Funciones generales
void inicializar(void);
void finalizar(void);

void interrupt new_timer(__CPPARGS);    // Nuevas interrupciones
void interrupt new_keyboard(__CPPARGS);

void borrar_pantalla(void);             // Funciones especiales
void escribir(unsigned char*);
void leer_tecla(int*);
void mover_cursor(int,int);
void color(int);
void aleatorio(int*);
void esperar(int);
void leer_caracter(int*);
void poner_caracter(unsigned int);

//---------------------------------------------------------------------------
//      Funci¢n para llamar a una funci¢n
//---------------------------------------------------------------------------

void call(void*_){((void(*)())_)();}

//---------------------------------------------------------------------------
//	Punteros al c¢digo principal y las funciones especiales
//---------------------------------------------------------------------------

void * funciones[]={&main,&borrar_pantalla,&escribir,&leer_tecla,
                    &mover_cursor,&color,&aleatorio,&esperar,&leer_caracter,
                    &poner_caracter};

//---------------------------------------------------------------------------
//      Variables globales
//---------------------------------------------------------------------------

word far * tabla_funciones = (word far *) MK_FP(0,0x45E);
void * far * direccion_carga = (void * far *) MK_FP(0,0x45C);
word far * reloj = (word far *) MK_FP(0,0x46C);
word far * pantalla = (word far *) MK_FP(0xB800,0);

word contador=0;
int color_actual=7;
int posicion_cursor=0;

void interrupt ( * old_timer ) ( __CPPARGS );
void interrupt ( * old_keyboard ) ( __CPPARGS );

//---------------------------------------------------------------------------
//	C¢digo principal
//---------------------------------------------------------------------------

void main(void) {

  int n;

  // Debe comunicar la direcci¢n de la tabla funciones[] al compilador
  // y la direcci¢n de carga del programa.

  if (*funciones==&main) {
    *tabla_funciones=(int)funciones;
    *direccion_carga=malloc(1);
    free(*direccion_carga);
    return;
  }

  inicializar();           // Llama al c¢digo de inicializaci¢n
  call(funciones[0]);      // Programa en letra
  finalizar();		   // Llama al c¢digo de finalizaci¢n

}

//---------------------------------------------------------------------------
//	C¢digo de inicializaci¢n
//---------------------------------------------------------------------------

void inicializar(void) {

  union REGS regs;

  regs.x.ax=3;
  int86(0x10,&regs,&regs); // Inicializa modo texto 80x25

  regs.h.ah=2;
  regs.x.dx=0x1900;
  regs.h.bh=0;
  int86(0x10,&regs,&regs); // Oculta el cursor

  old_timer=_dos_getvect(8);
  old_keyboard=_dos_getvect(9);

  regs.x.dx=FP_OFF(new_timer); // _dos_setvect(8,new_timer);
  regs.x.ax=0x2508;
  int86(0x21,&regs,&regs);

  regs.x.dx=FP_OFF(new_keyboard); // _dos_setvect(9,new_keyboard);
  regs.x.ax=0x2509;
  int86(0x21,&regs,&regs);

  outp(0x43,0x36);
  outp(0x40,0x98);
  outp(0x40,0x2E);
}

//---------------------------------------------------------------------------
//	C¢digo de finalizaci¢n
//---------------------------------------------------------------------------

void finalizar(void) {

  union REGS regs;

  outp(0x43,0x36);
  outp(0x40,0xff);
  outp(0x40,0xff);

  _dos_setvect(8,old_timer);
  _dos_setvect(9,old_keyboard);

  regs.h.ah=2;
  regs.x.dx=0;
  regs.h.bh=0;
  int86(0x10,&regs,&regs); // Muestra de nuevo el cursor

  borrar_pantalla();
}

//---------------------------------------------------------------------------
//      Nueva interrupci¢n de reloj (incrementa contador cada cent‚sima)
//---------------------------------------------------------------------------

void interrupt new_timer(__CPPARGS) {

  contador++;
  outp(0x20,0x20); // old_timer();
}

//---------------------------------------------------------------------------
//      Nueva interrupci¢n de reloj (incrementa contador cada cent‚sima)
//---------------------------------------------------------------------------

void interrupt new_keyboard(__CPPARGS) {

  byte n;

  asm sti;
  if (inp(0x60)==1) { // Se abort¢ la ejecuci¢n del programa con ESC
    n=inp(0x61);          
    outp(0x61,n|0x80);
    outp(0x61,n&0x7F);
    asm { push cs; pop ds; cli; }
    outp(0x20,0x20);
    finalizar();
    exit(0);
  } else old_keyboard();
}

//---------------------------------------------------------------------------
//      Funci¢n especial n§ 1 "borrar_pantalla"
//---------------------------------------------------------------------------

void borrar_pantalla(void) {
  int n=0;
  do pantalla[n++]=0x720; while (n<80*25);
}

//---------------------------------------------------------------------------
//      Funci¢n especial n§ 2 "escribir"
//---------------------------------------------------------------------------

void escribir(unsigned char * p) {
  while (*p) pantalla[posicion_cursor++]=color_actual*256+(word)*p++;
}

//---------------------------------------------------------------------------
//      Funci¢n especial n§ 3 "leer_tecla"
//---------------------------------------------------------------------------

void leer_tecla(int * p) {

  union REGS regs;

  regs.x.ax=0x100;
  int86(0x16,&regs,&regs);

  if (regs.x.flags&64) { *p=0; return; }

  do {
    regs.x.ax=0;
    int86(0x16,&regs,&regs);
    *p=regs.h.al;
    regs.x.ax=0x100;
    int86(0x16,&regs,&regs);
  } while (!(regs.x.flags&64));

}

//---------------------------------------------------------------------------
//      Funci¢n especial n§ 4 "mover_cursor"
//---------------------------------------------------------------------------

void mover_cursor(int y,int x) { // Varios par metros se reciben al rev‚s
  posicion_cursor=y*80+x;
}

//---------------------------------------------------------------------------
//      Funci¢n especial n§ 5 "color"
//---------------------------------------------------------------------------

void color(int n) {
  color_actual=n;
}

//---------------------------------------------------------------------------
//      Funci¢n especial n§ 6 "aleatorio"
//---------------------------------------------------------------------------

void aleatorio(int * p) {
  *p=rand();
}

//---------------------------------------------------------------------------
//      Funci¢n especial n§ 7 "esperar"
//---------------------------------------------------------------------------

void esperar(int n) {
  n+=contador;
  do {} while(n!=contador);
}

//---------------------------------------------------------------------------
//      Funci¢n especial n§ 8 "leer_caracter"
//---------------------------------------------------------------------------

void leer_caracter(int * p) {
  *p=pantalla[posicion_cursor]&255;
}

//---------------------------------------------------------------------------
//      Funci¢n especial n§ 9 "poner_caracter"
//---------------------------------------------------------------------------

void poner_caracter(unsigned int n) {
  pantalla[posicion_cursor++]=color_actual*256+(n&255);
}
