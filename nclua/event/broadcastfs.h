/* Broadcastfs SBTVD DSM-CC file watcher
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

#ifndef HAVE_BROADCASTFS_H__
#define HAVE_BROADCASTFS_H__

#include <stdint.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DOWNLOAD_EVENT 1
#define REMOVED_EVENT 2
#define CHANGED_EVENT 3
#define LOST_EVENT 4

// Information obtained by the DSM-CC parser
    typedef struct {
        char *directory_name; // Carousel root directory
        uint32_t carousel_id; // carousel id is 32bits, by the standard
    } fs_info;

    // Call this function to start the carousel file events monitoring
    int broadcastfs_start(fs_info *handler);

    // This is the callback function, called each time an event happens
    int broadcastfs_event(int event_type, char *path, uint32_t carousel_id, float progress);

#ifdef __cplusplus
};
#endif

#endif /* HAVE_BROADCASTFS__ */
