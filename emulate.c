//
// Created by aruni on 19-09-2024.
//
/*
 * file:        emulate-soln.c
 * description: solution to Lab 1
 */

#include <stdio.h>
#include <stdlib.h>

#include "lab1.h"


void store2(struct cpu *cpu, uint16_t data, uint16_t addr) {
    cpu->memory[addr] = data & 0xFF;
    cpu->memory[addr+1] = (data >> 8) & 0xFF;
}

// TODO: CONFIRM THAT THE CORRECT BYTE IS BEEN STORED AND LOADED IN 8_BIT FUNCTIONS
void store2_8bit(struct cpu *cpu, uint16_t data, uint16_t addr) {
    cpu->memory[addr] = data & 0xFF;
}


//  takes CPU and address as argument, and returns 16bit data stored at PC pointed address
uint16_t load2(struct cpu *cpu, uint16_t addr) {
    return (cpu->memory[addr] | (cpu->memory[addr+1] << 8));
}

//  take CPU and address as argument, and returns 8bit data stored at PC pointed address
uint8_t load2_8bit(struct cpu *cpu, uint16_t addr) {
    return (cpu->memory[addr]);
}

/* set instruction, (load constant value into register)
 */
int set(struct cpu *cpu, uint16_t insn)
{
    // fetch 16bit instruction from memory form
//    uint16_t insn = load2(cpu, cpu->PC);
    // pc is pointing to 16 bit address memory, but the program counter can only get a byte at a time
    // thus, 16 bits -> 2 bytes, so to access 16bit data, we have to fetch data from PC and PC+1
    // the load2 function(already implemented) fetches the 16bit data(2 bytes).
    // for 4 byte instructions, we have to fetch everything stored from pc and pc+2

    // fetching data from 2nd address in 4byte instruction
    uint16_t data = load2(cpu, cpu->PC+2);

    // get the register to put data to
    int registerNumberPut = insn & 7;

    // assign value to register
    cpu->R[registerNumberPut] = data;

    // increment the PC by 4, to point to next instruction
    cpu->PC += 4;

    // succesful execution, non-halt instruction
    return 0;
}

/* load from constant address, (read from memory address into register)
 */
int loadFromConstantAddress(struct cpu *cpu, uint16_t insn)
{   
    printf("loadfromConstantAddress is working fine\n");
    // fetching address to data in 4byte instruction
    uint16_t memoryAddressFetch = load2(cpu, cpu->PC+2);

    // get the register to put data to
    int registerNumberPut = insn & 7;

    // fetch the 6th bit from left to determine 16bit load or 8 bit load
    int B = insn & 0x0400;

    // 8 bit load
    if (B) {
        // get 8 bit data from required address specified in Rb
        uint8_t data = load2_8bit(cpu, memoryAddressFetch);

        // assign 8bit value to register
        cpu->R[registerNumberPut] = data;
    }
    // 16 bit load
    else {
        // fetch 16 bit data from the required address
        uint16_t data = load2(cpu, memoryAddressFetch);

        // assign 16bit value to register
        cpu->R[registerNumberPut] = data;
    }

    // increment the PC by 4, to point to next instruction
    cpu->PC += 4;

    // succesful execution, non-halt instruction
    return 0;
}

/* load from register instruction, (read from memory address into register)
 */
int loadFromRegisterAddress(struct cpu *cpu, uint16_t insn)
{
    // get the register to put data to
    int registerNumberPut = insn & 7;

    // get the register number(Rb) to fetch data address from
    int registerNumberAddressFetch = (insn >> 3) & 7;

    // fetch the 6th bit from left to determine 16bit load or 8 bit load
    int B = insn & 0x0400;

    // 8 bit load
    if (B) {
        // get 8 bit data from required address specified in Rb
        uint8_t data = load2_8bit(cpu, cpu->R[registerNumberAddressFetch]);

        // assign 8bit value to register
        cpu->R[registerNumberPut] = data;
    }
    // 16 bit load
    else {
        // fetch 16 bit data from the required address
        uint16_t data = load2(cpu, cpu->R[registerNumberAddressFetch]);

        // assign 16bit value to register
        cpu->R[registerNumberPut] = data;
    }

    // increment the PC by 2, to point to next instruction
    cpu->PC += 2;

    // succesful execution, non-halt instruction
    return 0;
}

/* store from register to constant address , (read from register into constant memory address )
 */
