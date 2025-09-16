// chip8_fontset each character is 5 bytes
unsigned char chip8_fontset[80] = { 
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

// Basic chip-8 system structure
class chipeu {
    public:
        chipeu() : pc (0x200){
            // Loading the fonstet in to the memory
            for (int i = 0; i < 80; ++i){
                memory[i] = chip8_fontset[i];
            }
        }
        void emulateCycle();
        void loadRom(const char* filename);
    private:
        // 2 bytes for an opcode
        unsigned short opcode{};
        // 4 KiB of memory
        unsigned char memory[4096]{};
        // 16 CPU registers, 15 general purpose, 1 for carry flag
        unsigned char V[16]{};
        // Index Reigster
        unsigned short I{};
        // Program counter
        unsigned short pc{};
        // Display, sprites are XOR'ed on the screen
        unsigned char gfx[64 * 32]{};
        // Interuppts and hardware registers
        unsigned char delay_timer{};
        unsigned char sound_timer{};
        // Stack
        unsigned short stack[16]{};
        unsigned short sp{};
        // Input
        unsigned char key[16]{};
        bool drawflag {true};
};
