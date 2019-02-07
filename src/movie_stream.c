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

#include "movie_stream.h"

//////////////////////////////////////////////////////////////////////////
ae_result_t ae_magic_read_string( aeMovieStream * _stream, ae_string_t * _str )
{
    ae_uint32_t size = AE_READZ( _stream );

    ae_string_t buffer = AE_NEWN( _stream->instance, ae_char_t, size + 1U );

    AE_RESULT_PANIC_MEMORY( buffer );

    AE_READN( _stream, buffer, size );

    buffer[size] = '\0';

    *_str = buffer;

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
ae_result_t ae_magic_read_polygon( aeMovieStream * _stream, ae_polygon_t * _polygon )
{
    ae_uint32_t point_count = AE_READZ( _stream );

    _polygon->point_count = point_count;

    if( point_count == 0 )
    {
        _polygon->points = AE_NULLPTR;

        return AE_RESULT_SUCCESSFUL;
    }

    ae_vector2_t * points = AE_NEWN( _stream->instance, ae_vector2_t, point_count );

    AE_RESULT_PANIC_MEMORY( points );

    AE_READN( _stream, points, point_count );

    _polygon->points = (const ae_vector2_t *)points;

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
ae_uint32_t ae_magic_read_size( aeMovieStream * _stream )
{
    ae_uint8_t size255;
    AE_READ( _stream, size255 );

    if( size255 != 255 )
    {
        return (ae_uint32_t)size255;
    }

    ae_uint16_t size65535;
    AE_READ( _stream, size65535 );

    if( size65535 != 65535 )
    {
        return (ae_uint32_t)size65535;
    }

    ae_uint32_t size;
    AE_READ( _stream, size );

    return size;
}
//////////////////////////////////////////////////////////////////////////
ae_result_t ae_magic_read_mesh( aeMovieStream * _stream, ae_mesh_t * _mesh )
{
    ae_uint32_t vertex_count = AE_READZ( _stream );

    if( vertex_count == 0 || vertex_count > AE_MOVIE_MAX_VERTICES )
    {
        _mesh->vertex_count = 0;
        _mesh->index_count = 0;

        _mesh->positions = AE_NULLPTR;
        _mesh->uvs = AE_NULLPTR;
        _mesh->indices = AE_NULLPTR;

        return AE_RESULT_SUCCESSFUL;
    }

    ae_uint32_t indices_count = AE_READZ( _stream );

    _mesh->vertex_count = vertex_count;
    _mesh->index_count = indices_count;

    ae_vector2_t * positions = AE_NEWN( _stream->instance, ae_vector2_t, vertex_count );

    AE_RESULT_PANIC_MEMORY( positions );

    AE_READN( _stream, positions, vertex_count );
    _mesh->positions = (const ae_vector2_t *)positions;

    ae_vector2_t * uvs = AE_NEWN( _stream->instance, ae_vector2_t, vertex_count );

    AE_RESULT_PANIC_MEMORY( uvs );

    AE_READN( _stream, uvs, vertex_count );
    _mesh->uvs = (const ae_vector2_t *)uvs;

    ae_uint16_t * indices = AE_NEWN( _stream->instance, ae_uint16_t, indices_count );

    AE_RESULT_PANIC_MEMORY( indices );

    AE_READN( _stream, indices, indices_count );
    _mesh->indices = indices;

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_magic_read_color( aeMovieStream * _stream, ae_color_t * _color )
{
    AE_READ_COLOR_CHANNEL( _stream, _color->r );
    AE_READ_COLOR_CHANNEL( _stream, _color->g );
    AE_READ_COLOR_CHANNEL( _stream, _color->b );
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_magic_read_viewport( aeMovieStream * _stream, ae_viewport_t * _viewport )
{
    AE_READF( _stream, _viewport->begin_x );
    AE_READF( _stream, _viewport->begin_y );
    AE_READF( _stream, _viewport->end_x );
    AE_READF( _stream, _viewport->end_y );
}