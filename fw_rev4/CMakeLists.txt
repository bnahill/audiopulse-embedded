set(TC_PREFIX arm-none-eabi-)
set(CMAKE_TOOLCHAIN_FILE toolchain.cmake)

set(TARGET ap)
set(OUTFILE ${TARGET}.elf)

project(${TARGET})
cmake_minimum_required(VERSION 2.8)

set(CFLAG_STD "-std=gnu11")
set(CFLAG_CPU "-mcpu=cortex-m7 -mthumb")
set(CFLAG_OPT "-Og")
set(CFLAG_OPT_DSP "-O3")
set(CFLAG_FLOAT "-mfpu=fpv5-d16 -mfloat-abi=hard")
set(CFLAG_DEBUG "-gdwarf-2 -g")
set(CPP_IN "-DUSE_HAL_DRIVER -DSTM32H753xx -DARM_MATH_CM7 -D__FPU_PRESENT")

add_definitions(${CPP_IN})

set(MIDDLEWARE_DIR ${CMAKE_SOURCE_DIR}/Middlewares)
set(FREERTOS_DIR ${MIDDLEWARE_DIR}/Third_Party/FreeRTOS)
set(USB_DRIVER_DIR ${MIDDLEWARE_DIR}/ST/STM32_USB_Device_Library)
set(DRIVERS_DIR ${CMAKE_SOURCE_DIR}/Drivers)
set(ST_DRIVERS_DIR ${DRIVERS_DIR}/STM32H7xx_HAL_Driver)
set(CMSIS_DIR ${DRIVERS_DIR}/CMSIS)
set(DSP_DIR ${CMSIS_DIR}/DSP)
set(ST_CORE_DIR ${CMAKE_SOURCE_DIR}/Core)

include_directories(
  ${ST_CORE_DIR}/Inc
  ${CMAKE_SOURCE_DIR}/USB_DEVICE/App
  ${CMAKE_SOURCE_DIR}/USB_DEVICE/Target
  ${ST_DRIVERS_DIR}/Inc
  ${ST_DRIVERS_DIR}/Inc/Legacy
  ${FREERTOS_DIR}/Source/include
  ${FREERTOS_DIR}/Source/CMSIS_RTOS_V2
  ${FREERTOS_DIR}/Source/portable/GCC/ARM_CM4F
  ${USB_DRIVER_DIR}/Core/Inc
  ${USB_DRIVER_DIR}/Class/AUDIO/Inc
  ${CMSIS_DIR}/Device/ST/STM32H7xx/Include
  ${CMSIS_DIR}/Include
  ${CMSIS_DIR}/RTOS2/Include
  ${CMSIS_DIR}/Core_A/Include
  ${DSP_DIR}/Include
)

set(SRC_ST_DRIVERS
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_qspi.c
  #${ST_DRIVERS_DIR}/Src/stm32h7xx_ll_utils.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_smbus.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_dma_ex.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_usart_ex.c
  #${ST_DRIVERS_DIR}/Src/stm32h7xx_ll_fmc.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_rcc_ex.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_irda.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_sai_ex.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_gpio.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_rtc.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_pwr_ex.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_hash_ex.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_hash.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_ll_spi.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_ll_adc.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_ll_mdma.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_ll_dma.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_ll_sdmmc.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_hrtim.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_cortex.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_mdma.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_pwr.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_ll_dma2d.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_i2c_ex.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_sram.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_rng.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_dfsdm.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_ll_lptim.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_jpeg.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_usart.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_pcd.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_ll_dac.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_ll_swpmi.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_wwdg.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_uart.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_opamp_ex.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_ltdc_ex.c
  #${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_timebase_rtc_alarm_template.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_hsem.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_lptim.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_ll_gpio.c
  #${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_timebase_rtc_wakeup_template.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_smartcard.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_smartcard_ex.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_tim.c
  #${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_timebase_tim_template.c
  #${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_msp_template.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_opamp.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_flash_ex.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_rcc.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_sd.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_ll_usb.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_ll_usart.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_exti.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_dma2d.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_sdram.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_mdios.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_spi_ex.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_ll_rtc.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_ll_delayblock.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_i2s_ex.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_ll_bdma.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_cryp.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_crc_ex.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_adc_ex.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_mmc_ex.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_cryp_ex.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_i2s.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_ltdc.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_crc.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_sai.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_eth.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_ll_exti.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_uart_ex.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_dsi.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_hcd.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_dac.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_ll_crc.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_mmc.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_eth_ex.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_fdcan.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_pcd_ex.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_ll_comp.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_rtc_ex.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_ll_pwr.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_ll_hrtim.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_swpmi.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_ramecc.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_comp.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_cec.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_ll_i2c.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_ll_lpuart.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_adc.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_ll_rcc.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_i2c.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_dac_ex.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_spdifrx.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_iwdg.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_dcmi.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_spi.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_flash.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_ll_tim.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_sd_ex.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_tim_ex.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_nor.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_ll_opamp.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_nand.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_hal_dma.c
  ${ST_DRIVERS_DIR}/Src/stm32h7xx_ll_rng.c
)

