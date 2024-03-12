/*
 * Copyright 2017-2020 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "FreeRTOS.h"

#include "Source/Library/Web/AWSIoTClient/mflash/mflash_file.h"
#include "mflash_drv.h"
#include "uEZErrors.h"

/* Magic numbers to check for presence of the structures below */
#define MFLASH_DIR_MAGIC_NO  (0xF17E07ABu)
#define MFLASH_META_MAGIC_NO (0xABECEDA8u)
#define MFLASH_FS_VERSION    (0x00010000u)
#define MFLASH_BLANK_PATTERN (0xFFu)

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
// linker symbols imported as described in https://www.keil.com/support/man/docs/armlink/armlink_pge1362065952432.htm
extern char Image$$mflash_fs$$Base[];
#define MFLASH_FS_START ((void *)Image$$mflash_fs$$Base)
#else
extern char __MFLASH_FS_START[];
#define MFLASH_FS_START ((void *)__MFLASH_FS_START)
#endif

/*
 * The table header and table record structures have to be aligned
 * with pages/sectors that are expected to be of 2**n size, hence there is some padding
 */
typedef struct
{
    uint32_t file_offset;
    uint32_t alloc_size;
    uint8_t path[MFLASH_MAX_PATH_LEN];
} mflash_dir_record_t;

typedef struct
{
    uint32_t magic_no;
    uint32_t version;
    uint32_t page_size;
    uint32_t sector_size;
    uint32_t file_count;
    uint32_t total_size;
    uint8_t padding[sizeof(mflash_dir_record_t) - 6 * sizeof(uint32_t)];
} mflash_fs_header_t;

typedef struct
{
    mflash_fs_header_t header;
    mflash_dir_record_t records[];
} mflash_fs_t;

/* Metadata prepended to the file itself to identify valid (already written) file and keep actual length of the file */
typedef struct
{
    uint32_t file_size;
    uint32_t magic_no;
} mflash_file_meta_t;

/* Pointer to the filesystem */
static mflash_fs_t *g_mflash_fs = NULL;

/* API - True if mflash is already initialized */
bool mflash_is_initialized(void)
{
    return (g_mflash_fs != NULL);
}

/* Store path string to directory record structure */
static bool dir_path_store(mflash_dir_record_t *dr, char *path)
{
#if 0
    assert(dr);
    assert(path);
#endif

    for (int i = 0; i < MFLASH_MAX_PATH_LEN; i++)
    {
        dr->path[i] = (uint8_t)(*path);

        /* End of string, exit the loop */
        if (*path == '\0')
        {
            break;
        }

        path++;
    }

    /* Check whether the whole given path string was processed */
    if (*path != '\0')
    {
        return false;
    }

    return true;
}

/* Match path string against directory record */
static bool dir_path_match(mflash_dir_record_t *dr, char *path)
{
#if 0
    assert(dr);
    assert(path);
#endif

    for (int i = 0; i < MFLASH_MAX_PATH_LEN; i++)
    {
        if (dr->path[i] != (uint8_t)(*path))
        {
            return false;
        }

        /* End of string, there is match */
        if (*path == '\0')
        {
            return true;
        }

        path++;
    }

    /* Check whether the whole given path string was processed */
    if (*path != '\0')
        return false;

    return true;
}

/* Buffer allocation wrapper */
static void *mflash_page_buf_get(void)
{
    void *page_buf;

    page_buf = pvPortMalloc(MFLASH_PAGE_SIZE);

    return page_buf;
}

/* Buffer allocation wrapper */
static void mflash_page_buf_release(void *page_buf)
{
    vPortFree(page_buf);
}

/* Low level abstraction - erase sector of the filesystem */
static T_uezError mflash_fs_sector_erase(mflash_fs_t *fs, uint32_t sector_offset)
{
    uint32_t phys_addr;

    /* Translate filesystem offset to physical address in FLASH */
    phys_addr = mflash_drv_log2phys((uint8_t *)fs + sector_offset, MFLASH_SECTOR_SIZE);
    if (phys_addr == MFLASH_INVALID_ADDRESS)
    {
        return UEZ_ERROR_FAIL;
    }

    return (T_uezError) mflash_drv_sector_erase(phys_addr);
}

