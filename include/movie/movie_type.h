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

#ifndef MOVIE_TYPE_H_
#define MOVIE_TYPE_H_

#include <stdint.h>
#include <stddef.h>

#include "movie/movie_config.h"

typedef void ae_void_t;
typedef uint32_t ae_bool_t;
typedef char ae_char_t;
typedef uint8_t ae_uint8_t;
typedef uint16_t ae_uint16_t;
typedef uint32_t ae_uint32_t;
typedef int32_t ae_int32_t;
typedef size_t ae_size_t;
typedef int32_t ae_enum_t;
typedef float ae_float_t;
typedef float ae_time_t;
typedef ae_void_t * ae_voidptr_t;
typedef ae_voidptr_t * ae_voidptrptr_t;
typedef const ae_void_t * ae_constvoidptr_t;
typedef ae_uint8_t * ae_byteptr_t;
typedef const ae_uint8_t * ae_constbyteptr_t;
typedef ae_voidptr_t ae_userdata_t;
typedef ae_userdata_t * ae_userdataptr_t;

typedef ae_char_t * ae_string_t;

typedef ae_float_t ae_vector2_t[2];
typedef ae_float_t ae_vector3_t[3];
typedef ae_float_t ae_quaternionzw_t[2];
typedef ae_float_t ae_quaternion_t[4];
typedef ae_float_t ae_skew_t[3];
typedef ae_float_t ae_matrix34_t[12];

typedef const ae_float_t * ae_vector3_ptr_t;
typedef const ae_float_t * ae_matrix34_ptr_t;
typedef void( *ae_function_t )(void);

static const ae_bool_t AE_TRUE = 1;
static const ae_bool_t AE_FALSE = 0;
static const ae_voidptr_t AE_NULLPTR = 0;
static const ae_userdata_t AE_USERDATA_NULL = 0;
static const ae_function_t AE_FUNCTION_NULL = 0;

#define AE_BITWISE(X) (1 << X)
#define AE_OPTION(A, B, C, D) ((A << 24) | (B << 16) | (C << 8) | D)

typedef ae_float_t ae_color_channel_t;

typedef struct
{
    ae_color_channel_t r;
    ae_color_channel_t g;
    ae_color_channel_t b;

} ae_color_t;

typedef struct
{
    ae_uint32_t point_count;
    const ae_vector2_t * points;

} ae_polygon_t;

typedef struct
{
    ae_float_t begin_x;
    ae_float_t begin_y;
    ae_float_t end_x;
    ae_float_t end_y;

} ae_viewport_t;

typedef struct
{
    ae_uint32_t vertex_count;
    ae_uint32_t index_count;
    const ae_vector2_t * positions;
    const ae_vector2_t * uvs;
    const ae_uint16_t * indices;

} ae_mesh_t;

typedef struct aeMovieStream aeMovieStream;

#ifndef AE_MOVIE_MAX_LAYER_NAME
#	define AE_MOVIE_MAX_LAYER_NAME 128U
#endif

#ifndef AE_MOVIE_MAX_COMPOSITION_NAME
#	define AE_MOVIE_MAX_COMPOSITION_NAME 128U
#endif

#ifndef AE_MOVIE_MAX_VERTICES
#	define AE_MOVIE_MAX_VERTICES 1024U
#endif

typedef enum
{
    AE_MOVIE_RESOURCE_NONE = 0,
    AE_MOVIE_RESOURCE_SOLID = 4,
    AE_MOVIE_RESOURCE_VIDEO = 5,
    AE_MOVIE_RESOURCE_SOUND = 6,
    AE_MOVIE_RESOURCE_IMAGE = 7,
    AE_MOVIE_RESOURCE_SEQUENCE = 8,
    AE_MOVIE_RESOURCE_PARTICLE = 9,
    AE_MOVIE_RESOURCE_SLOT = 10,
} aeMovieResourceTypeEnum;

typedef enum
{
    AE_MOVIE_EXTENSION_SHADER_PARAMETER_SLIDER = 3,
    AE_MOVIE_EXTENSION_SHADER_PARAMETER_COLOR = 5,
    AE_MOVIE_EXTENSION_SHADER_PARAMETER_TIME = 6,
} aeMovieShaderParameterTypeEnum;

