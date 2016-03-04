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

# Include the correct toolchain file and set a variable so that we can
# determine the correct host type later
if(WIN32)
	message("Detected WIN32 build environment")
	set(IS_UNIX false)
	set(DETECTED_HOST "Windows")
	set(CMAKE_TOOLCHAIN_FILE toolchain_win32.cmake)
	set(ENV{HOME} ${CMAKE_CURRENT_BINARY_DIR})
endif(WIN32)

if(UNIX)
	message("Detected UNIX build environment")
	set(DETECTED_HOST "UNIX")
	set(CMAKE_TOOLCHAIN_FILE toolchain.cmake)
endif(UNIX)

set(BUILD_HOST ${DETECTED_HOST} CACHE STRING "Build host (Check this!)")


set(TARGET apulse_iface)
set(OUTFILE ${TARGET}.elf)

project(${TARGET})
cmake_minimum_required(VERSION 2.8)


########################################################
# Source locations
########################################################

# Path to CMSIS
set(CMSIS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS)
set(DSP_SRC ${CMSIS_DIR}/DSP_Lib/Source)
set(USB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/FreescaleUSBv4.1.1)
set(PT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pt-1.4)

option(DEBUG_USE_SVD "Use pycmdebug and an SVD file for debugging" OFF)

option(TARGET_K22 "Target the 120MHz K22 chip with floating point" OFF)
option(ENABLE_LOOPBACK "Enable a loopback connection from output to input" OFF)
option(ENABLE_INPUT_DEBUG "Check for errors in input timing" ON)
option(ENABLE_INPUT_IIR "Use an IIR filter instead of FIR for decimation" OFF)
option(ENABLE_INPUT_CALIBRATION "Apply per-frequency calibration to input" OFF)
option(POWER_ALWAYS_ON "Keep analog power always on" ON)

if(${TARGET_K22})
	add_definitions(-DCFG_TARGET_K22=true)
	add_definitions(-DCFG_TARGET_K20=false)
	add_definitions(-DCPU_MK22FN512VLH12)
	message("Building for Kinetis K22")
	set(JLINK_DEVICE "MK22FN512xxx12")
	set(CLK "120000000")
else()
	add_definitions(-DCFG_TARGET_K22=false)
	add_definitions(-DCFG_TARGET_K20=true)
	add_definitions(-DCPU_MK20DX256VLH7)
	message("Building for Kinetis K20")
	set(JLINK_DEVICE "MK20DX256xxx7")
	set(CLK "48000000")
endif()

if(${ENABLE_LOOPBACK})
	add_definitions(-DCFG_ENABLE_LOOPBACK=true)
	message("Building with output loopback")
else()
	add_definitions(-DCFG_ENABLE_LOOPBACK=false)
	message("Building without output loopback")
endif()

if(${ENABLE_INPUT_DEBUG})
	add_definitions(-DCFG_ENABLE_INPUT_DEBUG=true)
	message("Building with input debugging")
else()
	add_definitions(-DCFG_ENABLE_INPUT_DEBUG=false)
	message("Building without input debugging")
endif()

if(${ENABLE_INPUT_IIR})
	add_definitions(-DCFG_ENABLE_INPUT_IIR=true)
	message("Building with IIR input filtering")
else()
	add_definitions(-DCFG_ENABLE_INPUT_IIR=false)
	message("Building with FIR input filtering")
endif()

if(${ENABLE_INPUT_CALIBRATION})
	add_definitions(-DCFG_ENABLE_INPUT_CALIBRATION=true)
	message("Building with input calibration")
else()
	add_definitions(-DCFG_ENABLE_INPUT_CALIBRATION=false)
	message("Building without input calibration")
endif()



if(${POWER_ALWAYS_ON})
        add_definitions(-DCFG_POWER_ALWAYS_ON=true)
        message("Building with power always on")
else()
        add_definitions(-DCFG_POWER_ALWAYS_ON=false)
        message("Building with power selectively on")
endif()

########################################################
# Build Options
########################################################

set(OPTIMIZATION "-Og")
set(LTO )#"-flto")

set(MCU cortex-m4)

set(MCFLAGS "-mcpu=${MCU} -g3 -mthumb -fomit-frame-pointer -D__NEWLIB__ -fno-math-errno -fdelete-null-pointer-checks")



if(${TARGET_K22})
	set(MCFLAGS "${MCFLAGS} -mfloat-abi=hard -mfpu=fpv4-sp-d16")
	add_definitions(-DCORTEX_USE_FPU=1 )
	add_definitions(${MCFLAGS})
	add_definitions(-DARM_MATH_CM4 -DMCU_MK22FN512VLH7 -D__FPU_PRESENT=1 -DLITTLE_ENDIAN)
	set(SRAM_BANK_SIZE "64k")
	set(FLASH_SIZE "512k")
else()
	set(MCFLAGS "${MCFLAGS} -mfloat-abi=soft")
	add_definitions(-DCORTEX_USE_FPU=0)
	add_definitions(${MCFLAGS})
	add_definitions(-DARM_MATH_CM4 -DMCU_MK20DX128VLH7 -DLITTLE_ENDIAN)
	set(SRAM_BANK_SIZE "16k")
	set(FLASH_SIZE "256k")
