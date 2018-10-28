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

#ifndef MOVIE_PROVIDERS_H_
#define MOVIE_PROVIDERS_H_

#include "movie_type.h"
#include "movie_render.h"

/**
@addtogroup providers
@{
*/

/*
 case AE_MOVIE_STATE_UPDATE_BEGIN:
 case AE_MOVIE_STATE_UPDATE_PROCESS:
 case AE_MOVIE_STATE_UPDATE_PAUSE:
 case AE_MOVIE_STATE_UPDATE_RESUME:
 case AE_MOVIE_STATE_UPDATE_STOP:
 case AE_MOVIE_STATE_UPDATE_END:
 case AE_MOVIE_STATE_UPDATE_SKIP:
 */

typedef enum
{
    AE_MOVIE_STATE_UPDATE_BEGIN = 0,
    AE_MOVIE_STATE_UPDATE_PROCESS,
    AE_MOVIE_STATE_UPDATE_PAUSE,
    AE_MOVIE_STATE_UPDATE_RESUME,
    AE_MOVIE_STATE_UPDATE_INTERRUPT,
    AE_MOVIE_STATE_UPDATE_STOP,
    AE_MOVIE_STATE_UPDATE_END,
    AE_MOVIE_STATE_UPDATE_SKIP
} aeMovieStateUpdateEnum;

typedef struct aeMovieCameraProviderCallbackData
{
    /// @brief Camera name.
    const ae_char_t * name;

    /// @brief Field-of-view in degrees.
    ae_float_t fov;

    /// @brief Viewport width.
    ae_float_t width;

    /// @brief Viewport height.
    ae_float_t height;

    ae_vector3_t target;
    ae_vector3_t position;
    ae_quaternion_t quaternion;

} aeMovieCameraProviderCallbackData;

typedef struct aeMovieCameraDeleterCallbackData
{
    /// @brief Camera element data.
    ae_voidptr_t camera_data;

    /// @brief Camera name.
    const ae_char_t * name;
} aeMovieCameraDeleterCallbackData;

typedef struct aeMovieCameraUpdateCallbackData
{
    /// @brief Camera element data.
    ae_voidptr_t camera_data;

    /// @brief Camera name.
    const ae_char_t * name;

    /// @brief Camera target.
    ae_vector3_t target;

    /// @brief Camera position.
    ae_vector3_t position;

    /// @brief Camera quaternion.
    ae_quaternion_t quaternion;
} aeMovieCameraUpdateCallbackData;

typedef struct aeMovieNodeProviderCallbackData
{
    ae_uint32_t index;

    const aeMovieLayerData * layer;

    /// @brief if node incessantly timeline
    ae_bool_t incessantly;

    ae_bool_t immutable_matrix;

    /// @brief Additional transform, e.g. for slots/sockets.
    ae_matrix34_ptr_t matrix;

    /// @brief Value from 0.0 to 1.0.
    ae_color_t color;

    /// @brief Value from 0.0 to 1.0.
    ae_color_channel_t opacity;

    /// @brief Value from 0.0 to 1.0.
    ae_float_t volume;

    /// @brief Pointer to track matte layer, should be used to create user track matte structure inside provider.
    const aeMovieLayerData * track_matte_layer;
} aeMovieNodeProviderCallbackData;

typedef struct aeMovieNodeDeleterCallbackData
{
    ae_uint32_t index;

    ae_voidptr_t element;
    const aeMovieLayerData * layer;

    const aeMovieLayerData * track_matte_layer;
} aeMovieNodeDeleterCallbackData;

typedef struct aeMovieNodeUpdateCallbackData
{
    ae_uint32_t index;

    ae_voidptr_t element;
    const aeMovieLayerData * layer;

    ae_bool_t loop;
    aeMovieStateUpdateEnum state;
    ae_time_t offset;

    ae_bool_t immutable_matrix;

    /// @brief Additional transform, e.g. for slots/sockets.
    ae_matrix34_ptr_t matrix;

    ae_bool_t immutable_color;

    /// @brief Value from 0.0 to 1.0.
    ae_color_t color;

    /// @brief Value from 0.0 to 1.0.
    ae_color_channel_t opacity;

    /// @brief Value from 0.0 to 1.0.
    ae_float_t volume;
} aeMovieNodeUpdateCallbackData;

