#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "lab1.h"

extern void store2(struct cpu *cpu, uint16_t data, uint16_t addr);
extern uint16_t load2(struct cpu *cpu, uint16_t addr);

/* set all registers, flags, and memory to zero
 */
void zerocpu(struct cpu *cpu)
{   
    cpu->Z = cpu->N = 0;
    cpu->PC = cpu->SP = 0;
    for (int i = 0; i < 8; i++)
        cpu->R[i] = 0;
    memset(cpu->memory, 0, 64*1024);
}

/* 0000 : 50f1      : ADD R3 + R6 -> R1
 */
void test1(struct cpu *cpu)
{
    zerocpu(cpu);
    cpu->R[3] = 5;
    cpu->R[6] = 10;
    store2(cpu, 0x5073, 0);

    int val = emulate(cpu);
    assert(val == 0);
    assert(cpu->R[1] == 15);
}

// tests set register instruction by setting R[1] to 65535
void testSet(struct cpu *cpu)
{
    zerocpu(cpu);
    uint16_t instruction_addr = 0x1234;
    cpu->PC = instruction_addr;

    // instruction to set register 1
    uint16_t insn = 0x1001;
    store2(cpu, insn, instruction_addr);
    load2(cpu, instruction_addr);

    uint16_t data_addr = 0x1236;
    cpu->PC = data_addr;

    // 65535 in hexadecimal, register a should be set to this value
    uint16_t data = 0xFFFF;

    store2(cpu, data, data_addr);

    cpu->PC = instruction_addr;
    load2(cpu, cpu->PC);
    int val = emulate(cpu);
    assert(val == 0);
    assert(cpu->PC == 0x1238);
    assert(cpu->R[1] == 65535);
}

// tests load from memory instruction by loading 0xFFFF(65535) from memory address 0x2000 to R[1]
void testLoadFromMemory_16bit(struct cpu *cpu)
{
    zerocpu(cpu);
    // pointing PC to desired instruction address
    uint16_t instruction_addr = 0x1234;
    cpu->PC = instruction_addr;

    // instruction to do 8bit load from memory address and store to register 1
    // 0010000000000001 //
    uint16_t insn = 0x2001;

    // storing the instruction to instruction address
    store2(cpu, insn, instruction_addr);

    // incrementing PC to next byte and setting the memory_address to fetch data
    uint16_t memory_addr = cpu->PC+2;
    uint16_t data_in_memory_address = 0x2000;
    store2(cpu, data_in_memory_address, memory_addr);

    // setting the memory address 0x2000's value to 0xFFFF
    store2(cpu, 0xFFFF, data_in_memory_address);


    // setting PC back to start of instruction address
    cpu->PC = instruction_addr;
    load2(cpu, cpu->PC);
    int val = emulate(cpu);
    assert(val == 0);
    assert(cpu->PC == 0x1238);
    assert(cpu->R[1] == 65535);
}

// tests load from memory instruction by loading 0xFF(255) from memory address 0x2000 to R[1]
void testLoadFromMemory_8bit(struct cpu *cpu)
{
    zerocpu(cpu);
    // pointing PC to desired instruction address
    uint16_t instruction_addr = 0x1234;
    cpu->PC = instruction_addr;

    // instruction to do 8bit load from memory address and store to register 1
    // 0010000000000001 //
    uint16_t insn = 0x2001;

    // storing the instruction to instruction address
    store2(cpu, insn, instruction_addr);

    // incrementing PC to next byte and setting the memory_address to fetch data
    uint16_t memory_addr = cpu->PC+2;
    uint16_t data_in_memory_address = 0x2000;
    store2(cpu, data_in_memory_address, memory_addr);

    // setting the memory address 0x2000's value to 0xFFFF
    store2(cpu, 0xFF, data_in_memory_address);


    // setting PC back to start of instruction address
    cpu->PC = instruction_addr;
    load2(cpu, cpu->PC);
    int val = emulate(cpu);
    assert(val == 0);
    assert(cpu->PC == 0x1238);
    assert(cpu->R[1] == 255);
}


