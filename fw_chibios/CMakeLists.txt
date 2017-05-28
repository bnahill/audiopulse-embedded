#
# (C) Copyright 2013 Ben Nahill
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# @file
# @brief
# @author Ben Nahill <bnahill@gmail.com>
#


# CMakeLists for Kinetis K20/22 and asm/C/C++ sources
#
# Author: Ben Nahill <bnahill@gmail.com>
#



# Use the GNU ARM EABI toolchain
set(TC_PREFIX arm-none-eabi-)

set(CMAKE_TOOLCHAIN_FILE toolchain.cmake)

set(TARGET apulse_iface)
set(OUTFILE ${TARGET}.elf)

project(${TARGET})
cmake_minimum_required(VERSION 2.8)

########################################################
# Source locations
########################################################

# Path to CMSIS
set(OS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ChibiOS-RT)


########################################################
# Build Options
########################################################

set(TARGET_STM32F722 TRUE)

set(OPTIMIZATION "-O2")
set(LTO )#"-flto")

set(PROCESS_STACKSIZE 0x1000)
set(EXCEPTION_STACKSIZE 0x1000)

if(${TARGET_STM32F722})
	set(MCFLAGS "${MCFLAGS} -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-sp-d16 -fsingle-precision-constant")
	add_definitions(-DCORTEX_USE_FPU=1 )
	
	add_definitions(-DARM_MATH_CM7 -D__FPU_PRESENT=1 -DLITTLE_ENDIAN)
endif()

set(MCFLAGS "${MCFLAGS} -g3 -mthumb -fomit-frame-pointer -fdelete-null-pointer-checks -falign-functions=16 \
    -mno-thumb-interwork -DTHUMB_PRESENT -DTHUMB_NO_INTERWORKING -MD -MP -MF \
    -ffunction-sections -ffreestanding -fdata-sections -fno-common")
#add_definitions(${MCFLAGS})

set(LINKER_DIR ${OS_DIR}/os/common/startup/ARMCMx/compilers/GCC/ld)
set(LINKER_SCRIPT STM32F722xE.ld)

###################################################
# Compiler Flags
###################################################
set(CXX_WARNINGS "-Waddress -Warray-bounds -Wc++11-compat -Wchar-subscripts ")
set(CXX_WARNINGS "${CXX_WARNINGS} -Wcomment -Wformat -Wmaybe-uninitialized -Wnonnull -Wparentheses -Wreturn-type ")
set(CXX_WARNINGS "${CXX_WARNINGS} -Wsequence-point -Wstrict-aliasing -Wstrict-overflow=1 -Wswitch -Wtrigraphs ") #-Wsign-compare
set(CXX_WARNINGS "${CXX_WARNINGS} -Wuninitialized -Wunknown-pragmas -Wunused-function -Wunused-label -Wunused-value -Wunused-variable -Wvolatile-register-var")

set(C_WARNINGS "-Wall")

