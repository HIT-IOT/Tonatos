################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
HAL/HAL.obj: ../HAL/HAL.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/bin/cl430" -vmspx --data_model=restricted --near_data=none --use_hw_mpy=F5 --include_path="C:/ti/ccsv7/ccs_base/msp430/include" --include_path="E:/workspace_v7/Tonatos/BSP/driverlib/MSP430FR5xx_6xx/" --include_path="E:/workspace_v7/Tonatos/include" --include_path="E:/workspace_v7/Tonatos/BSP/include" --include_path="E:/workspace_v7/Tonatos/HAL/include" --include_path="E:/workspace_v7/Tonatos/OSAL/include" --include_path="E:/workspace_v7/Tonatos" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/include" --advice:power="all" --advice:hw_config=all --define=__MSP430FR6979__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="HAL/HAL.d_raw" --obj_directory="HAL" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

HAL/MAC_radio.obj: ../HAL/MAC_radio.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/bin/cl430" -vmspx --data_model=restricted --near_data=none --use_hw_mpy=F5 --include_path="C:/ti/ccsv7/ccs_base/msp430/include" --include_path="E:/workspace_v7/Tonatos/BSP/driverlib/MSP430FR5xx_6xx/" --include_path="E:/workspace_v7/Tonatos/include" --include_path="E:/workspace_v7/Tonatos/BSP/include" --include_path="E:/workspace_v7/Tonatos/HAL/include" --include_path="E:/workspace_v7/Tonatos/OSAL/include" --include_path="E:/workspace_v7/Tonatos" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/include" --advice:power="all" --advice:hw_config=all --define=__MSP430FR6979__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="HAL/MAC_radio.d_raw" --obj_directory="HAL" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

HAL/RTC.obj: ../HAL/RTC.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/bin/cl430" -vmspx --data_model=restricted --near_data=none --use_hw_mpy=F5 --include_path="C:/ti/ccsv7/ccs_base/msp430/include" --include_path="E:/workspace_v7/Tonatos/BSP/driverlib/MSP430FR5xx_6xx/" --include_path="E:/workspace_v7/Tonatos/include" --include_path="E:/workspace_v7/Tonatos/BSP/include" --include_path="E:/workspace_v7/Tonatos/HAL/include" --include_path="E:/workspace_v7/Tonatos/OSAL/include" --include_path="E:/workspace_v7/Tonatos" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/include" --advice:power="all" --advice:hw_config=all --define=__MSP430FR6979__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="HAL/RTC.d_raw" --obj_directory="HAL" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

HAL/SX1211.obj: ../HAL/SX1211.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/bin/cl430" -vmspx --data_model=restricted --near_data=none --use_hw_mpy=F5 --include_path="C:/ti/ccsv7/ccs_base/msp430/include" --include_path="E:/workspace_v7/Tonatos/BSP/driverlib/MSP430FR5xx_6xx/" --include_path="E:/workspace_v7/Tonatos/include" --include_path="E:/workspace_v7/Tonatos/BSP/include" --include_path="E:/workspace_v7/Tonatos/HAL/include" --include_path="E:/workspace_v7/Tonatos/OSAL/include" --include_path="E:/workspace_v7/Tonatos" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/include" --advice:power="all" --advice:hw_config=all --define=__MSP430FR6979__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="HAL/SX1211.d_raw" --obj_directory="HAL" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

HAL/SX1211_Board.obj: ../HAL/SX1211_Board.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/bin/cl430" -vmspx --data_model=restricted --near_data=none --use_hw_mpy=F5 --include_path="C:/ti/ccsv7/ccs_base/msp430/include" --include_path="E:/workspace_v7/Tonatos/BSP/driverlib/MSP430FR5xx_6xx/" --include_path="E:/workspace_v7/Tonatos/include" --include_path="E:/workspace_v7/Tonatos/BSP/include" --include_path="E:/workspace_v7/Tonatos/HAL/include" --include_path="E:/workspace_v7/Tonatos/OSAL/include" --include_path="E:/workspace_v7/Tonatos" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/include" --advice:power="all" --advice:hw_config=all --define=__MSP430FR6979__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="HAL/SX1211_Board.d_raw" --obj_directory="HAL" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

HAL/diagnose.obj: ../HAL/diagnose.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/bin/cl430" -vmspx --data_model=restricted --near_data=none --use_hw_mpy=F5 --include_path="C:/ti/ccsv7/ccs_base/msp430/include" --include_path="E:/workspace_v7/Tonatos/BSP/driverlib/MSP430FR5xx_6xx/" --include_path="E:/workspace_v7/Tonatos/include" --include_path="E:/workspace_v7/Tonatos/BSP/include" --include_path="E:/workspace_v7/Tonatos/HAL/include" --include_path="E:/workspace_v7/Tonatos/OSAL/include" --include_path="E:/workspace_v7/Tonatos" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/include" --advice:power="all" --advice:hw_config=all --define=__MSP430FR6979__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="HAL/diagnose.d_raw" --obj_directory="HAL" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

