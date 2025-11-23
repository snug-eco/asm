jsr main
brk

use lib.quad.s
use lib.mem.s
    


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
    jsr mem/cpy

    ldv _iter
    s06 ;open file iterator

    ldv _end
    s07 ;get end of file, start of next header


    ;setup done
lab loop
    ;check done
    ldv _iter
    ldv _end
    jsr quad/compare
    jcn done


    ;read and transmit char
    ldv _iter
    s02 ;disk read
    out

    ;advance iterator
    ldv _iter
    jsr quad/increment

    jmp loop


lab done
    ret






    






