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

; 3dnow
TestMmxRev pfcmpge
TestMmxRev pfcmpgt
TestMmxRev pfcmpeq
TestMmxRev pfmin
TestMmxRev pfmax
TestMmxRev pfmul
TestMmxRev pfrcp
TestMmxRev pfrcpit1
TestMmxRev pfrcpit2
TestMmxRev pfrsqrt
TestMmxRev pfrsqit1

; TestMmxRev pmulhrw ; yasm doesn't support yet?!
db 0x0f, 0x0f, 0x00, 0xb7

TestMmxRev pi2fw
TestMmxRev pf2iw
TestMmxRev pi2fd
TestMmxRev pf2id

TestMmxRev pfsub ; signed
TestMmxRev pfsubr ; signed
TestMmxRev pswapd ; signed
TestMmxRev pfpnacc ; signed
TestMmxRev pfadd ; signed
TestMmxRev pfacc ; signed
TestMmxRev pavgusb ; signed
TestMmxRev pfnacc ; signed

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
TestSimd movss
TestSimdRev movss
TestSimd movupd
TestSimdRev movupd
TestSimd movsd
TestSimdRev movsd

TestModRmMemory movlps, xmm0
TestModRmMemory movlps, xmm1
TestModRmMemory movlps, xmm2
TestModRmMemory movlps, xmm3
TestModRmMemory movlps, xmm4
TestModRmMemory movlps, xmm5
TestModRmMemory movlps, xmm6
TestModRmMemory movlps, xmm7
; FIXME: xmm8-15

TestModRmMemoryRev movlps, xmm0
TestModRmMemoryRev movlps, xmm1
TestModRmMemoryRev movlps, xmm2
TestModRmMemoryRev movlps, xmm3
TestModRmMemoryRev movlps, xmm4
TestModRmMemoryRev movlps, xmm5
TestModRmMemoryRev movlps, xmm6
TestModRmMemoryRev movlps, xmm7

TestSimd movsldup

TestModRmMemoryRev movlpd, xmm0
TestModRmMemoryRev movlpd, xmm1
TestModRmMemoryRev movlpd, xmm2
TestModRmMemoryRev movlpd, xmm3
TestModRmMemoryRev movlpd, xmm4
TestModRmMemoryRev movlpd, xmm5
TestModRmMemoryRev movlpd, xmm6
TestModRmMemoryRev movlpd, xmm7

TestModRmMemory movlpd, xmm0
TestModRmMemory movlpd, xmm1
TestModRmMemory movlpd, xmm2
TestModRmMemory movlpd, xmm3
TestModRmMemory movlpd, xmm4
TestModRmMemory movlpd, xmm5
TestModRmMemory movlpd, xmm6
TestModRmMemory movlpd, xmm7

TestSimd movddup

TestSimd movhlps
TestSimd unpcklps
TestSimd unpcklpd
TestSimd unpckhps
TestSimd unpckhpd
TestSimd movlhps

TestModRmMemory movhps, xmm0
TestModRmMemory movhps, xmm1
TestModRmMemory movhps, xmm2
TestModRmMemory movhps, xmm3
TestModRmMemory movhps, xmm4
TestModRmMemory movhps, xmm5
TestModRmMemory movhps, xmm6
TestModRmMemory movhps, xmm7

TestModRmMemoryRev movhps, xmm0
TestModRmMemoryRev movhps, xmm1
TestModRmMemoryRev movhps, xmm2
TestModRmMemoryRev movhps, xmm3
TestModRmMemoryRev movhps, xmm4
TestModRmMemoryRev movhps, xmm5
TestModRmMemoryRev movhps, xmm6
TestModRmMemoryRev movhps, xmm7

TestModRmMemoryRev movhpd, xmm0
TestModRmMemoryRev movhpd, xmm1
TestModRmMemoryRev movhpd, xmm2
TestModRmMemoryRev movhpd, xmm3
TestModRmMemoryRev movhpd, xmm4
TestModRmMemoryRev movhpd, xmm5
TestModRmMemoryRev movhpd, xmm6
TestModRmMemoryRev movhpd, xmm7

TestModRmMemory movhpd, xmm0
TestModRmMemory movhpd, xmm1
TestModRmMemory movhpd, xmm2
TestModRmMemory movhpd, xmm3
TestModRmMemory movhpd, xmm4
TestModRmMemory movhpd, xmm5
TestModRmMemory movhpd, xmm6
TestModRmMemory movhpd, xmm7

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

