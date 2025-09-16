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
    unsigned char x{}, y{}, kk{};
    x = (opcode & 0x0F00) >> 8;
    y = (opcode & 0x00F0) >> 4;
    kk = opcode & 0x00FF;
    switch(opcode & 0xF000){
        {   // 0x0 family
            case 0x0000:
                switch(opcode & 0x00FF){
                    case 0x00E0:
                        for(int i = 0; i < 2048; ++i){
                            gfx[i] = 0;
                        }
                        drawflag = true;
                        pc+=2;
                        break;
                    case 0x00EE:
                        --sp;
                        pc = stack[sp];
                        pc+=2;
                        break;
                    default:
                        std::cout << "Unknown opcode [0x" << std::hex << opcode << "]" << std::endl;
                        pc+=2;
                }
                break;
            case 0x1000:
                pc = opcode & 0x0FFF;
                break;
            case 0x2000:
                stack[sp] = pc;
                ++sp;
                pc = opcode & 0x0FFF;
                break;
            //conditional skips
            case 0x3000:
                if(V[x]==kk){
                    pc+=4;
                }
                else pc+=2;
                break;
            case 0x4000:
                if(V[x]!=kk){
                    pc+=4;
                }
                else pc+=2;
                break;
            case 0x5000:
                if(V[x]==V[y]){
                    pc+=4;
                }
                else pc+=2;
                break;
            case 0x9000:
                if(V[x]!=V[y]){
                    pc+=4;
                }
                else pc+=2;
            //arithmetic opcodes
            case 0x8000:
                switch(opcode & 0x000F){
                    case 0x0000: V[x] = V[y]; break;
                    case 0x0001: V[x] |= V[y]; break;
                    case 0x0002: V[x] &= V[y]; break;
                    case 0x0003: V[x] ^= V[y]; break;
                    case 0x0004:
                                 if(V[y] > (0xFF - V[x])) V[0xF] = 1;
                                 else V[0xF] = 0;
                                 V[x] += V[y];
                                 break;
                    case 0x0005:
                                 if(V[x] >= V[y]) V[0xF] = 1;
                                 else V[0xF] = 0;
                                 V[x] -= V[y];
                                 break;
                    case 0x0006:
                                 V[0xF] = V[x] & 0x1;
                                 V[x] >>= 1;
                                 break;
                    case 0x0007:
                                 if(V[y] >= V[x]) V[0xF]=1;
                                 else V[0xF] = 0;
                                 V[x] = V[y] - V[x];
                                 break;
                    case 0x000E:
                                 V[0xF] = V[x] >> 7;
                                 V[x] <<= 1;
                                 break;
                    default:
                        std::cout << "Unknown opcode [0x" << std::hex << opcode << "]" << std::endl;

                }
                pc+=2;
                break;
            case 0xA000:
                I = (opcode & 0x0FFF);
                pc+=2;
                break;
            case 0xB000:
                pc = (opcode & 0x0FFF) + V[0];
                break;
            case 0xC000:
                V[x] = (rand() % 256) & kk;
                pc+=2;
                break;
            case 0xD000:
                // TODO: Implement drawing
                pc+=2;
                break;
            case 0xE000:
                switch(opcode & 0x00FF){
                    case 0x009E:
                        if(key[V[x]]) pc+=4;
                        else pc+=2;
                        break;
                    case 0x00A1:
                        if(!key[V[x]]) pc+=4;
                        else pc+=2;
                        break;
                    default :
                        std::cout << "Unknown opcode [0x" << std::hex << opcode << "]" << std::endl;
                }
                break;
            case 0xF000:
                switch(opcode & 0x00FF){
                    case 0x0007:
                        V[x] = delay_timer;
                        pc+=2;
                        break;
                    case 0x000A:
                        // TODO: input setup
                        pc+=2;
                        break;
                    case 0x0015:
                        delay_timer = V[x];
                        pc+=2;
                        break;
                    case 0x0018:
                        sound_timer = V[x];
                        pc+=2;
                        break;
                    case 0x001E:
                        I += V[x];
                        pc+=2;
                        break;
                    case 0x0029:
                        // TODO: implement drawing
                        pc+=2;
                        break;
                    case 0x0033:
                        memory[I] = V[x] / 100;
                        memory[I+1] = (V[x] / 10) % 10;
                        memory[I+2] = (V[x] & 100) % 10;
                        pc+=2;
                        break;
                    case 0x0055:
                        for(int i = 0; i < x; ++i){
                            memory[I+i] = V[i];
                       }
                        pc+=2;
                        break;
 
                    case 0x0065:
                        for(int i = 0; i < x; ++i){
                            V[i] = memory[I+i];
                        }
                        pc+=2;
                        break;
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
