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

#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "pesfilter.h"


bool pid_list[8192]; // pid (13 bits)

int pesfilter_start(uint16_t *pid){
    uint8_t data[65536];

    if (*pid > 8191){
        fprintf(stderr,"Pid out of range (0-8191)\n");
        return -1;
    }

    pid_list[*pid] = true;

    // here the ts parser would pass the data from the pes packets...

    // the private sections generator, for simulation purposes...
    while (pid_list[*pid]){
        sprintf((char *) data, "   PID: %u   PES PAYLOAD: Much much more PES payload to come....", *pid);
        pesfilter_event(*pid, data);
        usleep(1000000);
    }

    fprintf(stderr, "PES Filter PID: %d Stopped.\n", *pid);

    return EXIT_SUCCESS;
}



int pesfilter_stop(uint16_t *pid){
    pid_list[*pid] = false;

    return EXIT_SUCCESS;
}

int pesfilter_event(uint16_t pid, uint8_t *data){

    fprintf(stderr, "Event: PESFILTER\n  data:\n%s\n", data);

    return EXIT_SUCCESS;
}
