%include "test.inc"

BITS 16

; Primary opcode table

; Row 0
TEST_ARITHMETIC16 add
push es
pop es
TEST_ARITHMETIC16 or
push cs

; Row 1
TEST_ARITHMETIC16 adc
push ss
pop ss
TEST_ARITHMETIC16 sbb
push ds
pop ds

; Row 2
TEST_ARITHMETIC16 and
daa
TEST_ARITHMETIC16 sub
das

; Row 3
TEST_ARITHMETIC16 xor
aaa
TEST_ARITHMETIC16 cmp
aas

; Row 4
TEST_ARITHMETIC_ONE_PARAM16 inc
TEST_ARITHMETIC_ONE_PARAM16 dec

; Row 5
TEST_ARITHMETIC_ONE_PARAM16 push
TEST_ARITHMETIC_ONE_PARAM16 pop

; Row6
pusha
popa

; TODO More thorough bound
bound ax, [1]
bound cx, [1]
bound dx, [1]
bound bx, [1]
bound sp, [1]
bound bp, [1]
bound si, [1]
bound di, [1]

TEST_ARITHMETIC_MODRM16 arpl

push 0
push 1
push 0xff
push 0xffff

; TODO: Write a better test
imul bx, cx, -1

push 0
push 1
push 0xff

; TODO: Write a better test
imul dx, ax, 0xff

; TODO: rep!
insb
insw
outsb
outsw

; Row 7
; Can't get yasm to emit the one byte form of these jmps, so do it here
; jo
db 0x70, 1
db 0x70, 0xff

; jno
db 0x71, 1
db 0x71, 0xff

; jb
db 0x72, 1
db 0x72, 0xff

; jnb
db 0x73, 1
db 0x73, 0xff

; jz
db 0x74, 1
db 0x74, 0xff

; jnz
db 0x75, 1
db 0x75, 0xff

; jbe
db 0x76, 1
db 0x76, 0xff

; jnbe
db 0x77, 1
db 0x77, 0xff

; js
db 0x78, 1
db 0x78, 0xff

; jns
db 0x79, 1
db 0x79, 0xff

; jp
db 0x7a, 1
db 0x7a, 0xff

; jnp
db 0x7b, 1
db 0x7b, 0xff

; jl
db 0x7c, 1
db 0x7c, 0xff

; jnl
db 0x7d, 1
db 0x7d, 0xff

; jle
db 0x7e, 1
db 0x7e, 0xff

; jnle
db 0x7f, 1
db 0x7f, 0xff

; Row 8
%macro TEST_GROUP1_16 1
TEST_ARITHMETIC_RM8 %1, 1
TEST_ARITHMETIC_RM8 %1, 0xff
TEST_ARITHMETIC_RM16 %1, 0xffff
%endmacro ; TEST_GROUP1_16

TEST_GROUP1_16 add
TEST_GROUP1_16 or
TEST_GROUP1_16 adc
TEST_GROUP1_16 sbb
TEST_GROUP1_16 and
TEST_GROUP1_16 sub
TEST_GROUP1_16 xor
TEST_GROUP1_16 cmp

TEST_ARITHMETIC16 test
TEST_ARITHMETIC_MODRM8_REV xchg
TEST_ARITHMETIC_MODRM16 xchg
TEST_ARITHMETIC16 mov

; TODO: test mov Mw/Rv, Sw

TEST_ARITHMETIC_RM16_REV mov, es
TEST_ARITHMETIC_RM16_REV mov, ds
TEST_ARITHMETIC_RM16_REV mov, cs
TEST_ARITHMETIC_RM16_REV mov, ss
TEST_ARITHMETIC_RM16_REV mov, fs
TEST_ARITHMETIC_RM16_REV mov, gs

; TODO: lea will be invalid for gpr
; Test ModRM Mod==0
%macro TEST_LEA16 1
lea %1, word [BX + SI]
lea %1, word [BX + DI]
lea %1, word [BP + SI]
lea %1, word [BP + DI]
lea %1, word [SI]
lea %1, word [DI]
lea %1, word [0xffff]
lea %1, word [0x0001]
lea %1, word [BX]

; Test ModRM Mod==1
lea %1, word [BX + SI + 1]
lea %1, word [BX + SI + 0xff]
lea %1, word [BX + DI + 1]
lea %1, word [BX + DI + 0xff]
lea %1, word [SI + 1]
lea %1, word [SI + 0xff]
lea %1, word [DI + 1]
lea %1, word [DI + 0xff]
lea %1, word [BP + 1]
lea %1, word [BP + 0xff]
lea %1, word [BX + 1]
lea %1, word [BX + 0xff]