int storeToConstantAddress(struct cpu *cpu, uint16_t insn)
{
    // fetching address to put to in 4byte instruction
    uint16_t memoryAddressPut = load2(cpu, cpu->PC+2);

    // get the register to fetch data from
    int registerNumberFetch = insn & 7;



    // fetch the 6th bit from left to determine 16bit load or 8 bit load
    int B = insn & 0x0400;

    // 8 bit load
    if (B) {
        // get the data to put to memory address
        uint8_t data = cpu->R[registerNumberFetch];

        // assign 8 bit data from register to destination address
        store2_8bit(cpu, data, memoryAddressPut);
    }
    // 16 bit load
    else {
        // get the data to put to memory address
        uint16_t data = cpu->R[registerNumberFetch];

        // assign 16 bit data from register to destination address
        store2(cpu,data, memoryAddressPut);
    }

    // increment the PC by 4, to point to next instruction
    cpu->PC += 4;

    // succesful execution, non-halt instruction
    return 0;
}


/* store from register to register specified address, (read from register into register specified address )
 */
int storeToRegisterAddress(struct cpu *cpu, uint16_t insn)
{

    // get the register to fetch data from
    int registerNumberDataFetch = insn & 7;

    // get the register to fetch address from
    int registerNumberAddressFetch = (insn >> 3) & 7;

    int memoryAddressPut = cpu->R[registerNumberAddressFetch];

    // fetch the 6th bit from left to determine 16bit load or 8 bit load
    int B = insn & 0x0400;

    // 8 bit load
    if (B) {
        // get the data to put to memory address
        uint8_t data = cpu->R[registerNumberDataFetch];

        // assign 8 bit data from register to destination address
        store2_8bit(cpu, data, memoryAddressPut);
    }
    // 16 bit load
    else {
        // get the data to put to memory address
        uint16_t data = cpu->R[registerNumberDataFetch];

        // assign 16 bit data from register to destination address
        store2(cpu,data, memoryAddressPut);
    }

    // increment the PC by 2, to point to next instruction
    cpu->PC += 2;

    // succesful execution, non-halt instruction
    return 0;
}


// move between registers instruction, copy one register to another
int move(struct cpu *cpu, uint16_t insn){

    // get source register number
    // int registerSourceNumber = insn & 0b1111;

    // get destination register number
    // int registerDestinationNumber = (insn >> 4) & 0b1111;

    int registerSourceNumber = insn & 0xF;
    int registerDestinationNumber = (insn & 0xF0) >> 4;

    // if source register number is 8, source is SP
    if(registerSourceNumber == 8 && registerDestinationNumber == 8){
        // copy value in stack pointer address into stack pointer address
        // do nothing, copying value of stack pointer address back into same address
    }
    //
    else if(registerSourceNumber == 8 && registerDestinationNumber != 8){
        // copy stack pointer address into destination register
        cpu->R[registerDestinationNumber] = cpu->SP;
    }
    else if(registerSourceNumber != 8 && registerDestinationNumber == 8){
        // copy the value in source register into stack pointer variable
        cpu->SP = cpu->R[registerSourceNumber];
    }
    else {
        cpu->R[registerDestinationNumber] = cpu->R[registerSourceNumber];
    }

    // increment the PC by 2, to point to next instruction
    cpu->PC += 2;

    // succesful execution, non-halt instruction
    return 0;
}

int arithmeticAndLogicalOperations(struct cpu *cpu, uint16_t insn)
{

    // before every arithmetic instruction, update the cpu->Z and cpu->N flags to zero?
    // so that the previous instructions flag states does not affect current instructions flag state?

    cpu->Z = 0;
    cpu->N = 0;

    uint16_t b = (insn >> 3) & 7;
    uint16_t a = insn & 7;
    uint16_t c = (insn >> 6) & 7;
    uint16_t val;
    if ((insn & 0x0E00) == 0x0000) /* ADD */ {
        val = cpu->R[a] + cpu->R[b];
        cpu->R[c] = val;
    }
    else if ((insn & 0x0E00) == 0x0200) /* SUB */ {
        val = cpu->R[a] - cpu->R[b];
        cpu->R[c] = val;
    }
    else if ((insn & 0x0E00) == 0x0400) /* AND */ {
        val = (cpu->R[a] & cpu->R[b]);
        cpu->R[c] = val;
    }
    else if ((insn & 0x0E00) == 0x0600) /* OR */ {
        val = (cpu->R[a] | cpu->R[b]);
        cpu->R[c] = val;
    }
    else if ((insn & 0x0E00) == 0x0800) /* XOR */ {
        val = cpu->R[a] ^ cpu->R[b];
        cpu->R[c] = val;
    }
    else if ((insn & 0x0E00) == 0x0A00) /* SHIFT right */ {
        val = cpu->R[a] >> cpu->R[b];
        cpu->R[c] = val;
    }
    else if ((insn & 0x0E00) == 0x0C00) /* CMP */ {
        val = cpu->R[a] - cpu->R[b];
    }
    else if ((insn & 0x0E00) == 0x0E00) /* TEST */ {
        val = cpu->R[a];
    }
    // setting Z and N flags is common for all arithmetic instructions
    int is_negative = (val & 0x8000) != 0;
    if (is_negative) {
        // set the negative flag
        cpu->N = 1;
    }
    int isZero = (val == 0x0000);
    if(isZero) {
        // set the zero flag
        cpu->Z = 1;
    }

    // increment the PC by 2, to point to next instruction
    cpu->PC += 2;
    // succesful execution, non-halt instruction
    return 0;
}