typedef struct aeMovieTrackMatteProviderCallbackData
{
    ae_uint32_t index;

    ae_voidptr_t element;
    const aeMovieLayerData * layer;

    ae_bool_t loop;
    ae_time_t offset;

    ae_bool_t immutable_matrix;

    /// @brief Additional transform, e.g. for slots/sockets.
    ae_matrix34_ptr_t matrix;

    ae_bool_t immutable_color;

    /// @brief Value from 0.0 to 1.0.
    ae_color_t color;

    /// @brief Value from 0.0 to 1.0.
    ae_color_channel_t opacity;

    aeMovieRenderMesh * mesh;

    ae_track_matte_mode_t track_matte_mode;
} aeMovieTrackMatteProviderCallbackData;

typedef struct aeMovieTrackMatteUpdateCallbackData
{
    ae_uint32_t index;

    ae_voidptr_t element;
    const aeMovieLayerData * layer;

    ae_bool_t loop;
    aeMovieStateUpdateEnum state;
    ae_time_t offset;

    ae_bool_t immutable_matrix;

    /// @brief Additional transform, e.g. for slots/sockets.
    ae_matrix34_ptr_t matrix;

    ae_bool_t immutable_color;

    /// @brief Value from 0.0 to 1.0.
    ae_color_t color;

    /// @brief Value from 0.0 to 1.0.
    ae_color_channel_t opacity;

    aeMovieRenderMesh * mesh;

    ae_voidptr_t track_matte_data;
} aeMovieTrackMatteUpdateCallbackData;

typedef struct aeMovieTrackMatteDeleterCallbackData
{
    ae_uint32_t index;

    ae_voidptr_t element;
    const aeMovieLayerData * layer;

    ae_voidptr_t track_matte_data;

} aeMovieTrackMatteDeleterCallbackData;

typedef struct aeMovieShaderProviderCallbackData
{
    ae_string_t name;
    ae_uint32_t version;

    ae_string_t shader_vertex;
    ae_string_t shader_fragment;

    ae_uint32_t parameter_count;
    ae_string_t parameter_names[32];
    ae_string_t parameter_uniforms[32];
    ae_uint8_t parameter_types[32];
} aeMovieShaderProviderCallbackData;

typedef struct aeMovieShaderPropertyUpdateCallbackData
{
    ae_uint32_t index;

    ae_voidptr_t element;

    ae_string_t name;
    ae_string_t uniform;
    aeMovieShaderParameterTypeEnum type;

    ae_color_t color;

    ae_float_t value;
} aeMovieShaderPropertyUpdateCallbackData;

typedef struct aeMovieShaderDeleterCallbackData
{
    ae_uint32_t index;

    ae_voidptr_t element;

    ae_string_t name;
    ae_uint32_t version;

} aeMovieShaderDeleterCallbackData;

typedef struct aeMovieCompositionEventCallbackData
{
    ae_uint32_t index;

    ae_voidptr_t element;

    /// @brief Name of the composition which sent the event.
    const ae_char_t * name;

    ae_bool_t immutable_matrix;

    /// @brief Additional transform, e.g. for slots/sockets.
    ae_matrix34_ptr_t matrix;

    ae_bool_t immutable_color;

    /// @brief Value from 0.0 to 1.0.
    ae_color_t color;

    /// @brief Value from 0.0 to 1.0.
    ae_color_channel_t opacity;

    ae_bool_t begin;
} aeMovieCompositionEventCallbackData;

typedef enum
{
    AE_MOVIE_COMPOSITION_PLAY = 0,
    AE_MOVIE_COMPOSITION_STOP,
    AE_MOVIE_COMPOSITION_PAUSE,
    AE_MOVIE_COMPOSITION_RESUME,
    AE_MOVIE_COMPOSITION_INTERRUPT,
    AE_MOVIE_COMPOSITION_END,
    AE_MOVIE_COMPOSITION_LOOP_END,
} aeMovieCompositionStateEnum;