#define AE_MOVIE_RESOURCE_BASE()\
	aeMovieResourceTypeEnum type;\
    ae_string_t name;\
	ae_voidptr_t userdata

typedef struct aeMovieResource
{
    AE_MOVIE_RESOURCE_BASE();

} aeMovieResource;

typedef enum
{
    AE_MOVIE_BLEND_ADD = 1,
    AE_MOVIE_BLEND_ALPHA_ADD,
    AE_MOVIE_BLEND_CLASSIC_COLOR_BURN,
    AE_MOVIE_BLEND_CLASSIC_COLOR_DODGE,
    AE_MOVIE_BLEND_CLASSIC_DIFFERENCE,
    AE_MOVIE_BLEND_COLOR,
    AE_MOVIE_BLEND_COLOR_BURN,
    AE_MOVIE_BLEND_COLOR_DODGE,
    AE_MOVIE_BLEND_DANCING_DISSOLVE,
    AE_MOVIE_BLEND_DARKEN,
    AE_MOVIE_BLEND_DARKER_COLOR,
    AE_MOVIE_BLEND_DIFFERENCE,
    AE_MOVIE_BLEND_DISSOLVE,
    AE_MOVIE_BLEND_EXCLUSION,
    AE_MOVIE_BLEND_HARD_LIGHT,
    AE_MOVIE_BLEND_HARD_MIX,
    AE_MOVIE_BLEND_HUE,
    AE_MOVIE_BLEND_LIGHTEN,
    AE_MOVIE_BLEND_LIGHTER_COLOR,
    AE_MOVIE_BLEND_LINEAR_BURN,
    AE_MOVIE_BLEND_LINEAR_DODGE,
    AE_MOVIE_BLEND_LINEAR_LIGHT,
    AE_MOVIE_BLEND_LUMINESCENT_PREMUL,
    AE_MOVIE_BLEND_LUMINOSITY,
    AE_MOVIE_BLEND_MULTIPLY,
    AE_MOVIE_BLEND_NORMAL,
    AE_MOVIE_BLEND_OVERLAY,
    AE_MOVIE_BLEND_PIN_LIGHT,
    AE_MOVIE_BLEND_SATURATION,
    AE_MOVIE_BLEND_SCREEN,
    AE_MOVIE_BLEND_SILHOUETE_ALPHA,
    AE_MOVIE_BLEND_SILHOUETTE_LUMA,
    AE_MOVIE_BLEND_SOFT_LIGHT,
    AE_MOVIE_BLEND_STENCIL_ALPHA,
    AE_MOVIE_BLEND_STENCIL_LUMA,
    AE_MOVIE_BLEND_VIVID_LIGHT,
} ae_blend_mode_t;

typedef enum
{
    AE_MOVIE_TRACK_MATTE_NONE = 0,
    AE_MOVIE_TRACK_MATTE_ALPHA = 1,
    AE_MOVIE_TRACK_MATTE_ALPHA_INVERTED = 2,
    AE_MOVIE_TRACK_MATTE_LUMA = 3,
    AE_MOVIE_TRACK_MATTE_LUMA_INVERTED = 4,
} ae_track_matte_mode_t;

typedef enum
{
    AE_RESULT_SUCCESSFUL = 0,
    AE_RESULT_INVALID_MAGIC = -2,
    AE_RESULT_INVALID_VERSION = -3,
    AE_RESULT_INVALID_HASH = -4,
    AE_RESULT_INVALID_STREAM = -5,
    AE_RESULT_INVALID_DATA = -6,
    AE_RESULT_INVALID_MEMORY = -7,
    AE_RESULT_INTERNAL_ERROR = -8,
} ae_result_t;

typedef ae_size_t( *ae_movie_stream_memory_read_t )(ae_voidptr_t _data, ae_voidptr_t _buff, ae_size_t _carriage, ae_size_t _size);
typedef ae_void_t( *ae_movie_stream_memory_copy_t )(ae_voidptr_t _data, ae_constvoidptr_t _src, ae_voidptr_t _dst, ae_size_t _size);

#endif