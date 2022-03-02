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

#include <openssl/sha.h>

#ifndef LIB_H
#define LIB_H

/**
 * This library was created for the code in 'main.cpp' in @todo <reponame>. It is divided in three parts:
 *  1) Hash Computing:
 *      All the hashes are computed by using 'openssl/sha.h' and so stored with type 'unsigned char*'. The functions
 *      contained in this part operate with these hashes and embead 'openssl/sha.h'.
 *  2) Sorting and searching:
 *      This section provides the functions @interface quickSort and @interface ricerca, wich sort an array of
 *      'unsigned char*' and do a binary search over it.
 *  3) Output's prints:
 *      Define some functions for the run-times outputs.
 */

// -------------------------------------------------- Hash Computing --------------------------------------------------

/// This function print the n-length hash in hexadecimal format
void printhash(int n, unsigned char* h);

/// These functions generate at random or copy hexadecimal n-length strings
unsigned char* randhexstring(int n);
unsigned char* copyhexstring(unsigned char* in, int n);

/// 'bytehash' use 'openssl/sha.h' to return the hash on the n-length input @param in
unsigned char* bytehash(int n, unsigned char in[]);

/**
 * The 'chainhash', update the input @param s with its hash for @param chainlength times without leve allocated the
 * middle hases. The @return value is stored in @param s.
 */
void chainhash (unsigned char** s, unsigned char** h, int hashlength, unsigned long long chainlength);

/// Function for finding the 00 byte in an hexadecimal string (wich generate trubles)
int find00(int n, unsigned char* h);

// ---------------------------------------------- Sorting and searching -----------------------------------------------

/// It sort the value in @param vec and does the same swaps on @param vec2 and @param vec3.
void quickSort(unsigned char **vec,unsigned char **vec2,unsigned char **vec3,long long unsigned low,long long unsigned high, int n);

/** It search if @param elem is included in @param vec. If it is true it @return 1 as output and stores the relative
 * index in @param r, otherwise, it returns 0.
 */
int ricerca(unsigned char **vec,unsigned char *elem, long long unsigned start, long long unsigned stop, long long unsigned* r, int n);

// ------------------------------------------------- Output's prints --------------------------------------------------

void run_time_output_start (const char msg[], int incycle_flag);
void run_time_output (unsigned long long round, unsigned long long max_round);
void run_time_output_end (int incycle_flag);

#endif // LIB_H