typedef struct aeMovieCompositionStateCallbackData
{
    /// @brief New composition state.
    aeMovieCompositionStateEnum state;

} aeMovieCompositionStateCallbackData;

typedef struct aeMovieCompositionExtraInterruptCallbackData
{
    ae_uint32_t dummy;

} aeMovieCompositionExtraInterruptCallbackData;

typedef struct aeMovieCompositionSceneEffectProviderCallbackData
{
    ae_uint32_t index;

    ae_voidptr_t element;

    /// @brief Additional transform, e.g.
    ae_vector2_t anchor_point;
    ae_vector2_t position;
    ae_vector2_t scale;
    ae_quaternionzw_t quaternion;
    ae_skew_t skew;

    ae_color_t color;
    ae_color_channel_t opacity;

} aeMovieCompositionSceneEffectProviderCallbackData;

typedef struct aeMovieCompositionSceneEffectDeleterCallbackData
{
    ae_voidptr_t element;

    ae_voidptr_t scene_effect_data;

} aeMovieCompositionSceneEffectDeleterCallbackData;

typedef struct aeMovieCompositionSceneEffectUpdateCallbackData
{
    ae_voidptr_t element;

    /// @brief Additional transform, e.g.
    ae_vector2_t anchor_point;
    ae_vector2_t position;
    ae_vector2_t scale;
    ae_quaternionzw_t quaternion;
    ae_skew_t skew;

    ae_color_t color;
    ae_color_channel_t opacity;

    ae_voidptr_t scene_effect_data;

} aeMovieCompositionSceneEffectUpdateCallbackData;

typedef struct aeMovieSubCompositionProviderCallbackData
{
    const aeMovieLayerData * layer;
    const aeMovieCompositionData * composition_data;
    const struct aeMovieCompositionAnimation * animation;

} aeMovieSubCompositionProviderCallbackData;

typedef struct aeMovieSubCompositionDeleterCallbackData
{
    ae_voidptr_t subcomposition_data;

} aeMovieSubCompositionDeleterCallbackData;

typedef struct aeMovieSubCompositionStateCallbackData
{
    /// @brief New composition state.
    aeMovieCompositionStateEnum state;

    ae_voidptr_t subcomposition_data;

} aeMovieSubCompositionStateCallbackData;

typedef ae_bool_t( *ae_movie_composition_callback_node_provider_t )(const aeMovieNodeProviderCallbackData * _callbackData, ae_voidptrptr_t _nd, ae_voidptr_t _ud);
typedef ae_void_t( *ae_movie_composition_callback_node_deleter_t )(const aeMovieNodeDeleterCallbackData * _callbackData, ae_voidptr_t _ud);
typedef ae_void_t( *ae_movie_composition_callback_node_update_t )(const aeMovieNodeUpdateCallbackData * _callbackData, ae_voidptr_t _ud);

typedef ae_bool_t( *ae_movie_composition_callback_camera_provider_t )(const aeMovieCameraProviderCallbackData * _callbackData, ae_voidptrptr_t _cd, ae_voidptr_t _ud);
typedef ae_void_t( *ae_movie_composition_callback_camera_deleter_t )(const aeMovieCameraDeleterCallbackData * _callbackData, ae_voidptr_t _ud);
typedef ae_void_t( *ae_movie_composition_callback_camera_update_t )(const aeMovieCameraUpdateCallbackData * _callbackData, ae_voidptr_t _ud);

typedef ae_bool_t( *ae_movie_composition_callback_track_matte_provider_t )(const aeMovieTrackMatteProviderCallbackData * _callbackData, ae_voidptrptr_t _tmd, ae_voidptr_t _ud);
typedef ae_void_t( *ae_movie_composition_callback_track_matte_deleter_t )(const aeMovieTrackMatteDeleterCallbackData * _callbackData, ae_voidptr_t _ud);
typedef ae_void_t( *ae_movie_composition_callback_track_matte_update_t )(const aeMovieTrackMatteUpdateCallbackData * _callbackData, ae_voidptr_t _ud);

