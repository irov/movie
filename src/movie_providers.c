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

#include "movie/movie_providers.h"

//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_bool_t __dummy_movie_composition_callback_node_provider(const aeMovieNodeProviderCallbackData * _callbackData, ae_voidptrptr_t _rd, ae_voidptr_t _ud)
{
    AE_UNUSED(_callbackData);
    AE_UNUSED(_ud);

    *_rd = AE_NULL;

    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __dummy_movie_composition_callback_node_deleter(const aeMovieNodeDeleterCallbackData * _callbackData, ae_voidptr_t _ud)
{
    AE_UNUSED(_callbackData);
    AE_UNUSED(_ud);
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __dummy_movie_composition_callback_node_update(const aeMovieNodeUpdateCallbackData * _callbackData, ae_voidptr_t _ud)
{
    AE_UNUSED(_callbackData);
    AE_UNUSED(_ud);
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_bool_t __dummy_movie_composition_callback_camera_provider(const aeMovieCameraProviderCallbackData * _callbackData, ae_voidptrptr_t _rd, ae_voidptr_t _ud)
{
    AE_UNUSED(_callbackData);
    AE_UNUSED(_ud);

    *_rd = AE_NULL;

    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __dummy_movie_composition_callback_camera_deleter(const aeMovieCameraDeleterCallbackData * _callbackData, ae_voidptr_t _ud)
{
    AE_UNUSED(_callbackData);
    AE_UNUSED(_ud);
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __dummy_movie_composition_callback_camera_update(const aeMovieCameraUpdateCallbackData * _callbackData, ae_voidptr_t _ud)
{
    AE_UNUSED(_callbackData);
    AE_UNUSED(_ud);
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_bool_t __dummy_movie_composition_callback_track_matte_provider(const aeMovieTrackMatteProviderCallbackData * _callbackData, ae_voidptrptr_t _rd, ae_voidptr_t _ud)
{
    AE_UNUSED(_callbackData);
    AE_UNUSED(_ud);

    *_rd = AE_NULL;

    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __dummy_movie_composition_callback_track_matte_deleter(const aeMovieTrackMatteDeleterCallbackData * _callbackData, ae_voidptr_t _ud)
{
    AE_UNUSED(_callbackData);
    AE_UNUSED(_ud);
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __dummy_movie_composition_callback_track_matte_update(const aeMovieTrackMatteUpdateCallbackData * _callbackData, ae_voidptr_t _ud)
{
    AE_UNUSED(_callbackData);
    AE_UNUSED(_ud);
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_bool_t __dummy_movie_composition_callback_shader_provider(const aeMovieShaderProviderCallbackData * _callbackData, ae_voidptrptr_t _rd, ae_voidptr_t _ud)
{
    AE_UNUSED(_callbackData);
    AE_UNUSED(_ud);

    *_rd = AE_NULL;

    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __dummy_movie_composition_callback_shader_deleter(const aeMovieShaderDeleterCallbackData * _callbackData, ae_voidptr_t _ud)
{
    AE_UNUSED(_callbackData);
    AE_UNUSED(_ud);
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __dummy_movie_composition_callback_shader_property_update(const aeMovieShaderPropertyUpdateCallbackData * _callbackData, ae_voidptr_t _ud)
{
    AE_UNUSED(_callbackData);
    AE_UNUSED(_ud);
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __dummy_movie_composition_callback_composition_event(const aeMovieCompositionEventCallbackData * _callbackData, ae_voidptr_t _ud)
{
    AE_UNUSED(_callbackData);
    AE_UNUSED(_ud);
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __dummy_movie_composition_callback_composition_state(const aeMovieCompositionStateCallbackData * _callbackData, ae_voidptr_t _ud)
{
    AE_UNUSED(_callbackData);
    AE_UNUSED(_ud);
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_bool_t __dummy_movie_composition_callback_composition_extra_interrupt(const aeMovieCompositionExtraInterruptCallbackData * _callbackData, ae_voidptr_t _ud)
{
    AE_UNUSED(_callbackData);
    AE_UNUSED(_ud);

    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_bool_t __dummy_movie_composition_callback_scene_effect_provider(const aeMovieCompositionSceneEffectProviderCallbackData * _callbackData, ae_voidptrptr_t _rd, ae_voidptr_t _ud)
{
    AE_UNUSED(_callbackData);
    AE_UNUSED(_ud);

    *_rd = AE_NULL;

    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __dummy_movie_composition_callback_scene_effect_deleter(const aeMovieCompositionSceneEffectDeleterCallbackData * _callbackData, ae_voidptr_t _ud)
{
    AE_UNUSED(_callbackData);
    AE_UNUSED(_ud);
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __dummy_movie_composition_callback_scene_effect_update(const aeMovieCompositionSceneEffectUpdateCallbackData * _callbackData, ae_voidptr_t _ud)
{
    AE_UNUSED(_callbackData);
    AE_UNUSED(_ud);
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_bool_t __dummy_movie_composition_callback_subcomposition_provider(const aeMovieSubCompositionProviderCallbackData * _callbackData, ae_voidptrptr_t _rd, ae_voidptr_t _ud)
{
    AE_UNUSED(_callbackData);
    AE_UNUSED(_ud);

    *_rd = AE_NULL;

    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __dummy_movie_composition_callback_subcomposition_deleter(const aeMovieSubCompositionDeleterCallbackData * _callbackData, ae_voidptr_t _ud)
{
    AE_UNUSED(_callbackData);
    AE_UNUSED(_ud);
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __dummy_movie_composition_callback_subcomposition_state(const aeMovieSubCompositionStateCallbackData * _callbackData, ae_voidptr_t _ud)
{
    AE_UNUSED(_callbackData);
    AE_UNUSED(_ud);
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_clear_movie_composition_providers( aeMovieCompositionProviders * _providers )
{
    _providers->node_provider = &__dummy_movie_composition_callback_node_provider;
    _providers->node_deleter = &__dummy_movie_composition_callback_node_deleter;
    _providers->node_update = &__dummy_movie_composition_callback_node_update;
    _providers->camera_provider = &__dummy_movie_composition_callback_camera_provider;
    _providers->camera_deleter = &__dummy_movie_composition_callback_camera_deleter;
    _providers->camera_update = &__dummy_movie_composition_callback_camera_update;
    _providers->track_matte_provider = &__dummy_movie_composition_callback_track_matte_provider;
    _providers->track_matte_deleter = &__dummy_movie_composition_callback_track_matte_deleter;
    _providers->track_matte_update = &__dummy_movie_composition_callback_track_matte_update;
    _providers->shader_provider = &__dummy_movie_composition_callback_shader_provider;
    _providers->shader_deleter = &__dummy_movie_composition_callback_shader_deleter;
    _providers->shader_property_update = &__dummy_movie_composition_callback_shader_property_update;
    _providers->composition_event = &__dummy_movie_composition_callback_composition_event;
    _providers->composition_state = &__dummy_movie_composition_callback_composition_state;
    _providers->composition_extra_interrupt = &__dummy_movie_composition_callback_composition_extra_interrupt;
    _providers->scene_effect_provider = &__dummy_movie_composition_callback_scene_effect_provider;
    _providers->scene_effect_deleter = &__dummy_movie_composition_callback_scene_effect_deleter;
    _providers->scene_effect_update = &__dummy_movie_composition_callback_scene_effect_update;
    _providers->subcomposition_provider = &__dummy_movie_composition_callback_subcomposition_provider;
    _providers->subcomposition_deleter = &__dummy_movie_composition_callback_subcomposition_deleter;
    _providers->subcomposition_state = &__dummy_movie_composition_callback_subcomposition_state;
}