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
TestMovnt movntdqa
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
; FIXME: Memory
TestGprSimdImm pextrb, eax
TestGprSimdImm pextrb, ecx
TestGprSimdImm pextrb, edx
TestGprSimdImm pextrb, ebx
TestGprSimdImm pextrb, esp
TestGprSimdImm pextrb, ebp
TestGprSimdImm pextrb, esi
TestGprSimdImm pextrb, edi

; FIXME: Memory
TestGprSimdImm pextrw, eax
TestGprSimdImm pextrw, ecx
TestGprSimdImm pextrw, edx
TestGprSimdImm pextrw, ebx
TestGprSimdImm pextrw, esp
TestGprSimdImm pextrw, ebp
TestGprSimdImm pextrw, esi
TestGprSimdImm pextrw, edi

; FIXME: Memory
TestGprSimdImm pextrd, eax
TestGprSimdImm pextrd, ecx
TestGprSimdImm pextrd, edx
TestGprSimdImm pextrd, ebx
TestGprSimdImm pextrd, esp
TestGprSimdImm pextrd, ebp
TestGprSimdImm pextrd, esi
TestGprSimdImm pextrd, edi

; FIXME: Memory
TestGprSimdImm extractps, eax
TestGprSimdImm extractps, ecx
TestGprSimdImm extractps, edx
TestGprSimdImm extractps, ebx
TestGprSimdImm extractps, esp
TestGprSimdImm extractps, ebp
TestGprSimdImm extractps, esi
TestGprSimdImm extractps, edi

; Row 2
; FIXME: Memory
TestSimdGprImm pinsrb, eax
TestSimdGprImm pinsrb, ecx
TestSimdGprImm pinsrb, edx
TestSimdGprImm pinsrb, ebx
TestSimdGprImm pinsrb, esp
TestSimdGprImm pinsrb, ebp
TestSimdGprImm pinsrb, esi
TestSimdGprImm pinsrb, edi

; FIXME: Memory
TestSimdImm insertps
TestSimdImm insertps
TestSimdImm insertps
TestSimdImm insertps
TestSimdImm insertps
TestSimdImm insertps
TestSimdImm insertps
TestSimdImm insertps

; FIXME: Memory
TestSimdGprImm pinsrd, eax
TestSimdGprImm pinsrd, ecx
TestSimdGprImm pinsrd, edx
TestSimdGprImm pinsrd, ebx
TestSimdGprImm pinsrd, esp
TestSimdGprImm pinsrd, ebp
TestSimdGprImm pinsrd, esi
TestSimdGprImm pinsrd, edi

; Row 4
; FIXME: Memory
TestSimdImm dpps
TestSimdImm dppd
TestSimdImm mpsadbw
TestSimdImm pclmulqdq

; Row 6
TestSimdImm pcmpestrm
TestSimdImm pcmpestri
TestSimdImm pcmpestrm
TestSimdImm pcmpistri


; Row 0xf
TestSimdImm aeskeygenassist