set(SRC_ST_CORE
  ${ST_CORE_DIR}/Src/stm32h7xx_hal_timebase_tim.c
  ${ST_CORE_DIR}/Src/stm32h7xx_hal_msp.c
  ${ST_CORE_DIR}/Src/system_stm32h7xx.c
  ${ST_CORE_DIR}/Src/stm32h7xx_it.c
  ${ST_CORE_DIR}/Src/freertos.c
  ${ST_CORE_DIR}/Src/main.c
  ${CMAKE_SOURCE_DIR}/USB_DEVICE/App/usbd_audio_if.c
  ${CMAKE_SOURCE_DIR}/USB_DEVICE/App/usb_device.c
  ${CMAKE_SOURCE_DIR}/USB_DEVICE/App/usbd_desc.c
  ${CMAKE_SOURCE_DIR}/USB_DEVICE/Target/usbd_conf.c
  #${CMSIS_DIR}/RTOS2/Template/cmsis_os1.c
  #${CMSIS_DIR}/RTOS2/Source/os_tick_gtim.c
  #${CMSIS_DIR}/RTOS2/Source/os_tick_ptim.c
  #${CMSIS_DIR}/RTOS2/Source/os_systick.c
  ${CMAKE_SOURCE_DIR}/startup_stm32h753xx.s
)

set_property(SOURCE ${CMAKE_SOURCE_DIR}/startup_stm32h753xx.s PROPERTY LANGUAGE C)

set(SRC_FREERTOS
  ${FREERTOS_DIR}/Source/event_groups.c
  ${FREERTOS_DIR}/Source/queue.c
  ${FREERTOS_DIR}/Source/CMSIS_RTOS_V2/cmsis_os2.c
  #${FREERTOS_DIR}/Source/CMSIS_RTOS_V2/cmsis_os1.c
  ${FREERTOS_DIR}/Source/timers.c
  ${FREERTOS_DIR}/Source/tasks.c
  ${FREERTOS_DIR}/Source/portable/GCC/ARM_CM4F/port.c
  ${FREERTOS_DIR}/Source/portable/MemMang/heap_4.c
  #${FREERTOS_DIR}/Source/portable/MemMang/heap_2.c
  #${FREERTOS_DIR}/Source/portable/MemMang/heap_3.c
  #${FREERTOS_DIR}/Source/portable/MemMang/heap_1.c
  #${FREERTOS_DIR}/Source/portable/MemMang/heap_5.c
  ${FREERTOS_DIR}/Source/stream_buffer.c
  ${FREERTOS_DIR}/Source/list.c
  ${FREERTOS_DIR}/Source/croutine.c
)

set(SRC_USB_DRIVER
  ${USB_DRIVER_DIR}/Core/Src/usbd_core.c
  ${USB_DRIVER_DIR}/Core/Src/usbd_ctlreq.c
  ${USB_DRIVER_DIR}/Core/Src/usbd_ioreq.c
  #${USB_DRIVER_DIR}/Core/Src/usbd_conf_template.c
  #${USB_DRIVER_DIR}/Core/Src/usbd_desc_template.c
  ${USB_DRIVER_DIR}/Class/AUDIO/Src/usbd_audio.c
  #${USB_DRIVER_DIR}/Class/AUDIO/Src/usbd_audio_if_template.c
)