// TODO
// HOW TO DETERMINE BETWEEN JUMP WITH REGISTER ADDRESS AND JUMP WITH EXPLICIT ADDRESS

// jump function, jump to address fetched from register
int jumpToRegisterAddress(struct cpu *cpu, uint16_t insn)
{
    // get the jump type
    int op = (insn >> 9) & 7;

    // get the register number storing jump to address
    int registerToFetchJumpAddress = insn & 7;

    // fetch flag values
    int isZero = cpu->Z;
    int isNegative = cpu->N;

    if (op == 0b000) /* UNCONDITIONAL JUMP */ {
        cpu->PC = cpu->R[registerToFetchJumpAddress];
        return 0;
    }
    if (op == 0b001) /* JUMP IF ZERO */ {
        if(isZero) {
            cpu->PC = cpu->R[registerToFetchJumpAddress];
            return 0;
        }
    }
    if (op == 0b010) /* JUMP IF NON-ZERO */ {
        if(!isZero) {
            cpu->PC = cpu->R[registerToFetchJumpAddress];
            return 0;
        }
    }
    if (op == 0b011) /* JUMP IF LESS THAN / NEGATIVE */ {
        if(isNegative) {
            cpu->PC = cpu->R[registerToFetchJumpAddress];
            return 0;
        }
    }
    if (op == 0b100) /* JUMP IF GREATER THAN / POSITIVE */ {
        if(!isNegative && !isZero) {
            cpu->PC = cpu->R[registerToFetchJumpAddress];
            return 0;
        }
    }
    if (op == 0b101) /* JUMP IF LESS OR EQUAL */ {
        if(isNegative || isZero) {
            cpu->PC = cpu->R[registerToFetchJumpAddress];
            return 0;
        }
    }
    if (op == 0b110) /* JUMP IF GREATER OR EQUAL */ {
        if(!isNegative) {
            cpu->PC = cpu->R[registerToFetchJumpAddress];
            return 0;
        }
    }
    cpu->PC = cpu->PC+2;
    return 0;
}

// jump function, jump to address fetched from 2nd byte of instruction
int jumpToExplicitAddress(struct cpu *cpu, uint16_t insn)
{
    // get the jump type
    int op = (insn >> 9) & 7;

    // get the jump to address from 2nd byte of instruction
    uint16_t jumpToAddress = load2(cpu,cpu->PC+2);

    // fetch flag values
    int isZero = cpu->Z;
    int isNegative = cpu->N;

    if (op == 0b000) /* UNCONDITIONAL JUMP */ {
        cpu->PC = jumpToAddress;
        return 0;
    }
    if (op == 0b001) /* JUMP IF ZERO */ {
        if(isZero) {
            cpu->PC = jumpToAddress;
            return 0;
        }
    }
    if (op == 0b010) /* JUMP IF NON-ZERO */ {
        if(!isZero) {
            cpu->PC = jumpToAddress;
            return 0;
        }
    }
    if (op == 0b011) /* JUMP IF LESS THAN / NEGATIVE */ {
        if(isNegative) {
            cpu->PC = jumpToAddress;
            return 0;
        }
    }
    if (op == 0b100) /* JUMP IF GREATER THAN / POSITIVE */ {
        if(!isNegative && !isZero) {
            cpu->PC = jumpToAddress;
            return 0;
        }
    }
    if (op == 0b101) /* JUMP IF LESS OR EQUAL */ {
        if(isNegative || isZero) {
            cpu->PC = jumpToAddress;
            return 0;
        }
    }
    if (op == 0b110) /* JUMP IF GREATER OR EQUAL */ {
        if(!isNegative) {
            cpu->PC = jumpToAddress;
            return 0;
        }
    }
    cpu->PC = cpu->PC+4;
    return 0;
}

// 4660 + 8721
// 28740 + 24867


// call absolute
// function to implement jump and push return address with specified address
int jumpAndPushReturnAddressExplicit(struct cpu *cpu, uint16_t insn) {

    // decrement the SP register by 2
    cpu->SP = cpu->SP-2;

    // GET ADDRESS OF PC+4, AND STORE THAT ADDRESS TO WHERE SP POINTS TO
    store2(cpu,cpu->PC+4,cpu->SP);

    // read a 16-bit value from bytes 3 and 4 of the instruction and set the PC register to that value.
    uint16_t second_word_of_instruction = load2(cpu,cpu->PC+2);
    cpu->PC = second_word_of_instruction;

    return 0;
}

