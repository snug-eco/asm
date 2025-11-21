jsr main
brk

var _ptr
var _buffer

lab main
    lit 0
    stv _buffer

    ;write string into data memory at $0
    lit _buffer
    dup
    stv _ptr ;store base ptr
    str "hello world" ;write string into buffer

lab main/loop
    ldv _ptr
    dup
    lda ;read char from buffer
    dup
    lit 0
    equ ;check null terminator
    jcn main/done
    out ;output char
    inc ;inc ptr
    stv _ptr
    jmp main/loop

lab main/done
    pop ;remove char copy
    pop ;remove ptr copy

    lit 10
    out
    ret