set(SRC_DSP
  ${DSP_DIR}/Source/SupportFunctions/arm_copy_q31.c
  ${DSP_DIR}/Source/SupportFunctions/arm_q31_to_q7.c
  ${DSP_DIR}/Source/SupportFunctions/arm_copy_q7.c
  ${DSP_DIR}/Source/SupportFunctions/arm_q31_to_float.c
  ${DSP_DIR}/Source/SupportFunctions/arm_q7_to_q31.c
  ${DSP_DIR}/Source/SupportFunctions/arm_copy_q15.c
  ${DSP_DIR}/Source/SupportFunctions/arm_q31_to_q15.c
  ${DSP_DIR}/Source/SupportFunctions/arm_copy_f32.c
  ${DSP_DIR}/Source/SupportFunctions/arm_float_to_q31.c
  ${DSP_DIR}/Source/SupportFunctions/arm_float_to_q7.c
  ${DSP_DIR}/Source/SupportFunctions/arm_float_to_q15.c
  ${DSP_DIR}/Source/SupportFunctions/arm_q15_to_q31.c
  ${DSP_DIR}/Source/SupportFunctions/arm_fill_q15.c
  ${DSP_DIR}/Source/SupportFunctions/arm_fill_q7.c
  ${DSP_DIR}/Source/SupportFunctions/arm_q7_to_float.c
  ${DSP_DIR}/Source/SupportFunctions/arm_q7_to_q15.c
  ${DSP_DIR}/Source/SupportFunctions/arm_q15_to_float.c
  ${DSP_DIR}/Source/SupportFunctions/arm_fill_q31.c
  ${DSP_DIR}/Source/SupportFunctions/arm_q15_to_q7.c
  ${DSP_DIR}/Source/StatisticsFunctions/arm_power_q31.c
  ${DSP_DIR}/Source/StatisticsFunctions/arm_mean_f32.c
  ${DSP_DIR}/Source/StatisticsFunctions/arm_min_q15.c
  ${DSP_DIR}/Source/StatisticsFunctions/arm_rms_q15.c
  ${DSP_DIR}/Source/StatisticsFunctions/arm_rms_f32.c
  ${DSP_DIR}/Source/StatisticsFunctions/arm_mean_q31.c
  ${DSP_DIR}/Source/StatisticsFunctions/arm_var_f32.c
  ${DSP_DIR}/Source/StatisticsFunctions/arm_power_q15.c
  ${DSP_DIR}/Source/StatisticsFunctions/arm_std_q31.c
  ${DSP_DIR}/Source/StatisticsFunctions/arm_var_q31.c
  ${DSP_DIR}/Source/StatisticsFunctions/arm_min_q31.c
  ${DSP_DIR}/Source/StatisticsFunctions/arm_power_f32.c
  ${DSP_DIR}/Source/StatisticsFunctions/arm_max_q15.c
  ${DSP_DIR}/Source/StatisticsFunctions/arm_max_q7.c
  ${DSP_DIR}/Source/StatisticsFunctions/arm_std_q15.c
  ${DSP_DIR}/Source/StatisticsFunctions/arm_rms_q31.c
  ${DSP_DIR}/Source/StatisticsFunctions/arm_min_q7.c
  ${DSP_DIR}/Source/StatisticsFunctions/arm_max_q31.c
  ${DSP_DIR}/Source/StatisticsFunctions/arm_mean_q7.c
  ${DSP_DIR}/Source/StatisticsFunctions/arm_mean_q15.c
  ${DSP_DIR}/Source/StatisticsFunctions/arm_var_q15.c
  ${DSP_DIR}/Source/StatisticsFunctions/arm_std_f32.c
  ${DSP_DIR}/Source/StatisticsFunctions/arm_max_f32.c
  ${DSP_DIR}/Source/StatisticsFunctions/arm_power_q7.c
  ${DSP_DIR}/Source/StatisticsFunctions/arm_min_f32.c
  ${DSP_DIR}/Source/MatrixFunctions/arm_mat_cmplx_mult_f32.c
  ${DSP_DIR}/Source/MatrixFunctions/arm_mat_scale_q15.c
  ${DSP_DIR}/Source/MatrixFunctions/arm_mat_mult_q31.c
  ${DSP_DIR}/Source/MatrixFunctions/arm_mat_mult_f32.c
  ${DSP_DIR}/Source/MatrixFunctions/arm_mat_add_f32.c
  ${DSP_DIR}/Source/MatrixFunctions/arm_mat_cmplx_mult_q15.c
  ${DSP_DIR}/Source/MatrixFunctions/arm_mat_scale_q31.c
  ${DSP_DIR}/Source/MatrixFunctions/arm_mat_mult_fast_q15.c
  ${DSP_DIR}/Source/MatrixFunctions/arm_mat_sub_q15.c
  ${DSP_DIR}/Source/MatrixFunctions/arm_mat_trans_q15.c
  ${DSP_DIR}/Source/MatrixFunctions/arm_mat_trans_q31.c
  ${DSP_DIR}/Source/MatrixFunctions/arm_mat_add_q31.c
  ${DSP_DIR}/Source/MatrixFunctions/arm_mat_trans_f32.c
  ${DSP_DIR}/Source/MatrixFunctions/arm_mat_init_q15.c
  ${DSP_DIR}/Source/MatrixFunctions/arm_mat_init_f32.c
  ${DSP_DIR}/Source/MatrixFunctions/arm_mat_inverse_f64.c
  ${DSP_DIR}/Source/MatrixFunctions/arm_mat_inverse_f32.c
  ${DSP_DIR}/Source/MatrixFunctions/arm_mat_sub_f32.c
  ${DSP_DIR}/Source/MatrixFunctions/arm_mat_init_q31.c
  ${DSP_DIR}/Source/MatrixFunctions/arm_mat_mult_q15.c
  ${DSP_DIR}/Source/MatrixFunctions/arm_mat_add_q15.c
  ${DSP_DIR}/Source/MatrixFunctions/arm_mat_scale_f32.c
  ${DSP_DIR}/Source/MatrixFunctions/arm_mat_mult_fast_q31.c
  ${DSP_DIR}/Source/MatrixFunctions/arm_mat_cmplx_mult_q31.c
  ${DSP_DIR}/Source/MatrixFunctions/arm_mat_sub_q31.c
  ${DSP_DIR}/Source/ComplexMathFunctions/arm_cmplx_conj_f32.c
  ${DSP_DIR}/Source/ComplexMathFunctions/arm_cmplx_mag_squared_f32.c
  ${DSP_DIR}/Source/ComplexMathFunctions/arm_cmplx_mag_q31.c
  ${DSP_DIR}/Source/ComplexMathFunctions/arm_cmplx_dot_prod_f32.c
  ${DSP_DIR}/Source/ComplexMathFunctions/arm_cmplx_mag_squared_q31.c
  ${DSP_DIR}/Source/ComplexMathFunctions/arm_cmplx_conj_q15.c
  ${DSP_DIR}/Source/ComplexMathFunctions/arm_cmplx_mag_q15.c
  ${DSP_DIR}/Source/ComplexMathFunctions/arm_cmplx_mult_cmplx_f32.c
  ${DSP_DIR}/Source/ComplexMathFunctions/arm_cmplx_mult_real_q31.c
  ${DSP_DIR}/Source/ComplexMathFunctions/arm_cmplx_dot_prod_q31.c
  ${DSP_DIR}/Source/ComplexMathFunctions/arm_cmplx_mag_f32.c
  ${DSP_DIR}/Source/ComplexMathFunctions/arm_cmplx_mult_cmplx_q15.c
  ${DSP_DIR}/Source/ComplexMathFunctions/arm_cmplx_mult_real_q15.c
  ${DSP_DIR}/Source/ComplexMathFunctions/arm_cmplx_dot_prod_q15.c
  ${DSP_DIR}/Source/ComplexMathFunctions/arm_cmplx_mult_cmplx_q31.c
  ${DSP_DIR}/Source/ComplexMathFunctions/arm_cmplx_mult_real_f32.c
  ${DSP_DIR}/Source/ComplexMathFunctions/arm_cmplx_mag_squared_q15.c
  ${DSP_DIR}/Source/ComplexMathFunctions/arm_cmplx_conj_q31.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_offset_q15.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_dot_prod_q7.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_add_f32.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_add_q7.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_sub_f32.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_mult_q31.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_sub_q15.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_dot_prod_f32.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_offset_f32.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_abs_q15.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_mult_f32.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_scale_q15.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_add_q15.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_sub_q31.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_scale_q31.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_shift_q15.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_dot_prod_q15.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_abs_q7.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_offset_q7.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_mult_q7.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_shift_q7.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_abs_f32.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_abs_q31.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_negate_f32.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_add_q31.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_scale_q7.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_dot_prod_q31.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_offset_q31.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_negate_q15.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_sub_q7.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_shift_q31.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_scale_f32.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_mult_q15.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_negate_q31.c
  ${DSP_DIR}/Source/BasicMathFunctions/arm_negate_q7.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_sparse_q31.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_conv_partial_q31.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_biquad_cascade_df1_fast_q31.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_conv_fast_opt_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_conv_fast_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_lms_norm_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_correlate_opt_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_lattice_f32.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_biquad_cascade_df2T_init_f64.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_decimate_init_q31.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_conv_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_iir_lattice_f32.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_lattice_init_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_conv_q31.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_biquad_cascade_df2T_f32.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_biquad_cascade_df1_init_f32.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_correlate_opt_q7.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_decimate_init_f32.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_init_f32.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_conv_partial_q7.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_init_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_conv_partial_fast_q31.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_correlate_q31.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_lms_norm_init_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_interpolate_init_f32.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_biquad_cascade_stereo_df2T_init_f32.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_sparse_init_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_biquad_cascade_df1_q31.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_correlate_f32.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_decimate_fast_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_lms_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_conv_q7.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_sparse_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_biquad_cascade_df2T_f64.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_correlate_q7.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_conv_opt_q7.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_interpolate_init_q31.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_correlate_fast_opt_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_conv_partial_opt_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_fast_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_correlate_fast_q31.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_lms_q31.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_lms_init_q31.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_sparse_init_q7.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_iir_lattice_init_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_interpolate_q31.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_sparse_init_f32.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_lattice_init_q31.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_interpolate_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_sparse_q7.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_biquad_cascade_df1_f32.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_init_q31.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_biquad_cascade_df1_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_lattice_init_f32.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_lms_norm_f32.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_conv_partial_fast_opt_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_decimate_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_iir_lattice_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_conv_fast_q31.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_lattice_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_fast_q31.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_lms_norm_init_f32.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_iir_lattice_init_q31.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_lms_init_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_f32.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_decimate_init_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_biquad_cascade_df1_init_q31.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_interpolate_init_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_q31.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_biquad_cascade_df1_32x64_init_q31.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_interpolate_f32.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_iir_lattice_q31.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_biquad_cascade_df2T_init_f32.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_decimate_q31.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_lms_norm_q31.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_biquad_cascade_df1_init_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_correlate_fast_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_correlate_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_lms_f32.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_conv_f32.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_biquad_cascade_df1_fast_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_lattice_q31.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_decimate_fast_q31.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_sparse_init_q31.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_lms_init_f32.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_decimate_f32.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_init_q7.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_conv_partial_fast_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_conv_opt_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_q7.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_lms_norm_init_q31.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_conv_partial_q15.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_fir_sparse_f32.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_conv_partial_opt_q7.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_conv_partial_f32.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_biquad_cascade_df1_32x64_q31.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_biquad_cascade_stereo_df2T_f32.c
  ${DSP_DIR}/Source/FilteringFunctions/arm_iir_lattice_init_f32.c
  ${DSP_DIR}/Source/ControllerFunctions/arm_pid_reset_q31.c
  ${DSP_DIR}/Source/ControllerFunctions/arm_pid_init_q31.c
  ${DSP_DIR}/Source/ControllerFunctions/arm_pid_init_q15.c
  ${DSP_DIR}/Source/ControllerFunctions/arm_pid_reset_q15.c
  ${DSP_DIR}/Source/ControllerFunctions/arm_sin_cos_q31.c
  ${DSP_DIR}/Source/ControllerFunctions/arm_sin_cos_f32.c
  ${DSP_DIR}/Source/ControllerFunctions/arm_pid_init_f32.c
  ${DSP_DIR}/Source/ControllerFunctions/arm_pid_reset_f32.c
  ${DSP_DIR}/Source/TransformFunctions/arm_dct4_q31.c
  ${DSP_DIR}/Source/TransformFunctions/arm_cfft_radix8_f32.c
  ${DSP_DIR}/Source/TransformFunctions/arm_dct4_init_q15.c
  ${DSP_DIR}/Source/TransformFunctions/arm_cfft_radix2_q15.c
  ${DSP_DIR}/Source/TransformFunctions/arm_rfft_init_f32.c
  ${DSP_DIR}/Source/TransformFunctions/arm_cfft_radix4_q15.c
  ${DSP_DIR}/Source/TransformFunctions/arm_dct4_init_q31.c
  ${DSP_DIR}/Source/TransformFunctions/arm_bitreversal.c
  ${DSP_DIR}/Source/TransformFunctions/arm_cfft_radix2_f32.c
  ${DSP_DIR}/Source/TransformFunctions/arm_cfft_q31.c
  ${DSP_DIR}/Source/TransformFunctions/arm_cfft_radix2_q31.c
  ${DSP_DIR}/Source/TransformFunctions/arm_rfft_init_q15.c
  ${DSP_DIR}/Source/TransformFunctions/arm_rfft_q15.c
  ${DSP_DIR}/Source/TransformFunctions/arm_dct4_q15.c
  ${DSP_DIR}/Source/TransformFunctions/arm_cfft_radix4_init_q31.c
  ${DSP_DIR}/Source/TransformFunctions/arm_cfft_radix4_q31.c
  ${DSP_DIR}/Source/TransformFunctions/arm_rfft_fast_init_f32.c
  ${DSP_DIR}/Source/TransformFunctions/arm_cfft_radix2_init_f32.c
  ${DSP_DIR}/Source/TransformFunctions/arm_cfft_radix4_init_f32.c
  ${DSP_DIR}/Source/TransformFunctions/arm_cfft_radix2_init_q31.c
  ${DSP_DIR}/Source/TransformFunctions/arm_rfft_f32.c
  ${DSP_DIR}/Source/TransformFunctions/arm_cfft_f32.c
  ${DSP_DIR}/Source/TransformFunctions/arm_dct4_f32.c
  ${DSP_DIR}/Source/TransformFunctions/arm_cfft_radix2_init_q15.c
  ${DSP_DIR}/Source/TransformFunctions/arm_cfft_radix4_init_q15.c
  ${DSP_DIR}/Source/TransformFunctions/arm_cfft_radix4_f32.c
  ${DSP_DIR}/Source/TransformFunctions/arm_rfft_init_q31.c
  ${DSP_DIR}/Source/TransformFunctions/arm_dct4_init_f32.c
  ${DSP_DIR}/Source/TransformFunctions/arm_rfft_fast_f32.c
  ${DSP_DIR}/Source/TransformFunctions/arm_rfft_q31.c
  ${DSP_DIR}/Source/TransformFunctions/arm_cfft_q15.c
  ${DSP_DIR}/Source/FastMathFunctions/arm_sqrt_q15.c
  ${DSP_DIR}/Source/FastMathFunctions/arm_sin_q15.c
  ${DSP_DIR}/Source/FastMathFunctions/arm_sin_q31.c
  ${DSP_DIR}/Source/FastMathFunctions/arm_cos_f32.c
  ${DSP_DIR}/Source/FastMathFunctions/arm_sin_f32.c
  ${DSP_DIR}/Source/FastMathFunctions/arm_cos_q31.c
  ${DSP_DIR}/Source/FastMathFunctions/arm_sqrt_q31.c
  ${DSP_DIR}/Source/FastMathFunctions/arm_cos_q15.c
  ${DSP_DIR}/Source/CommonTables/arm_common_tables.c
  ${DSP_DIR}/Source/CommonTables/arm_const_structs.c
)

