rem set PATH=D:\browser\pic32\MPLAB\MPLAB C32 Suite\bin;%PATH%

del  Objects\*.o

pic32-gcc.exe -mprocessor=32MX220F032B -x c -c "usb_descriptors.c" -o".\Objects\usb_descriptors.o" -MMD -MF".\Objects\usb_descriptors.d" -I"." -I".." -I"..\Microchip\Include" -I"..\Microchip\Include\USB" -D__DEBUG -g -ffunction-sections -fdata-sections -mips16 -mno-float -DUBW32 -Os

pic32-gcc.exe -mprocessor=32MX220F032B -x c -c "main.c" -o".\Objects\main.o" -MMD -MF".\Objects\main.d" -I"." -I".." -I"..\Microchip\Include" -I"..\Microchip\Include\USB" -D__DEBUG -g -ffunction-sections -fdata-sections -mips16 -mno-float -DUBW32 -Os

pic32-gcc.exe -mprocessor=32MX220F032B -x c -c "..\Microchip\Usb\usb_device.c" -o".\Objects\usb_device.o" -MMD -MF".\Objects\usb_device.d" -I"." -I".." -I"..\Microchip\Include" -I"..\Microchip\Include\USB" -D__DEBUG -g -ffunction-sections -fdata-sections -mips16 -mno-float -DUBW32 -Os

pic32-gcc.exe -mprocessor=32MX220F032B -x c -c "..\Microchip\Usb\HID Device Driver\usb_function_hid.c" -o".\Objects\usb_function_hid.o" -MMD -MF".\Objects\usb_function_hid.d" -I"." -I".." -I"..\Microchip\Include" -I"..\Microchip\Include\USB" -D__DEBUG -g -ffunction-sections -fdata-sections -mips16 -mno-float -DUBW32 -Os

pic32-gcc.exe -mprocessor=32MX220F032B -x c -c "NVMem.c" -o".\Objects\NVMem.o" -MMD -MF".\Objects\NVMem.d" -I"." -I".." -I"..\Microchip\Include" -I"..\Microchip\Include\USB" -D__DEBUG -g -ffunction-sections -fdata-sections -mips16 -mno-float -DUBW32 -Os

pic32-gcc.exe -mprocessor=32MX220F032B "Objects\usb_descriptors.o" "Objects\main.o" "Objects\usb_device.o" "Objects\usb_function_hid.o" "Objects\NVMem.o" -o"HIDBoot.elf" -Os -mips16 -mno-float -Wl,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,-Map="HIDBoot.map",--gc-sections

pic32-bin2hex.exe HIDBoot.elf

