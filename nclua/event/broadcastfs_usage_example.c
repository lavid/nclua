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

#include <stdlib.h>
#include <stdio.h>

#include "broadcastfs.h"

void *helper_thread(void *foo){
    broadcastfs_start((fs_info *)foo);

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
    // 2 broadcastfs filter examples
    fs_info carousel_1;
    fs_info carousel_2;

    carousel_1.carousel_id = 0;
    carousel_1.directory_name = "/home/rafael2k/files/dsmcc1";

    carousel_2.carousel_id = 1;
    carousel_2.directory_name = "/home/rafael2k/files/dsmcc2";

    /* For each new carousel found by the TS parser, do: */
    pthread_t tid1;
    pthread_create(&tid1, NULL, helper_thread, (void *) &carousel_1);

    // lets do the call broadcastfs_start( ) for a second carousel simulation
    broadcastfs_start(&carousel_2);

    return EXIT_SUCCESS;
}
