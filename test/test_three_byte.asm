%include "test.inc"

; 0f 38

; Row 0
TestMmxRev pshufb
TestSimdRev pshufb

TestMmxRev phaddw
TestSimdRev phaddw

TestMmxRev phaddd
TestSimdRev phaddd

TestMmxRev phaddsw
TestSimdRev phaddsw

TestMmxRev pmaddubsw
TestSimdRev pmaddubsw

TestMmxRev phsubw
TestSimdRev phsubw

TestMmxRev phsubd
TestSimdRev phsubd

TestMmxRev phsubsw
TestSimdRev phsubsw

TestMmxRev psignb
TestSimdRev psignb

TestMmxRev psignw
TestSimdRev psignw

TestMmxRev psignd
TestSimdRev psignd

TestMmxRev pmulhrsw
TestSimdRev pmulhrsw

; Row 1
TestSimdRev pblendvb
TestSimdRev blendvps
TestSimdRev blendvpd
TestSimdRev ptest

TestMmxRev pabsb
TestSimdRev pabsb

TestMmxRev pabsw
TestSimdRev pabsw

TestMmxRev pabsd
TestSimdRev pabsd

; Row 2
TestSimdRev pmovsxbw
TestSimdRev pmovsxbd
TestSimdRev pmovsxbq
TestSimdRev pmovsxwd
TestSimdRev pmovsxwq
TestSimdRev pmovsxdq
TestSimdRev pmuldq
TestSimdRev pcmpeqq

TestModRmMemoryRev movntdqa, xmm0
TestModRmMemoryRev movntdqa, xmm1
TestModRmMemoryRev movntdqa, xmm2
TestModRmMemoryRev movntdqa, xmm3
TestModRmMemoryRev movntdqa, xmm4
TestModRmMemoryRev movntdqa, xmm5
TestModRmMemoryRev movntdqa, xmm6
TestModRmMemoryRev movntdqa, xmm7
%if ARCH==64
TestModRmMemoryRev movntdqa, xmm8
TestModRmMemoryRev movntdqa, xmm9
TestModRmMemoryRev movntdqa, xmm10
TestModRmMemoryRev movntdqa, xmm11
TestModRmMemoryRev movntdqa, xmm12
TestModRmMemoryRev movntdqa, xmm13
TestModRmMemoryRev movntdqa, xmm14
TestModRmMemoryRev movntdqa, xmm15
%endif

TestSimdRev packusdw

; Row 3
TestSimdRev pmovzxbw
TestSimdRev pmovzxbd
TestSimdRev pmovzxbq
TestSimdRev pmovzxwd
TestSimdRev pmovzxwq
TestSimdRev pmovzxdq
TestSimdRev pcmpgtq
TestSimdRev pminsb
TestSimdRev pminsd
TestSimdRev pminuw
TestSimdRev pminud
TestSimdRev pmaxsb
TestSimdRev pmaxsd
TestSimdRev pmaxuw
TestSimdRev pmaxud

; Row 4
TestSimdRev pmulld
TestSimdRev phminposuw

; Row 0xf
TestModRmMemory movbe, ax
TestModRmMemory movbe, cx
TestModRmMemory movbe, dx
TestModRmMemory movbe, bx
TestModRmMemory movbe, bp
TestModRmMemory movbe, sp
TestModRmMemory movbe, si
TestModRmMemory movbe, di

TestModRmMemoryRev movbe, ax
TestModRmMemoryRev movbe, cx
TestModRmMemoryRev movbe, dx
TestModRmMemoryRev movbe, bx
TestModRmMemoryRev movbe, bp
TestModRmMemoryRev movbe, sp
TestModRmMemoryRev movbe, si
TestModRmMemoryRev movbe, di

%macro TestCrc 2
	%1 eax, %2
	%1 ecx, %2
	%1 edx, %2
	%1 ebx, %2
	%1 esp, %2
	%1 ebp, %2
	%1 esi, %2
	%1 edi, %2
%endmacro ; TestCrc