; Test ModRM Mod==2
lea %1, word [BX + SI + 0xffff]
lea %1, word [BX + DI + 0xffff]
lea %1, word [BP + SI + 0xffff]
lea %1, word [BP + DI + 0xffff]
lea %1, word [SI + 0xffff]
lea %1, word [DI + 0xffff]
lea %1, word [BP + 0xffff]
lea %1, word [BX + 0xffff]
%endmacro ; TEST_LEA16

TEST_LEA16 ax
TEST_LEA16 cx
TEST_LEA16 dx
TEST_LEA16 bx
TEST_LEA16 sp
TEST_LEA16 bp
TEST_LEA16 si
TEST_LEA16 di

TEST_ARITHMETIC_RM16 mov, es
TEST_ARITHMETIC_RM16 mov, ds
TEST_ARITHMETIC_RM16 mov, cs
TEST_ARITHMETIC_RM16 mov, ss
TEST_ARITHMETIC_RM16 mov, fs
TEST_ARITHMETIC_RM16 mov, gs

; Row 9
; one byte r/r form
nop
xchg cx, ax
xchg dx, ax
xchg bx, ax
xchg sp, ax
xchg bp, ax
xchg si, ax
xchg di, ax

cbw
cwd
call [0xffff]
fwait
pusf
popf
sahf
lahf

; Row 0xa
mov al, [0x1]
mov al, [0xff]
mov ax, [0x1]
mov ax, [0xffff]
mov [0x1], al
mov [0xff], al
mov [0x1], ax
mov [0xffff], ax

movsb
movsw
cmpsb
cmpsw

test al, 0x1
test ax, 0xffff

stosb
stosw
lodsb
lodsw
scasb
scasw

; Row 0xb
mov al, 0xff
mov cl, 0xff
mov dl, 0xff
mov bl, 0xff
mov ah, 0xff
mov ch, 0xff
mov dh, 0xff
mov bh, 0xff

mov ax, 0xffff
mov cx, 0xffff
mov dx, 0xffff
mov bx, 0xffff
mov sp, 0xffff
mov bp, 0xffff
mov si, 0xffff
mov di, 0xffff

; Row 0xc
%macro Group2Row 2
rol %1, %2
ror %1, %2
rcl %1, %2
rcr %1, %2
shl %1, %2
shr %1, %2
sal %1, %2
sar %1, %2
%endmacro ; Group2Row
%macro Group2Col 1
; Row 0 (c0)
Group2Row %1l, 2
Group2Row %1h, 2

; Row 1 (c1)
Group2Row %1x, 2

; Row 2 (d0)
Group2Row %1l, 1
Group2Row %1h, 1

; Row 3 (d1)
Group2Row %1x, 1

; Row 4 (d2)
Group2Row %1l, cl
Group2Row %1h, cl

; Row 5 (d3)
Group2Row %1x, cl
%endmacro ; Group2Col

Group2Col a
Group2Col c
Group2Col d
Group2Col b
Group2Row sp, 2
Group2Row bp, 2
Group2Row si, 2
Group2Row di, 2

aam 0x1
aad 0x1

xlat
xlatb

retn 0xffff
retn
; FIXME
; les [0xffff]
; lds [0xffff]

; Group11 eb, ib, ev, iz
mov al, 2
mov cl, 2
mov dl, 2
mov bl, 2
mov ah, 2
mov ch, 2
mov dh, 2
mov bh, 2

mov ax, 0xffff
mov cx, 0xffff
mov dx, 0xffff
mov bx, 0xffff
mov sp, 0xffff
mov bp, 0xffff
mov si, 0xffff
mov di, 0xffff

enter 0xffff, 0x1f
leave

retf 0xffff
retf

int3
int 2

into

iret

; Row 0xd

