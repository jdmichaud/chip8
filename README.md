# Chip8 emulator

![Example](trip8.gif)

## Usage

You will need a C compiler and make.
```bash
make c8
```

To build the disassembler:
```bash
make d8
```

To run a rom:
```
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./raylib-2.5.0-Linux-amd64/lib/ ./c8 <roms_file>
```

## Dependencies

This chip8 emulator relied on [raylib](https://www.raylib.com/).