/* Low level abstraction - program page of the filesystem */
static T_uezError mflash_fs_page_program(mflash_fs_t *fs, uint32_t page_offset, uint32_t *data)
{
    uint32_t phys_addr;

    /* Translate filesystem offset to physical address in FLASH */
    phys_addr = mflash_drv_log2phys((uint8_t *)fs + page_offset, MFLASH_PAGE_SIZE);
    if (phys_addr == MFLASH_INVALID_ADDRESS)
    {
        return UEZ_ERROR_FAIL;
    }

    return (T_uezError) mflash_drv_page_program(phys_addr, data);
}

/* Low level abstraction - get pointer to filesystem location specified by offset */
static inline void *mflash_fs_get_ptr(mflash_fs_t *fs, uint32_t offset)
{
    return (void *)((uint8_t *)fs + offset);
}

/*
 * Check whether give area of FLASH is readable by direct pointer access.
 * This is necessary on plaforms featuring page checksums as access to page containing invalid data may result in a
 * hardfault.
 */
static T_uezError mflash_readable_check(void *ptr, uint32_t size)
{
#if defined(MFLASH_PAGE_INTEGRITY_CHECKS) && MFLASH_PAGE_INTEGRITY_CHECKS
    T_uezError status;

    uintptr_t start_addr = (uintptr_t)ptr - (uintptr_t)ptr % MFLASH_PAGE_SIZE;
    uintptr_t end_addr   = (uintptr_t)ptr + size;

    for (uintptr_t check_addr = start_addr; check_addr < end_addr; check_addr += MFLASH_PAGE_SIZE)
    {
        status = mflash_drv_is_readable(check_addr);
        if (status != UEZ_ERROR_NONE)
        {
            return status;
        }
    }

    return UEZ_ERROR_NONE;
#else
    return UEZ_ERROR_NONE;
#endif
}

/* Check for filesystem presence and validity */
static T_uezError mflash_fs_check(mflash_fs_t *fs)
{
    T_uezError status = UEZ_ERROR_NONE;

    /* Check params */
    if (fs == NULL)
    {
        return UEZ_ERROR_ILLEGAL_ARGUMENT;
    }

    /* Check readability before accessing filesystem structure by pointer */
    status = mflash_readable_check(fs, sizeof(mflash_fs_header_t));
    if (status != UEZ_ERROR_NONE)
    {
        return status;
    }

    /* Check magic */
    if (fs->header.magic_no != MFLASH_DIR_MAGIC_NO)
    {
        return UEZ_ERROR_FAIL;
    }

    /* Check major version */
    if ((fs->header.version & 0xFFFF0000u) != (MFLASH_FS_VERSION & 0xFFFF0000u))
    {
        return UEZ_ERROR_FAIL;
    }

    /* Check FLASH memory characteristics */
    if (fs->header.page_size != MFLASH_PAGE_SIZE || fs->header.sector_size != MFLASH_SECTOR_SIZE)
    {
        return UEZ_ERROR_FAIL;
    }

    /* Check readability of the whole directory */
    status =
        mflash_readable_check(fs, sizeof(mflash_fs_header_t) + fs->header.file_count * sizeof(mflash_dir_record_t));

    return status;
}

/* Check for presence of a file data */
static T_uezError mflash_file_check(mflash_fs_t *fs, mflash_dir_record_t *dr)
{
    T_uezError status = UEZ_ERROR_NONE;
    mflash_file_meta_t *meta;

    /* Check params */
    if (fs == NULL)
    {
        return UEZ_ERROR_ILLEGAL_ARGUMENT;
    }

    if (dr == NULL)
    {
        return UEZ_ERROR_ILLEGAL_ARGUMENT;
    }

    /* Get pointer to file meta structure */
    meta = mflash_fs_get_ptr(fs, dr->file_offset);

    /* Check readability before accessing file meta structure */
    status = mflash_readable_check(meta, sizeof(mflash_file_meta_t));
    if (status != UEZ_ERROR_NONE)
    {
        return status;
    }

    /* Check magic signature */
    if (meta->magic_no != MFLASH_META_MAGIC_NO)
    {
        return UEZ_ERROR_FAIL;
    }

    /* Check wheter actual file size in meta fits the pre-allocated area */
    if (meta->file_size + sizeof(mflash_file_meta_t) > dr->alloc_size)
    {
        return UEZ_ERROR_FAIL;
    }

    /* Check readability of the whole file */
    status = mflash_readable_check(meta, sizeof(mflash_file_meta_t) + meta->file_size);

    return UEZ_ERROR_NONE;
}