TestCrc crc32, ax
TestCrc crc32, cx
TestCrc crc32, dx
TestCrc crc32, bx
TestCrc crc32, bp
TestCrc crc32, sp
TestCrc crc32, si
TestCrc crc32, di

TestCrc crc32, ah
TestCrc crc32, ch
TestCrc crc32, dh
TestCrc crc32, bh
TestCrc crc32, al
TestCrc crc32, cl
TestCrc crc32, dl
TestCrc crc32, bl

TestSimdRev aesimc
TestSimdRev aesenc
TestSimdRev aesenclast
TestSimdRev aesdec
TestSimdRev aesdeclast

; 0f 3a

; Row 0
TestSimdImmThreeOperand roundps
TestSimdImmThreeOperand roundpd
TestSimdImmThreeOperand roundss
TestSimdImmThreeOperand roundsd
TestSimdImmThreeOperand blendps
TestSimdImmThreeOperand blendpd
TestSimdImmThreeOperand pblendw
TestMmxImm palignr
TestSimdImm palignr

; Row 1
%macro TestExt 1
TestModRmMemoryImmRev %1, xmm0, 1
TestGprSimdImm %1, eax
TestGprSimdImm %1, ecx
TestGprSimdImm %1, edx
TestGprSimdImm %1, ebx
TestGprSimdImm %1, esp
TestGprSimdImm %1, ebp
TestGprSimdImm %1, esi
TestGprSimdImm %1, edi
%endmacro ; TestExt

TestExt pextrb
TestExt pextrw
TestExt pextrd

TestExt extractps

; Row 2
%macro TestModRmMemoryImmAll 2
TestModRmMemoryImm %1, xmm0, %2
TestModRmMemoryImm %1, xmm1, %2
TestModRmMemoryImm %1, xmm2, %2
TestModRmMemoryImm %1, xmm3, %2
TestModRmMemoryImm %1, xmm4, %2
TestModRmMemoryImm %1, xmm5, %2
TestModRmMemoryImm %1, xmm6, %2
TestModRmMemoryImm %1, xmm7, %2
; TestModRmMemoryImm %1, xmm8, %2
; TestModRmMemoryImm %1, xmm9, %2
; TestModRmMemoryImm %1, xmm10, %2
; TestModRmMemoryImm %1, xmm11, %2
; TestModRmMemoryImm %1, xmm12, %2
; TestModRmMemoryImm %1, xmm13, %2
; TestModRmMemoryImm %1, xmm14, %2
; TestModRmMemoryImm %1, xmm15, %2
%endmacro ;TestModRmMemoryImmAll

%macro TestIns 1
TestModRmMemoryImmAll %1, 0
TestModRmMemoryImmAll %1, 1
TestModRmMemoryImmAll %1, 0xff
TestSimdGprImm %1, eax
TestSimdGprImm %1, ecx
TestSimdGprImm %1, edx
TestSimdGprImm %1, ebx
TestSimdGprImm %1, esp
TestSimdGprImm %1, ebp
TestSimdGprImm %1, esi
TestSimdGprImm %1, edi
%endmacro ; TestIns

TestIns pinsrb

TestModRmMemoryImmAll insertps, 0
TestModRmMemoryImmAll insertps, 1
TestModRmMemoryImmAll insertps, 0xff
TestSimdImm insertps

TestIns pinsrd

; Row 4
TestSimdImmThreeOperand dpps
TestSimdImmThreeOperand dppd
TestSimdImmThreeOperand mpsadbw
TestSimdImmThreeOperand pclmulqdq

; Row 6
TestSimdImmThreeOperand pcmpestrm
TestSimdImmThreeOperand pcmpestri
TestSimdImmThreeOperand pcmpestrm
TestSimdImmThreeOperand pcmpistri

; Row 8
%if ARCH <> 64
invept eax, [0xff88]
invvpid ecx, [0xf90]
invpcid edx, [0x990]
%else
invept rax, [0xff88]
invvpid rcx, [0xf90]
invpcid rdx, [0x990]
%endif

; Row 0xf
TestSimdImmThreeOperand aeskeygenassist

