
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;       C¢digo "Start Up" en Ensamblador - Letra
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

        assume cs:code, ds:code, es:code, ss:code
code    segment
        org 100h
prog    proc

tics    equ 11928

        jmp init

        org 104h

	dw 0,fcls,fesc,flee,fcur,fcol ;tabla de direcciones de las funciones
        dw fale,fesp,flec,fpoc

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;       C¢digo de las funciones especiales
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

fcls:   mov ax,600h ;borra la pantalla
        mov bh,byte ptr color
        xor cx,cx
        mov dx,184fh
        int 10h
        ret

fesc:   mov bp,sp ;escribe un literal en pantalla
        mov si,[bp+2]
        mov ah,byte ptr color
        mov di,destino
fescb:  lodsb
        or al,al
        jz fescr
        stosw
        jmp short fescb
fescr:  mov destino,di
        ret

flee:   mov bp,sp ;lee una tecla y vacia el buffer
        mov ah,1                
        int 16h
        mov al,0
        jz fleer
fleeb:  xor ah,ah
        int 16h
        push ax
        mov ah,1
        int 16h
        pop ax
        jnz fleeb
fleer:  xor ah,ah
        mov bp,sp
        mov bp,[bp+2]
        mov [bp],ax
        ret

fcur:   mov bp,sp ;posiciona el cursor
        mov ax,[bp+2]
        shl ax,1
        shl ax,1
        shl ax,1
        shl ax,1
        mov di,ax
        shl ax,1
        shl ax,1
        add di,ax
        add di,[bp+4]
        shl di,1
        mov destino,di
        ret

fcol:   mov bp,sp ;selecciona un color
        mov ax,[bp+2]
        mov color,ax
        ret

fale:   mov bp,sp ;aleatorio
        mov ax,random
        add ax,contador
        xor ax,conta
        inc conta
        mov bp,[bp+2]
        mov [bp],ax
        xchg al,ah
        mov random,ax
        ret

fesp:   mov bp,sp ;espera durante un lapso
        mov ax,[bp+2]
        add ax,contador
fespb:  cmp ax,contador
        jne fespb
        ret

flec:   mov bp,sp ;lee un caracter de la pantalla
        mov di,destino          
        mov al,es:[di]
        xor ah,ah
        mov bp,[bp+2]
        mov [bp],ax
        ret

fpoc:   mov bp,sp ;escribe un caracter y avanza cursor
        mov ax,[bp+2]
        mov bx,color
        mov ah,bl
        mov di,destino
        stosw
        mov destino,di
        ret

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;       Rutina de finalizacion del programa principal
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

ffin:   mov al,00110110b ;canal 0, contador 2 bytes, modo 3, binario.
        out 43h,al
        mov ax,0ffffh
        out 40h,al
        mov al,ah
        out 40h,al ;int 8 se ejecuta 18.2 veces por segundo

        call fcls

        mov ah,2
        xor dx,dx
        xor bh,bh
        int 10h

        push cs
        pop ds
        lea si,cs:data_1
        xor ax,ax
        mov es,ax
        mov di,20h
        mov cx,4
        cld
        rep movsw

        lea si,color
        call flee ;vacia el buffer de teclado al salir

        mov ax,4c00h
        int 21h

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
_rutina1: ;la rutina de reloj no hace nada, solo incrementa un contador
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

        sti
        push ax
        inc cs:contador
        cli
        mov al,20h
        out 20h,al
        pop ax
        iret

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
_rutina2: ;la rutina de teclado detiene la ejecucion si se pulsa esc
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

        sti
        push ax
        in al,60h
        cmp al,1
        je _r_esc
        pop ax
        jmp cs:data_2

_r_esc: in al,61h
        or al,80h
        jmp $+2
        out 61h,al
        and al,7fh
        jmp $+2
        out 61h,al

        push cs
        pop ds
        mov color,7

        cli
        mov al,20h
        out 20h,al
        pop ax
        jmp ffin

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;       Variables de uso gen‚rico
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

color   dw 7            ;color seleccionada

destino dw 0            ;posicion cursor

random  dw 11111        ;aleatorios
conta   dw 44444

contador dw 0           ;interrupcion de reloj

data_1  dd 0            ;int 8 al entrar
data_2  dd 0            ;int 9 al entrar

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;       Rutina de inicio del programa principal
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

init:   xor ax,ax
        mov es,ax
        cmp word ptr ds:[104h],ax
        jne init2
	mov word ptr es:[45eh],104h ;offset tabla de funciones
        mov word ptr es:[45ch],offset endofcode ;direccion carga
        mov ax,4c00h
        int 21h

init2:  pop ax          ;fija la direccion de retorno
        lea ax,ffin
        push ax

        mov ax,es:[46ch] ;modo de video
        mov cs:contador,ax
        cmp es:[463h],3d4h
        je ini0
        mov ax,0b000h
        mov es,ax
        mov ax,7
        jmp short ini1
ini0:   mov ax,0b800h
        mov es,ax
        mov ax,3
ini1:   int 10h

        mov ah,2        ;oculta el cursor
        mov dx,1900h
        xor bh,bh
        int 10h

        push es         ;guarda vectores de interrupcion 8 y 9 (data_1/2)
        push ds         ;activa las nuevas rutinas de tratamiento
        push cs
        pop es
        lea di,cs:data_1
        xor ax,ax
        mov ds,ax
        mov si,20h
        cld
        movsw
        movsw
        movsw
        movsw
        push cs
        pop ds
        lea dx,_rutina1 ;int 8
        mov ax,2508h
        int 21h
        lea dx,_rutina2 ;int 9
        mov ax,2509h
        int 21h
        pop ds
        pop es

        mov al,00110110b ;canal 0, contador 2 bytes, modo 3, binario.
        out 43h,al
        mov ax,tics
        out 40h,al
        mov al,ah
        out 40h,al ;int 8 se ejecuta ñ100 veces por segundo

endofcode label byte

prog    endp
code    ends
        end     prog

