%include "test.inc"

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

%if ARCH == 64
mov cr8, %1
%endif

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
%if ARCH <> 64
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
%else
TestSpecialReg rax
TestSpecialReg rcx
TestSpecialReg rdx
TestSpecialReg rbx
TestSpecialReg rsp
TestSpecialReg rbp
TestSpecialReg rsi
TestSpecialReg rdi

TestSpecialRegRev rax
TestSpecialRegRev rcx
TestSpecialRegRev rdx
TestSpecialRegRev rbx
TestSpecialRegRev rsp
TestSpecialRegRev rbp
TestSpecialRegRev rsi
TestSpecialRegRev rdi
%endif

TestSimd movaps
TestSimdRev movaps

TestSimd movapd
TestSimdRev movapd

TestSimdMmx cvtpi2ps

%macro TestSimdGprRowRev 2
TestModRmMemoryRev %1, %2
%1 %2, eax
%1 %2, ecx
%1 %2, edx
%1 %2, ebx
%1 %2, ebp
%1 %2, esp
%1 %2, esi
%1 %2, edi
%endmacro ; TestSimdGprRow

%macro TestSimdGpr 1
TestSimdGprRowRev movd, xmm0
TestSimdGprRowRev movd, xmm1
TestSimdGprRowRev movd, xmm2
TestSimdGprRowRev movd, xmm3
TestSimdGprRowRev movd, xmm4
TestSimdGprRowRev movd, xmm5
TestSimdGprRowRev movd, xmm6
TestSimdGprRowRev movd, xmm7
%endmacro ; TestSimdGpr

TestSimdGpr cvtsi2ss
TestSimdGpr cvtpi2pd
TestSimdGpr cvtsi2sd

TestMovnt movntps
TestMovnt movntss
TestMovnt movntpd
TestMovnt movntsd

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

%if ARCH <> 64
sysenter
sysexit
%endif

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

TestMmxRev punpckhwd
TestSimd punpckhwd

TestMmxRev punpckhdq
TestSimd punpckhdq

TestMmxRev packssdw
TestSimd packssdw

TestSimd punpcklqdq
TestSimd punpckhqdq

%macro TestMmxGprRow 2
TestModRmMemoryRev %1, %2
%1 %2, eax
%1 %2, ecx
%1 %2, edx
%1 %2, ebx
%1 %2, ebp
%1 %2, esp
%1 %2, esi
%1 %2, edi
%endmacro

%macro TestMmxGpr 1
TestMmxGprRow movd, mm0
TestMmxGprRow movd, mm1
TestMmxGprRow movd, mm2
TestMmxGprRow movd, mm3
TestMmxGprRow movd, mm4
TestMmxGprRow movd, mm5
TestMmxGprRow movd, mm6
TestMmxGprRow movd, mm7
%endmacro ; TestMmxGpr

TestMmxGpr movd
TestSimdGpr movd

TestSimd movdqu
TestSimd movdqa

; Row 7
TestMmxImm pshufw
TestSimdImmThreeOperand pshufhw
TestSimdImmThreeOperand pshufd
TestSimdImmThreeOperand pshuflw

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

%macro TestSimdRowImm2Operand 1
%1 xmm0, 0
%1 xmm0, 0xff
%1 xmm1, 0
%1 xmm1, 0xff
%1 xmm2, 0
%1 xmm2, 0xff
%1 xmm3, 0
%1 xmm3, 0xff
%1 xmm4, 0
%1 xmm4, 0xff
%1 xmm5, 0
%1 xmm5, 0xff
%1 xmm6, 0
%1 xmm6, 0xff
%1 xmm7, 0
%1 xmm7, 0xff
%endmacro

; Group 12
TestMmxRowImm2Operand psrlw
TestSimdRowImm2Operand psrlw
TestMmxRowImm2Operand psraw
TestSimdRowImm2Operand psraw
TestMmxRowImm2Operand psllw
TestSimdRowImm2Operand psllw

; Group 13
TestMmxRowImm2Operand psrld
TestSimdRowImm2Operand psrld
TestMmxRowImm2Operand psrad
TestSimdRowImm2Operand psrad
TestMmxRowImm2Operand pslld
TestSimdRowImm2Operand pslld

