BITS ARCH

%macro TEST_ARITHMETIC_RM8_MEMORY 2
%if ARCH <> 64
	; Test ModRM Mod==0
	%1 byte [BX + SI], %2
	%1 byte [BX + DI], %2
	%1 byte [BP + SI], %2
	%1 byte [BP + DI], %2
	%1 byte [SI], %2
	%1 byte [DI], %2
	%1 byte [0xffff], %2
	%1 byte [0x0000], %2
	%1 byte [0x0001], %2
	%1 byte [BX], %2

	; Test ModRM Mod==1
	%1 byte [BX + SI + 1], %2
	%1 byte [BX + SI + 0xff], %2
	%1 byte [BX + DI + 1], %2
	%1 byte [BX + DI + 0xff], %2
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
	%1 byte [BX + DI + 0xffff], %2
	%1 byte [BP + SI + 0xffff], %2
	%1 byte [BP + DI + 0xffff], %2
	%1 byte [SI + 0xffff], %2
	%1 byte [DI + 0xffff], %2
	%1 byte [BP + 0xffff], %2
	%1 byte [BX + 0xffff], %2
%endif ; ARCH <> 64
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
%if ARCH <> 64
	; Test ModRM Mod==0
	%1 %2, byte [BX + SI]
	%1 %2, byte [BX + DI]
	%1 %2, byte [BP + SI]
	%1 %2, byte [BP + DI]
	%1 %2, byte [SI]
	%1 %2, byte [DI]
	%1 %2, byte [0xffff]
	%1 %2, byte [0x0001]
	%1 %2, byte [BX]

	; Test ModRM Mod==1
	%1 %2, byte [BX + SI + 1]
	%1 %2, byte [BX + SI + 0xff]
	%1 %2, byte [BX + DI + 1]
	%1 %2, byte [BX + DI + 0xff]
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
	%1 %2, byte [BX + DI + 0xffff]
	%1 %2, byte [BP + SI + 0xffff]
	%1 %2, byte [BP + DI + 0xffff]
	%1 %2, byte [SI + 0xffff]
	%1 %2, byte [DI + 0xffff]
	%1 %2, byte [BP + 0xffff]
	%1 %2, byte [BX + 0xffff]
%endif ; ARCH <> 64
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
%endmacro ; TEST_ARITHMETIC_RM8_REV

%macro TEST_ARITHMETIC_RM16_MEMORY 2
%if ARCH <> 64
	; Test ModRM Mod==0
	%1 word [BX + SI], %2
	%1 word [BX + DI], %2
	%1 word [BP + SI], %2
	%1 word [BP + DI], %2
	%1 word [SI], %2
	%1 word [DI], %2
	%1 word [0xffff], %2
	%1 word [0x0001], %2
	%1 word [BX], %2

	; Test ModRM Mod==1
	%1 word [BX + SI + 1], %2
	%1 word [BX + SI + 0xff], %2
	%1 word [BX + DI + 1], %2
	%1 word [BX + DI + 0xff], %2
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
	%1 word [BX + DI + 0xffff], %2
	%1 word [BP + SI + 0xffff], %2
	%1 word [BP + DI + 0xffff], %2
	%1 word [SI + 0xffff], %2
	%1 word [DI + 0xffff], %2
	%1 word [BP + 0xffff], %2
	%1 word [BX + 0xffff], %2
%endif ; ARCH <> 64
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

%macro TEST_ARITHMETIC_RM16_MEMORY_REV 2
%if ARCH <> 64
	; Test ModRM Mod==0
	%1 %2, word [BX + SI]
	%1 %2, word [BX + DI]
	%1 %2, word [BP + SI]
	%1 %2, word [BP + DI]
	%1 %2, word [SI]
	%1 %2, word [DI]
	%1 %2, word [0xffff]
	%1 %2, word [0x0001]
	%1 %2, word [BX]

	; Test ModRM Mod==1
	%1 %2, word [BX + SI + 1]
	%1 %2, word [BX + SI + 0xff]
	%1 %2, word [BX + DI + 1]
	%1 %2, word [BX + DI + 0xff]
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
	%1 %2, word [BX + DI + 0xffff]
	%1 %2, word [BP + SI + 0xffff]
	%1 %2, word [BP + DI + 0xffff]
	%1 %2, word [SI + 0xffff]
	%1 %2, word [DI + 0xffff]
	%1 %2, word [BP + 0xffff]
	%1 %2, word [BX + 0xffff]
%endif ; ARCH <> 64
%endmacro ; TEST_ARITHMETIC_RM16_MEMORY_REV

