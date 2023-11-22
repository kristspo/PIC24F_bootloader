### Boot loader for PIC24F micro controllers
Boot loader is based on [PIC24 Bully boot loader](https://sites.google.com/site/pic24micro/Home/pic24-software-library-collection/pic24-bully-bootloader) but code is rewritten from scratch and linker scripts re-created. New boot loader implements write protected Boot code segment configuration of PIC24F micro controllers. This prevents any possibility to re-write or erase boot loader or micro controller configuration by boot loader itself or uploaded user code. The code does not use additional library functions and is quite compact. ISR functions are not used, so they can be defined in user code. The size of runtime startup code is further reduced by linker configuration in boot loader MPLAB X project.

Several MPLAB X projects are included.

**PIC24_boot.X** - boot loader code.  
**PIC24_user_led_blink.X** - led blink code using timer interrupt.  
**PIC24_user_pin_test.X** - tests all pins on development board in sequential pattern by setting each pin as output high, low, open drain and input with pull up and pull down.  
**PIC24_user_uart_buffer.X** - UART receive and transmit code using receive buffer to store received line and circular transmit buffer to send characters. Received line is changed to uppercase letters for transmitting.

There are some differences in how interrupt vectors are arranged comparing to original PIC24 Bully boot loader. This frees two more pages for user code. It also makes this version not compatible with original PIC24 Bully boot loader PC utility as original version will not upload user code below address 0xC00.

Boot loader uses following flash memory arrangement:  
**0x000**	Reset vector to write protected Boot code segment.  
**0x004**	Interrupts vector table with vectors to first page of General (user) code section  
**0x104**	Alternate vector table with same vectors as main interrupts vector table  
**0x200** 	Boot code segment that is write protected by configuration bits  
**0xB00**	First page of General code section with BRA instructions to user defined interrupt functions or default interrupt function provided by XC16 compiler. Instead of GOTO instruction that uses two program words branch instruction with one program word is used. It reduces interrupt ‘jump’ vector table size but limits possible locations of user defined interrupt functions to first 32 Kb of flash memory  
**0xC00**	User uploaded code

After reset if serial communication to boot loader is not activated within 0.5 seconds, boot loader checks if instruction at user code flash memory location is present. Boot loader then redirects control to User code or stays in boot loader mode indefinitely.

Boot loader updates EEPROM memory locations as well. Although not really documented, original Bully boot loader upload utility uses 0x05 code to send EEPROM memory contents if present in user hex file.

Boot loader code was tested on PIC24F16KA301 and PIC24F32KA302 micro controllers with XC16 compiler version v1.70 and v2.10. As next steps boot loader upload utility will need to be updated. There is also dormant [BullyCPP bootloader](https://github.com/thirtythreeforty/bullycpp) utility project that can be updated.
