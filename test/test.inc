%macro TEST_ARITHMETIC_RM8_MEMORY 2
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
%endmacro ; TEST_ARITHMETIC_RM8_MEMORY

%macro TEST_ARITHMETIC_RM8 2
	TEST_ARITHMETIC_RM8_MEMORY %1, %2
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

%macro TEST_ARITHMETIC_RM8_REV_MEMORY 2
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
%endmacro ; TEST_ARITHMETIC_RM8_REV_MEMORY

%macro TEST_ARITHMETIC_RM8_REV 2
	TEST_ARITHMETIC_RM8_REV_MEMORY %1, %2
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

%macro TEST_ARITHMETIC_RM16_MEMORY 2
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
%endmacro ; TEST_ARITHMETIC_RM16_MEMORY

%macro TEST_ARITHMETIC_RM16 2
	TEST_ARITHMETIC_RM16_MEMORY %1, %2
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

%macro TEST_ARITHMETIC_RM32_MEMORY 2
	; Test ModRM Mod==0
	%1 dword [BX + SI], %2
	%1 dword [BX + DI], %2
	%1 dword [BP + SI], %2
	%1 dword [SI], %2
	%1 dword [DI], %2
	%1 dword [0xffff], %2
	%1 dword [0x0001], %2
	%1 dword [BX], %2

	; Test ModRM Mod==1
	%1 dword [BX + SI + 1], %2
	%1 dword [BX + SI + 0xff], %2
	%1 dword [SI + 1], %2
	%1 dword [SI + 0xff], %2
	%1 dword [DI + 1], %2
	%1 dword [DI + 0xff], %2
	%1 dword [BP + 1], %2
	%1 dword [BP + 0xff], %2
	%1 dword [BX + 1], %2
	%1 dword [BX + 0xff], %2

	; Test ModRM Mod==2
	%1 dword [BX + SI + 0xffff], %2
	%1 dword [SI + 0xffff], %2
	%1 dword [DI + 0xffff], %2
	%1 dword [BP + 0xffff], %2
	%1 dword [BX + 0xffff], %2
%endmacro ; TEST_ARITHMETIC_RM32_MEMORY

%macro TEST_ARITHMETIC_RM64_MEMORY 2
	; Test ModRM Mod==0
	%1 qword [BX + SI], %2
	%1 qword [BX + DI], %2
	%1 qword [BP + SI], %2
	%1 qword [SI], %2
	%1 qword [DI], %2
	%1 qword [0xffff], %2
	%1 qword [0x0001], %2
	%1 qword [BX], %2

	; Test ModRM Mod==1
	%1 qword [BX + SI + 1], %2
	%1 qword [BX + SI + 0xff], %2
	%1 qword [SI + 1], %2
	%1 qword [SI + 0xff], %2
	%1 qword [DI + 1], %2
	%1 qword [DI + 0xff], %2
	%1 qword [BP + 1], %2
	%1 qword [BP + 0xff], %2
	%1 qword [BX + 1], %2
	%1 qword [BX + 0xff], %2

	; Test ModRM Mod==2
	%1 qword [BX + SI + 0xffff], %2
	%1 qword [SI + 0xffff], %2
	%1 qword [DI + 0xffff], %2
	%1 qword [BP + 0xffff], %2
	%1 qword [BX + 0xffff], %2
%endmacro ; TEST_ARITHMETIC_RM64_MEMORY