TestSimd movapd
TestSimdRev movapd

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
; TestSimdMmxRow %1, xmm8
; TestSimdMmxRow %1, xmm9
; TestSimdMmxRow %1, xmm10
; TestSimdMmxRow %1, xmm11
; TestSimdMmxRow %1, xmm12
; TestSimdMmxRow %1, xmm13
; TestSimdMmxRow %1, xmm14
; TestSimdMmxRow %1, xmm15
%endmacro ; TestSimdMmx

TestSimdMmx cvtpi2ps
; TestSimdGpr cvtsi2ss ; FIXME
; TestSimd cvtpi2pd ; FIXME
; TestSimd cvtsi2sd

%macro TestMovnt 1
TestModRmMemory movntps, xmm0
TestModRmMemory movntps, xmm1
TestModRmMemory movntps, xmm2
TestModRmMemory movntps, xmm3
TestModRmMemory movntps, xmm4
TestModRmMemory movntps, xmm5
TestModRmMemory movntps, xmm6
TestModRmMemory movntps, xmm7
; TestModRmMemory movntps, xmm8
; TestModRmMemory movntps, xmm9
; TestModRmMemory movntps, xmm10
; TestModRmMemory movntps, xmm11
; TestModRmMemory movntps, xmm12
; TestModRmMemory movntps, xmm13
; TestModRmMemory movntps, xmm14
; TestModRmMemory movntps, xmm15
%endmacro ; TestMovnt

TestMovnt movntps
TestMovnt movntss
TestMovnt movntpd
TestMovnt movntsd

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
; %1, xmm8, %2
; %1, xmm9, %2
; %1, xmm10, %2
; %1, xmm11, %2
; %1, xmm12, %2
; %1, xmm13, %2
; %1, xmm14, %2
; %1, xmm15, %2
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

TestMmxSimd cvttps2pi
TestMmxSimd cvtps2pi
TestMmxSimd cvttpd2pi

%macro TestGprSimdRow 2
%1 eax, %2
%1 ecx, %2
%1 edx, %2
%1 ebx, %2
%1 ebp, %2
%1 esp, %2
%1 esi, %2
%1 edi, %2
%endmacro ; TestGprSimdRow

%macro TestGprSimd 1
TestGprSimdRow %1, xmm0
TestGprSimdRow %1, xmm1
TestGprSimdRow %1, xmm2
TestGprSimdRow %1, xmm3
TestGprSimdRow %1, xmm4
TestGprSimdRow %1, xmm5
TestGprSimdRow %1, xmm6
TestGprSimdRow %1, xmm7
; TestGprSimdRow %1, xmm8
; TestGprSimdRow %1, xmm9
; TestGprSimdRow %1, xmm10
; TestGprSimdRow %1, xmm11
; TestGprSimdRow %1, xmm12
; TestGprSimdRow %1, xmm13
; TestGprSimdRow %1, xmm14
; TestGprSimdRow %1, xmm15
%endmacro ; TestGprSimd

TestGprSimd cvttsd2si

TestSimd ucomiss
TestSimd ucomisd
TestSimd comiss
TestSimd comisd

; Row 3
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

TEST_ARITHMETIC_MODRM16_REV cmovs
TEST_ARITHMETIC_MODRM16_REV cmovns
TEST_ARITHMETIC_MODRM16_REV cmovp
TEST_ARITHMETIC_MODRM16_REV cmovnp
TEST_ARITHMETIC_MODRM16_REV cmovl
TEST_ARITHMETIC_MODRM16_REV cmovnl
TEST_ARITHMETIC_MODRM16_REV cmovle
TEST_ARITHMETIC_MODRM16_REV cmovnle


; Row 5
TestGprSimd movmskps
TestGprSimd movmskpd

TestSimd sqrtps
TestSimd sqrtss
TestSimd sqrtpd
TestSimd sqrtsd

TestSimd rsqrtps
TestSimd rsqrtss

TestSimd rcpps
TestSimd rcpss

TestSimd andps
TestSimd andpd

TestSimd andnps
TestSimd andnpd

TestSimd orps
TestSimd orpd

TestSimd xorps
TestSimd xorpd

TestSimd addps
TestSimd addss
TestSimd addpd
TestSimd addsd

TestSimd mulps
TestSimd mulss
TestSimd mulpd
TestSimd mulsd

TestSimd cvtps2pd
TestSimd cvtss2sd
TestSimd cvtpd2ps
TestSimd cvtsd2ss

TestMmxSimd cvtps2pi
TestGprSimd cvtss2si
TestMmxSimd cvtpd2pi
TestGprSimd cvtsd2si

TestSimd subps
TestSimd subss
TestSimd subpd
TestSimd subsd

