/******************************************************************************
* libMOVIE Software License v1.0
*
* Copyright (c) 2016-2019, Yuriy Levchenko <irov13@mail.ru>
* All rights reserved.
*
* You are granted a perpetual, non-exclusive, non-sublicensable, and
* non-transferable license to use, install, execute, and perform the libMOVIE
* software and derivative works solely for personal or internal
* use. Without the written permission of Yuriy Levchenko, you may not (a) modify, translate,
* adapt, or develop new applications using the libMOVIE or otherwise
* create derivative works or improvements of the libMOVIE or (b) remove,
* delete, alter, or obscure any trademarks or any copyright, trademark, patent,
* or other intellectual property or proprietary rights notices on or in the
* Software, including any copy thereof. Redistributions in binary or source
* form must include this license and terms.
*
* THIS SOFTWARE IS PROVIDED BY YURIY LEVCHENKO "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
* EVENT SHALL YURIY LEVCHENKO BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES, BUSINESS INTERRUPTION,
* OR LOSS OF USE, DATA, OR PROFITS) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

#ifndef MOVIE_INSTANCE_H_
#define MOVIE_INSTANCE_H_

#include "movie_type.h"
#include "movie_typedef.h"

/**
@addtogroup instance
@{
*/

#ifdef AE_MOVIE_MEMORY_DEBUG
typedef ae_voidptr_t( *ae_movie_alloc_t )(ae_userdata_t _userdata, ae_size_t _size, const ae_char_t * _file, ae_uint32_t _line);
typedef ae_voidptr_t( *ae_movie_alloc_n_t )(ae_userdata_t _userdata, ae_size_t _size, ae_size_t _n, const ae_char_t * _file, ae_uint32_t _line);
#else
typedef ae_voidptr_t( *ae_movie_alloc_t )(ae_userdata_t _userdata, ae_size_t _size);
typedef ae_voidptr_t( *ae_movie_alloc_n_t )(ae_userdata_t _userdata, ae_size_t _size, ae_size_t _n);
#endif

typedef ae_void_t( *ae_movie_free_t )(ae_userdata_t _userdata, ae_constvoidptr_t _ptr);
typedef ae_void_t( *ae_movie_free_n_t )(ae_userdata_t _userdata, ae_constvoidptr_t _ptr);

typedef ae_int32_t( *ae_movie_strncmp_t )(ae_userdata_t _userdata, const ae_char_t * _src, const ae_char_t * _dst, ae_size_t _count);

typedef enum
{
    AE_ERROR_INFO,
    AE_ERROR_MEMORY,
    AE_ERROR_STREAM,
    AE_ERROR_WARNING,
    AE_ERROR_UNSUPPORT,
    AE_ERROR_ERROR,
    AE_ERROR_INTERNAL,
    AE_ERROR_CRITICAL,
} aeMovieErrorCode;

typedef ae_void_t( *ae_movie_logger_t )(ae_userdata_t _userdata, aeMovieErrorCode _code, const ae_char_t * _message, ...);

/**
@brief Create a new instance.
@param [in] _alloc,_alloc_n,_free,_free_n,_strncmp,_logger User pointers to utility functions.
@param [in] _data Usually pointer to the creating object.
*/
const aeMovieInstance * ae_create_movie_instance(
    const ae_char_t * _hashkey,
    ae_movie_alloc_t _alloc,
    ae_movie_alloc_n_t _alloc_n,
    ae_movie_free_t _free,
    ae_movie_free_n_t _free_n,
    ae_movie_strncmp_t _strncmp,
    ae_movie_logger_t _logger,
    ae_userdata_t _userdata );

/**
@brief Delete an existing instance.
@param [in] _instance Instance.
*/
ae_void_t ae_delete_movie_instance( const aeMovieInstance * _instance );

// instance
/// @}

#endif