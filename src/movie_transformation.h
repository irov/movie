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

#ifndef MOVIE_TRANSFORMATION_H_
#define MOVIE_TRANSFORMATION_H_

#include "movie/movie_instance.h"
#include "movie/movie_type.h"

typedef enum aeMoviePropertyImmutableEnum
{
    AE_MOVIE_PROPERTY_NONE = 0x00000000,
    AE_MOVIE_PROPERTY_ANCHOR_POINT_X = 0x00000001,
    AE_MOVIE_PROPERTY_ANCHOR_POINT_Y = 0x00000002,
    AE_MOVIE_PROPERTY_ANCHOR_POINT_Z = 0x00000004,
    AE_MOVIE_PROPERTY_POSITION_X = 0x00000010,
    AE_MOVIE_PROPERTY_POSITION_Y = 0x00000020,
    AE_MOVIE_PROPERTY_POSITION_Z = 0x00000040,
    AE_MOVIE_PROPERTY_SCALE_X = 0x00000100,
    AE_MOVIE_PROPERTY_SCALE_Y = 0x00000200,
    AE_MOVIE_PROPERTY_SCALE_Z = 0x00000400,
    AE_MOVIE_PROPERTY_QUATERNION_X = 0x00001000,
    AE_MOVIE_PROPERTY_QUATERNION_Y = 0x00002000,
    AE_MOVIE_PROPERTY_QUATERNION_Z = 0x00004000,
    AE_MOVIE_PROPERTY_QUATERNION_W = 0x00008000,
    AE_MOVIE_PROPERTY_SKEW = 0x00010000,
    AE_MOVIE_PROPERTY_SKEW_QUATERNION_Z = 0x00020000,
    AE_MOVIE_PROPERTY_SKEW_QUATERNION_W = 0x00040000,
    AE_MOVIE_PROPERTY_COLOR_R = 0x10000000,
    AE_MOVIE_PROPERTY_COLOR_G = 0x20000000,
    AE_MOVIE_PROPERTY_COLOR_B = 0x40000000,
    AE_MOVIE_PROPERTY_OPACITY = 0x80000000,

    AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL_SKEW = AE_MOVIE_PROPERTY_NONE
    | AE_MOVIE_PROPERTY_SKEW | AE_MOVIE_PROPERTY_SKEW_QUATERNION_Z | AE_MOVIE_PROPERTY_SKEW_QUATERNION_W,

    AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL_QUATERNION = AE_MOVIE_PROPERTY_NONE
    | AE_MOVIE_PROPERTY_QUATERNION_X | AE_MOVIE_PROPERTY_QUATERNION_Y | AE_MOVIE_PROPERTY_QUATERNION_Z | AE_MOVIE_PROPERTY_QUATERNION_W,

    AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL_CAMERA = AE_MOVIE_PROPERTY_NONE
    | AE_MOVIE_PROPERTY_ANCHOR_POINT_X | AE_MOVIE_PROPERTY_ANCHOR_POINT_Y | AE_MOVIE_PROPERTY_ANCHOR_POINT_Z
    | AE_MOVIE_PROPERTY_POSITION_X | AE_MOVIE_PROPERTY_POSITION_Y | AE_MOVIE_PROPERTY_POSITION_Z
    | AE_MOVIE_PROPERTY_QUATERNION_X | AE_MOVIE_PROPERTY_QUATERNION_Y | AE_MOVIE_PROPERTY_QUATERNION_Z | AE_MOVIE_PROPERTY_QUATERNION_W,

    AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL = AE_MOVIE_PROPERTY_NONE
    | AE_MOVIE_PROPERTY_ANCHOR_POINT_X | AE_MOVIE_PROPERTY_ANCHOR_POINT_Y | AE_MOVIE_PROPERTY_ANCHOR_POINT_Z
    | AE_MOVIE_PROPERTY_POSITION_X | AE_MOVIE_PROPERTY_POSITION_Y | AE_MOVIE_PROPERTY_POSITION_Z
    | AE_MOVIE_PROPERTY_SCALE_X | AE_MOVIE_PROPERTY_SCALE_Y | AE_MOVIE_PROPERTY_SCALE_Z
    | AE_MOVIE_PROPERTY_QUATERNION_X | AE_MOVIE_PROPERTY_QUATERNION_Y | AE_MOVIE_PROPERTY_QUATERNION_Z | AE_MOVIE_PROPERTY_QUATERNION_W
    | AE_MOVIE_PROPERTY_SKEW | AE_MOVIE_PROPERTY_SKEW_QUATERNION_Z | AE_MOVIE_PROPERTY_SKEW_QUATERNION_W,

    AE_MOVIE_PROPERTY_COLOR_SUPER_ALL = AE_MOVIE_PROPERTY_NONE
    | AE_MOVIE_PROPERTY_COLOR_R | AE_MOVIE_PROPERTY_COLOR_G | AE_MOVIE_PROPERTY_COLOR_B
    | AE_MOVIE_PROPERTY_OPACITY,
} aeMoviePropertyImmutableEnum;