TestSimd minps
TestSimd minss
TestSimd minpd
TestSimd minsd

TestSimd divps
TestSimd divss
TestSimd divpd
TestSimd divsd

TestSimd maxps
TestSimd maxss
TestSimd maxpd
TestSimd maxsd

; Row 6
TestMmxRev punpcklbw
TestSimdRev punpcklbw

TestMmxRev punpcklwd
TestSimdRev punpcklwd

TestMmxRev punpckldq
TestSimdRev punpckldq

TestMmxRev packsswb
TestSimdRev packsswb

TestMmxRev pcmpgtb
TestSimdRev pcmpgtb

TestMmxRev pcmpgtw
TestSimdRev pcmpgtw

TestMmxRev pcmpgtd
TestSimdRev pcmpgtd

TestMmxRev packuswb
TestSimdRev packuswb

TestMmxRev punpckhbw
TestSimd punpckhbw

TestMmxRev punpckhbw
TestMmxRev punpckhwd
TestMmxRev punpckhdq
TestMmxRev packssdw

; Row 7
%macro TestModRmMemoryThreeOperandRev 3
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
%endmacro ; TestModRmMemoryThreeOperandRev

%macro TestMmxRowImm 2
TestModRmMemoryThreeOperandRev %1, %2, 0
TestModRmMemoryThreeOperandRev %1, %2, 1
TestModRmMemoryThreeOperandRev %1, %2, 0xff
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

TestMmxRev pcmpeqb
TestMmxRev pcmpeqw
TestMmxRev pcmpeqd
emms

%macro TestMmxGpr 2
%1 mm0, %2
%1 mm1, %2
%1 mm2, %2
%1 mm3, %2
%1 mm4, %2
%1 mm5, %2
%1 mm6, %2
%1 mm7, %2
%endmacro ; TestMmxGpr

%macro TestMmxGprRev 2
%1 %2, mm0
%1 %2, mm1
%1 %2, mm2
%1 %2, mm3
%1 %2, mm4
%1 %2, mm5
%1 %2, mm6
%1 %2, mm7
%endmacro ; TestMmxGpr

TestModRmMemory movd, mm0
TestModRmMemory movd, mm1
TestModRmMemory movd, mm2
TestModRmMemory movd, mm3
TestModRmMemory movd, mm4
TestModRmMemory movd, mm5
TestModRmMemory movd, mm6
TestModRmMemory movd, mm7

TestModRmMemoryRev movd, mm0
TestModRmMemoryRev movd, mm1
TestModRmMemoryRev movd, mm2
TestModRmMemoryRev movd, mm3
TestModRmMemoryRev movd, mm4
TestModRmMemoryRev movd, mm5
TestModRmMemoryRev movd, mm6
TestModRmMemoryRev movd, mm7

TestMmxGpr movd, eax
TestMmxGpr movd, ecx
TestMmxGpr movd, edx
TestMmxGpr movd, ebx
TestMmxGpr movd, ebp
TestMmxGpr movd, esp
TestMmxGpr movd, esi
TestMmxGpr movd, edi

TestMmxGprRev movd, eax
TestMmxGprRev movd, ecx
TestMmxGprRev movd, edx
TestMmxGprRev movd, ebx
TestMmxGprRev movd, ebp
TestMmxGprRev movd, esp
TestMmxGprRev movd, esi
TestMmxGprRev movd, edi

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

%macro TestModRmMemoryThreeOperand 3
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
%endmacro ; TestModRmMemoryThreeOperand

%macro TestShiftDoubleRow 2
TestModRmMemoryThreeOperand %1, %2, 0
TestModRmMemoryThreeOperand %1, %2, 0xff
TestModRmMemoryThreeOperand %1, %2, cl
%1 %2, eax, 0
%1 %2, eax, 0xff
%1 %2, eax, cl
%1 %2, ecx, 0
%1 %2, ecx, 0xff
%1 %2, ecx, cl
%1 %2, edx, 0
%1 %2, edx, 0xff
%1 %2, edx, cl
%1 %2, ebx, 0
%1 %2, ebx, 0xff
%1 %2, ebx, cl
%1 %2, ebp, 0
%1 %2, ebp, 0xff
%1 %2, ebp, cl
%1 %2, esp, 0
%1 %2, esp, 0xff
%1 %2, esp, cl
%1 %2, esi, 0
%1 %2, esi, 0xff
%1 %2, esi, cl
%1 %2, edi, 0
%1 %2, edi, 0xff
%1 %2, edi, cl
%endmacro ; TestShiftDoubleRow