; 0xd8
%macro FpuMemoryOperand 2
	; Test ModRM Mod==0
	%1 %2 [BX + SI]
	%1 %2 [BX + DI]
	%1 %2 [BP + SI]
	%1 %2 [BP + DI]
	%1 %2 [SI]
	%1 %2 [DI]
	%1 %2 [0xffff]
	%1 %2 [0x0001]
	%1 %2 [BX]

	; Test ModRM Mod==1
	%1 %2 [BX + SI + 1]
	%1 %2 [BX + SI + 0xff]
	%1 %2 [BX + DI + 1]
	%1 %2 [BX + DI + 0xff]
	%1 %2 [SI + 1]
	%1 %2 [SI + 0xff]
	%1 %2 [DI + 1]
	%1 %2 [DI + 0xff]
	%1 %2 [BP + 1]
	%1 %2 [BP + 0xff]
	%1 %2 [BX + 1]
	%1 %2 [BX + 0xff]

	; Test ModRM Mod==2
	%1 %2 [BX + SI + 0xffff]
	%1 %2 [BX + DI + 0xffff]
	%1 %2 [BP + SI + 0xffff]
	%1 %2 [BP + DI + 0xffff]
	%1 %2 [SI + 0xffff]
	%1 %2 [DI + 0xffff]
	%1 %2 [BP + 0xffff]
	%1 %2 [BX + 0xffff]
%endmacro ; TEST_ARITHMETIC_RM16

%macro FpuCol 2
FpuMemoryOperand %1, dword
	%1 %2
	%1 %2
	%1 %2
	%1 %2
	%1 %2
	%1 %2
	%1 %2
	%1 %2
%endmacro ; FpuCol

%macro FpuColNoMem 2
	%1 %2
	%1 %2
	%1 %2
	%1 %2
	%1 %2
	%1 %2
	%1 %2
	%1 %2
%endmacro ; FpuCol

%macro FpuRowD8 1
FpuCol fadd, %1
FpuCol fmul, %1
FpuCol fcom, %1
FpuCol fcomp, %1
FpuCol fsub, %1
FpuCol fsubr, %1
FpuCol fdiv, %1
FpuCol fdivr, %1
%endmacro ; FpuRowD8

FpuRowD8 st0
FpuRowD8 st1
FpuRowD8 st2
FpuRowD8 st3
FpuRowD8 st4
FpuRowD8 st5
FpuRowD8 st6
FpuRowD8 st0

; 0xd9
%macro FpuRowD9 1
FpuCol fld, %1
%endmacro FpuRowD9

FpuRowD9 st0
FpuRowD9 st1
FpuRowD9 st2
FpuRowD9 st3
FpuRowD9 st4
FpuRowD9 st5
FpuRowD9 st6
FpuRowD9 st7

%macro FpuRowNoMemD9 1
FpuColNoMem %1, st0
FpuColNoMem %1, st0
FpuColNoMem %1, st0
FpuColNoMem %1, st0
FpuColNoMem %1, st0
FpuColNoMem %1, st0
FpuColNoMem %1, st0
FpuColNoMem %1, st0
%endmacro

FpuRowNoMem fxch
FpuMemoryOperand fst, dword
fnop
FpuMemoryOperand fstp, dword
FpuMemoryOperand fstp, dword
fldenv [0xffff]
FpuMemoryOperand fldcw, word
fstenv [0xffff]
FpuMemoryOperand fnstcw, word
fchs
fabs
ftst
fxam
fld1
fldl2t
fldl2e
fldpi
fldlg2
fldln2
fldz
f2xm1
fyl2x
fptan
fpatan
fxtract
fprem1
fdecstp
fincstp
fprem
fyl2xp1
fsqrt
fsincos
frndint
fscale
fsin
fcos

; 0xda
fiadd dword [0xffff]
fimul dword [0xffff]
ficom dword [0xffff]
ficomp dword [0xffff]
fisub dword [0xffff]
fisubr dword [0xffff]
fidiv dword [0xffff]
fidivr dword [0xffff]

%macro fpcomp 1
fcmovb st0, %1
fcmove st0, %1
fcmovbe st0, %1
fcmovu st0, %1
%endmacro ;fpcomp

fpcomp st0
fpcomp st1
fpcomp st2
fpcomp st3
fpcomp st4
fpcomp st5
fpcomp st6
fpcomp st7

fucompp

; 0xdb
FpuMemoryOperand fild, dword
FpuMemoryOperand fisttp, dword
FpuMemoryOperand fist, dword
FpuMemoryOperand fistp, dword
FpuMemoryOperand fld, tword
FpuMemoryOperand fstp, tword

fnclex
fninit

%macro fpcomp2 1
fcmovnb st0, %1
fcmovne st0, %1
fcmovnbe st0, %1
fcmovnu st0, %1
fucomi st0, %1
fcomi st0, %1
%endmacro ; fpcomp2

