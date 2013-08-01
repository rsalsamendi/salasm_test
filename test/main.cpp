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
#endif /* WIN32 */

#include "src/gtest-all.cc"
#include "salsasm.h"
#include "udis86.h"

class AsmTest;

static const char* g_fileName = NULL;

struct OpcodeData
{
	// Represent the bytes for the current disassembly test
	uint8_t* opcodeBytes;
	size_t opcodeLen;
	size_t opcodeIndex;
};

// The fixture for testing class Foo.
class AsmTest : public ::testing::Test
{
// protected:
public:
	// You can remove any or all of the following functions if its body
	// is empty.

	AsmTest()
	{
		// You can do set-up work for each test here.
	}

	virtual ~AsmTest()
	{
		// You can do clean-up work that doesn't throw exceptions here.
		if (m_data.opcodeBytes)
			free(m_data.opcodeBytes);
		if (m_ud86Data.opcodeBytes)
			free(m_ud86Data.opcodeBytes);
	}

	// If the constructor and destructor are not enough for setting up
	// and cleaning up each test, you can define the following methods:

	virtual void SetUp()
	{
		// Code here will be called immediately after the constructor (right
		// before each test).
		m_data.opcodeBytes = NULL;
		m_data.opcodeLen = 0;
		m_ud86Data.opcodeBytes = NULL;
		m_ud86Data.opcodeLen = 0;
	}

	virtual void TearDown()
	{
		// Code here will be called immediately after each test (right
		// before the destructor).
	}

	// Objects declared here can be used by all tests in the test case for Foo.
	size_t GetOpcodeLength(OpcodeData* data) const;
	size_t GetOpcodeBytes(OpcodeData* data, uint8_t* const opcode, const size_t len);
	void SetOpcodeBytes(OpcodeData* data, const uint8_t* const opcode, const size_t len);

	static bool Fetch(void* ctxt, size_t len, uint8_t* result);
	static int FetchForUd86(struct ud* u);

	OpcodeData m_data;
	OpcodeData m_ud86Data;
};


size_t AsmTest::GetOpcodeLength(OpcodeData* data) const
{
	return data->opcodeLen;
}


size_t AsmTest::GetOpcodeBytes(OpcodeData* data, uint8_t* const opcode, const size_t len)
{
	const size_t bytesToCopy = len > data->opcodeLen ? data->opcodeLen : len;
	memcpy(opcode, &data->opcodeBytes[data->opcodeIndex], bytesToCopy);
	data->opcodeIndex += bytesToCopy;
	data->opcodeLen -= len;
	return bytesToCopy;
}


void AsmTest::SetOpcodeBytes(OpcodeData* data, const uint8_t* const opcode, const size_t len)
{
	if (data->opcodeBytes)
	{
		free(data->opcodeBytes);
		data->opcodeBytes = NULL;
	}
	data->opcodeBytes = (uint8_t*)malloc(len);
	memcpy(data->opcodeBytes, opcode, len);
	data->opcodeLen = len;
	data->opcodeIndex = 0;
}


int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	int result;

	if (argc < 2)
	{
		fprintf(stderr, "Need a binary file to disassemble.\n");
		return 0;
	}

	g_fileName = argv[1];

	result = RUN_ALL_TESTS();

#ifdef WIN32
	fprintf(stderr, "Press any key to continue...\n");
	while (!_kbhit());
#endif /* WIN32 */

	return result;
}


bool AsmTest::Fetch(void* ctxt, size_t len, uint8_t* result)
{
	AsmTest* asmTest = (AsmTest*)ctxt;
	size_t opcodeLen;

	// Ensure there are enough bytes available
	opcodeLen = asmTest->GetOpcodeLength(&asmTest->m_data);
	if (opcodeLen < len)
		return false;

	// Fetch the opcode
	asmTest->GetOpcodeBytes(&asmTest->m_data, result, len);

	return true;
}


int AsmTest::FetchForUd86(struct ud* u)
{
	AsmTest* const asmTest = (AsmTest*)u->user_opaque_data;
	size_t opcodeLen;
	uint8_t result;

	// Ensure there are enough bytes available
	opcodeLen = asmTest->GetOpcodeLength(&asmTest->m_ud86Data);
	if (opcodeLen < 1)
		return UD_EOI;

	// Fetch the opcode
	asmTest->GetOpcodeBytes(&asmTest->m_ud86Data, &result, 1);

	return result;
}