%macro TEST_ARITHMETIC_RM16_REV 2
	TEST_ARITHMETIC_RM16_MEMORY_REV %1, %2
	; Test ModRM Mod==3
	%1 %2, AX
	%1 %2, CX
	%1 %2, DX
	%1 %2, BX
	%1 %2, SP
	%1 %2, BP
	%1 %2, SI
	%1 %2, DI
%endmacro ; TEST_ARITHMETIC_RM16_REV

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

%macro TEST_ARITHMETIC_ONE_PARAM32 1
%if ARCH <> 64
%1 eax
%1 ecx
%1 edx
%1 ebx
%1 esp
%1 ebp
%1 esi
%1 edi
%endif ; ARCH <> 64
%endmacro ; TEST_ARITHMETIC_ONE_PARAM32

%macro TEST_ARITHMETIC_ONE_PARAM64 1
%if ARCH == 64
%1 rax
%1 rcx
%1 rdx
%1 rbx
%1 rsp
%1 rbp
%1 rsi
%1 rdi
%endif ; ARCH <> 64
%endmacro ; TEST_ARITHMETIC_ONE_PARAM64


%macro TEST_ARITHMETIC_ONE_OPERAND 2
%1 %2
%endmacro ; TEST_ARITHMETIC_ONE_OPERAND

%macro TEST_ARITHMETIC_RM32_MEMORY 2
	; Test ModRM Mod==0
	%1 dword [BX + SI], %2
	%1 dword [BX + DI], %2
	%1 dword [BP + SI], %2
	%1 dword [BP + DI], %2
	%1 dword [SI], %2
	%1 dword [DI], %2
	%1 dword [0xffff], %2
	%1 dword [0x0001], %2
	%1 dword [BX], %2

	; Test ModRM Mod==1
	%1 dword [BX + SI + 1], %2
	%1 dword [BX + SI + 0xff], %2
	%1 dword [BX + DI + 1], %2
	%1 dword [BX + DI + 0xff], %2
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
	%1 dword [BX + DI + 0xffff], %2
	%1 dword [BP + SI + 0xffff], %2
	%1 dword [BP + DI + 0xffff], %2
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
	%1 qword [BP + DI], %2
	%1 qword [SI], %2
	%1 qword [DI], %2
	%1 qword [0xffff], %2
	%1 qword [0x0001], %2
	%1 qword [BX], %2

	; Test ModRM Mod==1
	%1 qword [BX + SI + 1], %2
	%1 qword [BX + SI + 0xff], %2
	%1 qword [BX + DI + 1], %2
	%1 qword [BX + DI + 0xff], %2
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
	%1 qword [BX + DI + 0xffff], %2
	%1 qword [BP + SI + 0xffff], %2
	%1 qword [BP + DI + 0xffff], %2
	%1 qword [SI + 0xffff], %2
	%1 qword [DI + 0xffff], %2
	%1 qword [BP + 0xffff], %2
	%1 qword [BX + 0xffff], %2
%endmacro ; TEST_ARITHMETIC_RM64_MEMORY

%macro TEST_ARITHMETIC_RM128_MEMORY 2
	; Test ModRM Mod==0
	%1 oword [BX + SI], %2
	%1 oword [BX + DI], %2
	%1 oword [BP + SI], %2
	%1 oword [BP + DI], %2
	%1 oword [SI], %2
	%1 oword [DI], %2
	%1 oword [0xffff], %2
	%1 oword [0x0001], %2
	%1 oword [BX], %2

	; Test ModRM Mod==1
	%1 oword [BX + SI + 1], %2
	%1 oword [BX + SI + 0xff], %2
	%1 oword [BX + DI + 1], %2
	%1 oword [BX + DI + 0xff], %2
	%1 oword [SI + 1], %2
	%1 oword [SI + 0xff], %2
	%1 oword [DI + 1], %2
	%1 oword [DI + 0xff], %2
	%1 oword [BP + 1], %2
	%1 oword [BP + 0xff], %2
	%1 oword [BX + 1], %2
	%1 oword [BX + 0xff], %2

	; Test ModRM Mod==2
	%1 oword [BX + SI + 0xffff], %2
	%1 oword [BX + DI + 0xffff], %2
	%1 oword [BP + SI + 0xffff], %2
	%1 oword [BP + DI + 0xffff], %2
	%1 oword [SI + 0xffff], %2
	%1 oword [DI + 0xffff], %2
	%1 oword [BP + 0xffff], %2
	%1 oword [BX + 0xffff], %2
%endmacro ; TEST_ARITHMETIC_RM128_MEMORY

