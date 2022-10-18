 /* Copyright (C) 2019 Matheus Castanho <msc@linux.ibm.com>, IBM
  * For conditions of distribution and use, see copyright notice in zlib.h
  */

#include <altivec.h>
#include "../../deflate.h"

local inline void slide_hash_power8_loop OF((deflate_state *s,
      unsigned n_elems, Posf *table_end)) __attribute__((always_inline));

local void slide_hash_power8_loop(
    deflate_state *s,
    unsigned n_elems,
    Posf *table_end)
{
    vector unsigned short vw, vm, vc, vs, *vp;
    const vector unsigned short v0 = {0};
    unsigned chunks;

    /* Each vector register (chunk) corresponds to 128 bits == 8 Posf,
     * so instead of processing each of the n_elems in the hash table
     * individually, we can do it in chunks of 8 with vector instructions.
     *
     * This function is only called from slide_hash_power8(), and both calls
     * pass n_elems as a power of 2 higher than 2^7, as defined by
     * deflateInit2_(), so n_elems will always be a multiple of 8. */
    chunks = n_elems >> 3;
    Assert(n_elems % 8 == 0, "Weird hash table size!");

    /* This type casting is safe since s->w_size is always <= 64KB
     * as defined by deflateInit2_() and Posf == unsigned short */
    vw[0] = (Posf) s->w_size;
    vw = vec_splat(vw,0);

    vp = (vector unsigned short *) table_end;

    do {
        /* Processing 8 elements at a time */
        vp--;
        vm = *vp;

        /* m < s->w_size ? */
        vc = (vector unsigned short) vec_cmplt(vm,vw);

        /* calculate (m - s->w_size)
         * Overflowed results will be discarded later  */
        vs = vec_sub(vm,vw);

        /* Choose 0 or (m - s->w_size) based on comparison */
        vm = vec_sel(vs,v0,vc);

        *vp = vm;
    } while (--chunks);
};

void ZLIB_INTERNAL _slide_hash_power8(deflate_state *s)
{
    unsigned n;
    Posf *p;

    n = s->hash_size;
    p = &s->head[n];
    slide_hash_power8_loop(s,n,p);

#ifndef FASTEST
    n = s->w_size;
    p = &s->prev[n];
    slide_hash_power8_loop(s,n,p);
#endif
}