/* Searches for directory record with given path and retrieves a copy of it */
static T_uezError mflash_dir_lookup(mflash_fs_t *fs, char *path, mflash_dir_record_t *dr_ptr)
{
    uint32_t file_count     = fs->header.file_count;
    mflash_dir_record_t *dr = fs->records;

    for (uint32_t i = 0; i < file_count; i++)
    {
        if (dir_path_match(dr, path))
        {
            if (NULL != dr_ptr)
            {
                *dr_ptr = *dr;
            }
            return UEZ_ERROR_NONE;
        }
        dr++;
    }

    return UEZ_ERROR_FAIL;
}

/* Create filesystem structure in FLASH according to given directory template */
static T_uezError mflash_format_internal(mflash_fs_t *fs,
                                       void *page_buf,
                                       uint32_t fs_size_limit,
                                       const mflash_file_t *dir_template)
{
    T_uezError status = UEZ_ERROR_NONE;

    uint32_t file_count;
    uint32_t total_sectors;

    uint32_t dir_size;
    uint32_t dir_sectors;

    uint32_t file_offset;
    uint32_t dir_offset;

    mflash_fs_header_t *fsh;

#if 0
    /* The directory records shall be aligned to page size */
    assert((MFLASH_PAGE_SIZE % sizeof(mflash_dir_record_t)) == 0u);
#endif

    /* Count the files and calculate number of FLASH sectors to be occupied by the filesystem */
    file_count    = 0;
    total_sectors = 0;
    for (const mflash_file_t *dt = dir_template; (NULL != dt->path) && ('\0' != dt->path[0]) && (0 != dt->max_size);
         dt++)
    {
        /* Calculate number of sectors to be occupied by the file */
        uint32_t file_sectors = (dt->max_size + MFLASH_SECTOR_SIZE - 1) / MFLASH_SECTOR_SIZE;
        total_sectors += file_sectors;
        file_count++;
    }

    dir_size    = file_count * sizeof(mflash_dir_record_t) + sizeof(mflash_fs_header_t);
    dir_sectors = (dir_size + MFLASH_SECTOR_SIZE - 1) / MFLASH_SECTOR_SIZE;
    total_sectors += dir_sectors;

    /* Check whether the filestytem fits into the given FLASH area */
    if ((0 != fs_size_limit) && (fs_size_limit < total_sectors * MFLASH_SECTOR_SIZE))
    {
        return UEZ_ERROR_OUT_OF_RANGE;
    }

    /* Erase the whole FLASH area to be occupied by the filesystem */
    for (uint32_t i = 0; i < total_sectors; i++)
    {
        status = mflash_fs_sector_erase(fs, i * MFLASH_SECTOR_SIZE);
        if (status != UEZ_ERROR_NONE)
        {
            return status;
        }
    }

    /* Clear the page buffer and set inital values for offsets */
    memset(page_buf, MFLASH_BLANK_PATTERN, MFLASH_PAGE_SIZE);
    dir_offset  = file_count * sizeof(mflash_dir_record_t) + sizeof(mflash_fs_header_t);
    file_offset = total_sectors * MFLASH_SECTOR_SIZE;

#if 1
    /* Create directory entries in reverse order so that programming of the page containing the dir header is the last
     * step */
    for (int fi = file_count; 0 != fi--;)
    {
#if 0
        /* Check for enough space for the directory record */
        assert(dir_offset >= sizeof(mflash_dir_record_t));
#endif
        dir_offset -= sizeof(mflash_dir_record_t);

        mflash_dir_record_t *dr = (mflash_dir_record_t *)((uint8_t *)page_buf + (dir_offset % MFLASH_PAGE_SIZE));
        const mflash_file_t *dt = &dir_template[fi];

        /* Calculate number of sectors to be occupied by the file */
        uint32_t file_sectors = (dt->max_size + MFLASH_SECTOR_SIZE - 1) / MFLASH_SECTOR_SIZE;

        /* Fill in directory record */
        dr->alloc_size  = file_sectors * MFLASH_SECTOR_SIZE;
        dr->file_offset = (file_offset -= dr->alloc_size);
        dir_path_store(dr, dt->path);

        if (dir_offset % MFLASH_PAGE_SIZE == 0u)
        {
            /* We reached the beginning of a page, program it and start over */
            status = mflash_fs_page_program(fs, dir_offset, page_buf);
            if (status != UEZ_ERROR_NONE)
            {
                return status;
            }

            /* Clear the page buffer */
            memset(page_buf, MFLASH_BLANK_PATTERN, MFLASH_PAGE_SIZE);
        }
    }
#if 0
    /* There should be space left exactly for the filesystem header */
    assert(dir_offset == sizeof(mflash_fs_header_t));
#endif
    /* Create filesystem header at the very beginning of the first page */
    fsh              = (mflash_fs_header_t *)page_buf;
    fsh->magic_no    = MFLASH_DIR_MAGIC_NO;
    fsh->version     = MFLASH_FS_VERSION;
    fsh->page_size   = MFLASH_PAGE_SIZE;
    fsh->sector_size = MFLASH_SECTOR_SIZE;
    fsh->total_size  = total_sectors * MFLASH_SECTOR_SIZE;
    fsh->file_count  = file_count;

    /* Programming of the first page puts header into place marking the filesystem as valid */
    status = mflash_fs_page_program(fs, 0, page_buf);
#else // TODO fix ordering to not be in reverse order!!
    file_offset = MFLASH_SECTOR_SIZE;

    /* Create filesystem header at the very beginning of the first page */
    fsh              = (mflash_fs_header_t *)page_buf;
    fsh->magic_no    = MFLASH_DIR_MAGIC_NO;
    fsh->version     = MFLASH_FS_VERSION;
    fsh->page_size   = MFLASH_PAGE_SIZE;
    fsh->sector_size = MFLASH_SECTOR_SIZE;
    fsh->total_size  = total_sectors * MFLASH_SECTOR_SIZE;
    fsh->file_count  = file_count;

    /* Programming of the first page puts header into place marking the filesystem as valid */
    status = mflash_fs_page_program(fs, 0, page_buf);

    /* Create directory entries in normal order so that programming of the page containing the dir header is the last
     * step */
    for (int fi = 0; fi < file_count; fi++)
    {
        dir_offset += sizeof(mflash_dir_record_t);

        mflash_dir_record_t *dr = (mflash_dir_record_t *)((uint8_t *)page_buf + (dir_offset % MFLASH_PAGE_SIZE));
        const mflash_file_t *dt = &dir_template[fi];

        /* Calculate number of sectors to be occupied by the file */
        uint32_t file_sectors = (dt->max_size + MFLASH_SECTOR_SIZE - 1) / MFLASH_SECTOR_SIZE;

        /* Fill in directory record */
        dr->alloc_size  = file_sectors * MFLASH_SECTOR_SIZE;
        dr->file_offset = (file_offset += dr->alloc_size);
        dir_path_store(dr, dt->path);

        if (dir_offset % MFLASH_PAGE_SIZE == 0u)
        {
            /* We reached the beginning of a page, program it and start over */
            status = mflash_fs_page_program(fs, dir_offset, page_buf);
            if (status != UEZ_ERROR_NONE)
            {
                return status;
            }

            /* Clear the page buffer */
            memset(page_buf, MFLASH_BLANK_PATTERN, MFLASH_PAGE_SIZE);
        }
    }
#endif

    return status;
}

