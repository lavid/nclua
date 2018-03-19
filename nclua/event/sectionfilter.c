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

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "sectionfilter.h"


bool filter_list[8192][256]; // pid (13 bits) * table_id (8 bits)

int sectionfilter_start(sectionfilter_info *handler){
    uint8_t data[4096];

    if (handler->pid > 8191){
        fprintf(stderr,"Pid out of range (0-8191)\n");
        return -1;
    }

    if (handler->table_id > 255 && handler->table_id != EMPTY_TABLE_ID){
        fprintf(stderr,"Table_id out of range (0-255)\n");
        return -1;
    }

    if (handler->table_id == EMPTY_TABLE_ID){
        for (int i = 0; i < 256; i++)
            filter_list[handler->pid][i] = true;
    }
    else
        filter_list[handler->pid][handler->table_id] = true;

    // here the ts parser would pass the data from the private section...

    // the private sections generator, for simulation purposes...
    bool cont = true;
    while (cont){
        cont = false;
        for (int j = 0; j < 256; j++){
            if(filter_list[handler->pid][j] == true){
                sprintf((char *) data, "   PID: %d Table_id: %d Subtable_id: %d", handler->pid, (handler->table_id != EMPTY_TABLE_ID)?handler->table_id:j, (handler->subtable_id != EMPTY_SUBTABLE_ID)?handler->subtable_id:0);
                sectionfilter_event(handler->pid, handler->table_id, handler->subtable_id, data);
                usleep(1000000);
                cont = true;
            }
        }
    }

    fprintf(stderr, "Section Filter PID: %d Table_id: %d Subtable_id: %d Stopped.\n", handler->pid, (handler->table_id != EMPTY_TABLE_ID)?handler->table_id:-1, (handler->subtable_id != EMPTY_SUBTABLE_ID)?handler->subtable_id:-1);

    return EXIT_SUCCESS;
}



int sectionfilter_stop(sectionfilter_info *handler){


    if (handler->table_id == EMPTY_TABLE_ID){
        for (int i = 0; i < 256; i++)
            filter_list[handler->pid][i] = false;
    }
    else
        filter_list[handler->pid][handler->table_id] = false;

    return EXIT_SUCCESS;
}

int sectionfilter_event(uint16_t pid, uint16_t table_id, uint16_t subtable_id, uint8_t *data){

    fprintf(stderr, "Event: SECTIONFILTER\n  data:\n%s\n", data);

    return EXIT_SUCCESS;
}
