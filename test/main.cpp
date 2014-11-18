/*
	Copyright (c) 2013 Ryan Salsamendi

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
*/
#ifdef WIN32
#include <conio.h>
#include <Windows.h>
#else /* Linux */
#include <time.h>
#endif /* WIN32 */

#include "src/gtest-all.cc"
#include "salsasm.h"
#include "udis86.h"

using ::testing::TestWithParam;
using ::testing::Values;

#ifdef WIN32
#define INIT_PERF_CTR(name) \
	uint64_t time ## name = 0; \
	LARGE_INTEGER name ## Freq; \
	LARGE_INTEGER begin ## name ## Time, end ## name ## Time; \
	ASSERT_NE(QueryPerformanceFrequency(&name ## Freq), 0);

#define BEGIN_PERF_CTR(name) \
	begin ## name ## Time.QuadPart = 0; \
	end ## name ## Time.QuadPart = 0; \
	QueryPerformanceCounter(&begin ## name ## Time);

#define END_PERF_CTR(name) \
	QueryPerformanceCounter(&end ## name ## Time); \
	time ## name += (end ## name ## Time.QuadPart - begin ## name ## Time.QuadPart);

#define PRINT_PERF_CTR(name) \
	printf(# name ": %llu us\n", time ## name / (name ## Freq.QuadPart / 1000000));
#else /* Linux */
#define INIT_PERF_CTR(name) \
	uint64_t time ## name = 0; \
	timespec begin ## name ## ts; \
	timespec end ## name ## ts;

#define BEGIN_PERF_CTR(name) \
	memset(&begin ## name ## ts, 0, sizeof(timespec)); \
	clock_gettime(CLOCK_REALTIME, &begin ## name ## ts); \
	memset(&end ## name ## ts, 0, sizeof(timespec));

#define END_PERF_CTR(name) \
 	clock_gettime(CLOCK_REALTIME, &end ## name ## ts); \
 	time ## name += ((uint64_t)end ## name ## ts.tv_nsec - (uint64_t)begin ## name ## ts.tv_nsec) + (((uint64_t)end ## name ## ts.tv_sec * 1000000000) - ((uint64_t)begin ## name ## ts.tv_sec * 1000000000));

#define PRINT_PERF_CTR(name) \
	printf(# name ": %llu us\n", (long long unsigned int)time ## name / 1000);
#endif /* WIN32 */


static void PlatformInitWorkingDir()
{
#ifdef WIN32
	char* path = (char*)malloc(MAX_PATH);
	char* ptr;
	GetModuleFileNameA(NULL, path, MAX_PATH);
	ptr = strrchr(path, '\\');
	if ((ptr != NULL) && (strchr(ptr, '\\') != NULL))
	{
		*ptr = '\0';
		SetCurrentDirectoryA(path);
	}
	free(path);
#endif
}


struct OpcodeData
{
	// Represent the bytes for the current disassembly test
	uint8_t* opcodeBytes;
	size_t opcodeLen;
	size_t opcodeIndex;
};

class AsmTest
{
protected:
	// Objects declared here can be used by all tests in the test case for Foo.
	size_t GetOpcodeLength(OpcodeData& data) const;
	size_t GetOpcodeBytes(OpcodeData& data, uint8_t* const opcode, const size_t len);
	void SetOpcodeBytes(OpcodeData& data, const uint8_t* const opcode, const size_t len);

	static bool Fetch(void* ctxt, size_t len, uint8_t* result);
	static int FetchForUd86(struct ud* u);

	OpcodeData m_data;
	OpcodeData m_ud86Data;
public:
	virtual ~AsmTest()
	{
		// You can do clean-up work that doesn't throw exceptions here.
		if (m_data.opcodeBytes)
			free(m_data.opcodeBytes);
		if (m_ud86Data.opcodeBytes)
			free(m_ud86Data.opcodeBytes);
	}

	virtual void Init()
	{
		// Code here will be called immediately after the constructor (right
		// before each test).
		m_data.opcodeBytes = NULL;
		m_data.opcodeLen = 0;
		m_ud86Data.opcodeBytes = NULL;
		m_ud86Data.opcodeLen = 0;
	}
};

class AsmStandaloneTest: public AsmTest, public ::testing::Test
{
public:
	virtual void SetUp()
	{
		AsmTest::Init();
	};
};

// The fixture for testing class Foo.
class AsmFileTest: public AsmTest, public TestWithParam<const char*>
{
protected:
	const char* m_fileName;

public:
	// You can remove any or all of the following functions if its body
	// is empty.
	// If the constructor and destructor are not enough for setting up
	// and cleaning up each test, you can define the following methods:

	virtual void SetUp()
	{
		AsmTest::Init();
		m_fileName = GetParam();
	}

	virtual void TearDown()
	{
		// Code here will be called immediately after each test (right
		// before the destructor).
	}

	virtual void TestDisassemble(uint8_t bits);
};


size_t AsmTest::GetOpcodeLength(OpcodeData& data) const
{
	return data.opcodeLen;
}


size_t AsmTest::GetOpcodeBytes(OpcodeData& data, uint8_t* const opcode, const size_t len)
{
	const size_t bytesToCopy = len > data.opcodeLen ? data.opcodeLen : len;
	memcpy(opcode, &data.opcodeBytes[data.opcodeIndex], bytesToCopy);
	data.opcodeIndex += bytesToCopy;
	data.opcodeLen -= len;
	return bytesToCopy;
}


void AsmTest::SetOpcodeBytes(OpcodeData& data, const uint8_t* const opcode, const size_t len)
{
	if (data.opcodeBytes)
	{
		free(data.opcodeBytes);
		data.opcodeBytes = NULL;
	}
	data.opcodeBytes = (uint8_t*)malloc(len);
	memcpy(data.opcodeBytes, opcode, len);
	data.opcodeLen = len;
	data.opcodeIndex = 0;
}


int main(int argc, char **argv)
{
	int result;

	PlatformInitWorkingDir();

	::testing::InitGoogleTest(&argc, argv);
	result = RUN_ALL_TESTS();

#ifdef WIN32
	if (IsDebuggerPresent())
	{
		fprintf(stderr, "Press any key to continue...\n");
		while (!_kbhit());
	}
#endif /* WIN32 */

	return result;
}


bool AsmTest::Fetch(void* ctxt, size_t len, uint8_t* result)
{
	AsmTest* asmTest = (AsmTest*)ctxt;
	size_t opcodeLen;

	// Ensure there are enough bytes available
	opcodeLen = asmTest->GetOpcodeLength(asmTest->m_data);
	if (opcodeLen < len)
		return false;

	// Fetch the opcode
	asmTest->GetOpcodeBytes(asmTest->m_data, result, len);

	return true;
}


int AsmTest::FetchForUd86(struct ud* u)
{
	AsmTest* const asmTest = (AsmTest*)u->user_opaque_data;
	size_t opcodeLen;
	uint8_t result;

	// Ensure there are enough bytes available
	opcodeLen = asmTest->GetOpcodeLength(asmTest->m_ud86Data);
	if (opcodeLen < 1)
		return UD_EOI;

	// Fetch the opcode
	asmTest->GetOpcodeBytes(asmTest->m_ud86Data, &result, 1);

	return result;
}


#define TEST_ARITHMETIC_RM(operation, bytes, addrSize, operandSize, dest, component0, component1) \
{ \
	X86Instruction instr; \
	const size_t opcodeLen = sizeof(bytes); \
	SetOpcodeBytes(m_data, bytes, opcodeLen); \
	bool result = Disassemble ## addrSize(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr); \
	ASSERT_TRUE(result); \
	ASSERT_TRUE(instr.op == operation); \
	ASSERT_TRUE(instr.operandCount == 2); \
	ASSERT_TRUE(instr.operands[0].size == operandSize); \
	ASSERT_TRUE(instr.operands[0].operandType == dest); \
	ASSERT_TRUE(instr.operands[1].segment == X86_DS); \
	ASSERT_TRUE(instr.operands[1].components[0] == component0); \
	ASSERT_TRUE(instr.operands[1].components[1] == component1); \
	ASSERT_TRUE(instr.operands[1].operandType == X86_MEM); \
	ASSERT_TRUE(instr.operands[1].size == operandSize); \
}

#define TEST_ARITHMETIC_MR(operation, bytes, addrSize, operandSize, src, component0, component1) \
{ \
	X86Instruction instr; \
	const size_t opcodeLen = sizeof(bytes); \
	SetOpcodeBytes(m_data, bytes, opcodeLen); \
	bool result = Disassemble ## addrSize(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr); \
	ASSERT_TRUE(result); \
	ASSERT_TRUE(instr.op == operation); \
	ASSERT_TRUE(instr.operandCount == 2); \
	ASSERT_TRUE(instr.operands[0].size == operandSize); \
	ASSERT_TRUE(instr.operands[0].operandType == X86_MEM); \
	ASSERT_TRUE(instr.operands[0].segment == X86_DS); \
	ASSERT_TRUE(instr.operands[0].components[0] == component0); \
	ASSERT_TRUE(instr.operands[0].components[1] == component1); \
	ASSERT_TRUE(instr.operands[1].operandType == src); \
	ASSERT_TRUE(instr.operands[1].size == operandSize); \
}

#define TEST_ARITHMETIC_RR(operation, bytes, addrSize, operandSize, dest, src) \
{ \
	X86Instruction instr; \
	const size_t opcodeLen = sizeof(bytes); \
	SetOpcodeBytes(m_data, bytes, opcodeLen); \
	bool result = Disassemble ## addrSize(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr); \
	ASSERT_TRUE(result); \
	ASSERT_TRUE(instr.op == operation); \
	ASSERT_TRUE(instr.operandCount == 2); \
	ASSERT_TRUE(instr.operands[0].size == operandSize); \
	ASSERT_TRUE(instr.operands[0].operandType == dest); \
	ASSERT_TRUE(instr.operands[1].operandType == src); \
	ASSERT_TRUE(instr.operands[1].size == operandSize); \
}

#define TEST_ARITHMETIC_RI(operation, bytes, addrSize, operandSize, dest, imm) \
{ \
	X86Instruction instr; \
	const size_t opcodeLen = sizeof(bytes); \
	SetOpcodeBytes(m_data, bytes, opcodeLen); \
	bool result = Disassemble ## addrSize(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr); \
	ASSERT_TRUE(result); \
	ASSERT_TRUE(instr.op == operation); \
	ASSERT_TRUE(instr.operandCount == 2); \
	ASSERT_TRUE(instr.operands[0].size == operandSize); \
	ASSERT_TRUE(instr.operands[0].operandType == dest); \
	ASSERT_TRUE(instr.operands[0].segment == X86_NONE); \
	ASSERT_TRUE(instr.operands[1].operandType == X86_IMMEDIATE); \
	ASSERT_TRUE(instr.operands[1].immediate == imm); \
	ASSERT_TRUE(instr.operands[1].size == operandSize); \
	ASSERT_TRUE(instr.operands[1].segment == X86_NONE); \
}


TEST_F(AsmStandaloneTest, DisassemblePrimaryAdd)
{
	static const uint8_t addByteMemDest[] = {0, 0, 0};
	TEST_ARITHMETIC_MR(X86_ADD, addByteMemDest, 16, 1, X86_AL, X86_BX, X86_SI);
	// TEST_ARITHMETIC_MR(X86_ADD, addByteMemDest, 32, 1, X86_AL, X86_EAX, X86_NONE);

	static const uint8_t addByteRegDest[] = {0, 0xc0, 0};
	TEST_ARITHMETIC_RR(X86_ADD, addByteRegDest, 16, 1, X86_AL, X86_AL);
	// TEST_ARITHMETIC_RR(X86_ADD, addByteRegDest, 32, 1, X86_AL, X86_AL);
}


TEST_F(AsmStandaloneTest, DisassemblePastBugs)
{
	static const uint8_t addByteImm[] = {4, 1};
	TEST_ARITHMETIC_RI(X86_ADD, addByteImm, 16, 1, X86_AL, 1);

	static const uint8_t orByteMemDest[] = {8, 0};
	TEST_ARITHMETIC_MR(X86_OR, orByteMemDest, 16, 1, X86_AL, X86_BX, X86_SI);

	static const uint8_t das = 0x2f;
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, &das, 1);
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_DAS);
		ASSERT_TRUE(instr.length == 1);
	}

	static const uint8_t jmpByteImm[] = {0x70, 1};
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, jmpByteImm, sizeof(jmpByteImm));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_JO);
		ASSERT_TRUE(instr.length == 2);
	}

	static const uint8_t xchgRbxRax = 0x92;
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, &xchgRbxRax, sizeof(xchgRbxRax));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_XCHG);
		ASSERT_TRUE(instr.length == 1);
	}

	static const uint8_t movAxOffset[3] = {0xa2, 1, 0};
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, movAxOffset, sizeof(movAxOffset));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_MOV);
		ASSERT_TRUE(instr.length == 3);
	}

	static const uint8_t movsb = 0xa4;
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, &movsb, sizeof(movsb));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_MOVSB);
		ASSERT_TRUE(instr.length == 1);
	}

	static const uint8_t movsw = 0xa5;
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, &movsw, sizeof(movsw));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_MOVSW);
		ASSERT_TRUE(instr.length == 1);
	}

	static const uint8_t pushCs = 0x0e;
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, &pushCs, sizeof(pushCs));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_PUSH);
		ASSERT_TRUE(instr.length == 1);
		ASSERT_TRUE(instr.operands[0].operandType == X86_CS);
	}

	static const uint8_t decAx = 0x48;
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, &decAx, sizeof(decAx));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_DEC);
		ASSERT_TRUE(instr.length == 1);
		ASSERT_TRUE(instr.operands[0].operandType == X86_AX);
	}

	static const uint8_t group1Add[] = {0x81, 0xc1, 0xff, 0xff};
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, group1Add, sizeof(group1Add));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_ADD);
		ASSERT_TRUE(instr.length == 4);
		ASSERT_TRUE(instr.operands[0].operandType == X86_CX);
		ASSERT_TRUE(instr.operands[0].size == 2);
		ASSERT_TRUE(instr.operands[1].operandType == X86_IMMEDIATE);
		ASSERT_TRUE(instr.operands[1].immediate == (int64_t)0xffffffffffffffff);
	}

	static const uint8_t xchgAxCx = 0x91;
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, &xchgAxCx, sizeof(xchgAxCx));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_XCHG);
		ASSERT_TRUE(instr.length == 1);
		ASSERT_TRUE(instr.operands[0].operandType == X86_CX);
		ASSERT_TRUE(instr.operands[0].size == 2);
		ASSERT_TRUE(instr.operands[1].operandType == X86_AX);
		ASSERT_TRUE(instr.operands[1].size == 2);
	}

	static const uint8_t movModRm[2] = {0x88, 0x00};
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, movModRm, sizeof(movModRm));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_MOV);
		ASSERT_TRUE(instr.length == 2);
		ASSERT_TRUE(instr.operands[0].operandType == X86_MEM);
		ASSERT_TRUE(instr.operands[0].size == 1);
		ASSERT_TRUE(instr.operands[1].operandType == X86_AL);
		ASSERT_TRUE(instr.operands[1].size == 1);
	}

	static const uint8_t pushImmByte[2] = {0x6a, 0x00};
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, pushImmByte, sizeof(pushImmByte));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_PUSH);
		ASSERT_TRUE(instr.length == 2);
		ASSERT_TRUE(instr.operands[0].operandType == X86_IMMEDIATE);
		ASSERT_TRUE(instr.operands[0].immediate == 0);
		ASSERT_TRUE(instr.operands[0].size == 1);
	}

	static const uint8_t imul16[] = {0x6b, 0xd9, 0xff};
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, imul16, sizeof(imul16));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_IMUL);
		ASSERT_TRUE(instr.length == 3);
		ASSERT_TRUE(instr.operands[0].operandType == X86_BX);
		ASSERT_TRUE(instr.operands[0].immediate == 0);
		ASSERT_TRUE(instr.operands[0].size == 2);
		ASSERT_TRUE(instr.operands[1].operandType == X86_CX);
		ASSERT_TRUE(instr.operands[1].immediate == 0);
		ASSERT_TRUE(instr.operands[1].size == 2);
		ASSERT_TRUE(instr.operands[2].operandType == X86_IMMEDIATE);
		ASSERT_TRUE(instr.operands[2].immediate == SIGN_EXTEND64(0xff, 1));
		ASSERT_TRUE(instr.operands[2].size == 1);
	}

	static const uint8_t testImmByte[] = {0xa8, 0x01};
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, testImmByte, sizeof(testImmByte));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_TEST);
		ASSERT_TRUE(instr.length == 2);
		ASSERT_TRUE(instr.operands[0].operandType == X86_AL);
		ASSERT_TRUE(instr.operands[0].size == 1);
	}

	static const uint8_t movImmByte[] = {0xb8, 0x01, 0x01};
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, movImmByte, sizeof(movImmByte));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_MOV);
		ASSERT_TRUE(instr.length == 3);
		ASSERT_TRUE(instr.operands[0].operandType == X86_AX);
		ASSERT_TRUE(instr.operands[0].size == 2);
		X86OperandType src = (X86OperandType)(X86_AX + ((movImmByte[0] & 7) >> 3));
		ASSERT_TRUE(instr.operands[0].operandType == src);
		ASSERT_TRUE(instr.operands[0].size == 2);
	}

	static const uint8_t movSeg[] = {0x8e, 0x28};
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, movSeg, sizeof(movSeg));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_MOV);
		ASSERT_TRUE(instr.length == 2);
		ASSERT_TRUE(instr.operands[0].operandType == X86_GS);
		ASSERT_TRUE(instr.operands[0].size == 2);
		ASSERT_TRUE(instr.operands[1].operandType == X86_MEM);
		ASSERT_TRUE(instr.operands[1].size == 2);
	}

	static const uint8_t lea[] = {0x8d, 0x00};
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, lea, sizeof(lea));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_LEA);
		ASSERT_TRUE(instr.length == 2);
		ASSERT_TRUE(instr.operands[0].operandType == X86_AX);
		ASSERT_TRUE(instr.operands[0].size == 2);
		ASSERT_TRUE(instr.operands[1].operandType == X86_MEM);
		ASSERT_TRUE(instr.operands[1].size == 2);
		ASSERT_TRUE(instr.operands[1].components[0] == X86_BX);
		ASSERT_TRUE(instr.operands[1].components[1] == X86_SI);
	}

	static const uint8_t popf = 0x9d;
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, &popf, sizeof(popf));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_POPF);
		ASSERT_TRUE(instr.length == 1);
	}

	static const uint8_t stosw = 0xab;
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, &stosw, sizeof(stosw));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_STOSW);
		ASSERT_TRUE(instr.length == 1);
		ASSERT_TRUE(instr.operands[0].operandType == X86_MEM);
		ASSERT_TRUE(instr.operands[0].components[0] == X86_DI);
		ASSERT_TRUE(instr.operands[1].operandType == X86_AX);
	}

	static const uint8_t lodsb = 0xac;
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, &lodsb, sizeof(lodsb));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_LODSB);
		ASSERT_TRUE(instr.length == 1);
		ASSERT_TRUE(instr.operands[0].operandType == X86_AL);
		ASSERT_TRUE(instr.operands[1].operandType == X86_MEM);
		ASSERT_TRUE(instr.operands[1].components[0] == X86_SI);
	}

	static const uint8_t repMovsd[] = {0xf3, 0xa5};
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, repMovsd, sizeof(repMovsd));
		bool result = Disassemble32(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_MOVSD);
		ASSERT_TRUE(instr.length == 2);
		ASSERT_TRUE(instr.operands[0].operandType == X86_MEM);
		ASSERT_TRUE(instr.operands[0].components[0] == X86_EDI);
		ASSERT_TRUE(instr.operands[1].operandType == X86_MEM);
		ASSERT_TRUE(instr.operands[1].components[0] == X86_ESI);
	}

	static const uint8_t rolCl[] = {0xd2, 0xc0};
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, rolCl, sizeof(rolCl));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_ROL);
		ASSERT_TRUE(instr.length == 2);
		ASSERT_TRUE(instr.operands[1].operandType == X86_CL);
		ASSERT_TRUE(instr.operands[1].size == 1);
	}

	static const uint8_t inByte[] = {0xe4, 0xff};
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, inByte, sizeof(inByte));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_IN);
		ASSERT_TRUE(instr.length == 2);
		ASSERT_TRUE(instr.operandCount == 2);
		ASSERT_TRUE(instr.operands[1].size == 1);
		ASSERT_TRUE(instr.operands[1].operandType == X86_IMMEDIATE);
	}

	static const uint8_t faddMem[] = {0xd8, 0x01};
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, faddMem, sizeof(faddMem));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_FADD);
		ASSERT_TRUE(instr.length == 2);
		ASSERT_TRUE(instr.operandCount == 2);
		ASSERT_TRUE(instr.operands[1].size == 4);
		ASSERT_TRUE(instr.operands[1].operandType == X86_MEM);
		ASSERT_TRUE(instr.operands[0].operandType == X86_ST0);
	}

	static const uint8_t fldMem32[] = {0xd9, 0x00};
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, fldMem32, sizeof(fldMem32));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_FLD);
		ASSERT_TRUE(instr.operandCount == 2);
		ASSERT_TRUE(instr.length == 2);
		ASSERT_TRUE(instr.operands[1].size == 4);
		ASSERT_TRUE(instr.operands[1].operandType == X86_MEM);
		ASSERT_TRUE(instr.operands[0].operandType == X86_ST0);
	}

	static const uint8_t fldReg[] = {0xd9, 0xc0};
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, fldReg, sizeof(fldReg));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_FLD);
		ASSERT_TRUE(instr.length == 2);
		ASSERT_TRUE(instr.operandCount == 2);
		ASSERT_TRUE(instr.operands[0].size == 10);
		ASSERT_TRUE(instr.operands[1].size == 10);
		ASSERT_TRUE(instr.operands[1].operandType == X86_ST0);
		ASSERT_TRUE(instr.operands[0].operandType == X86_ST0);
	}

	static const uint8_t fnop[] = {0xd9, 0xd0};
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, fnop, sizeof(fnop));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_FNOP);
		ASSERT_TRUE(instr.length == 2);
		ASSERT_TRUE(instr.operandCount == 0);
	}

	static const uint8_t fiaddMem32[] = {0xda, 0x06, 0xff, 0xff};
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, fiaddMem32, sizeof(fiaddMem32));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_FIADD);
		ASSERT_TRUE(instr.length == 4);
		ASSERT_TRUE(instr.operandCount == 2);
		ASSERT_TRUE(instr.operands[0].operandType == X86_ST0);
		ASSERT_TRUE(instr.operands[1].operandType == X86_MEM);
		ASSERT_TRUE(instr.operands[0].size == 10);
		ASSERT_TRUE(instr.operands[1].size == 4);
	}

	static const uint8_t fild[] = {0xdb, 0x00};
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, fild, sizeof(fild));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_FILD);
		ASSERT_TRUE(instr.length == 2);
		ASSERT_TRUE(instr.operandCount == 2);
		ASSERT_TRUE(instr.operands[0].operandType == X86_ST0);
		ASSERT_TRUE(instr.operands[1].operandType == X86_MEM);
		ASSERT_TRUE(instr.operands[0].size == 10);
		ASSERT_TRUE(instr.operands[1].size == 4);
	}

	static const uint8_t fcmovnbe[] = {0xdb, 0xd0};
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, fcmovnbe, sizeof(fcmovnbe));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_FCMOVNBE);
		ASSERT_TRUE(instr.length == 2);
		ASSERT_TRUE(instr.operandCount == 2);
		ASSERT_TRUE(instr.operands[0].operandType == X86_ST0);
		ASSERT_TRUE(instr.operands[1].operandType == X86_ST0);
		ASSERT_TRUE(instr.operands[0].size == 10);
		ASSERT_TRUE(instr.operands[1].size == 10);
	}

	static const uint8_t fnstsw[] = {0xdd, 0x38};
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, fnstsw, sizeof(fnstsw));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_FNSTSW);
		ASSERT_TRUE(instr.length == 2);
		ASSERT_TRUE(instr.operandCount == 1);
		ASSERT_TRUE(instr.operands[0].operandType == X86_MEM);
		ASSERT_TRUE(instr.operands[0].size == 2);
	}

	static const uint8_t fild16[] = {0xdf, 0x00};
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, fild16, sizeof(fild16));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_FILD);
		ASSERT_TRUE(instr.length == 2);
		ASSERT_TRUE(instr.operandCount == 2);
		ASSERT_TRUE(instr.operands[0].operandType == X86_ST0);
		ASSERT_TRUE(instr.operands[0].size == 10);
		ASSERT_TRUE(instr.operands[1].operandType == X86_MEM);
		ASSERT_TRUE(instr.operands[1].size == 2);
	}

	static const uint8_t fnstswAx[] = {0xdf, 0xe0};
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, fnstswAx, sizeof(fnstswAx));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_FNSTSW);
		ASSERT_TRUE(instr.length == 2);
		ASSERT_TRUE(instr.operandCount == 1);
		ASSERT_TRUE(instr.operands[0].operandType == X86_AX);
		ASSERT_TRUE(instr.operands[0].size == 2);
	}

	static const uint8_t movups[] = {0x0f, 0x10, 0xc0};
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, movups, sizeof(movups));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_MOVUPS);
		ASSERT_TRUE(instr.length == 3);
		ASSERT_TRUE(instr.operandCount == 2);
		ASSERT_TRUE(instr.operands[0].operandType == X86_XMM0);
		ASSERT_TRUE(instr.operands[0].size == 16);
		ASSERT_TRUE(instr.operands[1].operandType == X86_XMM0);
		ASSERT_TRUE(instr.operands[1].size == 16);
	}

	static const uint8_t unpckhps[] = {0x0f, 0x15, 0xc0};
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, unpckhps, sizeof(unpckhps));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_UNPCKHPS);
		ASSERT_TRUE(instr.length == 3);
		ASSERT_TRUE(instr.operandCount == 2);
		ASSERT_TRUE(instr.operands[0].operandType == X86_XMM0);
		ASSERT_TRUE(instr.operands[0].size == 16);
		ASSERT_TRUE(instr.operands[1].operandType == X86_XMM0);
		ASSERT_TRUE(instr.operands[1].size == 16);
	}

	static const uint8_t unpcklps[] = {0x0f, 0x14, 0xc1};
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, unpcklps, sizeof(unpcklps));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_UNPCKLPS);
		ASSERT_TRUE(instr.length == 3);
		ASSERT_TRUE(instr.operandCount == 2);
		ASSERT_TRUE(instr.operands[0].operandType == X86_XMM0);
		ASSERT_TRUE(instr.operands[0].size == 16);
		ASSERT_TRUE(instr.operands[1].operandType == X86_XMM1);
		ASSERT_TRUE(instr.operands[1].size == 16);
	}

	static const uint8_t movControlReg[] = {0x0f, 0x22, 0xc0};
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, movControlReg, sizeof(movControlReg));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_MOV);
		ASSERT_TRUE(instr.length == 3);
		ASSERT_TRUE(instr.operandCount == 2);
		ASSERT_TRUE(instr.operands[0].operandType == X86_CR0);
		ASSERT_TRUE(instr.operands[0].size == 4);
		ASSERT_TRUE(instr.operands[1].operandType == X86_EAX);
		ASSERT_TRUE(instr.operands[1].size == 4);
	}

	static const uint8_t punpcklbwSse[] = {0x66, 0x0f, 0x60, 0xc8};
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, punpcklbwSse, sizeof(punpcklbwSse));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_PUNPCKLBW);
		ASSERT_TRUE(instr.length == 4);
		ASSERT_TRUE(instr.operandCount == 2);
		ASSERT_TRUE(instr.operands[0].operandType == X86_XMM1);
		ASSERT_TRUE(instr.operands[0].size == 8);
		ASSERT_TRUE(instr.operands[1].operandType == X86_XMM0);
		ASSERT_TRUE(instr.operands[1].size == 8);
	}

	static const uint8_t movqMmx[] = {0x0f, 0x7f, 0x00};
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, movqMmx, sizeof(movqMmx));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_MOVQ);
		ASSERT_TRUE(instr.length == sizeof(movqMmx));
		ASSERT_TRUE(instr.operandCount == 2);
		ASSERT_TRUE(instr.operands[0].operandType == X86_MEM);
		ASSERT_TRUE(instr.operands[0].components[0] == X86_BX);
		ASSERT_TRUE(instr.operands[0].components[1] == X86_SI);
		ASSERT_TRUE(instr.operands[0].size == 8);
		ASSERT_TRUE(instr.operands[1].operandType == X86_MM0);
		ASSERT_TRUE(instr.operands[1].size == 8);
	}

	static const uint8_t movCr0[] = {0x0f, 0x22, 0xc0};
	{
		X86Instruction instr;
		SetOpcodeBytes(m_data, movCr0, sizeof(movCr0));
		bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_MOV);
		ASSERT_TRUE(instr.length == sizeof(movCr0));
		ASSERT_TRUE(instr.operands[0].operandType == X86_CR0);
		ASSERT_TRUE(instr.operands[0].size == 4);
		ASSERT_TRUE(instr.operands[1].operandType == X86_EAX);
		ASSERT_TRUE(instr.operands[1].size == 4);
	}
}