%macro TEST_ARITHMETIC_RM128_MEMORY_REV 2
	; Test ModRM Mod==0
	%1 %2, oword [BX + SI]
	%1 %2, oword [BX + DI]
	%1 %2, oword [BP + SI]
	%1 %2, oword [BP + DI]
	%1 %2, oword [SI]
	%1 %2, oword [DI]
	%1 %2, oword [0xffff]
	%1 %2, oword [0x0001]
	%1 %2, oword [BX]

	; Test ModRM Mod==1
	%1 %2, oword [BX + SI + 1]
	%1 %2, oword [BX + SI + 0xff]
	%1 %2, oword [BX + DI + 1]
	%1 %2, oword [BX + DI + 0xff]
	%1 %2, oword [SI + 1]
	%1 %2, oword [SI + 0xff]
	%1 %2, oword [DI + 1]
	%1 %2, oword [DI + 0xff]
	%1 %2, oword [BP + 1]
	%1 %2, oword [BP + 0xff]
	%1 %2, oword [BX + 1]
	%1 %2, oword [BX + 0xff]

	; Test ModRM Mod==2
	%1 %2, oword [BX + SI + 0xffff]
	%1 %2, oword [BX + DI + 0xffff]
	%1 %2, oword [BP + SI + 0xffff]
	%1 %2, oword [BP + DI + 0xffff]
	%1 %2, oword [SI + 0xffff]
	%1 %2, oword [DI + 0xffff]
	%1 %2, oword [BP + 0xffff]
	%1 %2, oword [BX + 0xffff]
%endmacro ; TEST_ARITHMETIC_RM128_MEMORY_REV

%macro TestModRmMemory 2
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
	%1 [BX + DI + 1], %2
	%1 [BX + DI + 0xff], %2
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
	%1 [BX + DI + 0xffff], %2
	%1 [BP + SI + 0xffff], %2
	%1 [BP + DI + 0xffff], %2
	%1 [SI + 0xffff], %2
	%1 [DI + 0xffff], %2
	%1 [BP + 0xffff], %2
	%1 [BX + 0xffff], %2
%endmacro ; TestModRmMemory

%macro TestMmxRow 2
TestModRmMemory %1, %2
%1 mm0, %2
%1 mm1, %2
%1 mm2, %2
%1 mm3, %2
%1 mm4, %2
%1 mm5, %2
%1 mm6, %2
%1 mm7, %2
%endmacro

%macro TestMmx 1
TestMmxRow %1, mm0
TestMmxRow %1, mm1
TestMmxRow %1, mm2
TestMmxRow %1, mm3
TestMmxRow %1, mm4
TestMmxRow %1, mm5
TestMmxRow %1, mm6
TestMmxRow %1, mm7
%endmacro ; TestMmx

%macro TestModRmMemoryRev 2
	; Test ModRM Mod==0
	%1 %2, [BX + SI]
	%1 %2, [BX + DI]
	%1 %2, [BP + SI]
	%1 %2, [SI]
	%1 %2, [DI]
	%1 %2, [0xffff]
	%1 %2, [0x0001]
	%1 %2, [BX]

	; Test ModRM Mod==1
	%1 %2, [BX + SI + 1]
	%1 %2, [BX + SI + 0xff]
	%1 %2, [BX + DI + 1]
	%1 %2, [BX + DI + 0xff]
	%1 %2, [SI + 1]
	%1 %2, [SI + 0xff]
	%1 %2, [DI + 1]
	%1 %2, [DI + 0xff]
	%1 %2, [BP + 1]
	%1 %2, [BP + 0xff]
	%1 %2, [BX + 1]
	%1 %2, [BX + 0xff]

	; Test ModRM Mod==2
	%1 %2, [BX + SI + 0xffff]
	%1 %2, [BX + DI + 0xffff]
	%1 %2, [BP + SI + 0xffff]
	%1 %2, [BP + DI + 0xffff]
	%1 %2, [SI + 0xffff]
	%1 %2, [DI + 0xffff]
	%1 %2, [BP + 0xffff]
	%1 %2, [BX + 0xffff]
	%1 %2, [BX + 1]
%endmacro ; TestModRmMemoryRev

%macro MmxModRmRevRow 2
TestModRmMemoryRev %1, %2
%1 mm0, %2
%1 mm1, %2
%1 mm2, %2
%1 mm3, %2
%1 mm4, %2
%1 mm5, %2
%1 mm6, %2
%1 mm7, %2
%endmacro ; MmxModRmRevRow

%macro TestMmxRev 1
MmxModRmRevRow %1, mm0
MmxModRmRevRow %1, mm1
MmxModRmRevRow %1, mm2
MmxModRmRevRow %1, mm3
MmxModRmRevRow %1, mm4
MmxModRmRevRow %1, mm5
MmxModRmRevRow %1, mm6
MmxModRmRevRow %1, mm7
%endmacro ; TestMmxRev