// tests load from memory instruction by loading 0xFFFF(65535) from memory address 0x2000 to R[1]
// memory address 0x2000 was fetched from R[2]
void testLoadFromRegister_16bit(struct cpu *cpu)
{
    zerocpu(cpu);
    // pointing PC to desired instruction address
    uint16_t instruction_addr = 0x1234;
    cpu->PC = instruction_addr;

    // instruction to do 16bit load from address in register 2 and store to register 1
    // 0010100000010001 //
    uint16_t insn = 0x2811;

    // storing the instruction to instruction address
    store2(cpu, insn, instruction_addr);

    // setting the memory address to fetch from in register 2
    uint16_t memory_addr = 0x2000;
    cpu->R[2] = memory_addr;

    // setting the memory address 0x2000's value to 0xFFFF(65535)
    store2(cpu, 0xFFFF, memory_addr);


    // setting PC back to start of instruction address
    cpu->PC = instruction_addr;
    load2(cpu, cpu->PC);
    int val = emulate(cpu);
    assert(val == 0);
    assert(cpu->PC == 0x1236);
    assert(cpu->R[1] == 65535);
}

// tests load from memory instruction by loading 0xFF(255) from memory address 0x2000 to R[1]
// memory address 0x2000 was fetched from R[2]
void testLoadFromRegister_8bit(struct cpu *cpu)
{
    zerocpu(cpu);
    // pointing PC to desired instruction address
    uint16_t instruction_addr = 0x1234;
    cpu->PC = instruction_addr;

    // instruction to do 16bit load from address in register 2 and store to register 1
    // 0010110000010001 //
    uint16_t insn = 0x2C11;

    // storing the instruction to instruction address
    store2(cpu, insn, instruction_addr);

    // setting the memory address to fetch from in register 2
    uint16_t memory_addr = 0x2000;
    cpu->R[2] = memory_addr;

    // setting the memory address 0x2000's value to 0xFFFF(65535)
    store2(cpu, 0xFFFF, memory_addr);


    // setting PC back to start of instruction address
    cpu->PC = instruction_addr;
    load2(cpu, cpu->PC);
    int val = emulate(cpu);
    assert(val == 0);
    assert(cpu->PC == 0x1236);
    assert(cpu->R[1] == 255);
}

// tests move instruction by copying value 0xFFFF(65535) from R[2] to R[1]
void copyDataBetweenRegisters(struct cpu *cpu)
{
    zerocpu(cpu);
    // pointing PC to desired instruction address
    uint16_t instruction_addr = 0x1234;
    cpu->PC = instruction_addr;

    // instruction to do copy contents of register 2 to register 1
    // 0100 0000 0001 0010 //
    uint16_t insn = 0x4012;

    // storing the instruction to instruction address
    store2(cpu, insn, instruction_addr);

    // setting the source data in register 1
    uint16_t memory_addr = 0xFFFF;
    cpu->R[2] = memory_addr;

    // setting PC back to start of instruction address
    cpu->PC = instruction_addr;
    load2(cpu, cpu->PC);
    int val = emulate(cpu);
    assert(val == 0);
    assert(cpu->PC == 0x1236);
    assert(cpu->R[1] == 65535);
}

// tests move instruction by copying SP value(0x2000) to R[1]
void copyDataFromSPAddressToRegister(struct cpu *cpu)
{
    zerocpu(cpu);
    // pointing PC to desired instruction address
    uint16_t instruction_addr = 0x1234;
    cpu->PC = instruction_addr;

    // instruction to do copy contents of register 2 to register 1
    // 0100 0000 0001 1000 //
    uint16_t insn = 0x4018;

    // storing the instruction to instruction address
    store2(cpu, insn, instruction_addr);

    // setting the SP address
    uint16_t stack_pointer_memory_addr = 0x2000;
    cpu->SP = stack_pointer_memory_addr;

    // setting PC back to start of instruction address
    cpu->PC = instruction_addr;
    load2(cpu, cpu->PC);
    int val = emulate(cpu);
    assert(val == 0);
    assert(cpu->PC == 0x1236);
    assert(cpu->R[1] == 0x2000);
}

// tests move instruction by copying value 0x2000 from R[1] to SP
void copyDataFromRegisterToSPAddress(struct cpu *cpu)
{
    zerocpu(cpu);
    // pointing PC to desired instruction address
    uint16_t instruction_addr = 0x1234;
    cpu->PC = instruction_addr;

    // instruction to do copy contents of register 1 to SP
    // 0100 0000 1000 0001 //
    uint16_t insn = 0x4081;

    // storing the instruction to instruction address
    store2(cpu, insn, instruction_addr);

    // setting the source data / destination SP address in Register 1
    cpu->R[1] = 0x2000;


    // setting PC back to start of instruction address
    cpu->PC = instruction_addr;
    load2(cpu, cpu->PC);
    int val = emulate(cpu);
    assert(val == 0);
    assert(cpu->PC == 0x1236);
    assert(cpu->SP == 0x2000);
}

