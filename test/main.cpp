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

// The fixture for testing class Foo.
class AsmTest : public ::testing::Test
{
protected:
	// You can remove any or all of the following functions if its body
	// is empty.

	AsmTest()
	{
		// You can do set-up work for each test here.
	}

	virtual ~AsmTest()
	{
		// You can do clean-up work that doesn't throw exceptions here.
	}

	// If the constructor and destructor are not enough for setting up
	// and cleaning up each test, you can define the following methods:

	virtual void SetUp()
	{
		// Code here will be called immediately after the constructor (right
		// before each test).
		memset(m_opcodeBytes, 0, sizeof(m_opcodeBytes));
		m_opcodeLen = 0;
	}

	virtual void TearDown()
	{
		// Code here will be called immediately after each test (right
		// before the destructor).
	}

	// Objects declared here can be used by all tests in the test case for Foo.
	size_t GetOpcodeLength() const;
	size_t GetOpcodeBytes(uint8_t* const opcode, const size_t len);
	void SetOpcodeBytes(const uint8_t* const opcode, const size_t len);


	static bool Fetch(void* ctxt, size_t len, uint8_t* result);

	// Represent the bytes for the current disassembly test
	uint8_t m_opcodeBytes[4096];
	size_t m_opcodeLen;
	size_t m_opcodeIndex;
};


size_t AsmTest::GetOpcodeLength() const
{
	return m_opcodeLen;
}


size_t AsmTest::GetOpcodeBytes(uint8_t* const opcode, const size_t len)
{
	const size_t bytesToCopy = len > m_opcodeLen ? m_opcodeLen : len;
	memcpy(opcode, &m_opcodeBytes[m_opcodeIndex], bytesToCopy);
	m_opcodeIndex += bytesToCopy;
	m_opcodeLen -= len;
	return bytesToCopy;
}


void AsmTest::SetOpcodeBytes(const uint8_t* const opcode, const size_t len)
{
	if (len > sizeof(m_opcodeBytes))
	{
		fprintf(stderr, "Tried to add too much opcode data!");
		return;
	}
	memcpy(m_opcodeBytes, opcode, len);
	m_opcodeLen = len;
	m_opcodeIndex = 0;
}


int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	int result = RUN_ALL_TESTS();

#ifdef WIN32
	printf("Press any key to continue...\n");
	while (!_kbhit());
#endif /* WIN32 */

	return result;
}


bool AsmTest::Fetch(void* ctxt, size_t len, uint8_t* result)
{
	AsmTest* asmTest = (AsmTest*)ctxt;
	size_t opcodeLen;

	// Ensure there are enough bytes available
	opcodeLen = asmTest->GetOpcodeLength();
	if (opcodeLen < len)
		return false;

	// Fetch the opcode
	asmTest->GetOpcodeBytes(result, len);

	return true;
}

#define TEST_ARITHMETIC_RM(operation, bytes, addrSize, operandSize, dest, src, component0, comonent1) \
{ \
	X86Instruction instr; \
	const size_t opcodeLen = sizeof(bytes); \
	SetOpcodeBytes(bytes, opcodeLen); \
	bool result = Disassemble ## addrSize ##(AsmTest::Fetch, this, &instr); \
	ASSERT_TRUE(result); \
	ASSERT_TRUE(instr.op == operation); \
	ASSERT_TRUE(instr.operandCount == 2); \
	ASSERT_TRUE(instr.operands[0].size == operandSize); \
	ASSERT_TRUE(instr.operands[0].operandType == dest); \
	ASSERT_TRUE(instr.operands[1].segment == X86_DS); \
	ASSERT_TRUE(instr.operands[0].components[0] == component0); \
	ASSERT_TRUE(instr.operands[0].components[1] == component1); \
	ASSERT_TRUE(instr.operands[1].operandType == src); \
	ASSERT_TRUE(instr.operands[1].size == operandSize); \
}

#define TEST_ARITHMETIC_MR(operation, bytes, addrSize, operandSize, dest, src, component0, component1) \
{ \
	X86Instruction instr; \
	const size_t opcodeLen = sizeof(bytes); \
	SetOpcodeBytes(bytes, opcodeLen); \
	bool result = Disassemble ## addrSize ##(AsmTest::Fetch, this, &instr); \
	ASSERT_TRUE(result); \
	ASSERT_TRUE(instr.op == operation); \
	ASSERT_TRUE(instr.operandCount == 2); \
	ASSERT_TRUE(instr.operands[0].size == operandSize); \
	ASSERT_TRUE(instr.operands[0].operandType == dest); \
	ASSERT_TRUE(instr.operands[0].segment == X86_DS); \
	ASSERT_TRUE(instr.operands[1].components[0] == component0); \
	ASSERT_TRUE(instr.operands[1].components[1] == component1); \
	ASSERT_TRUE(instr.operands[1].operandType == src); \
	ASSERT_TRUE(instr.operands[1].size == operandSize); \
}

#define TEST_ARITHMETIC_RR(operation, bytes, addrSize, operandSize, dest, src) \
{ \
	X86Instruction instr; \
	const size_t opcodeLen = sizeof(bytes); \
	SetOpcodeBytes(bytes, opcodeLen); \
	bool result = Disassemble ## addrSize ##(AsmTest::Fetch, this, &instr); \
	ASSERT_TRUE(result); \
	ASSERT_TRUE(instr.op == operation); \
	ASSERT_TRUE(instr.operandCount == 2); \
	ASSERT_TRUE(instr.operands[0].size == operandSize); \
	ASSERT_TRUE(instr.operands[0].operandType == dest); \
	ASSERT_TRUE(instr.operands[1].operandType == src); \
	ASSERT_TRUE(instr.operands[1].size == operandSize); \
}


TEST_F(AsmTest, DisassemblePrimaryAdd)
{
	static const uint8_t addByteMemDest[] = {0, 0, 0};
	TEST_ARITHMETIC_MR(X86_ADD, addByteMemDest, 16, 1, X86_MEM, X86_AL, X86_BX, X86_SI);
	TEST_ARITHMETIC_MR(X86_ADD, addByteMemDest, 32, 1, X86_MEM, X86_AL, X86_EBX, X86_ESI);

	static const uint8_t addByteRegDest[] = {0, 0xc0, 0};
	TEST_ARITHMETIC_RR(X86_ADD, addByteRegDest, 16, 1, X86_AL, X86_AL);
	TEST_ARITHMETIC_RR(X86_ADD, addByteRegDest, 32, 1, X86_AL, X86_AL);
}


TEST_F(AsmTest, Disassemble16)
{
	FILE* file = fopen("BIOS-bochs-latest", "rb");
	ASSERT_TRUE(file != NULL);

	while (!feof(file))
	{
		X86Instruction instr;
		bool result;
		uint8_t bytes[4096];
		size_t len;

		// Read data from the file and add it to the test helper class
		len = fread(bytes, 1, 4096, file);
		SetOpcodeBytes(bytes, len);

		// Disassemble the page.
		while (len)
		{
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
			len -= instr.length;
		}
	}
}
