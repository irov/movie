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

#ifndef MOVIE_STRUCT_H_
#define MOVIE_STRUCT_H_

#include "movie/movie_type.h"
#include "movie/movie_composition.h"

#include "movie_bezier.h"

//////////////////////////////////////////////////////////////////////////
typedef struct aeMovieBezierWarp
{
    ae_vector2_t corners[4];
    ae_vector2_t beziers[8];

} aeMovieBezierWarp;
//////////////////////////////////////////////////////////////////////////
typedef enum aeMovieLayerExtensionEnum
{
    AE_LAYER_EXTENSION_TIMEREMAP = 1,
    AE_LAYER_EXTENSION_MESH = 2,
    AE_LAYER_EXTENSION_BEZIERWARP = 3,
    AE_LAYER_EXTENSION_COLORVERTEX = 4,
    AE_LAYER_EXTENSION_POLYGON = 5,
    AE_LAYER_EXTENSION_SHADER = 6,
    AE_LAYER_EXTENSION_VIEWPORT = 7,
    AE_LAYER_EXTENSION_VOLUME = 8,
} aeMovieLayerExtensionEnum;
//////////////////////////////////////////////////////////////////////////
typedef enum aeMovieNodeAnimationStateEnum
{
    AE_MOVIE_NODE_ANIMATE_STATIC,
    AE_MOVIE_NODE_ANIMATE_BEGIN,
    AE_MOVIE_NODE_ANIMATE_PROCESS,
    AE_MOVIE_NODE_ANIMATE_END,
} aeMovieNodeAnimationStateEnum;
//////////////////////////////////////////////////////////////////////////
typedef struct aeMovieLayerExtensions
{
    const aeMovieLayerExtensionTimeremap * timeremap;
    const aeMovieLayerExtensionMesh * mesh;
    const aeMovieLayerExtensionBezierWarp * bezier_warp;
    const aeMovieLayerExtensionPolygon * polygon;
    const aeMovieLayerExtensionShader * shader;
    const aeMovieLayerExtensionViewport * viewport;
    const aeMovieLayerExtensionVolume * volume;

} aeMovieLayerExtensions;
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __clear_layer_extensions( aeMovieLayerExtensions * _extensions )
{
    _extensions->timeremap = AE_NULLPTR;
    _extensions->mesh = AE_NULLPTR;
    _extensions->bezier_warp = AE_NULLPTR;
    _extensions->polygon = AE_NULLPTR;
    _extensions->shader = AE_NULLPTR;
    _extensions->viewport = AE_NULLPTR;
    _extensions->volume = AE_NULLPTR;
}
//////////////////////////////////////////////////////////////////////////
struct aeMovieInstance
{
    ae_uint32_t hashmask[5];

    ae_movie_alloc_t memory_alloc;
    ae_movie_alloc_n_t memory_alloc_n;
    ae_movie_free_t memory_free;
    ae_movie_free_n_t memory_free_n;
    ae_movie_strncmp_t strncmp;
    ae_movie_logger_t logger;
    ae_userdata_t instance_userdata;

    ae_vector2_t sprite_uv[4];
    ae_uint16_t sprite_indices[6];

    const ae_vector2_t * bezier_warp_uvs[AE_MOVIE_BEZIER_MAX_QUALITY];
    const ae_uint16_t * bezier_warp_indices[AE_MOVIE_BEZIER_MAX_QUALITY];

    aeMovieLayerExtensions layer_extensions_default;
};
//////////////////////////////////////////////////////////////////////////
struct aeMovieStream
{
    const aeMovieInstance * instance;

    ae_movie_stream_memory_read_t memory_read;
    ae_movie_stream_memory_copy_t memory_copy;
    ae_userdata_t read_userdata;
    ae_userdata_t copy_userdata;

    ae_constvoidptr_t buffer;
    ae_size_t carriage;
};
//////////////////////////////////////////////////////////////////////////
struct aeMovieCompositionAnimation
{
    ae_bool_t enable;

    ae_bool_t play;
    ae_bool_t pause;
    ae_bool_t interrupt;
    ae_bool_t loop;

    ae_time_t time;

    ae_time_t loop_segment_begin;
    ae_time_t loop_segment_end;

    ae_time_t work_area_begin;
    ae_time_t work_area_end;
};
//////////////////////////////////////////////////////////////////////////
struct aeMovieSubComposition
{
    const aeMovieLayerData * layer;
    const aeMovieCompositionData * composition_data;
    const aeMovieCompositionData * subcomposition_data;

    struct aeMovieCompositionAnimation * animation;

    ae_userdata_t subcomposition_userdata;
};
//////////////////////////////////////////////////////////////////////////
struct aeMovieNode
{
    const aeMovieLayerData * layer;

    struct aeMovieNode * relative_node;
    struct aeMovieNode * track_matte_node;

    const ae_viewport_t * viewport;

    const aeMovieSubComposition * subcomposition;

    ae_float_t start_time;
    ae_float_t in_time;
    ae_float_t out_time;

