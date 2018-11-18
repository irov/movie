/******************************************************************************
* libMOVIE Software License v1.0
*
* Copyright (c) 2016-2018, Yuriy Levchenko <irov13@mail.ru>
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

#ifndef MOVIE_MEMORY_H_
#define MOVIE_MEMORY_H_

#include "movie/movie_type.h"

#include "movie_struct.h"

#ifdef AE_MOVIE_MEMORY_DEBUG
//////////////////////////////////////////////////////////////////////////
#	define AE_NEW(instance, type) ((type *)__magic_memory_alloc( instance, #type, sizeof(type), __FILE__, __LINE__))
#	define AE_NEWV(instance, size, doc) (__magic_memory_alloc( instance, doc, size, __FILE__, __LINE__))
#	define AE_NEWN(instance, type, n) ((type *)__magic_memory_alloc_n(instance, #type, sizeof(type), n, __FILE__, __LINE__))
#	define AE_DELETE(instance, ptr) (__magic_memory_free(instance, #ptr, ptr))
#	define AE_DELETEN(instance, ptr) (__magic_memory_free_n(instance, #ptr, ptr))
#	define AE_DELETE_STRING(instance, ptr) (__magic_memory_free_n(instance, #ptr, ptr))
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_voidptr_t __magic_memory_alloc( const aeMovieInstance * _instance, const ae_char_t * _doc, ae_size_t _size, const ae_char_t * _file, ae_uint32_t _line )
{
    AE_UNUSED( _doc );

    ae_userdata_t ptr = _instance->memory_alloc( _instance->instance_userdata, _size, _file, _line );

#   ifdef AE_MOVIE_MEMORY_INFO
    _instance->logger( _instance->instance_userdata, AE_ERROR_MEMORY, "alloc type '%s' ptr '%p' size '%d'\n", _doc, ptr, (ae_uint32_t)_size );
#   endif

    return ptr;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_voidptr_t __magic_memory_alloc_n( const aeMovieInstance * _instance, const ae_char_t * _type, ae_size_t _size, ae_size_t _count, const ae_char_t * _file, ae_uint32_t _line )
{
    AE_UNUSED( _type );

    ae_userdata_t ptr = _instance->memory_alloc_n( _instance->instance_userdata, _size, _count, _file, _line );

#   ifdef AE_MOVIE_MEMORY_INFO
    _instance->logger( _instance->instance_userdata, AE_ERROR_MEMORY, "alloc n type '%s' ptr '%p' size '%d' count '%d'\n", _type, ptr, (ae_uint32_t)_size, (ae_uint32_t)_count );
#   endif

    return ptr;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __magic_memory_free( const aeMovieInstance * _instance, const ae_char_t * _type, ae_constvoidptr_t _ptr )
{
    AE_UNUSED( _type );
#   ifdef AE_MOVIE_MEMORY_INFO
    _instance->logger( _instance->instance_userdata, AE_ERROR_MEMORY, "free type '%s' ptr '%p'\n", _type, _ptr );
#   endif

    _instance->memory_free( _instance->instance_userdata, _ptr );
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __magic_memory_free_n( const aeMovieInstance * _instance, const ae_char_t * _type, ae_constvoidptr_t _ptr )
{
    AE_UNUSED( _type );
#   ifdef AE_MOVIE_MEMORY_INFO
    _instance->logger( _instance->instance_userdata, AE_ERROR_MEMORY, "free n type '%s' ptr '%p'\n", _type, _ptr );
#   endif

    _instance->memory_free_n( _instance->instance_userdata, _ptr );
}
//////////////////////////////////////////////////////////////////////////
#else
//////////////////////////////////////////////////////////////////////////
#	define AE_NEW(instance, type) ((type *)instance->memory_alloc( instance->instance_userdata, sizeof(type) ))
#	define AE_NEWV(instance, size, doc) (instance->memory_alloc( instance->instance_userdata, size ))
#	define AE_NEWN(instance, type, n) ((type *)instance->memory_alloc_n( instance->instance_userdata, sizeof(type), n ))
#	define AE_DELETE(instance, ptr) (instance->memory_free( instance->instance_userdata, ptr))
#	define AE_DELETEN(instance, ptr) (instance->memory_free_n( instance->instance_userdata, ptr))
#	define AE_DELETE_STRING(instance, ptr) (instance->memory_free_n( instance->instance_userdata, ptr))
//////////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////////
#define AE_STRNCMP(instance, src, dst, count) (instance->strncmp(instance->instance_userdata, src, dst, count))

#endif
