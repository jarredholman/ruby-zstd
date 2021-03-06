/**
 * Copyright (c) 2016-present, Yann Collet, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

#ifndef ZSTDv06_H
#define ZSTDv06_H

#if defined (__cplusplus)
extern "C" {
#endif

/*-*************************************
*  Dependencies
***************************************/
#include <stddef.h>   /* size_t */


/*-***************************************************************
*  Export parameters
*****************************************************************/
/*!
*  ZSTDv06_DLL_EXPORT :
*  Enable exporting of functions when building a Windows DLL
*/
#if defined(_WIN32) && defined(ZSTDv06_DLL_EXPORT) && (ZSTDv06_DLL_EXPORT==1)
#  define ZSTDLIB_API __declspec(dllexport)
#else
#  define ZSTDLIB_API
#endif


/* *************************************
*  Simple functions
***************************************/
/*! ZSTDv06_decompress() :
    `compressedSize` : is the _exact_ size of the compressed blob, otherwise decompression will fail.
    `dstCapacity` must be large enough, equal or larger than originalSize.
    @return : the number of bytes decompressed into `dst` (<= `dstCapacity`),
              or an errorCode if it fails (which can be tested using ZSTDv06_isError()) */
ZSTDLIB_API size_t ZSTDv06_decompress( void* dst, size_t dstCapacity,
                              const void* src, size_t compressedSize);


/* *************************************
*  Helper functions
***************************************/
ZSTDLIB_API size_t      ZSTDv06_compressBound(size_t srcSize); /*!< maximum compressed size (worst case scenario) */

/* Error Management */
ZSTDLIB_API unsigned    ZSTDv06_isError(size_t code);          /*!< tells if a `size_t` function result is an error code */
ZSTDLIB_API const char* ZSTDv06_getErrorName(size_t code);     /*!< provides readable string for an error code */


/* *************************************
*  Explicit memory management
***************************************/
/** Decompression context */
typedef struct ZSTDv06_DCtx_s ZSTDv06_DCtx;
ZSTDLIB_API ZSTDv06_DCtx* ZSTDv06_createDCtx(void);
ZSTDLIB_API size_t     ZSTDv06_freeDCtx(ZSTDv06_DCtx* dctx);      /*!< @return : errorCode */

/** ZSTDv06_decompressDCtx() :
*   Same as ZSTDv06_decompress(), but requires an already allocated ZSTDv06_DCtx (see ZSTDv06_createDCtx()) */
ZSTDLIB_API size_t ZSTDv06_decompressDCtx(ZSTDv06_DCtx* ctx, void* dst, size_t dstCapacity, const void* src, size_t srcSize);


/*-***********************
*  Dictionary API
*************************/
/*! ZSTDv06_decompress_usingDict() :
*   Decompression using a pre-defined Dictionary content (see dictBuilder).
*   Dictionary must be identical to the one used during compression, otherwise regenerated data will be corrupted.
*   Note : dict can be NULL, in which case, it's equivalent to ZSTDv06_decompressDCtx() */
ZSTDLIB_API size_t ZSTDv06_decompress_usingDict(ZSTDv06_DCtx* dctx,
                                             void* dst, size_t dstCapacity,
                                       const void* src, size_t srcSize,
                                       const void* dict,size_t dictSize);


/*-************************
*  Advanced Streaming API
***************************/
struct ZSTDv06_frameParams_s { unsigned long long frameContentSize; unsigned windowLog; };
typedef struct ZSTDv06_frameParams_s ZSTDv06_frameParams;

ZSTDLIB_API size_t ZSTDv06_getFrameParams(ZSTDv06_frameParams* fparamsPtr, const void* src, size_t srcSize);   /**< doesn't consume input */
ZSTDLIB_API size_t ZSTDv06_decompressBegin_usingDict(ZSTDv06_DCtx* dctx, const void* dict, size_t dictSize);
ZSTDLIB_API void   ZSTDv06_copyDCtx(ZSTDv06_DCtx* dctx, const ZSTDv06_DCtx* preparedDCtx);