%macro SimdModRmRow 2
; TestModRmMemory %1, %2
%1 %2, xmm0
%1 %2, xmm1
%1 %2, xmm2
%1 %2, xmm3
%1 %2, xmm4
%1 %2, xmm5
%1 %2, xmm6
%1 %2, xmm7
; %1 %2, xmm8
; %1 %2, xmm9
; %1 %2, xmm10
; %1 %2, xmm11
; %1 %2, xmm12
; %1 %2, xmm13
; %1 %2, xmm14
; %1 %2, xmm15
%endmacro ; SimdModRmRow

%macro TestSimd 1
SimdModRmRow %1, xmm0
SimdModRmRow %1, xmm1
SimdModRmRow %1, xmm2
SimdModRmRow %1, xmm3
SimdModRmRow %1, xmm4
SimdModRmRow %1, xmm5
SimdModRmRow %1, xmm6
SimdModRmRow %1, xmm7
%if ARCH == 64
SimdModRmRow %1, xmm8
SimdModRmRow %1, xmm9
SimdModRmRow %1, xmm10
SimdModRmRow %1, xmm11
SimdModRmRow %1, xmm12
SimdModRmRow %1, xmm13
SimdModRmRow %1, xmm14
SimdModRmRow %1, xmm15
%endif ; ARCH == 64
%endmacro ; TestSimd

%macro SimdModRmRevRow 2
TestModRmMemoryRev %1, %2
%1 xmm0, %2
%1 xmm1, %2
%1 xmm2, %2
%1 xmm3, %2
%1 xmm4, %2
%1 xmm5, %2
%1 xmm6, %2
%1 xmm7, %2
%if ARCH == 64
%1 xmm8, %2
%1 xmm9, %2
%1 xmm10, %2
%1 xmm11, %2
%1 xmm12, %2
%1 xmm13, %2
%1 xmm14, %2
%1 xmm15, %2
%endif ; ARCH == 64
%endmacro ; TestModRmMemoryRev

%macro TestSimdRev 1
SimdModRmRevRow %1, xmm0
SimdModRmRevRow %1, xmm1
SimdModRmRevRow %1, xmm2
SimdModRmRevRow %1, xmm3
SimdModRmRevRow %1, xmm4
SimdModRmRevRow %1, xmm5
SimdModRmRevRow %1, xmm6
SimdModRmRevRow %1, xmm7
%if ARCH == 64
SimdModRmRevRow %1, xmm8
SimdModRmRevRow %1, xmm9
SimdModRmRevRow %1, xmm10
SimdModRmRevRow %1, xmm11
SimdModRmRevRow %1, xmm12
SimdModRmRevRow %1, xmm13
SimdModRmRevRow %1, xmm14
SimdModRmRevRow %1, xmm15
%endif ; ARCH == 64
%endmacro ; TestSimdRev

%macro TestMovnt 1
TestModRmMemory movntps, xmm0
TestModRmMemory movntps, xmm1
TestModRmMemory movntps, xmm2
TestModRmMemory movntps, xmm3
TestModRmMemory movntps, xmm4
TestModRmMemory movntps, xmm5
TestModRmMemory movntps, xmm6
TestModRmMemory movntps, xmm7
%if ARCH == 64
TestModRmMemory movntps, xmm8
TestModRmMemory movntps, xmm9
TestModRmMemory movntps, xmm10
TestModRmMemory movntps, xmm11
TestModRmMemory movntps, xmm12
TestModRmMemory movntps, xmm13
TestModRmMemory movntps, xmm14
TestModRmMemory movntps, xmm15
%endif ; ARCH == 64
%endmacro ; TestMovnt

%macro TestModRmMemoryImm 3
	; Test ModRM Mod==0
	%1 %2, [BX + SI], %3
	%1 %2, [BX + DI], %3
	%1 %2, [BP + SI], %3
	%1 %2, [SI], %3
	%1 %2, [DI], %3
	%1 %2, [0xffff], %3
	%1 %2, [0x0001], %3
	%1 %2, [BX], %3

	; Test ModRM Mod==1
	%1 %2, [BX + SI + 1], %3
	%1 %2, [BX + SI + 0xff], %3
	%1 %2, [BX + DI + 1], %3
	%1 %2, [BX + DI + 0xff], %3
	%1 %2, [SI + 1], %3
	%1 %2, [SI + 0xff], %3
	%1 %2, [DI + 1], %3
	%1 %2, [DI + 0xff], %3
	%1 %2, [BP + 1], %3
	%1 %2, [BP + 0xff], %3
	%1 %2, [BX + 1], %3
	%1 %2, [BX + 0xff], %3

	; Test ModRM Mod==2
	%1 %2, [BX + SI + 0xffff], %3
	%1 %2, [BX + DI + 0xffff], %3
	%1 %2, [BP + SI + 0xffff], %3
	%1 %2, [BP + DI + 0xffff], %3
	%1 %2, [SI + 0xffff], %3
	%1 %2, [DI + 0xffff], %3
	%1 %2, [BP + 0xffff], %3
	%1 %2, [BX + 0xffff], %3
%endmacro ; TestModRmMemoryImm

%macro TestModRmMemoryImmRev 3
	; Test ModRM Mod==0
	%1 [BX + SI], %2, %3
	%1 [BX + DI], %2, %3
	%1 [BP + SI], %2, %3
	%1 [SI], %2, %3
	%1 [DI], %2, %3
	%1 [0xffff], %2, %3
	%1 [0x0001], %2, %3
	%1 [BX], %2, %3

	; Test ModRM Mod==1
	%1 [BX + SI + 1], %2, %3
	%1 [BX + SI + 0xff], %2, %3
	%1 [BX + DI + 1], %2, %3
	%1 [BX + DI + 0xff], %2, %3
	%1 [SI + 1], %2, %3
	%1 [SI + 0xff], %2, %3
	%1 [DI + 1], %2, %3
	%1 [DI + 0xff], %2, %3
	%1 [BP + 1], %2, %3
	%1 [BP + 0xff], %2, %3
	%1 [BX + 1], %2, %3
	%1 [BX + 0xff], %2, %3

	; Test ModRM Mod==2
	%1 [BX + SI + 0xffff], %2, %3
	%1 [BX + DI + 0xffff], %2, %3
	%1 [BP + SI + 0xffff], %2, %3
	%1 [BP + DI + 0xffff], %2, %3
	%1 [SI + 0xffff], %2, %3
	%1 [DI + 0xffff], %2, %3
	%1 [BP + 0xffff], %2, %3
	%1 [BX + 0xffff], %2, %3
%endmacro ; TestModRmMemoryImmRev

%macro TestMmxRowImm 2
TestModRmMemoryImm %1, %2, 0
TestModRmMemoryImm %1, %2, 1
TestModRmMemoryImm %1, %2, 0xff
%1 mm0, %2, 0
%1 mm0, %2, 1
%1 mm0, %2, 0xff
%1 mm1, %2, 0
%1 mm1, %2, 1
%1 mm1, %2, 0xff
%1 mm2, %2, 0
%1 mm2, %2, 1
%1 mm2, %2, 0xff
%1 mm3, %2, 0
%1 mm3, %2, 1
%1 mm3, %2, 0xff
%1 mm4, %2, 0
%1 mm4, %2, 1
%1 mm4, %2, 0xff
%1 mm5, %2, 0
%1 mm5, %2, 1
%1 mm5, %2, 0xff
%1 mm6, %2, 0
%1 mm6, %2, 1
%1 mm6, %2, 0xff
%1 mm7, %2, 0
%1 mm7, %2, 1
%1 mm7, %2, 0xff
%endmacro ; TestMmxRowImm

%macro TestMmxImm 1
TestMmxRowImm %1, mm0
TestMmxRowImm %1, mm1
TestMmxRowImm %1, mm2
TestMmxRowImm %1, mm3
TestMmxRowImm %1, mm4
TestMmxRowImm %1, mm5
TestMmxRowImm %1, mm6
TestMmxRowImm %1, mm7
%endmacro ; TestMmxImm

%macro TestSimdRowImmThreeOperand 2
TestModRmMemoryImm %1, %2, 0
TestModRmMemoryImm %1, %2, 1
TestModRmMemoryImm %1, %2, 0xff
%1 xmm0, %2, 0
%1 xmm0, %2, 1
%1 xmm0, %2, 0xff
%1 xmm1, %2, 0
%1 xmm1, %2, 1
%1 xmm1, %2, 0xff
%1 xmm2, %2, 0
%1 xmm2, %2, 1
%1 xmm2, %2, 0xff
%1 xmm3, %2, 0
%1 xmm3, %2, 1
%1 xmm3, %2, 0xff
%1 xmm4, %2, 0
%1 xmm4, %2, 1
%1 xmm4, %2, 0xff
%1 xmm5, %2, 0
%1 xmm5, %2, 1
%1 xmm5, %2, 0xff
%1 xmm6, %2, 0
%1 xmm6, %2, 1
%1 xmm6, %2, 0xff
%1 xmm7, %2, 0
%1 xmm7, %2, 1
%1 xmm7, %2, 0xff
%if ARCH == 64
%1 xmm8, %2, 0
%1 xmm8, %2, 1
%1 xmm8, %2, 0xff
%1 xmm9, %2, 0
%1 xmm9, %2, 1
%1 xmm9, %2, 0xff
%1 xmm10, %2, 0
%1 xmm10, %2, 1
%1 xmm10, %2, 0xff
%1 xmm11, %2, 0
%1 xmm11, %2, 1
%1 xmm11, %2, 0xff
%1 xmm12, %2, 0
%1 xmm12, %2, 1
%1 xmm12, %2, 0xff
%1 xmm13, %2, 0
%1 xmm13, %2, 1
%1 xmm13, %2, 0xff
%1 xmm14, %2, 0
%1 xmm14, %2, 1
%1 xmm14, %2, 0xff
%1 xmm15, %2, 0
%1 xmm15, %2, 1
%1 xmm15, %2, 0xff
%endif ; ARCH == 64
%endmacro ; TestSimdRowImmThreeOperand

%macro TestSimdImmThreeOperand 1
TestSimdRowImmThreeOperand %1, xmm0
TestSimdRowImmThreeOperand %1, xmm0
TestSimdRowImmThreeOperand %1, xmm1
TestSimdRowImmThreeOperand %1, xmm1
TestSimdRowImmThreeOperand %1, xmm2
TestSimdRowImmThreeOperand %1, xmm2
TestSimdRowImmThreeOperand %1, xmm3
TestSimdRowImmThreeOperand %1, xmm3
TestSimdRowImmThreeOperand %1, xmm4
TestSimdRowImmThreeOperand %1, xmm4
TestSimdRowImmThreeOperand %1, xmm5
TestSimdRowImmThreeOperand %1, xmm5
TestSimdRowImmThreeOperand %1, xmm6
TestSimdRowImmThreeOperand %1, xmm6
TestSimdRowImmThreeOperand %1, xmm7
TestSimdRowImmThreeOperand %1, xmm7
%if ARCH == 64
TestSimdRowImmThreeOperand %1, xmm8
TestSimdRowImmThreeOperand %1, xmm8
TestSimdRowImmThreeOperand %1, xmm9
TestSimdRowImmThreeOperand %1, xmm9
TestSimdRowImmThreeOperand %1, xmm10
TestSimdRowImmThreeOperand %1, xmm10
TestSimdRowImmThreeOperand %1, xmm11
TestSimdRowImmThreeOperand %1, xmm11
TestSimdRowImmThreeOperand %1, xmm12
TestSimdRowImmThreeOperand %1, xmm12
TestSimdRowImmThreeOperand %1, xmm13
TestSimdRowImmThreeOperand %1, xmm13
TestSimdRowImmThreeOperand %1, xmm14
TestSimdRowImmThreeOperand %1, xmm14
TestSimdRowImmThreeOperand %1, xmm15
TestSimdRowImmThreeOperand %1, xmm15
TestSimdRowImmThreeOperand %1, xmm16
TestSimdRowImmThreeOperand %1, xmm16
%endif ; ARCH == 64
%endmacro ; TestSimdImmThreeOperand

%macro TestMmxGprImm 2
%1 mm0, %2, 0
%1 mm0, %2, 1
%1 mm0, %2, 0xff
%1 mm1, %2, 0
%1 mm1, %2, 1
%1 mm1, %2, 0xff
%1 mm2, %2, 0
%1 mm2, %2, 1
%1 mm2, %2, 0xff
%1 mm3, %2, 0
%1 mm3, %2, 1
%1 mm3, %2, 0xff
%1 mm4, %2, 0
%1 mm4, %2, 1
%1 mm4, %2, 0xff
%1 mm5, %2, 0
%1 mm5, %2, 1
%1 mm5, %2, 0xff
%1 mm6, %2, 0
%1 mm6, %2, 1
%1 mm6, %2, 0xff
%1 mm7, %2, 0
%1 mm7, %2, 1
%1 mm7, %2, 0xff
%endmacro ; TestMmxGprImm

%macro TestSimdGprImm 2
%1 xmm0, %2, 0
%1 xmm0, %2, 1
%1 xmm0, %2, 0xff
%1 xmm1, %2, 0
%1 xmm1, %2, 1
%1 xmm1, %2, 0xff
%1 xmm2, %2, 0
%1 xmm2, %2, 1
%1 xmm2, %2, 0xff
%1 xmm3, %2, 0
%1 xmm3, %2, 1
%1 xmm3, %2, 0xff
%1 xmm4, %2, 0
%1 xmm4, %2, 1
%1 xmm4, %2, 0xff
%1 xmm5, %2, 0
%1 xmm5, %2, 1
%1 xmm5, %2, 0xff
%1 xmm6, %2, 0
%1 xmm6, %2, 1
%1 xmm6, %2, 0xff
%1 xmm7, %2, 0
%1 xmm7, %2, 1
%1 xmm7, %2, 0xff
%if ARCH == 64
%1 xmm8, %2, 0
%1 xmm8, %2, 1
%1 xmm8, %2, 0xff
%1 xmm9, %2, 0
%1 xmm9, %2, 1
%1 xmm9, %2, 0xff
%1 xmm10, %2, 0
%1 xmm10, %2, 1
%1 xmm10, %2, 0xff
%1 xmm11, %2, 0
%1 xmm11, %2, 1
%1 xmm11, %2, 0xff
%1 xmm12, %2, 0
%1 xmm12, %2, 1
%1 xmm12, %2, 0xff
%1 xmm13, %2, 0
%1 xmm13, %2, 1
%1 xmm13, %2, 0xff
%1 xmm14, %2, 0
%1 xmm14, %2, 1
%1 xmm14, %2, 0xff
%1 xmm15, %2, 0
%1 xmm15, %2, 1
%1 xmm15, %2, 0xff
%endif ; ARCH == 64
%endmacro ; TestSimdGprImm

