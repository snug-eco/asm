jsr main
brk

( n -- !n )
lab fac
    dup
    lit 1
    equ
    jcn fac/done
    dup
    lit 1
    sub
    jsr fac
    mul
    ret

lab fac/done
    ret

lab main
    lit 5
    jsr fac
    dbg