/* Create filesystem structure in FLASH according to given directory template */
static T_uezError mflash_format(mflash_fs_t *fs, uint32_t fs_size_limit, const mflash_file_t *dir_template)
{
    T_uezError status = UEZ_ERROR_NONE;
    void *page_buf;

    /* Check parameters */
    if (dir_template == NULL)
    {
        return UEZ_ERROR_ILLEGAL_ARGUMENT;
    }

    /* Get page buffer for FLASH writes */
    page_buf = mflash_page_buf_get();
    if (page_buf == NULL)
    {
        return UEZ_ERROR_FAIL;
    }

    /* Actual formatting of the filesystem */
    status = mflash_format_internal(fs, page_buf, fs_size_limit, dir_template);

    /* Release page buffer */
    mflash_page_buf_release(page_buf);

    return status;
}

/* Match dir against given template. Checks whether all files defined in the template are pre-allocsated in the fs
 * directory */
static T_uezError mflash_template_match(mflash_fs_t *fs, const mflash_file_t *dir_template)
{
    T_uezError status = UEZ_ERROR_NONE;

    for (const mflash_file_t *dt = dir_template; (NULL != dt->path) && ('\0' != dt->path[0]) && (0 != dt->max_size);
         dt++)
    {
        mflash_dir_record_t dr;

        /* Lookup directory record */
        status = mflash_dir_lookup(fs, dt->path, &dr);
        if (status != UEZ_ERROR_NONE)
        {
            return status;
        }

        /* Check whether pre-allocated size is sufficient */
        if (dr.alloc_size < dt->max_size)
        {
            return UEZ_ERROR_FAIL;
        }
    }

    return UEZ_ERROR_NONE;
}

