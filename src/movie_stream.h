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

#ifndef MOVIE_STREAM_H_
#define MOVIE_STREAM_H_

#include "movie/movie_type.h"

#include "movie_memory.h"
#include "movie_struct.h"
#include "movie_debug.h"

#include <stddef.h>

//////////////////////////////////////////////////////////////////////////
#define AE_READ(stream, value) ae_magic_read_value(stream, &(value), sizeof(value))
#define AE_READP(stream, value) ae_magic_read_value(stream, (value), sizeof(*(value)))
#define AE_READF(stream, value) ae_magic_read_value(stream, &(value), sizeof(value))
#define AE_READF2(stream, value) ae_magic_read_value(stream, &(value), sizeof(value))
#define AE_READF3(stream, value) ae_magic_read_value(stream, &(value), sizeof(value))
#define AE_READF4(stream, value) ae_magic_read_value(stream, &(value), sizeof(value))
#define AE_READV(stream, value, size) ae_magic_read_value(stream, value, size)
#define AE_READN(stream, ptr, n) ae_magic_read_value(stream, ptr, sizeof(*ptr) * (n))
//////////////////////////////////////////////////////////////////////////
#define AE_READB(stream) ae_magic_read_bool(stream)
#define AE_READZ(stream) ae_magic_read_size(stream)
#define AE_READ8(stream) ae_magic_read_8(stream)
//////////////////////////////////////////////////////////////////////////
#define AE_READ_COLOR(stream, ptr) ae_magic_read_color(stream, (ptr))
#define AE_READ_COLOR_CHANNEL(stream, ptr) AE_READ(stream, (ptr))
#define AE_READ_VIEWPORT(stream, ptr) ae_magic_read_viewport, (stream, (ptr))
//////////////////////////////////////////////////////////////////////////
#define AE_READ_STRING(stream, ptr) AE_RESULT(ae_magic_read_string, (stream, &(ptr)))
#define AE_READ_POLYGON(stream, ptr) AE_RESULT(ae_magic_read_polygon, (stream, (ptr)))
#define AE_READ_MESH(stream, ptr) AE_RESULT(ae_magic_read_mesh, (stream, (ptr)))
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t ae_magic_read_value( aeMovieStream * _stream, ae_voidptr_t _ptr, ae_size_t _size )
{
    ae_size_t bytesRead = _stream->memory_read( _ptr, _stream->carriage, _size, _stream->read_userdata );

    _stream->carriage += bytesRead;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_bool_t ae_magic_read_bool( aeMovieStream * _stream )
{
    ae_uint8_t value;
    AE_READ( _stream, value );

    AE_MOVIE_ASSERTION_RESULT( value == 0 || value == 1, AE_FALSE );

    return value;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_uint32_t ae_magic_read_8( aeMovieStream * _stream )
{
    ae_uint8_t value;
    AE_READ( _stream, value );

    return (ae_uint32_t)value;
}
//////////////////////////////////////////////////////////////////////////
ae_uint32_t ae_magic_read_size( aeMovieStream * _stream );
ae_result_t ae_magic_read_string( aeMovieStream * _stream, ae_string_t * _str );
ae_result_t ae_magic_read_polygon( aeMovieStream * _stream, ae_polygon_t * _polygon );
ae_void_t ae_magic_read_color( aeMovieStream * _stream, ae_color_t * _color );
ae_void_t ae_magic_read_viewport( aeMovieStream * _stream, ae_viewport_t * _viewport );
ae_void_t ae_magic_read_aabb( aeMovieStream * _stream, ae_aabb_t * _aabb );
ae_result_t ae_magic_read_mesh( aeMovieStream * _stream, ae_mesh_t * _mesh );
//////////////////////////////////////////////////////////////////////////
#endif