// tests addition of data stored in Register 1 and Register 2, and storing result(4) in Register 3
void testAdd(struct cpu *cpu) {
    // zerocpu(cpu);
    // pointing PC to desired instruction address
    uint16_t instruction_addr = 0x1234;
    cpu->PC = instruction_addr;

    // instruction to do copy contents of register 2 to register 1
    // 0101 0000 1101 0001 //
    uint16_t insn = 0x50D1;

    // storing the instruction to instruction address
    store2(cpu, insn, instruction_addr);

    // setting register a/1
    cpu->R[1] = 4660;

    // setting register b/2
    cpu->R[2] = 8721;

    // setting PC back to start of instruction address
    cpu->PC = instruction_addr;
    load2(cpu, cpu->PC);
    int val = emulate(cpu);
    assert(val == 0);
    assert(cpu->PC == 0x1236);
    assert(cpu->Z == 0);
    assert(cpu->N == 0);
    assert(cpu->R[3] == 13381);
}

// tests addition of data stored in Register 1 and Register 2, and storing result in Register 3
void testSubtract(struct cpu *cpu) {
    // zerocpu(cpu);
    // pointing PC to desired instruction address
    uint16_t instruction_addr = 0x1234;
    cpu->PC = instruction_addr;

    // instruction to do copy contents of register 2 to register 1
    // 0101 0010 1101 0001 //
    uint16_t insn = 0x52D1;

    // storing the instruction to instruction address
    store2(cpu, insn, instruction_addr);

    // setting register a/1
    cpu->R[1] = 0x1234;

    // setting register b/2
    cpu->R[2] = 0x1234;

    // setting PC back to start of instruction address
    cpu->PC = instruction_addr;
    load2(cpu, cpu->PC);
    int val = emulate(cpu);
    assert(val == 0);
    assert(cpu->PC == 0x1236);
    assert(cpu->Z == 1);
    assert(cpu->N == 0);
    assert(cpu->R[3] == 0);
}


// tests AND of data stored in Register 1 and Register 2, and storing result in Register 3
void testAnd(struct cpu *cpu) {
    // zerocpu(cpu);
    // pointing PC to desired instruction address
    uint16_t instruction_addr = 0x1234;
    cpu->PC = instruction_addr;

    // instruction to do copy contents of register 2 to register 1
    // 0101 0100 1101 0001 //
    uint16_t insn = 0x54D1;

    // storing the instruction to instruction address
    store2(cpu, insn, instruction_addr);

    // setting register a/1
    cpu->R[1] = 1;

    // setting register b/2
    cpu->R[2] = 3;

    // setting PC back to start of instruction address
    cpu->PC = instruction_addr;
    load2(cpu, cpu->PC);
    int val = emulate(cpu);
    assert(val == 0);
    assert(cpu->PC == 0x1236);
    assert(cpu->Z == 0);
    assert(cpu->N == 0);
    assert(cpu->R[3] == 0b0000000000000001);
}

// tests OR of data stored in Register 1 and Register 2, and storing result in Register 3
void testOr(struct cpu *cpu) {
    // zerocpu(cpu);
    // pointing PC to desired instruction address
    uint16_t instruction_addr = 0x1234;
    cpu->PC = instruction_addr;

    // instruction to do copy contents of register 2 to register 1
    // 0101 0110 1101 0001 //
    uint16_t insn = 0x56D1;

    // storing the instruction to instruction address
    store2(cpu, insn, instruction_addr);

    // setting register a/1
    cpu->R[1] = 1;

    // setting register b/2
    cpu->R[2] = 3;

    // setting PC back to start of instruction address
    cpu->PC = instruction_addr;
    load2(cpu, cpu->PC);
    int val = emulate(cpu);
    assert(val == 0);
    assert(cpu->PC == 0x1236);
    assert(cpu->Z == 0);
    assert(cpu->N == 0);
    assert(cpu->R[3] == 0b0000000000000011);
}


