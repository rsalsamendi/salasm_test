%macro TEST_ARITHMETIC_RM8 2
	; Test ModRM Mod==0
	%1 [BX + SI], %2
	%1 [BX + DI], %2
	%1 [BP + SI], %2
	%1 [SI], %2
	%1 [DI], %2
	%1 [0xffff], %2
	%1 [0x0001], %2
	%1 [BX], %2

	; Test ModRM Mod==1
	%1 [BX + SI + 1], %2
	%1 [BX + SI + 0xff], %2
	%1 [SI + 1], %2
	%1 [SI + 0xff], %2
	%1 [DI + 1], %2
	%1 [DI + 0xff], %2
	%1 [BP + 1], %2
	%1 [BP + 0xff], %2
	%1 [BX + 1], %2
	%1 [BX + 0xff], %2

	; Test ModRM Mod==2
	%1 [BX + SI + 0xffff], %2
	%1 [SI + 0xffff], %2
	%1 [DI + 0xffff], %2
	%1 [BP + 0xffff], %2
	%1 [BX + 0xffff], %2

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

%macro TEST_ARITHMETIC_RM16 2
	; Test ModRM Mod==0
	%1 [BX + SI], %2
	%1 [BX + DI], %2
	%1 [BP + SI], %2
	%1 [SI], %2
	%1 [DI], %2
	%1 [0xffff], %2
	%1 [0x0001], %2
	%1 [BX], %2

	; Test ModRM Mod==1
	%1 [BX + SI + 1], %2
	%1 [BX + SI + 0xff], %2
	%1 [SI + 1], %2
	%1 [SI + 0xff], %2
	%1 [DI + 1], %2
	%1 [DI + 0xff], %2
	%1 [BP + 1], %2
	%1 [BP + 0xff], %2
	%1 [BX + 1], %2
	%1 [BX + 0xff], %2

	; Test ModRM Mod==2
	%1 [BX + SI + 0xffff], %2
	%1 [SI + 0xffff], %2
	%1 [DI + 0xffff], %2
	%1 [BP + 0xffff], %2
	%1 [BX + 0xffff], %2

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
	TEST_ARITHMETIC_MODRM16 %1
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

; Row 8






