;FASMARM MACROS
PROCESSOR $3FE     ; ARM V4T
COPROCESSOR $0
CODE32
@       fix @@:
macro   pad addr,data
{       match o, data \{          ;if not blank
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
macro   address [lab=$]
{       bits = 32
        forward
        match o, lab \{         ;if not blank
         repeat bits/4
         d = '0' + (lab) shr (bits-%*4) and $0F
          if d > '9'
          d = d + 'A'-'9'-1
          end if
         display d

         end repeat
         display " "
        \}
        common
        display 13,10   }
macro   cp15 op1,op2,op3,op4
{       if op1 eqtype r0
        mrc p15,0,op1,op2,op3,op4

        else
         if op4 eqtype 0
         mov r0,op4
         mcr p15,0,r0,op1,op2,op3

         else if op4 eqtype [0]
         ldr r0,op4
         mcr p15,0,r0,op1,op2,op3

         else
         mcr p15,0,op4,op1,op2,op3

         end if
        end if  }
macro no$brk      { dw $E1A0B00B }
macro halt      { swi 60000h}

CODE32
ALIGN 4
org 02000000h
;=============================================================================
;       INIT
;=============================================================================
DS9INIT:mov     r4, $4000000
        mov     r0, 0
        str     r0, [r4,$208]   ;IME OFF
        mov     r0, $9F
        msr     cpsr, r0

        cp15    c1,c0,0, $2000
        mov     r0, 0
        cp15    c7,c5,0, r0     ;??? Invalidate Entire Instruction Cache ??????
        cp15    c7,c6,0, r0     ;??? Invalidate Entire Data Cache ?????????????
        cp15    c7,c10,4,r0     ;??? Drain Write Buffer ???????????????????????
        cp15    c6,c0,0, [.PUDataRegion0]
        cp15    c6,c1,0, [.PUDataRegion1]
        cp15    c6,c2,0, [.PUDataRegion2]
        cp15    c6,c3,0, [.PUDataRegion3]
        cp15    c6,c4,0, [.PUDataRegion4]
        cp15    c6,c5,0, [.PUDataRegion5]
        cp15    c6,c6,0, [.PUDataRegion6]
        cp15    c6,c7,0, [.PUDataRegion7]
        cp15    c9,c1,1,  .ITCMBaseSize
        cp15    c9,c1,0, [.DTCMBaseSize]
        cp15    c2,c0,1,  .PUCodeCache
        cp15    c2,c0,0,  .PUDataCache
        cp15    c3,c0,0,  .PUWriteBuffer
        cp15    c5,c0,3, [.PUCodeAccessE]
        cp15    c5,c0,2, [.PUDataAccessE]
        cp15    c1,c0,0, [.CP15Control]

        mov     r0, $92
        msr     cpsr, r0
        ldr     sp, [.StackBaseIRQ]
        ldr     r0, [.VectorIRQ]
        str     r0, [sp,$60]
        mov     r0, $93
        msr     cpsr, r0
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
        ldr     r0, [.GFXPControl]
        str     r0, [r4,$304]
        ldr     r0, [.VRAMControl+0]
        str     r0, [r4,$240]
        ldr     r0, [.VRAMControl+4]
        str     r0, [r4,$244]
        ldr     r0, [.VRAMControl+8]
        str     r0, [r4,$248]

        ldr     r0, [.LCDControlA]
        str     r0, [r4,$000]
        ldr     r0, [.BGControlA+0]
        str     r0, [r4,$008]
        ldr     r0, [.BGControlA+4]
        str     r0, [r4,$00C]
        orr     r4, $1000
        ldr     r0, [.LCDControlB]
        str     r0, [r4,$000]
        ldr     r0, [.BGControlB+0]
        str     r0, [r4,$008]
        ldr     r0, [.BGControlB+4]
        str     r0, [r4,$00C]

        bl      GenerateRGB332

        mov     r1, $7000000    ;HIDE OBJS
        mov     r0, $200
      @ str     r0, [r1],8
        tst     r1, $800
        beq     @b

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
;------------------------------------------------------------------------------
;PUDataRegion      76543210
.PUDataCache    =  00100010b
.PUCodeCache    =  00100010b
.PUWriteBuffer  =  00000010b
.PUDataAccessE  dw 03633333h ;3=Read/Write, 6=Read
.PUCodeAccessE  dw 03630033h

.PUDataRegion0  dw $04000033 ; 00000001 Enable
.PUDataRegion1  dw $0200002B ; 0000003E Size (2 SHL X) ;min=(X=11)=4KB
.PUDataRegion2  dw $08000033 ; 00000FC0 Reserved/zero
.PUDataRegion3  dw $0100002F ; FFFFF000 Base address (must be SIZE-aligned)
.PUDataRegion4  dw $0000002F ;
.PUDataRegion5  dw $FFFF001D ;          Overlapping Regions are allowed,
.PUDataRegion6  dw $027FF017 ;          Region 7 is having highest priority,
.PUDataRegion7  dw $00000000 ;          region 0 lowest priority.

.ITCMBaseSize   =  $0000001E
.DTCMBaseSize   dw $0100001E

.CP15Control    dw $0005707D

.StackBaseSYS   dw $01003D9C
.StackBaseSVC   dw $01003E9C
.StackBaseIRQ   dw $01003F9C
.StackBaseDBG   dw $027FFD9C
.VectorIRQ      dw DS9IRQ
.VectorDBG      dw DS9DBG
.IRQSetup       dw $00000001

.FIFOmessage:   db "DONE"
;------------------------------------------------------------------------------
                ;  A    B    C    D    E    F    G    wram H    I   - -
.VRAMControl:   db $81, $82, $84, $8a, $00, $00, $00,   3, $00, $82,0,0
.LCDControlA:   dw $00011100
.BGControlA:    dh $1C80, $0000, $0000, $0000
.LCDControlB:   dw $00011100
.BGControlB:    dh $1C80, $0000, $0000, $0000
.GFXPControl:   dw $8203
;==============================================================================
GenerateRGB332:
        mov     r7, $5000000
        mov     r1, 3
        mov     r2, 7
        mov     r3, 7
.loop:  adr     r5, .red
        ldrb    r0, [r5,r3]
        adr     r5, .green
        ldrb    r4, [r5,r2]
        orr     r0, r4,lsl 5
        adr     r5, .blue
        ldrb    r4, [r5,r1]
        orr     r0, r4,lsl 10
        strh    r0, [r7],$2

        subs    r1,1
        movmi   r1,3
        subsmi  r2,1
        movmi   r2,7
        subsmi  r3,1
        movmi   r3,7

        tst     r7, 800h
        beq     .loop

        bx      lr

.blue: db 31,21,11,0
.green:db 31,27,23,18,13,9,4,0
.red:  db 31,27,22,18,13,9,5,0
;##############################################################################
DS9IRQ: ;no$brk
        mov     r3, $4000000
        ldr     r0, [r3,210h]   ;IE
        ldr     r1, [r3,214h]   ;IF
        and     r0, r1
        str     r0, [r3,214h]   ;IF

        cp15    r3, c9,c1,0
        lsr     r3, 12
        lsl     r3, 12
        add     r3, 4000h
        ldr     r1, [r3,-8]
        orr     r1, r0
        str     r1, [r3,-8]

        bx      lr
;==============================================================================
DS9DBG: no$brk
        SWI     $60000
        B       DS9DBG
;##############################################################################
MEMCOPY:ldr     r1, [r0], $4    ;r1=DST
        ldr     r2, [r0], $4    ;r2=SRC
        ldr     r3, [r0]        ;r3=COUNT
.loop:  ldr     r0, [r2], $4
        str     r0, [r1], $4
        subs    r3, $4
        bne     .loop
        bx      lr
;##############################################################################
ipcreceive:
        mov     r4, $4000000                  
        mov     r0, $900
        str     r0, [r4,$180]                 
                                              
        mov     r0, $C000
.wait:; ldr     r1, [r4,$184]
      ; tst     r1, $200
      ; bne     .full
                                              
      ; tst     r1, $100
      ; bne     .empty
      ; beq     .empty

.full:  str     r0, [r4,$184]
        mov     r4, $4100000
        mov     r3, 56
        adr     r2, fiforeceivedata
      @ ldr     r0, [r4]
        str     r0, [r2], $4
        subs    r3, 4                         
        bne @b                                
                                              
.empty: bx      lr
                                              
fiforeceivedata:
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
;##############################################################################
START: