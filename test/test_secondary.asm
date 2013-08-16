%include "test.inc"
BITS 16

; Row 0

; Group 6
%macro MemAndReg16 1
%1 word [0xffff]
%1 ax
%1 cx
%1 dx
%1 bx
%endmacro ; MemAndReg16

MemAndReg16 sldt
MemAndReg16 str
MemAndReg16 lldt
MemAndReg16 ltr
MemAndReg16 verr
MemAndReg16 verw

; Group 7
sgdt [0xffff]
sidt [0xffff]
monitor
mwait
lgdt [0xffff]
xgetbv
xsetbv
lidt [0xffff]
vmrun
vmmcall
vmload
vmsave
stgi
clgi
skinit
invlpga
MemAndReg16 smsw
MemAndReg16 lmsw
invlpg byte [0xffff]
swapgs
rdtscp

TEST_ARITHMETIC_MODRM16_REV lar
TEST_ARITHMETIC_MODRM16_REV lsl

syscall
clts
sysret

invd
wbinvd
ud2
prefetch [0xffff]
prefetchw [0xffff]
femms

; TODO: 3dnow


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
%endmacro ; TestModRmMemory

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
	%1 %2, [SI + 0xffff]
	%1 %2, [DI + 0xffff]
	%1 %2, [BP + 0xffff]
	%1 %2, [BX + 0xffff]
%endmacro ; TestModRmMemoryRev

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
; %1 xmm8, %2
; %1 xmm9, %2
; %1 xmm10, %2
; %1 xmm11, %2
; %1 xmm12, %2
; %1 xmm13, %2
; %1 xmm14, %2
; %1 xmm15, %2
%endmacro ; TestModRmMemoryRev

%macro TestSimd 1
SimdModRmRow %1, xmm0
SimdModRmRow %1, xmm1
SimdModRmRow %1, xmm2
SimdModRmRow %1, xmm3
SimdModRmRow %1, xmm4
SimdModRmRow %1, xmm5
SimdModRmRow %1, xmm6
SimdModRmRow %1, xmm7
; SimdModRmRow %1, xmm8
; SimdModRmRow %1, xmm9
; SimdModRmRow %1, xmm10
; SimdModRmRow %1, xmm11
; SimdModRmRow %1, xmm12
; SimdModRmRow %1, xmm13
; SimdModRmRow %1, xmm14
; SimdModRmRow %1, xmm15
%endmacro ; TestSimd

%macro TestSimdRev 1
SimdModRmRevRow %1, xmm0
SimdModRmRevRow %1, xmm1
SimdModRmRevRow %1, xmm2
SimdModRmRevRow %1, xmm3
SimdModRmRevRow %1, xmm4
SimdModRmRevRow %1, xmm5
SimdModRmRevRow %1, xmm6
SimdModRmRevRow %1, xmm7
; SimdModRmRevRow %1, xmm8
; SimdModRmRevRow %1, xmm9
; SimdModRmRevRow %1, xmm10
; SimdModRmRevRow %1, xmm11
; SimdModRmRevRow %1, xmm12
; SimdModRmRevRow %1, xmm13
; SimdModRmRevRow %1, xmm14
; SimdModRmRevRow %1, xmm15
%endmacro ; TestSimdRev

; Row 1
TestSimd movups
TestSimdRev movups
; TestSimd movlps ; FIXME
; TestSimdRev movlps ; FIXME
TestSimd movhlps
TestSimd unpcklps
TestSimd unpckhps
TestSimd movlhps
; TestSimd movhps ; FIXME
; TestSimdRev movhps

prefetchnta [0xffff]
prefetcht0 [0xffff]
prefetcht1 [0xffff]
prefetcht2 [0xffff]

%macro TestSpecialReg 1
mov cr0, %1
mov cr2, %1
mov cr3, %1
mov cr4, %1
; mov cr8, %1
mov dr0, %1
mov dr1, %1
mov dr2, %1
mov dr3, %1
mov dr4, %1
mov dr6, %1
mov dr7, %1
%endmacro ; TestSpecialReg

%macro TestSpecialRegRev 1
mov %1, cr0
mov %1, cr2
mov %1, cr3
mov %1, cr4
; mov %1, cr8
mov %1, dr0
mov %1, dr1
mov %1, dr2
mov %1, dr3
mov %1, dr4
mov %1, dr6
mov %1, dr7
%endmacro ; TestSpecialRegRev

; Row 2
TestSpecialReg eax
TestSpecialReg ecx
TestSpecialReg edx
TestSpecialReg ebx
TestSpecialReg esp
TestSpecialReg ebp
TestSpecialReg esi
TestSpecialReg edi

TestSpecialRegRev eax
TestSpecialRegRev ecx
TestSpecialRegRev edx
TestSpecialRegRev ebx
TestSpecialRegRev esp
TestSpecialRegRev ebp
TestSpecialRegRev esi
TestSpecialRegRev edi

TestSimd movaps
TestSimdRev movaps
; FIXME: Operands are not simple.
; TestSimd cvtpi2ps
; TestSimdRev movntps
; TestSimd cvttps2pi
; TestSimd cvtps2pi
TestSimd ucomiss
TestSimd comiss

wrmsr
rdtsc
rdmsr
rdpmc
sysenter
sysexit

; Row 4
TEST_ARITHMETIC_MODRM16_REV cmovo
TEST_ARITHMETIC_MODRM16_REV cmovno
TEST_ARITHMETIC_MODRM16_REV cmovb
TEST_ARITHMETIC_MODRM16_REV cmovnb
TEST_ARITHMETIC_MODRM16_REV cmovz
TEST_ARITHMETIC_MODRM16_REV cmovnz
TEST_ARITHMETIC_MODRM16_REV cmovbe
TEST_ARITHMETIC_MODRM16_REV cmovnbe
TEST_ARITHMETIC_MODRM16_REV cmovs
TEST_ARITHMETIC_MODRM16_REV cmovns
TEST_ARITHMETIC_MODRM16_REV cmovp
TEST_ARITHMETIC_MODRM16_REV cmovnp
TEST_ARITHMETIC_MODRM16_REV cmovl
TEST_ARITHMETIC_MODRM16_REV cmovnl
TEST_ARITHMETIC_MODRM16_REV cmovle
TEST_ARITHMETIC_MODRM16_REV cmovnle

; Row 5
; FIXME
; TestSimd movmskps
TestSimd sqrtss
TestSimd rsqrtss
TestSimd rcpss

TEST_ARITHMETIC_MODRM16_REV cmovs
TEST_ARITHMETIC_MODRM16_REV cmovns
TEST_ARITHMETIC_MODRM16_REV cmovp
TEST_ARITHMETIC_MODRM16_REV cmovnp
TEST_ARITHMETIC_MODRM16_REV cmovl
TEST_ARITHMETIC_MODRM16_REV cmovnl
TEST_ARITHMETIC_MODRM16_REV cmovle
TEST_ARITHMETIC_MODRM16_REV cmovnle

; Row 6
TestSimdRev punpcklbw
TestSimdRev punpcklwd
TestSimdRev punpckldq
TestSimdRev packsswb
TestSimdRev pcmpgtb
TestSimdRev pcmpgtw
TestSimdRev pcmpgtd
TestSimdRev packuswb

%macro TestMmxRow 2
TestModRmMemoryRev %1, %2
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

TestMmx punpckhbw
TestMmx punpckhwd
TestMmx punpckhdq
TestMmx packssdw

; Row 7
%macro TestMmxRowImm 2
; TestModRmMemoryImm %1, %2 ; FIXME
%1 mm0, %2, 0
%1 mm0, %2, 0xff
%1 mm1, %2, 0
%1 mm1, %2, 0xff
%1 mm2, %2, 0
%1 mm2, %2, 0xff
%1 mm3, %2, 0
%1 mm3, %2, 0xff
%1 mm4, %2, 0
%1 mm4, %2, 0xff
%1 mm5, %2, 0
%1 mm5, %2, 0xff
%1 mm6, %2, 0
%1 mm6, %2, 0xff
%1 mm7, %2, 0
%1 mm7, %2, 0xff
%endmacro ; TestMmxRowImm

%macro TestMmxImm 1
TestMmxRowImm %1, mm0
TestMmxRowImm %1, mm0
TestMmxRowImm %1, mm1
TestMmxRowImm %1, mm1
TestMmxRowImm %1, mm2
TestMmxRowImm %1, mm2
TestMmxRowImm %1, mm3
TestMmxRowImm %1, mm3
TestMmxRowImm %1, mm4
TestMmxRowImm %1, mm4
TestMmxRowImm %1, mm5
TestMmxRowImm %1, mm5
TestMmxRowImm %1, mm6
TestMmxRowImm %1, mm6
TestMmxRowImm %1, mm7
TestMmxRowImm %1, mm7
%endmacro ; TestMmxImm

TestMmxImm pshufw

%macro TestMmxRowImm2Operand 1
%1 mm0, 0
%1 mm0, 0xff
%1 mm1, 0
%1 mm1, 0xff
%1 mm2, 0
%1 mm2, 0xff
%1 mm3, 0
%1 mm3, 0xff
%1 mm4, 0
%1 mm4, 0xff
%1 mm5, 0
%1 mm5, 0xff
%1 mm6, 0
%1 mm6, 0xff
%1 mm7, 0
%1 mm7, 0xff
%endmacro

; Group 12
TestMmxRowImm2Operand psrlw
TestMmxRowImm2Operand psraw
TestMmxRowImm2Operand psllw

; Group 13
TestMmxRowImm2Operand psrld
TestMmxRowImm2Operand psrad
TestMmxRowImm2Operand pslld

; Group 14
TestMmxRowImm2Operand psrlq
TestMmxRowImm2Operand psllq

TestMmx pcmpeqb
TestMmx pcmpeqw
TestMmx pcmpeqd
emms

%macro TestMmxRowRev 2
%1 %2, mm0
%1 %2, mm1
%1 %2, mm2
%1 %2, mm3
%1 %2, mm4
%1 %2, mm5
%1 %2, mm6
%1 %2, mm7
%endmacro

TestMmxRowRev movd, eax
TestMmxRowRev movd, ecx
TestMmxRowRev movd, edx
TestMmxRowRev movd, ebx
TestMmxRowRev movd, ebp
TestMmxRowRev movd, esp
TestMmxRowRev movd, esi
TestMmxRowRev movd, edi

TestMmx movq

; Row 8
jo 0xffff
jno 0xffff
jb 0xffff
jnb 0xffff
jz 0xffff
jnz 0xffff
jbe 0xffff
jnbe 0xffff
js 0xffff
jns 0xffff
jp 0xffff
jnp 0xffff
jl 0xffff
jnl 0xffff
jle 0xffff
jnle 0xffff

; Row 9
%macro TestGprOneByte 1
%1 al
%1 ah
%1 cl
%1 ch
%1 dl
%1 dh
%1 bl
%1 bh
%endmacro

TestGprOneByte seto
TestGprOneByte setno
TestGprOneByte setb
TestGprOneByte setnb
TestGprOneByte setz
TestGprOneByte setnz
TestGprOneByte setbe
TestGprOneByte setnbe
TestGprOneByte sets
TestGprOneByte setns
TestGprOneByte setp
TestGprOneByte setnp
TestGprOneByte setl
TestGprOneByte setnl
TestGprOneByte setle
TestGprOneByte setnle

; Row 0xa
push fs
pop fs
cpuid
TEST_ARITHMETIC_MODRM16 bt


hld eax, ebx, 1



; Row 0xb

%macro TestLoadSegment 1
TestModRmMemoryRev %1, eax
TestModRmMemoryRev %1, ecx
TestModRmMemoryRev %1, edx
TestModRmMemoryRev %1, ebx
TestModRmMemoryRev %1, ebp
TestModRmMemoryRev %1, esp
TestModRmMemoryRev %1, esi
TestModRmMemoryRev %1, edi
%endmacro ; TestLoadSegment

TestLoadSegment lfs
TestLoadSegment lgs

