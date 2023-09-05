cmake_minimum_required(VERSION 3.13)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tools")
include(arm-none-eabi-gcc)

set(PRJ_NAME stm32g431_fd_can_fw)


project(${PRJ_NAME}
  LANGUAGES ASM C CXX
)


set(EXECUTABLE ${PRJ_NAME}.elf)
set(LINKER_FILE ${CMAKE_SOURCE_DIR}/STM32G431CBUX_FLASH.ld)


file(GLOB SRC_FILES CONFIGURE_DEPENDS
  App/hw/*.c
  App/hw/driver/*.c
  App/hw/driver/usb/*.c
  App/hw/driver/usb/usb_cdc/*.c
  App/exhw/*.c
  App/exhw/driver/*.c
  App/bsp/*.c
  Core/Src/main.cpp
  Core/Src/stm32g4xx_hal_msp.c
  Core/Src/stm32g4xx_it.c
  Core/Src/syscalls.c
  Core/Src/sysmem.c
  Core/Src/system_stm32g4xx.c

 # lib/EEPROM_Emul/Core/*.c
 # lib/EEPROM_Emul/Porting/STM32G4/*.c
)

file(GLOB_RECURSE SRC_FILES_RECURSE CONFIGURE_DEPENDS
App/*.cpp
App/ap/*.cpp
App/ap/*.c
App/common/*.c    
Drivers/STM32G4xx_HAL_Driver/Src/*.c
  #lib/rtos/*.c
lib/STM32_USB_Device_Library/Core/Src/*.c
lib/littlefs/*.c
lib/EEPROM_Emul/Core/*.c
lib/EEPROM_Emul/Porting/STM32G4/*.c
)


add_executable(${EXECUTABLE} 
Core/Startup/startup_stm32g431cbux.s
  ${SRC_FILES}
  ${SRC_FILES_RECURSE}
)



target_include_directories(${EXECUTABLE} PRIVATE 
App  
App/ap
App/ap/_inc
App/bsp
App/common 
App/common/core
App/common/inc_hw
App/common/inc_hw/qspi
App/common/inc_exhw
App/exhw
App/hw
App/hw/driver/usb
App/hw/driver/usb/usb_cdc
App/hw/driver/eeprom
Core/Inc

Drivers/CMSIS/Device/ST/STM32G4xx/Include
Drivers/CMSIS/Include
Drivers/STM32G4xx_HAL_Driver/Inc

lib/
lib/STM32_USB_Device_Library/Core/Inc
lib/EEPROM_Emul/Core
lib/EEPROM_Emul/Porting/STM32G4

)

target_compile_definitions(${EXECUTABLE} PRIVATE
  -DSTM32G431xx 
  )

target_compile_options(${EXECUTABLE} PRIVATE
  -mcpu=cortex-m4
  -mthumb
  -mfpu=fpv4-sp-d16
  -mfloat-abi=hard

  -fdata-sections
  -ffunction-sections

  -Wall
  -g3
  -Os
  )

target_link_options(${EXECUTABLE} PRIVATE
  -T${LINKER_FILE}  
  #-T../src/bsp/ldscript/STM32G431CBUx_FLASH.ld
  # -T../src/bsp/ldscript/STM32G431CBUx_BOOT.ld
  -mcpu=cortex-m4
  -mthumb
  -mfpu=fpv4-sp-d16
  -mfloat-abi=hard
  -specs=nano.specs
  -lc
  -lm
  -lstdc++
  -lsupc++
  #-lnosys
  -Wl,-Map=${PRJ_NAME}.map,--cref
  -Wl,--gc-sections
  -Xlinker -print-memory-usage -Xlinker
  )

add_custom_command(TARGET ${EXECUTABLE} 
  POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} ARGS -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin
  COMMENT "Invoking: Make Binary"
  )  