set(SRC_ALL
  # Exclude DSP so it can be built with its own flags
  #${SRC_DSP}
  ${SRC_USB_DRIVER}
  ${SRC_FREERTOS}
  ${SRC_ST_CORE}
  ${SRC_ST_DRIVERS}
)

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32H753VITx_FLASH.ld)

set(CMAKE_EXE_LINKER_FLAGS "-specs=nano.specs -T${LINKER_SCRIPT} -Wl,-Map=${TARGET}.map,--cref -Wl,--gc-sections  ${CMAKE_EXE_LINKER_FLAGS}")

set(CMAKE_C_FLAGS "${CFLAG_STD} ${CFLAG_CPU} ${CFLAG_FLOAT} ${CFLAG_DEBUG} -fdata-sections -ffunction-sections")

add_library(dsp STATIC ${SRC_DSP})
add_executable(${OUTFILE} ${SRC_ALL})

target_link_libraries(${OUTFILE} c m nosys dsp)

set_target_properties(dsp PROPERTIES
    COMPILE_FLAGS ${CFLAG_OPT_DSP})

set_target_properties(${OUTFILE} PROPERTIES
    COMPILE_FLAGS ${CFLAG_OPT})

add_custom_command(
        TARGET ${OUTFILE} POST_BUILD
        COMMAND arm-none-eabi-size --format=berkeley ${OUTFILE}
)