    ae_float_t stretchInv;
    ae_float_t current_time;
    ae_uint32_t current_frame;
    ae_float_t current_frame_t;

    ae_bool_t active;
    ae_bool_t ignore;
    ae_bool_t enable;
    ae_bool_t incessantly;
    ae_bool_t immutable_matrix;
    ae_bool_t immutable_color;

    ae_uint32_t animate;

    ae_uint32_t update_revision;
    ae_matrix34_t matrix;

    ae_color_t composition_color;
    ae_float_t composition_opacity;

    ae_color_t color;
    ae_color_channel_t opacity;

    ae_float_t volume;

    ae_blend_mode_t blend_mode;

    ae_userdata_t element_userdata;
    ae_userdata_t camera_userdata;    
    ae_userdata_t shader_userdata;
    ae_userdata_t track_matte_userdata;
};
//////////////////////////////////////////////////////////////////////////
struct aeMovieComposition
{
    const aeMovieData * movie_data;
    const aeMovieCompositionData * composition_data;

    struct aeMovieCompositionAnimation * animation;

    ae_userdata_t camera_userdata;

    ae_uint32_t * update_revision;

    ae_bool_t interpolate;

    ae_uint32_t node_count;
    aeMovieNode * nodes;

    aeMovieNode * scene_effect_node;
    ae_userdata_t scene_effect_userdata;

    ae_uint32_t subcomposition_count;
    aeMovieSubComposition * subcompositions;

    aeMovieCompositionProviders providers;
    ae_userdata_t provider_userdata;
};
//////////////////////////////////////////////////////////////////////////
typedef struct aeMovieCompositionCameraImuttable
{
    ae_float_t target_x;
    ae_float_t target_y;
    ae_float_t target_z;
    ae_float_t position_x;
    ae_float_t position_y;
    ae_float_t position_z;
    ae_float_t quaternion_x;
    ae_float_t quaternion_y;
    ae_float_t quaternion_z;
    ae_float_t quaternion_w;
} aeMovieCompositionCameraImuttable;
//////////////////////////////////////////////////////////////////////////
typedef struct aeMovieCompositionCameraTimeline
{
    ae_constvoidptr_t target_x;
    ae_constvoidptr_t target_y;
    ae_constvoidptr_t target_z;
    ae_constvoidptr_t position_x;
    ae_constvoidptr_t position_y;
    ae_constvoidptr_t position_z;
    ae_constvoidptr_t quaternion_x;
    ae_constvoidptr_t quaternion_y;
    ae_constvoidptr_t quaternion_z;
    ae_constvoidptr_t quaternion_w;
} aeMovieCompositionCameraTimeline;
//////////////////////////////////////////////////////////////////////////
struct aeMovieCompositionCamera
{
    ae_string_t name;

    ae_float_t zoom;
    ae_float_t fov;

    ae_uint32_t immutable_property_mask;
    ae_uint32_t identity_property_mask;

    aeMovieCompositionCameraImuttable immutable;
    aeMovieCompositionCameraTimeline * timeline;
};
//////////////////////////////////////////////////////////////////////////
struct aeMovieCompositionData
{
    ae_string_t name;

    ae_bool_t master;

    ae_float_t width;
    ae_float_t height;

    ae_time_t duration;

    ae_time_t frameDuration;
    ae_time_t frameDurationInv;

    ae_uint32_t frameCount;

    ae_uint32_t flags;

    ae_vector2_t loop_segment;
    ae_vector3_t anchor_point;
    ae_vector3_t offset_point;
    ae_viewport_t bounds;

    const aeMovieCompositionCamera * camera;

    ae_uint32_t layer_count;
    const aeMovieLayerData * layers;
};
//////////////////////////////////////////////////////////////////////////
struct aeMovieData
{
    const aeMovieInstance * instance;

    ae_string_t name;

    ae_bool_t common_store;

    aeMovieDataProviders providers;
    ae_userdata_t provider_userdata;

    ae_uint32_t atlas_count;
    const aeMovieResource * const * atlases;

    ae_uint32_t resource_count;
    const aeMovieResource * const * resources;

    ae_uint32_t composition_count;
    const aeMovieCompositionData * compositions;
};
//////////////////////////////////////////////////////////////////////////
struct aeMovieLayerData
{
    ae_string_t name;

    ae_uint32_t index;
    aeMovieLayerTypeEnum type;

    ae_bool_t renderable;

    const struct aeMovieCompositionData * composition_data;

    ae_bool_t is_track_matte;
    ae_bool_t has_track_matte;
    ae_track_matte_mode_t track_matte_mode;
    const struct aeMovieLayerData * track_matte_layer;

    ae_uint32_t frame_count;

    const aeMovieLayerExtensions * extensions;

    const aeMovieResource * resource;
    const aeMovieCompositionData * subcomposition_data;

    ae_uint32_t parent_index;

    ae_float_t in_time;
    ae_float_t out_time;
    ae_float_t start_time;
    ae_float_t finish_time;

    ae_bool_t reverse_time;
    ae_bool_t trimmed_time;