typedef ae_bool_t( *ae_movie_composition_callback_shader_provider_t )(const aeMovieShaderProviderCallbackData * _callbackData, ae_voidptrptr_t _sd, ae_voidptr_t _ud);
typedef ae_void_t( *ae_movie_composition_callback_shader_deleter_t )(const aeMovieShaderDeleterCallbackData * _callbackData, ae_voidptr_t _ud);
typedef ae_void_t( *ae_movie_composition_callback_shader_property_update_t )(const aeMovieShaderPropertyUpdateCallbackData * _callbackData, ae_voidptr_t _ud);

typedef ae_void_t( *ae_movie_composition_callback_composition_event_t )(const aeMovieCompositionEventCallbackData * _callbackData, ae_voidptr_t _ud);
typedef ae_void_t( *ae_movie_composition_callback_composition_state_t )(const aeMovieCompositionStateCallbackData * _callbackData, ae_voidptr_t _ud);
typedef ae_bool_t( *ae_movie_composition_callback_composition_extra_interrupt_t )(const aeMovieCompositionExtraInterruptCallbackData * _callbackData, ae_voidptr_t _ud);

typedef ae_bool_t( *ae_movie_composition_callback_scene_effect_provider_t )(const aeMovieCompositionSceneEffectProviderCallbackData * _callbackData, ae_voidptrptr_t _sed, ae_voidptr_t _ud);
typedef ae_void_t( *ae_movie_composition_callback_scene_effect_deleter_t )(const aeMovieCompositionSceneEffectDeleterCallbackData * _callbackData, ae_voidptr_t _ud);
typedef ae_void_t( *ae_movie_composition_callback_scene_effect_update_t )(const aeMovieCompositionSceneEffectUpdateCallbackData * _callbackData, ae_voidptr_t _ud);

typedef ae_bool_t( *ae_movie_composition_callback_subcomposition_provider_t )(const aeMovieSubCompositionProviderCallbackData * _callbackData, ae_voidptrptr_t _scd, ae_voidptr_t _ud);
typedef ae_void_t( *ae_movie_composition_callback_subcomposition_deleter_t )(const aeMovieSubCompositionDeleterCallbackData * _callbackData, ae_voidptr_t _ud);
typedef ae_void_t( *ae_movie_composition_callback_subcomposition_state_t )(const aeMovieSubCompositionStateCallbackData * _callbackData, ae_voidptr_t _ud);

typedef struct aeMovieCompositionProviders
{
    ae_movie_composition_callback_node_provider_t node_provider;
    ae_movie_composition_callback_node_deleter_t node_deleter;
    ae_movie_composition_callback_node_update_t node_update;

    ae_movie_composition_callback_camera_provider_t camera_provider;
    ae_movie_composition_callback_camera_deleter_t camera_deleter;
    ae_movie_composition_callback_camera_update_t camera_update;

    ae_movie_composition_callback_track_matte_provider_t track_matte_provider;
    ae_movie_composition_callback_track_matte_update_t track_matte_update;
    ae_movie_composition_callback_track_matte_deleter_t track_matte_deleter;

    ae_movie_composition_callback_shader_provider_t shader_provider;
    ae_movie_composition_callback_shader_deleter_t shader_deleter;
    ae_movie_composition_callback_shader_property_update_t shader_property_update;

    ae_movie_composition_callback_composition_event_t composition_event;
    ae_movie_composition_callback_composition_state_t composition_state;
    ae_movie_composition_callback_composition_extra_interrupt_t composition_extra_interrupt;

    ae_movie_composition_callback_scene_effect_provider_t scene_effect_provider;
    ae_movie_composition_callback_scene_effect_deleter_t scene_effect_deleter;
    ae_movie_composition_callback_scene_effect_update_t scene_effect_update;

    ae_movie_composition_callback_subcomposition_provider_t subcomposition_provider;
    ae_movie_composition_callback_subcomposition_deleter_t subcomposition_deleter;
    ae_movie_composition_callback_subcomposition_state_t subcomposition_state;

} aeMovieCompositionProviders;

ae_void_t ae_clear_movie_composition_providers( aeMovieCompositionProviders * _providers );

// providers
/// @}

#endif