%macro TEST_ARITHMETIC_RM8 2
	; Test ModRM Mod==0
	%1 byte [BX + SI], %2
	%1 byte [BX + DI], %2
	%1 byte [BP + SI], %2
	%1 byte [SI], %2
	%1 byte [DI], %2
	%1 byte [0xffff], %2
	%1 byte [0x0001], %2
	%1 byte [BX], %2

	; Test ModRM Mod==1
	%1 byte [BX + SI + 1], %2
	%1 byte [BX + SI + 0xff], %2
	%1 byte [SI + 1], %2
	%1 byte [SI + 0xff], %2
	%1 byte [DI + 1], %2
	%1 byte [DI + 0xff], %2
	%1 byte [BP + 1], %2
	%1 byte [BP + 0xff], %2
	%1 byte [BX + 1], %2
	%1 byte [BX + 0xff], %2

	; Test ModRM Mod==2
	%1 byte [BX + SI + 0xffff], %2
	%1 byte [SI + 0xffff], %2
	%1 byte [DI + 0xffff], %2
	%1 byte [BP + 0xffff], %2
	%1 byte [BX + 0xffff], %2

	; Test ModRM Mod==3
	%1 AL, %2
	%1 CL, %2
	%1 DL, %2
	%1 BL, %2
	%1 AH, %2
	%1 CH, %2
	%1 DH, %2
	%1 BH, %2
%endmacro ; TEST_ARITHMETIC_RM8

%macro TEST_ARITHMETIC_RM8_REV 2
	; Test ModRM Mod==0
	%1 %2, byte [BX + SI]
	%1 %2, byte [BX + DI]
	%1 %2, byte [BP + SI]
	%1 %2, byte [SI]
	%1 %2, byte [DI]
	%1 %2, byte [0xffff]
	%1 %2, byte [0x0001]
	%1 %2, byte [BX]

	; Test ModRM Mod==1
	%1 %2, byte [BX + SI + 1]
	%1 %2, byte [BX + SI + 0xff]
	%1 %2, byte [SI + 1]
	%1 %2, byte [SI + 0xff]
	%1 %2, byte [DI + 1]
	%1 %2, byte [DI + 0xff]
	%1 %2, byte [BP + 1]
	%1 %2, byte [BP + 0xff]
	%1 %2, byte [BX + 1]
	%1 %2, byte [BX + 0xff]

	; Test ModRM Mod==2
	%1 %2, byte [BX + SI + 0xffff]
	%1 %2, byte [SI + 0xffff]
	%1 %2, byte [DI + 0xffff]
	%1 %2, byte [BP + 0xffff]
	%1 %2, byte [BX + 0xffff]

	; Test ModRM Mod==3
	%1 %2, AL
	%1 %2, CL
	%1 %2, DL
	%1 %2, BL
	%1 %2, AH
	%1 %2, CH
	%1 %2, DH
	%1 %2, BH
%endmacro ; TEST_ARITHMETIC_RM8

%macro TEST_ARITHMETIC_RM16 2
	; Test ModRM Mod==0
	%1 word [BX + SI], %2
	%1 word [BX + DI], %2
	%1 word [BP + SI], %2
	%1 word [SI], %2
	%1 word [DI], %2
	%1 word [0xffff], %2
	%1 word [0x0001], %2
	%1 word [BX], %2

	; Test ModRM Mod==1
	%1 word [BX + SI + 1], %2
	%1 word [BX + SI + 0xff], %2
	%1 word [SI + 1], %2
	%1 word [SI + 0xff], %2
	%1 word [DI + 1], %2
	%1 word [DI + 0xff], %2
	%1 word [BP + 1], %2
	%1 word [BP + 0xff], %2
	%1 word [BX + 1], %2
	%1 word [BX + 0xff], %2

	; Test ModRM Mod==2
	%1 word [BX + SI + 0xffff], %2
	%1 word [SI + 0xffff], %2
	%1 word [DI + 0xffff], %2
	%1 word [BP + 0xffff], %2
	%1 word [BX + 0xffff], %2

	; Test ModRM Mod==3
	%1 AX, %2
	%1 CX, %2
	%1 DX, %2
	%1 BX, %2
	%1 SP, %2
	%1 BP, %2
	%1 SI, %2
	%1 DI, %2
%endmacro ; TEST_ARITHMETIC_RM16

%macro TEST_ARITHMETIC_RM16_REV 2
	; Test ModRM Mod==0
	%1 %2, word [BX + SI]
	%1 %2, word [BX + DI]
	%1 %2, word [BP + SI]
	%1 %2, word [SI]
	%1 %2, word [DI]
	%1 %2, word [0xffff]
	%1 %2, word [0x0001]
	%1 %2, word [BX]

	; Test ModRM Mod==1
	%1 %2, word [BX + SI + 1]
	%1 %2, word [BX + SI + 0xff]
	%1 %2, word [SI + 1]
	%1 %2, word [SI + 0xff]
	%1 %2, word [DI + 1]
	%1 %2, word [DI + 0xff]
	%1 %2, word [BP + 1]
	%1 %2, word [BP + 0xff]
	%1 %2, word [BX + 1]
	%1 %2, word [BX + 0xff]

	; Test ModRM Mod==2
	%1 %2, word [BX + SI + 0xffff]
	%1 %2, word [SI + 0xffff]
	%1 %2, word [DI + 0xffff]
	%1 %2, word [BP + 0xffff]
	%1 %2, word [BX + 0xffff]

	; Test ModRM Mod==3
	%1 %2, AX
	%1 %2, CX
	%1 %2, DX
	%1 %2, BX
	%1 %2, SP
	%1 %2, BP
	%1 %2, SI
	%1 %2, DI