#define TEST_INVALID_IN_64BIT_MODE(name, opcode) \
TEST_F(AsmStandaloneTest, Test_ ## name ## _InvalidIn64BitMode) \
{ \
	static const uint8_t opcodeByte[] = {opcode, 0xc0, 0x01}; \
	X86Instruction instr; \
	SetOpcodeBytes(m_data, opcodeByte, sizeof(opcodeByte)); \
	bool result = Disassemble64(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr); \
	ASSERT_FALSE(result); \
}

TEST_INVALID_IN_64BIT_MODE(PUSH_ES, 0x06);
TEST_INVALID_IN_64BIT_MODE(POP_ES, 0x07);
TEST_INVALID_IN_64BIT_MODE(PUSH_SS, 0x07);
TEST_INVALID_IN_64BIT_MODE(POP_SS, 0x17);
TEST_INVALID_IN_64BIT_MODE(PUSH_CS, 0x0e);
TEST_INVALID_IN_64BIT_MODE(PUSH_DS, 0x1e);
TEST_INVALID_IN_64BIT_MODE(POP_DS, 0x1f);

TEST_INVALID_IN_64BIT_MODE(DAA, 0x27);
TEST_INVALID_IN_64BIT_MODE(AAA, 0x37);
TEST_INVALID_IN_64BIT_MODE(DAS, 0xf7);
TEST_INVALID_IN_64BIT_MODE(AAS, 0xf7);

TEST_INVALID_IN_64BIT_MODE(PUSHA, 0x60);
TEST_INVALID_IN_64BIT_MODE(POPA, 0x61);
TEST_INVALID_IN_64BIT_MODE(BOUND, 0x62);

TEST_INVALID_IN_64BIT_MODE(GROUP1_COL2, 0x82);
TEST_INVALID_IN_64BIT_MODE(CALLF, 0x9a);

TEST_INVALID_IN_64BIT_MODE(INTO, 0xce);

TEST_INVALID_IN_64BIT_MODE(AAM, 0xd4);
TEST_INVALID_IN_64BIT_MODE(AAD, 0xd5);

TEST_INVALID_IN_64BIT_MODE(JMPF, 0xea);

// Undocumented
TEST_INVALID_IN_64BIT_MODE(SALC, 0xd6);

#define TEST_INVALID_IN_64BIT_MODE_TWO_BYTE(name, opcode) \
TEST_F(AsmStandaloneTest, Test_ ## name ## _InvalidIn64BitMode) \
{ \
	static const uint8_t opcodeByte[] = {0x0f, opcode, 0xc0, 0x01}; \
	X86Instruction instr; \
	SetOpcodeBytes(m_data, opcodeByte, sizeof(opcodeByte)); \
	bool result = Disassemble64(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr); \
	ASSERT_FALSE(result); \
}

TEST_INVALID_IN_64BIT_MODE_TWO_BYTE(SYSENTER, 0x34);
TEST_INVALID_IN_64BIT_MODE_TWO_BYTE(SYSEXIT, 0x35);

#define TEST_ONLY_VALID_IN_64BIT_MODE_TWO_BYTE(name, opcode) \
TEST_F(AsmStandaloneTest, Test_ ## name ## _OnlyValidIn64BitMode) \
{ \
	static const uint8_t opcodeByte[] = {0x0f, opcode, 0xc0, 0x01}; \
	X86Instruction instr; \
	SetOpcodeBytes(m_data, opcodeByte, sizeof(opcodeByte)); \
	bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr); \
	ASSERT_FALSE(result); \
	SetOpcodeBytes(m_data, opcodeByte, sizeof(opcodeByte)); \
	result = Disassemble32(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr); \
	ASSERT_FALSE(result); \
}

TEST_ONLY_VALID_IN_64BIT_MODE_TWO_BYTE(SYSCALL, 0x05);
TEST_ONLY_VALID_IN_64BIT_MODE_TWO_BYTE(SYSRET, 0x07);

TEST_F(AsmStandaloneTest, Test_SWAPGS_OnlyValidIn64BitMode) \
{ \
	static const uint8_t opcodeByte[] = {0x0f, 0x01, 0xc8, 0xf8}; \
	X86Instruction instr; \
	SetOpcodeBytes(m_data, opcodeByte, sizeof(opcodeByte)); \
	bool result = Disassemble16(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr); \
	ASSERT_FALSE(!result); \
	SetOpcodeBytes(m_data, opcodeByte, sizeof(opcodeByte)); \
	result = Disassemble32(0, AsmTest::Fetch, static_cast<AsmTest*>(this), &instr); \
	ASSERT_FALSE(!result); \
}

static bool SkipOperationCheck(X86Operation op1, enum ud_mnemonic_code op2)
{
	if ((op1 == X86_POPF) && (op2 == UD_Iinvalid))
		return true;
	if ((op1 == X86_OUTSD) && (op2 == UD_Iinvalid))
		return true;
	if ((op1 == X86_INVALID) && (op2 == UD_Ipunpckhwd))
		return true;
	if ((op1 == X86_INSD) && (op2 == UD_Iinvalid))
		return true;
	if ((op1 == X86_VMCALL) && (op2 == UD_Iinvalid))
		return true;
	if ((op1 == X86_VMLAUNCH) && (op2 == UD_Iinvalid))
		return true;
	if ((op1 == X86_VMRESUME) && (op2 == UD_Iinvalid))
		return true;
	if ((op1 == X86_VMXOFF) && (op2 == UD_Iinvalid))
		return true;
	if ((op1 == X86_VMPTRLD) && (op2 == UD_Iinvalid))
		return true;
	if ((op1 == X86_VMCLEAR) && (op2 == UD_Iinvalid))
		return true;
	if ((op1 == X86_VMPTRST) && (op2 == UD_Iinvalid))
		return true;
	if ((op1 == X86_VMXON) && (op2 == UD_Iinvalid))
		return true;
	if ((op1 == X86_INVEPT) && (op2 == UD_Iinvalid))
		return true;
	if ((op1 == X86_RDSEED) && (op2 == UD_Iinvalid))
		return true;

	switch (op2)
	{
	// Not yet implemented
	case UD_Igetsec:
	case UD_Ipabsb:
		return true;
	// udis86 does not fail these in 16/32bit mode. Probably a bug.
	case UD_Isyscall:
	case UD_Isysret:
		return true;
	case UD_Ilock:
		return true;
	case UD_Irep:
	case UD_Irepne:
		return true;

	// VEX instructions not yet implemented.
	case UD_Ivpextrw:
	case UD_Ivpcmpgtd:
	case UD_Ivmovss:
		return true;
	default:
		break;
	}

	switch (op1)
	{
	// Oracle and I don't agree on Group16 NOPs, but neither do Intel and AMD
	case X86_NOP:
		return true;
	// Not yet implemented by ud86
	case X86_TZCNT:
	case X86_LZCNT:
	case X86_INSERTQ:
	case X86_EXTRQ:
	case X86_MOVMSKPS:
	case X86_ADX:
	case X86_AMX:
	case X86_VBLENDPD:
	case X86_VBLENDPS:
	case X86_VBLENDVPD:
	case X86_VBLENDVPS:
	case X86_VDPPD:
	case X86_VDPPS:
	case X86_VEXTRACTPS:
	case X86_VINSERTPS:
	case X86_VMOVNTDQA:
	case X86_VMPSADBW:
	case X86_VPACKUDSW:
	case X86_VBLENDVB:
	case X86_VPBLENDW:
	case X86_VPCMPEQQ:
	case X86_VPEXTRB:
	case X86_VPEXTRD:
	case X86_VPEXTRW:
	case X86_VPHMINPOSUW:
	case X86_VPINSRB:
	case X86_VPINSRD:
	case X86_VPINSRQ:
	case X86_VPMAXSB:
	case X86_VPMAXSD:
	case X86_VMPAXUD:
	case X86_VPMAXUW:
	case X86_VPMINSB:
	case X86_VPMINSD:
	case X86_VPMINUD:
	case X86_VPMINUW:
	case X86_VPMOVSXBD:
	case X86_VPMOVSXBQ:
	case X86_VPMOVSXBW:
	case X86_VPMOVSXWD:
	case X86_VPMOVSXWQ:
	case X86_VPMOVSXDQ:
	case X86_VPMOVZXBD:
	case X86_VPMOVZXBQ:
	case X86_VPMOVZXBW:
	case X86_VPMOVZXWD:
	case X86_VPMOVZXWQ:
	case X86_VPMOVZXDQ:
	case X86_VPMULDQ:
	case X86_VPMULLD:
	case X86_VPTEST:
	case X86_VROUNDPD:
	case X86_VROUNDPS:
	case X86_VROUNDSD:
	case X86_VROUNDSS:
	case X86_VPCMPESTRI:
	case X86_VPCMPESTRM:
	case X86_VPCMPGTQ:
	case X86_VPCMPISTRI:
	case X86_VPCMPISTRM:
	case X86_VAESDEC:
	case X86_VAESDECLAST:
	case X86_VAESENC:
	case X86_VAESENCLAST:
	case X86_VAESIMC:
	case X86_VAESKEYGENASSIST:
	case X86_VPABSB:
	case X86_VPABSD:
	case X86_VPABSW:
	case X86_VPALIGNR:
	case X86_VPAHADD:
	case X86_VPHADDW:
	case X86_VPHADDSW:
	case X86_VPHSUBD:
	case X86_VPHSUBW:
	case X86_VPHSUBSW:
	case X86_VPMADDUBSW:
	case X86_VPMULHRSW:
	case X86_VPSHUFB:
	case X86_VPSIGNB:
	case X86_VPSIGND:
	case X86_VPSIGNW:
	case X86_VADDSUBPD:
	case X86_VADDSUBPS:
	case X86_VHADDPD:
	case X86_VHADDPS:
	case X86_VHSUBPD:
	case X86_VHSUBPS:
	case X86_VLDDQU:
	case X86_VMOVDDUP:
	case X86_VMOVHLPS:
	case X86_VMOVSHDUP:
	case X86_VMOVSLDUP:
	case X86_VADDPD:
	case X86_VADDSD:
	case X86_VANPD:
	case X86_VANDNPD:
	case X86_VCMPPD:
	case X86_VCMPSD:
	case X86_VCOMISD:
	case X86_VCVTDQ2PD:
	case X86_VCVTDQ2PS:
	case X86_VCVTPD2DQ:
	case X86_VCVTPD2PS:
	case X86_VCVTPS2DQ:
	case X86_VCVTPS2PD:
	case X86_VCVTSD2SI:
	case X86_VCVTSD2SS:
	case X86_VCVTSI2SD:
	case X86_VCVTSS2SD:
	case X86_VCVTTPD2DQ:
	case X86_VCVTTPS2DQ:
	case X86_VCVTTSD2SI:
	case X86_VDIVPD:
	case X86_VDIVSD:
	case X86_VMASKMOVDQU:
	case X86_VMAXPD:
	case X86_VMAXSD:
	case X86_VMINPD:
	case X86_VMINSD:
	case X86_VMOVAPD:
	case X86_VMOVD:
	case X86_VMOVQ:
	case X86_VMOVDQA:
	case X86_VMOVDQU:
	case X86_VMOVHPD:
	case X86_VMOVLPD:
	case X86_VMOVMSKPD:
	case X86_VMOVNTDQ:
	case X86_VMOVNTPD:
	case X86_VMOVSD:
	case X86_VMOVUPD:
	case X86_VMULPD:
	case X86_VMULSD:
	case X86_VORPD:
	case X86_VPACKSSWB:
	case X86_VPACKSSDW:
	case X86_VPACKUSWB:
	case X86_VPADDB:
	case X86_VPADDW:
	case X86_VPADDD:
	case X86_VPADDQ:
	case X86_VPADDSB:
	case X86_VPADDSW:
	case X86_VPADDUSB:
	case X86_VPADDUSW:
	case X86_VPAND:
	case X86_VPANDN:
	case X86_VPAVGB:
	case X86_VPAVGW:
	case X86_VPCMPEQB:
	case X86_VPCMPEQW:
	case X86_VPCMPEQD:
	case X86_VPCMPGTB:
	case X86_VPCMPGTW:
	case X86_VPCMPGTD:
	case X86_VPEXTSRW:
	case X86_VPMADDWD:
	case X86_VPMAXSW:
	case X86_VPMAXUB:
	case X86_VPMINSW:
	case X86_VPMINUB:
	case X86_VPMOVMSKB:
	case X86_VPMULHUW:
	case X86_VPMULHW:
	case X86_VPMULLW:
	case X86_VPMULUDQ:
	case X86_VPOR:
	case X86_VPSADBW:
	case X86_VPSHUFD:
	case X86_VPSHUFHW:
	case X86_VPSHUFLW:
	case X86_VPSLLDQ:
	case X86_VPSLLW:
	case X86_VPSLLD:
	case X86_VPSLLQ:
	case X86_VPSRAW:
	case X86_VPSRAD:
	case X86_VPSRLDQ:
	case X86_VPSRLW:
	case X86_VPSRLD:
	case X86_VPSRLQ:
	case X86_VPSUBB:
	case X86_VPSUBW:
	case X86_VPSUBD:
	case X86_VPSQUBQ:
	case X86_VPSUBSB:
	case X86_VPSUBSW:
	case X86_VPSUBUSB:
	case X86_VPSUBUSW:
	case X86_VPUNPCKHBW:
	case X86_VPUNPCKHWD:
	case X86_VPUNPCKHDQ:
	case X86_VPUNPCKHQDQ:
	case X86_VPUNPCKLBW:
	case X86_VPUNPCKLWD:
	case X86_VPUNPCKLDQ:
	case X86_VPUNCKLQDQ:
	case X86_VPXOR:
	case X86_VSHUFPD:
	case X86_VSQRTPD:
	case X86_VSQRTSD:
	case X86_VSUBPD:
	case X86_VSUBSD:
	case X86_VUCOMISD:
	case X86_VUNPCKHPD:
	case X86_VUNPCKHPS:
	case X86_VUNPCKLPD:
	case X86_VUNPCKLPS:
	case X86_VXORPD:
	case X86_VADDPS:
	case X86_VADDSS:
	case X86_VANDPS:
	case X86_VANDNPS:
	case X86_VCMPPS:
	case X86_VCMPSS:
	case X86_VCOMISS:
	case X86_VCVTSI2SS:
	case X86_VCVTSS2SI:
	case X86_VCVTTSS2SI:
	case X86_VDIVPS:
	case X86_VLDMXCSR:
	case X86_VMAXPS:
	case X86_VMAXSS:
	case X86_VMINPS:
	case X86_VMINSS:
	case X86_VMOVAPS:
	case X86_VMOVHPS:
	case X86_VMOVLHPS:
	case X86_VMOVLPS:
	case X86_VMOVMSKPS:
	case X86_VMOVNTPS:
	case X86_VMOVSS:
	case X86_VMOVUPS:
	case X86_VMULPS:
	case X86_VMULSS:
	case X86_VORPS:
	case X86_VRCPPS:
	case X86_VRCPSS:
	case X86_VRSQRTPS:
	case X86_VRSQRTSS:
	case X86_VSQRTPS:
	case X86_VSQRTSS:
	case X86_VSTMXCSR:
	case X86_VSUBPS:
	case X86_VSUBSS:
	case X86_VUCOMISS:
	case X86_VXORPS:
	case X86_VBROADCAST:
	case X86_VEXTRACTF128:
	case X86_VINSERTF128:
	case X86_VPERMILPD:
	case X86_VPERMILPS:
	case X86_VPERM2F128:
	case X86_VTESTPD:
	case X86_VTESTPS:
	case X86_XABORT:
	case X86_XBEGIN:
		return true;
	case X86_INSERTPS:
		// AMD supports reg form of RM, Intel does not say it does in opcode table
		// all other refs in Intel docs say it does. Probably a bug, should be reported to ud86
		return true;
	default:
		break;
	}

	return false;
}


static bool CompareOperation(X86Operation op1, enum ud_mnemonic_code op2)
{
	switch (op1)
	{
	case X86_INVALID:
		return (op2 == UD_Iinvalid);
	case X86_AAA:
		return (op2 == UD_Iaaa);
	case X86_AAD:
		return (op2 == UD_Iaad);
	case X86_AAM:
		return (op2 == UD_Iaam);
	case X86_AAS:
		return (op2 == UD_Iaas);
	case X86_AESIMC:
		return (op2 == UD_Iaesimc);
	case X86_AESENC:
		return (op2 == UD_Iaesenc);
	case X86_AESENCLAST:
		return (op2 == UD_Iaesenclast);
	case X86_AESDEC:
		return (op2 == UD_Iaesdec);
	case X86_AESDECLAST:
		return (op2 == UD_Iaesdeclast);
	case X86_AESKEYGENASSIST:
		return (op2 == UD_Iaeskeygenassist);
	case X86_ADC:
		return (op2 == UD_Iadc);
	case X86_ADD:
		return (op2 == UD_Iadd);
	case X86_ADDPD:
		return (op2 == UD_Iaddpd);
	case X86_ADDPS:
		return (op2 == UD_Iaddps);
	case X86_ADDSD:
		return (op2 == UD_Iaddsd);
	case X86_ADDSS:
		return (op2 == UD_Iaddss);
	case X86_ADDSUBPD:
		return (op2 == UD_Iaddsubpd);
	case X86_ADDSUBPS:
		return (op2 == UD_Iaddsubps);
	case X86_ADX:
		// FIXME
		return false;
	case X86_AMX:
		// FIXME
		return false;
	case X86_AND:
		return (op2 == UD_Iand);
	case X86_ANDNPD:
		return (op2 == UD_Iandnpd);
	case X86_ANDNPS:
		return (op2 == UD_Iandnps);
	case X86_ANDPD:
		return (op2 == UD_Iandpd);
	case X86_ANDPS:
		return (op2 == UD_Iandps);
	case X86_ARPL:
		return (op2 == UD_Iarpl);
	case X86_BLENDPD:
		return (op2 == UD_Iblendpd);
	case X86_BLENDPS:
		return (op2 == UD_Iblendps);
	case X86_BLENDVPD:
		return (op2 == UD_Iblendvpd);
	case X86_BLENDVPS:
		return (op2 == UD_Iblendvps);
	case X86_BOUND:
		return (op2 == UD_Ibound);
	case X86_BSF:
		return (op2 == UD_Ibsf);
	case X86_BSR:
		return (op2 == UD_Ibsr);
	case X86_BSWAP:
		return (op2 == UD_Ibswap);
	case X86_BT:
		return (op2 == UD_Ibt);
	case X86_BTC:
		return (op2 == UD_Ibtc);
	case X86_BTR:
		return (op2 == UD_Ibtr);
	case X86_BTS:
		return (op2 == UD_Ibts);
	case X86_CALLN:
		return (op2 == UD_Icall);
	case X86_CALLF:
		return (op2 == UD_Icall);
	case X86_CBW:
		return (op2 == UD_Icbw);
	case X86_CWDE:
		return (op2 == UD_Icwde);
	case X86_CDQE:
		return (op2 == UD_Icdqe);
	case X86_CLC:
		return (op2 == UD_Iclc);
	case X86_CLD:
		return (op2 == UD_Icld);
	case X86_CLFLUSH:
		return (op2 == UD_Iclflush);
	case X86_CLGI:
		return (op2 == UD_Iclgi);
	case X86_CLI:
		return (op2 == UD_Icli);
	case X86_CLTS:
		return (op2 == UD_Iclts);
	case X86_CMC:
		return (op2 == UD_Icmc);
	case X86_CMOVB:
	case X86_CMOVNAE:
		return (op2 == UD_Icmovb);
	case X86_CMOVC:
		return false;
	case X86_CMOVBE:
	case X86_CMOVNA: // Fall through
		return (op2 == UD_Icmovbe);
	case X86_CMOVL:
	case X86_CMOVNGE: // Fall through
		return (op2 == UD_Icmovl);
	case X86_CMOVLE:
		return (op2 == UD_Icmovle);
	case X86_CMOVNG:
		return false;
	case X86_CMOVNB:
	case X86_CMOVAE:
		return (op2 == UD_Icmovae);
	case X86_CMOVNC:
		return false;
	case X86_CMOVNBE:
	case X86_CMOVA:
		return (op2 == UD_Icmova);
	case X86_CMOVNL:
	case X86_CMOVGE:
		return (op2 == UD_Icmovge);
	case X86_CMOVNLE:
	case X86_CMOVG:
		return (op2 == UD_Icmovg);
	case X86_CMOVNO:
		return (op2 == UD_Icmovno);
	case X86_CMOVNP:
		return (op2 == UD_Icmovnp);
	case X86_CMOVPO:
		return false;
	case X86_CMOVNS:
		return (op2 == UD_Icmovns);
	case X86_CMOVNZ:
		return (op2 == UD_Icmovnz);
	case X86_CMOVNE:
		return false;
	case X86_CMOVO:
		return (op2 == UD_Icmovo);
	case X86_CMOVP:
		return (op2 == UD_Icmovp);
	case X86_CMOVPE:
		return false;
	case X86_CMOVS:
		return (op2 == UD_Icmovs);
	case X86_CMOVZ:
		return (op2 == UD_Icmovz);
	case X86_CMOVE:
		return false;
	case X86_CMP:
		return (op2 == UD_Icmp);
	case X86_CMPPD:
		return (op2 == UD_Icmppd);
	case X86_CMPPS:
		return (op2 == UD_Icmpps);
	case X86_CMPS:
		return false;
	case X86_CMPSB:
		return (op2 == UD_Icmpsb);
	case X86_CMPSW:
		return (op2 == UD_Icmpsw);
	case X86_CMPSD:
		return (op2 == UD_Icmpsd);
	case X86_CMPSQ:
		return (op2 == UD_Icmpsq);
	case X86_CMPSS:
		return (op2 == UD_Icmpss);
	case X86_CMPXCHG:
		return (op2 == UD_Icmpxchg);
	case X86_CMPXCHG8B:
		return (op2 == UD_Icmpxchg8b);
	case X86_CMPXCHG16B:
		return (op2 == UD_Icmpxchg16b);
	case X86_COMISD:
		return (op2 == UD_Icomisd);
	case X86_COMISS:
		return (op2 == UD_Icomiss);
	case X86_CPUID:
		return (op2 == UD_Icpuid);
	case X86_CRC32:
		return (op2 == UD_Icrc32);
	case X86_CVTDQ2PD:
		return (op2 == UD_Icvtdq2pd);
	case X86_CVTDQ2PS:
		return (op2 == UD_Icvtdq2ps);
	case X86_CVTPD2DQ:
		return (op2 == UD_Icvtpd2dq);
	case X86_CVTPD2PI:
		return (op2 == UD_Icvtpd2pi);
	case X86_CVTPD2PS:
		return (op2 == UD_Icvtpd2ps);
	case X86_CVTPI2PD:
		return (op2 == UD_Icvtpi2pd);
	case X86_CVTPI2PS:
		return (op2 == UD_Icvtpi2ps);
	case X86_CVTPS2DQ:
		return (op2 == UD_Icvtps2dq);
	case X86_CVTPS2PD:
		return (op2 == UD_Icvtps2pd);
	case X86_CVTPS2PI:
		return (op2 == UD_Icvtps2pi);
	case X86_CVTSD2SI:
		return (op2 == UD_Icvtsd2si);
	case X86_CVTSD2SS:
		return (op2 == UD_Icvtsd2ss);
	case X86_CVTSI2SD:
		return (op2 == UD_Icvtsi2sd);
	case X86_CVTSI2SS:
		return (op2 == UD_Icvtsi2ss);
	case X86_CVTSS2SD:
		return (op2 == UD_Icvtss2sd);
	case X86_CVTSS2SI:
		return (op2 == UD_Icvtss2si);
	case X86_CVTTPD2DQ:
		return (op2 == UD_Icvttpd2dq);
	case X86_CVTTPD2PI:
		return (op2 == UD_Icvttpd2pi);
	case X86_CVTTPS2DQ:
		return (op2 == UD_Icvttps2dq);
	case X86_CVTTPS2PI:
		return (op2 == UD_Icvttps2pi);
	case X86_CVTTSD2SI:
		return (op2 == UD_Icvttsd2si);
	case X86_CVTTSS2SI:
		return (op2 == UD_Icvttss2si);
	case X86_CWD:
		return (op2 == UD_Icwd);
	case X86_CDQ:
		return (op2 == UD_Icdq);
	case X86_CQO:
		return (op2 == UD_Icqo);
	case X86_DAA:
		return (op2 == UD_Idaa);
	case X86_DAS:
		return (op2 == UD_Idas);
	case X86_DEC:
		return (op2 == UD_Idec);
	case X86_DIV:
		return (op2 == UD_Idiv);
	case X86_DIVPD:
		return (op2 == UD_Idivpd);
	case X86_DIVPS:
		return (op2 == UD_Idivps);
	case X86_DIVSD:
		return (op2 == UD_Idivsd);
	case X86_DIVSS:
		return (op2 == UD_Idivss);
	case X86_DPPD:
		return (op2 == UD_Idppd);
	case X86_DPPS:
		return (op2 == UD_Idpps);
	case X86_EMMS:
		return (op2 == UD_Iemms);
	case X86_ENTER:
		return (op2 == UD_Ienter);
	case X86_EXTRACTPS:
		return (op2 == UD_Iextractps);
	case X86_F2XM1:
		return (op2 == UD_If2xm1);
	case X86_FABS:
		return (op2 == UD_Ifabs);
	case X86_FADD:
		return (op2 == UD_Ifadd);
	case X86_FADDP:
		return (op2 == UD_Ifaddp);
	case X86_FBLD:
		return (op2 == UD_Ifbld);
	case X86_FBSTP:
		return (op2 == UD_Ifbstp);
	case X86_FCHS:
		return (op2 == UD_Ifchs);
	case X86_FCLEX:
		return (op2 == UD_Ifclex);
	case X86_FCMOVB:
		return (op2 == UD_Ifcmovb);
	case X86_FCMOVBE:
		return (op2 == UD_Ifcmovbe);
	case X86_FCMOVE:
		return (op2 == UD_Ifcmove);
	case X86_FCMOVNB:
		return (op2 == UD_Ifcmovnb);
	case X86_FCMOVNBE:
		return (op2 == UD_Ifcmovnbe);
	case X86_FCMOVNE:
		return (op2 == UD_Ifcmovne);
	case X86_FCMOVNU:
		return (op2 == UD_Ifcmovnu);
	case X86_FCMOVU:
		return (op2 == UD_Ifcmovu);
	case X86_FCOM:
		return (op2 == UD_Ifcom);
	case X86_FCOM2:
		return (op2 == UD_Ifcom2);
	case X86_FCOMI:
		return (op2 == UD_Ifcomi);
	case X86_FCOMIP:
		return (op2 == UD_Ifcomip);
	case X86_FCOMP:
		return (op2 == UD_Ifcomp);
	case X86_FCOMP3:
		return (op2 == UD_Ifcomp3);
	case X86_FCOMP5:
		return (op2 == UD_Ifcomp5);
	case X86_FCOMPP:
		return (op2 == UD_Ifcompp);
	case X86_FCOS:
		return (op2 == UD_Ifcos);
	case X86_FDECSTP:
		return (op2 == UD_Ifdecstp);
	case X86_FDIV:
		return (op2 == UD_Ifdiv);
	case X86_FDIVP:
		return (op2 == UD_Ifdivp);
	case X86_FDIVR:
		return (op2 == UD_Ifdivr);
	case X86_FDIVRP:
		return (op2 == UD_Ifdivrp);
	case X86_FEMMS:
		return (op2 == UD_Ifemms);
	case X86_FFREE:
		return (op2 == UD_Iffree);
	case X86_FFREEP:
		return (op2 == UD_Iffreep);
	case X86_FIADD:
		return (op2 == UD_Ifiadd);
	case X86_FICOM:
		return (op2 == UD_Ificom);
	case X86_FICOMP:
		return (op2 == UD_Ificomp);
	case X86_FIDIV:
		return (op2 == UD_Ifidiv);
	case X86_FIDIVR:
		return (op2 == UD_Ifidivr);
	case X86_FILD:
		return (op2 == UD_Ifild);
	case X86_FIMUL:
		return (op2 == UD_Ifimul);
	case X86_FINCSTP:
		return (op2 == UD_Ifincstp);
	case X86_FINIT:
		// return (op2 == UD_Ifinit);
		return false;
	case X86_FNINIT:
		return (op2 == UD_Ifninit);
	case X86_FIST:
		return (op2 == UD_Ifist);
	case X86_FISTP:
		return (op2 == UD_Ifistp);
	case X86_FISTTP:
		return (op2 == UD_Ifisttp);
	case X86_FISUB:
		return (op2 == UD_Ifisub);
	case X86_FISUBR:
		return (op2 == UD_Ifisubr);
	case X86_FLD:
		return (op2 == UD_Ifld);
	case X86_FLD1:
		return (op2 == UD_Ifld1);
	case X86_FLDCW:
		return (op2 == UD_Ifldcw);
	case X86_FLDENV:
		return (op2 == UD_Ifldenv);
	case X86_FLDL2E:
		return (op2 == UD_Ifldl2e);
	case X86_FLDL2T:
		return (op2 == UD_Ifldl2t);
	case X86_FLDLG2:
		return (op2 == UD_Ifldlg2);
	case X86_FLDLN2:
		return (op2 == UD_Ifldln2);
	case X86_FLDPI:
		return (op2 == UD_Ifldpi);
	case X86_FLDZ:
		return (op2 == UD_Ifldz);
	case X86_FMUL:
		return (op2 == UD_Ifmul);
	case X86_FMULP:
		return (op2 == UD_Ifmulp);
	case X86_FNCLEX:
		return (op2 == UD_Ifclex);
	case X86_FNDISI:
	case X86_FNENI:
		return false;
	case X86_FNOP:
		return (op2 == UD_Ifnop);
	case X86_FNSAVE:
		return (op2 == UD_Ifnsave);
	case X86_FNSETPM:
		return false;
	case X86_FNSTCW:
		return (op2 == UD_Ifnstcw);
	case X86_FNSTENV:
		return (op2 == UD_Ifnstenv);
	case X86_FNSTSW:
		return (op2 == UD_Ifnstsw);
	case X86_FPATAN:
		return (op2 == UD_Ifpatan);
	case X86_FPREM:
		return (op2 == UD_Ifprem);
	case X86_FPREM1:
		return (op2 == UD_Ifprem1);
	case X86_FPTAN:
		return (op2 == UD_Ifptan);
	case X86_FRNDINT:
		return (op2 == UD_Ifrndint);
	case X86_FRSTOR:
		return (op2 == UD_Ifrstor);
	case X86_FSAVE:
		return false;
	case X86_FSCALE:
		return (op2 == UD_Ifscale);
	case X86_FSIN:
		return (op2 == UD_Ifsin);
	case X86_FSINCOS:
		return (op2 == UD_Ifsincos);
	case X86_FSQRT:
		return (op2 == UD_Ifsqrt);
	case X86_FST:
		return (op2 == UD_Ifst);
	case X86_FSTCW:
		return false;
	case X86_FSTENV:
		return false;
	case X86_FSTP:
		return (op2 == UD_Ifstp);
	case X86_FSTP1:
		return (op2 == UD_Ifstp1);
	case X86_FSTP8:
		return (op2 == UD_Ifstp8);
	case X86_FSTP9:
		return (op2 == UD_Ifstp9);
	case X86_FSTSW:
		return false;
	case X86_FSUB:
		return (op2 == UD_Ifsub);
	case X86_FSUBP:
		return (op2 == UD_Ifsubp);
	case X86_FSUBR:
		return (op2 == UD_Ifsubr);
	case X86_FSUBRP:
		return (op2 == UD_Ifsubrp);
	case X86_FTST:
		return (op2 == UD_Iftst);
	case X86_FUCOM:
		return (op2 == UD_Ifucom);
	case X86_FUCOMI:
		return (op2 == UD_Ifucomi);
	case X86_FUCOMIP:
		return (op2 == UD_Ifucomip);
	case X86_FUCOMP:
		return (op2 == UD_Ifucomp);
	case X86_FUCOMPP:
		return (op2 == UD_Ifucompp);
	case X86_WAIT:
	case X86_FWAIT:
		return (op2 == UD_Iwait);
	case X86_FXAM:
		return (op2 == UD_Ifxam);
	case X86_FXCH:
		return (op2 == UD_Ifxch);
	case X86_FXCH4:
		return (op2 == UD_Ifxch4);
	case X86_FXCH7:
		return (op2 == UD_Ifxch7);
	case X86_FXRSTOR:
		return (op2 == UD_Ifxrstor);
	case X86_FXSAVE:
		return (op2 == UD_Ifxsave);
	case X86_FXTRACT:
		return (op2 == UD_Ifxtract);
	case X86_FYL2X:
		return (op2 == UD_Ifyl2x);
	case X86_FYL2XP1:
		return (op2 == UD_Ifyl2xp1);
	case X86_GETSEC:
		return (op2 == UD_Igetsec);
	case X86_HADDPD:
		return (op2 == UD_Ihaddpd);
	case X86_HADDPS:
		return (op2 == UD_Ihaddps);
	case X86_HINT_NOP:
		return false;
	case X86_HLT:
		return (op2 == UD_Ihlt);
	case X86_HSUBPD:
		return (op2 == UD_Ihsubpd);
	case X86_HSUBPS:
		return (op2 == UD_Ihsubps);
	case X86_IDIV:
		return (op2 == UD_Iidiv);
	case X86_IMUL:
		return (op2 == UD_Iimul);
	case X86_IN:
		return (op2 == UD_Iin);
	case X86_INC:
		return (op2 == UD_Iinc);
	case X86_INS:
		return false;
	case X86_INSB:
		return (op2 == UD_Iinsb);
	case X86_INSW:
		return (op2 == UD_Iinsw);
	case X86_INSD:
		return (op2 == UD_Iinsd);
	case X86_INSERTPS:
		return (op2 == UD_Iinsertps);
	case X86_INT:
		return (op2 == UD_Iint);
	case X86_INT1:
		return (op2 == UD_Iint1);
	case X86_INT3:
		return (op2 == UD_Iint3);
	case X86_ICEBP:
		return false;
	case X86_INTO:
		return (op2 == UD_Iinto);
	case X86_INVD:
		return (op2 == UD_Iinvd);
	case X86_INVEPT:
		return (op2 == UD_Iinvept);
	case X86_INVLPG:
		return (op2 == UD_Iinvlpg);
	case X86_INVLPGA:
		return (op2 == UD_Iinvlpga);
	case X86_INVVPID:
		return (op2 == UD_Iinvvpid);
	case X86_IRET:
		return (op2 == UD_Iiretw);
	case X86_IRETD:
		return (op2 == UD_Iiretd);
	case X86_IRETQ:
		return (op2 == UD_Iiretq);
	case X86_JB:
	case X86_JNAE: // Fall through
		return (op2 == UD_Ijb);
	case X86_JC:
		return false;
	case X86_JBE:
	case X86_JNA: // Fall through
		return (op2 == UD_Ijbe);
	case X86_JCXZ:
		return (op2 == UD_Ijcxz);
	case X86_JECXZ:
		return (op2 == UD_Ijecxz);
	case X86_JRCXZ:
		return (op2 == UD_Ijrcxz);
	case X86_JL:
	case X86_JNGE: // Fall through
		return (op2 == UD_Ijl);
	case X86_JLE:
	case X86_JNG: // Fall through
		return (op2 == UD_Ijle);
	case X86_JMPN:
		return (op2 == UD_Ijmp);
	case X86_JMPF:
		return (op2 == UD_Ijmp);
	case X86_JNC:
		return false;
	case X86_JNB:
	case X86_JAE: // Fall through
		return (op2 == UD_Ijae);
	case X86_JA:
		return (op2 == UD_Ija);
	case X86_JNBE:
		return ((op2 == UD_Ijbe) || (op2 == UD_Ija));
	case X86_JNL:
	case X86_JGE: // Fall through
		return (op2 == UD_Ijge);
	case X86_JNLE:
	case X86_JG: // Fall through
		return (op2 == UD_Ijg);
	case X86_JNO:
		return (op2 == UD_Ijno);
	case X86_JPO:
		return false;
	case X86_JNP:
		return (op2 == UD_Ijnp);
	case X86_JNS:
		return (op2 == UD_Ijns);
	case X86_JNZ:
	case X86_JNE: // Fall through
		return (op2 == UD_Ijnz);
	case X86_JO:
		return (op2 == UD_Ijo);
	case X86_JP:
		return (op2 == UD_Ijp);
	case X86_JPE:
		return false;
	case X86_JS:
		return (op2 == UD_Ijs);
	case X86_JZ:
	case X86_JE: // Fall through
		return (op2 == UD_Ijz);
	case X86_LAHF:
		return (op2 == UD_Ilahf);
	case X86_LAR:
		return (op2 == UD_Ilar);
	case X86_LDDQU:
		return (op2 == UD_Ilddqu);
	case X86_LDMXCSR:
		return (op2 == UD_Ildmxcsr);
	case X86_LDS:
		return (op2 == UD_Ilds);
	case X86_LEA:
		return (op2 == UD_Ilea);
	case X86_LEAVE:
		return (op2 == UD_Ileave);
	case X86_LES:
		return (op2 == UD_Iles);
	case X86_LFENCE:
		return (op2 == UD_Ilfence);
	case X86_LFS:
		return (op2 == UD_Ilfs);
	case X86_LGDT:
		return (op2 == UD_Ilgdt);
	case X86_LGS:
		return (op2 == UD_Ilgs);
	case X86_LIDT:
		return (op2 == UD_Ilidt);
	case X86_LLDT:
		return (op2 == UD_Illdt);
	case X86_LMSW:
		return (op2 == UD_Ilmsw);
	case X86_LODS:
		return false;
	case X86_LODSB:
		return (op2 == UD_Ilodsb);
	case X86_LODSW:
		return (op2 == UD_Ilodsw);
	case X86_LODSD:
		return (op2 == UD_Ilodsd);
	case X86_LODSQ:
		return (op2 == UD_Ilodsq);
	case X86_LOOP:
		return (op2 == UD_Iloop);
	case X86_LOOPNZ:
	case X86_LOOPNE:
		return (op2 == UD_Iloopne);
	case X86_LOOPZ:
	case X86_LOOPE:
		return (op2 == UD_Iloope);
	case X86_LSL:
		return (op2 == UD_Ilsl);
	case X86_LSS:
		return (op2 == UD_Ilss);
	case X86_LTR:
		return (op2 == UD_Iltr);
	case X86_MASKMOVDQU:
		return (op2 == UD_Imaskmovdqu);
	case X86_MASKMOVQ:
		return (op2 == UD_Imaskmovq);
	case X86_MAXPD:
		return (op2 == UD_Imaxpd);
	case X86_MAXPS:
		return (op2 == UD_Imaxps);
	case X86_MAXSD:
		return (op2 == UD_Imaxsd);
	case X86_MAXSS:
		return (op2 == UD_Imaxss);
	case X86_MFENCE:
		return (op2 == UD_Imfence);
	case X86_MINPD:
		return (op2 == UD_Iminpd);
	case X86_MINPS:
		return (op2 == UD_Iminps);
	case X86_MINSD:
		return (op2 == UD_Iminsd);
	case X86_MINSS:
		return (op2 == UD_Iminss);
	case X86_MONITOR:
		return (op2 == UD_Imonitor);
	case X86_MOV:
		return (op2 == UD_Imov);
	case X86_MOVAPD:
		return (op2 == UD_Imovapd);
	case X86_MOVAPS:
		return (op2 == UD_Imovaps);
	case X86_MOVBE:
		return (op2 == UD_Imovbe);
	case X86_MOVD:
		return (op2 == UD_Imovd);
	case X86_MOVQ:
		return (op2 == UD_Imovq);
	case X86_MOVDDUP:
		return (op2 == UD_Imovddup);
	case X86_MOVDQ2Q:
		return (op2 == UD_Imovdq2q);
	case X86_MOVDQA:
		return (op2 == UD_Imovdqa);
	case X86_MOVDQU:
		return (op2 == UD_Imovdqu);
	case X86_MOVHLPS:
		return (op2 == UD_Imovhlps);
	case X86_MOVHPD:
		return (op2 == UD_Imovhpd);
	case X86_MOVHPS:
		return (op2 == UD_Imovhps);
	case X86_MOVLHPS:
		return (op2 == UD_Imovlhps);
	case X86_MOVLPD:
		return (op2 == UD_Imovlpd);
	case X86_MOVLPS:
		return (op2 == UD_Imovlps);
	case X86_MOVMSKPD:
		return (op2 == UD_Imovmskpd);
	case X86_MOVMSKPS:
		return (op2 == UD_Imovmskps);
	case X86_MOVNTDQ:
		return (op2 == UD_Imovntdq);
	case X86_MOVNTDQA:
		return (op2 == UD_Imovntdqa);
	case X86_MOVNTI:
		return (op2 == UD_Imovnti);
	case X86_MOVNTPD:
		return (op2 == UD_Imovntpd);
	case X86_MOVNTPS:
		return (op2 == UD_Imovntps);
	case X86_MOVNTQ:
		return (op2 == UD_Imovntq);
	case X86_MOVQ2DQ:
		return (op2 == UD_Imovq2dq);
	case X86_MOVS:
		return false;
	case X86_MOVSB:
		return (op2 == UD_Imovsb);
	case X86_MOVSW:
		return (op2 == UD_Imovsw);
	case X86_MOVSQ:
		return (op2 == UD_Imovsq);
	case X86_MOVSD:
		return (op2 == UD_Imovsd);
	case X86_MOVSLDUP:
		return (op2 == UD_Imovsldup);
	case X86_MOVSHDUP:
		return (op2 == UD_Imovshdup);
	case X86_MOVSS:
		return (op2 == UD_Imovss);
	case X86_MOVSX:
		return (op2 == UD_Imovsx);
	case X86_MOVSXD:
		return (op2 == UD_Imovsxd);
	case X86_MOVUPD:
		return (op2 == UD_Imovupd);
	case X86_MOVUPS:
		return (op2 == UD_Imovups);
	case X86_MOVZX:
		return (op2 == UD_Imovzx);
	case X86_MPSADBW:
		return (op2 == UD_Impsadbw);
	case X86_MUL:
		return (op2 == UD_Imul);
	case X86_MULPD:
		return (op2 == UD_Imulpd);
	case X86_MULPS:
		return (op2 == UD_Imulps);
	case X86_MULSD:
		return (op2 == UD_Imulsd);
	case X86_MULSS:
		return (op2 == UD_Imulss);
	case X86_MWAIT:
		return (op2 == UD_Imwait);
	case X86_NEG:
		return (op2 == UD_Ineg);
	case X86_NOP:
		return (op2 == UD_Inop);
	case X86_NOT:
		return (op2 == UD_Inot);;
	case X86_OR:
		return (op2 == UD_Ior);
	case X86_ORPD:
		return (op2 == UD_Iorpd);
	case X86_ORPS:
		return (op2 == UD_Iorps);
	case X86_OUT:
		return (op2 == UD_Iout);
	case X86_OUTS:
		return false;
	case X86_OUTSB:
		return (op2 == UD_Ioutsb);
	case X86_OUTSW:
		return (op2 == UD_Ioutsw);
	case X86_OUTSD:
		return (op2 == UD_Ioutsd);
	case X86_PABSB:
		return (op2 == UD_Ipabsb);
	case X86_PABSD:
		return (op2 == UD_Ipabsd);
	case X86_PABSW:
		return (op2 == UD_Ipabsw);
	case X86_PACKSSDW:
		return (op2 == UD_Ipackssdw);
	case X86_PACKSSWB:
		return (op2 == UD_Ipacksswb);
	case X86_PACKUSDW:
		return (op2 == UD_Ipackusdw);
	case X86_PACKUSWB:
		return (op2 == UD_Ipackuswb);
	case X86_PADDB:
		return (op2 == UD_Ipaddb);
	case X86_PADDD:
		return (op2 == UD_Ipaddd);
	case X86_PADDQ:
		return (op2 == UD_Ipaddq);
	case X86_PADDSB:
		return (op2 == UD_Ipaddsb);
	case X86_PADDSW:
		return (op2 == UD_Ipaddsw);
	case X86_PADDUSB:
		return (op2 == UD_Ipaddusb);
	case X86_PADDUSW:
		return (op2 == UD_Ipaddusw);
	case X86_PADDW:
		return (op2 == UD_Ipaddw);
	case X86_PALIGNR:
		return (op2 == UD_Ipalignr);
	case X86_PAND:
		return (op2 == UD_Ipand);
	case X86_PANDN:
		return (op2 == UD_Ipandn);
	case X86_PAUSE:
		return (op2 == UD_Ipause);
	case X86_PAVGB:
		return (op2 == UD_Ipavgb);
	case X86_PAVGW:
		return (op2 == UD_Ipavgw);
	case X86_PBLENDVB:
		return (op2 == UD_Ipblendvb);
	case X86_PBLENDW:
		return (op2 == UD_Ipblendw);
	case X86_PCLMULQDQ:
		return (op2 == UD_Ipclmulqdq);
	case X86_PCMPEQB:
		return (op2 == UD_Ipcmpeqb);
	case X86_PCMPEQD:
		return (op2 == UD_Ipcmpeqd);
	case X86_PCMPEQQ:
		return (op2 == UD_Ipcmpeqq);
	case X86_PCMPEQW:
		return (op2 == UD_Ipcmpeqw);
	case X86_PCMPESTRI:
		return (op2 == UD_Ipcmpestri);
	case X86_PCMPESTRM:
		return (op2 == UD_Ipcmpestrm);
	case X86_PCMPGTB:
		return (op2 == UD_Ipcmpgtb);
	case X86_PCMPGTD:
		return (op2 == UD_Ipcmpgtd);
	case X86_PCMPGTQ:
		return (op2 == UD_Ipcmpgtq);
	case X86_PCMPGTW:
		return (op2 == UD_Ipcmpgtw);
	case X86_PCMPISTRI:
		return (op2 == UD_Ipcmpistri);
	case X86_PCMPISTRM:
		return (op2 == UD_Ipcmpistrm);
	case X86_PEXTRB:
		return (op2 == UD_Ipextrb);
	case X86_PEXTRD:
		return (op2 == UD_Ipextrd);
	case X86_PEXTRQ:
		return (op2 == UD_Ipextrq);
	case X86_PEXTRW:
		return (op2 == UD_Ipextrw);
	// 3dnow!
	case X86_PFCMPGE:
		return (op2 == UD_Ipfcmpge);
	case X86_PFCMPGT:
		return (op2 == UD_Ipfcmpgt);
	case X86_PFCMPEQ:
		return (op2 == UD_Ipfcmpeq);
	case X86_PFMIN:
		return (op2 == UD_Ipfmin);
	case X86_PFMAX:
		return (op2 == UD_Ipfmax);
	case X86_PFMUL:
		return (op2 == UD_Ipfmul);
	case X86_PFRCP:
		return (op2 == UD_Ipfrcp);
	case X86_PFRCPIT1:
		return (op2 == UD_Ipfrcpit1);
	case X86_PFRCPIT2:
		return (op2 == UD_Ipfrcpit2);
	case X86_PFRSQRT:
		return (op2 == UD_Ipfrsqrt);
	case X86_PFRSQIT1:
		return (op2 == UD_Ipfrsqit1);
	case X86_PFNACC:
		return (op2 == UD_Ipfnacc);
	case X86_PFSUB:
		return (op2 == UD_Ipfsub);
	case X86_PFSUBR:
		return (op2 == UD_Ipfsubr);
	case X86_PSWAPD:
		return (op2 == UD_Ipswapd);
	case X86_PFPNACC:
		return (op2 == UD_Ipfpnacc);
	case X86_PFADD:
		return (op2 == UD_Ipfadd);
	case X86_PFACC:
		return (op2 == UD_Ipfacc);
	case X86_PAVGUSB:
		return (op2 == UD_Ipavgusb);
	case X86_PI2FW:
		return (op2 == UD_Ipi2fw);
	case X86_PF2IW:
		return (op2 == UD_Ipf2iw);
	case X86_PI2FD:
		return (op2 == UD_Ipi2fd);
	case X86_PF2ID:
		return (op2 == UD_Ipf2id);
	case X86_PMULHRW:
		return (op2 == UD_Ipmulhrw);
	case X86_PHADDD:
		return (op2 == UD_Iphaddd);
	case X86_PHADDSW:
		return (op2 == UD_Iphaddsw);
	case X86_PHADDW:
		return (op2 == UD_Iphaddw);
	case X86_PHMINPOSUW:
		return (op2 == UD_Iphminposuw);
	case X86_PHSUBD:
		return (op2 == UD_Iphsubd);
	case X86_PHSUBSW:
		return (op2 == UD_Iphsubsw);
	case X86_PHSUBW:
		return (op2 == UD_Iphsubw);
	case X86_PINSRB:
		return (op2 == UD_Ipinsrb);
	case X86_PINSRD:
		return (op2 == UD_Ipinsrd);
	case X86_PINSRQ:
		return (op2 == UD_Ipinsrq);
	case X86_PINSRW:
		return (op2 == UD_Ipinsrw);
	case X86_PMADDUBSW:
		return (op2 == UD_Ipmaddubsw);
	case X86_PMADDWD:
		return (op2 == UD_Ipmaddwd);
	case X86_PMAXSB:
		return (op2 == UD_Ipmaxsb);
	case X86_PMAXSD:
		return (op2 == UD_Ipmaxsd);
	case X86_PMAXSW:
		return (op2 == UD_Ipmaxsw);
	case X86_PMAXUB:
		return (op2 == UD_Ipmaxub);
	case X86_PMAXUD:
		return (op2 == UD_Ipmaxud);
	case X86_PMAXUW:
		return (op2 == UD_Ipmaxuw);
	case X86_PMINSB:
		return (op2 == UD_Ipminsb);
	case X86_PMINSD:
		return (op2 == UD_Ipminsd);
	case X86_PMINSW:
		return (op2 == UD_Ipminsw);
	case X86_PMINUB:
		return (op2 == UD_Ipminub);
	case X86_PMINUD:
		return (op2 == UD_Ipminud);
	case X86_PMINUW:
		return (op2 == UD_Ipminuw);
	case X86_PMOVMSKB:
		return (op2 == UD_Ipmovmskb);
	case X86_PMOVSXBD:
		return (op2 == UD_Ipmovsxbd);
	case X86_PMOVSXBQ:
		return (op2 == UD_Ipmovsxbq);
	case X86_PMOVSXBW:
		return (op2 == UD_Ipmovsxbw);
	case X86_PMOVSXDQ:
		return (op2 == UD_Ipmovsxdq);
	case X86_PMOVSXWD:
		return (op2 == UD_Ipmovsxwd);
	case X86_PMOVSXWQ:
		return (op2 == UD_Ipmovsxwq);
	case X86_PMOVZXBD:
		return (op2 == UD_Ipmovzxbd);
	case X86_PMOVZXBQ:
		return (op2 == UD_Ipmovzxbq);
	case X86_PMOVZXBW:
		return (op2 == UD_Ipmovzxbw);
	case X86_PMOVZXDQ:
		return (op2 == UD_Ipmovzxdq);
	case X86_PMOVZXWD:
		return (op2 == UD_Ipmovzxwd);
	case X86_PMOVZXWQ:
		return (op2 == UD_Ipmovzxwq);
	case X86_PMULDQ:
		return (op2 == UD_Ipmuldq);
	case X86_PMULHRSW:
		return (op2 == UD_Ipmulhrsw);
	case X86_PMULHUW:
		return (op2 == UD_Ipmulhuw);
	case X86_PMULHW:
		return (op2 == UD_Ipmulhw);
	case X86_PMULLD:
		return (op2 == UD_Ipmulld);
	case X86_PMULLW:
		return (op2 == UD_Ipmullw);
	case X86_PMULUDQ:
		return (op2 == UD_Ipmuludq);
	case X86_POP:
		return (op2 == UD_Ipop);
	case X86_POPA:
		return (op2 == UD_Ipopa);
	case X86_POPAD:
		return (op2 == UD_Ipopad);
	case X86_POPCNT:
		return (op2 == UD_Ipopcnt);
	case X86_POPF:
		return (op2 == UD_Ipopfw);
	case X86_POPFQ:
		return (op2 == UD_Ipopfq);
	case X86_POPFD:
		return (op2 == UD_Ipopfd);
	case X86_POR:
		return (op2 == UD_Ipor);
	case X86_PREFETCH:
	case X86_PREFETCHW:
		return (op2 == UD_Iprefetch);
	case X86_PREFETCHNTA:
		return (op2 == UD_Iprefetchnta);
	case X86_PREFETCHT0:
		return (op2 == UD_Iprefetcht0);
	case X86_PREFETCHT1:
		return (op2 == UD_Iprefetcht1);
	case X86_PREFETCHT2:
		return (op2 == UD_Iprefetcht2);
	case X86_PSADBW:
		return (op2 == UD_Ipsadbw);
	case X86_PSHUFB:
		return (op2 == UD_Ipshufb);
	case X86_PSHUFD:
		return (op2 == UD_Ipshufd);
	case X86_PSHUFHW:
		return (op2 == UD_Ipshufhw);
	case X86_PSHUFLW:
		return (op2 == UD_Ipshuflw);
	case X86_PSHUFW:
		return (op2 == UD_Ipshufw);
	case X86_PSIGNB:
		return (op2 == UD_Ipsignb);
	case X86_PSIGND:
		return (op2 == UD_Ipsignd);
	case X86_PSIGNW:
		return (op2 == UD_Ipsignw);
	case X86_PSLLD:
		return (op2 == UD_Ipslld);
	case X86_PSLLDQ:
		return (op2 == UD_Ipslldq);
	case X86_PSLLQ:
		return (op2 == UD_Ipsllq);
	case X86_PSLLW:
		return (op2 == UD_Ipsllw);
	case X86_PSRAD:
		return (op2 == UD_Ipsrad);
	case X86_PSRAW:
		return (op2 == UD_Ipsraw);
	case X86_PSRLD:
		return (op2 == UD_Ipsrld);
	case X86_PSRLDQ:
		return (op2 == UD_Ipsrldq);
	case X86_PSRLQ:
		return (op2 == UD_Ipsrlq);
	case X86_PSRLW:
		return (op2 == UD_Ipsrlw);
	case X86_PSUBB:
		return (op2 == UD_Ipsubb);
	case X86_PSUBD:
		return (op2 == UD_Ipsubd);
	case X86_PSUBQ:
		return (op2 == UD_Ipsubq);
	case X86_PSUBSB:
		return (op2 == UD_Ipsubsb);
	case X86_PSUBSW:
		return (op2 == UD_Ipsubsw);
	case X86_PSUBUSB:
		return (op2 == UD_Ipsubusb);
	case X86_PSUBUSW:
		return (op2 == UD_Ipsubusw);
	case X86_PSUBW:
		return (op2 == UD_Ipsubw);
	case X86_PTEST:
		return (op2 == UD_Iptest);
	case X86_PUNPCKHBW:
		return (op2 == UD_Ipunpckhbw);
	case X86_PUNPCKHDQ:
		return (op2 == UD_Ipunpckhdq);
	case X86_PUNPCKHQDQ:
		return (op2 == UD_Ipunpckhqdq);
	case X86_PUNPCKHWD:
		return (op2 == UD_Ipunpckhwd);
	case X86_PUNPCKLBW:
		return (op2 == UD_Ipunpcklbw);
	case X86_PUNPCKLDQ:
		return (op2 == UD_Ipunpckldq);
	case X86_PUNPCKLQDQ:
		return (op2 == UD_Ipunpcklqdq);
	case X86_PUNPCKLWD:
		return (op2 == UD_Ipunpcklwd);
	case X86_PUSH:
		return (op2 == UD_Ipush);
	case X86_PUSHA:
		return (op2 == UD_Ipusha);
	case X86_PUSHAD:
		return (op2 == UD_Ipushad);
	case X86_PUSHF:
		return (op2 == UD_Ipushfw);
	case X86_PUSHFQ:
		return (op2 == UD_Ipushfq);
	case X86_PUSHFD:
		return (op2 == UD_Ipushfd);
	case X86_PXOR:
		return (op2 == UD_Ipxor);
	case X86_RCL:
		return (op2 == UD_Ircl);
	case X86_RCPPS:
		return (op2 == UD_Ircpps);
	case X86_RCPSS:
		return (op2 == UD_Ircpss);
	case X86_RCR:
		return (op2 == UD_Ircr);
	case X86_RDMSR:
		return (op2 == UD_Irdmsr);
	case X86_RDPMC:
		return (op2 == UD_Irdpmc);
	case X86_RDRAND:
		return (op2 == UD_Irdrand);
	case X86_RDTSC:
		return (op2 == UD_Irdtsc);
	case X86_RDTSCP:
		return (op2 == UD_Irdtscp);
	case X86_RETF:
		return (op2 == UD_Iretf);
	case X86_RETN:
		return (op2 == UD_Iret);
	case X86_ROL:
		return (op2 == UD_Irol);
	case X86_ROR:
		return (op2 == UD_Iror);
	case X86_ROUNDPD:
		return (op2 == UD_Iroundpd);
	case X86_ROUNDPS:
		return (op2 == UD_Iroundps);
	case X86_ROUNDSD:
		return (op2 == UD_Iroundsd);
	case X86_ROUNDSS:
		return (op2 == UD_Iroundss);
	case X86_RSM:
		return (op2 == UD_Irsm);
	case X86_RSQRTPS:
		return (op2 == UD_Irsqrtps);
	case X86_RSQRTSS:
		return (op2 == UD_Irsqrtss);
	case X86_SAHF:
		return (op2 == UD_Isahf);
	case X86_SAL:
	case X86_SHL:
		return (op2 == UD_Ishl);
	case X86_SALC:
		return (op2 == UD_Isalc);
	case X86_SETALC:
		return false;
	case X86_SAR:
		return (op2 == UD_Isar);
	case X86_SBB:
		return (op2 == UD_Isbb);
	case X86_SCAS:
		return false;
	case X86_SCASB:
		return (op2 == UD_Iscasb);
	case X86_SCASW:
		return (op2 == UD_Iscasw);
	case X86_SCASD:
		return (op2 == UD_Iscasd);
	case X86_SCASQ:
		return (op2 == UD_Iscasq);
	case X86_SETB:
		return (op2 == UD_Isetb);
	case X86_SETNAE:
		return (op2 == UD_Isetb);
	case X86_SETC:
		return false;
	case X86_SETBE:
		return (op2 == UD_Isetbe);
	case X86_SETNA:
		return false;
	case X86_SETL:
		return (op2 == UD_Isetl);
	case X86_SETNGE:
		return false;
	case X86_SETLE:
		return (op2 == UD_Isetle);
	case X86_SETNG:
		return false;
	case X86_SETNB:
	case X86_SETAE:
		return (op2 == UD_Isetae);
	case X86_SETNC:
		return false;
	case X86_SETNBE:
	case X86_SETA:
		return (op2 == UD_Iseta);
	case X86_SETNL:
	case X86_SETGE:
		return (op2 == UD_Isetge);
	case X86_SETNLE:
	case X86_SETG:
		return (op2 == UD_Isetg);
	case X86_SETNO:
		return (op2 == UD_Isetno);
	case X86_SETNP:
		return (op2 == UD_Isetnp);
	case X86_SETPO:
		return false;
	case X86_SETNS:
		return (op2 == UD_Isetns);
	case X86_SETNZ:
		return (op2 == UD_Isetnz);
	case X86_SETNE:
		return false;
	case X86_SETO:
		return (op2 == UD_Iseto);
	case X86_SETP:
		return (op2 == UD_Isetp);
	case X86_SETPE:
		return false;
	case X86_SETS:
		return (op2 == UD_Isets);
	case X86_SETZ:
		return (op2 == UD_Isetz);
	case X86_SETE:
		return false;
	case X86_SFENCE:
		return (op2 == UD_Isfence);
	case X86_SGDT:
		return (op2 == UD_Isgdt);
	case X86_SKINIT:
		return (op2 == UD_Iskinit);
	case X86_SLDT:
		return (op2 == UD_Isldt);
	case X86_SHLD:
		return (op2 == UD_Ishld);
	case X86_SHR:
		return (op2 == UD_Ishr);
	case X86_SHRD:
		return (op2 == UD_Ishrd);
	case X86_SHUFPD:
		return (op2 == UD_Ishufpd);
	case X86_SHUFPS:
		return (op2 == UD_Ishufps);
	case X86_SIDT:
		return (op2 == UD_Isidt);
	case X86_SMSW:
		return (op2 == UD_Ismsw);
	case X86_SQRTPD:
		return (op2 == UD_Isqrtpd);
	case X86_SQRTPS:
		return (op2 == UD_Isqrtps);
	case X86_SQRTSD:
		return (op2 == UD_Isqrtsd);
	case X86_SQRTSS:
		return (op2 == UD_Isqrtss);
	case X86_STC:
		return (op2 == UD_Istc);
	case X86_STD:
		return (op2 == UD_Istd);
	case X86_STGI:
		return (op2 == UD_Istgi);
	case X86_STI:
		return (op2 == UD_Isti);
	case X86_STMXCSR:
		return (op2 == UD_Istmxcsr);
	case X86_STOS:
		return false;
	case X86_STOSB:
		return (op2 == UD_Istosb);
	case X86_STOSW:
		return (op2 == UD_Istosw);
	case X86_STOSD:
		return (op2 == UD_Istosd);
	case X86_STOSQ:
		return (op2 == UD_Istosq);
	case X86_STR:
		return (op2 == UD_Istr);
	case X86_SUB:
		return (op2 == UD_Isub);
	case X86_SUBPD:
		return (op2 == UD_Isubpd);
	case X86_SUBPS:
		return (op2 == UD_Isubps);
	case X86_SUBSD:
		return (op2 == UD_Isubsd);
	case X86_SUBSS:
		return (op2 == UD_Isubss);
	case X86_SWAPGS:
		return (op2 == UD_Iswapgs);
	case X86_SYSCALL:
		return (op2 == UD_Isyscall);
	case X86_SYSENTER:
		return (op2 == UD_Isysenter);
	case X86_SYSRET:
		return (op2 == UD_Isysret);
	case X86_SYSEXIT:
		return (op2 == UD_Isysexit);
	case X86_TEST:
		return (op2 == UD_Itest);
	case X86_UCOMISD:
		return (op2 == UD_Iucomisd);
	case X86_UCOMISS:
		return (op2 == UD_Iucomiss);
	case X86_UD:
	case X86_UD1:
		return (op2 == UD_Iinvalid);
	case X86_UD2:
		return (op2 == UD_Iud2);
	case X86_UNPCKHPD:
		return (op2 == UD_Iunpckhpd);
	case X86_UNPCKHPS:
		return (op2 == UD_Iunpckhps);
	case X86_UNPCKLPD:
		return (op2 == UD_Iunpcklpd);
	case X86_UNPCKLPS:
		return (op2 == UD_Iunpcklps);
	case X86_VBLENDPD:
	case X86_VBLENDPS:
	case X86_VBLENDVPD:
	case X86_VBLENDVPS:
	case X86_VDPPD:
	case X86_VDPPS:
	case X86_VEXTRACTPS:
	case X86_VINSERTPS:
	case X86_VMOVNTDQA:
	case X86_VMPSADBW:
	case X86_VPACKUDSW:
	case X86_VBLENDVB:
	case X86_VPBLENDW:
	case X86_VPCMPEQQ:
	case X86_VPEXTRB:
	case X86_VPEXTRD:
	case X86_VPEXTRW:
	case X86_VPHMINPOSUW:
	case X86_VPINSRB:
	case X86_VPINSRD:
	case X86_VPINSRQ:
	case X86_VPMAXSB:
	case X86_VPMAXSD:
	case X86_VMPAXUD:
	case X86_VPMAXUW:
	case X86_VPMINSB:
	case X86_VPMINSD:
	case X86_VPMINUD:
	case X86_VPMINUW:
	case X86_VPMOVSXBD:
	case X86_VPMOVSXBQ:
	case X86_VPMOVSXBW:
	case X86_VPMOVSXWD:
	case X86_VPMOVSXWQ:
	case X86_VPMOVSXDQ:
	case X86_VPMOVZXBD:
	case X86_VPMOVZXBQ:
	case X86_VPMOVZXBW:
	case X86_VPMOVZXWD:
	case X86_VPMOVZXWQ:
	case X86_VPMOVZXDQ:
	case X86_VPMULDQ:
	case X86_VPMULLD:
	case X86_VPTEST:
	case X86_VROUNDPD:
	case X86_VROUNDPS:
	case X86_VROUNDSD:
	case X86_VROUNDSS:
	case X86_VPCMPESTRI:
	case X86_VPCMPESTRM:
	case X86_VPCMPGTQ:
	case X86_VPCMPISTRI:
	case X86_VPCMPISTRM:
	case X86_VAESDEC:
	case X86_VAESDECLAST:
	case X86_VAESENC:
	case X86_VAESENCLAST:
	case X86_VAESIMC:
	case X86_VAESKEYGENASSIST:
	case X86_VPABSB:
	case X86_VPABSD:
	case X86_VPABSW:
	case X86_VPALIGNR:
	case X86_VPAHADD:
	case X86_VPHADDW:
	case X86_VPHADDSW:
	case X86_VPHSUBD:
	case X86_VPHSUBW:
	case X86_VPHSUBSW:
	case X86_VPMADDUBSW:
	case X86_VPMULHRSW:
	case X86_VPSHUFB:
	case X86_VPSIGNB:
	case X86_VPSIGND:
	case X86_VPSIGNW:
	case X86_VADDSUBPD:
	case X86_VADDSUBPS:
	case X86_VHADDPD:
	case X86_VHADDPS:
	case X86_VHSUBPD:
	case X86_VHSUBPS:
	case X86_VLDDQU:
	case X86_VMOVDDUP:
	case X86_VMOVHLPS:
	case X86_VMOVSHDUP:
	case X86_VMOVSLDUP:
	case X86_VADDPD:
	case X86_VADDSD:
	case X86_VANPD:
	case X86_VANDNPD:
	case X86_VCMPPD:
	case X86_VCMPSD:
	case X86_VCOMISD:
	case X86_VCVTDQ2PD:
	case X86_VCVTDQ2PS:
	case X86_VCVTPD2DQ:
	case X86_VCVTPD2PS:
	case X86_VCVTPS2DQ:
	case X86_VCVTPS2PD:
	case X86_VCVTSD2SI:
	case X86_VCVTSD2SS:
	case X86_VCVTSI2SD:
	case X86_VCVTSS2SD:
	case X86_VCVTTPD2DQ:
	case X86_VCVTTPS2DQ:
	case X86_VCVTTSD2SI:
	case X86_VDIVPD:
	case X86_VDIVSD:
	case X86_VMASKMOVDQU:
	case X86_VMAXPD:
	case X86_VMAXSD:
	case X86_VMINPD:
	case X86_VMINSD:
	case X86_VMOVAPD:
	case X86_VMOVD:
	case X86_VMOVQ:
	case X86_VMOVDQA:
	case X86_VMOVDQU:
	case X86_VMOVHPD:
	case X86_VMOVLPD:
	case X86_VMOVMSKPD:
	case X86_VMOVNTDQ:
	case X86_VMOVNTPD:
	case X86_VMOVSD:
	case X86_VMOVUPD:
	case X86_VMULPD:
	case X86_VMULSD:
	case X86_VORPD:
	case X86_VPACKSSWB:
	case X86_VPACKSSDW:
	case X86_VPACKUSWB:
	case X86_VPADDB:
	case X86_VPADDW:
	case X86_VPADDD:
	case X86_VPADDQ:
	case X86_VPADDSB:
	case X86_VPADDSW:
	case X86_VPADDUSB:
	case X86_VPADDUSW:
	case X86_VPAND:
	case X86_VPANDN:
	case X86_VPAVGB:
	case X86_VPAVGW:
	case X86_VPCMPEQB:
	case X86_VPCMPEQW:
	case X86_VPCMPEQD:
	case X86_VPCMPGTB:
	case X86_VPCMPGTW:
	case X86_VPCMPGTD:
	case X86_VPEXTSRW:
	case X86_VPMADDWD:
	case X86_VPMAXSW:
	case X86_VPMAXUB:
	case X86_VPMINSW:
	case X86_VPMINUB:
	case X86_VPMOVMSKB:
	case X86_VPMULHUW:
	case X86_VPMULHW:
	case X86_VPMULLW:
	case X86_VPMULUDQ:
	case X86_VPOR:
	case X86_VPSADBW:
	case X86_VPSHUFD:
	case X86_VPSHUFHW:
	case X86_VPSHUFLW:
	case X86_VPSLLDQ:
	case X86_VPSLLW:
	case X86_VPSLLD:
	case X86_VPSLLQ:
	case X86_VPSRAW:
	case X86_VPSRAD:
	case X86_VPSRLDQ:
	case X86_VPSRLW:
	case X86_VPSRLD:
	case X86_VPSRLQ:
	case X86_VPSUBB:
	case X86_VPSUBW:
	case X86_VPSUBD:
	case X86_VPSQUBQ:
	case X86_VPSUBSB:
	case X86_VPSUBSW:
	case X86_VPSUBUSB:
	case X86_VPSUBUSW:
	case X86_VPUNPCKHBW:
	case X86_VPUNPCKHWD:
	case X86_VPUNPCKHDQ:
	case X86_VPUNPCKHQDQ:
	case X86_VPUNPCKLBW:
	case X86_VPUNPCKLWD:
	case X86_VPUNPCKLDQ:
	case X86_VPUNCKLQDQ:
	case X86_VPXOR:
	case X86_VSHUFPD:
	case X86_VSQRTPD:
	case X86_VSQRTSD:
	case X86_VSUBPD:
	case X86_VSUBSD:
	case X86_VUCOMISD:
	case X86_VUNPCKHPD:
	case X86_VUNPCKHPS:
	case X86_VUNPCKLPD:
	case X86_VUNPCKLPS:
	case X86_VXORPD:
	case X86_VADDPS:
	case X86_VADDSS:
	case X86_VANDPS:
	case X86_VANDNPS:
	case X86_VCMPPS:
	case X86_VCMPSS:
	case X86_VCOMISS:
	case X86_VCVTSI2SS:
	case X86_VCVTSS2SI:
	case X86_VCVTTSS2SI:
	case X86_VDIVPS:
	case X86_VLDMXCSR:
	case X86_VMAXPS:
	case X86_VMAXSS:
	case X86_VMINPS:
	case X86_VMINSS:
	case X86_VMOVAPS:
	case X86_VMOVHPS:
	case X86_VMOVLHPS:
	case X86_VMOVLPS:
	case X86_VMOVMSKPS:
	case X86_VMOVNTPS:
	case X86_VMOVSS:
	case X86_VMOVUPS:
	case X86_VMULPS:
	case X86_VMULSS:
	case X86_VORPS:
	case X86_VRCPPS:
	case X86_VRCPSS:
	case X86_VRSQRTPS:
	case X86_VRSQRTSS:
	case X86_VSQRTPS:
	case X86_VSQRTSS:
	case X86_VSTMXCSR:
	case X86_VSUBPS:
	case X86_VSUBSS:
	case X86_VUCOMISS:
	case X86_VXORPS:
	case X86_VBROADCAST:
	case X86_VEXTRACTF128:
	case X86_VINSERTF128:
	case X86_VPERMILPD:
	case X86_VPERMILPS:
	case X86_VPERM2F128:
	case X86_VTESTPD:
	case X86_VTESTPS:
		return false;
	case X86_VERR:
		return (op2 == UD_Iverr);
	case X86_VERW:
		return (op2 == UD_Iverw);
	case X86_VMCALL:
		return (op2 == UD_Ivmcall);
	case X86_VMCLEAR:
		return (op2 == UD_Ivmclear);
	case X86_VMLAUNCH:
		return (op2 == UD_Ivmlaunch);
	case X86_VMPTRLD:
		return (op2 == UD_Ivmptrld);
	case X86_VMPTRST:
		return (op2 == UD_Ivmptrst);
	case X86_VMREAD:
		return (op2 == UD_Ivmread);
	case X86_VMFUNC:
		// return (op2 == UD_Ivmfunc);
		return false;
	case X86_VMRESUME:
		return (op2 == UD_Ivmresume);
	case X86_VMWRITE:
		return (op2 == UD_Ivmwrite);
	case X86_VMXOFF:
		return (op2 == UD_Ivmxoff);
	case X86_VMXON:
		return (op2 == UD_Ivmxon);
	case X86_VMLOAD:
		return (op2 == UD_Ivmload);
	case X86_VMMCALL:
		return (op2 == UD_Ivmmcall);
	case X86_VMRUN:
		return (op2 == UD_Ivmrun);
	case X86_VMSAVE:
		return (op2 == UD_Ivmsave);
	case X86_WBINVD:
		return (op2 == UD_Iwbinvd);
	case X86_WRMSR:
		return (op2 == UD_Iwrmsr);
	case X86_XADD:
		return (op2 == UD_Ixadd);
	case X86_XCHG:
		return (op2 == UD_Ixchg);
	case X86_XGETBV:
		return (op2 == UD_Ixgetbv);
	case X86_XLAT:
	case X86_XLATB:
		return (op2 == UD_Ixlatb);
	case X86_XOR:
		return (op2 == UD_Ixor);
	case X86_XORPD:
		return (op2 == UD_Ixorpd);
	case X86_XORPS:
		return (op2 == UD_Ixorps);
	case X86_XRSTOR:
		return (op2 == UD_Ixrstor);
	case X86_XSAVE:
		return (op2 == UD_Ixsave);
	case X86_XSETBV:
		return (op2 == UD_Ixsetbv);
	case X86_XABORT:
		return false;
	case X86_XBEGIN:
		return false;
	default:
		return false;
	}
}


static bool CompareRegisters(X86OperandType operand1, enum ud_type operand2)
{
	switch (operand1)
	{
	case X86_NONE:
		return (operand2 == UD_NONE);
	case X86_IP: // Fall through
	case X86_EIP: // Fall through
	case X86_RIP:
		return (operand2 == UD_R_RIP);
	case X86_AL:
		return (operand2 == UD_R_AL);
	case X86_AH:
		return (operand2 == UD_R_AH);
	case X86_AX:
		return (operand2 == UD_R_AX);
	case X86_EAX:
		return (operand2 == UD_R_EAX);
	case X86_RAX:
		return (operand2 == UD_R_RAX);
	case X86_CL:
		return (operand2 == UD_R_CL);
	case X86_CH:
		return (operand2 == UD_R_CH);
	case X86_CX:
		return (operand2 == UD_R_CX);
	case X86_ECX:
		return (operand2 == UD_R_ECX);
	case X86_RCX:
		return (operand2 == UD_R_RCX);
	case X86_DL:
		return (operand2 == UD_R_DL);
	case X86_DH:
		return (operand2 == UD_R_DH);
	case X86_DX:
		return (operand2 == UD_R_DX);
	case X86_EDX:
		return (operand2 == UD_R_EDX);
	case X86_RDX:
		return (operand2 == UD_R_RDX);
	case X86_BL:
		return (operand2 == UD_R_BL);
	case X86_BH:
		return (operand2 == UD_R_BH);
	case X86_BX:
		return (operand2 == UD_R_BX);
	case X86_EBX:
		return (operand2 == UD_R_EBX);
	case X86_RBX:
		return (operand2 == UD_R_RBX);
	case X86_SIL:
		return (operand2 == UD_R_SIL);
	case X86_SI:
		return (operand2 == UD_R_SI);
	case X86_ESI:
		return (operand2 == UD_R_ESI);
	case X86_RSI:
		return (operand2 == UD_R_RSI);
	case X86_DIL:
		return (operand2 == UD_R_DIL);
	case X86_DI:
		return (operand2 == UD_R_DI);
	case X86_EDI:
		return (operand2 == UD_R_EDI);
	case X86_RDI:
		return (operand2 == UD_R_RDI);
	case X86_SPL:
		return (operand2 == UD_R_SPL);
	case X86_SP:
		return (operand2 == UD_R_SP);
	case X86_ESP:
		return (operand2 == UD_R_ESP);
	case X86_RSP:
		return (operand2 == UD_R_RSP);
	case X86_BPL:
		return (operand2 == UD_R_BPL);
	case X86_BP:
		return (operand2 == UD_R_BP);
	case X86_EBP:
		return (operand2 == UD_R_EBP);
	case X86_RBP:
		return (operand2 == UD_R_RBP);
	case X86_R8B:
		return (operand2 == UD_R_R8B);
	case X86_R8W:
		return (operand2 == UD_R_R8W);
	case X86_R8D:
		return (operand2 == UD_R_R8D);
	case X86_R8:
		return (operand2 == UD_R_R8);
	case X86_R9B:
		return (operand2 == UD_R_R9B);
	case X86_R9W:
		return (operand2 == UD_R_R9W);
	case X86_R9D:
		return (operand2 == UD_R_R9D);
	case X86_R9:
		return (operand2 == UD_R_R9);
	case X86_R10B:
		return (operand2 == UD_R_R10B);
	case X86_R10W:
		return (operand2 == UD_R_R10W);
	case X86_R10D:
		return (operand2 == UD_R_R10D);
	case X86_R10:
		return (operand2 == UD_R_R10);
	case X86_R11B:
		return (operand2 == UD_R_R11B);
	case X86_R11W:
		return (operand2 == UD_R_R11W);
	case X86_R11D:
		return (operand2 == UD_R_R11D);
	case X86_R11:
		return (operand2 == UD_R_R11);
	case X86_R12B:
		return (operand2 == UD_R_R12B);
	case X86_R12W:
		return (operand2 == UD_R_R12W);
	case X86_R12D:
		return (operand2 == UD_R_R12D);
	case X86_R12:
		return (operand2 == UD_R_R12);
	case X86_R13B:
		return (operand2 == UD_R_R13B);
	case X86_R13W:
		return (operand2 == UD_R_R13W);
	case X86_R13D:
		return (operand2 == UD_R_R13D);
	case X86_R13:
		return (operand2 == UD_R_R13);
	case X86_R14B:
		return (operand2 == UD_R_R14B);
	case X86_R14W:
		return (operand2 == UD_R_R14W);
	case X86_R14D:
		return (operand2 == UD_R_R14D);
	case X86_R14:
		return (operand2 == UD_R_R14);
	case X86_R15B:
		return (operand2 == UD_R_R15B);
	case X86_R15W:
		return (operand2 == UD_R_R15W);
	case X86_R15D:
		return (operand2 == UD_R_R15D);
	case X86_R15:
		return (operand2 == UD_R_R15);
	case X86_ES:
		return (operand2 == UD_R_ES);
	case X86_SS:
		return (operand2 == UD_R_SS);
	case X86_CS:
		return (operand2 == UD_R_CS);
	case X86_DS:
		return (operand2 == UD_R_DS);
	case X86_FS:
		return (operand2 == UD_R_FS);
	case X86_GS:
		return (operand2 == UD_R_GS);
	case X86_ST0:
		return (operand2 == UD_R_ST0);
	case X86_ST1:
		return (operand2 == UD_R_ST1);
	case X86_ST2:
		return (operand2 == UD_R_ST2);
	case X86_ST3:
		return (operand2 == UD_R_ST3);
	case X86_ST4:
		return (operand2 == UD_R_ST4);
	case X86_ST5:
		return (operand2 == UD_R_ST5);
	case X86_ST6:
		return (operand2 == UD_R_ST6);
	case X86_ST7:
		return (operand2 == UD_R_ST7);
	case X86_MM0:
		return (operand2 == UD_R_MM0);
	case X86_MM1:
		return (operand2 == UD_R_MM1);
	case X86_MM2:
		return (operand2 == UD_R_MM2);
	case X86_MM3:
		return (operand2 == UD_R_MM3);
	case X86_MM4:
		return (operand2 == UD_R_MM4);
	case X86_MM5:
		return (operand2 == UD_R_MM5);
	case X86_MM6:
		return (operand2 == UD_R_MM6);
	case X86_MM7:
		return (operand2 == UD_R_MM7);
	case X86_XMM0:
		return (operand2 == UD_R_XMM0);
	case X86_XMM1:
		return (operand2 == UD_R_XMM1);
	case X86_XMM2:
		return (operand2 == UD_R_XMM2);
	case X86_XMM3:
		return (operand2 == UD_R_XMM3);
	case X86_XMM4:
		return (operand2 == UD_R_XMM4);
	case X86_XMM5:
		return (operand2 == UD_R_XMM5);
	case X86_XMM6:
		return (operand2 == UD_R_XMM6);
	case X86_XMM7:
		return (operand2 == UD_R_XMM7);
	case X86_XMM8:
		return (operand2 == UD_R_XMM8);
	case X86_XMM9:
		return (operand2 == UD_R_XMM9);
	case X86_XMM10:
		return (operand2 == UD_R_XMM10);
	case X86_XMM11:
		return (operand2 == UD_R_XMM11);
	case X86_XMM12:
		return (operand2 == UD_R_XMM12);
	case X86_XMM13:
		return (operand2 == UD_R_XMM13);
	case X86_XMM14:
		return (operand2 == UD_R_XMM14);
	case X86_XMM15:
		return (operand2 == UD_R_XMM15);
	case X86_YMM0:
	case X86_YMM1:
	case X86_YMM2:
	case X86_YMM3:
	case X86_YMM4:
	case X86_YMM5:
	case X86_YMM6:
	case X86_YMM7:
	case X86_YMM8:
	case X86_YMM9:
	case X86_YMM10:
	case X86_YMM11:
	case X86_YMM12:
	case X86_YMM13:
	case X86_YMM14:
	case X86_YMM15:
		// ud86 doesn't support these yet
		return false;
	case X86_ZMM0:
	case X86_ZMM1:
	case X86_ZMM2:
	case X86_ZMM3:
	case X86_ZMM4:
	case X86_ZMM5:
	case X86_ZMM6:
	case X86_ZMM7:
	case X86_ZMM8:
	case X86_ZMM9:
	case X86_ZMM10:
	case X86_ZMM11:
	case X86_ZMM12:
	case X86_ZMM13:
	case X86_ZMM14:
	case X86_ZMM15:
		// ud86 doesn't support these yet
		return false;
	case X86_CR0:
		return (operand2 == UD_R_CR0);
	case X86_CR2:
		return (operand2 == UD_R_CR2);
	case X86_CR3:
		return (operand2 == UD_R_CR3);
	case X86_CR4:
		return (operand2 == UD_R_CR4);
	case X86_CR8:
		return (operand2 == UD_R_CR8);
	case X86_DR0:
		return (operand2 == UD_R_DR0);
	case X86_DR1:
		return (operand2 == UD_R_DR1);
	case X86_DR2:
		return (operand2 == UD_R_DR2);
	case X86_DR3:
		return (operand2 == UD_R_DR3);
	case X86_DR4:
		return (operand2 == UD_R_DR4);
	case X86_DR5:
		return (operand2 == UD_R_DR5);
	case X86_DR6:
		return (operand2 == UD_R_DR6);
	case X86_DR7:
		return (operand2 == UD_R_DR7);
	default:
		return false;
	}
}


bool SkipOperandsCheck(const X86Instruction* const instr)
{
	switch (instr->op)
	{
	case X86_JMPN:
		if (instr->operands[0].size == 8)
			return true;
		return false;
	case X86_NOP:
	case X86_MOVSB:
	case X86_MOVSW:
	case X86_MOVSD:
	case X86_MOVSQ:
	case X86_MOVSXD:
	case X86_CMPSB:
	case X86_CMPSW:
	case X86_CMPSD:
	case X86_CMPSQ:
	case X86_INSB:
	case X86_INSW:
	case X86_INSD:
	case X86_OUTSB:
	case X86_OUTSW:
	case X86_OUTSD:
	case X86_STOSB:
	case X86_STOSW:
	case X86_STOSD:
	case X86_STOSQ:
	case X86_LODSB:
	case X86_LODSW:
	case X86_LODSD:
	case X86_LODSQ:
	case X86_SCASB:
	case X86_SCASW:
	case X86_SCASD:
	case X86_SCASQ:
	case X86_XLAT:
	case X86_XLATB:
	case X86_JMPF:
	case X86_CALLF:
	case X86_MOVSX: // FIXME: Test on real hardware in 16bit mode
	case X86_MOVZX: // FIXME: Test on real hardware in 16bit mode.
	case X86_PEXTRB: // Maybe report a bug to ud86. For Mod==c, arg0 should be 32 or 64bit
	case X86_PINSRB: // Docs disagree with themselves. Opcode table says r8, actual docs say 1 byte from r32
	case X86_PINSRW: // ud86 is buggy, says 64bit reg instead of 32bit
	case X86_RDRAND: // ud86 is buggy, says 32bit instead of 16bit
		return true;
	default:
		return false;
	}
}


bool SkipOperandsSizeCheck(const X86Instruction* const instr, size_t operand)
{
	switch (instr->op)
	{
	case X86_JRCXZ:
	case X86_MOVLPD:
	case X86_MOVSXD:
	case X86_BOUND:
	case X86_PREFETCH:
	case X86_PREFETCHW:
	case X86_PREFETCHNTA:
	case X86_PREFETCHT0:
	case X86_PREFETCHT1:
	case X86_PREFETCHT2:
	case X86_CLFLUSH:
	case X86_LSS:
	case X86_LFS:
	case X86_LGS:
	case X86_LES:
	case X86_LDS:
	case X86_CMPXCHG8B:
	case X86_MOVLPS: // Possibly report to ud86, these are zero size
	case X86_MOVHPS:
	case X86_MOVNTPS:
	case X86_MOVNTI:
	case X86_SGDT:
	case X86_SIDT:
	case X86_LGDT:
	case X86_LIDT:
	case X86_INVLPG:
	case X86_UNPCKLPD:
	case X86_UNPCKHPD:
	case X86_MOVHPD:
	case X86_PUNPCKHBW:
	case X86_PUNPCKHWD:
	case X86_PUNPCKHDQ:
	case X86_PACKSSDW:
	case X86_PUNPCKLQDQ:
	case X86_PUNPCKHQDQ:
	case X86_PUNPCKLBW: // Maybe report a bug to ud86 as these should be dwords not qwords
	case X86_PUNPCKLWD:
	case X86_PUNPCKLDQ:
	case X86_MOVD:
	case X86_MOVQ:
	case X86_MOVQ2DQ:
	case X86_MOVDQ2Q:
	case X86_MOVNTQ:
	case X86_MOVNTDQ:
	case X86_PSHUFHW:
	case X86_PSHUFLW:
	case X86_LDDQU:
	case X86_PTEST:
		return true;
	case X86_LEA:
	case X86_ROL:
	case X86_ROR:
	case X86_RCL:
	case X86_RCR:
	case X86_SHL:
	case X86_SHR:
	case X86_SAL:
	case X86_SAR:
		if (operand == 1)
			return true;
		break;
	case X86_MOVZX:
	case X86_MOVSX:
		if (operand == 0)
			return true;
		break;
	case X86_STOSB:
	case X86_STOSW:
	case X86_STOSD:
	case X86_STOSQ:
	case X86_LODSB:
	case X86_LODSW:
	case X86_LODSD:
	case X86_LODSQ:
	case X86_SCASB:
	case X86_SCASW:
	case X86_SCASD:
	case X86_SCASQ:
	case X86_MINSS:
	case X86_MINSD:
	case X86_DIVSS:
	case X86_DIVSD:
		return true;
	case X86_PSRLW:
	case X86_PSRAW:
	case X86_PSLLW:
	case X86_PSRLD:
	case X86_PSRAD:
	case X86_PSLLD:
	case X86_PSRLQ:
	case X86_PSLLQ:
		if (operand == 0)
			return true;
		break;
	case X86_MOVSS:
	case X86_MOVHLPS:
	case X86_MOVLHPS:
	case X86_CVTPS2PI:
	case X86_CVTTSD2SI:
	case X86_MOVMSKPD:
	case X86_CVTSS2SI:
	case X86_CVTSD2SI:
	case X86_PEXTRW:
	case X86_PMOVMSKB:
	case X86_MASKMOVDQU:
	case X86_PMOVSXBW:
	case X86_PMOVSXBD:
	case X86_PMOVSXBQ:
	case X86_PMOVSXWD:
	case X86_PMOVSXWQ:
	case X86_PMOVSXDQ:
	case X86_PMOVZXBW:
	case X86_PMOVZXBD:
	case X86_PMOVZXBQ:
	case X86_PMOVZXWD:
	case X86_PMOVZXWQ:
	case X86_PMOVZXDQ:
		if (operand == 1)
			return true;
		break;
	case X86_MOV:
		// Loading a segment from memory doesn't do what the docs say.
		// When this differs from an oracle (except the hw), it's NAB!
		// (reads/writes 4 bytes when only writing 2 on real hw)
		if (instr->operands[operand].operandType != X86_MEM)
			break;
		switch (instr->operands[((~operand) & 1)].operandType)
		{
		case X86_ES:
		case X86_DS:
		case X86_SS:
		case X86_CS:
		case X86_FS:
		case X86_GS:
			return true;
		default:
			break;
		}
	case X86_UCOMISD:
		return true;
	default: break;
	}

	switch (instr->operands[operand].operandType)
	{
	case X86_SS:
	case X86_CS:
	case X86_ES:
	case X86_DS:
	case X86_FS:
	case X86_GS:
	case X86_CR0:
	case X86_CR2:
	case X86_CR3:
	case X86_CR4:
	case X86_CR8:
	case X86_DR0:
	case X86_DR1:
	case X86_DR2:
	case X86_DR3:
	case X86_DR4:
	case X86_DR5:
	case X86_DR6:
	case X86_DR7:
		return true;
	default:
		break;
	}
	return false;
}


bool CompareImmediates(const X86Operand* const operand, const struct ud_operand* const operand2, size_t size)
{
	int result = memcmp(&operand2->lval.uqword, &operand->immediate, size);
	return (result == 0);
}


static bool CompareOperand(const X86Operand* const operand1, const struct ud_operand* const operand2)
{
	switch (operand1->operandType)
	{
	case X86_IMMEDIATE:
		if ((operand2->type != UD_OP_IMM)
			&& (operand2->type != UD_OP_CONST) && (operand2->type != UD_OP_JIMM))
		{
			return false;
		}
		if (!CompareImmediates(operand1, operand2, operand1->size))
			return false;
		break;
	case X86_MEM:
		if (operand2->type != UD_OP_MEM)
			return false;
		if (!CompareRegisters(operand1->components[0], operand2->base))
		{
			if ((operand1->components[0] == X86_RIP) && (operand2->base == UD_NONE))
				return true;
			return false;
		}
		if (!CompareRegisters(operand1->components[1], operand2->index))
			return false;
		if (operand1->scale != operand2->scale)
		{
			if ((operand1->scale != 1) || (operand2->scale != 0))
				return false;
		}
		// if (!CompareImmediates(operand1, operand2, don't know how many bytes of displacement?))
		// 	return false;
		break;
	default:
		// A register of some sort.
		if (!CompareRegisters(operand1->operandType, operand2->base))
			return false;
		break;
	}

	return true;
}


static bool FpuInstr(const X86Instruction* const instr, const ud_t* const ud_obj)
{
	const struct ud_operand* operand;

	switch(instr->op)
	{
	case X86_FNSTENV:
	case X86_FCHS:
	case X86_FABS:
	case X86_FTST:
	case X86_FXAM:
	case X86_FLDENV:
	case X86_FSTENV:
	case X86_FLD1:
	case X86_FLDL2T:
	case X86_FLDL2E:
	case X86_FLDPI:
	case X86_FLDLG2:
	case X86_FLDLN2:
	case X86_FLDZ:
	case X86_F2XM1:
	case X86_FYL2X:
	case X86_FPTAN:
	case X86_FPATAN:
	case X86_FXTRACT:
	case X86_FYL2XP1:
	case X86_FSQRT:
	case X86_FSINCOS:
	case X86_FSIN:
	case X86_FCOS:
	case X86_FPREM1:
	case X86_FDECSTP:
	case X86_FINCSTP:
	case X86_FPREM:
	case X86_FRNDINT:
	case X86_FNOP:
	case X86_FSCALE:
		return true;
	case X86_FLD:
	case X86_FILD:
	case X86_FLDCW:
	case X86_FST:
	case X86_FIST:
	case X86_FISTP:
	case X86_FISTTP:
	case X86_FNSTCW:
	case X86_FSTP:
	case X86_FADD:
	case X86_FIADD:
	case X86_FADDP:
	case X86_FMUL:
	case X86_FIMUL:
	case X86_FMULP:
	case X86_FCOM:
	case X86_FCOM2:
	case X86_FICOM:
	case X86_FCOMP:
	case X86_FICOMP:
	case X86_FCOMIP:
	case X86_FUCOM:
	case X86_FUCOMIP:
	case X86_FSUB:
	case X86_FISUB:
	case X86_FSUBR:
	case X86_FISUBR:
	case X86_FSUBP:
	case X86_FSUBRP:
	case X86_FDIV:
	case X86_FIDIV:
	case X86_FDIVR:
	case X86_FIDIVR:
	case X86_FDIVP:
	case X86_FDIVRP:
	case X86_FXCH:
	case X86_FBLD:
	case X86_FBSTP:
		break;
	default:
		return false;
	}

	operand = ud_insn_opr(ud_obj, 0);
	if ((operand->type != UD_OP_MEM) && (instr->op != X86_FLD)
		&& (instr->op != X86_FLDCW) && (instr->op != X86_FST)
		&& (instr->op != X86_FNSTCW) && (instr->op != X86_FSTP)
		&& (instr->op != X86_FUCOM) && (instr->op != X86_FADDP)
		&& (instr->op != X86_FMULP) && (instr->op != X86_FSUBRP)
		&& (instr->op != X86_FSUBP) && (instr->op != X86_FDIVP)
		&& (instr->op != X86_FDIVRP) && (instr->op != X86_FCOM2))
	{
		if (!CompareOperand(&instr->operands[0], operand))
			return false;
		operand = ud_insn_opr(ud_obj, 1);
		if (!operand)
			return false;
		if (!CompareOperand(&instr->operands[1], operand))
			return false;
	}
	else
	{
		// udis86 doesn't include implicit operands, only ones that are fetched.
		if (instr->operandCount == 1)
		{
			if (!CompareOperand(&instr->operands[0], operand))
				return false;
		}
		else
		{
			if ((!CompareOperand(&instr->operands[1], operand))
				&& (!CompareOperand(&instr->operands[0], operand)))
				return false;
		}
	}

	return true;
}


void AsmFileTest::TestDisassemble(uint8_t bits)
{
	FILE* file;
	X86Instruction instr;
	uint8_t* bytes;
	size_t len;
	size_t fileLen;
	ud_t ud_obj;

	INIT_PERF_CTR(salsasm);
	INIT_PERF_CTR(udis);

	file = fopen(m_fileName, "rb");
	ASSERT_TRUE(file != NULL);

	// Get the length of the file
	fseek(file, 0, SEEK_END);
	fileLen = ftell(file);
	fseek(file, 0, SEEK_SET);

	// Allocate space and read the whole thing
	bytes = (uint8_t*)malloc(fileLen);
	ASSERT_TRUE(fread(bytes, fileLen, 1, file) != fileLen);
	SetOpcodeBytes(m_data, bytes, fileLen);
	SetOpcodeBytes(m_ud86Data, bytes, fileLen);
	free(bytes);

	// Notify ud86 that we're using a fetch callback
	ud_init(&ud_obj);
	ud_set_user_opaque_data(&ud_obj, static_cast<AsmTest*>(this));
	ud_set_input_hook(&ud_obj, AsmTest::FetchForUd86);

	// Notify ud86 which addr/operand mode to begin in
	ud_set_mode(&ud_obj, bits);

	// Disassemble the data
	len = fileLen;
	while (len)
	{
		unsigned int bytes;
		bool result;
		uint8_t operandCount;

		// NOTE: There are potentially differing number of bytes fetched before
		// the determination is made that an encoding is invalid
		// Deal with that case here to keep disassembling in sync
		if (ud_obj.pc != (fileLen - len))
		{
			// If one is behind, catch it up
			uint8_t bytes;
			while (ud_obj.pc < (fileLen - len))
			{
				GetOpcodeBytes(m_ud86Data, &bytes, 1);
				ud_obj.pc++;
			}
			while ((fileLen - len) < ud_obj.pc)
			{
				GetOpcodeBytes(m_data, &bytes, 1);
				len--;
			}
		}

		if (bits == 16)
		{
			BEGIN_PERF_CTR(salsasm)
			result = Disassemble16((uint16_t)(fileLen - len), AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
			END_PERF_CTR(salsasm)
		}
		else if (bits == 32)
		{
			BEGIN_PERF_CTR(salsasm)
			result = Disassemble32((uint32_t)(fileLen - len), AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
			END_PERF_CTR(salsasm)
		}
		else if (bits == 64)
		{
			BEGIN_PERF_CTR(salsasm)
			result = Disassemble64((uint32_t)(fileLen - len), AsmTest::Fetch, static_cast<AsmTest*>(this), &instr);
			END_PERF_CTR(salsasm)
		}

		// Watch out for going too far.
		ASSERT_TRUE(instr.length <= len);

		// Now try the oracle
		BEGIN_PERF_CTR(udis)
		bytes = ud_disassemble(&ud_obj);
		END_PERF_CTR(udis);

		len -= instr.length;
		if (SkipOperationCheck(instr.op, ud_obj.mnemonic))
			continue;

		// Don't bother comparing if there are multiple/invalid rex prefixes, udis can't handle it at all.
		if ((bits == 64) && (instr.length >= 2))
		{
			bool ignore = false;
			bool rex = false;
			for (int32_t i = 0; i < (int32_t)instr.length - 1; i++)
			{
				if ((instr.bytes[i] & 0xf0) == 0x40)
					rex = true;

				if (!rex)
					continue;

				switch (instr.bytes[i + 1])
				{
					case 0x2e:
					case 0x36:
					case 0x26:
					case 0x3e:
					case 0x64:
					case 0x65:
					case 0x66:
					case 0x67:
					case 0xf0:
					case 0xf2:
					case 0xf3:
						ignore = true;
						break;
					default:
						break;
				}

				if (ignore)
					break;
			}

			if (ignore)
				continue;
		}

		// Verify that the instructions match mnemonics
		result = CompareOperation(instr.op, ud_obj.mnemonic);
		ASSERT_TRUE(result);

		if (instr.op == X86_INVALID)
			continue;

		// Watch out for stray flags
		ASSERT_FALSE(instr.flags.insufficientLength);

// 		{
// 			char buf[512];
// 			GetInstructionString(buf, sizeof(buf), "%a %b %i %o", &instr);
// 			printf("%s\n", buf);
// 		}

		// Ensure we both agree on how many bytes the instr is
		ASSERT_TRUE(bytes == instr.length);

		// We treat operands differently...
		if (SkipOperandsCheck(&instr))
			continue;

		if (FpuInstr(&instr, &ud_obj))
			continue;

		operandCount = 0;
		for (size_t i = 0; i < 4; i++)
		{
			const struct ud_operand* const operand = ud_insn_opr(&ud_obj, (unsigned int)i);
			if (operand == NULL)
				break;
			operandCount++;
		}

		ASSERT_TRUE(operandCount == instr.operandCount);

		for (size_t i = 0; i < instr.operandCount; i++)
		{
			// Ensure counts match.
			const struct ud_operand* const operand = ud_insn_opr(&ud_obj, (unsigned int)i);
			ASSERT_TRUE(operand != NULL);

			if (!SkipOperandsSizeCheck(&instr, i))
				ASSERT_TRUE(instr.operands[i].size == (operand->size >> 3));

			bool result = CompareOperand(&instr.operands[i], operand);
			ASSERT_TRUE(result);
		}
	}

	PRINT_PERF_CTR(salsasm);
	PRINT_PERF_CTR(udis);
}

TEST_P(AsmFileTest, Disassemble16)
{
	TestDisassemble(16);
}

TEST_P(AsmFileTest, Disassemble32)
{
	TestDisassemble(32);
}

TEST_P(AsmFileTest, Disassemble64)
{
	TestDisassemble(64);
}

static const char* const g_oneByteFile16 = "test_one_byte16.bin";
static const char* const g_twoByteFile16 = "test_two_byte16.bin";
static const char* const g_threeByteFile16 = "test_three_byte16.bin";

static const char* const g_oneByteFile32 = "test_one_byte32.bin";
static const char* const g_twoByteFile32 = "test_two_byte32.bin";
static const char* const g_threeByteFile32 = "test_three_byte32.bin";

static const char* const g_oneByteFile64 = "test_one_byte64.bin";
static const char* const g_twoByteFile64 = "test_two_byte64.bin";
static const char* const g_threeByteFile64 = "test_three_byte64.bin";

static const char* const g_bochsBiosFile = "BIOS-bochs-latest.bin";

INSTANTIATE_TEST_CASE_P(DisassembleTest, AsmFileTest,
	Values(g_oneByteFile16, g_twoByteFile16, g_threeByteFile16,
		g_oneByteFile32, g_twoByteFile32, g_threeByteFile32,
		g_oneByteFile64, /* g_twoByteFile64, g_threeByteFile64, */
		g_bochsBiosFile));

#ifndef WIN32
char* strlwr(char* const s)
{
	size_t len = strlen(s);
	size_t i;

	for (i = 0; i < len; i++)
		s[i] = tolower(s[i]);

	return s;
}
#endif /* WIN32 */

#define TEST_MNEMONIC(name) \
TEST(MnemonicTest, name) \
{ \
	X86Instruction instr; \
	char instrStr[255]; \
	char* const mnemonic = strdup(#name); \
	int result; \
	strlwr(mnemonic); \
	memset(&instr, 0, sizeof(X86Instruction)); \
	instr.op = X86_ ## name; \
	GetInstructionString(instrStr, sizeof(instrStr), "%i", &instr); \
	result = strcmp(mnemonic, instrStr); \
	ASSERT_EQ(result, 0); \
} \

#undef OUT
#undef INT

TEST_MNEMONIC(AAA);
TEST_MNEMONIC(AAD);
TEST_MNEMONIC(AAM);
TEST_MNEMONIC(AAS);
TEST_MNEMONIC(ADC);
TEST_MNEMONIC(ADD);
TEST_MNEMONIC(ADDPD);
TEST_MNEMONIC(ADDPS);
TEST_MNEMONIC(ADDSD);
TEST_MNEMONIC(ADDSS);
TEST_MNEMONIC(ADDSUBPD);
TEST_MNEMONIC(ADDSUBPS);
TEST_MNEMONIC(ADX);
TEST_MNEMONIC(AESIMC);
TEST_MNEMONIC(AESENC);
TEST_MNEMONIC(AESENCLAST);
TEST_MNEMONIC(AESDEC);
TEST_MNEMONIC(AESDECLAST);
TEST_MNEMONIC(AESKEYGENASSIST);
TEST_MNEMONIC(AMX);
TEST_MNEMONIC(AND);
TEST_MNEMONIC(ANDNPD);
TEST_MNEMONIC(ANDNPS);
TEST_MNEMONIC(ANDPD);
TEST_MNEMONIC(ANDPS);
TEST_MNEMONIC(ARPL);
TEST_MNEMONIC(BLENDPD);
TEST_MNEMONIC(BLENDPS);
TEST_MNEMONIC(BLENDVPD);
TEST_MNEMONIC(BLENDVPS);
TEST_MNEMONIC(BOUND);
TEST_MNEMONIC(BSF);
TEST_MNEMONIC(BSR);
TEST_MNEMONIC(BSWAP);
TEST_MNEMONIC(BT);
TEST_MNEMONIC(BTC);
TEST_MNEMONIC(BTR);
TEST_MNEMONIC(BTS);
TEST_MNEMONIC(CALLN);
TEST_MNEMONIC(CALLF);
TEST_MNEMONIC(CBW);
TEST_MNEMONIC(CWDE);
TEST_MNEMONIC(CDQE);
TEST_MNEMONIC(CLAC);
TEST_MNEMONIC(CLC);
TEST_MNEMONIC(CLD);
TEST_MNEMONIC(CLFLUSH);
TEST_MNEMONIC(CLGI);
TEST_MNEMONIC(CLI);
TEST_MNEMONIC(CLTS);
TEST_MNEMONIC(CMC);
TEST_MNEMONIC(CMOVB);
TEST_MNEMONIC(CMOVNAE);
TEST_MNEMONIC(CMOVC);
TEST_MNEMONIC(CMOVBE);
TEST_MNEMONIC(CMOVNA);
TEST_MNEMONIC(CMOVL);
TEST_MNEMONIC(CMOVNGE);
TEST_MNEMONIC(CMOVLE);
TEST_MNEMONIC(CMOVNG);
TEST_MNEMONIC(CMOVNB);
TEST_MNEMONIC(CMOVAE);
TEST_MNEMONIC(CMOVNC);
TEST_MNEMONIC(CMOVNBE);
TEST_MNEMONIC(CMOVA);
TEST_MNEMONIC(CMOVNL);
TEST_MNEMONIC(CMOVGE);
TEST_MNEMONIC(CMOVNLE);
TEST_MNEMONIC(CMOVG);
TEST_MNEMONIC(CMOVNO);
TEST_MNEMONIC(CMOVNP);
TEST_MNEMONIC(CMOVPO);
TEST_MNEMONIC(CMOVNS);
TEST_MNEMONIC(CMOVNZ);
TEST_MNEMONIC(CMOVNE);
TEST_MNEMONIC(CMOVO);
TEST_MNEMONIC(CMOVP);
TEST_MNEMONIC(CMOVPE);
TEST_MNEMONIC(CMOVS);
TEST_MNEMONIC(CMOVZ);
TEST_MNEMONIC(CMOVE);
TEST_MNEMONIC(CMP);
TEST_MNEMONIC(CMPPD);
TEST_MNEMONIC(CMPPS);
TEST_MNEMONIC(CMPS);
TEST_MNEMONIC(CMPSB);
TEST_MNEMONIC(CMPSW);
TEST_MNEMONIC(CMPSD);
TEST_MNEMONIC(CMPSQ);
TEST_MNEMONIC(CMPSS);
TEST_MNEMONIC(CMPXCHG);
TEST_MNEMONIC(CMPXCHG8B);
TEST_MNEMONIC(CMPXCHG16B);
TEST_MNEMONIC(COMISD);
TEST_MNEMONIC(COMISS);
TEST_MNEMONIC(CPUID);
TEST_MNEMONIC(CRC32);
TEST_MNEMONIC(CVTDQ2PD);
TEST_MNEMONIC(CVTDQ2PS);
TEST_MNEMONIC(CVTPD2DQ);
TEST_MNEMONIC(CVTPD2PI);
TEST_MNEMONIC(CVTPD2PS);
TEST_MNEMONIC(CVTPI2PD);
TEST_MNEMONIC(CVTPI2PS);
TEST_MNEMONIC(CVTPS2DQ);
TEST_MNEMONIC(CVTPS2PD);
TEST_MNEMONIC(CVTPS2PI);
TEST_MNEMONIC(CVTSD2SI);
TEST_MNEMONIC(CVTSD2SS);
TEST_MNEMONIC(CVTSI2SD);
TEST_MNEMONIC(CVTSI2SS);
TEST_MNEMONIC(CVTSS2SD);
TEST_MNEMONIC(CVTSS2SI);
TEST_MNEMONIC(CVTTPD2DQ);
TEST_MNEMONIC(CVTTPD2PI);
TEST_MNEMONIC(CVTTPS2DQ);
TEST_MNEMONIC(CVTTPS2PI);
TEST_MNEMONIC(CVTTSD2SI);
TEST_MNEMONIC(CVTTSS2SI);
TEST_MNEMONIC(CVTTPS2D);
TEST_MNEMONIC(CWD);
TEST_MNEMONIC(CDQ);
TEST_MNEMONIC(CQO);
TEST_MNEMONIC(DAA);
TEST_MNEMONIC(DAS);
TEST_MNEMONIC(DEC);
TEST_MNEMONIC(DIV);
TEST_MNEMONIC(DIVPD);
TEST_MNEMONIC(DIVPS);
TEST_MNEMONIC(DIVSD);
TEST_MNEMONIC(DIVSS);
TEST_MNEMONIC(DPPD);
TEST_MNEMONIC(DPPS);
TEST_MNEMONIC(EMMS);
TEST_MNEMONIC(ENTER);
TEST_MNEMONIC(EXTRACTPS);
TEST_MNEMONIC(EXTRQ);
TEST_MNEMONIC(F2XM1);
TEST_MNEMONIC(FABS);
TEST_MNEMONIC(FADD);
TEST_MNEMONIC(FADDP);
TEST_MNEMONIC(FBLD);
TEST_MNEMONIC(FBSTP);
TEST_MNEMONIC(FCHS);
TEST_MNEMONIC(FCLEX);
TEST_MNEMONIC(FCMOVB);
TEST_MNEMONIC(FCMOVBE);
TEST_MNEMONIC(FCMOVE);
TEST_MNEMONIC(FCMOVNB);
TEST_MNEMONIC(FCMOVNBE);
TEST_MNEMONIC(FCMOVNE);
TEST_MNEMONIC(FCMOVNU);
TEST_MNEMONIC(FCMOVU);
TEST_MNEMONIC(FCOM);
TEST_MNEMONIC(FCOM2);
TEST_MNEMONIC(FCOMI);
TEST_MNEMONIC(FCOMIP);
TEST_MNEMONIC(FCOMP);
TEST_MNEMONIC(FCOMP3);
TEST_MNEMONIC(FCOMP5);
TEST_MNEMONIC(FCOMPP);
TEST_MNEMONIC(FCOS);
TEST_MNEMONIC(FDECSTP);
TEST_MNEMONIC(FDIV);
TEST_MNEMONIC(FDIVP);
TEST_MNEMONIC(FDIVR);
TEST_MNEMONIC(FDIVRP);
TEST_MNEMONIC(FEMMS);
TEST_MNEMONIC(FFREE);
TEST_MNEMONIC(FFREEP);
TEST_MNEMONIC(FIADD);
TEST_MNEMONIC(FICOM);
TEST_MNEMONIC(FICOMP);
TEST_MNEMONIC(FIDIV);
TEST_MNEMONIC(FIDIVR);
TEST_MNEMONIC(FILD);
TEST_MNEMONIC(FIMUL);
TEST_MNEMONIC(FINCSTP);
TEST_MNEMONIC(FINIT);
TEST_MNEMONIC(FIST);
TEST_MNEMONIC(FISTP);
TEST_MNEMONIC(FISTTP);
TEST_MNEMONIC(FISUB);
TEST_MNEMONIC(FISUBR);
TEST_MNEMONIC(FLD);
TEST_MNEMONIC(FLD1);
TEST_MNEMONIC(FLDCW);
TEST_MNEMONIC(FLDENV);
TEST_MNEMONIC(FLDL2E);
TEST_MNEMONIC(FLDL2T);
TEST_MNEMONIC(FLDLG2);
TEST_MNEMONIC(FLDLN2);
TEST_MNEMONIC(FLDPI);
TEST_MNEMONIC(FLDZ);
TEST_MNEMONIC(FMUL);
TEST_MNEMONIC(FMULP);
TEST_MNEMONIC(FNCLEX);
TEST_MNEMONIC(FNDISI);
TEST_MNEMONIC(FNENI);
TEST_MNEMONIC(FNINIT);
TEST_MNEMONIC(FNOP);
TEST_MNEMONIC(FNSAVE);
TEST_MNEMONIC(FNSETPM);
TEST_MNEMONIC(FNSTCW);
TEST_MNEMONIC(FNSTENV);
TEST_MNEMONIC(FNSTSW);
TEST_MNEMONIC(FPATAN);
TEST_MNEMONIC(FPREM);
TEST_MNEMONIC(FPREM1);
TEST_MNEMONIC(FPTAN);
TEST_MNEMONIC(FRNDINT);
TEST_MNEMONIC(FRSTOR);
TEST_MNEMONIC(FSAVE);
TEST_MNEMONIC(FSCALE);
TEST_MNEMONIC(FSIN);
TEST_MNEMONIC(FSINCOS);
TEST_MNEMONIC(FSQRT);
TEST_MNEMONIC(FST);
TEST_MNEMONIC(FSTCW);
TEST_MNEMONIC(FSTENV);
TEST_MNEMONIC(FSTP);
TEST_MNEMONIC(FSTP1);
TEST_MNEMONIC(FSTP8);
TEST_MNEMONIC(FSTP9);
TEST_MNEMONIC(FSTSW);
TEST_MNEMONIC(FSUB);
TEST_MNEMONIC(FSUBP);
TEST_MNEMONIC(FSUBR);
TEST_MNEMONIC(FSUBRP);
TEST_MNEMONIC(FTST);
TEST_MNEMONIC(FUCOM);
TEST_MNEMONIC(FUCOMI);
TEST_MNEMONIC(FUCOMIP);
TEST_MNEMONIC(FUCOMP);
TEST_MNEMONIC(FUCOMPP);
TEST_MNEMONIC(FWAIT);
TEST_MNEMONIC(WAIT);
TEST_MNEMONIC(FXAM);
TEST_MNEMONIC(FXCH);
TEST_MNEMONIC(FXCH4);
TEST_MNEMONIC(FXCH7);
TEST_MNEMONIC(FXRSTOR);
TEST_MNEMONIC(FXSAVE);
TEST_MNEMONIC(FXTRACT);
TEST_MNEMONIC(FYL2X);
TEST_MNEMONIC(FYL2XP1);
TEST_MNEMONIC(GETSEC);
TEST_MNEMONIC(HADDPD);
TEST_MNEMONIC(HADDPS);
TEST_MNEMONIC(HINT_NOP);
TEST_MNEMONIC(HLT);
TEST_MNEMONIC(HSUBPD);
TEST_MNEMONIC(HSUBPS);
TEST_MNEMONIC(IDIV);
TEST_MNEMONIC(IMUL);
TEST_MNEMONIC(IN);
TEST_MNEMONIC(INC);
TEST_MNEMONIC(INS);
TEST_MNEMONIC(INSB);
TEST_MNEMONIC(INSW);
TEST_MNEMONIC(INSD);
TEST_MNEMONIC(INSERTPS);
TEST_MNEMONIC(INSERTQ);
TEST_MNEMONIC(INT);
TEST_MNEMONIC(INT1);
TEST_MNEMONIC(INT3);
TEST_MNEMONIC(ICEBP);
TEST_MNEMONIC(INTO);
TEST_MNEMONIC(INVD);
TEST_MNEMONIC(INVEPT);
TEST_MNEMONIC(INVLPG);
TEST_MNEMONIC(INVLPGA);
TEST_MNEMONIC(INVVPID);
TEST_MNEMONIC(INVPCID);
TEST_MNEMONIC(IRET);
TEST_MNEMONIC(IRETD);
TEST_MNEMONIC(IRETQ);
TEST_MNEMONIC(JB);
TEST_MNEMONIC(JNAE);
TEST_MNEMONIC(JC);
TEST_MNEMONIC(JBE);
TEST_MNEMONIC(JNA);
TEST_MNEMONIC(JCXZ);
TEST_MNEMONIC(JECXZ);
TEST_MNEMONIC(JRCXZ);
TEST_MNEMONIC(JL);
TEST_MNEMONIC(JNGE);
TEST_MNEMONIC(JLE);
TEST_MNEMONIC(JNG);
TEST_MNEMONIC(JMPN);
TEST_MNEMONIC(JMPF);
TEST_MNEMONIC(JNC);
TEST_MNEMONIC(JNB);
TEST_MNEMONIC(JAE);
TEST_MNEMONIC(JA);
TEST_MNEMONIC(JNBE);
TEST_MNEMONIC(JNL);
TEST_MNEMONIC(JGE);
TEST_MNEMONIC(JNLE);
TEST_MNEMONIC(JG);
TEST_MNEMONIC(JNO);
TEST_MNEMONIC(JPO);
TEST_MNEMONIC(JNP);
TEST_MNEMONIC(JNS);
TEST_MNEMONIC(JNZ);
TEST_MNEMONIC(JNE);
TEST_MNEMONIC(JO);
TEST_MNEMONIC(JP);
TEST_MNEMONIC(JPE);
TEST_MNEMONIC(JS);
TEST_MNEMONIC(JZ);
TEST_MNEMONIC(JE);
TEST_MNEMONIC(LAHF);
TEST_MNEMONIC(LAR);
TEST_MNEMONIC(LDDQU);
TEST_MNEMONIC(LDMXCSR);
TEST_MNEMONIC(LDS);
TEST_MNEMONIC(LEA);
TEST_MNEMONIC(LEAVE);
TEST_MNEMONIC(LES);
TEST_MNEMONIC(LFENCE);
TEST_MNEMONIC(LFS);
TEST_MNEMONIC(LGDT);
TEST_MNEMONIC(LGS);
TEST_MNEMONIC(LIDT);
TEST_MNEMONIC(LLDT);
TEST_MNEMONIC(LMSW);
TEST_MNEMONIC(LODS);
TEST_MNEMONIC(LODSB);
TEST_MNEMONIC(LODSW);
TEST_MNEMONIC(LODSD);
TEST_MNEMONIC(LODSQ);
TEST_MNEMONIC(LOOP);
TEST_MNEMONIC(LOOPNZ);
TEST_MNEMONIC(LOOPNE);
TEST_MNEMONIC(LOOPZ);
TEST_MNEMONIC(LOOPE);
TEST_MNEMONIC(LSL);
TEST_MNEMONIC(LSS);
TEST_MNEMONIC(LTR);
TEST_MNEMONIC(LZCNT);
TEST_MNEMONIC(MASKMOVDQU);
TEST_MNEMONIC(MASKMOVQ);
TEST_MNEMONIC(MAXPD);
TEST_MNEMONIC(MAXPS);
TEST_MNEMONIC(MAXSD);
TEST_MNEMONIC(MAXSS);
TEST_MNEMONIC(MFENCE);
TEST_MNEMONIC(MINPD);
TEST_MNEMONIC(MINPS);
TEST_MNEMONIC(MINSD);
TEST_MNEMONIC(MINSS);
TEST_MNEMONIC(MONITOR);
TEST_MNEMONIC(MOV);
TEST_MNEMONIC(MOVAPD);
TEST_MNEMONIC(MOVAPS);
TEST_MNEMONIC(MOVBE);
TEST_MNEMONIC(MOVD);
TEST_MNEMONIC(MOVQ);
TEST_MNEMONIC(MOVDDUP);
TEST_MNEMONIC(MOVDQ2Q);
TEST_MNEMONIC(MOVDQA);
TEST_MNEMONIC(MOVDQU);
TEST_MNEMONIC(MOVHLPS);
TEST_MNEMONIC(MOVHPD);
TEST_MNEMONIC(MOVHPS);
TEST_MNEMONIC(MOVLHPS);
TEST_MNEMONIC(MOVLPD);
TEST_MNEMONIC(MOVLPS);
TEST_MNEMONIC(MOVMSKPD);
TEST_MNEMONIC(MOVMSKPS);
TEST_MNEMONIC(MOVNTDQ);
TEST_MNEMONIC(MOVNTDQA);
TEST_MNEMONIC(MOVNTI);
TEST_MNEMONIC(MOVNTPD);
TEST_MNEMONIC(MOVNTPS);
TEST_MNEMONIC(MOVNTQ);
TEST_MNEMONIC(MOVNTSS);
TEST_MNEMONIC(MOVNTSD);
TEST_MNEMONIC(MOVQ2DQ);
TEST_MNEMONIC(MOVS);
TEST_MNEMONIC(MOVSB);
TEST_MNEMONIC(MOVSW);
TEST_MNEMONIC(MOVSQ);
TEST_MNEMONIC(MOVSD);
TEST_MNEMONIC(MOVSHDUP);
TEST_MNEMONIC(MOVSLDUP);
TEST_MNEMONIC(MOVSS);
TEST_MNEMONIC(MOVSX);
TEST_MNEMONIC(MOVSXD);
TEST_MNEMONIC(MOVUPD);
TEST_MNEMONIC(MOVUPS);
TEST_MNEMONIC(MOVZX);
TEST_MNEMONIC(MPSADBW);
TEST_MNEMONIC(MUL);
TEST_MNEMONIC(MULPD);
TEST_MNEMONIC(MULPS);
TEST_MNEMONIC(MULSD);
TEST_MNEMONIC(MULSS);
TEST_MNEMONIC(MWAIT);
TEST_MNEMONIC(NEG);
TEST_MNEMONIC(NOP);
TEST_MNEMONIC(NOT);
TEST_MNEMONIC(OR);
TEST_MNEMONIC(ORPD);
TEST_MNEMONIC(ORPS);
TEST_MNEMONIC(OUT);
TEST_MNEMONIC(OUTS);
TEST_MNEMONIC(OUTSB);
TEST_MNEMONIC(OUTSW);
TEST_MNEMONIC(OUTSD);
TEST_MNEMONIC(PABSB);
TEST_MNEMONIC(PABSD);
TEST_MNEMONIC(PABSW);
TEST_MNEMONIC(PACKSSDW);
TEST_MNEMONIC(PACKSSWB);
TEST_MNEMONIC(PACKUSDW);
TEST_MNEMONIC(PACKUSWB);
TEST_MNEMONIC(PADDB);
TEST_MNEMONIC(PADDD);
TEST_MNEMONIC(PADDQ);
TEST_MNEMONIC(PADDSB);
TEST_MNEMONIC(PADDSW);
TEST_MNEMONIC(PADDUSB);
TEST_MNEMONIC(PADDUSW);
TEST_MNEMONIC(PADDW);
TEST_MNEMONIC(PALIGNR);
TEST_MNEMONIC(PAND);
TEST_MNEMONIC(PANDN);
TEST_MNEMONIC(PAUSE);
TEST_MNEMONIC(PAVGB);
TEST_MNEMONIC(PAVGW);
TEST_MNEMONIC(PBLENDVB);
TEST_MNEMONIC(PBLENDW);
TEST_MNEMONIC(PCLMULQDQ);
TEST_MNEMONIC(PCMPEQB);
TEST_MNEMONIC(PCMPEQD);
TEST_MNEMONIC(PCMPEQQ);
TEST_MNEMONIC(PCMPEQW);
TEST_MNEMONIC(PCMPESTRI);
TEST_MNEMONIC(PCMPESTRM);
TEST_MNEMONIC(PCMPGTB);
TEST_MNEMONIC(PCMPGTD);
TEST_MNEMONIC(PCMPGTQ);
TEST_MNEMONIC(PCMPGTW);
TEST_MNEMONIC(PCMPISTRI);
TEST_MNEMONIC(PCMPISTRM);
TEST_MNEMONIC(PEXTRB);
TEST_MNEMONIC(PEXTRD);
TEST_MNEMONIC(PEXTRQ);
TEST_MNEMONIC(PEXTRW);
TEST_MNEMONIC(PFNACC);
TEST_MNEMONIC(PFPNACC);
TEST_MNEMONIC(PFCMPGE);
TEST_MNEMONIC(PFMIN);
TEST_MNEMONIC(PFRCP);
TEST_MNEMONIC(PFRSQRT);
TEST_MNEMONIC(PFMAX);
TEST_MNEMONIC(PFSQRT);
TEST_MNEMONIC(PFSUB);
TEST_MNEMONIC(PFADD);
TEST_MNEMONIC(PFCMPGT);
TEST_MNEMONIC(PFRCPIT1);
TEST_MNEMONIC(PFRSQIT1);
TEST_MNEMONIC(PFSUBR);
TEST_MNEMONIC(PFACC);
TEST_MNEMONIC(PFCMPEQ);
TEST_MNEMONIC(PFMUL);
TEST_MNEMONIC(PFRCPIT2);
TEST_MNEMONIC(PMULHRW);
TEST_MNEMONIC(PSWAPD);
TEST_MNEMONIC(PAVGUSB);
TEST_MNEMONIC(PHADDD);
TEST_MNEMONIC(PHADDSW);
TEST_MNEMONIC(PHADDW);
TEST_MNEMONIC(PHMINPOSUW);
TEST_MNEMONIC(PHSUBD);
TEST_MNEMONIC(PHSUBSW);
TEST_MNEMONIC(PHSUBW);
TEST_MNEMONIC(PI2FW);
TEST_MNEMONIC(PI2FD);
TEST_MNEMONIC(PF2IW);
TEST_MNEMONIC(PF2ID);
TEST_MNEMONIC(PINSRB);
TEST_MNEMONIC(PINSRD);
TEST_MNEMONIC(PINSRQ);
TEST_MNEMONIC(PINSRW);
TEST_MNEMONIC(PMADDUBSW);
TEST_MNEMONIC(PMADDWD);
TEST_MNEMONIC(PMAXSB);
TEST_MNEMONIC(PMAXSD);
TEST_MNEMONIC(PMAXSW);
TEST_MNEMONIC(PMAXUB);
TEST_MNEMONIC(PMAXUD);
TEST_MNEMONIC(PMAXUW);
TEST_MNEMONIC(PMINSB);
TEST_MNEMONIC(PMINSD);
TEST_MNEMONIC(PMINSW);
TEST_MNEMONIC(PMINUB);
TEST_MNEMONIC(PMINUD);
TEST_MNEMONIC(PMINUW);
TEST_MNEMONIC(PMOVMSKB);
TEST_MNEMONIC(PMOVSXBD);
TEST_MNEMONIC(PMOVSXBQ);
TEST_MNEMONIC(PMOVSXBW);
TEST_MNEMONIC(PMOVSXDQ);
TEST_MNEMONIC(PMOVSXWD);
TEST_MNEMONIC(PMOVSXWQ);
TEST_MNEMONIC(PMOVZXBD);
TEST_MNEMONIC(PMOVZXBQ);
TEST_MNEMONIC(PMOVZXBW);
TEST_MNEMONIC(PMOVZXDQ);
TEST_MNEMONIC(PMOVZXWD);
TEST_MNEMONIC(PMOVZXWQ);
TEST_MNEMONIC(PMULDQ);
TEST_MNEMONIC(PMULHRSW);
TEST_MNEMONIC(PMULHUW);
TEST_MNEMONIC(PMULHW);
TEST_MNEMONIC(PMULLD);
TEST_MNEMONIC(PMULLW);
TEST_MNEMONIC(PMULUDQ);
TEST_MNEMONIC(POP);
TEST_MNEMONIC(POPA);
TEST_MNEMONIC(POPAD);
TEST_MNEMONIC(POPCNT);
TEST_MNEMONIC(POPF);
TEST_MNEMONIC(POPFQ);
TEST_MNEMONIC(POPFD);
TEST_MNEMONIC(POR);
TEST_MNEMONIC(PREFETCH);
TEST_MNEMONIC(PREFETCHW);
TEST_MNEMONIC(PREFETCHNTA);
TEST_MNEMONIC(PREFETCHT0);
TEST_MNEMONIC(PREFETCHT1);
TEST_MNEMONIC(PREFETCHT2);
TEST_MNEMONIC(PSADBW);
TEST_MNEMONIC(PSHUFB);
TEST_MNEMONIC(PSHUFD);
TEST_MNEMONIC(PSHUFHW);
TEST_MNEMONIC(PSHUFLW);
TEST_MNEMONIC(PSHUFW);
TEST_MNEMONIC(PSIGNB);
TEST_MNEMONIC(PSIGND);
TEST_MNEMONIC(PSIGNW);
TEST_MNEMONIC(PSLLD);
TEST_MNEMONIC(PSLLDQ);
TEST_MNEMONIC(PSLLQ);
TEST_MNEMONIC(PSLLW);
TEST_MNEMONIC(PSRAD);
TEST_MNEMONIC(PSRAW);
TEST_MNEMONIC(PSRLD);
TEST_MNEMONIC(PSRLDQ);
TEST_MNEMONIC(PSRLQ);
TEST_MNEMONIC(PSRLW);
TEST_MNEMONIC(PSUBB);
TEST_MNEMONIC(PSUBD);
TEST_MNEMONIC(PSUBQ);
TEST_MNEMONIC(PSUBSB);
TEST_MNEMONIC(PSUBSW);
TEST_MNEMONIC(PSUBUSB);
TEST_MNEMONIC(PSUBUSW);
TEST_MNEMONIC(PSUBW);
TEST_MNEMONIC(PTEST);
TEST_MNEMONIC(PUNPCKHBW);
TEST_MNEMONIC(PUNPCKHDQ);
TEST_MNEMONIC(PUNPCKHQDQ);
TEST_MNEMONIC(PUNPCKHWD);
TEST_MNEMONIC(PUNPCKLBW);
TEST_MNEMONIC(PUNPCKLDQ);
TEST_MNEMONIC(PUNPCKLQDQ);
TEST_MNEMONIC(PUNPCKLWD);
TEST_MNEMONIC(PUSH);
TEST_MNEMONIC(PUSHA);
TEST_MNEMONIC(PUSHAD);
TEST_MNEMONIC(PUSHF);
TEST_MNEMONIC(PUSHFQ);
TEST_MNEMONIC(PUSHFD);
TEST_MNEMONIC(PXOR);
TEST_MNEMONIC(RCL);
TEST_MNEMONIC(RCPPS);
TEST_MNEMONIC(RCPSS);
TEST_MNEMONIC(RCR);
TEST_MNEMONIC(RDMSR);
TEST_MNEMONIC(RDPMC);
TEST_MNEMONIC(RDRAND);
TEST_MNEMONIC(RDSEED);
TEST_MNEMONIC(RDTSC);
TEST_MNEMONIC(RDTSCP);
TEST_MNEMONIC(RETF);
TEST_MNEMONIC(RETN);
TEST_MNEMONIC(ROL);
TEST_MNEMONIC(ROR);
TEST_MNEMONIC(ROUNDPD);
TEST_MNEMONIC(ROUNDPS);
TEST_MNEMONIC(ROUNDSD);
TEST_MNEMONIC(ROUNDSS);
TEST_MNEMONIC(RSM);
TEST_MNEMONIC(RSQRTPS);
TEST_MNEMONIC(RSQRTSS);
TEST_MNEMONIC(SAHF);
TEST_MNEMONIC(SAL);
TEST_MNEMONIC(SHL);
TEST_MNEMONIC(SALC);
TEST_MNEMONIC(SETALC);
TEST_MNEMONIC(SAR);
TEST_MNEMONIC(SBB);
TEST_MNEMONIC(SCAS);
TEST_MNEMONIC(SCASB);
TEST_MNEMONIC(SCASW);
TEST_MNEMONIC(SCASD);
TEST_MNEMONIC(SCASQ);
TEST_MNEMONIC(SETB);
TEST_MNEMONIC(SETNAE);
TEST_MNEMONIC(SETC);
TEST_MNEMONIC(SETBE);
TEST_MNEMONIC(SETNA);
TEST_MNEMONIC(SETL);
TEST_MNEMONIC(SETNGE);
TEST_MNEMONIC(SETLE);
TEST_MNEMONIC(SETNG);
TEST_MNEMONIC(SETNB);
TEST_MNEMONIC(SETAE);
TEST_MNEMONIC(SETNC);
TEST_MNEMONIC(SETNBE);
TEST_MNEMONIC(SETA);
TEST_MNEMONIC(SETNL);
TEST_MNEMONIC(SETGE);
TEST_MNEMONIC(SETNLE);
TEST_MNEMONIC(SETG);
TEST_MNEMONIC(SETNO);
TEST_MNEMONIC(SETNP);
TEST_MNEMONIC(SETPO);
TEST_MNEMONIC(SETNS);
TEST_MNEMONIC(SETNZ);
TEST_MNEMONIC(SETNE);
TEST_MNEMONIC(SETO);
TEST_MNEMONIC(SETP);
TEST_MNEMONIC(SETPE);
TEST_MNEMONIC(SETS);
TEST_MNEMONIC(SETZ);
TEST_MNEMONIC(SETE);
TEST_MNEMONIC(SFENCE);
TEST_MNEMONIC(SGDT);
TEST_MNEMONIC(SKINIT);
TEST_MNEMONIC(SLDT);
TEST_MNEMONIC(SHLD);
TEST_MNEMONIC(SHR);
TEST_MNEMONIC(SHRD);
TEST_MNEMONIC(SHUFPD);
TEST_MNEMONIC(SHUFPS);
TEST_MNEMONIC(SIDT);
TEST_MNEMONIC(SMSW);
TEST_MNEMONIC(SQRTPD);
TEST_MNEMONIC(SQRTPS);
TEST_MNEMONIC(SQRTSD);
TEST_MNEMONIC(SQRTSS);
TEST_MNEMONIC(STAC);
TEST_MNEMONIC(STC);
TEST_MNEMONIC(STD);
TEST_MNEMONIC(STGI);
TEST_MNEMONIC(STI);
TEST_MNEMONIC(STMXCSR);
TEST_MNEMONIC(STOS);
TEST_MNEMONIC(STOSB);
TEST_MNEMONIC(STOSW);
TEST_MNEMONIC(STOSD);
TEST_MNEMONIC(STOSQ);
TEST_MNEMONIC(STR);
TEST_MNEMONIC(SUB);
TEST_MNEMONIC(SUBPD);
TEST_MNEMONIC(SUBPS);
TEST_MNEMONIC(SUBSD);
TEST_MNEMONIC(SUBSS);
TEST_MNEMONIC(SWAPGS);
TEST_MNEMONIC(SYSCALL);
TEST_MNEMONIC(SYSENTER);
TEST_MNEMONIC(SYSEXIT);
TEST_MNEMONIC(SYSRET);
TEST_MNEMONIC(TEST);
TEST_MNEMONIC(TZCNT);
TEST_MNEMONIC(UCOMISD);
TEST_MNEMONIC(UCOMISS);
TEST_MNEMONIC(UD);
TEST_MNEMONIC(UD1);
TEST_MNEMONIC(UD2);
TEST_MNEMONIC(UNPCKHPD);
TEST_MNEMONIC(UNPCKHPS);
TEST_MNEMONIC(UNPCKLPD);
TEST_MNEMONIC(UNPCKLPS);
TEST_MNEMONIC(VBLENDPD);
TEST_MNEMONIC(VBLENDPS);
TEST_MNEMONIC(VBLENDVPD);
TEST_MNEMONIC(VBLENDVPS);
TEST_MNEMONIC(VDPPD);
TEST_MNEMONIC(VDPPS);
TEST_MNEMONIC(VEXTRACTPS);
TEST_MNEMONIC(VINSERTPS);
TEST_MNEMONIC(VMOVNTDQA);
TEST_MNEMONIC(VMPSADBW);
TEST_MNEMONIC(VPACKUDSW);
TEST_MNEMONIC(VBLENDVB);
TEST_MNEMONIC(VPBLENDW);
TEST_MNEMONIC(VPCMPEQQ);
TEST_MNEMONIC(VPEXTRB);
TEST_MNEMONIC(VPEXTRD);
TEST_MNEMONIC(VPEXTRW);
TEST_MNEMONIC(VPHMINPOSUW);
TEST_MNEMONIC(VPINSRB);
TEST_MNEMONIC(VPINSRD);
TEST_MNEMONIC(VPINSRQ);
TEST_MNEMONIC(VPMAXSB);
TEST_MNEMONIC(VPMAXSD);
TEST_MNEMONIC(VMPAXUD);
TEST_MNEMONIC(VPMAXUW);
TEST_MNEMONIC(VPMINSB);
TEST_MNEMONIC(VPMINSD);
TEST_MNEMONIC(VPMINUD);
TEST_MNEMONIC(VPMINUW);
TEST_MNEMONIC(VPMOVSXBD);
TEST_MNEMONIC(VPMOVSXBQ);
TEST_MNEMONIC(VPMOVSXBW);
TEST_MNEMONIC(VPMOVSXWD);
TEST_MNEMONIC(VPMOVSXWQ);
TEST_MNEMONIC(VPMOVSXDQ);
TEST_MNEMONIC(VPMOVZXBD);
TEST_MNEMONIC(VPMOVZXBQ);
TEST_MNEMONIC(VPMOVZXBW);
TEST_MNEMONIC(VPMOVZXWD);
TEST_MNEMONIC(VPMOVZXWQ);
TEST_MNEMONIC(VPMOVZXDQ);
TEST_MNEMONIC(VPMULDQ);
TEST_MNEMONIC(VPMULLD);
TEST_MNEMONIC(VPTEST);
TEST_MNEMONIC(VROUNDPD);
TEST_MNEMONIC(VROUNDPS);
TEST_MNEMONIC(VROUNDSD);
TEST_MNEMONIC(VROUNDSS);
TEST_MNEMONIC(VPCMPESTRI);
TEST_MNEMONIC(VPCMPESTRM);
TEST_MNEMONIC(VPCMPGTQ);
TEST_MNEMONIC(VPCMPISTRI);
TEST_MNEMONIC(VPCMPISTRM);
TEST_MNEMONIC(VAESDEC);
TEST_MNEMONIC(VAESDECLAST);
TEST_MNEMONIC(VAESENC);
TEST_MNEMONIC(VAESENCLAST);
TEST_MNEMONIC(VAESIMC);
TEST_MNEMONIC(VAESKEYGENASSIST);
TEST_MNEMONIC(VPABSB);
TEST_MNEMONIC(VPABSD);
TEST_MNEMONIC(VPABSW);
TEST_MNEMONIC(VPALIGNR);
TEST_MNEMONIC(VPAHADD);
TEST_MNEMONIC(VPHADDW);
TEST_MNEMONIC(VPHADDSW);
TEST_MNEMONIC(VPHSUBD);
TEST_MNEMONIC(VPHSUBW);
TEST_MNEMONIC(VPHSUBSW);
TEST_MNEMONIC(VPMADDUBSW);
TEST_MNEMONIC(VPMULHRSW);
TEST_MNEMONIC(VPSHUFB);
TEST_MNEMONIC(VPSIGNB);
TEST_MNEMONIC(VPSIGND);
TEST_MNEMONIC(VPSIGNW);
TEST_MNEMONIC(VADDSUBPD);
TEST_MNEMONIC(VADDSUBPS);
TEST_MNEMONIC(VHADDPD);
TEST_MNEMONIC(VHADDPS);
TEST_MNEMONIC(VHSUBPD);
TEST_MNEMONIC(VHSUBPS);
TEST_MNEMONIC(VLDDQU);
TEST_MNEMONIC(VMOVDDUP);
TEST_MNEMONIC(VMOVHLPS);
TEST_MNEMONIC(VMOVSHDUP);
TEST_MNEMONIC(VMOVSLDUP);
TEST_MNEMONIC(VADDPD);
TEST_MNEMONIC(VADDSD);
TEST_MNEMONIC(VANPD);
TEST_MNEMONIC(VANDNPD);
TEST_MNEMONIC(VCMPPD);
TEST_MNEMONIC(VCMPSD);
TEST_MNEMONIC(VCOMISD);
TEST_MNEMONIC(VCVTDQ2PD);
TEST_MNEMONIC(VCVTDQ2PS);
TEST_MNEMONIC(VCVTPD2DQ);
TEST_MNEMONIC(VCVTPD2PS);
TEST_MNEMONIC(VCVTPS2DQ);
TEST_MNEMONIC(VCVTPS2PD);
TEST_MNEMONIC(VCVTSD2SI);
TEST_MNEMONIC(VCVTSD2SS);
TEST_MNEMONIC(VCVTSI2SD);
TEST_MNEMONIC(VCVTSS2SD);
TEST_MNEMONIC(VCVTTPD2DQ);
TEST_MNEMONIC(VCVTTPS2DQ);
TEST_MNEMONIC(VCVTTSD2SI);
TEST_MNEMONIC(VDIVPD);
TEST_MNEMONIC(VDIVSD);
TEST_MNEMONIC(VMASKMOVDQU);
TEST_MNEMONIC(VMAXPD);
TEST_MNEMONIC(VMAXSD);
TEST_MNEMONIC(VMINPD);
TEST_MNEMONIC(VMINSD);
TEST_MNEMONIC(VMOVAPD);
TEST_MNEMONIC(VMOVD);
TEST_MNEMONIC(VMOVQ);
TEST_MNEMONIC(VMOVDQA);
TEST_MNEMONIC(VMOVDQU);
TEST_MNEMONIC(VMOVHPD);
TEST_MNEMONIC(VMOVLPD);
TEST_MNEMONIC(VMOVMSKPD);
TEST_MNEMONIC(VMOVNTDQ);
TEST_MNEMONIC(VMOVNTPD);
TEST_MNEMONIC(VMOVSD);
TEST_MNEMONIC(VMOVUPD);
TEST_MNEMONIC(VMULPD);
TEST_MNEMONIC(VMULSD);
TEST_MNEMONIC(VORPD);
TEST_MNEMONIC(VPACKSSWB);
TEST_MNEMONIC(VPACKSSDW);
TEST_MNEMONIC(VPACKUSWB);
TEST_MNEMONIC(VPADDB);
TEST_MNEMONIC(VPADDW);
TEST_MNEMONIC(VPADDD);
TEST_MNEMONIC(VPADDQ);
TEST_MNEMONIC(VPADDSB);
TEST_MNEMONIC(VPADDSW);
TEST_MNEMONIC(VPADDUSB);
TEST_MNEMONIC(VPADDUSW);
TEST_MNEMONIC(VPAND);
TEST_MNEMONIC(VPANDN);
TEST_MNEMONIC(VPAVGB);
TEST_MNEMONIC(VPAVGW);
TEST_MNEMONIC(VPCMPEQB);
TEST_MNEMONIC(VPCMPEQW);
TEST_MNEMONIC(VPCMPEQD);
TEST_MNEMONIC(VPCMPGTB);
TEST_MNEMONIC(VPCMPGTW);
TEST_MNEMONIC(VPCMPGTD);
TEST_MNEMONIC(VPEXTSRW);
TEST_MNEMONIC(VPMADDWD);
TEST_MNEMONIC(VPMAXSW);
TEST_MNEMONIC(VPMAXUB);
TEST_MNEMONIC(VPMINSW);
TEST_MNEMONIC(VPMINUB);
TEST_MNEMONIC(VPMOVMSKB);
TEST_MNEMONIC(VPMULHUW);
TEST_MNEMONIC(VPMULHW);
TEST_MNEMONIC(VPMULLW);
TEST_MNEMONIC(VPMULUDQ);
TEST_MNEMONIC(VPOR);
TEST_MNEMONIC(VPSADBW);
TEST_MNEMONIC(VPSHUFD);
TEST_MNEMONIC(VPSHUFHW);
TEST_MNEMONIC(VPSHUFLW);
TEST_MNEMONIC(VPSLLDQ);
TEST_MNEMONIC(VPSLLW);
TEST_MNEMONIC(VPSLLD);
TEST_MNEMONIC(VPSLLQ);
TEST_MNEMONIC(VPSRAW);
TEST_MNEMONIC(VPSRAD);
TEST_MNEMONIC(VPSRLDQ);
TEST_MNEMONIC(VPSRLW);
TEST_MNEMONIC(VPSRLD);
TEST_MNEMONIC(VPSRLQ);
TEST_MNEMONIC(VPSUBB);
TEST_MNEMONIC(VPSUBW);
TEST_MNEMONIC(VPSUBD);
TEST_MNEMONIC(VPSQUBQ);
TEST_MNEMONIC(VPSUBSB);
TEST_MNEMONIC(VPSUBSW);
TEST_MNEMONIC(VPSUBUSB);
TEST_MNEMONIC(VPSUBUSW);
TEST_MNEMONIC(VPUNPCKHBW);
TEST_MNEMONIC(VPUNPCKHWD);
TEST_MNEMONIC(VPUNPCKHDQ);
TEST_MNEMONIC(VPUNPCKHQDQ);
TEST_MNEMONIC(VPUNPCKLBW);
TEST_MNEMONIC(VPUNPCKLWD);
TEST_MNEMONIC(VPUNPCKLDQ);
TEST_MNEMONIC(VPUNCKLQDQ);
TEST_MNEMONIC(VPXOR);
TEST_MNEMONIC(VSHUFPD);
TEST_MNEMONIC(VSQRTPD);
TEST_MNEMONIC(VSQRTSD);
TEST_MNEMONIC(VSUBPD);
TEST_MNEMONIC(VSUBSD);
TEST_MNEMONIC(VUCOMISD);
TEST_MNEMONIC(VUNPCKHPD);
TEST_MNEMONIC(VUNPCKHPS);
TEST_MNEMONIC(VUNPCKLPD);
TEST_MNEMONIC(VUNPCKLPS);
TEST_MNEMONIC(VXORPD);
TEST_MNEMONIC(VADDPS);
TEST_MNEMONIC(VADDSS);
TEST_MNEMONIC(VANDPS);
TEST_MNEMONIC(VANDNPS);
TEST_MNEMONIC(VCMPPS);
TEST_MNEMONIC(VCMPSS);
TEST_MNEMONIC(VCOMISS);
TEST_MNEMONIC(VCVTSI2SS);
TEST_MNEMONIC(VCVTSS2SI);
TEST_MNEMONIC(VCVTTSS2SI);
TEST_MNEMONIC(VDIVPS);
TEST_MNEMONIC(VLDMXCSR);
TEST_MNEMONIC(VMAXPS);
TEST_MNEMONIC(VMAXSS);
TEST_MNEMONIC(VMINPS);
TEST_MNEMONIC(VMINSS);
TEST_MNEMONIC(VMOVAPS);
TEST_MNEMONIC(VMOVHPS);
TEST_MNEMONIC(VMOVLHPS);
TEST_MNEMONIC(VMOVLPS);
TEST_MNEMONIC(VMOVMSKPS);
TEST_MNEMONIC(VMOVNTPS);
TEST_MNEMONIC(VMOVSS);
TEST_MNEMONIC(VMOVUPS);
TEST_MNEMONIC(VMULPS);
TEST_MNEMONIC(VMULSS);
TEST_MNEMONIC(VORPS);
TEST_MNEMONIC(VRCPPS);
TEST_MNEMONIC(VRCPSS);
TEST_MNEMONIC(VRSQRTPS);
TEST_MNEMONIC(VRSQRTSS);
TEST_MNEMONIC(VSQRTPS);
TEST_MNEMONIC(VSQRTSS);
TEST_MNEMONIC(VSTMXCSR);
TEST_MNEMONIC(VSUBPS);
TEST_MNEMONIC(VSUBSS);
TEST_MNEMONIC(VUCOMISS);
TEST_MNEMONIC(VXORPS);
TEST_MNEMONIC(VBROADCAST);
TEST_MNEMONIC(VEXTRACTF128);
TEST_MNEMONIC(VINSERTF128);
TEST_MNEMONIC(VPERMILPD);
TEST_MNEMONIC(VPERMILPS);
TEST_MNEMONIC(VPERM2F128);
TEST_MNEMONIC(VTESTPD);
TEST_MNEMONIC(VTESTPS);
TEST_MNEMONIC(VERR);
TEST_MNEMONIC(VERW);
TEST_MNEMONIC(VMCLEAR);
TEST_MNEMONIC(VMLAUNCH);
TEST_MNEMONIC(VMPTRLD);
TEST_MNEMONIC(VMPTRST);
TEST_MNEMONIC(VMREAD);
TEST_MNEMONIC(VMFUNC);
TEST_MNEMONIC(VMRESUME);
TEST_MNEMONIC(VMWRITE);
TEST_MNEMONIC(VMXOFF);
TEST_MNEMONIC(VMXON);
TEST_MNEMONIC(VMLOAD);
TEST_MNEMONIC(VMCALL);
TEST_MNEMONIC(VMMCALL);
TEST_MNEMONIC(VMRUN);
TEST_MNEMONIC(VMSAVE);
TEST_MNEMONIC(WBINVD);
TEST_MNEMONIC(WRMSR);
TEST_MNEMONIC(XABORT);
TEST_MNEMONIC(XADD);
TEST_MNEMONIC(XBEGIN);
TEST_MNEMONIC(XCHG);
TEST_MNEMONIC(XEND);
TEST_MNEMONIC(XGETBV);
TEST_MNEMONIC(XLAT);
TEST_MNEMONIC(XLATB);
TEST_MNEMONIC(XOR);
TEST_MNEMONIC(XORPD);
TEST_MNEMONIC(XORPS);
TEST_MNEMONIC(XRSTOR);
TEST_MNEMONIC(XSAVE);
TEST_MNEMONIC(XSAVEOPT);
TEST_MNEMONIC(XSETBV);
TEST_MNEMONIC(XTEST);


#define TEST_OPERAND_TYPE_NAME(name) \
	TEST(OperandTypeNameTest, name) \
{ \
	X86Instruction instr; \
	char instrStr[255]; \
	char* const operandTypeName = strdup(#name); \
	int result; \
	strlwr(operandTypeName); \
	memset(&instr, 0, sizeof(X86Instruction)); \
	instr.op = X86_MOV; \
	instr.operands[0].operandType = X86_ ## name; \
	instr.operandCount = 1; \
	GetInstructionString(instrStr, sizeof(instrStr), "%o", &instr); \
	result = strcmp(operandTypeName, instrStr); \
	ASSERT_EQ(result, 0); \
} \

#define TEST_OPERAND_TYPE_NAME_WITH_STR(tag, name) \
	TEST(OperandTypeNameTest, tag) \
{ \
	X86Instruction instr; \
	char instrStr[255]; \
	char* const operandTypeName = strdup(name); \
	int result; \
	strlwr(operandTypeName); \
	memset(&instr, 0, sizeof(X86Instruction)); \
	instr.op = X86_MOV; \
	instr.operands[0].operandType = X86_ ## tag; \
	instr.operandCount = 1; \
	GetInstructionString(instrStr, sizeof(instrStr), "%o", &instr); \
	result = strcmp(operandTypeName, instrStr); \
	ASSERT_EQ(result, 0); \
} \

TEST_OPERAND_TYPE_NAME(CS);
TEST_OPERAND_TYPE_NAME(SS);
TEST_OPERAND_TYPE_NAME(DS);
TEST_OPERAND_TYPE_NAME(ES);
TEST_OPERAND_TYPE_NAME(FS);
TEST_OPERAND_TYPE_NAME(GS);
TEST_OPERAND_TYPE_NAME(IP);
TEST_OPERAND_TYPE_NAME(EIP);
TEST_OPERAND_TYPE_NAME(RIP);
TEST_OPERAND_TYPE_NAME(FLAGS);
TEST_OPERAND_TYPE_NAME(EFLAGS);
TEST_OPERAND_TYPE_NAME(RFLAGS);
TEST_OPERAND_TYPE_NAME(AL);
TEST_OPERAND_TYPE_NAME(AH);
TEST_OPERAND_TYPE_NAME(AX);
TEST_OPERAND_TYPE_NAME(EAX);
TEST_OPERAND_TYPE_NAME(RAX);
TEST_OPERAND_TYPE_NAME(CL);
TEST_OPERAND_TYPE_NAME(CH);
TEST_OPERAND_TYPE_NAME(CX);
TEST_OPERAND_TYPE_NAME(ECX);
TEST_OPERAND_TYPE_NAME(RCX);
TEST_OPERAND_TYPE_NAME(DL);
TEST_OPERAND_TYPE_NAME(DH);
TEST_OPERAND_TYPE_NAME(DX);
TEST_OPERAND_TYPE_NAME(EDX);
TEST_OPERAND_TYPE_NAME(RDX);
TEST_OPERAND_TYPE_NAME(BL);
TEST_OPERAND_TYPE_NAME(BH);
TEST_OPERAND_TYPE_NAME(BX);
TEST_OPERAND_TYPE_NAME(EBX);
TEST_OPERAND_TYPE_NAME(RBX);
TEST_OPERAND_TYPE_NAME(SPL);
TEST_OPERAND_TYPE_NAME(SP);
TEST_OPERAND_TYPE_NAME(ESP);
TEST_OPERAND_TYPE_NAME(RSP);
TEST_OPERAND_TYPE_NAME(BPL);
TEST_OPERAND_TYPE_NAME(BP);
TEST_OPERAND_TYPE_NAME(EBP);
TEST_OPERAND_TYPE_NAME(RBP);
TEST_OPERAND_TYPE_NAME(SIL);
TEST_OPERAND_TYPE_NAME(SI);
TEST_OPERAND_TYPE_NAME(ESI);
TEST_OPERAND_TYPE_NAME(RSI);
TEST_OPERAND_TYPE_NAME(DIL);
TEST_OPERAND_TYPE_NAME(DI);
TEST_OPERAND_TYPE_NAME(EDI);
TEST_OPERAND_TYPE_NAME(RDI);
TEST_OPERAND_TYPE_NAME(R8B);
TEST_OPERAND_TYPE_NAME(R8W);
TEST_OPERAND_TYPE_NAME(R8D);
TEST_OPERAND_TYPE_NAME(R8);
TEST_OPERAND_TYPE_NAME(R9B);
TEST_OPERAND_TYPE_NAME(R9W);
TEST_OPERAND_TYPE_NAME(R9D);
TEST_OPERAND_TYPE_NAME(R9);
TEST_OPERAND_TYPE_NAME(R10B);
TEST_OPERAND_TYPE_NAME(R10W);
TEST_OPERAND_TYPE_NAME(R10D);
TEST_OPERAND_TYPE_NAME(R10);
TEST_OPERAND_TYPE_NAME(R11B);
TEST_OPERAND_TYPE_NAME(R11W);
TEST_OPERAND_TYPE_NAME(R11D);
TEST_OPERAND_TYPE_NAME(R11);
TEST_OPERAND_TYPE_NAME(R12B);
TEST_OPERAND_TYPE_NAME(R12W);
TEST_OPERAND_TYPE_NAME(R12D);
TEST_OPERAND_TYPE_NAME(R12);
TEST_OPERAND_TYPE_NAME(R13B);
TEST_OPERAND_TYPE_NAME(R13W);
TEST_OPERAND_TYPE_NAME(R13D);
TEST_OPERAND_TYPE_NAME(R13);
TEST_OPERAND_TYPE_NAME(R14B);
TEST_OPERAND_TYPE_NAME(R14W);
TEST_OPERAND_TYPE_NAME(R14D);
TEST_OPERAND_TYPE_NAME(R14);
TEST_OPERAND_TYPE_NAME(R15B);
TEST_OPERAND_TYPE_NAME(R15W);
TEST_OPERAND_TYPE_NAME(R15D);
TEST_OPERAND_TYPE_NAME(R15);
TEST_OPERAND_TYPE_NAME_WITH_STR(FPU_TAG, "fptag");
TEST_OPERAND_TYPE_NAME_WITH_STR(FPU_STATUS, "fpsw");
TEST_OPERAND_TYPE_NAME_WITH_STR(FPU_CONTROL, "fpcw");
TEST_OPERAND_TYPE_NAME_WITH_STR(FPU_DATA, "fpdata");
TEST_OPERAND_TYPE_NAME_WITH_STR(FPU_IP, "fpip");
TEST_OPERAND_TYPE_NAME_WITH_STR(FP_OPCODE, "fpopcode");
TEST_OPERAND_TYPE_NAME(ST0);
TEST_OPERAND_TYPE_NAME(ST1);
TEST_OPERAND_TYPE_NAME(ST2);
TEST_OPERAND_TYPE_NAME(ST3);
TEST_OPERAND_TYPE_NAME(ST4);
TEST_OPERAND_TYPE_NAME(ST5);
TEST_OPERAND_TYPE_NAME(ST6);
TEST_OPERAND_TYPE_NAME(ST7);
TEST_OPERAND_TYPE_NAME(MM0);
TEST_OPERAND_TYPE_NAME(MM1);
TEST_OPERAND_TYPE_NAME(MM2);
TEST_OPERAND_TYPE_NAME(MM3);
TEST_OPERAND_TYPE_NAME(MM4);
TEST_OPERAND_TYPE_NAME(MM5);
TEST_OPERAND_TYPE_NAME(MM6);
TEST_OPERAND_TYPE_NAME(MM7);
TEST_OPERAND_TYPE_NAME(MXCSR);
TEST_OPERAND_TYPE_NAME(XMM0);
TEST_OPERAND_TYPE_NAME(XMM1);
TEST_OPERAND_TYPE_NAME(XMM2);
TEST_OPERAND_TYPE_NAME(XMM3);
TEST_OPERAND_TYPE_NAME(XMM4);
TEST_OPERAND_TYPE_NAME(XMM5);
TEST_OPERAND_TYPE_NAME(XMM6);
TEST_OPERAND_TYPE_NAME(XMM7);
TEST_OPERAND_TYPE_NAME(XMM8);
TEST_OPERAND_TYPE_NAME(XMM9);
TEST_OPERAND_TYPE_NAME(XMM10);
TEST_OPERAND_TYPE_NAME(XMM11);
TEST_OPERAND_TYPE_NAME(XMM12);
TEST_OPERAND_TYPE_NAME(XMM13);
TEST_OPERAND_TYPE_NAME(XMM14);
TEST_OPERAND_TYPE_NAME(XMM15);
TEST_OPERAND_TYPE_NAME(YMM0);
TEST_OPERAND_TYPE_NAME(YMM1);
TEST_OPERAND_TYPE_NAME(YMM2);
TEST_OPERAND_TYPE_NAME(YMM3);
TEST_OPERAND_TYPE_NAME(YMM4);
TEST_OPERAND_TYPE_NAME(YMM5);
TEST_OPERAND_TYPE_NAME(YMM6);
TEST_OPERAND_TYPE_NAME(YMM7);
TEST_OPERAND_TYPE_NAME(YMM8);
TEST_OPERAND_TYPE_NAME(YMM9);
TEST_OPERAND_TYPE_NAME(YMM10);
TEST_OPERAND_TYPE_NAME(YMM11);
TEST_OPERAND_TYPE_NAME(YMM12);
TEST_OPERAND_TYPE_NAME(YMM13);
TEST_OPERAND_TYPE_NAME(YMM14);
TEST_OPERAND_TYPE_NAME(YMM15);
TEST_OPERAND_TYPE_NAME(ZMM0);
TEST_OPERAND_TYPE_NAME(ZMM1);
TEST_OPERAND_TYPE_NAME(ZMM2);
TEST_OPERAND_TYPE_NAME(ZMM3);
TEST_OPERAND_TYPE_NAME(ZMM4);
TEST_OPERAND_TYPE_NAME(ZMM5);
TEST_OPERAND_TYPE_NAME(ZMM6);
TEST_OPERAND_TYPE_NAME(ZMM7);
TEST_OPERAND_TYPE_NAME(ZMM8);
TEST_OPERAND_TYPE_NAME(ZMM9);
TEST_OPERAND_TYPE_NAME(ZMM10);
TEST_OPERAND_TYPE_NAME(ZMM11);
TEST_OPERAND_TYPE_NAME(ZMM12);
TEST_OPERAND_TYPE_NAME(ZMM13);
TEST_OPERAND_TYPE_NAME(ZMM14);
TEST_OPERAND_TYPE_NAME(ZMM15);
TEST_OPERAND_TYPE_NAME(CR0);
TEST_OPERAND_TYPE_NAME(CR2);
TEST_OPERAND_TYPE_NAME(CR3);
TEST_OPERAND_TYPE_NAME(CR4);
TEST_OPERAND_TYPE_NAME(CR8);
TEST_OPERAND_TYPE_NAME(DR0);
TEST_OPERAND_TYPE_NAME(DR1);
TEST_OPERAND_TYPE_NAME(DR2);
TEST_OPERAND_TYPE_NAME(DR3);
TEST_OPERAND_TYPE_NAME(DR4);
TEST_OPERAND_TYPE_NAME(DR5);
TEST_OPERAND_TYPE_NAME(DR6);
TEST_OPERAND_TYPE_NAME(DR7);
TEST_OPERAND_TYPE_NAME(IDTR);
TEST_OPERAND_TYPE_NAME(GDTR);
TEST_OPERAND_TYPE_NAME(LDTR);
TEST_OPERAND_TYPE_NAME(TR);