fpcomp2 st0
fpcomp2 st1
fpcomp2 st2
fpcomp2 st3
fpcomp2 st4
fpcomp2 st5
fpcomp2 st6
fpcomp2 st7

; 0xdc
FpuMemoryOperand fadd, qword
FpuMemoryOperand fmul, qword
FpuMemoryOperand fcom, qword
FpuMemoryOperand fcomp, qword
FpuMemoryOperand fsub, qword
FpuMemoryOperand fsubr, qword
FpuMemoryOperand fdiv, qword
FpuMemoryOperand fdivr, qword

%macro farithmetic 1
fadd st0, %1
fmul st0, %1
fsubr st0, %1
fsub st0, %1
fdivr st0, %1
fdiv st0, %1
%endmacro ; farithmetic

farithmetic st0
farithmetic st1
farithmetic st2
farithmetic st3
farithmetic st4
farithmetic st5
farithmetic st6
farithmetic st7

; 0xdd
FpuMemoryOperand fld, qword
FpuMemoryOperand fisttp, qword
FpuMemoryOperand fst, qword
FpuMemoryOperand fstp, qword
frstor [0xffff]
fnsave [0xffff]
FpuMemoryOperand fnstsw, word

%macro floadstore 1
ffree %1
fst %1
fstp %1
fucom st0, %1
fucomp %1
%endmacro ; floadstore

floadstore st0
floadstore st1
floadstore st2
floadstore st3
floadstore st4
floadstore st5
floadstore st6
floadstore st7

; 0xde
FpuMemoryOperand fiadd, word
FpuMemoryOperand fimul, word
FpuMemoryOperand ficom, word
FpuMemoryOperand ficomp, word
FpuMemoryOperand fisub, word
FpuMemoryOperand fisubr, word
FpuMemoryOperand fidiv, word
FpuMemoryOperand fidivr, word

fcompp

%macro farithmeticp 1
faddp %1, st0
fmulp %1, st0
fsubrp %1, st0
fsubp %1, st0
fdivrp %1, st0
fdivp %1, st0
%endmacro ; farithmeticp

farithmeticp st0
farithmeticp st1
farithmeticp st2
farithmeticp st3
farithmeticp st4
farithmeticp st5
farithmeticp st6
farithmeticp st7

; 0xdf
FpuMemoryOperand fild, word
FpuMemoryOperand fisttp, word
FpuMemoryOperand fist, word
FpuMemoryOperand fistp, word
FpuMemoryOperand fbld, tword
FpuMemoryOperand fld, qword
FpuMemoryOperand fbstp, tword
FpuMemoryOperand fistp, qword

fnstsw ax

fucomip st0
fucomip st1
fucomip st2
fucomip st3
fucomip st4
fucomip st5
fucomip st6
fucomip st7

fcomip st0
fcomip st1
fcomip st2
fcomip st3
fcomip st4
fcomip st5
fcomip st6
fcomip st7

; Row 0xe
loopne .label
.label
loope .label2
.label2
loop .label3
.label3
jcxz .label4
.label4

in al, 0xff
in ax, 0xff
out 0xff, al
out 0xff, ax

call 0xffff
jmp 0xffff
jmp [0xffff]
jmp .label5
.label5

in al, dx
in ax, dx
out dx, al
out dx, ax

; Row 0xf
int1
hlt
cmc

; Group3
%macro Group3Col 1
test %1, 2
not %1
neg %1
mul %1
imul %1
div %1
idiv %1
%endmacro ; Group3Col
%macro Group3Row 1
Group3Col %1l
Group3Col %1h
Group3Col %1x
%endmacro ; Group3Row

Group3Row a
Group3Row b
Group3Row d
Group3Row b
Group3Col sp
Group3Col bp
Group3Col si
Group3Col di

clc
stc
cli
sti
cld
std

; Group4
inc al
inc cl
inc dl
inc bl
inc ah
inc ch
inc dh
inc bh
dec al
dec cl
dec dl
dec bl
dec ah
dec ch
dec dh
dec bh

; Group5
inc ax
inc cx
inc dx
inc bx
inc sp
inc bp
inc si
inc di

dec ax
dec cx
dec dx
dec bx
dec sp
dec bp
dec si
dec di

call ax
call cx
call dx
call bx
call sp
call bp
call si
call di

call [0xffff]

jmp ax
jmp cx
jmp dx
jmp bx
jmp sp
jmp bp
jmp si
jmp di

jmp [0xffff]

push ax
push cx
push dx
push bx
push sp
push bp
push si
push di
