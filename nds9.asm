include "include/nds9.inc"
;==============================================================================
 ;      mov     r4, $4000000

 ;      mov     r0, $00
 ;      strb    r0, [r4,1]




;        no$brk
        mov     r1, $000E000
        orr     r1, $6000000
        orr     r2, r1,$0200000
        mov     r0, $80
        mov     r3, $300
      @ strh    r0, [r1],$2
        strh    r0, [r2],$2
        add     r0, 1
        subs    r3, 1
        bne     @b

        adr     r0, Pics
        bl      MEMCOPY

        adr     r0, Pics+$10
        bl      MEMCOPY

        adr     r0, Pics+$20
        bl      MEMCOPY

        adr     r0, Pics+$30
        bl      MEMCOPY

        mov     r4, $7000000
        ldr     r0, [OAM]
        str     r0, [r4,$000]
        str     r0, [r4,$400]
        ldr     r0, [OAM+4]
        str     r0, [r4,$004]
        str     r0, [r4,$404]

        b       DaijuSekasi

Pics:   dw $6002000, LCDA, LCDA.e - LCDA, 0
        dw $6202000, LCDB, LCDB.e - LCDB, 0
        dw $6400000, OBJS, OBJS.e - OBJS, 0
        dw $6600000, OBJS, OBJS.e - OBJS, 0

OAM:    dh $2020,$4050,$0000,$0000

LCDA:
file "img/pic1.bin"
align 4
.e:

LCDB:
file "img/pic2.bin"
align 4
.e:

OBJS:
file "img/objsheet.bin"
align 4
.e:



;==============================================================================
DaijuSekasi:
        mov     r0,1
        mov     r1,1
        swi     $40000


        bl      ipcreceive


        ldr     r1, [.touchx]
        ldr     r2, [.firmuser]
        ldr     r0, [r1]
        ldrh    r4, [r2,$58]
        sub     r4, r0,r4

        ldrb    r0, [r2,$62]
        ldrb    r5, [r2,$5C]
        sub     r5, r0,r5

        ldrh    r0, [r2,$5E]
        ldrh    r6, [r2,$58]
        sub     r6, r0,r6

        ldrb    r0, [r2,$5C]
        sub     r7, r0,1

        mul     r0, r4,r5

        movs    r1,r6
        swine   $90000
        add     r0, r7
        strh    r0, [.objx]


        ldr     r1, [.touchy]
        ldr     r2, [.firmuser]
        ldr     r0, [r1]
        ldrh    r4, [r2,$5A]
        sub     r4, r0,r4

        ldrb    r0, [r2,$63]
        ldrb    r5, [r2,$5D]
        sub     r5, r0,r5

        ldrh    r0, [r2,$60]
        ldrh    r6, [r2,$5A]
        sub     r6, r0,r6

        ldrb    r0, [r2,$5D]
        sub     r7, r0,1

        mul     r0, r4,r5

        movs    r1, r6
        swine   $90000
        add     r0, r7
        strh    r0, [.objy]








      ; strh    r0, [.objy]

        mov     r4, 7000000h
;        ldrh    r0, [.scrollx]
;        ldrh    r1, [.scrolly]
        ldrh    r0, [.objx]
        ldrh    r1, [.objy]
        b @f
 ;      ldr     r3, [r5]
        tst     r3, $0010 ;ri
        addne   r0, 1
        tst     r3, $0020 ;le
        subne   r0, 1
        tst     r3, $0040 ;up
        subne   r1, 1
        tst     r3, $0080 ;do
        addne   r1, 1
;        strh    r0, [.scrollx]
;        strh    r1, [.scrolly]
;        strh    r0, [r4,010h]
;        strh    r1, [r4,012h]
      @ and     r0, $FF
        and     r1, $FF
        sub     r0, 4
        sub     r1, 4
        strh    r0, [.objx]
        strh    r1, [.objy]
        orr     r0, $4000
        orr     r1, $2000
        orr     r4, $400
        strh    r0, [r4,002h]
        strh    r1, [r4,000h]


        b       DaijuSekasi
.firmuser: dw $27FFC80
.touchx: dw penx
.touchy: dw peny
.scrollx: dh    0
.scrolly: dh    0
.objx:    dh    0
.objy:    dh    0