HAL/hal_LCDC_4.obj: ../HAL/hal_LCDC_4.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/bin/cl430" -vmspx --data_model=restricted --near_data=none --use_hw_mpy=F5 --include_path="C:/ti/ccsv7/ccs_base/msp430/include" --include_path="E:/workspace_v7/Tonatos/BSP/driverlib/MSP430FR5xx_6xx/" --include_path="E:/workspace_v7/Tonatos/include" --include_path="E:/workspace_v7/Tonatos/BSP/include" --include_path="E:/workspace_v7/Tonatos/HAL/include" --include_path="E:/workspace_v7/Tonatos/OSAL/include" --include_path="E:/workspace_v7/Tonatos" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/include" --advice:power="all" --advice:hw_config=all --define=__MSP430FR6979__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="HAL/hal_LCDC_4.d_raw" --obj_directory="HAL" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

HAL/hal_beeper.obj: ../HAL/hal_beeper.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/bin/cl430" -vmspx --data_model=restricted --near_data=none --use_hw_mpy=F5 --include_path="C:/ti/ccsv7/ccs_base/msp430/include" --include_path="E:/workspace_v7/Tonatos/BSP/driverlib/MSP430FR5xx_6xx/" --include_path="E:/workspace_v7/Tonatos/include" --include_path="E:/workspace_v7/Tonatos/BSP/include" --include_path="E:/workspace_v7/Tonatos/HAL/include" --include_path="E:/workspace_v7/Tonatos/OSAL/include" --include_path="E:/workspace_v7/Tonatos" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/include" --advice:power="all" --advice:hw_config=all --define=__MSP430FR6979__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="HAL/hal_beeper.d_raw" --obj_directory="HAL" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

HAL/hal_button.obj: ../HAL/hal_button.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/bin/cl430" -vmspx --data_model=restricted --near_data=none --use_hw_mpy=F5 --include_path="C:/ti/ccsv7/ccs_base/msp430/include" --include_path="E:/workspace_v7/Tonatos/BSP/driverlib/MSP430FR5xx_6xx/" --include_path="E:/workspace_v7/Tonatos/include" --include_path="E:/workspace_v7/Tonatos/BSP/include" --include_path="E:/workspace_v7/Tonatos/HAL/include" --include_path="E:/workspace_v7/Tonatos/OSAL/include" --include_path="E:/workspace_v7/Tonatos" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/include" --advice:power="all" --advice:hw_config=all --define=__MSP430FR6979__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="HAL/hal_button.d_raw" --obj_directory="HAL" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

HAL/hal_digio.obj: ../HAL/hal_digio.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/bin/cl430" -vmspx --data_model=restricted --near_data=none --use_hw_mpy=F5 --include_path="C:/ti/ccsv7/ccs_base/msp430/include" --include_path="E:/workspace_v7/Tonatos/BSP/driverlib/MSP430FR5xx_6xx/" --include_path="E:/workspace_v7/Tonatos/include" --include_path="E:/workspace_v7/Tonatos/BSP/include" --include_path="E:/workspace_v7/Tonatos/HAL/include" --include_path="E:/workspace_v7/Tonatos/OSAL/include" --include_path="E:/workspace_v7/Tonatos" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/include" --advice:power="all" --advice:hw_config=all --define=__MSP430FR6979__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="HAL/hal_digio.d_raw" --obj_directory="HAL" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

HAL/hal_int.obj: ../HAL/hal_int.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/bin/cl430" -vmspx --data_model=restricted --near_data=none --use_hw_mpy=F5 --include_path="C:/ti/ccsv7/ccs_base/msp430/include" --include_path="E:/workspace_v7/Tonatos/BSP/driverlib/MSP430FR5xx_6xx/" --include_path="E:/workspace_v7/Tonatos/include" --include_path="E:/workspace_v7/Tonatos/BSP/include" --include_path="E:/workspace_v7/Tonatos/HAL/include" --include_path="E:/workspace_v7/Tonatos/OSAL/include" --include_path="E:/workspace_v7/Tonatos" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/include" --advice:power="all" --advice:hw_config=all --define=__MSP430FR6979__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="HAL/hal_int.d_raw" --obj_directory="HAL" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

HAL/hal_led.obj: ../HAL/hal_led.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/bin/cl430" -vmspx --data_model=restricted --near_data=none --use_hw_mpy=F5 --include_path="C:/ti/ccsv7/ccs_base/msp430/include" --include_path="E:/workspace_v7/Tonatos/BSP/driverlib/MSP430FR5xx_6xx/" --include_path="E:/workspace_v7/Tonatos/include" --include_path="E:/workspace_v7/Tonatos/BSP/include" --include_path="E:/workspace_v7/Tonatos/HAL/include" --include_path="E:/workspace_v7/Tonatos/OSAL/include" --include_path="E:/workspace_v7/Tonatos" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/include" --advice:power="all" --advice:hw_config=all --define=__MSP430FR6979__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="HAL/hal_led.d_raw" --obj_directory="HAL" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

HAL/hal_mcu.obj: ../HAL/hal_mcu.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/bin/cl430" -vmspx --data_model=restricted --near_data=none --use_hw_mpy=F5 --include_path="C:/ti/ccsv7/ccs_base/msp430/include" --include_path="E:/workspace_v7/Tonatos/BSP/driverlib/MSP430FR5xx_6xx/" --include_path="E:/workspace_v7/Tonatos/include" --include_path="E:/workspace_v7/Tonatos/BSP/include" --include_path="E:/workspace_v7/Tonatos/HAL/include" --include_path="E:/workspace_v7/Tonatos/OSAL/include" --include_path="E:/workspace_v7/Tonatos" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/include" --advice:power="all" --advice:hw_config=all --define=__MSP430FR6979__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="HAL/hal_mcu.d_raw" --obj_directory="HAL" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


