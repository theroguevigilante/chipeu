#include <fstream>
#include <iostream>
#include "chipeu.h"

// Basic chip-8 system structure
chipeu::chipeu() : pc (0x200){
    // Loading the fonstet in to the memory
    for (int i = 0; i < 80; ++i){
        memory[i] = chip8_fontset[i];
    }
}

void chipeu::emulateCycle(){
    opcode = memory[pc] << 8 | memory[pc+1];
    switch(opcode & 0xF000){
        {
            case 0x0000:
                switch(opcode & 0x000F){
                    case 0x0000:
                        for(int i = 0; i < 2048; ++i){
                            gfx[i] = 0;
                        }
                        break;
                    case 0x000E:
                        --sp;
                        break;
                    default:
                        std::cout << "Unknown opcode [0x" << std::hex << opcode << "]" << std::endl;
                }
                break;
        }
    }
};

void chipeu::loadRom(const char* filename){
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (file.is_open()){
        std::streampos size = file.tellg();
        char* buffer = new char[size];

        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();
        if (size<=0xE00){
            for (long i = 0; i < size; ++i){
                memory[0x200 + i ] = buffer[i];
            } 
        }
        delete[] buffer;
    }
}
