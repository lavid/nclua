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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/inotify.h>

#include "broadcastfs.h"

// now starts inotify marvelous...
#define EVENT_SIZE  (sizeof(struct inotify_event))
#define BUF_LEN     (1024 * (EVENT_SIZE + 16))

int broadcastfs_event(int event_type, char *path, uint32_t carousel_id, float progress){

    switch(event_type){

    case DOWNLOAD_EVENT:
        fprintf(stderr, "Event: DOWNLOAD\n Path: %s\n  Carousel_id: 0x%x\n  Progress: %f\n", path, carousel_id, progress);
        break;
    case REMOVED_EVENT:
        fprintf(stderr, "Event: REMOVED\n Path: %s\n  Carousel_id: 0x%x\n  Progress: %f\n", path, carousel_id, progress);
        break;
    case CHANGED_EVENT:
        fprintf(stderr, "Event: CHANGED\n Path: %s\n  Carousel_id: 0x%x\n  Progress: %f\n", path, carousel_id, progress);
        break;
    case LOST_EVENT:
        fprintf(stderr, "Event: LOST\n Path: %s\n  Carousel_id: 0x%x\n  Progress: %f\n", path, carousel_id, progress);
        break;
    }

    return EXIT_SUCCESS;
}

int broadcastfs_start(fs_info *handler){ // carousel id is 32bits

    int length, i = 0;
    int fd;
    int wd;
    char buffer_inot[BUF_LEN];

    fd = inotify_init();

    if (fd < 0) {
        perror("inotify_init");
    }

    // The directory which the watch monitors can be the DSM-CC directory of Demuxfs...
    wd = inotify_add_watch(fd, handler->directory_name,
                           IN_MODIFY | IN_CREATE | IN_DELETE );

    while(1){
        i = 0;
        length = read(fd, buffer_inot, BUF_LEN);

        if (length < 0) {
            perror("read");
        }

        while (i < length) {
            struct inotify_event *event =
                (struct inotify_event *) &buffer_inot[i];
            if (event->len) {
                if (event->mask & IN_CREATE) {
                    broadcastfs_event(DOWNLOAD_EVENT, event->name, handler->carousel_id, 0.0);
                    usleep(1000000);
                    broadcastfs_event(DOWNLOAD_EVENT, event->name, handler->carousel_id, 0.5);
                    usleep(1000000);
                    broadcastfs_event(DOWNLOAD_EVENT, event->name, handler->carousel_id, 1.0);
                } else if (event->mask & IN_DELETE) {
                    broadcastfs_event(REMOVED_EVENT, event->name, handler->carousel_id, 1.0);
                } else if (event->mask & IN_MODIFY) {
                    broadcastfs_event(CHANGED_EVENT, event->name, handler->carousel_id, 1.0);
                }
            }
            i += EVENT_SIZE + event->len;
        }

    }
    (void) inotify_rm_watch(fd, wd);
    (void) close(fd);

    fprintf(stderr, "Broadcastfs is finishing...\n");

    return EXIT_SUCCESS;
}
