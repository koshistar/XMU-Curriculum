.486
.model flat, stdcall
option casemap :none
include \masm32\include\windows.inc
include \masm32\include\user32.inc
include \masm32\include\kernel32.inc
includelib \masm32\lib\user32.lib
includelib \masm32\lib\kernel32.lib

.data
    array db 8,1,5,2,7,9,6,4,3,10
    count equ 10
    newline db 13,10,0
    space db ' ',0
    buffer db 5 dup(0)
.code

start:
    mov ecx,count-1
    mov esi,0
outer_loop:
    mov edi,0
inner_loop:
    mov al,[array+edi]
    mov bl,[array+edi+1]
    cmp al,bl
    jle no_swap

    mov [array+edi],bl
    mov [array+edi+1],al
no_swap:
    inc edi
    cmp edi,ecx
    jl inner_loop
    dec ecx
    cmp ecx,0
    jg outer_loop

    mov esi,0
print_loop:
    movzx eax, byte ptr [array+esi]
    mov edi,offset buffer+4
    mov byte ptr [edi],0
    mov ebx,10

convert_loop:
    dec edi
    xor edx,edx
    div ebx
    add dl,'0'
    mov [edi],dl
    test eax,eax
    jnz convert_loop

    push offset buffer
    call GetStdHandle
    push STD_OUTPUT_HANDLE
    call GetStdHandle
    push 0
    push offset bytesWritten
    push 5
    push edi
    push eax
    call WriteFile

    inc esi
    cmp esi, count
    jge no_space
    push offset space
    call GetStdHandle
    push STD_OUTPUT_HANDLE
    call GetStdHandle
    push 0
    push offset bytesWritten
    push 1
    push offset space
    push eax
    call WriteFile
no_space:
    cmp esi, count
    jl print_loop

    push offset newline
    call GetStdHandle
    push STD_OUTPUT_HANDLE
    call GetStdHandle
    push 0
    push offset bytesWritten
    push 2
    push offset newline
    push eax
    call WriteFile

    invoke ExitProcess,NULL
.data?
    bytesWritten dd ?
end start