// tests XOR of data stored in Register 1 and Register 2, and storing result in Register 3
void testXor(struct cpu *cpu) {
    // zerocpu(cpu);
    // pointing PC to desired instruction address
    uint16_t instruction_addr = 0x1234;
    cpu->PC = instruction_addr;

    // instruction to do copy contents of register 2 to register 1
    // 0101 1000 1101 0001 //
    uint16_t insn = 0x58D1;

    // storing the instruction to instruction address
    store2(cpu, insn, instruction_addr);

    // setting register a/1
    cpu->R[1] = 1;

    // setting register b/2
    cpu->R[2] = 3;

    // setting PC back to start of instruction address
    cpu->PC = instruction_addr;
    load2(cpu, cpu->PC);
    int val = emulate(cpu);
    assert(val == 0);
    assert(cpu->PC == 0x1236);
    assert(cpu->Z == 0);
    assert(cpu->N == 0);
    assert(cpu->R[3] == 0b0010);
}

// tests SHIFT_RIGHT(>>) of number stored in R[1] by number stored in R[2], result in R[2]
void testShiftRight(struct cpu *cpu) {
    // zerocpu(cpu);
    // pointing PC to desired instruction address
    uint16_t instruction_addr = 0x1234;
    cpu->PC = instruction_addr;

    // instruction to do copy contents of register 2 to register 1
    // 0101 1010 1101 0001 //
    uint16_t insn = 0x5AD1;

    // storing the instruction to instruction address
    store2(cpu, insn, instruction_addr);

    // setting register a/1
    cpu->R[1] = 0b0000000011000000;

    // setting register b/2
    cpu->R[2] = 6;

    // setting PC back to start of instruction address
    cpu->PC = instruction_addr;
    load2(cpu, cpu->PC);
    int val = emulate(cpu);
    assert(val == 0);
    assert(cpu->PC == 0x1236);
    assert(cpu->Z == 0);
    assert(cpu->N == 0);
    assert(cpu->R[3] == 0b0000000000000011);
}

// tests compare of numbers stored in R[1] and R[2] (R[1] CMP R[2]), updates cpu->N and cpu->Z
void testCompareResultNegative(struct cpu *cpu) {
    // zerocpu(cpu);
    // pointing PC to desired instruction address
    uint16_t instruction_addr = 0x1234;
    cpu->PC = instruction_addr;

    // instruction to do copy contents of register 2 to register 1
    // 0101 1100 1101 0001 //
    uint16_t insn = 0x5CD1;

    // storing the instruction to instruction address
    store2(cpu, insn, instruction_addr);

    // setting register a/1
    cpu->R[1] = 2;

    // setting register b/2
    cpu->R[2] = 6;

    // setting PC back to start of instruction address
    cpu->PC = instruction_addr;
    load2(cpu, cpu->PC);
    int val = emulate(cpu);
    assert(val == 0);
    assert(cpu->PC == 0x1236);
    assert(cpu->Z == 0);
    assert(cpu->N == 1);
}

// tests compare of numbers stored in R[1] and R[2] (R[1] CMP R[2]), updates cpu->N and cpu->Z
void testCompareResultZero(struct cpu *cpu) {
    // zerocpu(cpu);
    // pointing PC to desired instruction address
    uint16_t instruction_addr = 0x1234;
    cpu->PC = instruction_addr;

    // instruction to do copy contents of register 2 to register 1
    // 0101 1100 1101 0001 //
    uint16_t insn = 0x5CD1;

    // storing the instruction to instruction address
    store2(cpu, insn, instruction_addr);

    // setting register a/1
    cpu->R[1] = 2;

    // setting register b/2
    cpu->R[2] = 2;

    // setting PC back to start of instruction address
    cpu->PC = instruction_addr;
    load2(cpu, cpu->PC);
    int val = emulate(cpu);
    assert(val == 0);
    assert(cpu->PC == 0x1236);
    assert(cpu->Z == 1);
    assert(cpu->N == 0);
}

// tests compare of numbers stored in R[1] and R[2] (R[1] CMP R[2]), updates cpu->N and cpu->Z
void testCompareResultPositive(struct cpu *cpu) {
    // TODO
    // WHY IS THIS REQUIRED?
    // zerocpu(cpu);
    // pointing PC to desired instruction address
    uint16_t instruction_addr = 0x1234;
    cpu->PC = instruction_addr;

    // instruction to do copy contents of register 2 to register 1
    // 0101 1100 1101 0001 //
    uint16_t insn = 0x5CD1;

    // storing the instruction to instruction address
    store2(cpu, insn, instruction_addr);

    // setting register a/1
    cpu->R[1] = 6;

    // setting register b/2
    cpu->R[2] = 2;

    // setting PC back to start of instruction address
    cpu->PC = instruction_addr;
    load2(cpu, cpu->PC);
    int val = emulate(cpu);
    assert(val == 0);
    assert(cpu->PC == 0x1236);
    assert(cpu->Z == 0);
    assert(cpu->N == 0);
}

