.global _start
_start:
    ldr r0,=10
    mov r1,#0
outer_loop:
    add r1,r1,#1
    cmp r1,r0
    beq end_start
    mov r2,#0
    ldr r6,=arr
    mov r3,#0
inner_loop:
    ldr r4,[r6]
    ldr r5,[r6,#4]

    cmp r4,r5
    ble no_swap
    str r5,[r6]
    str r4,[r6,#4]

    mov r3,#1
no_swap:
    add r6,r6,#4
    add r2,r2,#1
    sub r7,r0,r1
    cmp r2,r7
    blo inner_loop

    cmp r3,#0
    beq end_start
    b outer_loop
end_start:
    mov r7,#1
    mov r0,#0
    svc 0
    .data
    .align 2
arr:
    .word 8,1,5,2,7,9,6,4,3,10