# Flags for both C and C++ compilers
set(COMMON_FLAGS "${LTO}  ${MCFLAGS}")
# Eliminate the need for a bunch of extra support code that may not work on the chip anyways
set(CMAKE_CXX_FLAGS "--std=gnu++14 -fno-rtti -fno-exceptions ${CXX_WARNINGS} ${COMMON_FLAGS}")
# Use the new C11 standard
set(CMAKE_ASM_FLAGS "${MCFLAGS}")
set(CMAKE_C_FLAGS "--std=gnu11 ${C_WARNINGS} ${COMMON_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "-L${LINKER_DIR} -T${LINKER_SCRIPT} ${LTO} ${OPTIMIZATION} -Wl,-Map=${TARGET}.map,--cref,--gc-sections,--defsym=__main_stack_size__=${EXCEPTION_STACKSIZE},--defsym=__process_stack_size__=${PROCESS_STACKSIZE} -nostartfiles ${MCFLAGS} ${CMAKE_EXE_LINKER_FLAGS}")

########################################################
# Sources
########################################################

set(OS_STARTUP_SRC
    ${OS_DIR}/os/common/startup/ARMCMx/compilers/GCC/crt1.c
    ${OS_DIR}/os/common/startup/ARMCMx/compilers/GCC/vectors.c
    ${OS_DIR}/os/common/startup/ARMCMx/compilers/GCC/crt0_v7m.S
)

set(OS_STARTUP_INC
    ${OS_DIR}/os/common/startup/ARMCMx/compilers/GCC
    ${OS_DIR}/os/common/startup/ARMCMx/devices/STM32F7xx
    ${OS_DIR}/os/common/ext/CMSIS/include
    ${OS_DIR}/os/common/ext/CMSIS/ST/STM32F7xx
)

set(OS_HAL_SRC
    ${OS_DIR}/os/hal/src/hal.c
    ${OS_DIR}/os/hal/src/hal_buffers.c
    ${OS_DIR}/os/hal/src/hal_queues.c
    ${OS_DIR}/os/hal/src/hal_mmcsd.c
    ${OS_DIR}/os/hal/src/hal_adc.c
    ${OS_DIR}/os/hal/src/hal_can.c
    ${OS_DIR}/os/hal/src/hal_dac.c
    ${OS_DIR}/os/hal/src/hal_ext.c
    ${OS_DIR}/os/hal/src/hal_gpt.c
    ${OS_DIR}/os/hal/src/hal_i2c.c
    ${OS_DIR}/os/hal/src/hal_i2s.c
    ${OS_DIR}/os/hal/src/hal_icu.c
    ${OS_DIR}/os/hal/src/hal_mac.c
    ${OS_DIR}/os/hal/src/hal_mmc_spi.c
    ${OS_DIR}/os/hal/src/hal_pal.c
    ${OS_DIR}/os/hal/src/hal_pwm.c
    ${OS_DIR}/os/hal/src/hal_qspi.c
    ${OS_DIR}/os/hal/src/hal_rtc.c
    ${OS_DIR}/os/hal/src/hal_sdc.c
    ${OS_DIR}/os/hal/src/hal_serial.c
    ${OS_DIR}/os/hal/src/hal_serial_usb.c
    ${OS_DIR}/os/hal/src/hal_spi.c
    ${OS_DIR}/os/hal/src/hal_st.c
    ${OS_DIR}/os/hal/src/hal_uart.c
    ${OS_DIR}/os/hal/src/hal_usb.c
    ${OS_DIR}/os/hal/src/hal_wdg.c
)

set(OS_HAL_INC
    ${OS_DIR}/os/hal/include
)

set(OS_PLATFORM_SRC
    ${OS_DIR}/os/hal/ports/common/ARMCMx/nvic.c
    ${OS_DIR}/os/hal/ports/STM32/STM32F7xx/hal_lld.c
    ${OS_DIR}/os/hal/ports/STM32/STM32F7xx/hal_ext_lld_isr.c
    ${OS_DIR}/os/hal/ports/STM32/LLD/ADCv2/hal_adc_lld.c
    ${OS_DIR}/os/hal/ports/STM32/LLD/CANv1/hal_can_lld.c
    ${OS_DIR}/os/hal/ports/STM32/LLD/DACv1/hal_dac_lld.c
    ${OS_DIR}/os/hal/ports/STM32/LLD/DMAv2/stm32_dma.c
    ${OS_DIR}/os/hal/ports/STM32/LLD/EXTIv1/hal_ext_lld.c
    ${OS_DIR}/os/hal/ports/STM32/LLD/GPIOv2/hal_pal_lld.c
    ${OS_DIR}/os/hal/ports/STM32/LLD/I2Cv2/hal_i2c_lld.c
    ${OS_DIR}/os/hal/ports/STM32/LLD/MACv1/hal_mac_lld.c
    ${OS_DIR}/os/hal/ports/STM32/LLD/OTGv1/hal_usb_lld.c
    ${OS_DIR}/os/hal/ports/STM32/LLD/QUADSPIv1/hal_qspi_lld.c
    ${OS_DIR}/os/hal/ports/STM32/LLD/RTCv2/hal_rtc_lld.c
    ${OS_DIR}/os/hal/ports/STM32/LLD/SPIv2/hal_spi_lld.c
    ${OS_DIR}/os/hal/ports/STM32/LLD/SPIv2/hal_i2s_lld.c
    ${OS_DIR}/os/hal/ports/STM32/LLD/SDMMCv1/hal_sdc_lld.c
    ${OS_DIR}/os/hal/ports/STM32/LLD/TIMv1/hal_gpt_lld.c
    ${OS_DIR}/os/hal/ports/STM32/LLD/TIMv1/hal_icu_lld.c
    ${OS_DIR}/os/hal/ports/STM32/LLD/TIMv1/hal_pwm_lld.c
    ${OS_DIR}/os/hal/ports/STM32/LLD/TIMv1/hal_st_lld.c
    ${OS_DIR}/os/hal/ports/STM32/LLD/TIMv1/stm32_tim.c
    ${OS_DIR}/os/hal/ports/STM32/LLD/USARTv2/hal_serial_lld.c
    ${OS_DIR}/os/hal/ports/STM32/LLD/USARTv2/hal_uart_lld.c
    ${OS_DIR}/os/hal/ports/STM32/LLD/xWDGv1/hal_wdg_lld.c
)

set(OS_PLATFORM_INC
    ${OS_DIR}/os/hal/ports/common/ARMCMx
    ${OS_DIR}/os/hal/ports/STM32/STM32F7xx
    ${OS_DIR}/os/hal/ports/STM32/LLD/ADCv2
    ${OS_DIR}/os/hal/ports/STM32/LLD/CANv1
    ${OS_DIR}/os/hal/ports/STM32/LLD/DACv1
    ${OS_DIR}/os/hal/ports/STM32/LLD/DMAv2
    ${OS_DIR}/os/hal/ports/STM32/LLD/EXTIv1
    ${OS_DIR}/os/hal/ports/STM32/LLD/GPIOv2
    ${OS_DIR}/os/hal/ports/STM32/LLD/I2Cv2
    ${OS_DIR}/os/hal/ports/STM32/LLD/MACv1
    ${OS_DIR}/os/hal/ports/STM32/LLD/OTGv1
    ${OS_DIR}/os/hal/ports/STM32/LLD/QUADSPIv1
    ${OS_DIR}/os/hal/ports/STM32/LLD/RTCv2
    ${OS_DIR}/os/hal/ports/STM32/LLD/SPIv2
    ${OS_DIR}/os/hal/ports/STM32/LLD/SDMMCv1
    ${OS_DIR}/os/hal/ports/STM32/LLD/TIMv1
    ${OS_DIR}/os/hal/ports/STM32/LLD/USARTv2
    ${OS_DIR}/os/hal/ports/STM32/LLD/xWDGv1
)

set(OS_BOARD_SRC
    ${OS_DIR}/os/hal/boards/AUDIOPULSE/board.c
)

set(OS_BOARD_INC
    ${OS_DIR}/os/hal/boards/AUDIOPULSE
)

set(OS_OSAL_SRC
    ${OS_DIR}/os/hal/osal/rt/osal.c
)

set(OS_OSAL_INC
    ${OS_DIR}/os/hal/osal/rt
)

set(OS_KERNEL_SRC
    ${OS_DIR}/os/rt/src/chsys.c
    ${OS_DIR}/os/rt/src/chdebug.c
    ${OS_DIR}/os/rt/src/chtrace.c
    ${OS_DIR}/os/rt/src/chvt.c
    ${OS_DIR}/os/rt/src/chschd.c
    ${OS_DIR}/os/rt/src/chthreads.c
    ${OS_DIR}/os/rt/src/chtm.c
    ${OS_DIR}/os/rt/src/chstats.c
    ${OS_DIR}/os/rt/src/chregistry.c
    ${OS_DIR}/os/rt/src/chsem.c
    ${OS_DIR}/os/rt/src/chmtx.c
    ${OS_DIR}/os/rt/src/chcond.c
    ${OS_DIR}/os/rt/src/chevents.c
    ${OS_DIR}/os/rt/src/chmsg.c
    ${OS_DIR}/os/rt/src/chdynamic.c
    ${OS_DIR}/os/common/oslib/src/chmboxes.c
    ${OS_DIR}/os/common/oslib/src/chmemcore.c
    ${OS_DIR}/os/common/oslib/src/chheap.c
    ${OS_DIR}/os/common/oslib/src/chmempools.c
)

set(OS_KERNEL_INC
    ${OS_DIR}/os/rt/include
    ${OS_DIR}/os/common/oslib/include
)


set(OS_PORT_SRC
    ${OS_DIR}/os/common/ports/ARMCMx/chcore.c
    ${OS_DIR}/os/common/ports/ARMCMx/chcore_v7m.c
    ${OS_DIR}/os/common/ports/ARMCMx/compilers/GCC/chcoreasm_v7m.S
)

set(OS_PORT_INC
    ${OS_DIR}/os/common/ports/ARMCMx
    ${OS_DIR}/os/common/ports/ARMCMx/compilers/GCC
)

set(OS_STREAMS_SRC
    ${OS_DIR}/os/hal/lib/streams/chprintf.c
    ${OS_DIR}/os/hal/lib/streams/memstreams.c
    ${OS_DIR}/os/hal/lib/streams/nullstreams.c
)

set(OS_STREAMS_INC
    ${OS_DIR}/os/hal/lib/streams
)

set(OS_TEST_SRC
    ${OS_DIR}/test/lib/ch_test.c
    ${OS_DIR}/test/rt/source/test/test_root.c
    ${OS_DIR}/test/rt/source/test/test_sequence_001.c
    ${OS_DIR}/test/rt/source/test/test_sequence_002.c
    ${OS_DIR}/test/rt/source/test/test_sequence_003.c
    ${OS_DIR}/test/rt/source/test/test_sequence_004.c
    ${OS_DIR}/test/rt/source/test/test_sequence_005.c
    ${OS_DIR}/test/rt/source/test/test_sequence_006.c
    ${OS_DIR}/test/rt/source/test/test_sequence_007.c
    ${OS_DIR}/test/rt/source/test/test_sequence_008.c
    ${OS_DIR}/test/rt/source/test/test_sequence_009.c
    ${OS_DIR}/test/rt/source/test/test_sequence_010.c
    ${OS_DIR}/test/rt/source/test/test_sequence_011.c
    ${OS_DIR}/test/rt/source/test/test_sequence_012.c
)

set(OS_TEST_INC
    ${OS_DIR}/test/lib
    ${OS_DIR}/test/rt/source/test
)

set(APP_SRC
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/ctrl_if.cpp
    #${CMAKE_SOURCE_DIR}/src/usbcfg.c
    ${CMAKE_SOURCE_DIR}/src/codec.cpp
)

set(APP_INC
    ${OS_DIR}/os/license
    ${OS_DIR}/os/various
    ${CMAKE_SOURCE_DIR}/src
)

set_property(SOURCE ${OS_DIR}/os/common/ports/ARMCMx/compilers/GCC/chcoreasm_v7m.S PROPERTY LANGUAGE C)
set_property(SOURCE ${OS_DIR}/os/common/startup/ARMCMx/compilers/GCC/crt0_v7m.S PROPERTY LANGUAGE C)

##############################################################
# Executable and library targets
##############################################################

add_executable(${OUTFILE} ${APP_SRC} ${OS_BOARD_SRC} ${OS_HAL_SRC} ${OS_KERNEL_SRC} ${OS_OSAL_SRC} ${OS_PLATFORM_SRC} ${OS_PORT_SRC} ${OS_STARTUP_SRC} ${OS_STREAMS_SRC})
target_include_directories(${OUTFILE} PUBLIC ${APP_INC} ${OS_BOARD_INC} ${OS_HAL_INC} ${OS_KERNEL_INC} ${OS_OSAL_INC} ${OS_PLATFORM_INC} ${OS_PORT_INC} ${OS_STARTUP_INC} ${OS_STREAMS_INC})
set_target_properties(${OUTFILE} PROPERTIES COMPILE_FLAGS "${OPTIMIZATION}")


##############################################################
# Assembly output
##############################################################

add_custom_command(OUTPUT ${TARGET}.lst COMMAND ${CMAKE_OBJDUMP} -S --disassemble ${OUTFILE} > ${TARGET}.lst DEPENDS ${OUTFILE})
add_custom_target(assembly_output ALL DEPENDS ${TARGET}.lst)


##############################################################
# Configuration for GDB
##############################################################

set(GDB    "${TC_PREFIX}gdb")
set(GDBTUI ${TC_PREFIX}gdb -tui -ex \"layout split\" -ex \"focus CMD\")

#if(${DETECTED_HOST} STREQUAL "Windows")
#endif(${DETECTED_HOST} STREQUAL "Windows")

#if(${DETECTED_HOST} STREQUAL "Linux")
##configure_file(${CMAKE_SOURCE_DIR}/start.gdb.in ${CMAKE_CURRENT_BINARY_DIR}/.gdbinit)
#endif(${DETECTED_HOST} STREQUAL "Linux")

add_custom_target(gdb COMMAND ${GDB} ${GDB_ARGS})
add_custom_target(gdbtui COMMAND ${GDBTUI} ${GDB_ARGS})
add_custom_target(ddd COMMAND ddd --gdb --debugger \"${GDB} ${GDB_ARGS}\" > /dev/null 2>&1 &
	COMMENT "Launching DDD"
)

#add_dependencies(gdb gdb_script)
#add_dependencies(gdbtui gdb_script)
#add_dependencies(ddd gdb_script)


add_custom_command(
	OUTPUT ${TARGET}_size DEPENDS ${OUTFILE}
	COMMAND arm-none-eabi-size --format=berkeley ${OUTFILE}
)
add_custom_target(
	${TARGET}_outputs ALL DEPENDS ${TARGET}_size #${name}.map ${name}.bin ${name}.ihx ${name}.dump ${name}.rom
)

##############################################################
# Doxygen Output
##############################################################

#find_package(Doxygen)
#if(DOXYGEN_FOUND)
	#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
	#add_custom_target(doc
		#${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		#WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		#COMMENT "Generating documentation with Doxygen" VERBATIM
	#)
#endif(DOXYGEN_FOUND)

#############################################################
# HEX file generation
#############################################################

add_custom_target(${TARGET}.hex ALL
	COMMAND ${TC_PREFIX}objcopy -O ihex ${OUTFILE} ${TARGET}.hex
	DEPENDS ${OUTFILE}
	COMMENT "Generating Intel HEX output..."
)