/* Initialize mflash filesystem */
static T_uezError mflash_fs_init(mflash_fs_t *fs, uint32_t fs_size_limit, const mflash_file_t *dir_template)
{
    T_uezError status = UEZ_ERROR_NONE;

    /* Check whether there is a filesystem header and directory already in place */
    status = mflash_fs_check(fs);

    /* Filesystem is valid, check whether its directory provides records for all required files */
    if (status == UEZ_ERROR_NONE)
    {
        status = mflash_template_match(fs, dir_template);
    }

    /* The filesystem not present or does not fit the template, create a new one */
    if (status == UEZ_ERROR_FAIL) /* Error codes other then 'Fail' are not captured here but rather intentinally passed to
                                   the caller */
    {
        status = mflash_format(fs, fs_size_limit, dir_template); /* Format the filestem */
    }

    if (status == UEZ_ERROR_NONE)
    {
        g_mflash_fs = fs; /* If all went ok, keep pointer to the filesytem */
    }

    return status;
}

/* API - Initialize mflash driver and filesystem at default address specified by linker symbol */
T_uezError mflash_init(const mflash_file_t *dir_template, bool init_drv)
{
    T_uezError status = UEZ_ERROR_NONE;
    mflash_fs_t *fs;

    /* Initialize the driver */
    if (init_drv)
    {
        status = (T_uezError) mflash_drv_init();
        if (status == UEZ_ERROR_FAIL)
        {
            return status;
        }
    }
#ifdef MFLASH_FILE_BASEADDR
    /* Convert physical address in FLASH to memory pointer */
    fs = (mflash_fs_t *)mflash_drv_phys2log(MFLASH_FILE_BASEADDR, 0);
#else
    /* Otherwise take address from linker file */
    fs = (mflash_fs_t *)MFLASH_FS_START;
#endif

    if (fs == NULL)
    {
        return UEZ_ERROR_FAIL;
    }

    return mflash_fs_init(fs, 0, dir_template);
}

