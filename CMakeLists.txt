cmake_minimum_required(VERSION 3.21)

# Setup paths to arduino environment and libraries
file(REAL_PATH "~/.arduino15/packages/STMicroelectronics/hardware/stm32/2.7.1" CORE_PATH EXPAND_TILDE)
file(TO_CMAKE_PATH "${CORE_PATH}" CORE_PATH)

file(REAL_PATH "~/Arduino/libraries" USER_LIBS EXPAND_TILDE)
file(TO_CMAKE_PATH "${USER_LIBS}" USER_LIBS)

# Setup the cmake module path and toolchain
# FIXME Testing cmake fixes
#list(APPEND CMAKE_MODULE_PATH ${CORE_PATH}/cmake)
list(APPEND CMAKE_MODULE_PATH /home/dpd/opsol_projects/Arduino_Core_STM32/cmake)
set(CMAKE_TOOLCHAIN_FILE toolchain)

# You may remove this block when using this file as the sketch's CMakeLists.txt
if (NOT ${CMAKE_PARENT_LIST_FILE} STREQUAL ${CMAKE_CURRENT_LIST_FILE})
    # When we are imported from the main CMakeLists.txt, we should stop here
    # not to interfere with the true build config.
    return()
endif()

project("display")

include(set_board)

# Options
# SERIAL generic / disabled / none
# USB none / CDCgen / CDC / HID
# XUSB FS / HS / HSFS
# VIRTIO disable / generic / enabled
# BOOTLOADER dfuo / dfu2 / hid
set_board("BLUEPILL_F103CB"
  USB none
)

# Add in some extra targets to make it easy to flash the target firmware
# to a device. Use "make firmware.flash" to invoke the flash command
# It is possible to list all targets supported by the makefile with
# "make help"
# FIXME Maybe replace these with the actual command ran by the scripts
# and put it all in its own ".cmake" file
function(flash_target TGT)
  add_custom_target(${TGT}.flash
    DEPENDS ${TGT}
    COMMAND stm32_program ${TGT}.elf
  )
  add_custom_target(${TGT}.flash_bmd
    DEPENDS ${TGT}
    COMMAND stm32_program_bmd ${TGT}.elf
  )
endfunction()

include(overall_settings)
overall_settings(
  # STANDARD_LIBC
  # PRINTF_FLOAT
  # SCANF_FLOAT
  # DEBUG_SYMBOLS
  # LTO
  # NO_RELATIVE_MACRO
  # UNDEF_NDEBUG
  # OPTIMIZATION "s"
  # BUILD_OPT ./build.opt
  # DISABLE_HAL_MODULES ADC I2C RTC SPI TIM DAC EXTI ETH SD QSPI
  # CORE_CALLBACK
)

# Need to build U8g2
add_definitions(-DARDUINO -DU8X8_NO_HW_SPI -DU8X8_NO_HW_I2C)

# The arduino-cli command installs user libraries in ~/Arduino/libraries
include(external_library)
external_library(
  PATH "${USER_LIBS}/U8g2"
  DEPENDS
    SPI
    WIRE
)

include_directories(src)

include(build_sketch)
build_sketch(TARGET init_display
  SOURCES
    src/init_display.cpp
  DEPENDS
    U8g2
)
flash_target(init_display)

build_sketch(TARGET display_raw SOURCES
  src/display_raw.cpp
  src/DisplayLC7981.cpp
)
flash_target(display_raw)

build_sketch(TARGET display_raw_graphics SOURCES
  src/display_raw_graphics.cpp
  src/DisplayLC7981.cpp
)
flash_target(display_raw_graphics)

build_sketch(TARGET full_frame
  SOURCES
    src/full_frame.cpp
  DEPENDS
    U8g2
)
flash_target(full_frame)

build_sketch(TARGET glue_test
  SOURCES
  src/glue_test.cpp
  src/DisplayLC7981.cpp
  src/DisplayLC7981U8G2.cpp
  DEPENDS
    U8g2
)
flash_target(glue_test)
