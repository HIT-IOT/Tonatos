################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
APP/%.o: ../APP/%.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"C:/ti/ccsv7/tools/compiler/msp430-gcc-6.4.0.32_win32/bin/msp430-elf-gcc.exe" -c -mmcu=msp430fr5969 -I"E:/workspace_v7/Tonatos/HAL/include" -I"E:/workspace_v7/Tonatos/BSP/include" -I"E:/workspace_v7/Tonatos/include" -I"E:/workspace_v7/Tonatos/OSAL/include" -I"C:/ti/ccsv7/ccs_base/msp430/include_gcc" -I"E:/workspace_v7/Tonatos" -I"C:/ti/ccsv7/tools/compiler/msp430-gcc-6.4.0.32_win32/msp430-elf/include" -Os -g -gdwarf-3 -gstrict-dwarf -Wall -mlarge -mhwmult=f5series -mcode-region=none -mdata-region=none -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