void testJumpUnconditional(struct cpu *cpu) {
    zerocpu(cpu);

    uint16_t instruction_addr = 0x1000;
    cpu->PC = instruction_addr;

    uint16_t insn = 0x6000;

    store2(cpu, insn, instruction_addr);

    uint16_t target_addr = 0x2000; 

    store2(cpu, target_addr, instruction_addr + 2);

    int val = emulate(cpu);

    assert(val == 0);

    assert(cpu->PC == target_addr);
    printf("TestJumpUnconditional passed: PC is 0x%04X\n", cpu-> PC);

}

void testJumpIfZeroExplicit(struct cpu *cpu) {
    zerocpu(cpu);
    uint16_t instruction_addr = 0x1000;
    cpu->PC = instruction_addr; 

    cpu->Z = 1; 

    uint16_t insn = 0x6000 | (1 << 9); 
    store2(cpu, insn, instruction_addr); 

    uint16_t target_addr = 0x2000; 
    store2(cpu, target_addr, instruction_addr + 2);

    int val = emulate(cpu); 

    assert(val == 0);
    assert(cpu-> PC == target_addr);
    printf("TestJumpIfZeroExplicit passed: PC is 0x%04X\n", cpu-> PC);
}

void testJumpIfZeroRegister(struct cpu *cpu) {
    zerocpu(cpu);
    uint16_t instruction_addr = 0x1000;
    cpu->PC = instruction_addr; 

    cpu->Z = 1; 
    int Ra = 3;

    uint16_t target_addr = 0x3000; 
    cpu->R[Ra] = target_addr; 

    uint16_t insn = 0x7000 | (1 << 9) | Ra;
    store2(cpu, insn, instruction_addr);

    int val = emulate(cpu); 

    assert(val == 0);
    assert(cpu-> PC == target_addr);
    printf("TestJumpIfZeroRegister passed: PC is 0x%04X\n", cpu-> PC);
}

void testJumpIfNotZeroExplicit(struct cpu *cpu) {
    zerocpu(cpu);
    uint16_t instruction_addr = 0x1000;
    cpu->PC = instruction_addr; 

    cpu->Z = 0; 

    uint16_t insn = 0x6000 | (2 << 9); 
    store2(cpu, insn, instruction_addr); 

    uint16_t target_addr = 0x2000; 
    store2(cpu, target_addr, instruction_addr + 2);

    int val = emulate(cpu); 

    assert(val == 0);
    assert(cpu-> PC == target_addr);
    printf("TestJumpIfNotZeroExplicit passed: PC is 0x%04X\n", cpu-> PC);
}

void testJumpIfNotZeroRegister(struct cpu *cpu) {
    zerocpu(cpu);
    uint16_t instruction_addr = 0x1000;
    cpu->PC = instruction_addr; 

    cpu->Z = 0; 
    int Ra = 4;

    uint16_t target_addr = 0x4000; 
    cpu->R[Ra] = target_addr; 

    uint16_t insn = 0x7000 | (2 << 9) | Ra;
    store2(cpu, insn, instruction_addr);

    int val = emulate(cpu); 

    assert(val == 0);
    assert(cpu-> PC == target_addr);
    printf("TestJumpIfNotZeroRegister passed: PC is 0x%04X\n", cpu-> PC);
}


void testJumpConditionNotMetExplicit(struct cpu *cpu) {
    zerocpu(cpu);
    uint16_t instruction_addr = 0x1000;
    cpu->PC = instruction_addr; 

    cpu->Z = 0; 

    uint16_t insn = 0x6000 | (1 << 9); 
    store2(cpu, insn, instruction_addr); 

    uint16_t target_addr = 0x2000; 
    store2(cpu, target_addr, instruction_addr + 2);

    int val = emulate(cpu); 

    assert(val == 0);
    assert(cpu-> PC == instruction_addr + 4);
    printf("testJumpConditionNotMetExplicit passed: PC is 0x%04X\n", cpu-> PC);
}


