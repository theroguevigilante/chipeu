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
    std::cout << "Opcode:0x" << std::hex << opcode << " PC:0x" << pc << std::endl;
    x = (opcode & 0x0F00) >> 8;
    y = (opcode & 0x00F0) >> 4;
    kk = opcode & 0x00FF;
    switch(opcode & 0xF000){
        // 0x0 family
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
        case 0x6000:
            V[x] = kk;
            pc += 2;
            break;
        case 0x7000:
            V[x] += kk;
            pc+= 2;
            break;
        case 0x9000:
            if(V[x]!=V[y]){
                pc+=4;
            }
            else pc+=2;
            break;
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
            {
                unsigned char height = (opcode & 0x000F);
                unsigned char pixel{};
                unsigned char X = V[x];
                unsigned char Y = V[y];
                V[0xF] = 0;
                for (int row = 0; row < height; ++row){
                    pixel = memory[I+row];
                    for(int column = 0;column < 8; ++column){
                        if ((pixel & (0x80 >> column)) != 0){
                            unsigned int screenX = (column + X) % 64;
                            unsigned int screenY = (row + Y) % 32;
                            unsigned int index = screenX + (screenY * 64);
                            if (gfx[index] == 1) V[0xF] = 1;
                            gfx[index] ^= 1;
                        }
                    }
                }
                drawflag = true;
                pc+=2;
            }
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

void chipeu::loadRom(const char* filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open ROM: " << filename << std::endl;
        return;
    }

    std::streamsize size = file.tellg();
    if (size <= 0 || size > 0xE00) {
        std::cerr << "Invalid ROM size: " << size << std::endl;
        file.close();
        return;
    }

    file.seekg(0, std::ios::beg);
    char* buffer = new char[size];
    if (!file.read(buffer, size)) {
        std::cerr << "Failed to read ROM data." << std::endl;
        delete[] buffer;
        file.close();
        return;
    }
    file.close();

    // Reset emulator state
    pc = 0x200;
    I = 0;
    sp = 0;
    delay_timer = 0;
    sound_timer = 0;
    drawflag = false;

    std::fill(std::begin(V), std::end(V), 0);
    std::fill(std::begin(stack), std::end(stack), 0);
    std::fill(std::begin(gfx), std::end(gfx), 0);
    std::fill(std::begin(key), std::end(key), 0);
    std::fill(memory + 0x200, memory + 4096, 0);  // Clear program area

    // Reload fontset
    for (int i = 0; i < 80; ++i) {
        memory[i] = chip8_fontset[i];
    }

    // Load ROM into memory
    for (std::streamsize i = 0; i < size; ++i) {
        memory[0x200 + i] = static_cast<unsigned char>(buffer[i]);
    }

    delete[] buffer;
}

const bool chipeu::getDraw() const{
    return drawflag;
}

void chipeu::setDraw(bool x){
    drawflag = x;
}

const unsigned char* chipeu::getGfx() const{
    return gfx;
}

void chipeu::setGfx(unsigned char x, unsigned int pos){
    if(pos<(64*32)){
        gfx[pos] = x;
    }
    else std::cout << "index out of range, index:" << x << std::endl;
}

