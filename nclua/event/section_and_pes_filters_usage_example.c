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

#include <stdio.h>
#include <stdlib.h>

#include "sectionfilter.h"
#include "pesfilter.h"

void *section_thread(void *foo){
    sectionfilter_start((sectionfilter_info *)foo);

    return EXIT_SUCCESS;
}

void *pes_thread(void *foo){
    pesfilter_start((uint16_t *)foo);

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {

// 3 section filter examples...
    sectionfilter_info filter1;
    sectionfilter_info filter2;
    sectionfilter_info filter3;

    filter1.pid = 100;
    filter1.table_id = EMPTY_TABLE_ID;
    filter1.subtable_id = EMPTY_SUBTABLE_ID;

    filter2.pid = 200;
    filter2.table_id = 1;
    filter2.subtable_id = EMPTY_SUBTABLE_ID;

    filter3.pid = 300;
    filter3.table_id = 2;
    filter3.subtable_id = 171;

    pthread_t tid1;
    pthread_create(&tid1, NULL, section_thread, (void *) &filter1);

    pthread_t tid2;
    pthread_create(&tid2, NULL, section_thread, (void *) &filter2);

    pthread_t tid3;
    pthread_create(&tid3, NULL, section_thread, (void *) &filter3);


// 2 pes filter examples
    uint16_t pid1 = 66;
    uint16_t pid2 = 99;

    pthread_t tid4;
    pthread_create(&tid4, NULL, pes_thread, (void *) &pid1);

    pesfilter_start(&pid2);

    return EXIT_SUCCESS;
}
