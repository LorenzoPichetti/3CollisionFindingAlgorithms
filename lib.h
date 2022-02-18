/*
 * Copyright 2022 Lorenzo Pichetti lori.pichi@gmail.com
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LIB_H
#define LIB_H

// -------------------------------------------------- Hash Computing --------------------------------------------------

void printhash(int n, unsigned char* h);

unsigned char* randhexstring(int n);
unsigned char* copyhexstring(unsigned char* in, int n);

unsigned char* bytehash(int n, unsigned char in[]);

int find00(int n, unsigned char* h);

// ---------------------------------------------- Sorting and searching -----------------------------------------------

/* low  --> Starting index,  high  --> Ending index */
void quickSort(unsigned char **vec,unsigned char **vec2,unsigned char **vec3,long long unsigned low,long long unsigned high, int n);
/* low  --> Starting index,  high  --> Ending index */
void quickSort2(unsigned char **vec,unsigned char **vec2,long long unsigned low,long long unsigned high, int n);

int ricerca(unsigned char **vec,unsigned char *elem, long long unsigned start, long long unsigned stop, long long unsigned* r, int n);

// ------------------------------------------------- Output's prints --------------------------------------------------

void run_time_output (unsigned long long round, unsigned long long max_round);

#endif // LIB_H
