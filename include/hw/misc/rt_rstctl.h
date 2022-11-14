/*
 * Copyright (c) 2022, NXP
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#ifndef RT_RSTCTL_H
#define RT_RSTCTL_H

#include "hw/sysbus.h"
#include "qom/object.h"

#define TYPE_RT_RSTCTL0 "rt_rstctl0"
OBJECT_DECLARE_SIMPLE_TYPE(RTRSTCTL0State, RT_RSTCTL0)

struct RTRSTCTL0State {
    /*< private >*/
    SysBusDevice parent_obj;

    /*< public >*/
    MemoryRegion  iomem;

    uint32_t SYSRSTSTAT;                        /**< System Reset Status Register, offset: 0x0 */
//    uint8_t RESERVED_0[12];
    uint32_t PRSTCTL0;                          /**< Peripheral Reset Control Register 0, offset: 0x10 */
    uint32_t PRSTCTL1;                          /**< Peripheral Reset Control Register 1, offset: 0x14 */
    uint32_t PRSTCTL2;                          /**< Peripheral Reset Control Register 2, offset: 0x18 */
//    uint8_t RESERVED_1[36];
    uint32_t PRSTCTL0_SET;                      /**< Peripheral Reset Control Register 0 SET, offset: 0x40 */
    uint32_t PRSTCTL1_SET;                      /**< Peripheral Reset Control Register 1 SET, offset: 0x44 */
    uint32_t PRSTCTL2_SET;                      /**< Peripheral Reset Control Register 2 SET, offset: 0x48 */
//    uint8_t RESERVED_2[36];
    uint32_t PRSTCTL0_CLR;                      /**< Peripheral Reset Control Register 0 CLR, offset: 0x70 */
    uint32_t PRSTCTL1_CLR;                      /**< Peripheral Reset Control Register 1 CLR, offset: 0x74 */
    uint32_t PRSTCTL2_CLR;                      /**< Peripheral Reset Control Register 2 CLR, offset: 0x78 */
};

#define TYPE_RT_RSTCTL1 "rt_rstctl1"
OBJECT_DECLARE_SIMPLE_TYPE(RTRSTCTL1State, RT_RSTCTL1)

struct RTRSTCTL1State {
    /*< private >*/
    SysBusDevice parent_obj;

    /*< public >*/
    MemoryRegion  iomem;

    uint32_t SYSRSTSTAT;                        /**< System Reset Status Register, offset: 0x0 */
//    uint8_t RESERVED_0[12];
    uint32_t PRSTCTL0;                          /**< Peripheral Reset Control Register 0, offset: 0x10 */
    uint32_t PRSTCTL1;                          /**< Peripheral Reset Control Register 1, offset: 0x14 */
    uint32_t PRSTCTL2;                          /**< Peripheral Reset Control Register 2, offset: 0x18 */
//    uint8_t RESERVED_1[36];
    uint32_t PRSTCTL0_SET;                      /**< Peripheral Reset Control Register 0 SET, offset: 0x40 */
    uint32_t PRSTCTL1_SET;                      /**< Peripheral Reset Control Register 1 SET, offset: 0x44 */
    uint32_t PRSTCTL2_SET;                      /**< Peripheral Reset Control Register 2 SET, offset: 0x48 */
//    uint8_t RESERVED_2[36];
    uint32_t PRSTCTL0_CLR;                      /**< Peripheral Reset Control Register 0 CLR, offset: 0x70 */
    uint32_t PRSTCTL1_CLR;                      /**< Peripheral Reset Control Register 1 CLR, offset: 0x74 */
    uint32_t PRSTCTL2_CLR;                      /**< Peripheral Reset Control Register 2 CLR, offset: 0x78 */

};

#endif /* RT_RSTCTL_H */