ZSTDLIB_API size_t ZSTDv06_nextSrcSizeToDecompress(ZSTDv06_DCtx* dctx);
ZSTDLIB_API size_t ZSTDv06_decompressContinue(ZSTDv06_DCtx* dctx, void* dst, size_t dstCapacity, const void* src, size_t srcSize);



/* *************************************
*  ZBUFF API
***************************************/

typedef struct ZBUFFv06_DCtx_s ZBUFFv06_DCtx;
ZSTDLIB_API ZBUFFv06_DCtx* ZBUFFv06_createDCtx(void);
ZSTDLIB_API size_t      ZBUFFv06_freeDCtx(ZBUFFv06_DCtx* dctx);

ZSTDLIB_API size_t ZBUFFv06_decompressInit(ZBUFFv06_DCtx* dctx);
ZSTDLIB_API size_t ZBUFFv06_decompressInitDictionary(ZBUFFv06_DCtx* dctx, const void* dict, size_t dictSize);

ZSTDLIB_API size_t ZBUFFv06_decompressContinue(ZBUFFv06_DCtx* dctx,
                                            void* dst, size_t* dstCapacityPtr,
                                      const void* src, size_t* srcSizePtr);

/*-***************************************************************************
*  Streaming decompression howto
*
*  A ZBUFFv06_DCtx object is required to track streaming operations.
*  Use ZBUFFv06_createDCtx() and ZBUFFv06_freeDCtx() to create/release resources.
*  Use ZBUFFv06_decompressInit() to start a new decompression operation,
*   or ZBUFFv06_decompressInitDictionary() if decompression requires a dictionary.
*  Note that ZBUFFv06_DCtx objects can be re-init multiple times.
*
*  Use ZBUFFv06_decompressContinue() repetitively to consume your input.
*  *srcSizePtr and *dstCapacityPtr can be any size.
*  The function will report how many bytes were read or written by modifying *srcSizePtr and *dstCapacityPtr.
*  Note that it may not consume the entire input, in which case it's up to the caller to present remaining input again.
*  The content of `dst` will be overwritten (up to *dstCapacityPtr) at each function call, so save its content if it matters, or change `dst`.
*  @return : a hint to preferred nb of bytes to use as input for next function call (it's only a hint, to help latency),
*            or 0 when a frame is completely decoded,
*            or an error code, which can be tested using ZBUFFv06_isError().
*
*  Hint : recommended buffer sizes (not compulsory) : ZBUFFv06_recommendedDInSize() and ZBUFFv06_recommendedDOutSize()
*  output : ZBUFFv06_recommendedDOutSize== 128 KB block size is the internal unit, it ensures it's always possible to write a full block when decoded.
*  input  : ZBUFFv06_recommendedDInSize == 128KB + 3;
*           just follow indications from ZBUFFv06_decompressContinue() to minimize latency. It should always be <= 128 KB + 3 .
* *******************************************************************************/


/* *************************************
*  Tool functions
***************************************/
ZSTDLIB_API unsigned ZBUFFv06_isError(size_t errorCode);
ZSTDLIB_API const char* ZBUFFv06_getErrorName(size_t errorCode);

/** Functions below provide recommended buffer sizes for Compression or Decompression operations.
*   These sizes are just hints, they tend to offer better latency */
ZSTDLIB_API size_t ZBUFFv06_recommendedDInSize(void);
ZSTDLIB_API size_t ZBUFFv06_recommendedDOutSize(void);


/*-*************************************
*  Constants
***************************************/
#define ZSTDv06_MAGICNUMBER 0xFD2FB526   /* v0.6 */



#if defined (__cplusplus)
}
#endif

#endif  /* ZSTDv06_BUFFERED_H */
