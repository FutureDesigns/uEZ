// Copyright (c) 2009 Rowley Associates Limited.
//
// This file may be distributed under the terms of the License Agreement
// provided with this software.
//
// THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

#ifndef liblpc1000_h
#define liblpc1000_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <libmem.h>

#define IAP_CMD_PREPARE_SECTORS_FOR_WRITE_OPERATION 50
#define IAP_CMD_COPY_RAM_TO_FLASH 51
#define IAP_CMD_ERASE_SECTORS 52
#define IAP_CMD_BLANK_CHECK_SECTORS 53
#define IAP_CMD_READ_PART_ID 54
#define IAP_CMD_READ_BOOT_CODE_VERSION 55
#define IAP_CMD_COMPARE 56

/*! \brief Call an LPC1000 IAP command.
 *  \param cmd The IAP command to call.
 *  \param p0 IAP function parameter 0.
 *  \param p1 IAP function parameter 1.
 *  \param p2 IAP function parameter 2.
 *  \param p3 IAP function parameter 3.
 *  \param r0 Pointer to location to store result0 or NULL if not required.
 *  \param r1 Pointer to location to store result1 or NULL if not required.
 *  \param r2 Pointer to location to store result2 or NULL if not required.
 *  \param r3 Pointer to location to store result3 or NULL if not required.
 *  \return Result0
 */
unsigned long liblpc1000_iap_command(unsigned long cmd, unsigned long p0, unsigned long p1, unsigned long p2, unsigned long p3, unsigned long *r0, unsigned long *r1, unsigned long *r2, unsigned long *r3);

/*  \brief Get the internal FLASH geometry information.
 *  \return A pointer to the internal FLASH geometry.
 */
const libmem_geometry_t *liblpc1000_get_internal_flash_geometry(void);

/*  \brief Get the size of the internal FLASH memory.
 *  \return The FLASH memory size in bytes.
 */
size_t liblpc1000_get_internal_flash_size(void);

/* \brief Get the size of the internal SRAM memory.
 * \return The SRAM memory size in bytes.
 */
size_t liblpc1000_get_local_sram_size(void);

/*! \brief Return the current CPU clock frequency.
    \param fosc The oscillator frequency in Hz.
    \return The CPU clock (CCLK) frequency in Hz.
 */
unsigned long liblpc1000_get_cclk(unsigned long fosc);

/*! \brief Configure the PLL so that CCLK is running at a specific frequency.
    \param fosc The oscillator frequency.
    \param cclk The desired CCLK frequency.
    \return The achieved CCLK frequency or zero if the configuration is not possible.
 */
unsigned long liblpc1000_configure_pll(unsigned long fosc, unsigned long cclk);

/*! \brief Register a driver for the LPC1000 internal FLASH with the libmem library.
 *  \param h Pointer to libmem driver handle.
 *  \param cclk_frequency The the freqency of CCLK in Hz.
 *  \return The LIBMEM status result.
 */
int32_t liblpc1000_register_libmem_driver(libmem_driver_handle_t *h, uint32_t cclk_frequency);

/*! \brief Register a driver for the LPC1000 internal FLASH with the libmem library.
 *  You should use this version of the driver register function when memory is at a premium,
 *  it allows you to specify the FLASH size and geometry and therefore saves the overhead
 *  of the FLASH detection code. It also allows you to specify the location and size of
 *  the write buffer to use when writing to FLASH.
 *  \param h Pointer to libmem driver handle.
 *  \param size The size of the FLASH.
 *  \param geometry The FLASH geomeatry
 *  \param write_buffer Pointer to the write buffer.
 *  \param write_buffer_size The size in bytes of the write buffer pointed to by \a write_buffer. This can be 256, 512, 1024 or 4096 bytes.
 *  \param cclk_frequency The the freqency of CCLK in Hz.
 *  \return The LIBMEM status result.
 */
int32_t liblpc1000_register_libmem_driver_ex(libmem_driver_handle_t *h, size_t size, const libmem_geometry_t *geometry, uint8_t *write_buffer, uint32_t write_buffer_size, uint32_t cclk_frequency);

#ifdef __cplusplus
}
#endif

#endif

