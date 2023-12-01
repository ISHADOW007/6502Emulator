//
// Created by shobhit on 26/10/23.
//

#ifndef INC_6502EMULATOR_PROCESSOR_HPP
#define INC_6502EMULATOR_PROCESSOR_HPP


#include <cstdint>

#include "memory.hpp"

class Processor {
    // table to store how many cpu clock cycle is taken by a particular instruction
    // first index is the high nibble value
    // second index is the low nibble value
    int InstrTicksTable[16][16] = {
            {7,6,0,0,0,3,2,0,3,2,1,0,0,3,3,0},
            {2,5,0,0,0,4,6,0,2,4,0,0,0,4,7,0},
            {6,6,0,0,3,3,5,0,4,2,2,0,4,4,6,0},
            {2,5,0,0,0,4,6,0,2,4,0,0,0,4,7,0},
            {6,6,0,0,0,3,5,0,3,2,2,0,3,4,3,0},
            {2,6,0,0,0,4,6,0,2,4,0,0,0,4,7,0},
            {6,6,0,0,0,3,5,0,4,2,2,0,5,4,6,0},
            {2,5,0,0,0,4,6,0,2,4,0,0,0,4,7,0},
            {0,6,0,0,3,3,3,0,2,0,2,0,4,4,4,0},
            {2,6,0,0,4,4,4,0,2,5,2,0,0,5,0,0},
            {2,6,2,0,3,3,3,0,2,2,2,0,4,4,4,0},
            {2,5,0,0,4,4,4,0,2,4,2,0,4,4,4,0},
            {2,6,0,0,3,3,5,0,2,2,2,0,4,4,6,0},
            {2,5,0,0,0,4,6,0,2,4,0,0,0,4,7,0},
            {2,6,0,0,3,3,5,0,2,2,2,0,4,4,6,0},
            {2,5,0,0,0,4,6,0,2,4,0,0,0,4,6,0}
    };

    // table to store how many subsequent words(or bytes) is used by a particular instruction
    // first index is the high nibble value
    // second index is the low nibble value
    int InstrByteTable[16][16] = {
            {1,2,0,0,0,2,2,0,1,2,1,0,0,3,3,0},
            {2,2,0,0,0,2,2,0,1,3,0,0,0,3,3,0},
            {3,2,0,0,2,2,2,0,1,2,1,0,3,3,3,0},
            {2,2,0,0,0,2,2,0,1,3,0,0,0,3,3,0},
            {1,2,0,0,0,2,2,0,1,2,1,0,3,3,3,0},
            {2,2,0,0,0,2,2,0,1,3,0,0,0,3,3,0},
            {1,2,0,0,0,2,2,0,1,2,1,0,3,3,3,0},
            {2,2,0,0,0,2,2,0,1,3,0,0,0,3,3,0},
            {0,2,0,0,2,2,2,0,1,0,1,0,3,3,3,0},
            {2,2,0,0,2,2,2,0,1,3,1,0,0,3,0,0},
            {2,2,2,0,2,2,2,0,1,2,1,0,3,3,3,0},
            {2,2,0,0,2,2,2,0,1,3,1,0,3,3,3,0},
            {2,2,0,0,2,2,2,0,1,2,1,0,3,3,3,0},
            {2,2,0,0,0,2,2,0,1,3,0,0,0,3,3,0},
            {2,2,0,0,2,2,2,0,1,2,1,0,3,3,3,0},
            {2,2,0,0,0,2,2,0,1,3,0,0,0,3,3,0}
    };

    //Memory mem;
public:
    uint16_t PC; // 16 bit Program Counter register
    uint8_t A;   // 8 bit Accumulator register
    uint8_t X;   // 8 bit Index register X specific to 6502
    uint8_t Y;   // 8 bit Index register Y specific to 6502
    uint8_t SP;  // 8 bit Stack Pointer register

    // 8 bit Flag Status register
    // from 7th to 0th bit
    // negative(N), overflow(O), reserved, break(B), decimal(D), interrupt disable(I), zero(Z), and carry(C) bit
    uint8_t FR;

    // cpu clock
    long cpuClock = 0;

    // Initialises the values of relevant registers before instruction fetch
    void VMInit(Memory *mem);

    // increments the clock tick depending on opcode
    void updateClock(uint8_t opcode);

    // increments PC depending on opcode
    void UpdatePC(uint8_t opcode);

    // gives the location of top of the stack
    int retStackTopLoc();

    uint8_t lowNibble(uint8_t opcode);
    uint8_t highNibble(uint8_t opcode);

    void setNegativeBit();
    void setOverflowBit();
    void setBreakBit();
    void setDecimalBit();
    void setInterruptDisableBit();
    void setZeroBit();
    void setCarryBit();

    /*** LDA Instructions ***/
    // Processor Instructions
    void LDA_imdt(Memory *mem);  // LDA #oper
    void LDA_zpg(Memory *mem);   // LDA oper
    void LDA_zpgx(Memory *mem);  // LDA oper, X
    void LDA_abs(Memory *mem);   // LDA oper
    void LDA_absx(Memory *mem);  // LDA oper, X
    void LDA_absy(Memory *mem);  // LDA oper, Y
    void LDA_ind_x(Memory *mem); // LDA (oper ,X)
    void LDA_ind_y(Memory *mem); // LDA (oper), Y .... verify addressing mode

    /*** STA Instructions ***/
    void STA_zpg(Memory *mem);
    void STA_zpgx(Memory *mem);
    void STA_abs(Memory *mem);
    void STA_absx(Memory *mem);
    void STA_absy(Memory *mem);
    void STA_ind_x(Memory *mem);
    void STA_ind_y(Memory *mem);

    /*** CMP Instructions ***/
    void CMP_imdt(Memory *mem);
    //testing of following functions not done
    void CMP_zpg(Memory *mem);
    void CMP_zpgx(Memory *mem);
    void CMP_abs(Memory *mem);

    // testing of all the following instructions not done

    /*** TYA Instructions ***/
    void TYA_impl(Memory *mem);

    /*** TXS Instructions ***/
    void TXS_impl(Memory *mem);

    /*** TXA Instructions ***/
    void TXA_impl(Memory *mem);

    /*** JMP Instructions ***/
    void JMP_abs(Memory *mem);
    void JMP_idr(Memory *mem);

    /*** TAX Instructions ***/
    void TAX_impl(Memory *mem);

    /*** TAY Instructions ***/
    void TAY_impl(Memory *mem);

     /*** TSX Instructions ***/
    void TSX_impl(Memory *mem);

      /*** INX Instructions ***/
    void INX_impl(Memory *mem);

     /*** INY Instructions ***/
    void INY_impl(Memory *mem);

    /***DEX Instructions ***/
    void DEX_impl(Memory *mem);

    /***DEY Instructions ***/
    void DEY_impl(Memory *mem);

    /***CLC Instructions ***/
    void CLC_impl(Memory *mem);

    /***CLD Instructions ***/
    void CLD_impl(Memory *mem);

    /***CLI Instructions ***/
    void CLI_impl(Memory *mem);

    /***CLV Instructions ***/
    void CLV_impl(Memory *mem);

    /***TYA Instructions ***/
    void TYA_impl(Memory *mem);
};


#endif //INC_6502EMULATOR_PROCESSOR_HPP
