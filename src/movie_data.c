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

#include "movie/movie_data.h"
#include "movie/movie_resource.h"
#include "movie/movie_version.h"

#include "movie_transformation.h"
#include "movie_bezier.h"
#include "movie_utils.h"
#include "movie_memory.h"
#include "movie_stream.h"

//////////////////////////////////////////////////////////////////////////
aeMovieData * ae_create_movie_data( const aeMovieInstance * _instance, const aeMovieDataProviders * _providers, ae_userdata_t _userdata )
{
    aeMovieData * movie = AE_NEW( _instance, aeMovieData );

    if( movie == AE_NULLPTR )
    {
        return AE_NULLPTR;
    }

    movie->instance = _instance;

    movie->name = AE_NULLPTR;

    movie->providers = *_providers;
    movie->provider_userdata = _userdata;

    movie->atlas_count = 0;
    movie->atlases = AE_NULLPTR;

    movie->resource_count = 0;
    movie->resources = AE_NULLPTR;

    movie->composition_count = 0;
    movie->compositions = AE_NULLPTR;

    return movie;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __delete_mesh_t( const aeMovieInstance * _instance, const ae_mesh_t * _mesh )
{
    AE_DELETEN( _instance, _mesh->positions );
    AE_DELETEN( _instance, _mesh->uvs );
    AE_DELETEN( _instance, _mesh->indices );
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __delete_layer_mesh_t( const aeMovieInstance * _instance, const aeMovieLayerExtensionMesh * _layerMesh, ae_uint32_t _count )
{
    if( _layerMesh->immutable == AE_TRUE )
    {
        __delete_mesh_t( _instance, &_layerMesh->immutable_mesh );
    }
    else
    {
        const ae_mesh_t * it_mesh = _layerMesh->meshes;
        const ae_mesh_t * it_mesh_end = _layerMesh->meshes + _count;
        for( ; it_mesh != it_mesh_end; ++it_mesh )
        {
            const ae_mesh_t * mesh = it_mesh;

            __delete_mesh_t( _instance, mesh );
        }

        AE_DELETEN( _instance, _layerMesh->meshes );
    }
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __delete_property_value( const aeMovieInstance * _instance, const struct aeMoviePropertyValue * _property )
{
    AE_DELETEN( _instance, _property->values );
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __delete_property_color_channel( const aeMovieInstance * _instance, const struct aeMoviePropertyColorChannel * _property )
{
    AE_DELETEN( _instance, _property->values );
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __delete_property_color( const aeMovieInstance * _instance, const struct aeMoviePropertyColor * _property )
{
    __delete_property_color_channel( _instance, _property->color_channel_r );
    AE_DELETEN( _instance, _property->color_channel_r );

    __delete_property_color_channel( _instance, _property->color_channel_g );
    AE_DELETEN( _instance, _property->color_channel_g );

    __delete_property_color_channel( _instance, _property->color_channel_b );
    AE_DELETEN( _instance, _property->color_channel_b );
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __callback_cache_uv_deleter( const aeMovieData * _movieData, ae_userdata_t _userdata )
{
    aeMovieDataCacheUVDeleterCallbackData callbackData;
    callbackData.uv_cache_userdata = _userdata;

    (*_movieData->providers.cache_uv_deleter)(&callbackData, _movieData->provider_userdata);
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __delete_movie_resource( const aeMovieData * _movieData, const aeMovieResource * _resource )
{
    const aeMovieInstance * instance = _movieData->instance;

    aeMovieResourceTypeEnum type = _resource->type;

    switch( type )
    {
    case AE_MOVIE_RESOURCE_NONE:
        {
        }break;
    case AE_MOVIE_RESOURCE_SOLID:
        {
            const aeMovieResourceSolid * resource = (const aeMovieResourceSolid *)_resource;

            AE_UNUSED( resource );

        }break;
    case AE_MOVIE_RESOURCE_VIDEO:
        {
            const aeMovieResourceVideo * resource = (const aeMovieResourceVideo *)_resource;

            AE_DELETE_STRING( instance, resource->path );

            if( resource->cache != AE_NULLPTR )
            {
                __callback_cache_uv_deleter( _movieData, resource->cache->uv_cache_userdata );

                ae_uint32_t quality = 0;
                for( ; quality != AE_MOVIE_BEZIER_MAX_QUALITY; ++quality )
                {
                    ae_userdata_t uv_cache_data = resource->cache->bezier_warp_uv_cache_userdata[quality];

                    __callback_cache_uv_deleter( _movieData, uv_cache_data );
                }

                AE_DELETE( instance, resource->cache );
            }

        }break;
    case AE_MOVIE_RESOURCE_SOUND:
        {
            const aeMovieResourceSound * resource_sound = (const aeMovieResourceSound *)_resource;

            AE_DELETE_STRING( instance, resource_sound->path );

        }break;
    case AE_MOVIE_RESOURCE_IMAGE:
        {
            const aeMovieResourceImage * resource_image = (const aeMovieResourceImage *)_resource;

            AE_DELETE_STRING( instance, resource_image->path );

            if( resource_image->uvs != instance->sprite_uv )
            {
                AE_DELETEN( instance, resource_image->uvs );
            }

            ae_uint32_t index_bezier_warp_uv = 0;
            for( ; index_bezier_warp_uv != AE_MOVIE_BEZIER_MAX_QUALITY; ++index_bezier_warp_uv )
            {
                const ae_vector2_t * uvs = resource_image->bezier_warp_uvs[index_bezier_warp_uv];

                if( uvs != instance->bezier_warp_uvs[index_bezier_warp_uv] )
                {
                    AE_DELETEN( instance, uvs );
                }
            }

            if( resource_image->mesh != AE_NULLPTR )
            {
                __delete_mesh_t( instance, resource_image->mesh );

                AE_DELETE( instance, resource_image->mesh );
            }

            if( resource_image->cache != AE_NULLPTR )
            {
                __callback_cache_uv_deleter( _movieData, resource_image->cache->uv_cache_userdata );
                __callback_cache_uv_deleter( _movieData, resource_image->cache->mesh_uv_cache_userdata );

                ae_uint32_t quality = 0;
                for( ; quality != AE_MOVIE_BEZIER_MAX_QUALITY; ++quality )
                {
                    ae_userdata_t uv_cache_data = resource_image->cache->bezier_warp_uv_cache_userdata[quality];

                    __callback_cache_uv_deleter( _movieData, uv_cache_data );
                }

                AE_DELETE( instance, resource_image->cache );
            }

        }break;
    case AE_MOVIE_RESOURCE_SEQUENCE:
        {
            const aeMovieResourceSequence * resource_sequence = (const aeMovieResourceSequence *)_resource;

            AE_DELETEN( instance, resource_sequence->images );

        }break;
    case AE_MOVIE_RESOURCE_PARTICLE:
        {
            const aeMovieResourceParticle * resource_particle = (const aeMovieResourceParticle *)_resource;

            AE_DELETE_STRING( instance, resource_particle->path );

        }break;
    case AE_MOVIE_RESOURCE_SLOT:
        {
            const aeMovieResourceSlot * resource_slot = (const aeMovieResourceSlot *)_resource;

            AE_UNUSED( resource_slot );

        }break;
    }

    (*_movieData->providers.resource_deleter)(type, _resource->userdata, _movieData->provider_userdata);

    AE_DELETE_STRING( instance, _resource->name );
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_delete_movie_data( const aeMovieData * _movieData )
{
    const aeMovieInstance * instance = _movieData->instance;

    if( _movieData->atlases != AE_NULLPTR )
    {
        const aeMovieResource * const * it_atlas = _movieData->atlases;
        const aeMovieResource * const * it_atlas_end = _movieData->atlases + _movieData->atlas_count;
        for( ; it_atlas != it_atlas_end; ++it_atlas )
        {
            const aeMovieResource * atlas = *it_atlas;

            __delete_movie_resource( _movieData, atlas );

            AE_DELETE( instance, atlas );
        }

        AE_DELETEN( instance, _movieData->atlases );
    }

    if( _movieData->resources != AE_NULLPTR )
    {
        const aeMovieResource * const * it_resource = _movieData->resources;
        const aeMovieResource * const * it_resource_end = _movieData->resources + _movieData->resource_count;
        for( ; it_resource != it_resource_end; ++it_resource )
        {
            const aeMovieResource * resource = *it_resource;

            __delete_movie_resource( _movieData, resource );

            AE_DELETE( instance, resource );
        }
    }

    if( _movieData->compositions != AE_NULLPTR )
    {
        const aeMovieCompositionData * it_composition = _movieData->compositions;
        const aeMovieCompositionData * it_composition_end = _movieData->compositions + _movieData->composition_count;
        for( ; it_composition != it_composition_end; ++it_composition )
        {
            const aeMovieCompositionData * composition = it_composition;

            if( composition->camera != AE_NULLPTR )
            {
                const aeMovieCompositionCamera * camera = composition->camera;

                AE_DELETE_STRING( instance, camera->name );

                AE_DELETE( instance, composition->camera );
            }

            const aeMovieLayerData * it_layer = composition->layers;
            const aeMovieLayerData * it_layer_end = composition->layers + composition->layer_count;
            for( ; it_layer != it_layer_end; ++it_layer )
            {
                const aeMovieLayerData * layer = it_layer;

                if( layer->cache != AE_NULLPTR )
                {
                    __callback_cache_uv_deleter( _movieData, layer->cache->immutable_mesh_uv_cache_userdata );

                    ae_uint32_t frame_count = layer->frame_count;

                    if( layer->cache->mesh_uv_cache_userdata != AE_NULLPTR )
                    {
                        ae_uint32_t index = 0;
                        for( ; index != frame_count; ++index )
                        {
                            ae_userdata_t uv_cache_data = layer->cache->mesh_uv_cache_userdata[index];

                            __callback_cache_uv_deleter( _movieData, uv_cache_data );
                        }

                        AE_DELETEN( instance, layer->cache->mesh_uv_cache_userdata );
                    }

                    AE_DELETE( instance, layer->cache );
                }

                const aeMovieLayerExtensions * extensions = layer->extensions;

                if( extensions->timeremap != AE_NULLPTR )
                {
                    const aeMovieLayerExtensionTimeremap * timeremap = extensions->timeremap;

                    AE_DELETEN( instance, timeremap->times );

                    AE_DELETE( instance, extensions->timeremap );
                }

                if( extensions->mesh != AE_NULLPTR )
                {
                    const aeMovieLayerExtensionMesh * mesh = extensions->mesh;

                    __delete_layer_mesh_t( instance, mesh, layer->frame_count );

                    AE_DELETE( instance, extensions->mesh );
                }

                if( extensions->bezier_warp != AE_NULLPTR )
                {
                    const aeMovieLayerExtensionBezierWarp * bezier_warp = extensions->bezier_warp;

                    AE_DELETEN( instance, bezier_warp->bezier_warps );

                    AE_DELETE( instance, extensions->bezier_warp );
                }

                if( extensions->polygon != AE_NULLPTR )
                {
                    const aeMovieLayerExtensionPolygon * polygon = extensions->polygon;

                    AE_DELETEN( instance, polygon->polygons );

                    AE_DELETE( instance, extensions->polygon );
                }

                if( extensions->shader != AE_NULLPTR )
                {
                    const aeMovieLayerExtensionShader * shader = extensions->shader;

                    AE_DELETE_STRING( instance, shader->name );
                    AE_DELETE_STRING( instance, shader->description );

                    const struct aeMovieLayerShaderParameter ** it_parameter = shader->parameters;
                    const struct aeMovieLayerShaderParameter ** it_parameter_end = shader->parameters + shader->parameter_count;

                    for( ;
                        it_parameter != it_parameter_end;
                        ++it_parameter )
                    {
                        const struct aeMovieLayerShaderParameter * parameter = *it_parameter;

                        AE_DELETE_STRING( instance, parameter->name );
                        AE_DELETE_STRING( instance, parameter->uniform );

                        aeMovieShaderParameterTypeEnum parameter_type = parameter->type;

                        switch( parameter_type )
                        {
                        case AE_MOVIE_EXTENSION_SHADER_PARAMETER_SLIDER:
                            {
                                const struct aeMovieLayerShaderParameterSlider * parameter_slider = (const struct aeMovieLayerShaderParameterSlider *)parameter;

                                __delete_property_value( instance, parameter_slider->property_value );

                                AE_DELETE( instance, parameter_slider->property_value );
                            }break;
                        case AE_MOVIE_EXTENSION_SHADER_PARAMETER_ANGLE:
                            {
                                const struct aeMovieLayerShaderParameterAngle * parameter_angle = (const struct aeMovieLayerShaderParameterAngle *)parameter;

                                __delete_property_value( instance, parameter_angle->property_value );

                                AE_DELETE( instance, parameter_angle->property_value );
                            }break;
                        case AE_MOVIE_EXTENSION_SHADER_PARAMETER_COLOR:
                            {
                                const struct aeMovieLayerShaderParameterColor * parameter_color = (const struct aeMovieLayerShaderParameterColor *)parameter;

                                __delete_property_color( instance, parameter_color->property_color );

                                AE_DELETE( instance, parameter_color->property_color );
                            }break;
                        case AE_MOVIE_EXTENSION_SHADER_PARAMETER_TIME:
                            {
                            }break;
                        }

                        AE_DELETE( instance, parameter );
                    }

                    AE_DELETEN( instance, shader->parameters );

                    AE_DELETE( instance, extensions->shader );
                }

                if( extensions->viewport != AE_NULLPTR )
                {
                    const aeMovieLayerExtensionViewport * viewport = extensions->viewport;

                    AE_UNUSED( viewport );

                    AE_DELETE( instance, extensions->viewport );
                }

                if( extensions->volume != AE_NULLPTR )
                {
                    const aeMovieLayerExtensionVolume * volume = extensions->volume;

                    AE_DELETE( instance, volume->property_volume );

                    AE_DELETE( instance, extensions->volume );
                }

                if( extensions != &instance->layer_extensions_default )
                {
                    AE_DELETE( instance, layer->extensions );
                }

                ae_movie_delete_layer_transformation( instance, layer->transformation, layer->threeD );

                AE_DELETE( instance, layer->transformation );

                AE_DELETE_STRING( instance, layer->name );
            }

            AE_DELETEN( instance, composition->layers );

            AE_DELETE_STRING( instance, composition->name );
        }
    }

    AE_DELETEN( instance, _movieData->resources );
    AE_DELETEN( instance, _movieData->compositions );

    if( _movieData->name != AE_NULLPTR )
    {
        AE_DELETE_STRING( instance, _movieData->name );
    }

    AE_DELETE( instance, _movieData );
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __load_movie_data_composition_camera( aeMovieStream * _stream, aeMovieCompositionData * _compositionData )
{
    aeMovieCompositionCamera * camera = AE_NEW( _stream->instance, aeMovieCompositionCamera );

    AE_RESULT_PANIC_MEMORY( camera );

    ae_bool_t export_camera = AE_READB( _stream );

    if( export_camera == AE_FALSE )
    {
        ae_char_t * camera_name = AE_NEWN( _stream->instance, ae_char_t, 5 );

        AE_RESULT_PANIC_MEMORY( camera_name );

        camera_name[0] = 'N';
        camera_name[1] = 'o';
        camera_name[2] = 'n';
        camera_name[3] = 'e';
        camera_name[4] = '\0';

        camera->name = camera_name;

        AE_READF( _stream, camera->zoom );
        AE_READF( _stream, camera->fov );

        ae_float_t width = _compositionData->width;
        ae_float_t height = _compositionData->height;

        camera->immutable_property_mask = AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL_CAMERA;
        camera->identity_property_mask = AE_MOVIE_PROPERTY_ANCHOR_POINT_Z | AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL_QUATERNION;

        camera->immutable.position_x = width * 0.5f;
        camera->immutable.position_y = height * 0.5f;
        camera->immutable.position_z = -camera->zoom;

        camera->immutable.target_x = width * 0.5f;
        camera->immutable.target_y = height * 0.5f;
        camera->immutable.target_z = 0.f;

        camera->immutable.quaternion_x = 0.f;
        camera->immutable.quaternion_y = 0.f;
        camera->immutable.quaternion_z = 0.f;
        camera->immutable.quaternion_w = 1.f;

        camera->timeline = AE_NULLPTR;
    }
    else
    {
        AE_READ_STRING( _stream, camera->name );

        AE_READF( _stream, camera->zoom );
        AE_READF( _stream, camera->fov );

        AE_RESULT( ae_movie_load_camera_transformation, (_stream, camera) );
    }

    _compositionData->camera = camera;

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_bool_t __find_movie_data_composition_layer_position_by_index( const aeMovieCompositionData * _compositionData, const aeMovieLayerData * _layers, ae_uint32_t _index, ae_uint32_t * _position )
{
    ae_uint32_t iterator = 0U;

    const aeMovieLayerData * it_layer = _layers;
    const aeMovieLayerData * it_layer_end = _layers + _compositionData->layer_count;
    for( ; it_layer != it_layer_end; ++it_layer )
    {
        const aeMovieLayerData * layer = it_layer;

        if( layer->index == _index )
        {
            *_position = iterator;

            return AE_TRUE;
        }

        ++iterator;
    }

    return AE_FALSE;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __load_movie_property_value( aeMovieStream * _stream, const aeMovieLayerData * _layer, struct aeMoviePropertyValue * _property )
{
    _property->immutable = AE_READB( _stream );

    if( _property->immutable == AE_TRUE )
    {
        AE_READF( _stream, _property->immutable_value );

        _property->values = AE_NULLPTR;
    }
    else
    {
        _property->immutable_value = 0.f;

        ae_float_t * values = AE_NEWN( _stream->instance, ae_float_t, _layer->frame_count );

        AE_RESULT_PANIC_MEMORY( values );

        AE_READN( _stream, values, _layer->frame_count );

        _property->values = values;
    }

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __load_movie_property_color_channel( aeMovieStream * _stream, const aeMovieLayerData * _layer, struct aeMoviePropertyColorChannel * _property )
{
    _property->immutable = AE_READB( _stream );

    if( _property->immutable == AE_TRUE )
    {
        AE_READ_COLOR_CHANNEL( _stream, _property->immutable_value );

        _property->values = AE_NULLPTR;
    }
    else
    {
        _property->immutable_value = 1.f;

        ae_color_channel_t * values = AE_NEWN( _stream->instance, ae_color_channel_t, _layer->frame_count );

        AE_RESULT_PANIC_MEMORY( values );

        AE_READN( _stream, values, _layer->frame_count );

        _property->values = values;
    }

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __load_movie_property_color( aeMovieStream * _stream, const aeMovieLayerData * _layer, struct aeMoviePropertyColor * _property )
{
    const aeMovieInstance * instance = _stream->instance;

    struct aeMoviePropertyColorChannel * color_channel_r = AE_NEW( instance, struct aeMoviePropertyColorChannel );
    AE_RESULT( __load_movie_property_color_channel, (_stream, _layer, color_channel_r) );
    _property->color_channel_r = color_channel_r;

    struct aeMoviePropertyColorChannel * color_channel_g = AE_NEW( instance, struct aeMoviePropertyColorChannel );
    AE_RESULT( __load_movie_property_color_channel, (_stream, _layer, color_channel_g) );
    _property->color_channel_g = color_channel_g;

    struct aeMoviePropertyColorChannel * color_channel_b = AE_NEW( instance, struct aeMoviePropertyColorChannel );
    AE_RESULT( __load_movie_property_color_channel, (_stream, _layer, color_channel_b) );
    _property->color_channel_b = color_channel_b;

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __request_extensions( const aeMovieInstance * _instance, aeMovieLayerExtensions ** _extensions )
{
    if( *_extensions != AE_NULLPTR )
    {
        return AE_RESULT_SUCCESSFUL;
    }

    aeMovieLayerExtensions * extensions = AE_NEW( _instance, aeMovieLayerExtensions );

    AE_RESULT_PANIC_MEMORY( extensions );

    __clear_layer_extensions( extensions );

    *_extensions = extensions;

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __load_movie_data_layer_extension_timeremap( aeMovieLayerData * _layer, aeMovieStream * _stream, const aeMovieInstance * _instance, aeMovieLayerExtensions * _extensions )
{
    aeMovieLayerExtensionTimeremap * layer_timeremap = AE_NEW( _instance, aeMovieLayerExtensionTimeremap );

    AE_RESULT_PANIC_MEMORY( layer_timeremap );

    ae_float_t * times = AE_NEWN( _instance, ae_float_t, _layer->frame_count );

    AE_RESULT_PANIC_MEMORY( times );

    AE_READN( _stream, times, _layer->frame_count );

    layer_timeremap->times = times;

    for( ;; )
    {
        ae_uint8_t params;
        AE_READ( _stream, params );

        switch( params )
        {
        case 0:
            {
            }break;
        default:
            {
                AE_RETURN_ERROR_RESULT( AE_RESULT_INVALID_STREAM );
            }break;
        }

        if( params == 0 )
        {
            break;
        }
    }

    _extensions->timeremap = layer_timeremap;

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __load_movie_data_layer_extension_mesh( aeMovieLayerData * _layer, aeMovieStream * _stream, const aeMovieInstance * _instance, aeMovieLayerExtensions * _extensions )
{
    aeMovieLayerExtensionMesh * layer_mesh = AE_NEW( _instance, aeMovieLayerExtensionMesh );

    AE_RESULT_PANIC_MEMORY( layer_mesh );

    layer_mesh->immutable = AE_READB( _stream );

    if( layer_mesh->immutable == AE_TRUE )
    {
        AE_READ_MESH( _stream, &layer_mesh->immutable_mesh );

        layer_mesh->meshes = AE_NULLPTR;
    }
    else
    {
        ae_mesh_t * meshes = AE_NEWN( _instance, ae_mesh_t, _layer->frame_count );

        AE_RESULT_PANIC_MEMORY( meshes );

        ae_mesh_t * it_mesh = meshes;
        ae_mesh_t * it_mesh_end = meshes + _layer->frame_count;
        for( ; it_mesh != it_mesh_end; ++it_mesh )
        {
            AE_READ_MESH( _stream, it_mesh );
        }

        layer_mesh->meshes = meshes;
    }

    for( ;; )
    {
        ae_uint8_t params;
        AE_READ( _stream, params );

        switch( params )
        {
        case 0:
            {
            }break;
        default:
            {
                AE_RETURN_ERROR_RESULT( AE_RESULT_INVALID_STREAM );
            }break;
        }

        if( params == 0 )
        {
            break;
        }
    }

    _extensions->mesh = layer_mesh;

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __load_movie_data_layer_extension_bezier_warp( aeMovieLayerData * _layer, aeMovieStream * _stream, const aeMovieInstance * _instance, aeMovieLayerExtensions * _extensions )
{
    aeMovieLayerExtensionBezierWarp * layer_bezier_warp = AE_NEW( _instance, aeMovieLayerExtensionBezierWarp );

    AE_RESULT_PANIC_MEMORY( layer_bezier_warp );

    layer_bezier_warp->immutable = AE_READB( _stream );

    if( layer_bezier_warp->immutable == AE_TRUE )
    {
        AE_READN( _stream, layer_bezier_warp->immutable_bezier_warp.corners, 4 );
        AE_READN( _stream, layer_bezier_warp->immutable_bezier_warp.beziers, 8 );

        layer_bezier_warp->bezier_warps = AE_NULLPTR;
    }
    else
    {
        aeMovieBezierWarp * bezier_warps = AE_NEWN( _instance, aeMovieBezierWarp, _layer->frame_count );

        AE_RESULT_PANIC_MEMORY( bezier_warps );

        aeMovieBezierWarp * it_bezier_warp = bezier_warps;
        aeMovieBezierWarp * it_bezier_warp_end = bezier_warps + _layer->frame_count;
        for( ; it_bezier_warp != it_bezier_warp_end; ++it_bezier_warp )
        {
            AE_READN( _stream, it_bezier_warp->corners, 4 );
            AE_READN( _stream, it_bezier_warp->beziers, 8 );
        }

        layer_bezier_warp->bezier_warps = bezier_warps;
    }

    ae_uint8_t quality;
    AE_READ( _stream, quality );

    layer_bezier_warp->quality = quality;

    for( ;; )
    {
        ae_uint8_t params;
        AE_READ( _stream, params );

        switch( params )
        {
        case 0:
            {
            }break;
        default:
            {
                AE_RETURN_ERROR_RESULT( AE_RESULT_INVALID_STREAM );
            }break;
        }

        if( params == 0 )
        {
            break;
        }
    }

    _extensions->bezier_warp = layer_bezier_warp;

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __load_movie_data_layer_extension_polygon( aeMovieLayerData * _layer, aeMovieStream * _stream, const aeMovieInstance * _instance, aeMovieLayerExtensions * _extensions )
{
    AE_UNUSED( _layer );

    aeMovieLayerExtensionPolygon * layer_polygon = AE_NEW( _instance, aeMovieLayerExtensionPolygon );

    AE_RESULT_PANIC_MEMORY( layer_polygon );

    layer_polygon->immutable = AE_READB( _stream );

    if( layer_polygon->immutable == AE_TRUE )
    {
        AE_READ_POLYGON( _stream, &layer_polygon->immutable_polygon );

        layer_polygon->polygons = AE_NULLPTR;
    }
    else
    {
        ae_uint32_t polygon_count = AE_READZ( _stream );

        ae_polygon_t * polygons = AE_NEWN( _instance, ae_polygon_t, polygon_count );

        AE_RESULT_PANIC_MEMORY( polygons );

        ae_polygon_t * it_polygon = polygons;
        ae_polygon_t * it_polygon_end = polygons + polygon_count;
        for( ; it_polygon != it_polygon_end; ++it_polygon )
        {
            AE_READ_POLYGON( _stream, it_polygon );
        }

        layer_polygon->polygons = polygons;
    }

    for( ;; )
    {
        ae_uint8_t params;
        AE_READ( _stream, params );

        switch( params )
        {
        case 0:
            {
            }break;
        default:
            {
                AE_RETURN_ERROR_RESULT( AE_RESULT_INVALID_STREAM );
            }break;
        }

        if( params == 0 )
        {
            break;
        }
    }

    _extensions->polygon = layer_polygon;

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __load_movie_data_layer_extension_shader( aeMovieLayerData * _layer, aeMovieStream * _stream, const aeMovieInstance * _instance, aeMovieLayerExtensions * _extensions )
{
    aeMovieLayerExtensionShader * layer_shader = AE_NEW( _instance, aeMovieLayerExtensionShader );

    AE_RESULT_PANIC_MEMORY( layer_shader );

    AE_READ_STRING( _stream, layer_shader->name );
    AE_READ_STRING( _stream, layer_shader->description );
    AE_READ( _stream, layer_shader->version );
    AE_READ( _stream, layer_shader->flags );

    layer_shader->parameter_count = AE_READZ( _stream );

    const struct aeMovieLayerShaderParameter ** parameters = AE_NEWN( _stream->instance, const struct aeMovieLayerShaderParameter *, layer_shader->parameter_count );

    AE_RESULT_PANIC_MEMORY( parameters );

    const struct aeMovieLayerShaderParameter ** it_parameter = parameters;
    const struct aeMovieLayerShaderParameter ** it_parameter_end = parameters + layer_shader->parameter_count;

    for( ;
        it_parameter != it_parameter_end;
        ++it_parameter )
    {
        ae_uint8_t paramater_type;
        AE_READ( _stream, paramater_type );

        switch( paramater_type )
        {
        case AE_MOVIE_EXTENSION_SHADER_PARAMETER_SLIDER:
            {
                struct aeMovieLayerShaderParameterSlider * parameter_slider = AE_NEW( _stream->instance, struct aeMovieLayerShaderParameterSlider );

                AE_RESULT_PANIC_MEMORY( parameter_slider );

                parameter_slider->type = paramater_type;
                AE_READ_STRING( _stream, parameter_slider->name );
                AE_READ_STRING( _stream, parameter_slider->uniform );

                struct aeMoviePropertyValue * property_value = AE_NEW( _stream->instance, struct aeMoviePropertyValue );

                AE_RESULT_PANIC_MEMORY( property_value );

                AE_RESULT( __load_movie_property_value, (_stream, _layer, property_value) );

                parameter_slider->property_value = property_value;

                *it_parameter = (struct aeMovieLayerShaderParameter *)parameter_slider;
            }break;
        case AE_MOVIE_EXTENSION_SHADER_PARAMETER_ANGLE:
            {
                struct aeMovieLayerShaderParameterAngle * parameter_angle = AE_NEW( _stream->instance, struct aeMovieLayerShaderParameterAngle );

                AE_RESULT_PANIC_MEMORY( parameter_angle );

                parameter_angle->type = paramater_type;
                AE_READ_STRING( _stream, parameter_angle->name );
                AE_READ_STRING( _stream, parameter_angle->uniform );

                struct aeMoviePropertyValue * property_value = AE_NEW( _stream->instance, struct aeMoviePropertyValue );

                AE_RESULT_PANIC_MEMORY( property_value );

                AE_RESULT( __load_movie_property_value, (_stream, _layer, property_value) );

                parameter_angle->property_value = property_value;

                *it_parameter = (struct aeMovieLayerShaderParameter *)parameter_angle;
            }break;
        case AE_MOVIE_EXTENSION_SHADER_PARAMETER_COLOR:
            {
                struct aeMovieLayerShaderParameterColor * parameter_color = AE_NEW( _stream->instance, struct aeMovieLayerShaderParameterColor );

                AE_RESULT_PANIC_MEMORY( parameter_color );

                parameter_color->type = paramater_type;
                AE_READ_STRING( _stream, parameter_color->name );
                AE_READ_STRING( _stream, parameter_color->uniform );

                struct aeMoviePropertyColor * property_color = AE_NEW( _stream->instance, struct aeMoviePropertyColor );

                AE_RESULT_PANIC_MEMORY( property_color );

                AE_RESULT( __load_movie_property_color, (_stream, _layer, property_color) );

                parameter_color->property_color = property_color;

                *it_parameter = (struct aeMovieLayerShaderParameter *)parameter_color;
            }break;
        case AE_MOVIE_EXTENSION_SHADER_PARAMETER_TIME:
            {
                struct aeMovieLayerShaderParameterTime * parameter_time = AE_NEW( _stream->instance, struct aeMovieLayerShaderParameterTime );

                AE_RESULT_PANIC_MEMORY( parameter_time );

                parameter_time->type = paramater_type;
                AE_READ_STRING( _stream, parameter_time->name );
                AE_READ_STRING( _stream, parameter_time->uniform );

                AE_READ( _stream, parameter_time->scale );

                *it_parameter = (struct aeMovieLayerShaderParameter *)parameter_time;
            }break;
        }
    }

    layer_shader->parameters = parameters;

    for( ;; )
    {
        ae_uint8_t params;
        AE_READ( _stream, params );

        switch( params )
        {
        case 0:
            {
            }break;
        default:
            {
                AE_RETURN_ERROR_RESULT( AE_RESULT_INVALID_STREAM );
            }break;
        }

        if( params == 0 )
        {
            break;
        }
    }

    _extensions->shader = layer_shader;

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __load_movie_data_layer_extension_viewport( aeMovieLayerData * _layer, aeMovieStream * _stream, const aeMovieInstance * _instance, aeMovieLayerExtensions * _extensions )
{
    AE_UNUSED( _layer );

    aeMovieLayerExtensionViewport * layer_viewport = AE_NEW( _instance, aeMovieLayerExtensionViewport );

    AE_RESULT_PANIC_MEMORY( layer_viewport );

    ae_magic_read_viewport( _stream, &layer_viewport->viewport );

    for( ;; )
    {
        ae_uint8_t params;
        AE_READ( _stream, params );

        switch( params )
        {
        case 0:
            {
            }break;
        default:
            {
                AE_RETURN_ERROR_RESULT( AE_RESULT_INVALID_STREAM );
            }break;
        }

        if( params == 0 )
        {
            break;
        }
    }

    _extensions->viewport = layer_viewport;

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __load_movie_data_layer_extension_volume( aeMovieLayerData * _layer, aeMovieStream * _stream, const aeMovieInstance * _instance, aeMovieLayerExtensions * _extensions )
{
    aeMovieLayerExtensionVolume * layer_volume = AE_NEW( _instance, aeMovieLayerExtensionVolume );

    AE_RESULT_PANIC_MEMORY( layer_volume );

    struct aeMoviePropertyValue * property_volume = AE_NEW( _stream->instance, struct aeMoviePropertyValue );

    AE_RESULT_PANIC_MEMORY( property_volume );

    AE_RESULT( __load_movie_property_value, (_stream, _layer, property_volume) );

    layer_volume->property_volume = property_volume;

    for( ;; )
    {
        ae_uint8_t params;
        AE_READ( _stream, params );

        switch( params )
        {
        case 0:
            {
            }break;
        default:
            {
                AE_RETURN_ERROR_RESULT( AE_RESULT_INVALID_STREAM );
            }break;
        }

        if( params == 0 )
        {
            break;
        }
    }

    _extensions->volume = layer_volume;

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __load_movie_data_layer_extensions( aeMovieLayerData * _layer, aeMovieStream * _stream, const aeMovieInstance * _instance )
{
    aeMovieLayerExtensions * layer_extensions = AE_NULLPTR;

    for( ;; )
    {
        ae_uint8_t extension;
        AE_READ( _stream, extension );

        switch( extension )
        {
        case 0:
            {
            }break;
        case AE_LAYER_EXTENSION_TIMEREMAP:
            {
                AE_RESULT( __request_extensions, (_instance, &layer_extensions) );

                AE_RESULT( __load_movie_data_layer_extension_timeremap, (_layer, _stream, _instance, layer_extensions) );
            }break;
        case AE_LAYER_EXTENSION_MESH:
            {
                AE_RESULT( __request_extensions, (_instance, &layer_extensions) );

                AE_RESULT( __load_movie_data_layer_extension_mesh, (_layer, _stream, _instance, layer_extensions) );
            }break;
        case AE_LAYER_EXTENSION_BEZIERWARP:
            {
                AE_RESULT( __request_extensions, (_instance, &layer_extensions) );

                AE_RESULT( __load_movie_data_layer_extension_bezier_warp, (_layer, _stream, _instance, layer_extensions) );
            }break;
        case AE_LAYER_EXTENSION_POLYGON:
            {
                AE_RESULT( __request_extensions, (_instance, &layer_extensions) );

                AE_RESULT( __load_movie_data_layer_extension_polygon, (_layer, _stream, _instance, layer_extensions) );
            }break;
        case AE_LAYER_EXTENSION_SHADER:
            {
                AE_RESULT( __request_extensions, (_instance, &layer_extensions) );

                AE_RESULT( __load_movie_data_layer_extension_shader, (_layer, _stream, _instance, layer_extensions) );
            }break;
        case AE_LAYER_EXTENSION_VIEWPORT:
            {
                AE_RESULT( __request_extensions, (_instance, &layer_extensions) );

                AE_RESULT( __load_movie_data_layer_extension_viewport, (_layer, _stream, _instance, layer_extensions) );
            }break;
        case AE_LAYER_EXTENSION_VOLUME:
            {
                AE_RESULT( __request_extensions, (_instance, &layer_extensions) );

                AE_RESULT( __load_movie_data_layer_extension_volume, (_layer, _stream, _instance, layer_extensions) );
            }break;
        default:
            {
                AE_RETURN_ERROR_RESULT( AE_RESULT_INVALID_STREAM );
            }break;
        }

        if( extension == 0 )
        {
            break;
        }
    }

    if( layer_extensions == AE_NULLPTR )
    {
        _layer->extensions = &_instance->layer_extensions_default;
    }
    else
    {
        _layer->extensions = layer_extensions;
    }

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __setup_movie_data_layer_renderable( aeMovieLayerData * _layer )
{
    if( _layer->is_track_matte == AE_TRUE )
    {
        _layer->renderable = AE_FALSE;
    }
    else
    {
        aeMovieLayerTypeEnum layer_type = _layer->type;

        switch( layer_type )
        {
        case AE_MOVIE_LAYER_TYPE_MOVIE:
            {
                _layer->renderable = AE_FALSE;
            }break;
        case AE_MOVIE_LAYER_TYPE_SPRITE:
            {
                _layer->renderable = AE_TRUE;
            }break;
        case AE_MOVIE_LAYER_TYPE_TEXT:
            {
                _layer->renderable = AE_TRUE;
            }break;
        case AE_MOVIE_LAYER_TYPE_EVENT:
            {
                _layer->renderable = AE_FALSE;
            }break;
        case AE_MOVIE_LAYER_TYPE_SOCKET:
            {
                _layer->renderable = AE_TRUE;
            }break;
        case AE_MOVIE_LAYER_TYPE_SHAPE:
            {
                _layer->renderable = AE_TRUE;
            }break;
        case AE_MOVIE_LAYER_TYPE_SLOT:
            {
                _layer->renderable = AE_TRUE;
            }break;
        case AE_MOVIE_LAYER_TYPE_NULL:
            {
                _layer->renderable = AE_FALSE;
            }break;
        case AE_MOVIE_LAYER_TYPE_SCENE_EFFECT:
            {
                _layer->renderable = AE_FALSE;
            }break;
        case AE_MOVIE_LAYER_TYPE_SOLID:
            {
                _layer->renderable = AE_TRUE;
            }break;
        case AE_MOVIE_LAYER_TYPE_SEQUENCE:
            {
                _layer->renderable = AE_TRUE;
            }break;
        case AE_MOVIE_LAYER_TYPE_VIDEO:
            {
                _layer->renderable = AE_TRUE;
            }break;
        case AE_MOVIE_LAYER_TYPE_SOUND:
            {
                _layer->renderable = AE_FALSE;
            }break;
        case AE_MOVIE_LAYER_TYPE_PARTICLE:
            {
                _layer->renderable = AE_TRUE;
            }break;
        case AE_MOVIE_LAYER_TYPE_IMAGE:
            {
                _layer->renderable = AE_TRUE;
            }break;
        case AE_MOVIE_LAYER_TYPE_SUB_MOVIE:
            {
                _layer->renderable = AE_FALSE;
            }break;
        default:
            {
                AE_RETURN_ERROR_RESULT( AE_RESULT_INVALID_DATA );
            }break;
        }
    }

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __load_movie_data_layer( const aeMovieData * _movieData, const aeMovieCompositionData * _compositions, aeMovieStream * _stream, aeMovieLayerData * _layer )
{
    const aeMovieInstance * instance = _movieData->instance;

    AE_READ_STRING( _stream, _layer->name );

    _layer->index = AE_READZ( _stream );

    _layer->is_track_matte = AE_READB( _stream );
    _layer->has_track_matte = AE_READB( _stream );

    if( _layer->has_track_matte == AE_TRUE )
    {
        _layer->track_matte_mode = AE_READ8( _stream );
    }
    else
    {
        _layer->track_matte_mode = AE_MOVIE_TRACK_MATTE_NONE;
    }

    ae_uint8_t type;
    AE_READ( _stream, type );
    _layer->type = type;

    _layer->frame_count = AE_READZ( _stream );

    AE_RESULT( __load_movie_data_layer_extensions, (_layer, _stream, instance) );

    ae_bool_t is_resource_or_composition = AE_READB( _stream );

    if( is_resource_or_composition == AE_TRUE )
    {
        ae_uint32_t resource_index = AE_READZ( _stream );

        if( resource_index == 0 )
        {
            _layer->resource = AE_NULLPTR;
        }
        else
        {
            _layer->resource = _movieData->resources[resource_index - 1];
        }

        _layer->subcomposition_data = AE_NULLPTR;
    }
    else
    {
        ae_uint32_t composition_index = AE_READZ( _stream );
        _layer->subcomposition_data = _compositions + composition_index;

        _layer->resource = AE_NULLPTR;
    }

    ae_uint32_t parent_index = AE_READZ( _stream );

    _layer->parent_index = parent_index;


    AE_READF( _stream, _layer->in_time );
    AE_READF( _stream, _layer->out_time );
    AE_READF( _stream, _layer->start_time );
    AE_READF( _stream, _layer->finish_time );

    _layer->reverse_time = AE_READB( _stream );
    _layer->trimmed_time = AE_READB( _stream );

    ae_uint8_t blend_mode;
    AE_READ( _stream, blend_mode );
    _layer->blend_mode = blend_mode;

    _layer->threeD = AE_READB( _stream );

    _layer->incessantly = AE_FALSE;

    _layer->options_count = 0U;

    for( ;;)
    {
        ae_uint32_t option_value;
        AE_READ( _stream, option_value );

        if( option_value == 0U )
        {
            break;
        }
        else if( option_value == AE_OPTION( 'l', 'o', 'o', 'p' ) )
        {
            _layer->incessantly = AE_TRUE;
        }
        else
        {
            if( _layer->options_count == AE_MOVIE_LAYER_MAX_OPTIONS )
            {
                return AE_RESULT_INVALID_DATA;
            }

            _layer->options[_layer->options_count] = option_value;
            _layer->options_count++;
        }
    }

    _layer->play_count = AE_READZ( _stream );

    AE_READF( _stream, _layer->stretch );

    aeMovieLayerTransformation * transformation = AE_NULLPTR;

    if( _layer->threeD == AE_FALSE )
    {
        transformation = (aeMovieLayerTransformation *)AE_NEW( instance, aeMovieLayerTransformation2D );

        AE_RESULT_PANIC_MEMORY( transformation );
    }
    else
    {
        transformation = (aeMovieLayerTransformation *)AE_NEW( instance, aeMovieLayerTransformation3D );

        AE_RESULT_PANIC_MEMORY( transformation );
    }

    AE_RESULT( ae_movie_load_layer_transformation, (_stream, transformation, _layer->threeD) );

    _layer->transformation = transformation;

    AE_RESULT( __setup_movie_data_layer_renderable, (_layer) );

    _layer->cache = AE_NULLPTR;

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __load_movie_data_composition_layers( const aeMovieData * _movieData, const aeMovieCompositionData * _compositions, aeMovieLayerData * _layers, aeMovieStream * _stream, aeMovieCompositionData * _compositionData )
{
    aeMovieLayerData * it_layer = _layers;
    aeMovieLayerData * it_layer_end = _layers + _compositionData->layer_count;
    for( ; it_layer != it_layer_end; ++it_layer )
    {
        aeMovieLayerData * layer = it_layer;

        layer->composition_data = _compositionData;

        AE_RESULT( __load_movie_data_layer, (_movieData, _compositions, _stream, layer) );
    }

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __setup_movie_data_layer_track_matte( const aeMovieCompositionData * _compositionData, const aeMovieLayerData * _layers, aeMovieLayerData * _layer )
{
    if( _layer->has_track_matte == AE_TRUE )
    {
        ae_uint32_t layer_position;
        if( __find_movie_data_composition_layer_position_by_index( _compositionData, _layers, _layer->index, &layer_position ) == AE_FALSE )
        {
            AE_RETURN_ERROR_RESULT( AE_RESULT_INVALID_DATA );
        }

        if( layer_position + 1 >= _compositionData->layer_count )
        {
            AE_RETURN_ERROR_RESULT( AE_RESULT_INVALID_DATA );
        }

        _layer->track_matte_layer = _compositionData->layers + layer_position + 1U;
    }
    else
    {
        _layer->track_matte_layer = AE_NULLPTR;
    }

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __setup_movie_data_composition_layers( const aeMovieCompositionData * _compositionData, aeMovieLayerData * _layers )
{
    aeMovieLayerData * it_layer = _layers;
    aeMovieLayerData * it_layer_end = _layers + _compositionData->layer_count;
    for( ; it_layer != it_layer_end; ++it_layer )
    {
        aeMovieLayerData * layer = it_layer;

        AE_RESULT( __setup_movie_data_layer_track_matte, (_compositionData, _layers, layer) );
    }

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __callback_cache_uv_provider( const aeMovieData * _movieData, ae_userdataptr_t _cu, const aeMovieResource * _resource, ae_uint32_t _count, const ae_vector2_t * _uvs )
{
    aeMovieDataCacheUVProviderCallbackData callbackData;
    callbackData.resource = _resource;
    callbackData.vertex_count = _count;
    callbackData.uvs = _uvs;

    if( (*_movieData->providers.cache_uv_provider)(&callbackData, _cu, _movieData->provider_userdata) == AE_FALSE )
    {
        AE_RETURN_ERROR_RESULT( AE_RESULT_INTERNAL_ERROR );
    }

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __setup_movie_data_layer_cache( const aeMovieData * _movieData, aeMovieLayerData * _layer )
{
    const aeMovieInstance * instance = _movieData->instance;

    aeMovieResourceTypeEnum resource_type = _layer->resource->type;

    switch( resource_type )
    {
    case AE_MOVIE_RESOURCE_IMAGE:
        {
            if( _layer->extensions->mesh != AE_NULLPTR )
            {
                struct aeMovieLayerCache * cache = AE_NEW( instance, struct aeMovieLayerCache );

                cache->immutable_mesh_uv_cache_userdata = AE_NULLPTR;
                cache->mesh_uv_cache_userdata = AE_NULLPTR;

                if( _layer->extensions->mesh->immutable == AE_TRUE )
                {
                    const ae_mesh_t * immutable_mesh = &_layer->extensions->mesh->immutable_mesh;

                    ae_userdata_t uv_cache_userdata = AE_USERDATA_NULL;
                    AE_RESULT( __callback_cache_uv_provider, (_movieData, &uv_cache_userdata, _layer->resource, immutable_mesh->vertex_count, immutable_mesh->uvs) );

                    cache->immutable_mesh_uv_cache_userdata = uv_cache_userdata;
                }
                else
                {
                    ae_uint32_t layer_frame_count = _layer->frame_count;

                    ae_userdata_t * mesh_uv_cache_userdata = AE_NEWN( instance, ae_userdata_t, layer_frame_count );

                    ae_uint32_t index = 0;
                    for( ; index != layer_frame_count; ++index )
                    {
                        const ae_mesh_t * mesh = _layer->extensions->mesh->meshes + index;

                        if( mesh->vertex_count == 0 )
                        {
                            mesh_uv_cache_userdata[index] = AE_NULLPTR;

                            continue;
                        }

                        ae_userdata_t uv_cache_userdata = AE_USERDATA_NULL;
                        AE_RESULT( __callback_cache_uv_provider, (_movieData, &uv_cache_userdata, _layer->resource, mesh->vertex_count, mesh->uvs) );

                        mesh_uv_cache_userdata[index] = uv_cache_userdata;
                    }

                    cache->mesh_uv_cache_userdata = mesh_uv_cache_userdata;
                }

                _layer->cache = cache;
            }
        }break;
    default:
        {
        }break;
    }

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __setup_movie_data_composition_cache( const aeMovieData * _movieData, const aeMovieCompositionData * _compositionData, aeMovieLayerData * _layers )
{
    aeMovieLayerData * it_layer = _layers;
    aeMovieLayerData * it_layer_end = _layers + _compositionData->layer_count;
    for( ; it_layer != it_layer_end; ++it_layer )
    {
        aeMovieLayerData * layer = it_layer;

        if( layer->resource == AE_NULLPTR )
        {
            continue;
        }

        AE_RESULT( __setup_movie_data_layer_cache, (_movieData, layer) );
    }

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __load_movie_data_composition( const aeMovieData * _movieData, const aeMovieCompositionData * _compositions, aeMovieStream * _stream, aeMovieCompositionData * _compositionData, ae_bool_t _cache_uv_available )
{
    const aeMovieInstance * instance = _movieData->instance;

    AE_READ_STRING( _stream, _compositionData->name );

    _compositionData->master = AE_READB( _stream );

    AE_READF( _stream, _compositionData->width );
    AE_READF( _stream, _compositionData->height );

    AE_READF( _stream, _compositionData->duration_time );
    AE_READF( _stream, _compositionData->frameDuration );
    AE_READF( _stream, _compositionData->frameDurationInv );

    _compositionData->duration_frame = (ae_uint32_t)(_compositionData->duration_time * _compositionData->frameDurationInv + 0.5f);

    _compositionData->flags = 0;

    _compositionData->loop_segment[0] = 0.f;
    _compositionData->loop_segment[1] = _compositionData->duration_time;

    _compositionData->anchor_point[0] = 0.f;
    _compositionData->anchor_point[1] = 0.f;
    _compositionData->anchor_point[2] = 0.f;

    _compositionData->offset_point[0] = 0.f;
    _compositionData->offset_point[1] = 0.f;
    _compositionData->offset_point[2] = 0.f;

    _compositionData->bounds.begin_x = 0.f;
    _compositionData->bounds.begin_y = 0.f;
    _compositionData->bounds.end_x = 0.f;
    _compositionData->bounds.end_y = 0.f;

    _compositionData->camera = AE_NULLPTR;

    for( ;; )
    {
        ae_uint8_t flag;
        AE_READ( _stream, flag );

        switch( flag )
        {
        case 0:
            {
            }break;
        case 1:
            {
                AE_READF2( _stream, _compositionData->loop_segment );

                _compositionData->flags |= AE_MOVIE_COMPOSITION_LOOP_SEGMENT;
            }break;
        case 2:
            {
                AE_READF3( _stream, _compositionData->anchor_point );

                _compositionData->flags |= AE_MOVIE_COMPOSITION_ANCHOR_POINT;
            }break;
        case 3:
            {
                AE_READF3( _stream, _compositionData->offset_point );

                _compositionData->flags |= AE_MOVIE_COMPOSITION_OFFSET_POINT;
            }break;
        case 4:
            {
                AE_READF4( _stream, _compositionData->bounds );

                _compositionData->flags |= AE_MOVIE_COMPOSITION_BOUNDS;
            }break;
        case 5:
            {
                AE_RESULT( __load_movie_data_composition_camera, (_stream, _compositionData) );

                _compositionData->flags |= AE_MOVIE_COMPOSITION_CAMERA;
            }break;
        default:
            {
                AE_RETURN_ERROR_RESULT( AE_RESULT_INVALID_STREAM );
            }break;
        };

        if( flag == 0 )
        {
            break;
        }
    }

    ae_uint32_t layer_count = AE_READZ( _stream );

    _compositionData->layer_count = layer_count;
    aeMovieLayerData * layers = AE_NEWN( instance, aeMovieLayerData, layer_count );

    AE_RESULT_PANIC_MEMORY( layers );

    AE_RESULT( __load_movie_data_composition_layers, (_movieData, _compositions, layers, _stream, _compositionData) );

    AE_RESULT( __setup_movie_data_composition_layers, (_compositionData, layers) );

    if( _cache_uv_available == AE_TRUE )
    {
        AE_RESULT( __setup_movie_data_composition_cache, (_movieData, _compositionData, layers) );
    }

    _compositionData->layers = layers;

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
const aeMovieInstance * ae_get_movie_data_instance( const aeMovieData * _movieData )
{
    const aeMovieInstance * instance = _movieData->instance;

    return instance;
}
//////////////////////////////////////////////////////////////////////////
aeMovieStream * ae_create_movie_stream( const aeMovieInstance * _instance, ae_movie_stream_memory_read_t _read, ae_movie_stream_memory_copy_t _copy, ae_userdata_t _userdata )
{
#ifdef AE_MOVIE_DEBUG
    if( _instance == AE_NULLPTR )
    {
        return AE_NULLPTR;
    }

    if( _read == AE_NULLPTR )
    {
        return AE_NULLPTR;
    }

    if( _copy == AE_NULLPTR )
    {
        return AE_NULLPTR;
    }
#endif

    aeMovieStream * stream = AE_NEW( _instance, aeMovieStream );

    AE_MOVIE_PANIC_MEMORY( stream, AE_NULLPTR );

    stream->instance = _instance;
    stream->memory_read = _read;
    stream->memory_copy = _copy;
    stream->read_userdata = _userdata;
    stream->copy_userdata = _userdata;

    stream->buffer = AE_NULLPTR;
    stream->carriage = 0U;

    return stream;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_size_t __movie_read_buffer( ae_userdata_t _userdata, ae_voidptr_t _buff, ae_size_t _carriage, ae_size_t _size )
{
    aeMovieStream * stream = (aeMovieStream *)_userdata;

    stream->memory_copy( stream->copy_userdata, (ae_constbyteptr_t)stream->buffer + _carriage, _buff, _size );

    return _size;
}
//////////////////////////////////////////////////////////////////////////
aeMovieStream * ae_create_movie_stream_memory( const aeMovieInstance * _instance, ae_constvoidptr_t _buffer, ae_movie_stream_memory_copy_t _copy, ae_userdata_t _userdata )
{
#ifdef AE_MOVIE_DEBUG
    if( _instance == AE_NULLPTR )
    {
        return AE_NULLPTR;
    }

    if( _copy == AE_NULLPTR )
    {
        return AE_NULLPTR;
    }
#endif

    aeMovieStream * stream = AE_NEW( _instance, aeMovieStream );

    AE_MOVIE_PANIC_MEMORY( stream, AE_NULLPTR );

    stream->instance = _instance;
    stream->memory_read = &__movie_read_buffer;
    stream->memory_copy = _copy;
    stream->read_userdata = stream;
    stream->copy_userdata = _userdata;

    stream->buffer = _buffer;
    stream->carriage = 0U;

    return stream;
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_delete_movie_stream( const aeMovieStream * _stream )
{
    AE_DELETE( _stream->instance, _stream );
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __check_movie_data( aeMovieStream * _stream, ae_uint32_t * _major, ae_uint32_t * _minor )
{
    ae_uint8_t magic[4];
    AE_READN( _stream, magic, 4 );

    if( magic[0] != 'A' ||
        magic[1] != 'E' ||
        magic[2] != 'M' ||
        magic[3] != '1' )
    {
        AE_RETURN_ERROR_RESULT( AE_RESULT_INVALID_MAGIC );
    }

    ae_uint32_t major_version;
    AE_READ( _stream, major_version );

    *_major = major_version;

    if( major_version != AE_MOVIE_SDK_MAJOR_VERSION )
    {
        AE_RETURN_ERROR_RESULT( AE_RESULT_INVALID_VERSION );
    }

    ae_uint32_t minor_version;
    AE_READ( _stream, minor_version );

    *_minor = minor_version;

    if( minor_version != AE_MOVIE_SDK_MINOR_VERSION )
    {
        AE_RETURN_ERROR_RESULT( AE_RESULT_INVALID_VERSION );
    }

    ae_uint32_t hash_crc;
    AE_READ( _stream, hash_crc );

    ae_uint32_t ae_movie_hashmask_crc =
        _stream->instance->hashmask[0] ^
        _stream->instance->hashmask[1] ^
        _stream->instance->hashmask[2] ^
        _stream->instance->hashmask[3] ^
        _stream->instance->hashmask[4];

    if( hash_crc != ae_movie_hashmask_crc )
    {
        AE_RETURN_ERROR_RESULT( AE_RESULT_INVALID_HASH );
    }

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
ae_result_t ae_check_movie_data( aeMovieStream * _stream, ae_uint32_t * _major, ae_uint32_t * _minor )
{
    ae_result_t result = __check_movie_data( _stream, _major, _minor );

    return result;
}
//////////////////////////////////////////////////////////////////////////
ae_uint32_t ae_get_movie_sdk_major_version( void )
{
    return AE_MOVIE_SDK_MAJOR_VERSION;
}
//////////////////////////////////////////////////////////////////////////
ae_uint32_t ae_get_movie_sdk_minor_version( void )
{
    return AE_MOVIE_SDK_MINOR_VERSION;
}
//////////////////////////////////////////////////////////////////////////
const ae_char_t * ae_get_result_string_info( ae_result_t _result )
{
    switch( _result )
    {
    case AE_RESULT_SUCCESSFUL:
        {
            return "successful";
        }break;
    case AE_RESULT_INVALID_MAGIC:
        {
            return "invalid magic number";
        }break;
    case AE_RESULT_INVALID_VERSION:
        {
            return "invalid version";
        }break;
    case AE_RESULT_INVALID_HASH:
        {
            return "invalid hash";
        }break;
    case AE_RESULT_INVALID_STREAM:
        {
            return "invalid stream";
        }break;
    case AE_RESULT_INVALID_DATA:
        {
            return "invalid data";
        }break;
    case AE_RESULT_INVALID_MEMORY:
        {
            return "invalid memory";
        }break;
    case AE_RESULT_INTERNAL_ERROR:
        {
            return "internal error";
        }break;
    }

    return "invalid result";
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_bool_t __dummy_movie_data_callback_resource_provider( const aeMovieResource * _resource, ae_userdataptr_t _rd, ae_userdata_t _ud )
{
    AE_UNUSED( _resource );
    AE_UNUSED( _ud );

    *_rd = AE_NULLPTR;

    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __dummy_movie_data_callback_resource_deleter( aeMovieResourceTypeEnum _type, ae_voidptr_t _data, ae_userdata_t _ud )
{
    AE_UNUSED( _type );
    AE_UNUSED( _data );
    AE_UNUSED( _ud );
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_bool_t __dummy_movie_data_callback_cache_uv_available( const aeMovieDataCacheUVAvailableCallbackData * _callbackData, ae_userdata_t _ud )
{
    AE_UNUSED( _callbackData );
    AE_UNUSED( _ud );

    return AE_FALSE;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_bool_t __dummy_movie_data_callback_cache_uv_provider( const aeMovieDataCacheUVProviderCallbackData * _callbackData, ae_userdataptr_t _rd, ae_userdata_t _ud )
{
    AE_UNUSED( _callbackData );
    AE_UNUSED( _ud );

    *_rd = AE_NULLPTR;

    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __dummy_movie_data_callback_cache_uv_deleter( const aeMovieDataCacheUVDeleterCallbackData * _callbackData, ae_userdata_t _ud )
{
    AE_UNUSED( _callbackData );
    AE_UNUSED( _ud );
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_clear_movie_data_providers( aeMovieDataProviders * _providers )
{
    _providers->resource_provider = &__dummy_movie_data_callback_resource_provider;
    _providers->resource_deleter = &__dummy_movie_data_callback_resource_deleter;
    _providers->cache_uv_available = &__dummy_movie_data_callback_cache_uv_available;
    _providers->cache_uv_provider = &__dummy_movie_data_callback_cache_uv_provider;
    _providers->cache_uv_deleter = &__dummy_movie_data_callback_cache_uv_deleter;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __load_movie_resource_solid( const aeMovieInstance * _instance, aeMovieStream * _stream, const aeMovieResource ** _atlases, const aeMovieResource ** _resources, aeMovieResource ** _out )
{
    AE_UNUSED( _atlases );
    AE_UNUSED( _resources );

    aeMovieResourceSolid * resource = AE_NEW( _instance, aeMovieResourceSolid );

    AE_RESULT_PANIC_MEMORY( resource );

    AE_READF( _stream, resource->width );
    AE_READF( _stream, resource->height );
    AE_READ_COLOR( _stream, &resource->color );

    for( ;;)
    {
        ae_uint8_t param_type;
        AE_READ( _stream, param_type );

        switch( param_type )
        {
        case 0:
            {
            }break;
        default:
            {
                AE_RETURN_ERROR_RESULT( AE_RESULT_INVALID_STREAM );
            }
        }

        if( param_type == 0 )
        {
            break;
        }
    }

    *_out = (aeMovieResource *)resource;

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __load_movie_resource_video( const aeMovieInstance * _instance, aeMovieStream * _stream, const aeMovieResource ** _atlases, const aeMovieResource ** _resources, aeMovieResource ** _out )
{
    AE_UNUSED( _atlases );
    AE_UNUSED( _resources );

    aeMovieResourceVideo * resource = AE_NEW( _instance, aeMovieResourceVideo );

    AE_RESULT_PANIC_MEMORY( resource );

    AE_READ_STRING( _stream, resource->path );
    resource->codec = AE_READ8( _stream );

    //TODO 17.6
    //AE_READ( _stream, resource->options );
    resource->options = 0;

    AE_READF( _stream, resource->base_width );
    AE_READF( _stream, resource->base_height );

    resource->has_alpha_channel = AE_READB( _stream );

    AE_READF( _stream, resource->frameRate );
    AE_READF( _stream, resource->duration );

    resource->trim_width = resource->base_width;
    resource->trim_height = resource->base_height;
    resource->offset_x = 0.f;
    resource->offset_y = 0.f;

    resource->cache = AE_NULLPTR;

    for( ;;)
    {
        ae_uint8_t param_type;
        AE_READ( _stream, param_type );

        switch( param_type )
        {
        case 0:
            {
            }break;
        case 1:
            {
                AE_READF( _stream, resource->trim_width );
                AE_READF( _stream, resource->trim_height );
                AE_READF( _stream, resource->offset_x );
                AE_READF( _stream, resource->offset_y );
            }break;
        default:
            {
                AE_RETURN_ERROR_RESULT( AE_RESULT_INVALID_STREAM );
            }
        }

        if( param_type == 0 )
        {
            break;
        }
    }

    *_out = (aeMovieResource *)resource;

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __load_movie_resource_sound( const aeMovieInstance * _instance, aeMovieStream * _stream, const aeMovieResource ** _atlases, const aeMovieResource ** _resources, aeMovieResource ** _out )
{
    AE_UNUSED( _atlases );
    AE_UNUSED( _resources );

    aeMovieResourceSound * resource = AE_NEW( _instance, aeMovieResourceSound );

    AE_RESULT_PANIC_MEMORY( resource );

    AE_READ_STRING( _stream, resource->path );
    resource->codec = AE_READ8( _stream );

    AE_READF( _stream, resource->duration );

    for( ;;)
    {
        ae_uint8_t param_type;
        AE_READ( _stream, param_type );

        switch( param_type )
        {
        case 0:
            {
            }break;
        default:
            {
                AE_RETURN_ERROR_RESULT( AE_RESULT_INVALID_STREAM );
            }
        }

        if( param_type == 0 )
        {
            break;
        }
    }

    *_out = (aeMovieResource *)resource;

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __load_movie_resource_image( const aeMovieInstance * _instance, aeMovieStream * _stream, const aeMovieResource ** _atlases, const aeMovieResource ** _resources, aeMovieResource ** _out )
{
    AE_UNUSED( _resources );

    aeMovieResourceImage * resource = AE_NEW( _instance, aeMovieResourceImage );

    AE_RESULT_PANIC_MEMORY( resource );

    AE_READ_STRING( _stream, resource->path );
    resource->codec = AE_READ8( _stream );

    AE_READ( _stream, resource->options );
    resource->atlas_image = AE_NULLPTR;
    resource->atlas_rotate = AE_FALSE;

    AE_READF( _stream, resource->base_width );
    AE_READF( _stream, resource->base_height );

    resource->trim_width = resource->base_width;
    resource->trim_height = resource->base_height;
    resource->offset_x = 0.f;
    resource->offset_y = 0.f;
    resource->uvs = _instance->sprite_uv;

    ae_uint32_t quality_default_bezier_warp = 0;
    for( ; quality_default_bezier_warp != AE_MOVIE_BEZIER_MAX_QUALITY; ++quality_default_bezier_warp )
    {
        const ae_vector2_t * uvs = _instance->bezier_warp_uvs[quality_default_bezier_warp];

        resource->bezier_warp_uvs[quality_default_bezier_warp] = uvs;
    }

    resource->mesh = AE_NULLPTR;
    resource->cache = AE_NULLPTR;

    for( ;;)
    {
        ae_uint8_t param_type;
        AE_READ( _stream, param_type );

        switch( param_type )
        {
        case 0:
            {
            }break;
        case 1:
            {
                AE_READF( _stream, resource->trim_width );
                AE_READF( _stream, resource->trim_height );
                AE_READF( _stream, resource->offset_x );
                AE_READF( _stream, resource->offset_y );
            }break;
        case 2:
            {
                ae_vector2_t * uv = AE_NEWN( _instance, ae_vector2_t, 4 );

                AE_RESULT_PANIC_MEMORY( uv );

                AE_READF2( _stream, uv[0] );
                AE_READF2( _stream, uv[1] );
                AE_READF2( _stream, uv[2] );
                AE_READF2( _stream, uv[3] );

                resource->uvs = (const ae_vector2_t *)uv;

                ae_float_t u_base = resource->uvs[0][0];
                ae_float_t v_base = resource->uvs[0][1];

                ae_float_t u_width = resource->uvs[1][0] - u_base;
                ae_float_t v_width = resource->uvs[3][1] - v_base;

                ae_uint16_t quality_bezier_warp = 0U;
                for( ; quality_bezier_warp != AE_MOVIE_BEZIER_MAX_QUALITY; ++quality_bezier_warp )
                {
                    ae_uint32_t vertex_count = get_bezier_warp_vertex_count( quality_bezier_warp );

                    ae_vector2_t * bezier_warp_uvs = AE_NEWN( _instance, ae_vector2_t, vertex_count );

                    const ae_vector2_t * uvs = _instance->bezier_warp_uvs[quality_bezier_warp];

                    ae_uint32_t index_vertex = 0U;
                    for( ; index_vertex != vertex_count; ++index_vertex )
                    {
                        bezier_warp_uvs[index_vertex][0] = u_base + uvs[index_vertex][0] * u_width;
                        bezier_warp_uvs[index_vertex][1] = v_base + uvs[index_vertex][1] * v_width;
                    }

                    resource->bezier_warp_uvs[quality_bezier_warp] = (const ae_vector2_t *)bezier_warp_uvs;
                }
            }break;
        case 3:
            {
                ae_mesh_t * mesh = AE_NEW( _instance, ae_mesh_t );

                AE_RESULT_PANIC_MEMORY( mesh );

                AE_READ_MESH( _stream, mesh );

                resource->mesh = mesh;
            }break;
        case 4:
            {
                ae_uint32_t atlas_id = AE_READZ( _stream );

                resource->atlas_image = (const aeMovieResourceImage *)_atlases[atlas_id];

                resource->atlas_rotate = AE_READB( _stream );
            }break;
        default:
            {
                AE_RETURN_ERROR_RESULT( AE_RESULT_INVALID_STREAM );
            }
        }

        if( param_type == 0 )
        {
            break;
        }
    }

    *_out = (aeMovieResource *)resource;

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __load_movie_resource_sequence( const aeMovieInstance * _instance, aeMovieStream * _stream, const aeMovieResource ** _atlases, const aeMovieResource ** _resources, aeMovieResource ** _out )
{
    AE_UNUSED( _atlases );

    aeMovieResourceSequence * resource = AE_NEW( _instance, aeMovieResourceSequence );

    AE_RESULT_PANIC_MEMORY( resource );

    AE_READF( _stream, resource->frameDurationInv );

    ae_uint32_t image_count = AE_READZ( _stream );

    resource->image_count = image_count;
    const aeMovieResourceImage ** images = AE_NEWN( _instance, const aeMovieResourceImage *, image_count );

    AE_RESULT_PANIC_MEMORY( images );

    const aeMovieResourceImage ** it_image = images;
    const aeMovieResourceImage ** it_image_end = images + image_count;
    for( ; it_image != it_image_end; ++it_image )
    {
        ae_uint32_t resource_id = AE_READZ( _stream );

        *it_image = (const aeMovieResourceImage *)_resources[resource_id];
    }

    resource->images = images;

    for( ;;)
    {
        ae_uint8_t param_type;
        AE_READ( _stream, param_type );

        switch( param_type )
        {
        case 0:
            {
            }break;
        default:
            {
                AE_RETURN_ERROR_RESULT( AE_RESULT_INVALID_STREAM );
            }
        }

        if( param_type == 0 )
        {
            break;
        }
    }

    *_out = (aeMovieResource *)resource;

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __load_movie_resource_particle( const aeMovieInstance * _instance, aeMovieStream * _stream, const aeMovieResource ** _atlases, const aeMovieResource ** _resources, aeMovieResource ** _out )
{
    AE_UNUSED( _atlases );

    aeMovieResourceParticle * resource = AE_NEW( _instance, aeMovieResourceParticle );

    AE_RESULT_PANIC_MEMORY( resource );

    AE_READ_STRING( _stream, resource->path );
    resource->codec = AE_READ8( _stream );

    ae_uint32_t image_count = AE_READZ( _stream );

    resource->image_count = image_count;
    const aeMovieResourceImage ** images = AE_NEWN( _instance, const aeMovieResourceImage *, image_count );

    AE_RESULT_PANIC_MEMORY( images );

    const aeMovieResourceImage ** it_image = images;
    const aeMovieResourceImage ** it_image_end = images + image_count;
    for( ; it_image != it_image_end; ++it_image )
    {
        ae_uint32_t resource_id = AE_READZ( _stream );

        *it_image = (const aeMovieResourceImage *)_resources[resource_id];
    }

    resource->images = images;

    for( ;;)
    {
        ae_uint8_t param_type;
        AE_READ( _stream, param_type );

        switch( param_type )
        {
        case 0:
            {
            }break;
        default:
            {
                AE_RETURN_ERROR_RESULT( AE_RESULT_INVALID_STREAM );
            }
        }

        if( param_type == 0 )
        {
            break;
        }
    }

    *_out = (aeMovieResource *)resource;

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __load_movie_resource_slot( const aeMovieInstance * _instance, aeMovieStream * _stream, const aeMovieResource ** _atlases, const aeMovieResource ** _resources, aeMovieResource ** _out )
{
    AE_UNUSED( _atlases );
    AE_UNUSED( _resources );

    aeMovieResourceSlot * resource = AE_NEW( _instance, aeMovieResourceSlot );

    AE_RESULT_PANIC_MEMORY( resource );

    AE_READF( _stream, resource->width );
    AE_READF( _stream, resource->height );

    for( ;;)
    {
        ae_uint8_t param_type;
        AE_READ( _stream, param_type );

        switch( param_type )
        {
        case 0:
            {
            }break;
        default:
            {
                AE_RETURN_ERROR_RESULT( AE_RESULT_INVALID_STREAM );
            }
        }

        if( param_type == 0 )
        {
            break;
        }
    }

    *_out = (aeMovieResource *)resource;

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
typedef ae_result_t( *ae_load_movie_resource_t )(const aeMovieInstance * _instance, aeMovieStream * _stream, const aeMovieResource ** _atlases, const aeMovieResource ** _resources, aeMovieResource ** _out);
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __load_movie_resource( aeMovieData * _movieData, aeMovieStream * _stream, const aeMovieResource ** _atlases, const aeMovieResource ** _resources, aeMovieResource ** _resource )
{
    const aeMovieInstance * instance = _movieData->instance;

    ae_uint8_t type;
    AE_READ( _stream, type );

    ae_string_t name;
    AE_READ_STRING( _stream, name );

#ifdef AE_MOVIE_DEBUG_STREAM
    instance->logger( instance->instance_userdata, AE_ERROR_STREAM, "read type %d", type );
#endif

    static const ae_load_movie_resource_t resource_loaders[] = {
        0, //0
        0, //1
        0, //2
        0, //3
        &__load_movie_resource_solid, //AE_MOVIE_RESOURCE_SOLID
        &__load_movie_resource_video, //AE_MOVIE_RESOURCE_VIDEO
        &__load_movie_resource_sound, //AE_MOVIE_RESOURCE_SOUND
        &__load_movie_resource_image, //AE_MOVIE_RESOURCE_IMAGE
        &__load_movie_resource_sequence, //AE_MOVIE_RESOURCE_SEQUENCE
        &__load_movie_resource_particle, //AE_MOVIE_RESOURCE_PARTICLE
        &__load_movie_resource_slot, //AE_MOVIE_RESOURCE_SLOT 
    };

#ifdef AE_MOVIE_DEBUG
    if( type >= sizeof( resource_loaders ) / sizeof( resource_loaders[0] ) )
    {
        AE_RETURN_ERROR_RESULT( AE_RESULT_INVALID_STREAM );
    }
#endif

    aeMovieResource * new_resource = AE_NULLPTR;
    ae_load_movie_resource_t resource_loader = resource_loaders[type];

#ifdef AE_MOVIE_DEBUG
    if( resource_loader == AE_NULLPTR )
    {
        AE_RETURN_ERROR_RESULT( AE_RESULT_INVALID_STREAM );
    }
#endif

    AE_RESULT( (*resource_loader), (instance, _stream, _atlases, _resources, &new_resource) );

    new_resource->type = type;
    new_resource->name = name;

    ae_userdata_t resource_userdata = AE_USERDATA_NULL;
    if( (*_movieData->providers.resource_provider)(new_resource, &resource_userdata, _movieData->provider_userdata) == AE_FALSE )
    {
        return AE_RESULT_INTERNAL_ERROR;
    }

    new_resource->userdata = resource_userdata;

    *_resource = new_resource;

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __cache_movie_resource_data( aeMovieData * _movieData, aeMovieResource * _resource )
{
    const aeMovieInstance * instance = _movieData->instance;

    aeMovieResourceTypeEnum resource_type = _resource->type;

    switch( resource_type )
    {
    case AE_MOVIE_RESOURCE_IMAGE:
        {
            aeMovieResourceImage * resource_image = (aeMovieResourceImage *)_resource;

            struct aeMovieResourceImageCache * cache = AE_NEW( instance, struct aeMovieResourceImageCache );

            ae_userdata_t uv_cache_userdata = AE_USERDATA_NULL;
            AE_RESULT( __callback_cache_uv_provider, (_movieData, &uv_cache_userdata, _resource, 4, resource_image->uvs) );

            cache->uv_cache_userdata = uv_cache_userdata;

            if( resource_image->mesh != AE_NULLPTR )
            {
                ae_userdata_t mesh_uv_cache_userdata = AE_USERDATA_NULL;
                AE_RESULT( __callback_cache_uv_provider, (_movieData, &mesh_uv_cache_userdata, _resource, resource_image->mesh->vertex_count, resource_image->mesh->uvs) );

                cache->mesh_uv_cache_userdata = mesh_uv_cache_userdata;
            }
            else
            {
                cache->mesh_uv_cache_userdata = AE_NULLPTR;
            }

            ae_uint32_t quality = 0;
            for( ; quality != AE_MOVIE_BEZIER_MAX_QUALITY; ++quality )
            {
                ae_uint32_t vertex_count = get_bezier_warp_vertex_count( quality );
                const ae_vector2_t * uvs = instance->bezier_warp_uvs[quality];

                ae_userdata_t bezier_warp_uv_cache_userdata = AE_USERDATA_NULL;
                AE_RESULT( __callback_cache_uv_provider, (_movieData, &bezier_warp_uv_cache_userdata, _resource, vertex_count, uvs) );

                cache->bezier_warp_uv_cache_userdata[quality] = bezier_warp_uv_cache_userdata;
            }

            resource_image->cache = cache;
        }break;
    case AE_MOVIE_RESOURCE_VIDEO:
        {
            aeMovieResourceVideo * resource_video = (aeMovieResourceVideo *)_resource;

            struct aeMovieResourceVideoCache * cache = AE_NEW( instance, struct aeMovieResourceVideoCache );

            ae_userdata_t uv_cache_userdata = AE_USERDATA_NULL;
            AE_RESULT( __callback_cache_uv_provider, (_movieData, &uv_cache_userdata, _resource, 4, instance->sprite_uv) );

            cache->uv_cache_userdata = uv_cache_userdata;

            ae_uint32_t quality = 0;
            for( ; quality != AE_MOVIE_BEZIER_MAX_QUALITY; ++quality )
            {
                ae_uint32_t vertex_count = get_bezier_warp_vertex_count( quality );
                const ae_vector2_t * uvs = instance->bezier_warp_uvs[quality];

                ae_userdata_t bezier_warp_uv_cache_userdata = AE_USERDATA_NULL;
                AE_RESULT( __callback_cache_uv_provider, (_movieData, &bezier_warp_uv_cache_userdata, _resource, vertex_count, uvs) );

                cache->bezier_warp_uv_cache_userdata[quality] = bezier_warp_uv_cache_userdata;
            }

            resource_video->cache = cache;
        }break;
    default:
        {
        }break;
    }

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
ae_result_t ae_load_movie_data( aeMovieData * _movieData, aeMovieStream * _stream, ae_uint32_t * _major, ae_uint32_t * _minor )
{
    const aeMovieInstance * instance = _movieData->instance;

#ifdef AE_MOVIE_DEBUG_STREAM
    instance->logger( instance->instance_userdata, AE_ERROR_STREAM, "begin" );
#endif

    ae_result_t check_result = __check_movie_data( _stream, _major, _minor );

    if( check_result != AE_RESULT_SUCCESSFUL )
    {
        return check_result;
    }

    AE_READ_STRING( _stream, _movieData->name );

    _movieData->common_store = AE_READB( _stream );

    ae_uint32_t atlas_count = AE_READZ( _stream );

    _movieData->atlas_count = atlas_count;

    const aeMovieResource ** atlases = AE_NULLPTR;

    if( atlas_count != 0 )
    {
        atlases = AE_NEWN( instance, const aeMovieResource *, atlas_count );

        AE_RESULT_PANIC_MEMORY( atlases );

        const aeMovieResource ** it_atlas = atlases;
        const aeMovieResource ** it_atlas_end = atlases + atlas_count;
        for( ; it_atlas != it_atlas_end; ++it_atlas )
        {
            aeMovieResource * new_atlas;
            AE_RESULT( __load_movie_resource, (_movieData, _stream, AE_NULLPTR, atlases, &new_atlas) );

            *it_atlas = new_atlas;
        }

        _movieData->atlases = atlases;
    }

    ae_uint32_t resource_count = AE_READZ( _stream );

    aeMovieDataCacheUVAvailableCallbackData callbackData;
    callbackData.dummy = 0;

    ae_bool_t cache_uv_available = (*_movieData->providers.cache_uv_available)(&callbackData, _movieData->provider_userdata);

    _movieData->resource_count = resource_count;
    const aeMovieResource ** resources = AE_NEWN( instance, const aeMovieResource *, resource_count );

    AE_RESULT_PANIC_MEMORY( resources );

    const aeMovieResource ** it_resource = resources;
    const aeMovieResource ** it_resource_end = resources + resource_count;
    for( ; it_resource != it_resource_end; ++it_resource )
    {
        aeMovieResource * new_resource;
        AE_RESULT( __load_movie_resource, (_movieData, _stream, atlases, resources, &new_resource) );

        if( cache_uv_available == AE_TRUE )
        {
            AE_RESULT( __cache_movie_resource_data, (_movieData, new_resource) );
        }

        *it_resource = new_resource;
    }

    _movieData->resources = resources;

    ae_uint32_t composition_count = AE_READZ( _stream );

    _movieData->composition_count = composition_count;

    aeMovieCompositionData * compositions = AE_NEWN( instance, aeMovieCompositionData, composition_count );

    AE_RESULT_PANIC_MEMORY( compositions );

    aeMovieCompositionData * it_composition = compositions;
    aeMovieCompositionData * it_composition_end = compositions + composition_count;
    for( ; it_composition != it_composition_end; ++it_composition )
    {
        aeMovieCompositionData * composition = it_composition;

        AE_RESULT( __load_movie_data_composition, (_movieData, compositions, _stream, composition, cache_uv_available) );
    }

    _movieData->compositions = compositions;

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
const ae_char_t * ae_get_movie_name( const aeMovieData * _movieData )
{
    const ae_char_t * name = _movieData->name;

    return name;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_is_movie_common_store( const aeMovieData * _movieData )
{
    ae_bool_t common_store = _movieData->common_store;

    return common_store;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_has_movie_composition_data( const aeMovieData * _movieData, const ae_char_t * _name )
{
    const aeMovieInstance * instance = _movieData->instance;

    const aeMovieCompositionData * it_composition = _movieData->compositions;
    const aeMovieCompositionData * it_composition_end = _movieData->compositions + _movieData->composition_count;
    for( ; it_composition != it_composition_end; ++it_composition )
    {
        const aeMovieCompositionData * composition = it_composition;

        if( AE_STRNCMP( instance, composition->name, _name, AE_MOVIE_MAX_COMPOSITION_NAME ) != 0 )
        {
            continue;
        }

        return AE_TRUE;
    }

    return AE_FALSE;
}
//////////////////////////////////////////////////////////////////////////
const aeMovieCompositionData * ae_get_movie_composition_data( const aeMovieData * _movieData, const ae_char_t * _name )
{
    const aeMovieInstance * instance = _movieData->instance;

    const aeMovieCompositionData * it_composition = _movieData->compositions;
    const aeMovieCompositionData * it_composition_end = _movieData->compositions + _movieData->composition_count;
    for( ; it_composition != it_composition_end; ++it_composition )
    {
        const aeMovieCompositionData * composition = it_composition;

        if( AE_STRNCMP( instance, composition->name, _name, AE_MOVIE_MAX_COMPOSITION_NAME ) != 0 )
        {
            continue;
        }

        return composition;
    }

    return AE_NULLPTR;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_visit_movie_layer_data( const aeMovieData * _movieData, ae_movie_layer_data_visitor_t _visitor, ae_userdata_t _ud )
{
    const aeMovieCompositionData * it_composition = _movieData->compositions;
    const aeMovieCompositionData * it_composition_end = _movieData->compositions + _movieData->composition_count;
    for( ; it_composition != it_composition_end; ++it_composition )
    {
        const aeMovieCompositionData * compositionData = it_composition;

        const aeMovieLayerData *it_layer = compositionData->layers;
        const aeMovieLayerData *it_layer_end = compositionData->layers + compositionData->layer_count;
        for( ; it_layer != it_layer_end; ++it_layer )
        {
            const aeMovieLayerData * layerData = it_layer;

            if( (*_visitor)(compositionData, layerData, _ud) == AE_FALSE )
            {
                return AE_FALSE;
            }
        }
    }

    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
const ae_char_t * ae_get_movie_layer_data_name( const aeMovieLayerData * _layer )
{
    return _layer->name;
}
//////////////////////////////////////////////////////////////////////////
aeMovieLayerTypeEnum ae_get_movie_layer_data_type( const aeMovieLayerData * _layer )
{
    return _layer->type;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_is_movie_layer_data_track_mate( const aeMovieLayerData * _layer )
{
    return _layer->is_track_matte;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_is_movie_layer_data_threeD( const aeMovieLayerData * _layer )
{
    return _layer->threeD;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_is_movie_layer_data_incessantly( const aeMovieLayerData * _layer )
{
    return _layer->incessantly;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_has_movie_layer_data_option( const aeMovieLayerData * _layer, ae_uint32_t _option )
{
    ae_uint32_t index = 0;
    for( ; index != _layer->options_count; ++index )
    {
        ae_uint32_t option_value = _layer->options[index];

        if( option_value != _option )
        {
            continue;
        }

        return AE_TRUE;
    }

    return AE_FALSE;
}
//////////////////////////////////////////////////////////////////////////
ae_uint32_t ae_get_movie_layer_data_option_count( const aeMovieLayerData * _layer )
{
    return _layer->options_count;
}
//////////////////////////////////////////////////////////////////////////
ae_uint32_t ae_get_movie_layer_data_option( const aeMovieLayerData * _layer, ae_uint32_t _index )
{
    return _layer->options[_index];
}
//////////////////////////////////////////////////////////////////////////
const aeMovieResource * ae_get_movie_layer_data_resource( const aeMovieLayerData * _layer )
{
    return _layer->resource;
}
//////////////////////////////////////////////////////////////////////////
ae_voidptr_t ae_get_movie_layer_data_resource_userdata( const aeMovieLayerData * _layer )
{
    const aeMovieResource * resource = _layer->resource;

    return resource->userdata;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_test_movie_layer_data_opacity_transparent( const aeMovieLayerData * _layer )
{
    if( _layer->transformation->timeline_opacity != AE_NULLPTR )
    {
        return AE_FALSE;
    }

    if( _layer->transformation->immutable_opacity > 0.00390625f )
    {
        return AE_FALSE;
    }

    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
ae_track_matte_mode_t ae_get_movie_layer_data_track_matte_mode( const aeMovieLayerData * _layer )
{
    return _layer->track_matte_mode;
}
//////////////////////////////////////////////////////////////////////////
ae_blend_mode_t ae_get_movie_layer_data_blend_mode( const aeMovieLayerData * _layer )
{
    return _layer->blend_mode;
}
//////////////////////////////////////////////////////////////////////////
ae_float_t ae_get_movie_layer_data_stretch( const aeMovieLayerData * _layer )
{
    return _layer->stretch;
}
//////////////////////////////////////////////////////////////////////////
const ae_viewport_t * ae_get_movie_layer_data_viewport( const aeMovieLayerData * _layer )
{
    const aeMovieLayerExtensions * extensions = _layer->extensions;

    if( extensions == AE_NULLPTR )
    {
        return AE_NULLPTR;
    }

    const aeMovieLayerExtensionViewport * viewport = extensions->viewport;

    if( viewport == AE_NULLPTR )
    {
        return AE_NULLPTR;
    }

    return &viewport->viewport;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_get_movie_layer_data_socket_polygon( const aeMovieLayerData * _layerData, ae_uint32_t _frame, const ae_polygon_t ** _polygon )
{
#ifdef AE_MOVIE_DEBUG
    if( _layerData->type != AE_MOVIE_LAYER_TYPE_SOCKET )
    {
        return AE_FALSE;
    }

    if( _frame >= _layerData->frame_count )
    {
        return AE_FALSE;
    }
#endif

    const aeMovieLayerExtensionPolygon * polygon = _layerData->extensions->polygon;

    if( polygon->immutable == AE_TRUE )
    {
        *_polygon = &polygon->immutable_polygon;
    }
    else
    {
        *_polygon = polygon->polygons + _frame;
    }

    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
const ae_char_t * ae_get_movie_composition_data_name( const aeMovieCompositionData * _compositionData )
{
    return _compositionData->name;
}
//////////////////////////////////////////////////////////////////////////
ae_float_t ae_get_movie_composition_data_width( const aeMovieCompositionData * _compositionData )
{
    return _compositionData->width;
}
//////////////////////////////////////////////////////////////////////////
ae_float_t ae_get_movie_composition_data_height( const aeMovieCompositionData * _compositionData )
{
    return _compositionData->height;
}
//////////////////////////////////////////////////////////////////////////
ae_time_t ae_get_movie_composition_data_duration( const aeMovieCompositionData * _compositionData )
{
    return AE_TIME_OUTSCALE( _compositionData->duration_time );
}
//////////////////////////////////////////////////////////////////////////
ae_time_t ae_get_movie_composition_data_frame_duration( const aeMovieCompositionData * _compositionData )
{
    return AE_TIME_OUTSCALE( _compositionData->frameDuration );
}
//////////////////////////////////////////////////////////////////////////
ae_uint32_t ae_get_movie_composition_data_frame_count( const aeMovieCompositionData * _compositionData )
{
    return _compositionData->duration_frame;
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_get_movie_composition_data_loop_segment( const aeMovieCompositionData * _compositionData, ae_time_t * _in, ae_time_t * _out )
{
    *_in = AE_TIME_OUTSCALE( _compositionData->loop_segment[0] );
    *_out = AE_TIME_OUTSCALE( _compositionData->loop_segment[1] );
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_is_movie_composition_data_master( const aeMovieCompositionData * _compositionData )
{
    return _compositionData->master;
}
//////////////////////////////////////////////////////////////////////////
ae_uint32_t ae_get_movie_composition_data_count( const aeMovieData * _movieData )
{
    return _movieData->composition_count;
}
//////////////////////////////////////////////////////////////////////////
const aeMovieCompositionData * ae_get_movie_composition_data_by_index( const aeMovieData * _movieData, ae_uint32_t _index )
{
    return _movieData->compositions + _index;
}
//////////////////////////////////////////////////////////////////////////
ae_uint32_t ae_get_movie_composition_data_event_count( const aeMovieCompositionData * _compositionData )
{
    ae_uint32_t count = 0;

    const aeMovieLayerData * it_layer = _compositionData->layers;
    const aeMovieLayerData * it_layer_end = _compositionData->layers + _compositionData->layer_count;
    for( ; it_layer != it_layer_end; ++it_layer )
    {
        aeMovieLayerTypeEnum type = it_layer->type;

        if( type == AE_MOVIE_LAYER_TYPE_EVENT )
        {
            ++count;
        }
        else if( type == AE_MOVIE_LAYER_TYPE_MOVIE )
        {
            count += ae_get_movie_composition_data_event_count( it_layer->subcomposition_data );
        }
    }

    return count;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_bool_t __get_composition_data_event_name( const aeMovieCompositionData * _compositionData, ae_uint32_t * _iterator, ae_uint32_t _index, const ae_char_t ** _name )
{
    const aeMovieLayerData * it_layer = _compositionData->layers;
    const aeMovieLayerData * it_layer_end = _compositionData->layers + _compositionData->layer_count;
    for( ; it_layer != it_layer_end; ++it_layer )
    {
        aeMovieLayerTypeEnum type = it_layer->type;

        if( type == AE_MOVIE_LAYER_TYPE_EVENT )
        {
            if( (*_iterator)++ == _index )
            {
                *_name = it_layer->name;

                return AE_TRUE;
            }
        }
        else if( type == AE_MOVIE_LAYER_TYPE_MOVIE )
        {
            if( __get_composition_data_event_name( it_layer->subcomposition_data, _iterator, _index, _name ) == AE_TRUE )
            {
                return AE_TRUE;
            }
        }
    }

    return AE_FALSE;
}
//////////////////////////////////////////////////////////////////////////
const ae_char_t * ae_get_movie_composition_data_event_name( const aeMovieCompositionData * _compositionData, ae_uint32_t _index )
{
    ae_uint32_t iterator = 0U;
    const ae_char_t * name;
    if( __get_composition_data_event_name( _compositionData, &iterator, _index, &name ) == AE_FALSE )
    {
        return AE_NULLPTR;
    }

    return name;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_has_movie_composition_data_bounds( const aeMovieCompositionData * _compositionData )
{
    if( (_compositionData->flags & AE_MOVIE_COMPOSITION_BOUNDS) == 0 )
    {
        return AE_FALSE;
    }

    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_get_movie_composition_data_bounds( const aeMovieCompositionData * _compositionData, ae_viewport_t * _bounds )
{
    if( _compositionData->flags & AE_MOVIE_COMPOSITION_BOUNDS )
    {
        *_bounds = _compositionData->bounds;

        return AE_TRUE;
    }

    return AE_FALSE;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_has_movie_composition_data_layer( const aeMovieInstance * _instance, const aeMovieCompositionData * _compositionData, const ae_char_t * _layerName )
{
    const aeMovieLayerData * it_layer = _compositionData->layers;
    const aeMovieLayerData * it_layer_end = _compositionData->layers + _compositionData->layer_count;
    for( ; it_layer != it_layer_end; ++it_layer )
    {
        aeMovieLayerTypeEnum type = it_layer->type;

        if( AE_STRNCMP( _instance, it_layer->name, _layerName, AE_MOVIE_MAX_LAYER_NAME ) == 0 )
        {
            return AE_TRUE;
        }

        switch( type )
        {
        case AE_MOVIE_LAYER_TYPE_MOVIE:
            {
                if( ae_has_movie_composition_data_layer( _instance, it_layer->subcomposition_data, _layerName ) == AE_TRUE )
                {
                    return AE_TRUE;
                }
            }break;
        default:
            break;
        }
    }

    return AE_FALSE;
}
//////////////////////////////////////////////////////////////////////////