void testJumpConditionNotMetRegister(struct cpu *cpu) {
    zerocpu(cpu);
    uint16_t instruction_addr = 0x1000;
    cpu->PC = instruction_addr; 

    cpu->Z = 1; 

    uint16_t insn = 0x7000 | (2 << 9) | 1;
    store2(cpu, insn, instruction_addr);

    cpu->R[1] = 0x3000;

    int val = emulate(cpu); 

    assert(val == 0);
    assert(cpu-> PC == instruction_addr + 2);
    printf("testJumpConditionNotMetRegister passed: PC is 0x%04X\n", cpu-> PC);
}

void testPush(struct cpu *cpu) {
    zerocpu(cpu);

    cpu->R[3] = 0x1234;
    cpu->SP = 0xBFFE;

    uint16_t instruction_addr = 0x0050;
    cpu->PC = instruction_addr; 


    uint16_t insn = 0xB000 | (3 & 0x7);
    store2(cpu, insn, instruction_addr);

    int val = emulate(cpu); 
    printf("test push values: R3 = 0x%04X, SP = 0x%04X, PC is 0x%04X\n",cpu->R[3], cpu->SP, cpu-> PC);
    assert(val == 0);

    assert(cpu->SP == 0xBFFC);
    assert(cpu->PC == instruction_addr + 2);

    uint16_t value_at_SP = load2(cpu, cpu->SP);
    assert(value_at_SP == 0x1234);

    printf("testPush passed: R3 = 0x%04X, SP = 0x%04X, PC is 0x%04X\n",cpu->R[3], cpu->SP, cpu-> PC);
}


void testPop(struct cpu *cpu) {
    zerocpu(cpu);

    cpu->SP = 0xBFFC;
    store2(cpu, 0x1234, cpu->SP);

    uint16_t instruction_addr = 0x0050;
    cpu->PC = instruction_addr; 


    uint16_t insn = 0xC000 | (3 & 0x7);
    store2(cpu, insn, instruction_addr);


    int val = emulate(cpu); 

    assert(val == 0);
    assert(cpu->SP == 0xBFFE);
    assert(cpu->PC == instruction_addr + 2);
    assert(cpu-> R[3] == 0x1234);

    printf("testPop passed: R3 = 0x%04X, SP = 0x%04X, PC is 0x%04X\n",cpu->R[3], cpu->SP, cpu-> PC);
}


void testInInstruction(struct cpu *cpu) 
{

    zerocpu(cpu); 

    uint16_t instruction_addr = 0x1000;
    cpu-> PC = instruction_addr;

    uint16_t insn = 0xD000 | 2; 

    store2(cpu, insn, instruction_addr); 

    int input_char = 'A';
    ungetc(input_char, stdin); 

    int val = emulate(cpu);

    assert(val == 0);
    assert(cpu->R[2] == input_char);
    assert(cpu->PC == instruction_addr + 2);

    printf("TestIn passes");

}







char memory[64*1024];
struct cpu cpu;

int main(int argc, char **argv)
{
    cpu.memory = memory;
    printf("Starting tests");
    test1(&cpu);
    testSet(&cpu);
    testLoadFromMemory_16bit(&cpu);
    testLoadFromMemory_8bit(&cpu);
    testLoadFromRegister_16bit(&cpu);
    testLoadFromRegister_8bit(&cpu);
    copyDataBetweenRegisters(&cpu);
    copyDataFromSPAddressToRegister(&cpu);
    copyDataFromRegisterToSPAddress(&cpu);
    testAdd(&cpu);
    testSubtract(&cpu);
    testAnd(&cpu);
    testOr(&cpu);
    testXor(&cpu);
    testShiftRight(&cpu);
    testCompareResultNegative(&cpu);
    testCompareResultZero(&cpu);
    testCompareResultPositive(&cpu);
    testJumpUnconditional(&cpu);
    testJumpIfZeroExplicit(&cpu);
    testJumpIfNotZeroExplicit(&cpu);
    testJumpIfNotZeroRegister(&cpu);
    testJumpIfZeroRegister(&cpu);
    testJumpConditionNotMetExplicit(&cpu);
    testJumpConditionNotMetRegister(&cpu);
    testPush(&cpu);
    testPop(&cpu);
    testInInstruction(&cpu);
    printf("all tests PASS\n");
}