; Group 14
TestMmxRowImm2Operand psrlq
TestSimdRowImm2Operand psrlq
TestMmxRowImm2Operand psllq
TestSimdRowImm2Operand psllq

TestMmxRev pcmpeqb
TestSimdRev pcmpeqb
TestMmxRev pcmpeqw
TestSimdRev pcmpeqw
TestMmxRev pcmpeqd
TestSimdRev pcmpeqd
emms

TestSimd extrq
TestSimd insertq

TestSimd haddpd
TestSimd hsubpd
TestSimd haddps
TestSimd hsubps

%macro TestMmxGprRevRow 2
TestModRmMemory %1, %2
%1 eax, %2
%1 ecx, %2
%1 edx, %2
%1 ebx, %2
%1 ebp, %2
%1 esp, %2
%1 esi, %2
%1 edi, %2
%endmacro ; TestMmxGprRevRow

%macro TestMmxGprRev 1
TestMmxGprRevRow %1, mm0
TestMmxGprRevRow %1, mm1
TestMmxGprRevRow %1, mm2
TestMmxGprRevRow %1, mm3
TestMmxGprRevRow %1, mm4
TestMmxGprRevRow %1, mm5
TestMmxGprRevRow %1, mm6
TestMmxGprRevRow %1, mm7
%endmacro ; TestMmxGprRev

TestMmxGprRev movd
TestMmx movq
TestSimd movq
TestSimdRev movdqu
TestSimdRev movdqa

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

TEST_ARITHMETIC_MODRM16_REV popcnt
TEST_ARITHMETIC_MODRM16_REV tzcnt
TEST_ARITHMETIC_MODRM16_REV lzcnt

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

TestSimdImm cmpps
TestSimdImm cmpss
TestSimdImm cmppd
TestSimdImm cmpsd

TEST_ARITHMETIC_RM32_MEMORY movnti, eax
TEST_ARITHMETIC_RM32_MEMORY movnti, ecx
TEST_ARITHMETIC_RM32_MEMORY movnti, edx
TEST_ARITHMETIC_RM32_MEMORY movnti, ebx
TEST_ARITHMETIC_RM32_MEMORY movnti, ebp
TEST_ARITHMETIC_RM32_MEMORY movnti, esp
TEST_ARITHMETIC_RM32_MEMORY movnti, esi
TEST_ARITHMETIC_RM32_MEMORY movnti, edi

TestMmxGprImm pinsrw, eax
TestSimdGprImm pinsrw, eax
TestMmxGprImm pinsrw, ecx
TestSimdGprImm pinsrw, ecx
TestMmxGprImm pinsrw, edx
TestSimdGprImm pinsrw, edx
TestMmxGprImm pinsrw, ebx
TestSimdGprImm pinsrw, ebx
TestMmxGprImm pinsrw, ebp
TestSimdGprImm pinsrw, ebp
TestMmxGprImm pinsrw, esp
TestSimdGprImm pinsrw, esp
TestMmxGprImm pinsrw, esi
TestSimdGprImm pinsrw, esi
TestMmxGprImm pinsrw, edi
TestSimdGprImm pinsrw, edi

TestGprMmxImm pextrw, eax
TestGprSimdImm pextrw, eax
TestGprMmxImm pextrw, ecx
TestGprSimdImm pextrw, ecx
TestGprMmxImm pextrw, edx
TestGprSimdImm pextrw, edx
TestGprMmxImm pextrw, ebx
TestGprSimdImm pextrw, ebx
TestGprMmxImm pextrw, ebp
TestGprSimdImm pextrw, ebp
TestGprMmxImm pextrw, esp
TestGprSimdImm pextrw, esp
TestGprMmxImm pextrw, esi
TestGprSimdImm pextrw, esi
TestGprMmxImm pextrw, edi
TestGprSimdImm pextrw, edi

TestSimdImm shufps
TestSimdImm shufpd

cmpxchg8b [0xffff]

%if ARCH == 64
cmpxchg16b [0xffff]
%endif

bswap eax
bswap ecx
bswap edx
bswap ebx
bswap ebp
bswap esp
bswap esi
bswap edi

; Row 0xd
TestSimdRev addsubpd
TestSimdRev addsubps

