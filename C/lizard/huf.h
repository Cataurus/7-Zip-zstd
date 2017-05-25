/* ******************************************************************
   Huffman coder, part of New Generation Entropy library
   header file
   Copyright (C) 2013-2016, Yann Collet.

   BSD 2-Clause License (http://www.opensource.org/licenses/bsd-license.php)

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:

       * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
       * Redistributions in binary form must reproduce the above
   copyright notice, this list of conditions and the following disclaimer
   in the documentation and/or other materials provided with the
   distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   You can contact the author at :
   - Source repository : https://github.com/Cyan4973/FiniteStateEntropy
****************************************************************** */
#ifndef LIZHUF_H_298734234
#define LIZHUF_H_298734234

#if defined (__cplusplus)
extern "C" {
#endif


/* *** Dependencies *** */
#include <stddef.h>    /* size_t */


/* *** simple functions *** */
/**
LIZHUF_compress() :
    Compress content from buffer 'src', of size 'srcSize', into buffer 'dst'.
    'dst' buffer must be already allocated.
    Compression runs faster if `dstCapacity` >= LIZHUF_compressBound(srcSize).
    `srcSize` must be <= `LIZHUF_BLOCKSIZE_MAX` == 128 KB.
    @return : size of compressed data (<= `dstCapacity`).
    Special values : if return == 0, srcData is not compressible => Nothing is stored within dst !!!
                     if return == 1, srcData is a single repeated byte symbol (RLE compression).
                     if LIZHUF_isError(return), compression failed (more details using LIZHUF_getErrorName())
*/
size_t LIZHUF_compress(void* dst, size_t dstCapacity,
              const void* src, size_t srcSize);

/**
LIZHUF_decompress() :
    Decompress HUF data from buffer 'cSrc', of size 'cSrcSize',
    into already allocated buffer 'dst', of minimum size 'dstSize'.
    `dstSize` : **must** be the ***exact*** size of original (uncompressed) data.
    Note : in contrast with FSE, LIZHUF_decompress can regenerate
           RLE (cSrcSize==1) and uncompressed (cSrcSize==dstSize) data,
           because it knows size to regenerate.
    @return : size of regenerated data (== dstSize),
              or an error code, which can be tested using LIZHUF_isError()
*/
size_t LIZHUF_decompress(void* dst,  size_t dstSize,
                const void* cSrc, size_t cSrcSize);


/* ****************************************
*  Tool functions
******************************************/
#define LIZHUF_BLOCKSIZE_MAX (128 * 1024)
size_t LIZHUF_compressBound(size_t size);       /**< maximum compressed size (worst case) */

/* Error Management */
unsigned    LIZHUF_isError(size_t code);        /**< tells if a return value is an error code */
const char* LIZHUF_getErrorName(size_t code);   /**< provides error code string (useful for debugging) */


/* *** Advanced function *** */

/** LIZHUF_compress2() :
*   Same as LIZHUF_compress(), but offers direct control over `maxSymbolValue` and `tableLog` */
size_t LIZHUF_compress2 (void* dst, size_t dstSize, const void* src, size_t srcSize, unsigned maxSymbolValue, unsigned tableLog);


#ifdef LIZHUF_STATIC_LINKING_ONLY

/* *** Dependencies *** */
#include "mem.h"   /* U32 */


/* *** Constants *** */
#define LIZHUF_TABLELOG_ABSOLUTEMAX  16   /* absolute limit of LIZHUF_MAX_TABLELOG. Beyond that value, code does not work */
#define LIZHUF_TABLELOG_MAX  12           /* max configured tableLog (for static allocation); can be modified up to LIZHUF_ABSOLUTEMAX_TABLELOG */
#define LIZHUF_TABLELOG_DEFAULT  11       /* tableLog by default, when not specified */
#define LIZHUF_SYMBOLVALUE_MAX 255
#if (LIZHUF_TABLELOG_MAX > LIZHUF_TABLELOG_ABSOLUTEMAX)
#  error "LIZHUF_TABLELOG_MAX is too large !"
#endif


/* ****************************************
*  Static allocation
******************************************/
/* HUF buffer bounds */
#define LIZHUF_CTABLEBOUND 129
#define LIZHUF_BLOCKBOUND(size) (size + (size>>8) + 8)   /* only true if incompressible pre-filtered with fast heuristic */
#define LIZHUF_COMPRESSBOUND(size) (LIZHUF_CTABLEBOUND + LIZHUF_BLOCKBOUND(size))   /* Macro version, useful for static allocation */

/* static allocation of HUF's Compression Table */
#define LIZHUF_CREATE_STATIC_CTABLE(name, maxSymbolValue) \
    U32 name##hb[maxSymbolValue+1]; \
    void* name##hv = &(name##hb); \
    LIZHUF_CElt* name = (LIZHUF_CElt*)(name##hv)   /* no final ; */

/* static allocation of HUF's DTable */
typedef U32 LIZHUF_DTable;
#define LIZHUF_DTABLE_SIZE(maxTableLog)   (1 + (1<<(maxTableLog)))
#define LIZHUF_CREATE_STATIC_DTABLEX2(DTable, maxTableLog) \
        LIZHUF_DTable DTable[LIZHUF_DTABLE_SIZE((maxTableLog)-1)] = { ((U32)((maxTableLog)-1)*0x1000001) }
#define LIZHUF_CREATE_STATIC_DTABLEX4(DTable, maxTableLog) \
        LIZHUF_DTable DTable[LIZHUF_DTABLE_SIZE(maxTableLog)] = { ((U32)(maxTableLog)*0x1000001) }


/* ****************************************
*  Advanced decompression functions
******************************************/
size_t LIZHUF_decompress4X2 (void* dst, size_t dstSize, const void* cSrc, size_t cSrcSize);   /**< single-symbol decoder */
size_t LIZHUF_decompress4X4 (void* dst, size_t dstSize, const void* cSrc, size_t cSrcSize);   /**< double-symbols decoder */

size_t LIZHUF_decompress4X_DCtx (LIZHUF_DTable* dctx, void* dst, size_t dstSize, const void* cSrc, size_t cSrcSize);   /**< decodes RLE and uncompressed */
size_t LIZHUF_decompress4X_hufOnly(LIZHUF_DTable* dctx, void* dst, size_t dstSize, const void* cSrc, size_t cSrcSize); /**< considers RLE and uncompressed as errors */
size_t LIZHUF_decompress4X2_DCtx(LIZHUF_DTable* dctx, void* dst, size_t dstSize, const void* cSrc, size_t cSrcSize);   /**< single-symbol decoder */
size_t LIZHUF_decompress4X4_DCtx(LIZHUF_DTable* dctx, void* dst, size_t dstSize, const void* cSrc, size_t cSrcSize);   /**< double-symbols decoder */

size_t LIZHUF_decompress1X_DCtx (LIZHUF_DTable* dctx, void* dst, size_t dstSize, const void* cSrc, size_t cSrcSize);
size_t LIZHUF_decompress1X2_DCtx(LIZHUF_DTable* dctx, void* dst, size_t dstSize, const void* cSrc, size_t cSrcSize);   /**< single-symbol decoder */
size_t LIZHUF_decompress1X4_DCtx(LIZHUF_DTable* dctx, void* dst, size_t dstSize, const void* cSrc, size_t cSrcSize);   /**< double-symbols decoder */


/* ****************************************
*  HUF detailed API
******************************************/
/*!
LIZHUF_compress() does the following:
1. count symbol occurrence from source[] into table count[] using LIZFSE_count()
2. (optional) refine tableLog using LIZHUF_optimalTableLog()
3. build Huffman table from count using LIZHUF_buildCTable()
4. save Huffman table to memory buffer using LIZHUF_writeCTable()
5. encode the data stream using LIZHUF_compress4X_usingCTable()

The following API allows targeting specific sub-functions for advanced tasks.
For example, it's possible to compress several blocks using the same 'CTable',
or to save and regenerate 'CTable' using external methods.
*/
/* LIZFSE_count() : find it within "fse.h" */
unsigned LIZHUF_optimalTableLog(unsigned maxTableLog, size_t srcSize, unsigned maxSymbolValue);
typedef struct LIZHUF_CElt_s LIZHUF_CElt;   /* incomplete type */
size_t LIZHUF_buildCTable (LIZHUF_CElt* CTable, const unsigned* count, unsigned maxSymbolValue, unsigned maxNbBits);
size_t LIZHUF_writeCTable (void* dst, size_t maxDstSize, const LIZHUF_CElt* CTable, unsigned maxSymbolValue, unsigned huffLog);
size_t LIZHUF_compress4X_usingCTable(void* dst, size_t dstSize, const void* src, size_t srcSize, const LIZHUF_CElt* CTable);


/*! LIZHUF_readStats() :
    Read compact Huffman tree, saved by LIZHUF_writeCTable().
    `huffWeight` is destination buffer.
    @return : size read from `src` , or an error Code .
    Note : Needed by LIZHUF_readCTable() and LIZHUF_readDTableXn() . */
size_t LIZHUF_readStats(BYTE* huffWeight, size_t hwSize, U32* rankStats,
                     U32* nbSymbolsPtr, U32* tableLogPtr,
                     const void* src, size_t srcSize);

/** LIZHUF_readCTable() :
*   Loading a CTable saved with LIZHUF_writeCTable() */
size_t LIZHUF_readCTable (LIZHUF_CElt* CTable, unsigned maxSymbolValue, const void* src, size_t srcSize);


/*
LIZHUF_decompress() does the following:
1. select the decompression algorithm (X2, X4) based on pre-computed heuristics
2. build Huffman table from save, using LIZHUF_readDTableXn()
3. decode 1 or 4 segments in parallel using LIZHUF_decompressSXn_usingDTable
*/

/** LIZHUF_selectDecoder() :
*   Tells which decoder is likely to decode faster,
*   based on a set of pre-determined metrics.
*   @return : 0==LIZHUF_decompress4X2, 1==LIZHUF_decompress4X4 .
*   Assumption : 0 < cSrcSize < dstSize <= 128 KB */
U32 LIZHUF_selectDecoder (size_t dstSize, size_t cSrcSize);

size_t LIZHUF_readDTableX2 (LIZHUF_DTable* DTable, const void* src, size_t srcSize);
size_t LIZHUF_readDTableX4 (LIZHUF_DTable* DTable, const void* src, size_t srcSize);

size_t LIZHUF_decompress4X_usingDTable(void* dst, size_t maxDstSize, const void* cSrc, size_t cSrcSize, const LIZHUF_DTable* DTable);
size_t LIZHUF_decompress4X2_usingDTable(void* dst, size_t maxDstSize, const void* cSrc, size_t cSrcSize, const LIZHUF_DTable* DTable);
size_t LIZHUF_decompress4X4_usingDTable(void* dst, size_t maxDstSize, const void* cSrc, size_t cSrcSize, const LIZHUF_DTable* DTable);


/* single stream variants */

size_t LIZHUF_compress1X (void* dst, size_t dstSize, const void* src, size_t srcSize, unsigned maxSymbolValue, unsigned tableLog);
size_t LIZHUF_compress1X_usingCTable(void* dst, size_t dstSize, const void* src, size_t srcSize, const LIZHUF_CElt* CTable);

size_t LIZHUF_decompress1X2 (void* dst, size_t dstSize, const void* cSrc, size_t cSrcSize);   /* single-symbol decoder */
size_t LIZHUF_decompress1X4 (void* dst, size_t dstSize, const void* cSrc, size_t cSrcSize);   /* double-symbol decoder */

size_t LIZHUF_decompress1X_usingDTable(void* dst, size_t maxDstSize, const void* cSrc, size_t cSrcSize, const LIZHUF_DTable* DTable);
size_t LIZHUF_decompress1X2_usingDTable(void* dst, size_t maxDstSize, const void* cSrc, size_t cSrcSize, const LIZHUF_DTable* DTable);
size_t LIZHUF_decompress1X4_usingDTable(void* dst, size_t maxDstSize, const void* cSrc, size_t cSrcSize, const LIZHUF_DTable* DTable);


#endif /* LIZHUF_STATIC_LINKING_ONLY */


#if defined (__cplusplus)
}
#endif

#endif   /* LIZHUF_H_298734234 */