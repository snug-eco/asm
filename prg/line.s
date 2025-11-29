jsr main
brk


use lib.mem.s
use lib.heap.s
use lib.string.s
use lib.line.s


var _size


lab main
    lit 20
    stv _size

    ldv _size
    jsr heap/new
    dup

    ldv _size
    jsr line
    
    jsr string/print    

    lit 10
    out
    lit 13
    out

    ret
    



