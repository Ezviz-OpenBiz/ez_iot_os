/*
 * File      : fal_cfg.h
 * This file is part of FAL (Flash Abstraction Layer) package
 * COPYRIGHT (C) 2006 - 2018, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-17     armink       the first version
 */

#ifndef _FAL_CFG_H_
#define _FAL_CFG_H_

//#include <rtconfig.h>
//#include <board.h>

//#define FAL_DEBUG 1
#define FAL_PART_HAS_TABLE_CFG
#define NOR_FLASH_DEV_NAME "norflash0"

//#define FAL_PART_TABLE_FLASH_DEV_NAME NOR_FLASH_DEV_NAME
//#define FAL_PART_TABLE_END_OFFSET      65536

/* ===================== Flash device Configuration ========================= */
extern const struct fal_flash_dev nor_flash0;

/* flash device table */
#define FAL_FLASH_DEV_TABLE \
    {                       \
        &nor_flash0,        \
    }
/* ====================== Partition Configuration ========================== */
#ifdef FAL_PART_HAS_TABLE_CFG
/* partition table 
    采用8266 的日志分区作为存储kv区，大小0x14000
*/
#define FAL_PART_TABLE                                                                    \
    {                                                                                     \
        {FAL_PART_MAGIC_WORD, "ez_kvdb", NOR_FLASH_DEV_NAME, 0x100000, 32 * 1024, 0},     \
        {FAL_PART_MAGIC_WORD, "sdk_kvdb", NOR_FLASH_DEV_NAME, 0x108000, 8 * 1024, 0},     \
        {FAL_PART_MAGIC_WORD, "ez_tsdb", NOR_FLASH_DEV_NAME, 0x10a000, 40 * 1024, 0},     \
    }
#endif /* FAL_PART_HAS_TABLE_CFG */

#endif /* _FAL_CFG_H_ */
