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