%macro TestGprMmxImm 2
%1 %2, mm0, 0
%1 %2, mm0, 1
%1 %2, mm0, 0xff
%1 %2, mm1, 0
%1 %2, mm1, 1
%1 %2, mm1, 0xff
%1 %2, mm2, 0
%1 %2, mm2, 1
%1 %2, mm2, 0xff
%1 %2, mm3, 0
%1 %2, mm3, 1
%1 %2, mm3, 0xff
%1 %2, mm4, 0
%1 %2, mm4, 1
%1 %2, mm4, 0xff
%1 %2, mm5, 0
%1 %2, mm5, 1
%1 %2, mm5, 0xff
%1 %2, mm6, 0
%1 %2, mm6, 1
%1 %2, mm6, 0xff
%1 %2, mm7, 0
%1 %2, mm7, 1
%1 %2, mm7, 0xff
%endmacro ; TestGprMmxImm

%macro TestGprSimdImm 2
%1 %2, xmm0, 0
%1 %2, xmm0, 1
%1 %2, xmm0, 0xff
%1 %2, xmm1, 0
%1 %2, xmm1, 1
%1 %2, xmm1, 0xff
%1 %2, xmm2, 0
%1 %2, xmm2, 1
%1 %2, xmm2, 0xff
%1 %2, xmm3, 0
%1 %2, xmm3, 1
%1 %2, xmm3, 0xff
%1 %2, xmm4, 0
%1 %2, xmm4, 1
%1 %2, xmm4, 0xff
%1 %2, xmm5, 0
%1 %2, xmm5, 1
%1 %2, xmm5, 0xff
%1 %2, xmm6, 0
%1 %2, xmm6, 1
%1 %2, xmm6, 0xff
%1 %2, xmm7, 0
%1 %2, xmm7, 1
%1 %2, xmm7, 0xff
%if ARCH == 64
%1 %2, xmm8, 0
%1 %2, xmm8, 1
%1 %2, xmm8, 0xff
%1 %2, xmm9, 0
%1 %2, xmm9, 1
%1 %2, xmm9, 0xff
%1 %2, xmm10, 0
%1 %2, xmm10, 1
%1 %2, xmm10, 0xff
%1 %2, xmm11, 0
%1 %2, xmm11, 1
%1 %2, xmm11, 0xff
%1 %2, xmm12, 0
%1 %2, xmm12, 1
%1 %2, xmm12, 0xff
%1 %2, xmm13, 0
%1 %2, xmm13, 1
%1 %2, xmm13, 0xff
%1 %2, xmm14, 0
%1 %2, xmm14, 1
%1 %2, xmm14, 0xff
%1 %2, xmm15, 0
%1 %2, xmm15, 1
%1 %2, xmm15, 0xff
%endif ; ARCH == 64
%endmacro ; TestGprSimdImm

%macro TestSimdRowImm 2
TestModRmMemoryImm %1, %2, 0
TestModRmMemoryImm %1, %2, 1
TestModRmMemoryImm %1, %2, 0xff
%1 xmm0, %2, 0
%1 xmm0, %2, 1
%1 xmm0, %2, 0xff
%1 xmm1, %2, 0
%1 xmm1, %2, 1
%1 xmm1, %2, 0xff
%1 xmm2, %2, 0
%1 xmm2, %2, 1
%1 xmm2, %2, 0xff
%1 xmm3, %2, 0
%1 xmm3, %2, 1
%1 xmm3, %2, 0xff
%1 xmm4, %2, 0
%1 xmm4, %2, 1
%1 xmm4, %2, 0xff
%1 xmm5, %2, 0
%1 xmm5, %2, 1
%1 xmm5, %2, 0xff
%1 xmm6, %2, 0
%1 xmm6, %2, 1
%1 xmm6, %2, 0xff
%1 xmm7, %2, 0
%1 xmm7, %2, 1
%1 xmm7, %2, 0xff
%if ARCH == 64
%1 xmm8, %2, 0
%1 xmm8, %2, 1
%1 xmm8, %2, 0xff
%1 xmm9, %2, 0
%1 xmm9, %2, 1
%1 xmm9, %2, 0xff
%1 xmm10, %2, 0
%1 xmm10, %2, 1
%1 xmm10, %2, 0xff
%1 xmm11, %2, 0
%1 xmm11, %2, 1
%1 xmm11, %2, 0xff
%1 xmm12, %2, 0
%1 xmm12, %2, 1
%1 xmm12, %2, 0xff
%1 xmm13, %2, 0
%1 xmm13, %2, 1
%1 xmm13, %2, 0xff
%1 xmm14, %2, 0
%1 xmm14, %2, 1
%1 xmm14, %2, 0xff
%1 xmm15, %2, 0
%1 xmm15, %2, 1
%1 xmm15, %2, 0xff
%endif ; ARCH == 64
%endmacro ; TestSimdRowImm