endif()


set(LINKER_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/kinetis.ld)

###################################################
# Compiler Flags
###################################################
set(CXX_WARNINGS "-Waddress -Warray-bounds -Wc++11-compat -Wchar-subscripts ")
set(CXX_WARNINGS "${CXX_WARNINGS} -Wcomment -Wformat -Wmaybe-uninitialized -Wnonnull -Wparentheses -Wreturn-type ")
set(CXX_WARNINGS "${CXX_WARNINGS} -Wsequence-point -Wstrict-aliasing -Wstrict-overflow=1 -Wswitch -Wtrigraphs ") #-Wsign-compare
set(CXX_WARNINGS "${CXX_WARNINGS} -Wuninitialized -Wunknown-pragmas -Wunused-function -Wunused-label -Wunused-value -Wunused-variable -Wvolatile-register-var")

set(C_WARNINGS "-Wall")

# Flags for both C and C++ compilers
set(COMMON_FLAGS "${LTO} -ffunction-sections -ffreestanding -fdata-sections ${MCFLAGS}")
# Eliminate the need for a bunch of extra support code that may not work on the chip anyways
set(CMAKE_CXX_FLAGS "--std=gnu++11 -fno-rtti -fno-exceptions -fno-threadsafe-statics  ${CXX_WARNINGS} ${COMMON_FLAGS}")
# Use the new C11 standard
set(CMAKE_C_FLAGS "--std=gnu11 ${C_WARNINGS} ${COMMON_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "-T${LINKER_SCRIPT} ${LTO} ${OPTIMIZATION} -Wl,-Map=${TARGET}.map,--cref,-static,--gc-sections -nostartfiles --entry=__reset_vector ${MCFLAGS} ${CMAKE_EXE_LINKER_FLAGS}")
#set(CMAKE_EXE_LINKER_FLAGS "-T${LINKER_SCRIPT} ${LTO} ${OPTIMIZATION} -Wl,-Map=${TARGET}.map,--cref,-static,--gc-sections ${MCFLAGS} ${CMAKE_EXE_LINKER_FLAGS}")


include_directories(
	.
	src
	src/freescale
	FLogFS/inc
	${CMSIS_DIR}/Include
	${CMSIS_DIR}/Include/device/MK22F51212
	Device/ARM/ARMCM4/Include
	${USB_DIR}/Source/Device/source/driver
	${USB_DIR}/Source/Device/source/driver/kinetis
	${USB_DIR}/Source/Device/source/common
	${USB_DIR}/Source/Device/source/class
	${USB_DIR}/Source/Device/app/common
	${USB_DIR}/Source/Device/app/common/kinetis
	${PT_DIR}
	${SPIRAL_DIR}
)


########################################################
# Sources
########################################################

set(FREESCALE_SOURCES
	#src/freescale/kinetis_sysinit.c
	src/freescale/vectors.c
	src/freescale/system_MK22F51212.c
	src/usb_descriptor.c
	#src/audio_generator.c
	#src/audio_data.c
	#${USB_DIR}/Source/Device/app/common/kinetis/kinetis_sysinit.c
	${USB_DIR}/Source/Device/source/common/usb_class.c
	${USB_DIR}/Source/Device/source/driver/kinetis/usb_dci_kinetis.c
	${USB_DIR}/Source/Device/source/driver/usb_driver.c
	${USB_DIR}/Source/Device/source/common/usb_framework.c
	#${USB_DIR}/Source/Device/app/common/kinetis/wdt_kinetis.c
	#${USB_DIR}/Source/Device/app/common/kinetis/RealTimerCounter_kinetis.c
	${USB_DIR}/Source/Device/source/class/usb_composite.c
	${USB_DIR}/Source/Device/source/class/usb_audio.c
	${USB_DIR}/Source/Device/source/class/usb_hid.c
	${USB_DIR}/Source/Device/source/class/usb_cdc.c
)

set(APP_SOURCES
	src/main.cpp
	src/controller.cpp
	src/input_dsp.cpp
	src/filter_constants.cpp
	src/wavegen.cpp
	src/driver/platform.cpp
	src/driver/codec.cpp
	src/driver/usb.cpp
	src/driver/clocks.cpp
	src/driver/tpa6130a2.cpp
	src/driver/spi.cpp
	src/driver/mt29fxg01.cpp
	src/startup.s
)

set_property(SOURCE src/startup.s PROPERTY LANGUAGE C)

# Add required CMSIS-DSP sources as needed
set(DSP_SOURCES
	${DSP_SRC}/FilteringFunctions/arm_fir_decimate_init_f32.c
	${DSP_SRC}/FilteringFunctions/arm_fir_decimate_f32.c
	${DSP_SRC}/FilteringFunctions/arm_biquad_cascade_df1_f32.c
	${DSP_SRC}/FilteringFunctions/arm_biquad_cascade_df1_init_f32.c
	${DSP_SRC}/BasicMathFunctions/arm_shift_q31.c
	${DSP_SRC}/BasicMathFunctions/arm_mult_f32.c
	${DSP_SRC}/TransformFunctions/arm_rfft_fast_init_f32.c
	${DSP_SRC}/TransformFunctions/arm_rfft_fast_f32.c
	${DSP_SRC}/TransformFunctions/arm_cfft_radix4_q31.c
	${DSP_SRC}/TransformFunctions/arm_cfft_f32.c
	${DSP_SRC}/TransformFunctions/arm_cfft_radix8_f32.c
	${DSP_SRC}/TransformFunctions/arm_bitreversal.c
	${DSP_SRC}/TransformFunctions/arm_bitreversal2.S
	${DSP_SRC}/TransformFunctions/arm_bitreversal.c
	${DSP_SRC}/SupportFunctions/arm_copy_q31.c
	${DSP_SRC}/SupportFunctions/arm_copy_f32.c
	${DSP_SRC}/SupportFunctions/arm_q31_to_float.c
	${DSP_SRC}/SupportFunctions/arm_float_to_q31.c
	${DSP_SRC}/CommonTables/arm_common_tables.c
)
set_property(SOURCE ${DSP_SRC}/TransformFunctions/arm_bitreversal2.S PROPERTY LANGUAGE C)

##############################################################
# Executable and library targets
##############################################################

link_directories(${CMSIS_DIR}/Lib/G++)

add_library(dsp ${DSP_SOURCES})
SET_TARGET_PROPERTIES(dsp PROPERTIES COMPILE_FLAGS "-O2")


add_executable(${OUTFILE} ${APP_SOURCES}  ${FREESCALE_SOURCES})
set_target_properties(${OUTFILE} PROPERTIES COMPILE_FLAGS "${OPTIMIZATION}")
target_link_libraries(${OUTFILE} dsp)# libarm_cortexM4l_math.a)


# Warnings only for application code -- Optimizations only on ST/ARM code
# #set_target_properties(chibios PROPERTIES COMPILE_FLAGS "${OPTIMIZATION}")
# set_target_properties(imu_lib PROPERTIES COMPILE_FLAGS "${OPTIMIZATION} ${LTO}")
# set_target_properties(dsp_lib PROPERTIES COMPILE_FLAGS "-O2 ${LTO}")
# set_target_properties(${OUTFILE} PROPERTIES COMPILE_FLAGS "-W ${OPTIMIZATION} ${LTO}")
# #set_target_properties(imu_lib PROPERTIES COMPILE_FLAGS "-W ${OPTIMIZATION}")
# #set_target_properties(cmsis_dsp PROPERTIES COMPILE_FLAGS "-w -O2")


##############################################################
# Assembly output
##############################################################

add_custom_command(OUTPUT ${TARGET}.lst COMMAND ${CMAKE_OBJDUMP} -S --disassemble ${OUTFILE} > ${TARGET}.lst DEPENDS ${OUTFILE})
add_custom_target(assembly_output ALL DEPENDS ${TARGET}.lst)

##############################################################
# Configuration for OpenOCD
##############################################################

#set(OPENOCD openocd)
#set(OPENOCD_SCRIPTS board/stm32f4discovery.cfg)

#set(OPENOCD_ARGS "")
#foreach(s ${OPENOCD_SCRIPTS})
	#set(OPENOCD_ARGS "${OPENOCD_ARGS} -f ${s}")
#endforeach(s)

#add_custom_target(openocd COMMAND ${OPENOCD} ${OPENOCD_ARGS} -c "stm32f4x.cpu configure -rtos auto"
	#COMMENT "Launching OpenOCD..."
#)

#add_custom_target(openocd_no_os COMMAND ${OPENOCD} ${OPENOCD_ARGS}
	#COMMENT "Launching OpenOCD..."
#)

#if(${USE_FPU})
	#set(OPENOCD_RTOS "")
#else(${USE_FPU})
	#set(OPENOCD_RTOS "-c \"stm32f4x.cpu configure -rtos auto\"")
#endif(${USE_FPU})

#if(${DEBUG_USE_SVD})
	#set(GDB_LOAD_SVD
#"source ${CMAKE_SOURCE_DIR}/svd/gdb.py
#svd_load ${CMAKE_SOURCE_DIR}/svd/STM32F40x.svd")
#endif(${DEBUG_USE_SVD})

configure_file(${CMAKE_SOURCE_DIR}/kinetis.ld.in ${CMAKE_CURRENT_BINARY_DIR}/kinetis.ld)


if(${DEBUG_USE_SVD})
  set(USING_SVD_RESET_CMD 
"\tdwt reset counters
\tdwt cyccnt enable"
  )
  set(USING_SVD_INIT_CMD
"source ${CMAKE_SOURCE_DIR}/cmdebug/gdb.py
dwt configclk ${CLK}
dwt cyccnt reset
dwt cyccnt enable"
  )
endif()

configure_file(${CMAKE_SOURCE_DIR}/start.gdb.in ${CMAKE_CURRENT_BINARY_DIR}/start.gdb)
set(GDB_ARGS -x ${CMAKE_CURRENT_BINARY_DIR}/start.gdb)

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



