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
	for (size_t i = 0; i < len; i++)
		fprintf(stderr, "%.2x ", opcode[i]);
	fprintf(stderr, "\n");
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
	bool result = Disassemble ## addrSize ##(AsmTest::Fetch, this, &instr); \
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
	bool result = Disassemble ## addrSize ##(AsmTest::Fetch, this, &instr); \
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
	bool result = Disassemble ## addrSize ##(AsmTest::Fetch, this, &instr); \
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
	bool result = Disassemble ## addrSize ##(AsmTest::Fetch, this, &instr); \
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
		bool result = Disassemble16(AsmTest::Fetch, this, &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_DAS);
		ASSERT_TRUE(instr.length == 1);
	}

	static const uint8_t jmpByteImm[] = {0x70, 1};
	{
		X86Instruction instr;
		SetOpcodeBytes(&m_data, jmpByteImm, sizeof(jmpByteImm));
		bool result = Disassemble16(AsmTest::Fetch, this, &instr);
		ASSERT_TRUE(result);
		ASSERT_TRUE(instr.op == X86_JO);
		ASSERT_TRUE(instr.length == 2);
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
	case X86_CMPSB:
	case X86_CMPSW:
	case X86_CMPSD:
	case X86_CMPSQ:
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
	case X86_CWD:
	case X86_CDQ:
	case X86_CQO:
	case X86_DAA:
		return (op2 == UD_Idaa);
	case X86_DAS:
		return (op2 == UD_Idas);
	case X86_DEC:
		return (op2 == UD_Idec);
	case X86_DIV:
	case X86_DIVPD:
	case X86_DIVPS:
	case X86_DIVSD:
	case X86_DIVSS:
	case X86_DPPD:
	case X86_DPPS:
	case X86_EMMS:
	case X86_ENTER:
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
	case X86_FWAIT:
	case X86_WAIT:
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
	case X86_IDIV:
	case X86_IMUL:
		return (op2 == UD_Iimul);
	case X86_IN:
	case X86_INC:
		return (op2 == UD_Iinc);
	case X86_INS:
	case X86_INSB:
	case X86_INSW:
	case X86_INSD:
	case X86_INSERTPS:
	case X86_INT:
	case X86_INT1:
	case X86_INT3:
	case X86_ICEBP:
	case X86_INTO:
	case X86_INVD:
	case X86_INVEPT:
	case X86_INVLPG:
	case X86_INVVPID:
	case X86_IRET:
	case X86_IRETD:
	case X86_IRETQ:
		return false;
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
	case X86_JNP:
	case X86_JNS:
		return false;
	case X86_JNZ:
	case X86_JNE: // Fall through
		return (op2 == UD_Ijnz);
	case X86_JO:
		return (op2 == UD_Ijo);
	case X86_JP:
	case X86_JPE:
	case X86_JS:
		return false;
	case X86_JZ:
	case X86_JE: // Fall through
		return (op2 == UD_Ijz);
	case X86_LAHF:
	case X86_LAR:
	case X86_LDDQU:
	case X86_LDMXCSR:
	case X86_LDS:
	case X86_LEA:
	case X86_LEAVE:
	case X86_LES:
	case X86_LFENCE:
	case X86_LFS:
	case X86_LGDT:
	case X86_LGS:
	case X86_LIDT:
	case X86_LLDT:
	case X86_LMSW:
	case X86_LODS:
	case X86_LODSB:
	case X86_LODSW:
	case X86_LODSD:
	case X86_LODSQ:
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
	case X86_MOVSB:
	case X86_MOVSW:
	case X86_MOVSQ:
	case X86_MOVSD:
	case X86_MOVSHDUP:
	case X86_MOVSS:
	case X86_MOVSX:
	case X86_MOVSXD:
	case X86_MOVUPD:
	case X86_MOVUPS:
	case X86_MOVZX:
	case X86_MPSADBW:
	case X86_MUL:
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
	case X86_ORPS:
	case X86_OUT:
	case X86_OUTS:
	case X86_OUTSB:
	case X86_OUTSW:
	case X86_OUTSD:
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
	case X86_POPF:
	case X86_POPFQ:
	case X86_POPFD:
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
		// return (op2 == UD_Ipushf);
	case X86_PUSHFQ:
	case X86_PUSHFD:
	case X86_PXOR:
	case X86_RCL:
	case X86_RCPPS:
	case X86_RCR:
	case X86_RDMSR:
	case X86_RDPMC:
	case X86_RDTSC:
	case X86_RDTSCP:
	case X86_RETF:
	case X86_RETN:
	case X86_ROL:
	case X86_ROR:
	case X86_ROUNDPD:
	case X86_ROUNDPS:
	case X86_ROUNDSD:
	case X86_ROUNDSS:
	case X86_RSM:
	case X86_RSQRTPS:
	case X86_RSQRTSS:
	case X86_SAHF:
	case X86_SAL:
	case X86_SHL:
	case X86_SALC:
	case X86_SETALC:
	case X86_SAR:
		return false;
	case X86_SBB:
		return (op2 == UD_Isbb);
	case X86_SCAS:
	case X86_SCASB:
	case X86_SCASW:
	case X86_SCASD:
	case X86_SCASQ:
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
	case X86_SHR:
	case X86_SHRD:
	case X86_SHUFPD:
	case X86_SHUFPS:
	case X86_SIDT:
	case X86_SMSW:
	case X86_SQRTPD:
	case X86_SQRTPS:
	case X86_SQRTSD:
	case X86_SQRTSS:
	case X86_STC:
	case X86_STD:
	case X86_STI:
	case X86_STMXCSR:
	case X86_STOS:
	case X86_STOSB:
	case X86_STOSW:
	case X86_STOSD:
	case X86_STOSQ:
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
	case X86_XADD:
	case X86_XCHG:
	case X86_XGETBV:
	case X86_XLAT:
	case X86_XLATB:
		return false;
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


TEST_F(AsmTest, Disassemble16)
{
	FILE* file;
	X86Instruction instr;
	uint8_t* bytes;
	size_t len;
	ud_t ud_obj;

	file = fopen(g_fileName, "rb");
	ASSERT_TRUE(file != NULL);

	// Get the length of the file
	fseek(file, 0, SEEK_END);
	len = ftell(file);
	fseek(file, 0, SEEK_SET);

	// Allocate space and read the whole thing
	bytes = (uint8_t*)malloc(len);
	ASSERT_TRUE(fread(bytes, len, 1, file) != len);
	SetOpcodeBytes(&m_data, bytes, len);
	SetOpcodeBytes(&m_ud86Data, bytes, len);

	// Notify ud86 that we're in 16bit mode
	ud_set_mode(&ud_obj, 16);

	// Notify ud86 that we're using a fetch callback
	ud_init(&ud_obj);
	ud_set_user_opaque_data(&ud_obj, this);
	ud_set_input_hook(&ud_obj, AsmTest::FetchForUd86);

	// Disassemble the data
	while (len)
	{
		unsigned int bytes;
		bool result;

		result = Disassemble16(AsmTest::Fetch, this, &instr);
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
	}
}
