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
#include "gtest/gtest.h"
#include "salsasm.h"
#include "udis86.h"

class AsmPrintTest: public ::testing::Test
{
protected:
	X86Instruction m_instr;
	uint8_t m_bytes[16];

	void TestOperands(size_t maxLen);
public:
	virtual void SetUp();
	virtual void TearDown();
};


void AsmPrintTest::SetUp()
{
	memset(&m_instr, 0, sizeof(X86Instruction));
	memset(m_bytes, 0, sizeof(m_bytes));
}


void AsmPrintTest::TearDown()
{
}


TEST_F(AsmPrintTest, GetInstructionStringShortTest)
{
	char* instrStr;

	GetInstructionString(NULL, 0, "%a", &m_instr);

	instrStr = (char*)calloc(1, 5);
	GetInstructionString(instrStr, 5, "test%o%a%i", &m_instr);
	GetInstructionString(instrStr, 5, "%o", &m_instr);
	GetInstructionString(instrStr, 5, "%s", &m_instr);
	GetInstructionString(instrStr, 5, "%t", &m_instr);
	GetInstructionString(instrStr, 5, "%b", &m_instr);
	m_instr.op = X86_ADD;
	GetInstructionString(instrStr, 5, "%b", &m_instr);
	GetInstructionString(instrStr, 5, "%i", &m_instr);
	free(instrStr);
}

TEST_F(AsmPrintTest, GetInstructionStringTest)
{
	char* instrStr = (char*)calloc(1, 50);
	m_instr.op = X86_INVALID;
	GetInstructionString(instrStr, 50, "%i", &m_instr);
	m_instr.op = X86_ADD;
	m_instr.flags.rep = true;
	m_instr.flags.repe = true;
	m_instr.flags.repne = true;
	m_instr.flags.lock = true;
	GetInstructionString(instrStr, 50, "%i", &m_instr);
	free(instrStr);
}

void AsmPrintTest::TestOperands(size_t maxLen)
{
	char* const instrStr = (char*)calloc(1, maxLen);

	m_instr.op = X86_ADD;
	m_instr.operands[0].operandType = X86_IMMEDIATE;
	m_instr.operands[1].operandType = X86_IMMEDIATE;
	m_instr.operands[0].components[0] = X86_RAX;
	m_instr.operands[1].components[1] = X86_RAX;
	m_instr.operands[1].scale = 2;
	GetInstructionString(instrStr, maxLen, "%o", &m_instr);

	m_instr.operands[0].size = 2;
	m_instr.operands[1].size = 8;
	GetInstructionString(instrStr, maxLen, "%o", &m_instr);

	m_instr.operands[0].operandType = X86_MEM;
	m_instr.operands[1].operandType = X86_MEM;
	m_instr.operands[1].components[1] = X86_IMMEDIATE;
	m_instr.operands[1].components[1] = X86_IMMEDIATE;
	m_instr.operands[1].immediate = -0x7f;
	GetInstructionString(instrStr, maxLen, "%o", &m_instr);

	m_instr.operands[0].operandType = X86_MEM;
	m_instr.operands[0].components[0] = X86_NONE;
	m_instr.operands[0].components[1] = X86_IMMEDIATE;
	m_instr.operands[0].immediate = 0;
	GetInstructionString(instrStr, maxLen, "%o", &m_instr);

	m_instr.operands[1].immediate = 0x80;
	GetInstructionString(instrStr, maxLen, "%o", &m_instr);

	m_instr.operands[1].components[0] = X86_RAX;
	GetInstructionString(instrStr, maxLen, "%o", &m_instr);

	free(instrStr);
}

TEST_F(AsmPrintTest, GetInstructionStringOperandsTest)
{
	for (size_t i = 0; i < 50; i++)
		TestOperands(i);
}

TEST_F(AsmPrintTest, GetInstructionStringAddressTest)
{
	char* instrStr = (char*)calloc(1, 50);

	m_instr.op = X86_ADD;
	m_instr.rip = 0x8000;
	GetInstructionString(instrStr, 50, "%a", &m_instr);

	m_instr.rip = 0x80000000;
	GetInstructionString(instrStr, 50, "%a", &m_instr);

	m_instr.rip = 0x8000000000000000;
	GetInstructionString(instrStr, 50, "%a", &m_instr);

	m_instr.rip = 0;
	GetInstructionString(instrStr, 50, "%a", &m_instr);

	free(instrStr);
}

