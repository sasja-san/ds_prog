include "include/nds7.inc"
;==============================================================
forever:

       ;Halt
        mov     r0,1
        mov     r1,1
        swi     $40000
        bl      PADREAD
        mov     r1, $1000
      @ subs    r1,1
        bne @b
        bl      ipcsend
        b       forever






