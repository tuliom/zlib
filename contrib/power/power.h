/* Copyright (C) 2019 Matheus Castanho <msc@linux.ibm.com>, IBM
 *               2019 Rogerio Alves    <rogerio.alves@ibm.com>, IBM
 * For conditions of distribution and use, see copyright notice in zlib.h
 */
#include "../../deflate.h"
#include "../../zutil.h"

uLong _adler32_power8(uLong adler, const Bytef* buf, uInt len);

uInt _longest_match_power9(deflate_state *s, IPos cur_match);

void _slide_hash_power8(deflate_state *s);
