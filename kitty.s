jsr main
brk




var _n
var _src
var _dst
; (dst, src, n)
lab memcpy
    stv _n
    stv _src
    stv _dst

lab memcpy/loop
    ldv _n
    dup
    lit 0
    equ
    jcn memcpy/done
    lit 1
    sub
    stv _n

    ldv _src
    dup
    inc
    stv _src
    lda

    ldv _dst
    dup
    inc
    stv _dst
    sta

    jmp memcpy/loop

lab memcpy/done
    ret


var _a
var _b
var _return
; (a* b* -- bool)
lab compare32
    stv _b
    stv _a

    lit 1
    stv _return

    ; one byte, two byte, three byte, four
    jsr compare32/do
    jsr compare32/do
    jsr compare32/do
    jsr compare32/do

    ldv _return
    ret

lab compare32/do
    ldv _a
    dup
    inc
    stv _a
    lda

    ldv _b
    dup
    inc
    stv _b
    lda

    equ
    jcn compare32/good

    ;bad
    lit 0
    stv _return
    ret

lab compare32/good
    ret

    

var _done
;(a*)
lab inc32
    stv _a 

    jsr inc32/do
    jcn inc32/done
    jsr inc32/do
    jcn inc32/done
    jsr inc32/do
    jcn inc32/done
    jsr inc32/do
    jcn inc32/done

lab inc32/done
    ret
    

lab inc32/do
    ldv _a
    dup
    inc
    stv _a
    dup

    lda
    inc
    dup
    lit 0
    neq
    stv _done
    swp
    sta

    ldv _done
    ret

    

;(a -- ) IO
lab dbg/u32
    stv _a

    jsr dbg/u32/do
    jsr dbg/u32/do
    jsr dbg/u32/do
    jsr dbg/u32/do

    ret

lab dbg/u32/do
    ldv _a
    dup
    inc
    stv _a
    lda
    dbg
    ret


    
    




    
    


var _iter
var _end
var _name

lab main
    lit 0
    stv _iter
    lit 4
    stv _end
    lit 8
    stv _name

    ldv _name
    str "hello"
    
    ldv _iter
    ldv _name
    s05 ;seek file

    ;copy file header address
    ldv _end 
    ldv _iter
    lit 4
    jsr memcpy



    ldv _iter
    s06 ;open file iterator

    ldv _end
    s07 ;get end of file, start of next header

    ;setup done
lab loop
    ;check done
    ldv _iter
    ldv _end
    jsr compare32
    jcn done

    ;read and transmit char
    ldv _iter
    s02 ;disk read
    out

    ;advance iterator
    ldv _iter
    jsr inc32

    jmp loop


lab done
    ret






    