%macro TestShiftDouble 1
TestShiftDoubleRow %1, eax
TestShiftDoubleRow %1, ecx
TestShiftDoubleRow %1, edx
TestShiftDoubleRow %1, ebx
TestShiftDoubleRow %1, ebp
TestShiftDoubleRow %1, esp
TestShiftDoubleRow %1, esi
TestShiftDoubleRow %1, edi
%endmacro ; TestShiftDouble

TestShiftDouble shld
push gs
pop gs
rsm
TEST_ARITHMETIC_MODRM16 bts
TestShiftDouble shrd

; Group 15
fxsave [0xffff]
fxrstor [0xffff]
ldmxcsr [0xffff]
stmxcsr [0xffff]
xsave [0xffff]
lfence
mfence
sfence
xrstor [0xffff]
xsave [0xffff]
clflush [0xffff]

TEST_ARITHMETIC_MODRM16_REV imul


; Row 0xb

TEST_ARITHMETIC_MODRM8 cmpxchg
TEST_ARITHMETIC_MODRM16 cmpxchg

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

TestLoadSegment lss

TEST_ARITHMETIC_MODRM16 btr

TestLoadSegment lfs
TestLoadSegment lgs

TEST_ARITHMETIC_RM8_REV movzx, ax
TEST_ARITHMETIC_RM8_REV movzx, cx
TEST_ARITHMETIC_RM8_REV movzx, dx
TEST_ARITHMETIC_RM8_REV movzx, bx
TEST_ARITHMETIC_RM8_REV movzx, bp
TEST_ARITHMETIC_RM8_REV movzx, sp
TEST_ARITHMETIC_RM8_REV movzx, si
TEST_ARITHMETIC_RM8_REV movzx, di

TEST_ARITHMETIC_RM16_REV movzx, eax
TEST_ARITHMETIC_RM16_REV movzx, ecx
TEST_ARITHMETIC_RM16_REV movzx, edx
TEST_ARITHMETIC_RM16_REV movzx, ebx
TEST_ARITHMETIC_RM16_REV movzx, ebp
TEST_ARITHMETIC_RM16_REV movzx, esp
TEST_ARITHMETIC_RM16_REV movzx, esi
TEST_ARITHMETIC_RM16_REV movzx, edi

; Group 10
ud1

; Group 8
%macro TestBits 2
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

	; Test ModRM Mod==3
	%1 ax, %2
	%1 cx, %2
	%1 dx, %2
	%1 bx, %2
	%1 bp, %2
	%1 sp, %2
	%1 si, %2
	%1 di, %2
%endmacro ; TestBits

TestBits bt, 0
TestBits bt, 1
TestBits bt, 0xff
TestBits bts, 0
TestBits bts, 1
TestBits bts, 0xff
TestBits btr, 0
TestBits btr, 1
TestBits btr, 0xff
TestBits btc, 0
TestBits btc, 1
TestBits btc, 0xff

TEST_ARITHMETIC_MODRM16 btc
TEST_ARITHMETIC_MODRM16_REV bsf
TEST_ARITHMETIC_MODRM16_REV bsr

TEST_ARITHMETIC_RM8_REV movsx, ax
TEST_ARITHMETIC_RM8_REV movsx, cx
TEST_ARITHMETIC_RM8_REV movsx, dx
TEST_ARITHMETIC_RM8_REV movsx, bx
TEST_ARITHMETIC_RM8_REV movsx, bp
TEST_ARITHMETIC_RM8_REV movsx, sp
TEST_ARITHMETIC_RM8_REV movsx, si
TEST_ARITHMETIC_RM8_REV movsx, di

TEST_ARITHMETIC_RM16_REV movsx, eax
TEST_ARITHMETIC_RM16_REV movsx, ecx
TEST_ARITHMETIC_RM16_REV movsx, edx
TEST_ARITHMETIC_RM16_REV movsx, ebx
TEST_ARITHMETIC_RM16_REV movsx, ebp
TEST_ARITHMETIC_RM16_REV movsx, esp
TEST_ARITHMETIC_RM16_REV movsx, esi
TEST_ARITHMETIC_RM16_REV movsx, edi

; Row 0xc
TEST_ARITHMETIC_MODRM8 xadd
TEST_ARITHMETIC_MODRM16 xadd

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
%endmacro ; TestSimdImm

TestSimdImm cmpps

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