    ae_blend_mode_t blend_mode;
    ae_bool_t threeD;

    ae_bool_t incessantly;

    ae_uint32_t options_count;
    ae_uint32_t options[AE_MOVIE_LAYER_MAX_OPTIONS];

    ae_uint32_t play_count;

    ae_float_t stretch;

    const struct aeMovieLayerTransformation * transformation;
    const struct aeMovieLayerCache * cache;
};
//////////////////////////////////////////////////////////////////////////
struct aeMovieLayerExtensionTimeremap
{
    const ae_float_t * times;

};
//////////////////////////////////////////////////////////////////////////
struct aeMovieLayerExtensionMesh
{
    ae_bool_t immutable;
    ae_mesh_t immutable_mesh;

    const ae_mesh_t * meshes;

};
//////////////////////////////////////////////////////////////////////////
struct aeMovieLayerExtensionBezierWarp
{
    ae_bool_t immutable;
    aeMovieBezierWarp immutable_bezier_warp;

    const aeMovieBezierWarp * bezier_warps;

    ae_uint32_t quality;

};
//////////////////////////////////////////////////////////////////////////
struct aeMoviePropertyValue
{
    ae_bool_t immutable;

    ae_float_t immutable_value;

    const ae_float_t * values;
};
//////////////////////////////////////////////////////////////////////////
struct aeMoviePropertyColorChannel
{
    ae_bool_t immutable;

    ae_color_channel_t immutable_value;

    const ae_color_channel_t * values;
};
//////////////////////////////////////////////////////////////////////////
struct aeMoviePropertyColor
{
    const struct aeMoviePropertyColorChannel * color_channel_r;
    const struct aeMoviePropertyColorChannel * color_channel_g;
    const struct aeMoviePropertyColorChannel * color_channel_b;
};
//////////////////////////////////////////////////////////////////////////
#	define AE_MOVIE_SHADER_PARAMETER_BASE()\
    ae_string_t name;\
    ae_string_t uniform;\
    aeMovieShaderParameterTypeEnum type
//////////////////////////////////////////////////////////////////////////
struct aeMovieLayerShaderParameter
{
    AE_MOVIE_SHADER_PARAMETER_BASE();

};
//////////////////////////////////////////////////////////////////////////
struct aeMovieLayerShaderParameterColor
{
    AE_MOVIE_SHADER_PARAMETER_BASE();

    const struct aeMoviePropertyColor * property_color;
};
//////////////////////////////////////////////////////////////////////////
struct aeMovieLayerShaderParameterSlider
{
    AE_MOVIE_SHADER_PARAMETER_BASE();

    const struct aeMoviePropertyValue * property_value;
};
//////////////////////////////////////////////////////////////////////////
struct aeMovieLayerExtensionShader
{
    ae_string_t name;
    ae_uint32_t version;

    ae_string_t shader_vertex;
    ae_string_t shader_fragment;

    ae_uint32_t parameter_count;
    const struct aeMovieLayerShaderParameter ** parameters;

};
//////////////////////////////////////////////////////////////////////////
struct aeMovieLayerExtensionViewport
{
    ae_viewport_t viewport;

};
//////////////////////////////////////////////////////////////////////////
struct aeMovieLayerExtensionVolume
{
    const struct aeMoviePropertyValue * property_volume;
};
//////////////////////////////////////////////////////////////////////////
struct aeMovieLayerExtensionPolygon
{
    ae_bool_t immutable;
    ae_polygon_t immutable_polygon;

    const ae_polygon_t * polygons;

};
//////////////////////////////////////////////////////////////////////////
struct aeMovieResourceImageCache
{
    ae_userdata_t uv_cache_userdata;
    ae_userdata_t mesh_uv_cache_userdata;
    ae_userdata_t bezier_warp_uv_cache_userdata[AE_MOVIE_BEZIER_MAX_QUALITY];

};
//////////////////////////////////////////////////////////////////////////
struct aeMovieResourceVideoCache
{
    ae_userdata_t uv_cache_userdata;
    ae_userdata_t bezier_warp_uv_cache_userdata[AE_MOVIE_BEZIER_MAX_QUALITY];

};
//////////////////////////////////////////////////////////////////////////
struct aeMovieLayerCache
{
    ae_userdata_t immutable_mesh_uv_cache_userdata;
    ae_userdata_t * mesh_uv_cache_userdata;
};
//////////////////////////////////////////////////////////////////////////
#define AE_RESULT( Function, Args ) { ae_result_t result = (Function) Args; if( result != AE_RESULT_SUCCESSFUL ) { return result;}}
#define AE_SUCCESSFUL( Function, Args ) { ae_bool_t successful = (Function) Args; if( successful == AE_FALSE ) {return AE_FALSE;}}
#define AE_RESULT_PANIC_MEMORY( Memory ) AE_MOVIE_PANIC_MEMORY( Memory, AE_RESULT_INVALID_MEMORY )
//////////////////////////////////////////////////////////////////////////
#endif