typedef struct aeMovieLayerTransformation2DImuttable
{
    ae_float_t anchor_point_x;
    ae_float_t anchor_point_y;
    ae_float_t position_x;
    ae_float_t position_y;
    ae_float_t scale_x;
    ae_float_t scale_y;
    ae_float_t quaternion_z;
    ae_float_t quaternion_w;
    ae_float_t skew;
    ae_float_t skew_quaternion_z;
    ae_float_t skew_quaternion_w;
}aeMovieLayerTransformation2DImuttable;

typedef struct aeMovieLayerTransformation3DImuttable
{
    ae_float_t anchor_point_x;
    ae_float_t anchor_point_y;
    ae_float_t anchor_point_z;
    ae_float_t position_x;
    ae_float_t position_y;
    ae_float_t position_z;
    ae_float_t scale_x;
    ae_float_t scale_y;
    ae_float_t scale_z;
    ae_float_t quaternion_x;
    ae_float_t quaternion_y;
    ae_float_t quaternion_z;
    ae_float_t quaternion_w;
    ae_float_t skew;
    ae_float_t skew_quaternion_z;
    ae_float_t skew_quaternion_w;
}aeMovieLayerTransformation3DImuttable;

typedef struct aeMovieLayerTransformation2DTimeline
{
    ae_constvoidptr_t anchor_point_x;
    ae_constvoidptr_t anchor_point_y;
    ae_constvoidptr_t position_x;
    ae_constvoidptr_t position_y;
    ae_constvoidptr_t scale_x;
    ae_constvoidptr_t scale_y;
    ae_constvoidptr_t quaternion_z;
    ae_constvoidptr_t quaternion_w;
    ae_constvoidptr_t skew;
    ae_constvoidptr_t skew_quaternion_z;
    ae_constvoidptr_t skew_quaternion_w;
}aeMovieLayerTransformation2DTimeline;

typedef struct aeMovieLayerTransformation3DTimeline
{
    ae_constvoidptr_t anchor_point_x;
    ae_constvoidptr_t anchor_point_y;
    ae_constvoidptr_t anchor_point_z;
    ae_constvoidptr_t position_x;
    ae_constvoidptr_t position_y;
    ae_constvoidptr_t position_z;
    ae_constvoidptr_t scale_x;
    ae_constvoidptr_t scale_y;
    ae_constvoidptr_t scale_z;
    ae_constvoidptr_t quaternion_x;
    ae_constvoidptr_t quaternion_y;
    ae_constvoidptr_t quaternion_z;
    ae_constvoidptr_t quaternion_w;
    ae_constvoidptr_t skew;
    ae_constvoidptr_t skew_quaternion_z;
    ae_constvoidptr_t skew_quaternion_w;
}aeMovieLayerTransformation3DTimeline;

typedef struct aeMovieLayerColorImuttable
{
    ae_color_channel_t color_r;
    ae_color_channel_t color_g;
    ae_color_channel_t color_b;
}aeMovieLayerColorImuttable;

typedef struct aeMovieLayerColorTimeline
{
    ae_constvoidptr_t color_r;
    ae_constvoidptr_t color_g;
    ae_constvoidptr_t color_b;
}aeMovieLayerColorTimeline;

struct aeMovieLayerTransformation;

typedef ae_void_t( *ae_movie_make_layer_transformation_intepolate_t )(ae_matrix34_t _out, const struct aeMovieLayerTransformation * _transformation, ae_uint32_t _index, ae_float_t _t);
typedef ae_void_t( *ae_movie_make_layer_transformation_fixed_t )(ae_matrix34_t _out, const struct aeMovieLayerTransformation * _transformation, ae_uint32_t _index);

