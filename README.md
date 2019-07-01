# maple-reset
A simple tool to reset a Maple STM32F103 (aka 'BluePill') board on Windows

## About
Maple boards are ultra-cheap compact devices based on the STM32F103 chip.  They need a specific sequence sent on the serial port in order to enter DFU mode.  The sequence is DTR going from true to false, followed by the string `1EAF` being sent.

The program does exactly that - toggles DTR and sends `1EAF`, putting the board into DFU mode for programming.

## Usage
`maple-reset.exe [PORT]`

For example:
```
C:\>maple-reset.exe COM7
maple-reset 0.1
This program is Free Sofware and has NO WARRANTY

https://github.com/benlye/maple-reset

Reset sequence sent to COM7.

C:\>
```

## Download
Get the latest binary from the Releases page.