TestMmxRev psrlw
TestSimdRev psrlw
TestMmxRev psrld
TestSimdRev psrld
TestMmxRev psrlq
TestSimdRev psrlq
TestMmxRev paddq
TestSimdRev paddq
TestMmxRev pmullw
TestSimdRev pmullw

%macro TestSimdMmxRegRow 2
%1 mm0, %2
%1 mm1, %2
%1 mm2, %2
%1 mm3, %2
%1 mm4, %2
%1 mm5, %2
%1 mm6, %2
%1 mm7, %2
%endmacro ; TestSimdMmxRegRow

%macro TestSimdMmxReg 1
TestSimdMmxRegRow %1, xmm0
TestSimdMmxRegRow %1, xmm1
TestSimdMmxRegRow %1, xmm2
TestSimdMmxRegRow %1, xmm3
TestSimdMmxRegRow %1, xmm4
TestSimdMmxRegRow %1, xmm5
TestSimdMmxRegRow %1, xmm6
TestSimdMmxRegRow %1, xmm7
; TestSimdMmxRegRow %1, xmm8
; TestSimdMmxRegRow %1, xmm9
; TestSimdMmxRegRow %1, xmm10
; TestSimdMmxRegRow %1, xmm11
; TestSimdMmxRegRow %1, xmm12
; TestSimdMmxRegRow %1, xmm13
; TestSimdMmxRegRow %1, xmm14
; TestSimdMmxRegRow %1, xmm15
%endmacro ; TestSimdMmxReg

%macro TestSimdMmxRegRowRev 2
%1 %2, mm0
%1 %2, mm1
%1 %2, mm2
%1 %2, mm3
%1 %2, mm4
%1 %2, mm5
%1 %2, mm6
%1 %2, mm7
%endmacro ; TestSimdMmxRowRev

%macro TestSimdMmxRegRev 1
TestSimdMmxRegRowRev %1, xmm0
TestSimdMmxRegRowRev %1, xmm1
TestSimdMmxRegRowRev %1, xmm2
TestSimdMmxRegRowRev %1, xmm3
TestSimdMmxRegRowRev %1, xmm4
TestSimdMmxRegRowRev %1, xmm5
TestSimdMmxRegRowRev %1, xmm6
TestSimdMmxRegRowRev %1, xmm7
; TestSimdMmxRegRowRev %1, xmm8
; TestSimdMmxRegRowRev %1, xmm9
; TestSimdMmxRegRowRev %1, xmm10
; TestSimdMmxRegRowRev %1, xmm11
; TestSimdMmxRegRowRev %1, xmm12
; TestSimdMmxRegRowRev %1, xmm13
; TestSimdMmxRegRowRev %1, xmm14
; TestSimdMmxRegRowRev %1, xmm15
%endmacro ; TestSimdMmxRegRev

TestSimdMmxRegRev movq2dq
TestSimdRev movq
TestSimdMmxReg movdq2q

%macro TestGprMmxRow 2
%1 %2, mm0
%1 %2, mm1
%1 %2, mm2
%1 %2, mm3
%1 %2, mm4
%1 %2, mm5
%1 %2, mm6
%1 %2, mm7
%endmacro ; TestGprMmxRow

%macro TestGprMmx 1
TestGprMmxRow %1, eax
TestGprMmxRow %1, ecx
TestGprMmxRow %1, edx
TestGprMmxRow %1, ebx
TestGprMmxRow %1, ebp
TestGprMmxRow %1, esp
TestGprMmxRow %1, esi
TestGprMmxRow %1, edi
%endmacro ; TestGprMmx

TestGprSimd pmovmskb
TestGprMmx pmovmskb

TestMmxRev psubusb
TestSimdRev psubusb

TestMmxRev psubusw
TestSimdRev psubusw

TestMmxRev pminub
TestSimdRev pminub

TestMmxRev pand
TestSimdRev pand

TestMmxRev paddusb
TestSimdRev paddusb

TestMmxRev paddusw
TestSimdRev paddusw

TestMmxRev pmaxub
TestSimdRev pmaxub

TestMmxRev pandn
TestSimdRev pandn

; Row 0xe
TestMmxRev pavgb
TestSimdRev pavgb

TestMmxRev psraw
TestSimdRev psraw

TestMmxRev psrad
TestSimdRev psrad

TestMmxRev pavgw
TestSimdRev pavgw