#define TEST_ARITHMETIC_RM(operation, bytes, addrSize, operandSize, dest, component0, comonent1) \
{ \
	X86Instruction instr; \
	const size_t opcodeLen = sizeof(bytes); \
	SetOpcodeBytes(&m_data, bytes, opcodeLen); \
	bool result = Disassemble ## addrSize ##(0, AsmTest::Fetch, this, &instr); \
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
	SetOpcodeBytes(&m_data, bytes, opcodeLen); \
	bool result = Disassemble ## addrSize ##(0, AsmTest::Fetch, this, &instr); \
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
	SetOpcodeBytes(&m_data, bytes, opcodeLen); \
	bool result = Disassemble ## addrSize ##(0, AsmTest::Fetch, this, &instr); \
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
	SetOpcodeBytes(&m_data, bytes, opcodeLen); \
	bool result = Disassemble ## addrSize ##(0, AsmTest::Fetch, this, &instr); \
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


TEST_F(AsmTest, DisassemblePrimaryAdd)
{
	static const uint8_t addByteMemDest[] = {0, 0, 0};
	TEST_ARITHMETIC_MR(X86_ADD, addByteMemDest, 16, 1, X86_AL, X86_BX, X86_SI);
	// TEST_ARITHMETIC_MR(X86_ADD, addByteMemDest, 32, 1, X86_AL, X86_EAX, X86_NONE);

	static const uint8_t addByteRegDest[] = {0, 0xc0, 0};
	TEST_ARITHMETIC_RR(X86_ADD, addByteRegDest, 16, 1, X86_AL, X86_AL);
	// TEST_ARITHMETIC_RR(X86_ADD, addByteRegDest, 32, 1, X86_AL, X86_AL);
}

TEST_F(AsmTest, DisassemblePastBugs)
{
	static const uint8_t addByteImm[] = {4, 1};
	TEST_ARITHMETIC_RI(X86_ADD, addByteImm, 16, 1, X86_AL, 1);

	static const uint8_t orByteMemDest[] = {8, 0};
	TEST_ARITHMETIC_MR(X86_OR, orByteMemDest, 16, 1, X86_AL, X86_BX, X86_SI);

	static const uint8_t das = 0x2f;
	{
		X86Instruction instr;
		SetOpcodeBytes(&m_data, &das, 1);
		bool result = Disassemble16(0, AsmTest::Fetch, this, &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_DAS);
		ASSERT_TRUE(instr.length == 1);
	}

	static const uint8_t jmpByteImm[] = {0x70, 1};
	{
		X86Instruction instr;
		SetOpcodeBytes(&m_data, jmpByteImm, sizeof(jmpByteImm));
		bool result = Disassemble16(0, AsmTest::Fetch, this, &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_JO);
		ASSERT_TRUE(instr.length == 2);
	}

	static const uint8_t xchgRbxRax = 0x92;
	{
		X86Instruction instr;
		SetOpcodeBytes(&m_data, &xchgRbxRax, sizeof(xchgRbxRax));
		bool result = Disassemble16(0, AsmTest::Fetch, this, &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_XCHG);
		ASSERT_TRUE(instr.length == 1);
	}

	static const uint8_t movAxOffset[3] = {0xa2, 1, 0};
	{
		X86Instruction instr;
		SetOpcodeBytes(&m_data, movAxOffset, sizeof(movAxOffset));
		bool result = Disassemble16(0, AsmTest::Fetch, this, &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_MOV);
		ASSERT_TRUE(instr.length == 3);
	}

	static const uint8_t movsb = 0xa4;
	{
		X86Instruction instr;
		SetOpcodeBytes(&m_data, &movsb, sizeof(movsb));
		bool result = Disassemble16(0, AsmTest::Fetch, this, &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_MOVSB);
		ASSERT_TRUE(instr.length == 1);
	}

	static const uint8_t movsw = 0xa5;
	{
		X86Instruction instr;
		SetOpcodeBytes(&m_data, &movsw, sizeof(movsw));
		bool result = Disassemble16(0, AsmTest::Fetch, this, &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_MOVSW);
		ASSERT_TRUE(instr.length == 1);
	}

	static const uint8_t pushCs = 0x0e;
	{
		X86Instruction instr;
		SetOpcodeBytes(&m_data, &pushCs, sizeof(pushCs));
		bool result = Disassemble16(0, AsmTest::Fetch, this, &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_PUSH);
		ASSERT_TRUE(instr.length == 1);
		ASSERT_TRUE(instr.operands[0].operandType == X86_CS);
	}

	static const uint8_t decAx = 0x48;
	{
		X86Instruction instr;
		SetOpcodeBytes(&m_data, &decAx, sizeof(decAx));
		bool result = Disassemble16(0, AsmTest::Fetch, this, &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_DEC);
		ASSERT_TRUE(instr.length == 1);
		ASSERT_TRUE(instr.operands[0].operandType == X86_AX);
	}

	static const uint8_t group1Add[] = {0x81, 0xc1, 0xff, 0xff};
	{
		X86Instruction instr;
		SetOpcodeBytes(&m_data, group1Add, sizeof(group1Add));
		bool result = Disassemble16(0, AsmTest::Fetch, this, &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_ADD);
		ASSERT_TRUE(instr.length == 4);
		ASSERT_TRUE(instr.operands[0].operandType == X86_CX);
		ASSERT_TRUE(instr.operands[0].size == 2);
		ASSERT_TRUE(instr.operands[1].operandType == X86_IMMEDIATE);
		ASSERT_TRUE(instr.operands[1].immediate == 0xffffffffffffffff);
	}

	static const uint8_t xchgAxCx = 0x91;
	{
		X86Instruction instr;
		SetOpcodeBytes(&m_data, &xchgAxCx, sizeof(xchgAxCx));
		bool result = Disassemble16(0, AsmTest::Fetch, this, &instr);
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
		SetOpcodeBytes(&m_data, movModRm, sizeof(movModRm));
		bool result = Disassemble16(0, AsmTest::Fetch, this, &instr);
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
		SetOpcodeBytes(&m_data, pushImmByte, sizeof(pushImmByte));
		bool result = Disassemble16(0, AsmTest::Fetch, this, &instr);
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
		SetOpcodeBytes(&m_data, imul16, sizeof(imul16));
		bool result = Disassemble16(0, AsmTest::Fetch, this, &instr);
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
		SetOpcodeBytes(&m_data, testImmByte, sizeof(testImmByte));
		bool result = Disassemble16(0, AsmTest::Fetch, this, &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_TEST);
		ASSERT_TRUE(instr.length == 2);
		ASSERT_TRUE(instr.operands[0].operandType == X86_AL);
		ASSERT_TRUE(instr.operands[0].size == 1);
	}

	static const uint8_t movImmByte[] = {0xb8, 0x01, 0x01};
	{
		X86Instruction instr;
		SetOpcodeBytes(&m_data, movImmByte, sizeof(movImmByte));
		bool result = Disassemble16(0, AsmTest::Fetch, this, &instr);
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
		SetOpcodeBytes(&m_data, movSeg, sizeof(movSeg));
		bool result = Disassemble16(0, AsmTest::Fetch, this, &instr);
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
		SetOpcodeBytes(&m_data, lea, sizeof(lea));
		bool result = Disassemble16(0, AsmTest::Fetch, this, &instr);
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
		SetOpcodeBytes(&m_data, &popf, sizeof(popf));
		bool result = Disassemble16(0, AsmTest::Fetch, this, &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_POPF);
		ASSERT_TRUE(instr.length == 1);
	}

	static const uint8_t stosw = 0xab;
	{
		X86Instruction instr;
		SetOpcodeBytes(&m_data, &stosw, sizeof(stosw));
		bool result = Disassemble16(0, AsmTest::Fetch, this, &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_STOSW);
		ASSERT_TRUE(instr.length == 1);
	}

	static const uint8_t lodsb = 0xac;
	{
		X86Instruction instr;
		SetOpcodeBytes(&m_data, &lodsb, sizeof(lodsb));
		bool result = Disassemble16(0, AsmTest::Fetch, this, &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_LODSB);
		ASSERT_TRUE(instr.length == 1);
	}

	static const uint8_t rolCl[] = {0xd2, 0xc0};
	{
		X86Instruction instr;
		SetOpcodeBytes(&m_data, rolCl, sizeof(rolCl));
		bool result = Disassemble16(0, AsmTest::Fetch, this, &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_ROL);
		ASSERT_TRUE(instr.length == 2);
		ASSERT_TRUE(instr.operands[1].operandType == X86_CL);
		ASSERT_TRUE(instr.operands[1].size == 1);
	}

	static const uint8_t inByte[] = {0xe4, 0xff};
	{
		X86Instruction instr;
		SetOpcodeBytes(&m_data, inByte, sizeof(inByte));
		bool result = Disassemble16(0, AsmTest::Fetch, this, &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_IN);
		ASSERT_TRUE(instr.length == 2);
		ASSERT_TRUE(instr.operands[1].size == 1);
		ASSERT_TRUE(instr.operands[1].operandType == X86_IMMEDIATE);
	}
}


static bool CompareOperation(X86Operation op1, enum ud_mnemonic_code op2)
{
	switch (op1)
	{
	case X86_INVALID:
		return false;
	case X86_AAA:
		return (op2 == UD_Iaaa);
	case X86_AAD:
		return (op2 == UD_Iaad);
	case X86_AAM:
		return (op2 == UD_Iaam);
	case X86_AAS:
		return (op2 == UD_Iaas);
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
	case X86_CMOVNG:
	case X86_CMOVNB:
	case X86_CMOVAE:
	case X86_CMOVNC:
	case X86_CMOVNBE:
	case X86_CMOVA:
	case X86_CMOVNL:
	case X86_CMOVGE:
	case X86_CMOVNLE:
	case X86_CMOVG:
	case X86_CMOVNO:
	case X86_CMOVNP:
	case X86_CMOVPO:
	case X86_CMOVNS:
	case X86_CMOVNZ:
	case X86_CMOVNE:
	case X86_CMOVO:
	case X86_CMOVP:
	case X86_CMOVPE:
	case X86_CMOVS:
	case X86_CMOVZ:
	case X86_CMOVE:
		return false;
	case X86_CMP:
		return (op2 == UD_Icmp);
	case X86_CMPPD:
	case X86_CMPPS:
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
	case X86_CMPXCHG:
	case X86_CMPXCHG8B:
	case X86_CMPXCHG16B:
	case X86_COMISD:
	case X86_COMISS:
	case X86_CPUID:
	case X86_CRC32:
	case X86_CVTDQ2PD:
	case X86_CVTDQ2PS:
	case X86_CVTPD2DQ:
	case X86_CVTPD2PI:
	case X86_CVTPD2PS:
	case X86_CVTPI2PD:
	case X86_CVTPI2PS:
	case X86_CVTPS2DQ:
	case X86_CVTPS2PD:
	case X86_CVTPS2PI:
	case X86_CVTSD2SI:
	case X86_CVTSD2SS:
	case X86_CVTSI2SD:
	case X86_CVTSI2SS:
	case X86_CVTSS2SD:
	case X86_CVTSS2SI:
	case X86_CVTTPD2DQ:
	case X86_CVTTPD2PI:
	case X86_CVTTPS2DQ:
	case X86_CVTTPS2PI:
	case X86_CVTTSD2SI:
	case X86_CVTTSS2SI:
		return false;
	case X86_CWD:
		return (op2 == UD_Icwd);
	case X86_CDQ:
		return (op2 == UD_Icdq);
	case X86_CQO:
		return false;
	case X86_DAA:
		return (op2 == UD_Idaa);
	case X86_DAS:
		return (op2 == UD_Idas);
	case X86_DEC:
		return (op2 == UD_Idec);
	case X86_DIV:
		return (op2 == UD_Idiv);
	case X86_DIVPD:
	case X86_DIVPS:
	case X86_DIVSD:
	case X86_DIVSS:
	case X86_DPPD:
	case X86_DPPS:
	case X86_EMMS:
		return false;
	case X86_ENTER:
		return (op2 == UD_Ienter);
	case X86_EXTRACTPS:
	case X86_F2XM1:
	case X86_FABS:
	case X86_FADD:
	case X86_FADDP:
	case X86_FBLD:
	case X86_FBSTP:
	case X86_FCHS:
	case X86_FCLEX:
	case X86_FCMOVB:
	case X86_FCMOVBE:
	case X86_FCMOVE:
	case X86_FCMOVNB:
	case X86_FCMOVNBE:
	case X86_FCMOVNE:
	case X86_FCMOVNU:
	case X86_FCMOVU:
	case X86_FCOM:
	case X86_FCOM2:
	case X86_FCOMI:
	case X86_FCOMIP:
	case X86_FCOMP:
	case X86_FCOMP3:
	case X86_FCOMP5:
	case X86_FCOMPP:
	case X86_FCOS:
	case X86_FDECSTP:
	case X86_FDIV:
	case X86_FDIVP:
	case X86_FDIVR:
	case X86_FDIVRP:
	case X86_FFREE:
	case X86_FFREEP:
	case X86_FIADD:
	case X86_FICOM:
	case X86_FICOMP:
	case X86_FIDIV:
	case X86_FIDIVR:
	case X86_FILD:
	case X86_FIMUL:
	case X86_FINCSTP:
	case X86_FINIT:
	case X86_FIST:
	case X86_FISTP:
	case X86_FISTTP:
	case X86_FISUB:
	case X86_FISUBR:
	case X86_FLD:
	case X86_FLD1:
	case X86_FLDCW:
	case X86_FLDENV:
	case X86_FLDL2E:
	case X86_FLDL2T:
	case X86_FLDLG2:
	case X86_FLDLN2:
	case X86_FLDPI:
	case X86_FLDZ:
	case X86_FMUL:
	case X86_FMULP:
	case X86_FNCLEX:
	case X86_FNDISI:
	case X86_FNENI:
	case X86_FNINIT:
	case X86_FNOP:
	case X86_FNSAVE:
	case X86_FNSETPM:
	case X86_FNSTCW:
	case X86_FNSTENV:
	case X86_FNSTSW:
	case X86_FPATAN:
	case X86_FPREM:
	case X86_FPREM1:
	case X86_FPTAN:
	case X86_FRNDINT:
	case X86_FRSTOR:
	case X86_FSAVE:
	case X86_FSCALE:
	case X86_FSIN:
	case X86_FSINCOS:
	case X86_FSQRT:
	case X86_FST:
	case X86_FSTCW:
	case X86_FSTENV:
	case X86_FSTP:
	case X86_FSTP1:
	case X86_FSTP8:
	case X86_FSTP9:
	case X86_FSTSW:
	case X86_FSUB:
	case X86_FSUBP:
	case X86_FSUBR:
	case X86_FSUBRP:
	case X86_FTST:
	case X86_FUCOM:
	case X86_FUCOMI:
	case X86_FUCOMIP:
	case X86_FUCOMP:
	case X86_FUCOMPP:
		return false;
	case X86_WAIT:
	case X86_FWAIT:
		return (op2 == UD_Iwait);
	case X86_FXAM:
	case X86_FXCH:
	case X86_FXCH4:
	case X86_FXCH7:
	case X86_FXRSTOR:
	case X86_FXSAVE:
	case X86_FXTRACT:
	case X86_FYL2X:
	case X86_FYL2XP1:
	case X86_GETSEC:
	case X86_HADDPD:
	case X86_HADDPS:
	case X86_HINT_NOP:
	case X86_HLT:
	case X86_HSUBPD:
	case X86_HSUBPS:
		return false;
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
	case X86_JMP:
	case X86_JMPF: // Fall through
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
	case X86_LDDQU:
	case X86_LDMXCSR:
	case X86_LDS:
		return false;
	case X86_LEA:
		return (op2 == UD_Ilea);
	case X86_LEAVE:
		return (op2 == UD_Ileave);
	case X86_LES:
	case X86_LFENCE:
	case X86_LFS:
	case X86_LGDT:
	case X86_LGS:
	case X86_LIDT:
	case X86_LLDT:
	case X86_LMSW:
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
	case X86_LSS:
	case X86_LTR:
	case X86_MASKMOVDQU:
	case X86_MASKMOVQ:
	case X86_MAXPD:
	case X86_MAXPS:
	case X86_MAXSD:
	case X86_MAXSS:
	case X86_MFENCE:
	case X86_MINPD:
	case X86_MINPS:
	case X86_MINSD:
	case X86_MINSS:
		return false;
	case X86_MONITOR:
		return (op2 == UD_Imonitor);
	case X86_MOV:
		return (op2 == UD_Imov);
	case X86_MOVAPD:
	case X86_MOVAPS:
	case X86_MOVBE:
	case X86_MOVD:
	case X86_MOVQ:
	case X86_MOVDDUP:
	case X86_MOVDQ2Q:
	case X86_MOVDQA:
	case X86_MOVDQU:
	case X86_MOVHLPS:
	case X86_MOVHPD:
	case X86_MOVHPS:
	case X86_MOVLHPS:
	case X86_MOVLPD:
	case X86_MOVLPS:
	case X86_MOVMSKPD:
	case X86_MOVMSKPS:
	case X86_MOVNTDQ:
	case X86_MOVNTDQA:
	case X86_MOVNTI:
	case X86_MOVNTPD:
	case X86_MOVNTPS:
	case X86_MOVNTQ:
	case X86_MOVQ2DQ:
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
	case X86_MOVSHDUP:
	case X86_MOVSS:
	case X86_MOVSX:
	case X86_MOVSXD:
	case X86_MOVUPD:
	case X86_MOVUPS:
	case X86_MOVZX:
	case X86_MPSADBW:
		return false;
	case X86_MUL:
		return (op2 == UD_Imul);
	case X86_MULPD:
	case X86_MULPS:
	case X86_MULSD:
	case X86_MULSS:
	case X86_MWAIT:
		return false;
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
	case X86_PABSD:
	case X86_PABSW:
	case X86_PACKSSDW:
	case X86_PACKSSWB:
	case X86_PACKUSDW:
	case X86_PACKUSWB:
	case X86_PADDB:
	case X86_PADDD:
	case X86_PADDQ:
	case X86_PADDSB:
	case X86_PADDSW:
	case X86_PADDUSB:
	case X86_PADDUSW:
	case X86_PADDW:
	case X86_PALIGNR:
	case X86_PAND:
	case X86_PANDN:
	case X86_PAUSE:
	case X86_PAVGB:
	case X86_PAVGW:
	case X86_PBLENDVB:
	case X86_PBLENDW:
	case X86_PCMPEQB:
	case X86_PCMPEQD:
	case X86_PCMPEQQ:
	case X86_PCMPEQW:
	case X86_PCMPESTRI:
	case X86_PCMPESTRM:
	case X86_PCMPGTB:
	case X86_PCMPGTD:
	case X86_PCMPGTQ:
	case X86_PCMPGTW:
	case X86_PCMPISTRI:
	case X86_PCMPISTRM:
	case X86_PEXTRB:
	case X86_PEXTRD:
	case X86_PEXTRQ:
	case X86_PEXTRW:
	case X86_PHADDD:
	case X86_PHADDSW:
	case X86_PHADDW:
	case X86_PHMINPOSUW:
	case X86_PHSUBD:
	case X86_PHSUBSW:
	case X86_PHSUBW:
	case X86_PINSRB:
	case X86_PINSRD:
	case X86_PINSRQ:
	case X86_PINSRW:
	case X86_PMADDUBSW:
	case X86_PMADDWD:
	case X86_PMAXSB:
	case X86_PMAXSD:
	case X86_PMAXSW:
	case X86_PMAXUB:
	case X86_PMAXUD:
	case X86_PMAXUW:
	case X86_PMINSB:
	case X86_PMINSD:
	case X86_PMINSW:
	case X86_PMINUB:
	case X86_PMINUD:
	case X86_PMINUW:
	case X86_PMOVMSKB:
	case X86_PMOVSXBD:
	case X86_PMOVSXBQ:
	case X86_PMOVSXBW:
	case X86_PMOVSXDQ:
	case X86_PMOVSXWD:
	case X86_PMOVSXWQ:
	case X86_PMOVZXBD:
	case X86_PMOVZXBQ:
	case X86_PMOVZXBW:
	case X86_PMOVZXDQ:
	case X86_PMOVZXWD:
	case X86_PMOVZXWQ:
	case X86_PMULDQ:
	case X86_PMULHRSW:
	case X86_PMULHUW:
	case X86_PMULHW:
	case X86_PMULLD:
	case X86_PMULLW:
	case X86_PMULUDQ:
		return false;
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
	case X86_PREFETCHNTA:
	case X86_PREFETCHT0:
	case X86_PREFETCHT1:
	case X86_PREFETCHT2:
	case X86_PSADBW:
	case X86_PSHUFB:
	case X86_PSHUFD:
	case X86_PSHUFHW:
	case X86_PSHUFLW:
	case X86_PSHUFW:
	case X86_PSIGNB:
	case X86_PSIGND:
	case X86_PSIGNW:
	case X86_PSLLD:
	case X86_PSLLDQ:
	case X86_PSLLQ:
	case X86_PSLLW:
	case X86_PSRAD:
	case X86_PSRAW:
	case X86_PSRLD:
	case X86_PSRLDQ:
	case X86_PSRLQ:
	case X86_PSRLW:
	case X86_PSUBB:
	case X86_PSUBD:
	case X86_PSUBQ:
	case X86_PSUBSB:
	case X86_PSUBSW:
	case X86_PSUBUSB:
	case X86_PSUBUSW:
	case X86_PSUBW:
	case X86_PTEST:
	case X86_PUNPCKHBW:
	case X86_PUNPCKHDQ:
	case X86_PUNPCKHQDQ:
	case X86_PUNPCKHWD:
	case X86_PUNPCKLBW:
	case X86_PUNPCKLDQ:
	case X86_PUNPCKLQDQ:
	case X86_PUNPCKLWD:
		return false;
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
	case X86_RCR:
		return (op2 == UD_Ircr);
	case X86_RDMSR:
		return (op2 == UD_Irdmsr);
	case X86_RDPMC:
		return (op2 == UD_Irdpmc);
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
	case X86_ROUNDPS:
	case X86_ROUNDSD:
	case X86_ROUNDSS:
	case X86_RSM:
	case X86_RSQRTPS:
	case X86_RSQRTSS:
		return false;
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
	case X86_SETNAE:
	case X86_SETC:
	case X86_SETBE:
	case X86_SETNA:
	case X86_SETL:
	case X86_SETNGE:
	case X86_SETLE:
	case X86_SETNG:
	case X86_SETNB:
	case X86_SETAE:
	case X86_SETNC:
	case X86_SETNBE:
	case X86_SETA:
	case X86_SETNL:
	case X86_SETGE:
	case X86_SETNLE:
	case X86_SETG:
	case X86_SETNO:
	case X86_SETNP:
	case X86_SETPO:
	case X86_SETNS:
	case X86_SETNZ:
	case X86_SETNE:
	case X86_SETO:
	case X86_SETP:
	case X86_SETPE:
	case X86_SETS:
	case X86_SETZ:
	case X86_SETE:
	case X86_SFENCE:
	case X86_SGDT:
	case X86_SHLD:
		return false;
	case X86_SHR:
		return (op2 == UD_Ishr);
	case X86_SHRD:
	case X86_SHUFPD:
	case X86_SHUFPS:
	case X86_SIDT:
	case X86_SMSW:
	case X86_SQRTPD:
	case X86_SQRTPS:
	case X86_SQRTSD:
	case X86_SQRTSS:
		return false;
	case X86_STC:
		return (op2 == UD_Istc);
	case X86_STD:
		return (op2 == UD_Istd);
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
		return false;
	case X86_SUB:
		return (op2 == UD_Isub);
	case X86_SUBPD:
	case X86_SUBPS:
	case X86_SUBSD:
	case X86_SUBSS:
	case X86_SWAPGS:
	case X86_SYSCALL:
	case X86_SYSENTER:
	case X86_SYSRET:
		return false;
	case X86_TEST:
		return (op2 == UD_Itest);
	case X86_UCOMISD:
	case X86_UCOMISS:
	case X86_UD:
	case X86_UD2:
	case X86_UNPCKHPD:
	case X86_UNPCKHPS:
	case X86_UNPCKLPD:
	case X86_UNPCKLPS:
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
	case X86_VERR:
	case X86_VERW:
	case X86_VMCALL:
	case X86_VMCLEAR:
	case X86_VMLAUNCH:
	case X86_VMPTRLD:
	case X86_VMPTRST:
	case X86_VMREAD:
	case X86_VMFUNC:
	case X86_VMRESUME:
	case X86_VMWRITE:
	case X86_VMXOFF:
	case X86_VMXON:
	case X86_VMLOAD:
	case X86_VMMCALL:
	case X86_VMRUN:
	case X86_VMSAVE:
	case X86_WBINVD:
	case X86_WRMSR:
		return false;
	case X86_XADD:
		return (op2 == UD_Ixadd);
	case X86_XCHG:
		return (op2 == UD_Ixchg);
	case X86_XGETBV:
	case X86_XLAT:
	case X86_XLATB:
		return (op2 == UD_Ixlatb);
	case X86_XOR:
		return (op2 == UD_Ixor);
	case X86_XORPD:
	case X86_XORPS:
	case X86_XRSTOR:
	case X86_XSAVE:
	case X86_XSETBV:
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
	case X86_SI:
		return (operand2 == UD_R_SI);
	case X86_ESI:
		return (operand2 == UD_R_ESI);
	case X86_RSI:
		return (operand2 == UD_R_RSI);
	case X86_DI:
		return (operand2 == UD_R_DI);
	case X86_EDI:
		return (operand2 == UD_R_EDI);
	case X86_RDI:
		return (operand2 == UD_R_RDI);
	case X86_SP:
		return (operand2 == UD_R_SP);
	case X86_ESP:
		return (operand2 == UD_R_ESP);
	case X86_RSP:
		return (operand2 == UD_R_RSP);
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
	default:
		return false;
	}
}


bool SkipOperandsCheck(X86Operation op)
{
	switch (op)
	{
	case X86_MOVSB:
	case X86_MOVSW:
	case X86_MOVSD:
	case X86_MOVSQ:
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
		return true;
	default:
		return false;
	}
}


bool SkipOperandsSizeCheck(const X86Instruction* const instr, size_t operand)
{
	switch (instr->op)
	{
	case X86_BOUND:
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
		return true;
	}
	return false;
}


bool CompareImmediates(const X86Operand* const operand, const struct ud_operand* const operand2)
{
	switch (operand2->size)
	{
	case 8:
		return (operand2->lval.sbyte == (int8_t)operand->immediate);
	case 16:
		return (operand2->lval.sword == (int16_t)operand->immediate);
	case 32:
		return (operand2->lval.sdword == (int32_t)operand->immediate);
	case 64:
		return (operand2->lval.sqword == operand->immediate);
	default:
		// Evil instrs like lea have zero operand size but still an imm component
		if (operand->immediate == SIGN_EXTEND64(operand2->lval.uqword, operand->size))
			return true;
		return false;
	}
}


TEST_F(AsmTest, Disassemble16)
{
	FILE* file;
	X86Instruction instr;
	uint8_t* bytes;
	size_t len;
	size_t fileLen;
	ud_t ud_obj;

	file = fopen(g_fileName, "rb");
	ASSERT_TRUE(file != NULL);

	// Get the length of the file
	fseek(file, 0, SEEK_END);
	fileLen = ftell(file);
	fseek(file, 0, SEEK_SET);

	// Allocate space and read the whole thing
	bytes = (uint8_t*)malloc(fileLen);
	ASSERT_TRUE(fread(bytes, fileLen, 1, file) != fileLen);
	SetOpcodeBytes(&m_data, bytes, fileLen);
	SetOpcodeBytes(&m_ud86Data, bytes, fileLen);

	// Notify ud86 that we're in 16bit mode
	ud_set_mode(&ud_obj, 16);

	// Notify ud86 that we're using a fetch callback
	ud_init(&ud_obj);
	ud_set_user_opaque_data(&ud_obj, this);
	ud_set_input_hook(&ud_obj, AsmTest::FetchForUd86);

	// Disassemble the data
	len = fileLen;
	while (len)
	{
		unsigned int bytes;
		bool result;

		result = Disassemble16((uint16_t)(fileLen - len), AsmTest::Fetch, this, &instr);
		ASSERT_TRUE(result);

		if (instr.flags & X86_FLAG_INSUFFICIENT_LENGTH)
		{
			// FIXME: This means we reached the end of the page but
			// there is an instruction across the boundary, need to handle this case
			break;
		}

		// Watch out for going too far.
		ASSERT_TRUE(instr.length <= len);

		// Now try the oracle
		bytes = ud_disassemble(&ud_obj);

		// Ensure we both agree on how many bytes the instr is
		ASSERT_TRUE(bytes == instr.length);

		// Verify that the instructions match mnemonics
		result = CompareOperation(instr.op, ud_obj.mnemonic);
		ASSERT_TRUE(result);

		len -= instr.length;

		// We treat operands differently...
		if (SkipOperandsCheck(instr.op))
			continue;

		for (size_t i = 0; i < instr.operandCount; i++)
		{
			// Ensure counts match.
			bool result;
			const struct ud_operand* const operand = ud_insn_opr(&ud_obj, (unsigned int)i);
			ASSERT_TRUE(operand != NULL);

			if (!SkipOperandsSizeCheck(&instr, i))
				ASSERT_TRUE(instr.operands[i].size == (operand->size >> 3));

			switch (instr.operands[i].operandType)
			{
			case X86_IMMEDIATE:
				ASSERT_TRUE ((operand->type == UD_OP_IMM)
					|| (operand->type == UD_OP_CONST) || (operand->type == UD_OP_JIMM));
				result = CompareImmediates(&instr.operands[i], operand);
				ASSERT_TRUE(result);
				break;
			case X86_MEM:
				ASSERT_TRUE (operand->type == UD_OP_MEM);
				ASSERT_TRUE(CompareRegisters(instr.operands[i].components[0], operand->base));
				ASSERT_TRUE(CompareRegisters(instr.operands[i].components[1], operand->index));
				ASSERT_TRUE(instr.operands[i].scale == operand->scale);
				result = CompareImmediates(&instr.operands[i], operand);
				ASSERT_TRUE(result);
				break;
			default:
				// A register of some sort.
				bool match = CompareRegisters(instr.operands[i].operandType, operand->base);
				ASSERT_TRUE(match);
			}
		}
	}
}