// call register indirect
// function to implement jump and push return address with address in register
int jumpAndPushReturnAddressRegister(struct cpu *cpu, uint16_t insn) {

    // decrement the SP register by 2
    cpu->SP = cpu->SP-2;

    // getting register number to fetch the address from
    int a = insn & 7;

    // store the 16-bit value (PC+2) to the address in SP
    store2(cpu,cpu->PC+2,cpu->SP);

    // set the PC register to the 16-bit value  in register Ra.
    cpu->PC = cpu->R[a];

    return 0;
}

// RET – pop return address and jump to it
int popReturnAddressAndJumpToIt(struct cpu *cpu, uint16_t insn)
{
    // load a 16-bit value from the address in register SP and set PC to that value.
    uint16_t word_at_SP = load2(cpu,cpu->SP);
    cpu->PC = word_at_SP;

    // increment SP by 2.
    cpu->SP = cpu->SP+2;

    return 0;
}


//PUSH Ra:
int pushRegisterToStack(struct cpu *cpu, uint16_t insn)
{
    // decrement SP by 2
    cpu->SP = cpu->SP-2;

    // getting register number to fetch the address from
    int a = insn & 7;

    // store 16-bit value in register Ra to address in SP.
    uint16_t value_in_register = cpu->R[a];
    store2(cpu,value_in_register,cpu->SP);
    cpu->PC += 2;

    return 0;
}

//POP Ra:
int popRegisterToStack(struct cpu *cpu, uint16_t insn)
{   
    // getting register number to store value to
    int a = insn & 7;

    // Read 16-bit value from address in SP, store in register Ra
    uint16_t value_in_SP = load2(cpu,cpu->SP);
    cpu->R[a] = value_in_SP;

    // increment SP by 2
    cpu->SP = cpu->SP+2;
    cpu->PC += 2;

    return 0;
}

int inRa(struct cpu *cpu, uint16_t insn) {
    int a = insn & 7;
    cpu->R[a] = fgetc(stdin);
    cpu->PC = cpu->PC+2;
    return 0;
}

int outRa(struct cpu *cpu, uint16_t insn) {
    int a = insn & 7;

    // GET THE RIGHT MOST 8 BITS BEFORE FPUTC
    uint16_t lower_8_bits = cpu->R[a] & 0x00FF;
    fputc(lower_8_bits, stdout);

    cpu->PC = cpu->PC+2;

    return 0;
}

int halt(struct cpu *cpu, uint16_t insn) {
    return 1;
}


/* emulate(struct cpu*) - emulate a single instruction
 *     - returns 1 for halt, 0 for no halt
 */
int emulate(struct cpu *cpu)
{
    uint16_t insn = load2(cpu, cpu->PC);
    printf("Instruction: 0x%X\n",  insn);

    // getting the first 4 bits of the instruction to identify instruction type
    switch (insn & 0xF000) {
        case 0x1000:            /* SET */
            //        sprintf(buf, "SET R%d = 0x%04x", ra, next);
            return set(cpu,insn);
        case 0x2000:            /* LOAD */
            int is_load_from_register_address = (0x800 & insn);
            if (is_load_from_register_address) {
                return loadFromRegisterAddress(cpu,insn);
            }
            else {
                return loadFromConstantAddress(cpu,insn);
            }
        case 0x3000:            /* STORE */
            int is_store_to_register_address = (0x800 & insn);
            if (is_store_to_register_address) {
                return storeToRegisterAddress(cpu,insn);
            }
            else {
                return storeToConstantAddress(cpu,insn);
            }
        case 0x4000:            /* MOVE */
            return move(cpu,insn);
        case 0x5000:            /* ARITHMETIC OPERATIONS */
            return arithmeticAndLogicalOperations(cpu,insn);

        case 0x6000:            /* JMP absolute */
            return jumpToExplicitAddress(cpu,insn);
        case 0x7000:            /* JMP register indirect */
            printf("Comming Here");
            return jumpToRegisterAddress(cpu,insn);
        case 0x8000:            /* CALL absolute */
            return jumpAndPushReturnAddressExplicit(cpu,insn);
        case 0x9000:            /* CALL register indirect */
            return jumpAndPushReturnAddressRegister(cpu,insn);
        case 0xA000:            /* RET */
            return popReturnAddressAndJumpToIt(cpu,insn);
        case 0xB000:            /* PUSH */
            return pushRegisterToStack(cpu,insn);
        case 0xC000:            /* POP */
            return popRegisterToStack(cpu,insn);
        case 0xD000:            /* IN */
            return inRa(cpu,insn);
        case 0xE000:            /* OUT */
            return outRa(cpu,insn);
        case 0xF000:            /* HALT */
            return halt(cpu,insn);
    }
}