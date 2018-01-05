/**
 * meltdown.h
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

#ifndef _YUKI_MELTDOWN_H_
#define _YUKI_MELTDOWN_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * read one byte from an arbitrary address
 */
extern int meltdown(uintptr_t addr, uint8_t *val);

#ifdef __cplusplus
}
#endif

#endif
