/* Section and PES SBTVD filter
 * Copyright (C) 2018 UFPB
 * Author: Rafael Diniz <rafael@riseup.net>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef HAVE_SECTIONFILTER_H__
#define HAVE_SECTIONFILTER_H__

#include <stdint.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EMPTY_TABLE_ID 666
#define EMPTY_SUBTABLE_ID 66666

    typedef struct {
        uint16_t pid; // 13 bits
        uint16_t table_id; // 8 bits
        uint32_t subtable_id; // Ignoring this for now
    } sectionfilter_info;

    int sectionfilter_start(sectionfilter_info *handler);
    int sectionfilter_stop(sectionfilter_info *handler);
    int sectionfilter_event(uint16_t pid, uint16_t table_id, uint16_t subtable_id, uint8_t *data);

#ifdef __cplusplus
};
#endif

#endif /* HAVE_SECTIONFILTER__ */