TestMmxRev pmulhuw
TestSimdRev pmulhuw

TestMmxRev pmulhw
TestSimdRev pmulhw

TEST_ARITHMETIC_RM64_MEMORY movntq, mm0
TEST_ARITHMETIC_RM64_MEMORY movntq, mm1
TEST_ARITHMETIC_RM64_MEMORY movntq, mm2
TEST_ARITHMETIC_RM64_MEMORY movntq, mm3
TEST_ARITHMETIC_RM64_MEMORY movntq, mm4
TEST_ARITHMETIC_RM64_MEMORY movntq, mm5
TEST_ARITHMETIC_RM64_MEMORY movntq, mm6
TEST_ARITHMETIC_RM64_MEMORY movntq, mm7

TEST_ARITHMETIC_RM128_MEMORY movntdq, xmm0
TEST_ARITHMETIC_RM128_MEMORY movntdq, xmm1
TEST_ARITHMETIC_RM128_MEMORY movntdq, xmm2
TEST_ARITHMETIC_RM128_MEMORY movntdq, xmm3
TEST_ARITHMETIC_RM128_MEMORY movntdq, xmm4
TEST_ARITHMETIC_RM128_MEMORY movntdq, xmm5
TEST_ARITHMETIC_RM128_MEMORY movntdq, xmm6
TEST_ARITHMETIC_RM128_MEMORY movntdq, xmm7

TestMmxRev psubsb
TestSimdRev psubsb

TestMmxRev psubsw
TestSimdRev psubsw

TestMmxRev pminsw
TestSimdRev pminsw

TestMmxRev por
TestSimdRev por

TestMmxRev paddsb
TestSimdRev paddsb

TestMmxRev paddsw
TestSimdRev paddsw

TestMmxRev pmaxsw
TestSimdRev pmaxsw

TestMmxRev pxor
TestSimdRev pxor

; Row 0xf
TEST_ARITHMETIC_RM128_MEMORY_REV lddqu, xmm0
TEST_ARITHMETIC_RM128_MEMORY_REV lddqu, xmm1
TEST_ARITHMETIC_RM128_MEMORY_REV lddqu, xmm2
TEST_ARITHMETIC_RM128_MEMORY_REV lddqu, xmm3
TEST_ARITHMETIC_RM128_MEMORY_REV lddqu, xmm4
TEST_ARITHMETIC_RM128_MEMORY_REV lddqu, xmm5
TEST_ARITHMETIC_RM128_MEMORY_REV lddqu, xmm6
TEST_ARITHMETIC_RM128_MEMORY_REV lddqu, xmm7

TestMmxRev psllw
TestSimdRev psllw

TestMmxRev pslld
TestSimdRev pslld

TestMmxRev psllq
TestSimdRev psllq

TestMmxRev pmuludq
TestSimdRev pmuludq

TestMmxRev pmaddwd
TestSimdRev pmaddwd

TestMmxRev psadbw
TestSimdRev psadbw

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

%macro SimdModRmRevRegRow 2
%1 xmm0, %2
%1 xmm1, %2
%1 xmm2, %2
%1 xmm3, %2
%1 xmm4, %2
%1 xmm5, %2
%1 xmm6, %2
%1 xmm7, %2
%endmacro ; SimdModRmRevRegRow

%macro TestSimdRevReg 1
SimdModRmRevRegRow %1, xmm0
SimdModRmRevRegRow %1, xmm1
SimdModRmRevRegRow %1, xmm2
SimdModRmRevRegRow %1, xmm3
SimdModRmRevRegRow %1, xmm4
SimdModRmRevRegRow %1, xmm5
SimdModRmRevRegRow %1, xmm6
SimdModRmRevRegRow %1, xmm7
%endmacro ; TestSimdRevReg

TestMmxRevReg maskmovq
TestSimdRevReg maskmovdqu

TestMmxRev psubb
TestSimdRev psubb

TestMmxRev psubw
TestSimdRev psubw

TestMmxRev psubd
TestSimdRev psubd

TestMmxRev psubq
TestSimdRev psubq

TestMmxRev paddb
TestSimdRev paddb

TestMmxRev paddw
TestSimdRev paddw

TestMmxRev paddd
TestSimdRev paddd

ud0
