;FASMARM MACROS
PROCESSOR $FE     ; ARM V4T
COPROCESSOR $0
CODE32
@       fix @@:
macro   pad addr,data {
        match o, data \{          ;if not blank
         if  data eqtype ""
         virtual at 0            ; count "ascii" size
         db data                 ; for divider
         addrsize = ($-0)        ;
         end virtual             ;
         db (addr-$)/addrsize dup (data)
         else
         db (addr-$) dup (data)
         end if
        \}
        db (addr-$) dup $00     }
macro   hx [data] { db $#data }
macro no$brk      { dw $E1A0B00B }
macro Halt      { swi 60000h}
macro WaitByLoop op   { mov r0,op
                  swi 30000h}
CODE32
ALIGN 4
org 37F8000h
;=============================================================================
;       INIT
;=============================================================================
DS7INIT:mov     r4, $4000000
        mov     r0, 0
        str     r0, [r4,$208]   ;IME OFF

        mov     r0, $92
        msr     cpsr_fc, r0
        ldr     sp, [.StackBaseIRQ]
        ldr     r0, [.VectorIRQ]
        str     r0, [sp,$FC]

        mov     r0, $93
        msr     cpsr_fc, r0
        ldr     sp, [.StackBaseSVC]

        mov     r0, $97
        msr     cpsr, r0
        ldr     sp, [.StackBaseDBG]
        mov     r0, $9B
        msr     cpsr, r0
        ldr     sp, [.StackBaseDBG]
        ldr     r0, [.VectorDBG]
        str     r0, [sp]

        mov     r0, $9F
        msr     cpsr, r0
        ldr     sp, [.StackBaseSYS]

        mov     r4, $4000000
        mov     r0, $08
        strh    r0, [r4,$004]   ;DISPSTAT VBLANK IRQ ON
        orr     r0, $8000
        str     r0, [r4,$184]   ;Flush Send Fifo
        mov     r0, 1
        str     r0, [r4,$208]   ;IME ON
        ldr     r0, [.IRQSetup]
        str     r0, [r4,$210]   ;IE VBLANK IRQ ON
        mov     r0, $1F
        msr     cpsr, r0        ;IRQ ON / SYS MODE

        mov     r0, 0
        str     r0, [r4,$180]
        ldr     r1, [.FIFOmessage]
        str     r1, [r4,$188]
        orr     r4, $0100000
      @ ldr     r0, [r4]
        cmp     r0, r1
        bne @b
        B       START

.StackBaseSYS   dw $0380FD00
.StackBaseSVC   dw $0380FE00
.StackBaseIRQ   dw $0380FF00
.StackBaseDBG   dw $0380FD9C
.VectorIRQ      dw DS7IRQ
.VectorDBG      dw DS7DBG
.IRQSetup       dw $00000001

.FIFOmessage:   db "DONE"
;##############################################################
DS7IRQ: ;no$brk
        mov     r3, $4000000
        ldr     r0, [r3,210h]   ;IE
        ldr     r1, [r3,214h]   ;IF
        and     r0, r1
        str     r0, [r3,214h]   ;IF

        mov     r3, 03800000h
        orr     r3, 10000h
        ldr     r1, [r3,-8]
        orr     r1, r0
        str     r1, [r3,-8]
        bx      lr

DS7DBG: MOV     R11,R11
        SWI     $60000
        B       DS7DBG
;==============================================================
MEMCOPY:ldr     r1, [r0], $4    ;r1=DST
        ldr     r2, [r0], $4    ;r2=SRC
        ldr     r3, [r0]        ;r3=COUNT
.loop:  ldr     r0, [r2], $4
        str     r0, [r1], $4
        subs    r3, $4
        bne     .loop
        bx      lr
;==============================================================
ipcsend:
        mov     r4, $4000000
        mov     r0, $700
        str     r0, [r4,$180]

        mov     r0, $8000
;.wait:  ldr     r1, [r4,$184]
;        tst     r1, 1
;        bne     .empty

;        tst     r1, 2
;        beq     .done   ;not full

        orr     r0, $0008
.empty: str     r0, [r4,$184]
        mov     r3, 60
        adr     r2, fifosenddata
      @ ldr     r0, [r2], $4
        str     r0, [r4,$188]
        subs    r3, 4
        bne @b

.done:  bx lr
;==============================================================
fifosenddata:
padhold dw $00001000
padtap  dw $00001001
padrel  dw $00001002
; 00000001  Button A
; 00000002  Button B
; 00000004  Select
; 00000008  Start
; 00000010  Right
; 00000020  Left
; 00000040  Up
; 00000080  Down
; 00000100  Button R
; 00000200  Button L
; 00010000  Button X
; 00020000  Button Y
; 00080000  DEBUG button
; 00400000  Pen down
; 00800000  Hinge/folded
penx    dw $00001003
peny    dw $00001004
.data05 dw $00001005
.data06 dw $00001006
.data07 dw $00001007
.data08 dw $00001008
.data09 dw $00001009
.data10 dw $00001010
.data11 dw $00001011
.data12 dw $00001012
.data13 dw $00001013
.data14 dw $00001014
.data15 dw $00001015
;==============================================================
PADREAD:
        push    {lr}
        mov     r4, $4000000
        orr     r4, $130
        ldr     r1, [padhold]

        ldrh    r0, [r4,6]
        mov     r2, r0,LSL 16
        ldrh    r0, [r4]
        orr     r0, r2
        mov     r3, $ffffffff
        eor     r0, r3

        eor     r1, r0
        mov     r2, r1
        and     r1, r0
        str     r1, [padtap]
        eor     r2, r1
        str     r2, [padrel]
        str     r0, [padhold]

        tst     r1, 400000h
        bne .done
        tst     r2, 400000h
        bne .done
        tst     r0, 400000h
        beq .done
;--------------------------------------------------------------
.TOUCHREAD:
        mov     r4, 4000000h
        mov     r0, 0
        str     r0, [r4,208h]
        orr     r4, 1C0h

        bl      .readpen
        str     r2, [penx+8]
        str     r3, [peny+8]
        bl      .readpen

     ;  cmp     r2, $100
     ;  blo     .skip
     ;  cmp     r2, $F00
     ;  bhs     .skip
        ldr     r1, [penx+8]
        subs    r0,r1,r2
        negmi   r0
        cmp     r0,$20
        strlo   r2, [penx]

;.skip:  cmp     r3, $100
;        blo     .done
;        cmp     r3, $F00
;        bhs     .done
        ldr     r1, [peny+8]
        subs    r0,r1,r3
        negmi   r0
        cmp     r0,$20
        strlo   r3, [peny]


.done:  mov     r4, 4000000h
        mov     r0, 1
        str     r0, [r4,208h]
        pop     {lr}
        bx      lr

;------------------------------
.readpen:
        push    {lr}

        bl      .pollwait
        mov     r0, 8A00h
        orr     r0, 0001h
        strh    r0, [r4,0]
        mov     r0, 0D0h
        strh    r0, [r4,2]
        bl      .pollwait
        mov     r0, 0
        strh    r0, [r4,2]
        bl      .pollwait
        ldrh    r0, [r4,2]

        lsl     r0, 5
        and     r0, $FE0
        mov     r1, r0

        mov     r0, 090h
        strh    r0, [r4,2]
        bl      .pollwait
        ldrh    r0, [r4,2]

        lsr     r0, 3
        and     r0, $1F
        orr     r2, r0, r1

        mov     r0, 0
        strh    r0, [r4,2]
        bl      .pollwait
        ldrh    r0, [r4,2]

        lsl     r0, 5
        and     r0, $FE0
        mov     r1, r0

        mov     r0, 8200h
        orr     r0, 0001h
        strh    r0, [r4,0]
        mov     r0, 000h
        strh    r0, [r4,2]
        bl      .pollwait
        ldrh    r0, [r4,2]

        lsr     r0, 3
        and     r0, $1F
        orr     r3, r0, r1

        pop     {lr}
        bx      lr
;----------------------
.pollwait:
      @ ldr     r0, [r4,0]
        tst     r0, 80h
        bne     @b
        bx      lr
;##############################################################
START: