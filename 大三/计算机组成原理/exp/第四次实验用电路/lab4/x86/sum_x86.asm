; 数据段
.DATA
    msg db "Sum=$"      ; 注意 DOS 字符串必须以 '$' 结尾
    buffer db 6 dup(?)  ; 存放转换后的数字字符（最多5位+结尾符）
.CODE
    ; 输出提示字符串
    mov dx, OFFSET msg
    mov ah, 09h
    int 21h

    ; 转换 SUM 为十进制并输出
    mov ax, SUM         ; ax = 要输出的数值
    mov cx, 10          ; 除数
    mov di, OFFSET buffer + 5  ; 指向缓冲区末尾（从后往前存）
    mov byte ptr [di], '$'     ; 设置 DOS 字符串结束符
    dec di

convert_loop:
    xor dx, dx          ; 高位置0
    div cx              ; ax / 10 → 商 ax，余数 dx
    add dl, '0'         ; 余数转 ASCII
    mov [di], dl        ; 存入缓冲区（从后往前）
    dec di
    test ax, ax
    jnz convert_loop

    ; 输出转换好的字符串（此时 di 指向第一个数字字符的前一个位置，需要调整）
    inc di
    mov dx, di
    mov ah, 09h
    int 21h

    ; 程序结束（返回 DOS）
    mov ax, 4c00h
    int 21h