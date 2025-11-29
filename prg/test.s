jsr main
brk


use lib.mem.s
use lib.heap.s
use lib.string.s

var _size


lab main
    lit 80
    stv _size

    ldv _size
    jsr heap/new

    dup
    str "hello world skibidi rizz"

    dup
    jsr string/print
    jsr string/newline

lab loop
    lit 32 ;space
    jsr string/token
    dup
    lda
    lit 0
    equ
    jcn done
    jsr string/print
    jsr string/newline
    jmp loop

lab done
    ret
    



