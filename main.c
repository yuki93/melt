/**
 * main.c
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

#include <stdio.h>
#include <stdint.h>

int main(int argc, char **argv)
{
    uintptr_t addr;
    uint8_t v;
    printf("Input Address (HEX):\n");
    scanf("%lx", &addr);
    meltdown(addr, &v);
    return 0;
}