TEST_ARITHMETIC_RM32_MEMORY movnti, eax
TEST_ARITHMETIC_RM32_MEMORY movnti, ecx
TEST_ARITHMETIC_RM32_MEMORY movnti, edx
TEST_ARITHMETIC_RM32_MEMORY movnti, ebx
TEST_ARITHMETIC_RM32_MEMORY movnti, ebp
TEST_ARITHMETIC_RM32_MEMORY movnti, esp
TEST_ARITHMETIC_RM32_MEMORY movnti, esi
TEST_ARITHMETIC_RM32_MEMORY movnti, edi

TestMmxGprImm pinsrw, eax
TestMmxGprImm pinsrw, ecx
TestMmxGprImm pinsrw, edx
TestMmxGprImm pinsrw, ebx
TestMmxGprImm pinsrw, ebp
TestMmxGprImm pinsrw, esp
TestMmxGprImm pinsrw, esi
TestMmxGprImm pinsrw, edi

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

TestGprMmxImm pextrw, eax
TestGprMmxImm pextrw, ecx
TestGprMmxImm pextrw, edx
TestGprMmxImm pextrw, ebx
TestGprMmxImm pextrw, ebp
TestGprMmxImm pextrw, esp
TestGprMmxImm pextrw, esi
TestGprMmxImm pextrw, edi

TestSimdImm shufps

cmpxchg8b [0xffff]
; cmpxchg16b

bswap eax
bswap ecx
bswap edx
bswap ebx
bswap ebp
bswap esp
bswap esi
bswap edi

; Row 0xd
TestMmxRev psrlw
TestMmxRev psrld
TestMmxRev psrlq
TestMmxRev paddq
TestMmxRev pmullw

%macro TestGprMmx 2
%1 %2, mm0
%1 %2, mm1
%1 %2, mm2
%1 %2, mm3
%1 %2, mm4
%1 %2, mm5
%1 %2, mm6
%1 %2, mm7
%endmacro ; TestGprMmx

TestGprMmx pmovmskb, eax
TestGprMmx pmovmskb, ecx
TestGprMmx pmovmskb, edx
TestGprMmx pmovmskb, ebx
TestGprMmx pmovmskb, ebp
TestGprMmx pmovmskb, esp
TestGprMmx pmovmskb, esi
TestGprMmx pmovmskb, edi

TestMmxRev psubusb
TestMmxRev psubusw
TestMmxRev pminub
TestMmxRev pand
TestMmxRev paddusb
TestMmxRev paddusw
TestMmxRev pmaxub
TestMmxRev pandn

; Row 0xe
TestMmxRev pavgb
TestMmxRev psraw
TestMmxRev psrad
TestMmxRev pavgw
TestMmxRev pmulhuw
TestMmxRev pmulhw

; movntq
; FIXME
; TEST_ARITHMETIC_RM64_MEMORY movntq, rax
; TEST_ARITHMETIC_RM64_MEMORY movntq, rcx
; TEST_ARITHMETIC_RM64_MEMORY movntq, rdx
; TEST_ARITHMETIC_RM64_MEMORY movntq, rbx
; TEST_ARITHMETIC_RM64_MEMORY movntq, rbp
; TEST_ARITHMETIC_RM64_MEMORY movntq, rsp
; TEST_ARITHMETIC_RM64_MEMORY movntq, rsi
; TEST_ARITHMETIC_RM64_MEMORY movntq, rdi

TestMmxRev psubsb
TestMmxRev psubsw
TestMmxRev pminsw
TestMmxRev por
TestMmxRev paddsb
TestMmxRev paddsw
TestMmxRev pmaxsw
TestMmxRev pxor

; Row 0xf
TestMmxRev psllw
TestMmxRev pslld
TestMmxRev psllq
TestMmxRev pmuludq
TestMmxRev pmaddwd
TestMmxRev psadbw

%macro MmxModRmRevRegRow 2
%1 mm0, %2
%1 mm1, %2
%1 mm2, %2
%1 mm3, %2
%1 mm4, %2
%1 mm5, %2
%1 mm6, %2
%1 mm7, %2
%endmacro ; MmxModRmRevRegRow

%macro TestMmxRevReg 1
MmxModRmRevRegRow %1, mm0
MmxModRmRevRegRow %1, mm1
MmxModRmRevRegRow %1, mm2
MmxModRmRevRegRow %1, mm3
MmxModRmRevRegRow %1, mm4
MmxModRmRevRegRow %1, mm5
MmxModRmRevRegRow %1, mm6
MmxModRmRevRegRow %1, mm7
%endmacro ; TestMmxRevReg

TestMmxRevReg maskmovq

TestMmxRev psubb
TestMmxRev psubw
TestMmxRev psubd
TestMmxRev psubq
TestMmxRev paddb
TestMmxRev paddw
TestMmxRev paddd

ud0
