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

#include "movie_bezier.h"
#include "movie_struct.h"
#include "movie_math.h"

//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __bezier_setup( ae_bezier_t * _bt, ae_float_t t )
{
    ae_float_t t2 = t * t;
    ae_float_t t3 = t2 * t;

    ae_float_t ti = 1.f - t;
    ae_float_t ti2 = ti * ti;
    ae_float_t ti3 = ti2 * ti;

    _bt->ta = ti3;
    _bt->tb = 3.f * t * ti2;
    _bt->tc = 3.f * t2 * ti;
    _bt->td = t3;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_float_t __bezier_point( ae_float_t a, ae_float_t b, ae_float_t c, ae_float_t d, const ae_bezier_t * _bt )
{
    return a * _bt->ta + b * _bt->tb + c * _bt->tc + d * _bt->td;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_float_t __bezier_warp_x( const aeMovieBezierWarp * _bezierWarp, const ae_bezier_t * _bu, const ae_bezier_t * _bv )
{
    const ae_vector2_t * corners = _bezierWarp->corners;
    const ae_vector2_t * beziers = _bezierWarp->beziers;

    ae_float_t x0 = beziers[0][0] + beziers[1][0] - corners[0][0];
    ae_float_t x1 = beziers[2][0] + beziers[3][0] - corners[1][0];
    ae_float_t x2 = beziers[4][0] + beziers[5][0] - corners[2][0];
    ae_float_t x3 = beziers[6][0] + beziers[7][0] - corners[3][0];

    ae_float_t bu0x = __bezier_point( corners[0][0], beziers[0][0], beziers[7][0], corners[3][0], _bv );
    ae_float_t bu1x = __bezier_point( beziers[1][0], x0, x3, beziers[6][0], _bv );
    ae_float_t bu2x = __bezier_point( beziers[2][0], x1, x2, beziers[5][0], _bv );
    ae_float_t bu3x = __bezier_point( corners[1][0], beziers[3][0], beziers[4][0], corners[2][0], _bv );

    ae_float_t x = __bezier_point( bu0x, bu1x, bu2x, bu3x, _bu );

    return x;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_float_t __bezier_warp_y( const aeMovieBezierWarp * _bezierWarp, const ae_bezier_t * _bu, const ae_bezier_t * _bv )
{
    const ae_vector2_t * corners = _bezierWarp->corners;
    const ae_vector2_t * beziers = _bezierWarp->beziers;

    ae_float_t x0 = beziers[0][1] + beziers[1][1] - corners[0][1];
    ae_float_t x1 = beziers[2][1] + beziers[3][1] - corners[1][1];
    ae_float_t x2 = beziers[4][1] + beziers[5][1] - corners[2][1];
    ae_float_t x3 = beziers[6][1] + beziers[7][1] - corners[3][1];

    ae_float_t bu0x = __bezier_point( corners[0][1], beziers[0][1], beziers[7][1], corners[3][1], _bv );
    ae_float_t bu1x = __bezier_point( beziers[1][1], x0, x3, beziers[6][1], _bv );
    ae_float_t bu2x = __bezier_point( beziers[2][1], x1, x2, beziers[5][1], _bv );
    ae_float_t bu3x = __bezier_point( corners[1][1], beziers[3][1], beziers[4][1], corners[2][1], _bv );

    ae_float_t y = __bezier_point( bu0x, bu1x, bu2x, bu3x, _bu );

    return y;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_float_t __get_bezier_warp_grid_invf( ae_uint32_t _quality )
{
    ae_uint32_t line_count = get_bezier_warp_line_count( _quality );
    ae_float_t grid_invf = 1.f / (ae_float_t)(line_count - 1);

    return grid_invf;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __make_bezier_warp_vertices( const aeMovieInstance * _instance, ae_uint32_t _quality, const aeMovieBezierWarp * _bezierWarp, const ae_matrix34_t _matrix, aeMovieRenderMesh * _render )
{
    ae_uint32_t line_count = get_bezier_warp_line_count( _quality );
    ae_float_t grid_invf = __get_bezier_warp_grid_invf( _quality );

    _render->vertexCount = get_bezier_warp_vertex_count( _quality );
    _render->indexCount = get_bezier_warp_index_count( _quality );

    ae_float_t du = 0.f;
    ae_float_t dv = 0.f;

    ae_vector3_t * positions = _render->position;

    ae_uint32_t v = 0;
    for( ; v != line_count; ++v )
    {
        ae_bezier_t bv;
        __bezier_setup( &bv, dv );

        ae_uint32_t u = 0;
        for( ; u != line_count; ++u )
        {
            ae_bezier_t bu;
            __bezier_setup( &bu, du );

            ae_float_t x = __bezier_warp_x( _bezierWarp, &bu, &bv );
            ae_float_t y = __bezier_warp_y( _bezierWarp, &bu, &bv );

            ae_mul_v3_xy_m34( *positions++, x, y, _matrix );

            du += grid_invf;
        }

        du = 0.f;
        dv += grid_invf;
    }

    _render->indices = _instance->bezier_warp_indices[_quality];
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __setup_bezier_corners( aeMovieBezierWarp * _bezier, const ae_vector2_t * _current, const ae_vector2_t * _next, ae_float_t _t )
{
    ae_linerp_f2( _bezier->corners[0], _current[0], _next[0], _t );
    ae_linerp_f2( _bezier->corners[1], _current[1], _next[1], _t );
    ae_linerp_f2( _bezier->corners[2], _current[2], _next[2], _t );
    ae_linerp_f2( _bezier->corners[3], _current[3], _next[3], _t );
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __setup_bezier_beziers( aeMovieBezierWarp * _bezier, const ae_vector2_t * _current, const ae_vector2_t * _next, ae_float_t _t )
{
    ae_linerp_f2( _bezier->beziers[0], _current[0], _next[0], _t );
    ae_linerp_f2( _bezier->beziers[1], _current[1], _next[1], _t );
    ae_linerp_f2( _bezier->beziers[2], _current[2], _next[2], _t );
    ae_linerp_f2( _bezier->beziers[3], _current[3], _next[3], _t );
    ae_linerp_f2( _bezier->beziers[4], _current[4], _next[4], _t );
    ae_linerp_f2( _bezier->beziers[5], _current[5], _next[5], _t );
    ae_linerp_f2( _bezier->beziers[6], _current[6], _next[6], _t );
    ae_linerp_f2( _bezier->beziers[7], _current[7], _next[7], _t );
}
//////////////////////////////////////////////////////////////////////////
ae_void_t make_layer_bezier_warp_vertices( const aeMovieInstance * _instance, const aeMovieLayerExtensionBezierWarp * _layerBezierWarp, ae_uint32_t _frame, ae_bool_t _interpolate, ae_float_t _t, const ae_matrix34_t _matrix, const ae_vector2_t * _uvs, aeMovieRenderMesh * _render )
{
    ae_uint32_t bezier_warp_quality = _layerBezierWarp->quality;

    if( _layerBezierWarp->immutable == AE_TRUE )
    {
        __make_bezier_warp_vertices( _instance, bezier_warp_quality, &_layerBezierWarp->immutable_bezier_warp, _matrix, _render );
    }
    else
    {
        if( _interpolate == AE_FALSE )
        {
            const aeMovieBezierWarp * bezier_warp = _layerBezierWarp->bezier_warps + _frame;

            __make_bezier_warp_vertices( _instance, bezier_warp_quality, bezier_warp, _matrix, _render );
        }
        else
        {
            const aeMovieBezierWarp * bezier_warp_frame_current = _layerBezierWarp->bezier_warps + _frame + 0;
            const aeMovieBezierWarp * bezier_warp_frame_next = _layerBezierWarp->bezier_warps + _frame + 1;

            aeMovieBezierWarp bezierWarp;

            const ae_vector2_t * current_corners = bezier_warp_frame_current->corners;
            const ae_vector2_t * next_corners = bezier_warp_frame_next->corners;
            __setup_bezier_corners( &bezierWarp, current_corners, next_corners, _t );

            const ae_vector2_t * current_beziers = bezier_warp_frame_current->beziers;
            const ae_vector2_t * next_beziers = bezier_warp_frame_next->beziers;
            __setup_bezier_beziers( &bezierWarp, current_beziers, next_beziers, _t );

            __make_bezier_warp_vertices( _instance, bezier_warp_quality, &bezierWarp, _matrix, _render );
        }
    }

    ae_uint32_t vertex_count = get_bezier_warp_vertex_count( bezier_warp_quality );

    const ae_vector2_t * bezier_warp_uvs = _instance->bezier_warp_uvs[bezier_warp_quality];

    if( _uvs == AE_NULLPTR )
    {
        ae_uint32_t vertex_index = 0;
        for( ; vertex_index != vertex_count; ++vertex_index )
        {
            ae_copy_v2( _render->uv[vertex_index], bezier_warp_uvs[vertex_index] );
        }
    }
    else
    {
        ae_float_t bx = _uvs[0][0];
        ae_float_t by = _uvs[0][1];

        ae_float_t ux = _uvs[1][0] - _uvs[0][0];
        ae_float_t uy = _uvs[1][1] - _uvs[0][1];

        ae_float_t vx = _uvs[2][0] - _uvs[1][0];
        ae_float_t vy = _uvs[2][1] - _uvs[1][1];

        ae_uint32_t vertex_index = 0;
        for( ; vertex_index != vertex_count; ++vertex_index )
        {
            ae_float_t u = bezier_warp_uvs[vertex_index][0];
            ae_float_t v = bezier_warp_uvs[vertex_index][1];

            _render->uv[vertex_index][0] = bx + ux * u + vx * v;
            _render->uv[vertex_index][1] = by + uy * u + vy * v;
        }
    }
}