/* Save file */
static T_uezError mflash_file_save_internal(
    mflash_fs_t *fs, void *page_buf, mflash_dir_record_t *dr, uint8_t *data, uint32_t size)
{
    T_uezError status = UEZ_ERROR_NONE;

    /* Check whether the data + meta fits into the pre-allocated file area */
    if (size + sizeof(mflash_file_meta_t) > dr->alloc_size)
    {
        return UEZ_ERROR_OUT_OF_RANGE;
    }

    /* Erase the whole file area sector by sector */
    for (uint32_t sector_offset = 0; sector_offset < dr->alloc_size; sector_offset += MFLASH_SECTOR_SIZE)
    {
        /* Erase the sector */
        status = mflash_fs_sector_erase(fs, dr->file_offset + sector_offset);
        if (status != UEZ_ERROR_NONE)
        {
            return status;
        }
    }

    /* Program the file data page by page, skipping the first page containing meta that is going to be programmed in the
     * last step */
    for (int data_offset = MFLASH_PAGE_SIZE - sizeof(mflash_file_meta_t); data_offset < size;
         data_offset += MFLASH_PAGE_SIZE)
    {
        /* Pointer and size of the data portion to be programmed */
        void *copy_ptr     = data + data_offset;
        uint32_t copy_size = size - data_offset;
        if (copy_size > MFLASH_PAGE_SIZE)
        {
            copy_size = MFLASH_PAGE_SIZE;
        }

        memset(page_buf, MFLASH_BLANK_PATTERN, MFLASH_PAGE_SIZE);
        memcpy(page_buf, copy_ptr, copy_size);

        /* Data offset is off by sizeof(mflash_file_meta_t) as this structure occupies the very beginning of the first
         * page */
        status = mflash_fs_page_program(fs, dr->file_offset + data_offset + sizeof(mflash_file_meta_t), page_buf);
        if (status != UEZ_ERROR_NONE)
        {
            return status;
        }
    }

    /* Prepare the missing portion of data to be programme to the first page */
    uint32_t copy_size = size;
    if (copy_size > MFLASH_PAGE_SIZE - sizeof(mflash_file_meta_t))
    {
        copy_size = MFLASH_PAGE_SIZE - sizeof(mflash_file_meta_t);
    }

    memset(page_buf, MFLASH_BLANK_PATTERN, MFLASH_PAGE_SIZE);
    memcpy((uint8_t *)page_buf + sizeof(mflash_file_meta_t), data, copy_size);

    /* Set file metadata */
    mflash_file_meta_t *meta = (mflash_file_meta_t *)page_buf;
    meta->file_size          = size;
    meta->magic_no           = MFLASH_META_MAGIC_NO;

    /* Program the first page putting the metadata in place which marks the file as valid */
    status = mflash_fs_page_program(fs, dr->file_offset, page_buf);

    return status;
}

/* API, save data to file with given path */
T_uezError mflash_file_save(char *path, uint8_t *data, uint32_t size)
{
    T_uezError status = UEZ_ERROR_NONE;
    mflash_dir_record_t dr;
    mflash_fs_t *fs = g_mflash_fs;
    void *page_buf;

    if (path == NULL)
    {
        return UEZ_ERROR_ILLEGAL_ARGUMENT;
    }

    if (data == NULL && size != 0)
    {
        return UEZ_ERROR_ILLEGAL_ARGUMENT;
    }

    /* Lookup directory record */
    status = mflash_dir_lookup(fs, path, &dr);
    if (status != UEZ_ERROR_NONE)
    {
        return status;
    }

    /* Get page buffer for FLASH writes */
    page_buf = mflash_page_buf_get();
    if (page_buf == NULL)
    {
        return UEZ_ERROR_FAIL;
    }

    /* Save the file */
    status = mflash_file_save_internal(fs, page_buf, &dr, data, size);

    /* Release page buffer */
    mflash_page_buf_release(page_buf);

    return status;
}

/* Get direct pointer to file data */
static T_uezError mflash_file_mmap_internal(mflash_fs_t *fs, mflash_dir_record_t *dr, uint8_t **pdata, uint32_t *psize)
{
    T_uezError status = UEZ_ERROR_NONE;
    mflash_file_meta_t *meta;

    status = mflash_file_check(fs, dr);
    if (status != UEZ_ERROR_NONE)
    {
        return status;
    }

    meta = mflash_fs_get_ptr(fs, dr->file_offset);

    *pdata = (uint8_t *)meta + sizeof(*meta);
    *psize = meta->file_size;

    return UEZ_ERROR_NONE;
}

/* API, get direct pointer to data of file with given path */
T_uezError mflash_file_mmap(char *path, uint8_t **pdata, uint32_t *psize)
{
    T_uezError status = UEZ_ERROR_NONE;
    mflash_dir_record_t dr;
    mflash_fs_t *fs = g_mflash_fs;

    if (path == NULL)
    {
        return UEZ_ERROR_ILLEGAL_ARGUMENT;
    }

    if (pdata == NULL || psize == NULL)
    {
        return UEZ_ERROR_ILLEGAL_ARGUMENT;
    }

    /* Lookup directory record */
    status = mflash_dir_lookup(fs, path, &dr);
    if (status != UEZ_ERROR_NONE)
    {
        return status;
    }

    status = mflash_file_mmap_internal(fs, &dr, pdata, psize);

    return status;
}