#define AE_MOVIE_LAYER_TRANSFORMATION_BASE()\
    ae_uint32_t immutable_property_mask;\
    ae_uint32_t identity_property_mask;\
    aeMovieLayerColorImuttable immutable_color;\
    aeMovieLayerColorTimeline timeline_color;\
    aeMovieLayerColorImuttable initial_color;\
    ae_color_channel_t immutable_opacity;\
    ae_constvoidptr_t timeline_opacity;\
    ae_color_channel_t initial_opacity;\
    ae_movie_make_layer_transformation_intepolate_t transforamtion_interpolate_matrix;\
    ae_movie_make_layer_transformation_fixed_t transforamtion_fixed_matrix;\
    ae_matrix34_t * immutable_matrix

typedef struct aeMovieLayerTransformation
{
    AE_MOVIE_LAYER_TRANSFORMATION_BASE();
} aeMovieLayerTransformation;

typedef struct aeMovieLayerTransformation2D
{
    AE_MOVIE_LAYER_TRANSFORMATION_BASE();

    aeMovieLayerTransformation2DImuttable immutable;
    aeMovieLayerTransformation2DTimeline * timeline;
} aeMovieLayerTransformation2D;

typedef struct aeMovieLayerTransformation3D
{
    AE_MOVIE_LAYER_TRANSFORMATION_BASE();

    aeMovieLayerTransformation3DImuttable immutable;
    aeMovieLayerTransformation3DTimeline * timeline;
} aeMovieLayerTransformation3D;

ae_result_t ae_movie_load_layer_transformation( aeMovieStream * _stream, aeMovieLayerTransformation * _transformation, ae_bool_t _threeD );
ae_result_t ae_movie_load_camera_transformation( aeMovieStream * _stream, aeMovieCompositionCamera * _camera );
ae_void_t ae_movie_delete_layer_transformation( const aeMovieInstance * _instance, const aeMovieLayerTransformation * _transformation, ae_bool_t _threeD );
ae_color_channel_t ae_movie_make_layer_color_r( const aeMovieLayerTransformation * _transformation, ae_uint32_t _index, ae_bool_t _interpolate, ae_float_t _t );
ae_color_channel_t ae_movie_make_layer_color_g( const aeMovieLayerTransformation * _transformation, ae_uint32_t _index, ae_bool_t _interpolate, ae_float_t _t );
ae_color_channel_t ae_movie_make_layer_color_b( const aeMovieLayerTransformation * _transformation, ae_uint32_t _index, ae_bool_t _interpolate, ae_float_t _t );
ae_color_channel_t ae_movie_make_layer_opacity( const aeMovieLayerTransformation * _transformation, ae_uint32_t _index, ae_bool_t _interpolate, ae_float_t _t );
ae_void_t ae_movie_make_layer_matrix( ae_matrix34_t _out, const aeMovieLayerTransformation * _transformation, ae_bool_t _interpolate, ae_uint32_t _index, ae_float_t _t );
ae_void_t ae_movie_make_camera_transformation( ae_vector3_t _target, ae_vector3_t _position, ae_quaternion_t _quaternion, const aeMovieCompositionCamera * _camera, ae_uint32_t _index, ae_bool_t _interpolate, ae_float_t _t );

ae_void_t ae_movie_make_layer_transformation2d_interpolate( ae_vector2_t _anchor_point, ae_vector2_t _position, ae_vector2_t _scale, ae_quaternionzw_t _quaternion, ae_skew_t _skew, const aeMovieLayerTransformation2D * _transformation2d, ae_uint32_t _index, ae_float_t _t );
ae_void_t ae_movie_make_layer_transformation2d_fixed( ae_vector2_t _anchor_point, ae_vector2_t _position, ae_vector2_t _scale, ae_quaternionzw_t _quaternion, ae_skew_t _skew, const aeMovieLayerTransformation2D * _transformation2d, ae_uint32_t _index );

ae_void_t ae_movie_make_layer_transformation_color_interpolate( ae_color_t * _color, ae_color_channel_t * _opacity, const aeMovieLayerTransformation * _transformation, ae_uint32_t _index, ae_float_t _t );
ae_void_t ae_movie_make_layer_transformation_color_fixed( ae_color_t * _color, ae_color_channel_t * _opacity, const aeMovieLayerTransformation * _transformation, ae_uint32_t _index );

#endif