%macro TestSimdImm 1
TestSimdRowImm %1, xmm0
TestSimdRowImm %1, xmm0
TestSimdRowImm %1, xmm1
TestSimdRowImm %1, xmm1
TestSimdRowImm %1, xmm2
TestSimdRowImm %1, xmm2
TestSimdRowImm %1, xmm3
TestSimdRowImm %1, xmm3
TestSimdRowImm %1, xmm4
TestSimdRowImm %1, xmm4
TestSimdRowImm %1, xmm5
TestSimdRowImm %1, xmm5
TestSimdRowImm %1, xmm6
TestSimdRowImm %1, xmm6
TestSimdRowImm %1, xmm7
TestSimdRowImm %1, xmm7
%if ARCH == 64
TestSimdRowImm %1, xmm8
TestSimdRowImm %1, xmm8
TestSimdRowImm %1, xmm9
TestSimdRowImm %1, xmm9
TestSimdRowImm %1, xmm10
TestSimdRowImm %1, xmm10
TestSimdRowImm %1, xmm11
TestSimdRowImm %1, xmm11
TestSimdRowImm %1, xmm12
TestSimdRowImm %1, xmm12
TestSimdRowImm %1, xmm13
TestSimdRowImm %1, xmm13
TestSimdRowImm %1, xmm14
TestSimdRowImm %1, xmm14
TestSimdRowImm %1, xmm15
TestSimdRowImm %1, xmm15
%endif ; ARCH == 64
%endmacro ; TestSimdImm

%macro TestSimdMmxRow 2
TestModRmMemoryRev %1, %2
%1 %2, mm0
%1 %2, mm1
%1 %2, mm2
%1 %2, mm3
%1 %2, mm4
%1 %2, mm5
%1 %2, mm6
%1 %2, mm7
%endmacro ; TestSimdMmxRow

%macro TestSimdMmx 1
TestSimdMmxRow %1, xmm0
TestSimdMmxRow %1, xmm1
TestSimdMmxRow %1, xmm2
TestSimdMmxRow %1, xmm3
TestSimdMmxRow %1, xmm4
TestSimdMmxRow %1, xmm5
TestSimdMmxRow %1, xmm6
TestSimdMmxRow %1, xmm7
%if ARCH == 64
TestSimdMmxRow %1, xmm8
TestSimdMmxRow %1, xmm9
TestSimdMmxRow %1, xmm10
TestSimdMmxRow %1, xmm11
TestSimdMmxRow %1, xmm12
TestSimdMmxRow %1, xmm13
TestSimdMmxRow %1, xmm14
TestSimdMmxRow %1, xmm15
%endif ; ARCH == 64
%endmacro ; TestSimdMmx

%macro TestMmxSimdRow 2
TestModRmMemoryRev %1, %2
%1 %2, xmm0
%1 %2, xmm1
%1 %2, xmm2
%1 %2, xmm3
%1 %2, xmm4
%1 %2, xmm5
%1 %2, xmm6
%1 %2, xmm7
%if ARCH == 64
%1, xmm8, %2
%1, xmm9, %2
%1, xmm10, %2
%1, xmm11, %2
%1, xmm12, %2
%1, xmm13, %2
%1, xmm14, %2
%1, xmm15, %2
%endif ; ARCH == 64
%endmacro ; TestMmxSimdRow

%macro TestMmxSimd 1
TestMmxSimdRow %1, mm0
TestMmxSimdRow %1, mm1
TestMmxSimdRow %1, mm2
TestMmxSimdRow %1, mm3
TestMmxSimdRow %1, mm4
TestMmxSimdRow %1, mm5
TestMmxSimdRow %1, mm6
TestMmxSimdRow %1, mm7
%endmacro ; TestMmxSimd
