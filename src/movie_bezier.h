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

#ifndef MOVIE_BEZIER_H_
#define MOVIE_BEZIER_H_

#include "movie/movie_type.h"
#include "movie/movie_config.h"
#include "movie/movie_composition.h"

typedef struct ae_bezier_t
{
    ae_float_t ta;
    ae_float_t tb;
    ae_float_t tc;
    ae_float_t td;
} ae_bezier_t;


AE_INTERNAL ae_uint32_t get_bezier_warp_line_count( ae_uint32_t _quality )
{
    ae_uint32_t line_count = AE_MOVIE_BEZIER_WARP_BASE_GRID + _quality * 2;

    return line_count;
}

AE_INTERNAL ae_uint32_t get_bezier_warp_vertex_count( ae_uint32_t _quality )
{
    ae_uint32_t line_count = get_bezier_warp_line_count( _quality );
    ae_uint32_t vertex_count = line_count * line_count;

    return vertex_count;
}

AE_INTERNAL ae_uint32_t get_bezier_warp_index_count( ae_uint32_t _quality )
{
    ae_uint32_t line_count = get_bezier_warp_line_count( _quality );
    ae_uint32_t index_count = (line_count - 1) * (line_count - 1) * 6;

    return index_count;
}

ae_void_t make_layer_bezier_warp_vertices( const struct aeMovieInstance * _instance, const struct aeMovieLayerExtensionBezierWarp * _layerBezierWarp, ae_uint32_t _frame, ae_bool_t _interpolate, ae_float_t _t, const ae_matrix34_t _matrix, const ae_vector2_t * _uvs, aeMovieRenderMesh * _render );

#endif