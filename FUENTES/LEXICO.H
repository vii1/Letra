//-------------------------------------------------------------------------//
//      Analizador l‚xico de LETRA, Solo Programadores (Enero 1996)        //
//-------------------------------------------------------------------------//

#define long_linea 1024         // Longitud m xima de las l¡neas del fuente

//-------------------------------------------------------------------------//
//	Prototipos de funciones						   //
//-------------------------------------------------------------------------//

void inicializacion_lexica (char*);
void finalizacion_lexica (void);
void obtener_pieza (void);

//-------------------------------------------------------------------------//
//      Definici¢n de las piezas sint cticas del lenguaje Letra            //
//-------------------------------------------------------------------------//

enum t_piezas {	    // Diferentes piezas sint cticas del lenguaje

	// Palabras reservadas del lenguaje

	p_datos,p_cierto,p_falso,p_si,p_sino,p_fin,p_mientras,
	p_repetir,p_hasta,p_ir,p_hacer,p_volver,p_y,p_o,p_no,

	// Nombres de las funciones especiales

	p_borrar_pantalla,p_escribir,p_leer_tecla,p_mover_cursor,
	p_color,p_aleatorio,p_esperar,p_leer_caracter,p_poner_caracter,

	// S¡mbolos del lenguaje

	p_distinto,p_igual,p_mayor,p_mayor_igual,p_menor,p_menor_igual,
	p_mas,p_menos,p_multiplicar,p_dividir,p_modulo,p_abrir_corchete,
	p_cerrar_corchete,p_abrir_parentesis,p_cerrar_parentesis,
	p_dos_puntos,p_coma,

	// Piezas especiales

	p_identificador,p_etiqueta,p_entero,p_literal,p_eof,p_error};

//-------------------------------------------------------------------------//
//      Variables                                                          //
//-------------------------------------------------------------------------//

extern char * piezas[];      // Definiciones de las diferentes piezas

extern long linea;           // N£mero de l¡nea que est  siendo analizada

extern int pieza_actual;     // Ultima pieza sint ctica analizada

extern char nombre[];        // Para el paso de informaci¢n adicional

extern int error_lexico;     // Indica el tipo de error lexico producido
