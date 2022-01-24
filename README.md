# MDE
Code for the Memory Degredation Experiement

MDE Payload Prototype v1 (4/29/18)

Most of the code should be fairly self-explanatory, but there are a few things that I thought I'd point out.

First, the program has two different configurations that it can compile in, debug and non-debug. In debug mode
messages are output over UART0 in a human-readable format. In non-debug mode, commands and data are handled
on the UART2 pin. The format for these commands and data is as follows:
Commands of one byte may be sent to the payload.
Data is sent in a 6 byte set, with form 0x80 0x80 0x80 (data byte with leading 0) (data byte) (data byte).
The data bytes themselves have the following scheme: 0(3 bit chip ID)(17 bit data address)(3 bit bit number)

To change between debug and non-debug mode, MDE_Payload_Prototype.h must be edited by commenting or uncommenting
the line #define DEBUG. If uncommented, then the program will compile to debug mode, or to non-debug mode
if commented. The program must be rebuilt and redownloaded to change this setting on the microcontroller.

The project structure is fairly simple, with control of the program being handled in MDE_Payload_Prototype.c,
and the functions for reading and writing to each chip being handled in separate .c files with the same name
as the chip IDs. These reading and writing functions should not have to be edited in any way, and I've tested
them fairly extensively.

Some parts of the control code are unfinished, however. There is a structure in place for handling commands
in non-debug mode, but there is nothing written yet to handle any commands, as they have not been decided
upon. Additionally, the format of data transmission in non-debug mode may have to be changed depending on
what is agreed upon.

Finally, the program has not been tested at all in non-debug mode, since it is difficult for me to test in
that configuration. Testing should be performed before non-debug mode is trusted to actually work.'


MDE Payload Prototype v2 (7/4/18)

This update fixes a few issues that led to unexpected results when not attempting to seed errors.


MDE Payload Prototype v2.1 (9/18/18)

This update introduces a minor but necessary feature that allows the microcontroller to recognize unresponsive
chips based on a criteria set by the user in the MDE_Payload_Prototype.h file. If a user-specified number of
blank bytes occur in a row, chips will be deemed unresponsive instead of spamming the error log.


MDE Payload Prototype v2.1.1 (9/21/18)

This update fixes a minor issue where a wiped or unwritten Flash chip wouldn't be recognized as unresponsive, causing it to spam the error log.


MDE Payload Prototype v2.2 (10/5/18)

This update introduces a health data tracker that tracks the status of the payload. Outputted health data includes
chip responsiveness, the current indexes of the start value and offset value, and the current cycle. Health data
can be manually outputted on the menu screen and is outputted automatically during auto mode. In non-debug
configurations, health data is outputted when the byte 0x53 is recieved on UART2RX.


MDE Payload Prototype v3.0 (10/9/18)

This update is an overhaul of the way that UART io is handled throughout the program. This update allows for the active UART port to be configured 
independently of whether the program is in the debug configuration or not, by setting a defines flag of ENABLE_UART0 or ENABLE_UART2. This flag can be 
set in the same block with UART_PRIMARY, near the top of the main header file.

MDE Payload Prototype v4.0 (2/11/20)

It's been a while since I put out an update so I'm not sure all that I've changed. I'll list the major points here:
* Ported to TM4C1294.
* Rewrote all the files to support the new chips.
* Overhauled the way that chips are selected to support multiple chips of one type.
* Probably other stuff I don't even remember.