%endmacro ; TEST_ARITHMETIC_RM16

%macro TEST_ARITHMETIC_MODRM8 1
	TEST_ARITHMETIC_RM8 %1, al
	TEST_ARITHMETIC_RM8 %1, cl
	TEST_ARITHMETIC_RM8 %1, dl
	TEST_ARITHMETIC_RM8 %1, bl
	TEST_ARITHMETIC_RM8 %1, ah
	TEST_ARITHMETIC_RM8 %1, ch
	TEST_ARITHMETIC_RM8 %1, dh
	TEST_ARITHMETIC_RM8 %1, bh
%endmacro ; TEST_ARITHMETIC_MODRM8

%macro TEST_ARITHMETIC_MODRM8_REV 1
	TEST_ARITHMETIC_RM8_REV %1, al
	TEST_ARITHMETIC_RM8_REV %1, cl
	TEST_ARITHMETIC_RM8_REV %1, dl
	TEST_ARITHMETIC_RM8_REV %1, bl
	TEST_ARITHMETIC_RM8_REV %1, ah
	TEST_ARITHMETIC_RM8_REV %1, ch
	TEST_ARITHMETIC_RM8_REV %1, dh
	TEST_ARITHMETIC_RM8_REV %1, bh
%endmacro ; TEST_ARITHMETIC_MODRM8_REV

%macro TEST_ARITHMETIC_MODRM16 1
	TEST_ARITHMETIC_RM16 %1, ax
	TEST_ARITHMETIC_RM16 %1, cx
	TEST_ARITHMETIC_RM16 %1, dx
	TEST_ARITHMETIC_RM16 %1, bx
	TEST_ARITHMETIC_RM16 %1, sp
	TEST_ARITHMETIC_RM16 %1, bp
	TEST_ARITHMETIC_RM16 %1, si
	TEST_ARITHMETIC_RM16 %1, di
%endmacro ; TEST_ARITHMETIC_MODRM16

%macro TEST_ARITHMETIC_MODRM16_REV 1
	TEST_ARITHMETIC_RM16_REV %1, ax
	TEST_ARITHMETIC_RM16_REV %1, cx
	TEST_ARITHMETIC_RM16_REV %1, dx
	TEST_ARITHMETIC_RM16_REV %1, bx
	TEST_ARITHMETIC_RM16_REV %1, sp
	TEST_ARITHMETIC_RM16_REV %1, bp
	TEST_ARITHMETIC_RM16_REV %1, si
	TEST_ARITHMETIC_RM16_REV %1, di
%endmacro ; TEST_ARITHMETIC_MODRM16_REV

%macro TEST_ARITHMETIC_IMM8 1
	%1 al, 1
	%1 al, 0xff
%endmacro ; TEST_ARITHMETIC_IMM8

%macro TEST_ARITHMETIC_IMM16 1
	%1 ax, 0x0101
	%1 ax, 0xffff
%endmacro ; TEST_ARITHMETIC_IMM16

%macro TEST_ARITHMETIC16 1
	TEST_ARITHMETIC_MODRM8 %1
	TEST_ARITHMETIC_MODRM8_REV %1
	TEST_ARITHMETIC_MODRM16 %1
	TEST_ARITHMETIC_MODRM16_REV %1
	TEST_ARITHMETIC_IMM8 %1
	TEST_ARITHMETIC_IMM16 %1
%endmacro ; TEST_ARITHMETIC16

%macro TEST_ARITHMETIC_ONE_PARAM16 1
%1 ax
%1 cx
%1 dx
%1 bx
%1 sp
%1 bp
%1 si
%1 di
%endmacro ; TEST_ARITHMETIC_ONE_PARAM16

%macro TEST_ARITHMETIC_ONE_OPERAND 2
%1 %2
%endmacro ; TEST_ARITHMETIC_ONE_OPERAND

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
lea %1, word [SI]
lea %1, word [DI]
lea %1, word [0xffff]
lea %1, word [0x0001]
lea %1, word [BX]

; Test ModRM Mod==1
lea %1, word [BX + SI + 1]
lea %1, word [BX + SI + 0xff]
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
; one byte form
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
TEST_ARITHMETIC_MODRM8 mov
TEST_ARITHMETIC_MODRM16 mov

mov al, byte [1]
mov al, byte [0xffff]

mov ax, word [1]
mov ax, word [0xffff]

mov byte [1], al
mov byte [0xffff], al

mov word [1], ax
mov word [0xffff], ax

movsb
movsw
cmpsb
cmpsw







