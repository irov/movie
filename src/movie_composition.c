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

#include "movie/movie_composition.h"
#include "movie/movie_resource.h"

#include "movie_bezier.h"
#include "movie_transformation.h"
#include "movie_memory.h"
#include "movie_math.h"
#include "movie_debug.h"

#include "movie_struct.h"

#ifndef AE_MOVIE_FRAME_EPSILON
#define AE_MOVIE_FRAME_EPSILON 0.01f
#endif

//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_uint32_t __get_composition_update_revision( const aeMovieComposition * _composition )
{
    ae_uint32_t update_revision = *_composition->update_revision;

    return update_revision;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_uint32_t __inc_composition_update_revision( const aeMovieComposition * _composition )
{
    ae_uint32_t update_revision = ++(*_composition->update_revision);

    return update_revision;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __make_mesh_vertices( const ae_mesh_t * _mesh, const ae_matrix34_t _matrix, const ae_vector2_t * _uvs, aeMovieRenderMesh * _render )
{
    _render->vertexCount = _mesh->vertex_count;
    _render->indexCount = _mesh->index_count;

    ae_uint32_t vertex_count = _mesh->vertex_count;

    ae_uint32_t vertex_index = 0;
    for( ; vertex_index != vertex_count; ++vertex_index )
    {
        ae_mul_v3_v2_m34( _render->position[vertex_index], _mesh->positions[vertex_index], _matrix );
    }

    if( _uvs == AE_NULLPTR )
    {
        for( vertex_index = 0; vertex_index != vertex_count; ++vertex_index )
        {
            ae_copy_v2( _render->uv[vertex_index], _mesh->uvs[vertex_index] );
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

        for( vertex_index = 0; vertex_index != vertex_count; ++vertex_index )
        {
            ae_float_t u = _mesh->uvs[vertex_index][0];
            ae_float_t v = _mesh->uvs[vertex_index][1];
            
            _render->uv[vertex_index][0] = bx + ux * u + vx * v;
            _render->uv[vertex_index][1] = by + uy * u + vy * v;
        } 
    }

    _render->indices = _mesh->indices;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __make_layer_sprite_vertices( const aeMovieInstance * _instance, ae_float_t _offset_x, ae_float_t _offset_y, ae_float_t _width, ae_float_t _height, const ae_matrix34_t _matrix, const ae_vector2_t * _uv, aeMovieRenderMesh * _render )
{
    ae_vector2_t v_position[4];

    ae_float_t * v = &v_position[0][0];

    *v++ = _offset_x + _width * 0.f;
    *v++ = _offset_y + _height * 0.f;
    *v++ = _offset_x + _width * 1.f;
    *v++ = _offset_y + _height * 0.f;
    *v++ = _offset_x + _width * 1.f;
    *v++ = _offset_y + _height * 1.f;
    *v++ = _offset_x + _width * 0.f;
    *v++ = _offset_y + _height * 1.f;

    _render->vertexCount = 4;
    _render->indexCount = 6;

    ae_mul_v3_v2_m34( _render->position[0], v_position[0], _matrix );
    ae_mul_v3_v2_m34( _render->position[1], v_position[1], _matrix );
    ae_mul_v3_v2_m34( _render->position[2], v_position[2], _matrix );
    ae_mul_v3_v2_m34( _render->position[3], v_position[3], _matrix );

    ae_copy_v2( _render->uv[0], _uv[0] );
    ae_copy_v2( _render->uv[1], _uv[1] );
    ae_copy_v2( _render->uv[2], _uv[2] );
    ae_copy_v2( _render->uv[3], _uv[3] );

    _render->indices = _instance->sprite_indices;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __make_layer_mesh_vertices( const aeMovieLayerExtensionMesh * _extensionMesh, ae_uint32_t _frame, const ae_matrix34_t _matrix, const ae_vector2_t * _uvs, aeMovieRenderMesh * _render )
{
    const ae_mesh_t * mesh = (_extensionMesh->immutable == AE_TRUE) ? &_extensionMesh->immutable_mesh : (_extensionMesh->meshes + _frame);

    __make_mesh_vertices( mesh, _matrix, _uvs, _render );
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_float_t __compute_movie_property_value( const struct aeMoviePropertyValue * _property, ae_uint32_t _frame, ae_bool_t _interpolate, ae_float_t _t )
{
    if( _property->immutable == AE_TRUE )
    {
        return _property->immutable_value;
    }

    if( _interpolate == AE_FALSE )
    {
        ae_float_t value = _property->values[_frame];

        return value;
    }

    ae_float_t value0 = _property->values[_frame + 0];
    ae_float_t value1 = _property->values[_frame + 1];

    ae_float_t valuef = ae_linerp_f1( value0, value1, _t );

    return valuef;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_color_channel_t __compute_movie_property_color_channel( const struct aeMoviePropertyColorChannel * _property, ae_uint32_t _frame, ae_bool_t _interpolate, ae_float_t t )
{
    if( _property->immutable == AE_TRUE )
    {
        return _property->immutable_value;
    }

    if( _interpolate == AE_FALSE )
    {
        ae_color_channel_t c = _property->values[_frame];

        return c;
    }

    ae_color_channel_t c0 = _property->values[_frame + 0];
    ae_color_channel_t c1 = _property->values[_frame + 1];

    ae_color_channel_t cf = ae_linerp_f1( c0, c1, t );

    return cf;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_uint32_t __compute_movie_node_frame( const aeMovieNode * _node, ae_float_t * _t )
{
    ae_uint32_t frame = _node->current_frame;
    *_t = _node->current_frame_t;

    return frame;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __compute_movie_render_mesh( const aeMovieComposition * _composition, const aeMovieNode * _node, aeMovieRenderMesh * _render, ae_bool_t _interpolate, ae_bool_t _trackmatte )
{
    const aeMovieData * movie_data = _composition->movie_data;
    const aeMovieInstance * instance = movie_data->instance;
    const aeMovieLayerData * layer = _node->layer;
    const aeMovieResource * resource = layer->resource;

    aeMovieLayerTypeEnum layer_type = layer->type;

    _render->layer_type = layer_type;

    _render->blend_mode = _node->blend_mode;

    if( resource != AE_NULLPTR )
    {
        _render->resource_type = resource->type;
        _render->resource_userdata = resource->userdata;
    }
    else
    {
        _render->resource_type = AE_MOVIE_RESOURCE_NONE;
        _render->resource_userdata = AE_NULLPTR;
    }

    _render->camera_userdata = _node->camera_userdata;
    _render->element_userdata = _node->element_userdata;
    _render->shader_userdata = _node->shader_userdata;

    _render->viewport = _node->viewport;

    if( _node->track_matte_node != AE_NULLPTR && _node->track_matte_node->active == AE_TRUE )
    {
        _render->track_matte_mode = layer->track_matte_mode;
        _render->track_matte_userdata = _node->track_matte_node->track_matte_userdata;
    }
    else
    {
        _render->track_matte_mode = AE_MOVIE_TRACK_MATTE_NONE;
        _render->track_matte_userdata = AE_NULLPTR;
    }

    _render->uv_cache_userdata = AE_NULLPTR;

    ae_float_t t_frame = 0.f;
    ae_uint32_t frame = __compute_movie_node_frame( _node, &t_frame );

    switch( layer_type )
    {
    case AE_MOVIE_LAYER_TYPE_SHAPE:
        {
            __make_layer_mesh_vertices( layer->extensions->mesh, frame, _node->matrix, AE_NULLPTR, _render );

            _render->color = _node->color;
            _render->opacity = _node->opacity;
        }break;
    case AE_MOVIE_LAYER_TYPE_SOLID:
        {
            aeMovieResourceSolid * resource_solid = (aeMovieResourceSolid *)resource;

            if( layer->extensions->mesh != AE_NULLPTR )
            {
                __make_layer_mesh_vertices( layer->extensions->mesh, frame, _node->matrix, AE_NULLPTR, _render );
            }
            else if( layer->extensions->bezier_warp != AE_NULLPTR )
            {
                make_layer_bezier_warp_vertices( instance, layer->extensions->bezier_warp, frame, _interpolate, t_frame, _node->matrix, AE_NULLPTR, _render );
            }
            else
            {
                ae_float_t width = resource_solid->width;
                ae_float_t height = resource_solid->height;

                __make_layer_sprite_vertices( instance, 0.f, 0.f, width, height, _node->matrix, instance->sprite_uv, _render );
            }

            _render->color.r = _node->color.r * resource_solid->color.r;
            _render->color.g = _node->color.g * resource_solid->color.g;
            _render->color.b = _node->color.b * resource_solid->color.b;
            _render->opacity = _node->opacity;
        }break;
    case AE_MOVIE_LAYER_TYPE_SEQUENCE:
        {
            aeMovieResourceSequence * resource_sequence = (aeMovieResourceSequence *)resource;

            ae_uint32_t frame_sequence;

            if( layer->extensions->timeremap != AE_NULLPTR )
            {
                ae_float_t time = layer->extensions->timeremap->times[frame];

                frame_sequence = (ae_uint32_t)(time * resource_sequence->frameDurationInv);
            }
            else
            {
                if( layer->reverse_time == AE_TRUE )
                {
                    frame_sequence = (ae_uint32_t)((_node->out_time - _node->in_time - (layer->start_time + _node->current_time)) * resource_sequence->frameDurationInv);
                }
                else
                {
                    frame_sequence = (ae_uint32_t)((layer->start_time + _node->current_time) * resource_sequence->frameDurationInv);
                }
            }

            frame_sequence %= resource_sequence->image_count;

            const aeMovieResourceImage * resource_image = resource_sequence->images[frame_sequence];

            _render->resource_type = resource_image->type;
            _render->resource_userdata = resource_image->userdata;

            if( layer->extensions->mesh != AE_NULLPTR )
            {
                __make_layer_mesh_vertices( layer->extensions->mesh, frame, _node->matrix, resource_image->uvs, _render );

                if( layer->cache != AE_NULLPTR )
                {
                    if( layer->extensions->mesh->immutable == AE_TRUE )
                    {
                        _render->uv_cache_userdata = layer->cache->immutable_mesh_uv_cache_userdata;
                    }
                    else
                    {
                        _render->uv_cache_userdata = layer->cache->mesh_uv_cache_userdata[frame];
                    }
                }
            }
            else if( layer->extensions->bezier_warp != AE_NULLPTR )
            {
                make_layer_bezier_warp_vertices( instance, layer->extensions->bezier_warp, frame, _interpolate, t_frame, _node->matrix, resource_image->uvs, _render );

                if( resource_image->cache != AE_NULLPTR )
                {
                    _render->uv_cache_userdata = resource_image->cache->bezier_warp_uv_cache_userdata[layer->extensions->bezier_warp->quality];
                }
            }
            else if( resource_image->mesh != AE_NULLPTR && _trackmatte == AE_FALSE )
            {
                __make_mesh_vertices( resource_image->mesh, _node->matrix, resource_image->uvs, _render );

                if( resource_image->cache != AE_NULLPTR )
                {
                    _render->uv_cache_userdata = resource_image->cache->mesh_uv_cache_userdata;
                }
            }
            else
            {
                ae_float_t offset_x = resource_image->offset_x;
                ae_float_t offset_y = resource_image->offset_y;

                ae_float_t width = resource_image->trim_width;
                ae_float_t height = resource_image->trim_height;

                __make_layer_sprite_vertices( instance, offset_x, offset_y, width, height, _node->matrix, resource_image->uvs, _render );

                if( resource_image->cache != AE_NULLPTR )
                {
                    _render->uv_cache_userdata = resource_image->cache->uv_cache_userdata;
                }
            }

            _render->color = _node->color;
            _render->opacity = _node->opacity;
        }break;
    case AE_MOVIE_LAYER_TYPE_VIDEO:
        {
            aeMovieResourceVideo * resource_video = (aeMovieResourceVideo *)resource;

            if( layer->extensions->mesh != AE_NULLPTR )
            {
                __make_layer_mesh_vertices( layer->extensions->mesh, frame, _node->matrix, AE_NULLPTR, _render );

                if( layer->cache != AE_NULLPTR )
                {
                    if( layer->extensions->mesh->immutable == AE_TRUE )
                    {
                        _render->uv_cache_userdata = layer->cache->immutable_mesh_uv_cache_userdata;
                    }
                    else
                    {
                        _render->uv_cache_userdata = layer->cache->mesh_uv_cache_userdata[frame];
                    }
                }
            }
            else if( layer->extensions->bezier_warp != AE_NULLPTR )
            {
                make_layer_bezier_warp_vertices( instance, layer->extensions->bezier_warp, frame, _interpolate, t_frame, _node->matrix, AE_NULLPTR, _render );

                if( resource_video->cache != AE_NULLPTR )
                {
                    _render->uv_cache_userdata = resource_video->cache->bezier_warp_uv_cache_userdata[layer->extensions->bezier_warp->quality];
                }
            }
            else
            {
                ae_float_t offset_x = resource_video->offset_x;
                ae_float_t offset_y = resource_video->offset_y;

                ae_float_t width = resource_video->trim_width;
                ae_float_t height = resource_video->trim_height;

                __make_layer_sprite_vertices( instance, offset_x, offset_y, width, height, _node->matrix, instance->sprite_uv, _render );

                if( resource_video->cache != AE_NULLPTR )
                {
                    _render->uv_cache_userdata = resource_video->cache->uv_cache_userdata;
                }
            }

            _render->color = _node->color;
            _render->opacity = _node->opacity;
        }break;
    case AE_MOVIE_LAYER_TYPE_IMAGE:
        {
            aeMovieResourceImage * resource_image = (aeMovieResourceImage *)resource;

            if( layer->extensions->mesh != AE_NULLPTR )
            {
                __make_layer_mesh_vertices( layer->extensions->mesh, frame, _node->matrix, resource_image->uvs, _render );

                if( layer->cache != AE_NULLPTR )
                {
                    if( layer->extensions->mesh->immutable == AE_TRUE )
                    {
                        _render->uv_cache_userdata = layer->cache->immutable_mesh_uv_cache_userdata;
                    }
                    else
                    {
                        _render->uv_cache_userdata = layer->cache->mesh_uv_cache_userdata[frame];
                    }
                }
            }
            else if( layer->extensions->bezier_warp != AE_NULLPTR )
            {
                make_layer_bezier_warp_vertices( instance, layer->extensions->bezier_warp, frame, _interpolate, t_frame, _node->matrix, resource_image->uvs, _render );

                if( resource_image->cache != AE_NULLPTR )
                {
                    _render->uv_cache_userdata = resource_image->cache->bezier_warp_uv_cache_userdata[layer->extensions->bezier_warp->quality];
                }
            }
            else if( resource_image->mesh != AE_NULLPTR && _trackmatte == AE_FALSE )
            {
                __make_mesh_vertices( resource_image->mesh, _node->matrix, resource_image->uvs, _render );

                if( resource_image->cache != AE_NULLPTR )
                {
                    _render->uv_cache_userdata = resource_image->cache->mesh_uv_cache_userdata;
                }
            }
            else
            {
                ae_float_t offset_x = resource_image->offset_x;
                ae_float_t offset_y = resource_image->offset_y;

                ae_float_t width = resource_image->trim_width;
                ae_float_t height = resource_image->trim_height;

                __make_layer_sprite_vertices( instance, offset_x, offset_y, width, height, _node->matrix, resource_image->uvs, _render );

                if( resource_image->cache != AE_NULLPTR )
                {
                    _render->uv_cache_userdata = resource_image->cache->uv_cache_userdata;
                }
            }

            _render->color = _node->color;
            _render->opacity = _node->opacity;
        }break;
    default:
        {
            _render->vertexCount = 0;
            _render->indexCount = 0;

            _render->color = _node->color;
            _render->opacity = _node->opacity;
        }break;
    }
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL aeMovieNode * __find_node_by_layer( aeMovieNode * _nodes, ae_uint32_t _begin, ae_uint32_t _end, const aeMovieLayerData * _layer )
{
    aeMovieNode * it_node = _nodes + _begin;
    aeMovieNode * it_node_end = _nodes + _end;
    for( ; it_node != it_node_end; ++it_node )
    {
        aeMovieNode * node = it_node;

        const aeMovieLayerData * node_layer = node->layer;

        if( node_layer == _layer )
        {
            return node;
        }
    }

    return AE_NULLPTR;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL const aeMovieLayerData * __find_layer_by_index( const aeMovieCompositionData * _compositionData, ae_uint32_t _index )
{
    const aeMovieLayerData * it_layer = _compositionData->layers;
    const aeMovieLayerData * it_layer_end = _compositionData->layers + _compositionData->layer_count;
    for( ; it_layer != it_layer_end; ++it_layer )
    {
        const aeMovieLayerData * layer = it_layer;

        ae_uint32_t layer_index = layer->index;

        if( layer_index == _index )
        {
            return layer;
        }
    }

    return AE_NULLPTR;
}
//////////////////////////////////////////////////////////////////////////
#ifdef AE_MOVIE_DEBUG
AE_INTERNAL ae_bool_t __test_error_composition_layer_frame( const aeMovieInstance * _instance, const aeMovieCompositionData * _compositionData, const aeMovieLayerData * _layerData, ae_uint32_t _frameId, const ae_char_t * _msg )
{
    if( _frameId >= _layerData->frame_count )
    {
        _instance->logger( _instance->instance_userdata
            , AE_ERROR_INTERNAL
            , "composition '%s' layer '%s' - %s\n"
            , _compositionData->name
            , _layerData->name
            , _msg
        );

        return AE_FALSE;
    }

    return AE_TRUE;
}
#endif
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_uint32_t __get_movie_frame_time( const struct aeMovieCompositionAnimation * _animation, const struct aeMovieNode * _node, ae_bool_t _interpolate, ae_float_t * _t )
{
    ae_float_t animation_time = _animation->time;

    const aeMovieLayerData * layer = _node->layer;

    ae_float_t frameDurationInv = layer->composition_data->frameDurationInv;

    ae_float_t current_time = animation_time - _node->in_time + _node->start_time;

    ae_float_t frame_time = current_time * _node->stretchInv * frameDurationInv;

    if( frame_time < 0.f )
    {
        frame_time = 0.f;
    }

    ae_uint32_t frame_relative = (ae_uint32_t)frame_time;

    if( frame_relative >= layer->frame_count )
    {
        frame_relative = layer->frame_count - 1;

        *_t = 0.f;

        return frame_relative;
    }

    if( _interpolate == AE_TRUE )
    {
        ae_float_t t_relative = ae_fractional_f( frame_time );

        *_t = t_relative;
    }
    else
    {
        *_t = 0.f;
    }

    return frame_relative;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __update_movie_composition_node_matrix( aeMovieNode * _node, ae_uint32_t _revision, const aeMovieComposition * _composition, const aeMovieCompositionData * _compositionData, const aeMovieCompositionAnimation * _animation, const aeMovieSubComposition * _subcomposition, ae_uint32_t _frameId, ae_bool_t _interpolate, ae_float_t _t )
{
    if( _node->update_revision == _revision )
    {
        return;
    }

    const aeMovieLayerData * node_layer = _node->layer;

#ifdef AE_MOVIE_DEBUG	
    if( __test_error_composition_layer_frame( _composition->movie_data->instance
        , _compositionData
        , node_layer
        , _frameId
        , "__update_movie_composition_node_matrix frame id out count"
    ) == AE_FALSE )
    {
        return;
    }
#endif

#ifdef AE_MOVIE_SAFE
    if( _frameId >= node_layer->frame_count )
    {
        return;
    }
#endif

    const struct aeMovieLayerTransformation * layer_transformation = node_layer->transformation;

    ae_color_channel_t local_r = ae_movie_make_layer_color_r( layer_transformation, _frameId, _interpolate, _t );
    ae_color_channel_t local_g = ae_movie_make_layer_color_g( layer_transformation, _frameId, _interpolate, _t );
    ae_color_channel_t local_b = ae_movie_make_layer_color_b( layer_transformation, _frameId, _interpolate, _t );

    ae_color_channel_t local_opacity = ae_movie_make_layer_opacity( layer_transformation, _frameId, _interpolate, _t );

    if( node_layer->extensions->volume != AE_NULLPTR )
    {
        const struct aeMoviePropertyValue * property_volume = node_layer->extensions->volume->property_volume;

        ae_float_t volume = __compute_movie_property_value( property_volume, _frameId, _interpolate, _t );

        _node->volume = volume;
    }

    aeMovieNode * node_relative = _node->relative_node;

    if( node_relative == AE_NULLPTR )
    {
        ae_movie_make_layer_matrix( _node->matrix, layer_transformation, _interpolate, _frameId, _t );

        if( node_layer->subcomposition_data != AE_NULLPTR )
        {
            _node->composition_color.r = local_r;
            _node->composition_color.g = local_g;
            _node->composition_color.b = local_b;

            _node->composition_opacity = local_opacity;
        }
        else
        {
            _node->composition_color.r = 1.f;
            _node->composition_color.g = 1.f;
            _node->composition_color.b = 1.f;

            _node->composition_opacity = 1.f;
        }

        _node->color.r = local_r;
        _node->color.g = local_g;
        _node->color.b = local_b;

        _node->opacity = local_opacity;
        
        _node->transparent = ae_equal_f_z( _node->opacity );

        return;
    }

    if( _subcomposition == node_relative->subcomposition )
    {
        const aeMovieLayerData * node_relative_relative = node_relative->layer;

        ae_float_t t_relative = 0.f;
        ae_uint32_t frame_relative = __get_movie_frame_time( _animation, node_relative, _composition->interpolate, &t_relative );

        ae_bool_t interpolate_relative = (frame_relative + 1 == node_relative_relative->frame_count) ? AE_FALSE : _interpolate;

        __update_movie_composition_node_matrix( node_relative, _revision, _composition, _compositionData, _animation, _subcomposition, frame_relative, interpolate_relative, t_relative );
    }

    if( (layer_transformation->identity_property_mask & AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL) == AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL )
    {
        ae_copy_m34( _node->matrix, node_relative->matrix );
    }
    else
    {
        ae_matrix34_t local_matrix;
        ae_movie_make_layer_matrix( local_matrix, layer_transformation, _interpolate, _frameId, _t );

        ae_mul_m34_m34_r( _node->matrix, local_matrix, node_relative->matrix );
    }

    if( node_layer->subcomposition_data != AE_NULLPTR )
    {
        _node->composition_color.r = node_relative->composition_color.r * local_r;
        _node->composition_color.g = node_relative->composition_color.g * local_g;
        _node->composition_color.b = node_relative->composition_color.b * local_b;

        _node->composition_opacity = node_relative->composition_opacity * local_opacity;
    }
    else
    {
        _node->composition_color.r = node_relative->composition_color.r;
        _node->composition_color.g = node_relative->composition_color.g;
        _node->composition_color.b = node_relative->composition_color.b;

        _node->composition_opacity = node_relative->composition_opacity;
    }

    _node->color.r = node_relative->composition_color.r * local_r;
    _node->color.g = node_relative->composition_color.g * local_g;
    _node->color.b = node_relative->composition_color.b * local_b;

    _node->opacity = node_relative->composition_opacity * local_opacity;

    _node->transparent = ae_equal_f_z( _node->opacity );
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __update_movie_composition_node_shader( aeMovieNode * _node, ae_uint32_t _revision, const aeMovieComposition * _composition, const aeMovieCompositionData * _compositionData, ae_uint32_t _frameId, ae_bool_t _interpolate, ae_float_t _t )
{
    AE_UNUSED( _compositionData );

    if( _node->update_revision == _revision )
    {
        return;
    }

    const aeMovieLayerData * node_layer = _node->layer;

#ifdef AE_MOVIE_DEBUG	
    if( __test_error_composition_layer_frame( _composition->movie_data->instance
        , _compositionData
        , node_layer
        , _frameId
        , "__update_movie_composition_node_shader frame id out count"
    ) == AE_FALSE )
    {
        return;
    }
#endif

#ifdef AE_MOVIE_SAFE
    if( _frameId >= node_layer->frame_count )
    {
        return;
    }
#endif

    const aeMovieLayerExtensionShader * shader = node_layer->extensions->shader;

    const struct aeMovieLayerShaderParameter ** it_parameter = shader->parameters;
    const struct aeMovieLayerShaderParameter ** it_parameter_end = shader->parameters + shader->parameter_count;

    ae_uint32_t index = 0;

    for( ;
        it_parameter != it_parameter_end;
        ++it_parameter )
    {
        const struct aeMovieLayerShaderParameter * parameter = *it_parameter;

        aeMovieShaderParameterTypeEnum parameter_type = parameter->type;

        switch( parameter_type )
        {
        case AE_MOVIE_EXTENSION_SHADER_PARAMETER_SLIDER:
            {
                const struct aeMovieLayerShaderParameterSlider * parameter_slider = (const struct aeMovieLayerShaderParameterSlider *)parameter;

                ae_float_t value = __compute_movie_property_value( parameter_slider->property_value, _frameId, _interpolate, _t );

                aeMovieShaderPropertyUpdateCallbackData callbackData;
                callbackData.index = index;
                callbackData.element_userdata = _node->shader_userdata;
                callbackData.name = parameter_slider->name;
                callbackData.uniform = parameter_slider->uniform;
                callbackData.type = parameter_slider->type;
                callbackData.color.r = 0.f;
                callbackData.color.g = 0.f;
                callbackData.color.b = 0.f;
                callbackData.value = value;

                (*_composition->providers.shader_property_update)(&callbackData, _composition->provider_userdata);
            }break;
        case AE_MOVIE_EXTENSION_SHADER_PARAMETER_COLOR:
            {
                const struct aeMovieLayerShaderParameterColor * parameter_color = (const struct aeMovieLayerShaderParameterColor *)parameter;

                const struct aeMoviePropertyColor * property_color = parameter_color->property_color;

                ae_color_channel_t color_r = __compute_movie_property_color_channel( property_color->color_channel_r, _frameId, _interpolate, _t );
                ae_color_channel_t color_g = __compute_movie_property_color_channel( property_color->color_channel_g, _frameId, _interpolate, _t );
                ae_color_channel_t color_b = __compute_movie_property_color_channel( property_color->color_channel_b, _frameId, _interpolate, _t );

                aeMovieShaderPropertyUpdateCallbackData callbackData;
                callbackData.index = index;
                callbackData.element_userdata = _node->shader_userdata;
                callbackData.name = parameter_color->name;
                callbackData.uniform = parameter_color->uniform;
                callbackData.type = parameter_color->type;
                callbackData.color.r = color_r;
                callbackData.color.g = color_g;
                callbackData.color.b = color_b;
                callbackData.value = 0.f;

                (*_composition->providers.shader_property_update)(&callbackData, _composition->provider_userdata);
            }break;
        }

        ++index;
    }
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_uint32_t __get_movie_composition_data_node_count( const aeMovieCompositionData * _compositionData )
{
    ae_uint32_t count = _compositionData->layer_count;

    const aeMovieLayerData * it_layer = _compositionData->layers;
    const aeMovieLayerData * it_layer_end = _compositionData->layers + _compositionData->layer_count;
    for( ; it_layer != it_layer_end; ++it_layer )
    {
        const aeMovieLayerData * layer = it_layer;

        aeMovieLayerTypeEnum layer_type = layer->type;

        switch( layer_type )
        {
        case AE_MOVIE_LAYER_TYPE_MOVIE:
        case AE_MOVIE_LAYER_TYPE_SUB_MOVIE:
            {
                ae_uint32_t movie_layer_count = __get_movie_composition_data_node_count( layer->subcomposition_data );

                count += movie_layer_count;
            }break;
        default:
            {
            }break;
        }
    }

    return count;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_bool_t __setup_movie_node_track_matte( aeMovieNode * _nodes, ae_uint32_t * _iterator, const aeMovieCompositionData * _compositionData, aeMovieNode * _trackMatte )
{
    const aeMovieLayerData * it_layer = _compositionData->layers;
    const aeMovieLayerData * it_layer_end = _compositionData->layers + _compositionData->layer_count;
    for( ; it_layer != it_layer_end; ++it_layer )
    {
        const aeMovieLayerData * layer = it_layer;

        aeMovieNode * node = _nodes + ((*_iterator)++);

        aeMovieLayerTypeEnum layer_type = node->layer->type;

        if( _trackMatte == AE_NULLPTR )
        {
            if( layer->has_track_matte == AE_TRUE )
            {
                switch( layer_type )
                {
                case AE_MOVIE_LAYER_TYPE_MOVIE:
                case AE_MOVIE_LAYER_TYPE_SUB_MOVIE:
                    {
                        ae_uint32_t sub_composition_node_count = __get_movie_composition_data_node_count( node->layer->subcomposition_data );

                        aeMovieNode * track_matte_node = _nodes + (*_iterator) + sub_composition_node_count;

                        node->track_matte_node = track_matte_node;
                        node->track_matte_userdata = AE_NULLPTR;
                    }break;
                default:
                    {
                        aeMovieNode * track_matte_node = _nodes + (*_iterator);

                        node->track_matte_node = track_matte_node;
                        node->track_matte_userdata = AE_NULLPTR;
                    }break;
                }
            }
            else
            {
                node->track_matte_node = AE_NULLPTR;
                node->track_matte_userdata = AE_NULLPTR;
            }
        }
        else
        {
            if( layer->has_track_matte == AE_TRUE )
            {
                return AE_FALSE;
            }
            else
            {
                node->track_matte_node = _trackMatte;
                node->track_matte_userdata = AE_NULLPTR;
            }
        }

        switch( layer_type )
        {
        case AE_MOVIE_LAYER_TYPE_MOVIE:
        case AE_MOVIE_LAYER_TYPE_SUB_MOVIE:
            {
                if( __setup_movie_node_track_matte( _nodes, _iterator, layer->subcomposition_data, node->track_matte_node ) == AE_FALSE )
                {
                    return AE_FALSE;
                }
            }break;
        default:
            {
            }break;
        }
    }

    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_bool_t __setup_movie_node_track_matte2( aeMovieComposition * _composition )
{
    uint32_t enumerator = 0U;

    aeMovieNode * it_node = _composition->nodes;
    aeMovieNode * it_node_end = _composition->nodes + _composition->node_count;
    for( ; it_node != it_node_end; ++it_node, ++enumerator )
    {
        aeMovieNode * node = it_node;

        if( node->ignore == AE_TRUE )
        {
            continue;
        }

        const aeMovieLayerData * layer = node->layer;

        if( layer->is_track_matte == AE_TRUE )
        {
            aeMovieRenderMesh mesh;
            __compute_movie_render_mesh( _composition, node, &mesh, _composition->interpolate, AE_TRUE );

            aeMovieTrackMatteProviderCallbackData callbackData;
            callbackData.index = enumerator;
            callbackData.element_userdata = node->element_userdata;
            callbackData.layer = layer;
            callbackData.loop = AE_FALSE;
            callbackData.offset = AE_TIME_OUTSCALE( node->start_time );
            callbackData.immutable_matrix = node->immutable_matrix;
            callbackData.matrix = node->matrix;
            callbackData.immutable_color = node->immutable_color;
            callbackData.color = node->color;
            callbackData.opacity = node->opacity;
            callbackData.mesh = &mesh;
            callbackData.track_matte_mode = layer->track_matte_mode;

            ae_userdata_t track_matte_userdata = AE_USERDATA_NULL;
            if( (*_composition->providers.track_matte_provider)(&callbackData, &track_matte_userdata, _composition->provider_userdata) == AE_FALSE )
            {
                return AE_FALSE;
            }

            node->track_matte_userdata = track_matte_userdata;
        }
        else
        {
            node->track_matte_userdata = AE_NULLPTR;
        }
    }

    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_bool_t __setup_movie_composition_scene_effect( aeMovieComposition * _composition )
{
    _composition->scene_effect_node = AE_NULLPTR;
    _composition->scene_effect_userdata = AE_NULLPTR;

    ae_uint32_t enumerator = 0U;

    aeMovieNode * it_node = _composition->nodes;
    aeMovieNode * it_node_end = _composition->nodes + _composition->node_count;
    for( ; it_node != it_node_end; ++it_node, ++enumerator )
    {
        aeMovieNode * node = it_node;

        if( node->ignore == AE_TRUE )
        {
            continue;
        }

        const aeMovieLayerData * layer = node->layer;

        if( layer->type != AE_MOVIE_LAYER_TYPE_SCENE_EFFECT )
        {
            continue;
        }

        _composition->scene_effect_node = node;

        const aeMovieLayerTransformation2D * transformation2d = (const aeMovieLayerTransformation2D *)layer->transformation;

        aeMovieCompositionSceneEffectProviderCallbackData callbackData;
        callbackData.index = enumerator;
        callbackData.element_userdata = node->element_userdata;

        ae_movie_make_layer_transformation2d_fixed( callbackData.anchor_point, callbackData.position, callbackData.scale, callbackData.quaternion, callbackData.skew, transformation2d, 0 );
        ae_movie_make_layer_transformation_color_fixed( &callbackData.color, &callbackData.opacity, layer->transformation, 0 );

        ae_userdata_t scene_effect_userdata = AE_USERDATA_NULL;
        if( (*_composition->providers.scene_effect_provider)(&callbackData, &scene_effect_userdata, _composition->provider_userdata) == AE_FALSE )
        {
            return AE_FALSE;
        }

        _composition->scene_effect_userdata = scene_effect_userdata;

        break;
    }

    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_uint32_t __get_movie_subcomposition_count( const aeMovieComposition * _composition )
{
    ae_uint32_t count = 0;

    aeMovieNode * it_node = _composition->nodes;
    aeMovieNode * it_node_end = _composition->nodes + _composition->node_count;
    for( ; it_node != it_node_end; ++it_node )
    {
        aeMovieNode * node = it_node;

        if( node->ignore == AE_TRUE )
        {
            continue;
        }

        const aeMovieLayerData * layer = node->layer;

        if( layer->type != AE_MOVIE_LAYER_TYPE_SUB_MOVIE )
        {
            continue;
        }

        ++count;
    }

    return count;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_bool_t __setup_movie_subcomposition2( aeMovieComposition * _composition, ae_uint32_t * _node_iterator, aeMovieSubComposition * _subcompositions, ae_uint32_t * _subcomposition_iterator, const aeMovieCompositionData * _compositionData, const aeMovieSubComposition * _subcomposition )
{
    const aeMovieLayerData * it_layer = _compositionData->layers;
    const aeMovieLayerData * it_layer_end = _compositionData->layers + _compositionData->layer_count;
    for( ; it_layer != it_layer_end; ++it_layer )
    {
        const aeMovieLayerData * layer = it_layer;

        aeMovieNode * node = _composition->nodes + ((*_node_iterator)++);

        node->subcomposition = _subcomposition;

        const aeMovieLayerData * node_layer = node->layer;

        aeMovieLayerTypeEnum layer_type = node_layer->type;

        switch( layer_type )
        {
        case AE_MOVIE_LAYER_TYPE_MOVIE:
            {
                if( __setup_movie_subcomposition2( _composition, _node_iterator, _subcompositions, _subcomposition_iterator, layer->subcomposition_data, _subcomposition ) == AE_FALSE )
                {
                    return AE_FALSE;
                }
            }break;
        case AE_MOVIE_LAYER_TYPE_SUB_MOVIE:
            {
                aeMovieSubComposition * subcomposition = _subcompositions + ((*_subcomposition_iterator)++);

                subcomposition->layer = layer;
                subcomposition->composition_data = layer->composition_data;
                subcomposition->subcomposition_data = layer->subcomposition_data;

                aeMovieCompositionAnimation * animation = AE_NEW( _composition->movie_data->instance, aeMovieCompositionAnimation );

                AE_MOVIE_PANIC_MEMORY( animation, AE_FALSE );

                animation->enable = AE_TRUE;
                animation->play = AE_FALSE;
                animation->pause = AE_FALSE;
                animation->interrupt = AE_FALSE;

                animation->loop = AE_FALSE;

                animation->time = 0.f;

                const aeMovieCompositionData * sub_composition_data = layer->subcomposition_data;

                animation->loop_segment_begin = sub_composition_data->loop_segment[0];
                animation->loop_segment_end = sub_composition_data->loop_segment[1];

                animation->work_area_begin = 0.f;
                animation->work_area_end = sub_composition_data->duration;

                subcomposition->animation = animation;

                aeMovieSubCompositionProviderCallbackData callbackData;
                callbackData.layer = subcomposition->layer;
                callbackData.composition_data = subcomposition->composition_data;
                callbackData.animation = subcomposition->animation;

                ae_userdata_t subcomposition_userdata = AE_USERDATA_NULL;
                if( (*_composition->providers.subcomposition_provider)(&callbackData, &subcomposition_userdata, _composition->provider_userdata) == AE_FALSE )
                {
                    return AE_FALSE;
                }

                subcomposition->subcomposition_userdata = subcomposition_userdata;

                if( __setup_movie_subcomposition2( _composition, _node_iterator, _subcompositions, _subcomposition_iterator, layer->subcomposition_data, subcomposition ) == AE_FALSE )
                {
                    return AE_FALSE;
                }
            }break;
        default:
            {
            }break;
        }
    }

    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_bool_t __setup_movie_subcomposition( aeMovieComposition * _composition )
{
    ae_uint32_t subcomposition_count = __get_movie_subcomposition_count( _composition );

    if( subcomposition_count == 0U )
    {
        _composition->subcomposition_count = 0U;
        _composition->subcompositions = AE_NULLPTR;

        return AE_TRUE;
    }

    aeMovieSubComposition * subcompositions = AE_NEWN( _composition->movie_data->instance, aeMovieSubComposition, subcomposition_count );

    AE_MOVIE_PANIC_MEMORY( subcompositions, AE_FALSE );

    ae_uint32_t node_iterator = 0U;
    ae_uint32_t subcomposition_iterator = 0U;

    if( __setup_movie_subcomposition2( _composition, &node_iterator, subcompositions, &subcomposition_iterator, _composition->composition_data, AE_NULLPTR ) == AE_FALSE )
    {
        return AE_FALSE;
    }

    _composition->subcomposition_count = subcomposition_count;
    _composition->subcompositions = subcompositions;

    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __setup_movie_node_initialize( aeMovieNode * _nodes, uint32_t _count )
{
    aeMovieNode *it_node = _nodes;
    aeMovieNode *it_node_end = _nodes + _count;
    for( ; it_node != it_node_end; ++it_node )
    {
        aeMovieNode * node = it_node;

        node->subcomposition = AE_NULLPTR;
        node->volume = 1.f;
    }
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __setup_movie_node_relative( aeMovieNode * _nodes, ae_uint32_t * _iterator, const aeMovieCompositionData * _compositionData, aeMovieNode * _parent )
{
    ae_uint32_t begin_index = *_iterator;

    const aeMovieLayerData *it_layer = _compositionData->layers;
    const aeMovieLayerData *it_layer_end = _compositionData->layers + _compositionData->layer_count;
    for( ; it_layer != it_layer_end; ++it_layer )
    {
        const aeMovieLayerData * layer = it_layer;

        aeMovieNode * node = _nodes + ((*_iterator)++);

        node->layer = layer;
        node->update_revision = 0;

        node->active = AE_FALSE;
        node->ignore = AE_FALSE;
        node->enable = AE_TRUE;
        node->animate = AE_MOVIE_NODE_ANIMATE_STATIC;

        if( layer->parent_index == 0 )
        {
            node->relative_node = _parent;
        }

        node->current_time = 0.f;
        node->current_frame = 0;
        node->current_frame_t = 0.f;

        switch( layer->type )
        {
        case AE_MOVIE_LAYER_TYPE_MOVIE:
        case AE_MOVIE_LAYER_TYPE_SUB_MOVIE:
            {
                __setup_movie_node_relative( _nodes, _iterator, layer->subcomposition_data, node );
            }break;
        default:
            {
            }break;
        }
    }

    ae_uint32_t end_index = *_iterator;

    const aeMovieLayerData *it_layer2 = _compositionData->layers;
    const aeMovieLayerData *it_layer2_end = _compositionData->layers + _compositionData->layer_count;
    for( ; it_layer2 != it_layer2_end; ++it_layer2 )
    {
        const aeMovieLayerData * layer = it_layer2;

        ae_uint32_t parent_index = layer->parent_index;

        if( parent_index == 0 )
        {
            continue;
        }

        aeMovieNode * node = __find_node_by_layer( _nodes, begin_index, end_index, layer );

        const aeMovieLayerData * parent_layer = __find_layer_by_index( _compositionData, parent_index );

        aeMovieNode * parent_node = __find_node_by_layer( _nodes, begin_index, end_index, parent_layer );

        node->relative_node = parent_node;
    }
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_bool_t __is_layer_super_immutable_transform( const aeMovieLayerData * _layer )
{
    const struct aeMovieLayerTransformation * layer_transformation = _layer->transformation;

    return (layer_transformation->immutable_property_mask & AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL) == AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_bool_t __is_node_super_immutable_transform( const aeMovieNode * _node )
{
    const aeMovieLayerData * node_layer = _node->layer;

    ae_bool_t super_immutable = __is_layer_super_immutable_transform( node_layer );

    if( super_immutable == AE_FALSE )
    {
        return AE_FALSE;
    }

    struct aeMovieNode * relative_node = _node->relative_node;

    if( relative_node == AE_NULLPTR )
    {
        return AE_TRUE;
    }    

    ae_bool_t parent_super_immutable = __is_node_super_immutable_transform( relative_node );

    return parent_super_immutable;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __setup_movie_node_transform_immutable( aeMovieComposition * _composition )
{
    aeMovieNode* it_node = _composition->nodes;
    aeMovieNode* it_node_end = _composition->nodes + _composition->node_count;
    for( ; it_node != it_node_end; ++it_node )
    {
        aeMovieNode * node = it_node;

        ae_bool_t immutable_transform =  __is_node_super_immutable_transform( node );

        node->immutable_matrix = immutable_transform;
    }
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_bool_t __is_layer_super_immutable_color( const aeMovieLayerData * _layer )
{
    const struct aeMovieLayerTransformation * layer_transformation = _layer->transformation;

    return (layer_transformation->immutable_property_mask & AE_MOVIE_PROPERTY_COLOR_SUPER_ALL) == AE_MOVIE_PROPERTY_COLOR_SUPER_ALL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_bool_t __is_node_super_immutable_color( const aeMovieNode * _node )
{
    const aeMovieLayerData * node_layer = _node->layer;

    ae_bool_t super_immutable = __is_layer_super_immutable_color( node_layer );

    if( super_immutable == AE_FALSE )
    {
        return AE_FALSE;
    }

    struct aeMovieNode * relative_node = _node->relative_node;

    if( relative_node == AE_NULLPTR )
    {
        return AE_TRUE;
    }

    ae_bool_t parent_super_immutable = __is_node_super_immutable_color( relative_node );

    return parent_super_immutable;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __setup_movie_node_color_immutable( aeMovieComposition * _composition )
{
    aeMovieNode* it_node = _composition->nodes;
    aeMovieNode* it_node_end = _composition->nodes + _composition->node_count;
    for( ; it_node != it_node_end; ++it_node )
    {
        aeMovieNode * node = it_node;

        ae_bool_t immutable_color = __is_node_super_immutable_color( node );

        node->immutable_color = immutable_color;
    }
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __setup_movie_node_time( aeMovieNode * _nodes, ae_uint32_t * _iterator, const aeMovieCompositionData * _compositionData, const aeMovieNode * _parent, ae_float_t _stretch, ae_float_t _startTime )
{
    const aeMovieLayerData *it_layer = _compositionData->layers;
    const aeMovieLayerData *it_layer_end = _compositionData->layers + _compositionData->layer_count;
    for( ; it_layer != it_layer_end; ++it_layer )
    {
        const aeMovieLayerData * layer = it_layer;

        aeMovieNode * node = _nodes + ((*_iterator)++);

        if( _parent == AE_NULLPTR )
        {
            node->start_time = 0.f;
            node->in_time = layer->in_time;
            node->out_time = layer->out_time;
        }
        else
        {
            ae_float_t layer_in = layer->in_time * _stretch - _startTime;
            ae_float_t parent_in = _parent->in_time;

            if( parent_in > layer_in )
            {
                node->start_time = parent_in - layer_in;
                node->in_time = parent_in;
            }
            else
            {
                node->start_time = 0.f;
                node->in_time = layer_in;
            }

            ae_float_t layer_out = layer->out_time * _stretch - _startTime;
            ae_float_t parent_out = _parent->out_time;

            if( node->subcomposition == _parent->subcomposition )
            {
                node->out_time = ae_min_f_f( layer_out, parent_out );
            }
            else
            {
                node->out_time = layer->out_time;
            }

            if( node->out_time <= 0.f || node->out_time < node->in_time )
            {
                node->in_time = 0.f;
                node->out_time = 0.f;
                node->ignore = AE_TRUE;
            }
        }

        ae_float_t to_stretch = _stretch * layer->stretch;

        switch( layer->type )
        {
        case AE_MOVIE_LAYER_TYPE_MOVIE:
        case AE_MOVIE_LAYER_TYPE_SUB_MOVIE:
            {
                node->stretchInv = 1.f / _stretch;

                ae_float_t to_startTime = _startTime + layer->start_time - layer->in_time;

                __setup_movie_node_time( _nodes, _iterator, layer->subcomposition_data, node, to_stretch, to_startTime );
            }break;
        default:
            {
                node->stretchInv = 1.f / to_stretch;
            }break;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __setup_movie_node_incessantly( aeMovieComposition * _composition )
{
    const aeMovieCompositionData * composition_data = _composition->composition_data;
    ae_time_t duration = composition_data->duration;

    aeMovieNode *it_node = _composition->nodes;
    aeMovieNode *it_node_end = _composition->nodes + _composition->node_count;
    for( ; it_node != it_node_end; ++it_node )
    {
        aeMovieNode * node = it_node;

        const aeMovieLayerData * layer = node->layer;

        if( layer->type == AE_MOVIE_LAYER_TYPE_SUB_MOVIE )
        {
            node->incessantly = AE_TRUE;

            continue;
        }

        if( ae_has_movie_layer_data_option( layer, AE_OPTION( 'l', 'o', 'o', 'p' ) ) == AE_TRUE )
        {
            node->incessantly = AE_TRUE;

            continue;
        }

        if( layer->trimmed_time == AE_FALSE &&
            ae_equal_f_z( node->start_time ) == AE_TRUE &&
            ae_equal_f_z( node->in_time ) == AE_TRUE &&
            ae_equal_f_f( node->out_time, duration ) == AE_TRUE )
        {
            node->incessantly = AE_TRUE;
        }
        else
        {
            node->incessantly = AE_FALSE;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __setup_movie_node_incessantly2( aeMovieNode * _nodes, ae_uint32_t * _iterator, const aeMovieCompositionData * _compositionData, ae_bool_t _incessantly )
{
    const aeMovieLayerData *it_layer = _compositionData->layers;
    const aeMovieLayerData *it_layer_end = _compositionData->layers + _compositionData->layer_count;
    for( ; it_layer != it_layer_end; ++it_layer )
    {
        const aeMovieLayerData * layer = it_layer;

        aeMovieNode * node = _nodes + ((*_iterator)++);

        if( _incessantly == AE_TRUE )
        {
            node->incessantly = AE_TRUE;
        }

        switch( layer->type )
        {
        case AE_MOVIE_LAYER_TYPE_MOVIE:
        case AE_MOVIE_LAYER_TYPE_SUB_MOVIE:
            {
                ae_bool_t layer_incessantly = ae_is_movie_layer_data_incessantly( layer );

                if( _incessantly == AE_TRUE )
                {
                    layer_incessantly = AE_TRUE;
                }

                __setup_movie_node_incessantly2( _nodes, _iterator, layer->subcomposition_data, layer_incessantly );
            }break;
        default:
            {
            }break;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __setup_movie_node_blend_mode( aeMovieNode * _nodes, ae_uint32_t * _iterator, const aeMovieCompositionData * _compositionData, const aeMovieNode * _parent, ae_blend_mode_t _blendMode )
{
    AE_UNUSED( _parent ); //TODO

    const aeMovieLayerData *it_layer = _compositionData->layers;
    const aeMovieLayerData *it_layer_end = _compositionData->layers + _compositionData->layer_count;
    for( ; it_layer != it_layer_end; ++it_layer )
    {
        const aeMovieLayerData * layer = it_layer;

        aeMovieNode * node = _nodes + ((*_iterator)++);

        if( _blendMode != AE_MOVIE_BLEND_NORMAL )
        {
            node->blend_mode = _blendMode;
        }
        else
        {
            node->blend_mode = layer->blend_mode;
        }

        ae_blend_mode_t composition_blend_mode = AE_MOVIE_BLEND_NORMAL;

        if( layer->subcomposition_data != AE_NULLPTR )
        {
            composition_blend_mode = node->blend_mode;
        }

        switch( layer->type )
        {
        case AE_MOVIE_LAYER_TYPE_MOVIE:
        case AE_MOVIE_LAYER_TYPE_SUB_MOVIE:
            {
                __setup_movie_node_blend_mode( _nodes, _iterator, layer->subcomposition_data, node, composition_blend_mode );
            }break;
        default:
            {
            }break;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __setup_movie_node_camera2( aeMovieComposition * _composition, ae_uint32_t * _iterator, const aeMovieCompositionData * _compositionData, ae_userdata_t _userdata )
{
    const aeMovieLayerData *it_layer = _compositionData->layers;
    const aeMovieLayerData *it_layer_end = _compositionData->layers + _compositionData->layer_count;
    for( ; it_layer != it_layer_end; ++it_layer )
    {
        const aeMovieLayerData * layer = it_layer;

        aeMovieNode * node = _composition->nodes + ((*_iterator)++);

        if( layer->threeD == AE_TRUE )
        {
            node->camera_userdata = _composition->camera_userdata;
        }
        else
        {
            node->camera_userdata = _userdata;
        }

        switch( layer->type )
        {
        case AE_MOVIE_LAYER_TYPE_SUB_MOVIE:
        case AE_MOVIE_LAYER_TYPE_MOVIE:
            {
                __setup_movie_node_camera2( _composition, _iterator, layer->subcomposition_data, node->camera_userdata );
            }break;
        default:
            {
            }break;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_bool_t __setup_movie_node_camera( aeMovieComposition * _composition )
{
    const aeMovieCompositionData * composition_data = _composition->composition_data;

    if( composition_data->camera == AE_NULLPTR )
    {
        _composition->camera_userdata = AE_NULLPTR;

        aeMovieNode *it_node = _composition->nodes;
        aeMovieNode *it_node_end = _composition->nodes + _composition->node_count;
        for( ; it_node != it_node_end; ++it_node )
        {
            aeMovieNode * node = it_node;

            node->camera_userdata = AE_NULLPTR;
        }

        return AE_TRUE;
    }

    const aeMovieCompositionCamera * camera = composition_data->camera;

    ae_float_t width = composition_data->width;
    ae_float_t height = composition_data->height;

    aeMovieCameraProviderCallbackData callbackData;
    callbackData.name = composition_data->name;
    callbackData.fov = camera->fov;
    callbackData.width = width;
    callbackData.height = height;

    ae_movie_make_camera_transformation( callbackData.target, callbackData.position, callbackData.quaternion, camera, 0, AE_FALSE, 0.f );

    ae_userdata_t camera_userdata = AE_USERDATA_NULL;
    if( (*_composition->providers.camera_provider)(&callbackData, &camera_userdata, _composition->provider_userdata) == AE_FALSE )
    {
        return AE_FALSE;
    }

    _composition->camera_userdata = camera_userdata;

    ae_uint32_t node_camera_iterator = 0U;
    __setup_movie_node_camera2( _composition, &node_camera_iterator, composition_data, AE_NULLPTR );

    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __setup_movie_node_viewport2( aeMovieComposition * _composition, ae_uint32_t * _iterator, const aeMovieCompositionData * _compositionData, const ae_viewport_t * _viewport )
{
    const aeMovieLayerData *it_layer = _compositionData->layers;
    const aeMovieLayerData *it_layer_end = _compositionData->layers + _compositionData->layer_count;
    for( ; it_layer != it_layer_end; ++it_layer )
    {
        const aeMovieLayerData * layer = it_layer;

        aeMovieNode * node = _composition->nodes + ((*_iterator)++);

        if( layer->extensions->viewport != AE_NULLPTR )
        {
            node->viewport = &layer->extensions->viewport->viewport;
        }
        else
        {
            node->viewport = _viewport;
        }

        switch( layer->type )
        {
        case AE_MOVIE_LAYER_TYPE_SUB_MOVIE:
        case AE_MOVIE_LAYER_TYPE_MOVIE:
            {
                __setup_movie_node_viewport2( _composition, _iterator, layer->subcomposition_data, node->viewport );
            }break;
        default:
            {
            }break;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __setup_movie_node_viewport( aeMovieComposition * _composition )
{
    const aeMovieCompositionData * composition_data = _composition->composition_data;

    ae_uint32_t node_camera_iterator = 0U;
    __setup_movie_node_viewport2( _composition, &node_camera_iterator, composition_data, AE_NULLPTR );
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __setup_movie_node_matrix2( const aeMovieComposition * _composition, const aeMovieCompositionData * _compositionData, const aeMovieCompositionAnimation * _animation, const aeMovieSubComposition * _subcomposition )
{
    ae_uint32_t update_revision = __get_composition_update_revision( _composition );

    aeMovieNode *it_node = _composition->nodes;
    aeMovieNode *it_node_end = _composition->nodes + _composition->node_count;
    for( ; it_node != it_node_end; ++it_node )
    {
        aeMovieNode * node = it_node;

        if( node->subcomposition != _subcomposition )
        {
            continue;
        }

        const aeMovieLayerData * node_layer = node->layer;

        ae_float_t t = 0.f;
        ae_uint32_t frameId = __get_movie_frame_time( _animation, node, _composition->interpolate, &t );

        ae_bool_t node_interpolate = (frameId + 1 == node_layer->frame_count) ? AE_FALSE : _composition->interpolate;

        if( (node_layer->transformation->identity_property_mask & AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL) == AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL )
        { 
            ae_ident_m34( node->matrix );
        }

        __update_movie_composition_node_matrix( node, update_revision, _composition, _compositionData, _animation, _subcomposition, frameId, node_interpolate, t );
    }
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __setup_movie_node_matrix( const aeMovieComposition * _composition )
{
    const aeMovieCompositionData * composition_data = _composition->composition_data;
    const aeMovieCompositionAnimation * animation = _composition->animation;

    __setup_movie_node_matrix2( _composition, composition_data, animation, AE_NULLPTR );

    const aeMovieSubComposition *it_subcomposition = _composition->subcompositions;
    const aeMovieSubComposition *it_subcomposition_end = _composition->subcompositions + _composition->subcomposition_count;

    for( ; it_subcomposition != it_subcomposition_end; ++it_subcomposition )
    {
        const aeMovieSubComposition * subcomposition = it_subcomposition;

        const aeMovieCompositionData * subcomposition_composition_data = subcomposition->composition_data;
        aeMovieCompositionAnimation * subcomposition_animation = subcomposition->animation;

        __setup_movie_node_matrix2( _composition, subcomposition_composition_data, subcomposition_animation, subcomposition );
    }
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_bool_t __setup_movie_node_shader( aeMovieComposition * _composition )
{
    aeMovieNode* it_node = _composition->nodes;
    aeMovieNode* it_node_end = _composition->nodes + _composition->node_count;
    for( ; it_node != it_node_end; ++it_node )
    {
        aeMovieNode * node = it_node;

        const aeMovieLayerData * layer = node->layer;

        if( layer->extensions->shader == AE_NULLPTR )
        {
            node->shader_userdata = AE_NULLPTR;

            continue;
        }

        const aeMovieLayerExtensionShader * shader = layer->extensions->shader;

        aeMovieShaderProviderCallbackData callbackData;
        callbackData.name = shader->name;
        callbackData.version = shader->version;
        callbackData.shader_vertex = shader->shader_vertex;
        callbackData.shader_fragment = shader->shader_fragment;
        callbackData.parameter_count = shader->parameter_count;

        const struct aeMovieLayerShaderParameter ** it_parameter = shader->parameters;
        const struct aeMovieLayerShaderParameter ** it_parameter_end = shader->parameters + shader->parameter_count;

        ae_uint32_t paremeter_index = 0;

        for( ;
            it_parameter != it_parameter_end;
            ++it_parameter )
        {
            const struct aeMovieLayerShaderParameter * parameter = *it_parameter;

            callbackData.parameter_names[paremeter_index] = parameter->name;
            callbackData.parameter_uniforms[paremeter_index] = parameter->uniform;
            callbackData.parameter_types[paremeter_index] = parameter->type;
            paremeter_index++;
        }

        ae_userdata_t shader_userdata = AE_USERDATA_NULL;
        if( (*_composition->providers.shader_provider)(&callbackData, &shader_userdata, _composition->provider_userdata) == AE_FALSE )
        {
            return AE_FALSE;
        }

        node->shader_userdata = shader_userdata;
    }

    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_bool_t __setup_movie_composition_element( aeMovieComposition * _composition )
{
    ae_uint32_t enumerator = 0U;

    aeMovieNode* it_node = _composition->nodes;
    aeMovieNode* it_node_end = _composition->nodes + _composition->node_count;
    for( ; it_node != it_node_end; ++it_node, ++enumerator )
    {
        aeMovieNode * node = it_node;

        const aeMovieLayerData * track_matte_layer = node->track_matte_node == AE_NULLPTR ? AE_NULLPTR : node->track_matte_node->layer;

        aeMovieNodeProviderCallbackData callbackData;
        callbackData.index = enumerator;
        callbackData.layer = node->layer;
        callbackData.incessantly = node->incessantly;
        callbackData.immutable_matrix = node->immutable_matrix;
        callbackData.matrix = node->matrix;
        callbackData.color = node->color;
        callbackData.opacity = node->opacity;
        callbackData.volume = node->volume;
        callbackData.track_matte_layer = track_matte_layer;

        ae_userdata_t element_userdata = AE_USERDATA_NULL;
        if( (*_composition->providers.node_provider)(&callbackData, &element_userdata, _composition->provider_userdata) == AE_FALSE )
        {
            return AE_FALSE;
        }

        node->element_userdata = element_userdata;
    }

    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __setup_movie_composition_active( aeMovieComposition * _composition )
{
    aeMovieNode *it_node = _composition->nodes;
    aeMovieNode *it_node_end = _composition->nodes + _composition->node_count;
    for( ; it_node != it_node_end; ++it_node )
    {
        aeMovieNode * node = it_node;

        if( node->ignore == AE_TRUE )
        {
            continue;
        }

        if( ae_equal_f_z( node->in_time ) == AE_TRUE )
        {
            node->active = AE_TRUE;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
const aeMovieComposition * ae_create_movie_composition( const aeMovieData * _movieData, const aeMovieCompositionData * _compositionData, ae_bool_t _interpolate, const aeMovieCompositionProviders * _providers, ae_userdata_t _userdata )
{
    aeMovieComposition * composition = AE_NEW( _movieData->instance, aeMovieComposition );

    AE_MOVIE_PANIC_MEMORY( composition, AE_NULLPTR );

    composition->movie_data = _movieData;
    composition->composition_data = _compositionData;

    aeMovieCompositionAnimation * animation = AE_NEW( _movieData->instance, aeMovieCompositionAnimation );

    AE_MOVIE_PANIC_MEMORY( animation, AE_NULLPTR );

    animation->enable = AE_TRUE;
    animation->play = AE_FALSE;
    animation->pause = AE_FALSE;
    animation->interrupt = AE_FALSE;
    animation->loop = AE_FALSE;

    animation->time = 0.f;
    animation->loop_segment_begin = _compositionData->loop_segment[0];
    animation->loop_segment_end = _compositionData->loop_segment[1];
    animation->work_area_begin = 0.f;
    animation->work_area_end = _compositionData->duration;

    composition->animation = animation;

    ae_uint32_t * update_revision = AE_NEW( _movieData->instance, ae_uint32_t );

    AE_MOVIE_PANIC_MEMORY( update_revision, AE_NULLPTR );

    *update_revision = 0;

    composition->update_revision = update_revision;

    composition->interpolate = _interpolate;

    ae_uint32_t node_count = __get_movie_composition_data_node_count( _compositionData );

    composition->node_count = node_count;

    aeMovieNode * nodes = AE_NEWN( _movieData->instance, aeMovieNode, node_count );

    AE_MOVIE_PANIC_MEMORY( nodes, AE_NULLPTR );

    __setup_movie_node_initialize( nodes, node_count );

    composition->nodes = nodes;

    composition->providers = *_providers;
    composition->provider_userdata = _userdata;

    ae_uint32_t node_relative_iterator = 0U;
    __setup_movie_node_relative( composition->nodes, &node_relative_iterator, _compositionData, AE_NULLPTR );

    __setup_movie_node_transform_immutable( composition );
    __setup_movie_node_color_immutable( composition );

    if( __setup_movie_subcomposition( composition ) == AE_FALSE )
    {
        return AE_NULLPTR;
    }

    ae_uint32_t node_time_iterator = 0U;
    __setup_movie_node_time( composition->nodes, &node_time_iterator, _compositionData, AE_NULLPTR, 1.f, 0.f );

    __setup_movie_node_incessantly( composition );

    ae_uint32_t node_incessantly2_iterator = 0U;
    __setup_movie_node_incessantly2( composition->nodes, &node_incessantly2_iterator, _compositionData, AE_FALSE );

    ae_uint32_t node_blend_mode_iterator = 0U;
    __setup_movie_node_blend_mode( composition->nodes, &node_blend_mode_iterator, _compositionData, AE_NULLPTR, AE_MOVIE_BLEND_NORMAL );

    __inc_composition_update_revision( composition );

    if( __setup_movie_node_camera( composition ) == AE_FALSE )
    {
        return AE_NULLPTR;
    }

    __setup_movie_node_matrix( composition );

    __setup_movie_node_viewport( composition );

    __setup_movie_composition_active( composition );

    ae_uint32_t node_track_matte_iterator = 0;
    if( __setup_movie_node_track_matte( composition->nodes, &node_track_matte_iterator, _compositionData, AE_NULLPTR ) == AE_FALSE )
    {
        return AE_NULLPTR;
    }

    if( __setup_movie_node_shader( composition ) == AE_FALSE )
    {
        return AE_NULLPTR;
    }

    if( __setup_movie_composition_element( composition ) == AE_FALSE )
    {
        return AE_NULLPTR;
    }

    if( __setup_movie_node_track_matte2( composition ) == AE_FALSE )
    {
        return AE_NULLPTR;
    }

    if( __setup_movie_composition_scene_effect( composition ) == AE_FALSE )
    {
        return AE_NULLPTR;
    }

    return composition;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __delete_nodes( const aeMovieComposition * _composition )
{
    ae_uint32_t enumerator = 0U;

    const aeMovieNode *it_node = _composition->nodes;
    const aeMovieNode *it_node_end = _composition->nodes + _composition->node_count;
    for( ; it_node != it_node_end; ++it_node, ++enumerator )
    {
        const aeMovieNode * node = it_node;

        const aeMovieLayerData * layer = node->layer;

        if( node->shader_userdata != AE_NULLPTR )
        {
            aeMovieShaderDeleterCallbackData callbackData;
            callbackData.index = enumerator;
            callbackData.element_userdata = node->shader_userdata;
            callbackData.name = layer->extensions->shader->name;
            callbackData.version = layer->extensions->shader->version;

            (*_composition->providers.shader_deleter)(&callbackData, _composition->provider_userdata);
        }

        if( layer->is_track_matte == AE_TRUE )
        {
            aeMovieTrackMatteDeleterCallbackData callbackData;
            callbackData.index = enumerator;
            callbackData.element_userdata = node->element_userdata;
            callbackData.layer = layer;
            callbackData.track_matte_userdata = node->track_matte_userdata;

            (*_composition->providers.track_matte_deleter)(&callbackData, _composition->provider_userdata);
        }

        const aeMovieLayerData * track_matte_layer = node->track_matte_node == AE_NULLPTR ? AE_NULLPTR : node->track_matte_node->layer;

        aeMovieNodeDeleterCallbackData callbackData;
        callbackData.index = enumerator;
        callbackData.element_userdata = node->element_userdata;
        callbackData.layer = layer;
        callbackData.track_matte_layer = track_matte_layer;

        (*_composition->providers.node_deleter)(&callbackData, _composition->provider_userdata);
    }
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __delete_camera( const aeMovieComposition * _composition )
{
    if( _composition->camera_userdata == AE_NULLPTR )
    {
        return;
    }

    const aeMovieCompositionData * composition_data = _composition->composition_data;
    const aeMovieCompositionCamera * camera = composition_data->camera;

    aeMovieCameraDeleterCallbackData callbackData;
    callbackData.name = camera->name;
    callbackData.camera_userdata = _composition->camera_userdata;

    (*_composition->providers.camera_deleter)(&callbackData, _composition->provider_userdata);
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __delete_scene_effect( const aeMovieComposition * _composition )
{
    if( _composition->scene_effect_node == AE_NULLPTR )
    {
        return;
    }

    aeMovieNode * scene_effect_node = _composition->scene_effect_node;

    aeMovieCompositionSceneEffectDeleterCallbackData callbackData;
    callbackData.element_userdata = scene_effect_node->element_userdata;
    callbackData.scene_effect_userdata = _composition->scene_effect_userdata;

    (*_composition->providers.scene_effect_deleter)(&callbackData, _composition->provider_userdata);
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_delete_movie_composition( const aeMovieComposition * _composition )
{
    __delete_nodes( _composition );
    __delete_camera( _composition );
    __delete_scene_effect( _composition );

    const aeMovieInstance * instance = _composition->movie_data->instance;

    const aeMovieSubComposition *it_subcomposition = _composition->subcompositions;
    const aeMovieSubComposition *it_subcomposition_end = _composition->subcompositions + _composition->subcomposition_count;
    for( ; it_subcomposition != it_subcomposition_end; ++it_subcomposition )
    {
        const aeMovieSubComposition * subcomposition = it_subcomposition;

        aeMovieSubCompositionDeleterCallbackData callbackData;
        callbackData.subcomposition_userdata = subcomposition->subcomposition_userdata;

        (*_composition->providers.subcomposition_deleter)(&callbackData, _composition->provider_userdata);  

        AE_DELETE( instance, subcomposition->animation );
    }

    AE_DELETEN( instance, _composition->subcompositions );

    AE_DELETEN( instance, _composition->nodes );

    AE_DELETE( instance, _composition->animation );

    AE_DELETE( instance, _composition->update_revision );

    AE_DELETE( instance, _composition );
}
//////////////////////////////////////////////////////////////////////////
const aeMovieCompositionData * ae_get_movie_composition_composition_data( const aeMovieComposition * _composition )
{
    const aeMovieCompositionData * composition_data = _composition->composition_data;

    return composition_data;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_get_movie_composition_anchor_point( const aeMovieComposition * _composition, ae_vector3_t _point )
{
    const aeMovieCompositionData * composition_data = _composition->composition_data;

    if( composition_data->flags & AE_MOVIE_COMPOSITION_ANCHOR_POINT )
    {
        _point[0] = composition_data->anchor_point[0];
        _point[1] = composition_data->anchor_point[1];
        _point[2] = composition_data->anchor_point[2];

        return AE_TRUE;
    }

    return AE_FALSE;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_uint32_t __mesh_max_vertex_count( const aeMovieLayerExtensionMesh * _mesh, ae_uint32_t _count )
{
    if( _mesh->immutable == AE_TRUE )
    {
        return _mesh->immutable_mesh.vertex_count;
    }

    ae_uint32_t max_vertex_count = 0U;

    const ae_mesh_t * it_mesh = _mesh->meshes;
    const ae_mesh_t * it_mesh_end = _mesh->meshes + _count;
    for( ; it_mesh != it_mesh_end; ++it_mesh )
    {
        const ae_mesh_t * mesh = it_mesh;

        if( max_vertex_count < mesh->vertex_count )
        {
            max_vertex_count = mesh->vertex_count;
        }
    }

    return max_vertex_count;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_uint32_t __mesh_max_index_count( const aeMovieLayerExtensionMesh * _mesh, ae_uint32_t _count )
{
    if( _mesh->immutable == AE_TRUE )
    {
        return _mesh->immutable_mesh.index_count;
    }

    ae_uint32_t max_index_count = 0U;

    const ae_mesh_t * it_mesh = _mesh->meshes;
    const ae_mesh_t * it_mesh_end = _mesh->meshes + _count;
    for( ; it_mesh != it_mesh_end; ++it_mesh )
    {
        const ae_mesh_t * mesh = it_mesh;

        if( max_index_count < mesh->index_count )
        {
            max_index_count = mesh->index_count;
        }
    }

    return max_index_count;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_uint32_t __resource_sequence_images_max_vertex_count( const aeMovieResourceSequence * _resource_sequence )
{
    ae_uint32_t max_vertex_count = 0U;

    const aeMovieResourceImage * const * it_image = _resource_sequence->images;
    const aeMovieResourceImage * const * it_image_end = _resource_sequence->images + _resource_sequence->image_count;
    for( ; it_image != it_image_end; ++it_image )
    {
        const aeMovieResourceImage * image = *it_image;

        const ae_mesh_t * mesh = image->mesh;

        if( mesh == AE_NULLPTR )
        {
            if( max_vertex_count < 6 )
            {
                max_vertex_count = 6;
            }
        }
        else
        {
            if( max_vertex_count < mesh->vertex_count )
            {
                max_vertex_count = mesh->vertex_count;
            }
        }
    }

    return max_vertex_count;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_uint32_t __resource_sequence_images_max_index_count( const aeMovieResourceSequence * _resource_sequence )
{
    ae_uint32_t max_index_count = 0U;

    const aeMovieResourceImage * const * it_image = _resource_sequence->images;
    const aeMovieResourceImage * const * it_image_end = _resource_sequence->images + _resource_sequence->image_count;
    for( ; it_image != it_image_end; ++it_image )
    {
        const aeMovieResourceImage * image = *it_image;

        const ae_mesh_t * mesh = image->mesh;

        if( mesh == AE_NULLPTR )
        {
            if( max_index_count < 6 )
            {
                max_index_count = 6;
            }
        }
        else
        {
            if( max_index_count < mesh->index_count )
            {
                max_index_count = mesh->index_count;
            }
        }
    }

    return max_index_count;
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_calculate_movie_composition_render_info( const aeMovieComposition * _composition, aeMovieCompositionRenderInfo * _info )
{
    _info->max_render_node = 0U;
    _info->max_vertex_count = 0U;
    _info->max_index_count = 0U;

    ae_uint32_t node_count = _composition->node_count;

    ae_uint32_t iterator = 0U;
    for( ; iterator != node_count; ++iterator )
    {
        const aeMovieNode * node = _composition->nodes + iterator;

        const aeMovieLayerData * layer = node->layer;

        if( layer->renderable == AE_FALSE )
        {
            continue;
        }

        ++_info->max_render_node;

        aeMovieLayerTypeEnum layer_type = layer->type;
        const aeMovieResource * layer_resource = layer->resource;

        switch( layer_type )
        {
        case AE_MOVIE_LAYER_TYPE_SHAPE:
            {
                _info->max_vertex_count += __mesh_max_vertex_count( layer->extensions->mesh, layer->frame_count );
                _info->max_index_count += __mesh_max_index_count( layer->extensions->mesh, layer->frame_count );
            }break;
        case AE_MOVIE_LAYER_TYPE_SOLID:
            {
                //aeMovieResourceSolid * resource_solid = (aeMovieResourceSolid *)resource;

                if( layer->extensions->mesh != AE_NULLPTR )
                {
                    _info->max_vertex_count += __mesh_max_vertex_count( layer->extensions->mesh, layer->frame_count );
                    _info->max_index_count += __mesh_max_index_count( layer->extensions->mesh, layer->frame_count );
                }
                else if( layer->extensions->bezier_warp != AE_NULLPTR )
                {
                    _info->max_vertex_count += get_bezier_warp_vertex_count( layer->extensions->bezier_warp->quality );
                    _info->max_index_count += get_bezier_warp_index_count( layer->extensions->bezier_warp->quality );
                }
                else
                {
                    _info->max_vertex_count += 4U;
                    _info->max_index_count += 6U;
                }
            }break;
        case AE_MOVIE_LAYER_TYPE_SEQUENCE:
            {
                aeMovieResourceSequence * resource_sequence = (aeMovieResourceSequence *)layer_resource;

                if( layer->extensions->mesh != AE_NULLPTR )
                {
                    _info->max_vertex_count += __mesh_max_vertex_count( layer->extensions->mesh, layer->frame_count );
                    _info->max_index_count += __mesh_max_index_count( layer->extensions->mesh, layer->frame_count );
                }
                else if( layer->extensions->bezier_warp != AE_NULLPTR )
                {
                    _info->max_vertex_count += get_bezier_warp_vertex_count( layer->extensions->bezier_warp->quality );
                    _info->max_index_count += get_bezier_warp_index_count( layer->extensions->bezier_warp->quality );
                }
                else
                {
                    _info->max_vertex_count += __resource_sequence_images_max_vertex_count( resource_sequence );
                    _info->max_index_count += __resource_sequence_images_max_index_count( resource_sequence );
                }
            }break;
        case AE_MOVIE_LAYER_TYPE_VIDEO:
            {
                //aeMovieResourceVideo * resource_video = (aeMovieResourceVideo *)resource;

                if( layer->extensions->mesh != AE_NULLPTR )
                {
                    _info->max_vertex_count += __mesh_max_vertex_count( layer->extensions->mesh, layer->frame_count );
                    _info->max_index_count += __mesh_max_index_count( layer->extensions->mesh, layer->frame_count );
                }
                else if( layer->extensions->bezier_warp != AE_NULLPTR )
                {
                    _info->max_vertex_count += get_bezier_warp_vertex_count( layer->extensions->bezier_warp->quality );
                    _info->max_index_count += get_bezier_warp_index_count( layer->extensions->bezier_warp->quality );
                }
                else
                {
                    _info->max_vertex_count += 4U;
                    _info->max_index_count += 6U;
                }
            }break;
        case AE_MOVIE_LAYER_TYPE_IMAGE:
            {
                aeMovieResourceImage * resource_image = (aeMovieResourceImage *)layer_resource;

                if( layer->extensions->mesh != AE_NULLPTR )
                {
                    _info->max_vertex_count += __mesh_max_vertex_count( layer->extensions->mesh, layer->frame_count );
                    _info->max_index_count += __mesh_max_index_count( layer->extensions->mesh, layer->frame_count );
                }
                else if( layer->extensions->bezier_warp != AE_NULLPTR )
                {
                    _info->max_vertex_count += get_bezier_warp_vertex_count( layer->extensions->bezier_warp->quality );
                    _info->max_index_count += get_bezier_warp_index_count( layer->extensions->bezier_warp->quality );
                }
                else if( resource_image->mesh != AE_NULLPTR )
                {
                    const ae_mesh_t * mesh = resource_image->mesh;

                    _info->max_vertex_count += mesh->vertex_count;
                    _info->max_index_count += mesh->index_count;
                }
                else
                {
                    _info->max_vertex_count += 4U;
                    _info->max_index_count += 6U;
                }
            }break;
        default:
            {
            }break;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_set_movie_composition_loop( const aeMovieComposition * _composition, ae_bool_t _loop )
{
    aeMovieCompositionAnimation * animation = _composition->animation;

    animation->loop = _loop;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_get_movie_composition_loop( const  aeMovieComposition * _composition )
{
    const aeMovieCompositionAnimation * animation = _composition->animation;

    ae_bool_t loop = animation->loop;

    return loop;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_get_movie_composition_interpolate( const aeMovieComposition * _composition )
{
    ae_bool_t interpolate = _composition->interpolate;

    return interpolate;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_is_play_movie_composition( const aeMovieComposition * _composition )
{
    const aeMovieCompositionAnimation * animation = _composition->animation;

    ae_bool_t play = animation->play;

    return play;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_is_pause_movie_composition( const aeMovieComposition * _composition )
{
    const aeMovieCompositionAnimation * animation = _composition->animation;

    ae_bool_t pause = animation->pause;

    return pause;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_is_interrupt_movie_composition( const aeMovieComposition * _composition )
{
    const aeMovieCompositionAnimation * animation = _composition->animation;

    ae_bool_t interrupt = animation->interrupt;

    return interrupt;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __notify_stop_nodies( const aeMovieComposition * _composition, const aeMovieCompositionData * _compositionData, aeMovieCompositionAnimation * _animation, const aeMovieSubComposition * _subcomposition )
{
    __setup_movie_node_matrix2( _composition, _compositionData, _animation, _subcomposition );

    ae_uint32_t enumerator = 0U;

    aeMovieNode *it_node = _composition->nodes;
    aeMovieNode *it_node_end = _composition->nodes + _composition->node_count;
    for( ; it_node != it_node_end; ++it_node, ++enumerator )
    {
        aeMovieNode * node = it_node;

        if( node->subcomposition != _subcomposition )
        {
            continue;
        }

        const aeMovieLayerData * layer = node->layer;

        if( layer->is_track_matte == AE_TRUE )
        {
            if( node->animate != AE_MOVIE_NODE_ANIMATE_STATIC && node->animate != AE_MOVIE_NODE_ANIMATE_END )
            {
                aeMovieRenderMesh mesh;
                __compute_movie_render_mesh( _composition, node, &mesh, _composition->interpolate, AE_TRUE );

                aeMovieTrackMatteUpdateCallbackData callbackData;
                callbackData.index = enumerator;
                callbackData.element_userdata = node->element_userdata;
                callbackData.layer = layer;
                callbackData.loop = _animation->loop;
                callbackData.state = AE_MOVIE_STATE_UPDATE_STOP;
                callbackData.offset = AE_TIME_OUTSCALE( 0.f );
                callbackData.immutable_matrix = node->immutable_matrix;
                callbackData.matrix = node->matrix;
                callbackData.immutable_color = node->immutable_color;
                callbackData.color = node->color;
                callbackData.opacity = node->opacity;
                callbackData.mesh = &mesh;
                callbackData.track_matte_userdata = node->track_matte_userdata;

                (*_composition->providers.track_matte_update)(&callbackData, _composition->provider_userdata);

                node->animate = AE_MOVIE_NODE_ANIMATE_STATIC;
            }
        }
        else
        {
            if( node->animate != AE_MOVIE_NODE_ANIMATE_STATIC && node->animate != AE_MOVIE_NODE_ANIMATE_END )
            {
                aeMovieNodeUpdateCallbackData callbackData;
                callbackData.index = enumerator;
                callbackData.element_userdata = node->element_userdata;
                callbackData.layer = layer;
                callbackData.loop = _animation->loop;
                callbackData.state = AE_MOVIE_STATE_UPDATE_STOP;
                callbackData.offset = AE_TIME_OUTSCALE( 0.f );
                callbackData.immutable_matrix = node->immutable_matrix;
                callbackData.matrix = node->matrix;
                callbackData.immutable_color = node->immutable_color;
                callbackData.color = node->color;
                callbackData.opacity = node->opacity;
                callbackData.volume = node->volume;

                (*_composition->providers.node_update)(&callbackData, _composition->provider_userdata);

                node->animate = AE_MOVIE_NODE_ANIMATE_STATIC;
            }
        }
    }
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __notify_stop_nodies2( const aeMovieComposition * _composition )
{
    __notify_stop_nodies( _composition, _composition->composition_data, _composition->animation, AE_NULLPTR );

    const aeMovieSubComposition *it_subcomposition = _composition->subcompositions;
    const aeMovieSubComposition *it_subcomposition_end = _composition->subcompositions + _composition->subcomposition_count;
    for( ; it_subcomposition != it_subcomposition_end; ++it_subcomposition )
    {
        const aeMovieSubComposition * subcomposition = it_subcomposition;

        __notify_stop_nodies( _composition, subcomposition->composition_data, subcomposition->animation, subcomposition );
    }
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __notify_pause_nodies( const aeMovieComposition * _composition, const aeMovieCompositionData * _compositionData, aeMovieCompositionAnimation * _animation, const aeMovieSubComposition * _subcomposition )
{
    __setup_movie_node_matrix2( _composition, _compositionData, _animation, _subcomposition );

    ae_uint32_t enumerator = 0U;

    aeMovieNode *it_node = _composition->nodes;
    aeMovieNode *it_node_end = _composition->nodes + _composition->node_count;
    for( ; it_node != it_node_end; ++it_node, ++enumerator )
    {
        aeMovieNode * node = it_node;

        if( node->subcomposition != _subcomposition )
        {
            continue;
        }

        const aeMovieLayerData * layer = node->layer;

        if( layer->is_track_matte == AE_TRUE )
        {
            if( node->animate != AE_MOVIE_NODE_ANIMATE_STATIC && node->animate != AE_MOVIE_NODE_ANIMATE_END )
            {
                aeMovieRenderMesh mesh;
                __compute_movie_render_mesh( _composition, node, &mesh, _composition->interpolate, AE_TRUE );

                aeMovieTrackMatteUpdateCallbackData callbackData;
                callbackData.index = enumerator;
                callbackData.element_userdata = node->element_userdata;
                callbackData.layer = layer;
                callbackData.loop = _animation->loop;
                callbackData.state = AE_MOVIE_STATE_UPDATE_PAUSE;
                callbackData.offset = AE_TIME_OUTSCALE( node->current_time );
                callbackData.immutable_matrix = node->immutable_matrix;
                callbackData.matrix = node->matrix;
                callbackData.immutable_color = node->immutable_color;
                callbackData.color = node->color;
                callbackData.opacity = node->opacity;
                callbackData.mesh = &mesh;
                callbackData.track_matte_userdata = node->track_matte_userdata;

                (*_composition->providers.track_matte_update)(&callbackData, _composition->provider_userdata);
            }
        }
        else
        {
            if( node->animate != AE_MOVIE_NODE_ANIMATE_STATIC && node->animate != AE_MOVIE_NODE_ANIMATE_END )
            {
                aeMovieNodeUpdateCallbackData callbackData;
                callbackData.index = enumerator;
                callbackData.element_userdata = node->element_userdata;
                callbackData.layer = layer;
                callbackData.loop = _animation->loop;
                callbackData.state = AE_MOVIE_STATE_UPDATE_PAUSE;
                callbackData.offset = AE_TIME_OUTSCALE( node->current_time );
                callbackData.immutable_matrix = node->immutable_matrix;
                callbackData.matrix = node->matrix;
                callbackData.immutable_color = node->immutable_color;
                callbackData.color = node->color;
                callbackData.opacity = node->opacity;
                callbackData.volume = node->volume;

                (*_composition->providers.node_update)(&callbackData, _composition->provider_userdata);
            }
        }
    }
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __notify_pause_nodies2( const aeMovieComposition * _composition )
{
    __notify_pause_nodies( _composition, _composition->composition_data, _composition->animation, AE_NULLPTR );

    const aeMovieSubComposition *it_subcomposition = _composition->subcompositions;
    const aeMovieSubComposition *it_subcomposition_end = _composition->subcompositions + _composition->subcomposition_count;
    for( ; it_subcomposition != it_subcomposition_end; ++it_subcomposition )
    {
        const aeMovieSubComposition * subcomposition = it_subcomposition;

        __notify_pause_nodies( _composition, subcomposition->composition_data, subcomposition->animation, subcomposition );
    }
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_pause_movie_composition( const aeMovieComposition * _composition )
{
    aeMovieCompositionAnimation * animation = _composition->animation;

    if( animation->play == AE_FALSE )
    {
        return;
    }

    if( animation->pause == AE_TRUE )
    {
        return;
    }

    animation->pause = AE_TRUE;

    __notify_pause_nodies2( _composition );
	
    aeMovieCompositionStateCallbackData callbackData;
    callbackData.state = AE_MOVIE_COMPOSITION_PAUSE;

    (*_composition->providers.composition_state)(&callbackData, _composition->provider_userdata);
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __notify_resume_nodies( const aeMovieComposition * _composition, const aeMovieCompositionData * _compositionData, aeMovieCompositionAnimation * _animation, const aeMovieSubComposition * _subcomposition )
{
    __setup_movie_node_matrix2( _composition, _compositionData, _animation, _subcomposition );

    ae_uint32_t enumerator = 0U;

    aeMovieNode *it_node = _composition->nodes;
    aeMovieNode *it_node_end = _composition->nodes + _composition->node_count;
    for( ; it_node != it_node_end; ++it_node, ++enumerator )
    {
        aeMovieNode * node = it_node;

        if( node->subcomposition != _subcomposition )
        {
            continue;
        }

        const aeMovieLayerData * layer = node->layer;

        if( layer->is_track_matte == AE_TRUE )
        {
            if( node->animate != AE_MOVIE_NODE_ANIMATE_STATIC && node->animate != AE_MOVIE_NODE_ANIMATE_END )
            {
                aeMovieRenderMesh mesh;
                __compute_movie_render_mesh( _composition, node, &mesh, _composition->interpolate, AE_TRUE );

                aeMovieTrackMatteUpdateCallbackData callbackData;
                callbackData.index = enumerator;
                callbackData.element_userdata = node->element_userdata;
                callbackData.layer = layer;
                callbackData.loop = _animation->loop;
                callbackData.state = AE_MOVIE_STATE_UPDATE_RESUME;
                callbackData.offset = AE_TIME_OUTSCALE( node->current_time );
                callbackData.immutable_matrix = node->immutable_matrix;
                callbackData.matrix = node->matrix;
                callbackData.immutable_color = node->immutable_color;
                callbackData.color = node->color;
                callbackData.opacity = node->opacity;
                callbackData.mesh = &mesh;
                callbackData.track_matte_userdata = node->track_matte_userdata;

                (*_composition->providers.track_matte_update)(&callbackData, _composition->provider_userdata);
            }
        }
        else
        {
            if( node->animate != AE_MOVIE_NODE_ANIMATE_STATIC && node->animate != AE_MOVIE_NODE_ANIMATE_END )
            {
                aeMovieNodeUpdateCallbackData callbackData;
                callbackData.index = enumerator;
                callbackData.element_userdata = node->element_userdata;
                callbackData.layer = layer;
                callbackData.loop = _animation->loop;
                callbackData.state = AE_MOVIE_STATE_UPDATE_RESUME;
                callbackData.offset = AE_TIME_OUTSCALE( node->current_time );
                callbackData.immutable_matrix = node->immutable_matrix;
                callbackData.matrix = node->matrix;
                callbackData.immutable_color = node->immutable_color;
                callbackData.color = node->color;
                callbackData.opacity = node->opacity;
                callbackData.volume = node->volume;

                (*_composition->providers.node_update)(&callbackData, _composition->provider_userdata);
            }
        }
    }
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __notify_resume_nodies2( const aeMovieComposition * _composition )
{
    __notify_resume_nodies( _composition, _composition->composition_data, _composition->animation, AE_NULLPTR );

    const aeMovieSubComposition *it_subcomposition = _composition->subcompositions;
    const aeMovieSubComposition *it_subcomposition_end = _composition->subcompositions + _composition->subcomposition_count;
    for( ; it_subcomposition != it_subcomposition_end; ++it_subcomposition )
    {
        const aeMovieSubComposition * subcomposition = it_subcomposition;

        __notify_resume_nodies( _composition, subcomposition->composition_data, subcomposition->animation, subcomposition );
    }
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_resume_movie_composition( const aeMovieComposition * _composition )
{
    aeMovieCompositionAnimation * animation = _composition->animation;

    if( animation->play == AE_FALSE )
    {
        return;
    }

    if( animation->pause == AE_FALSE )
    {
        return;
    }

    animation->pause = AE_FALSE;

    __notify_resume_nodies2( _composition );

    aeMovieCompositionStateCallbackData callbackData;
    callbackData.state = AE_MOVIE_COMPOSITION_RESUME;

    (*_composition->providers.composition_state)(&callbackData, _composition->provider_userdata);
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_float_t __get_animation_loop_work_begin( const aeMovieCompositionAnimation * _animation )
{
    ae_bool_t loop = _animation->loop;

    if( loop == AE_TRUE )
    {
        ae_float_t work_begin = ae_max_f_f( _animation->loop_segment_begin, _animation->work_area_begin );

        return work_begin;
    }
    else
    {
        ae_float_t work_begin = _animation->work_area_begin;

        return work_begin;
    }
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_float_t __get_animation_loop_work_end( const aeMovieCompositionAnimation * _animation )
{
    ae_bool_t loop = _animation->loop;

    if( loop == AE_TRUE )
    {
        ae_float_t work_end = ae_min_f_f( _animation->loop_segment_end, _animation->work_area_end );

        return work_end;
    }
    else
    {
        ae_float_t work_end = _animation->work_area_end;

        return work_end;
    }
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __update_movie_composition_node_normal_state( const aeMovieComposition * _composition, const aeMovieCompositionAnimation * _animation, aeMovieNode * _node, ae_uint32_t _index, ae_bool_t _loop, ae_bool_t _begin, ae_float_t _time )
{
    if( _node->element_userdata == AE_NULLPTR )
    {
        return;
    }

    const aeMovieLayerData * layer = _node->layer;

    aeMovieNodeUpdateCallbackData callbackData;
    callbackData.index = _index;
    callbackData.element_userdata = _node->element_userdata;
    callbackData.layer = layer;
    callbackData.interrupt = _animation->interrupt;
    callbackData.loop = _loop;
    callbackData.immutable_matrix = _node->immutable_matrix;
    callbackData.matrix = _node->matrix;
    callbackData.immutable_color = _node->immutable_color;
    callbackData.color = _node->color;
    callbackData.opacity = _node->opacity;
    callbackData.volume = _node->volume;


    if( _begin == AE_TRUE )
    {
        if( _node->animate == AE_MOVIE_NODE_ANIMATE_STATIC || _node->animate == AE_MOVIE_NODE_ANIMATE_END )
        {
            _node->animate = AE_MOVIE_NODE_ANIMATE_BEGIN;

            callbackData.state = AE_MOVIE_STATE_UPDATE_BEGIN;

            ae_float_t offset = layer->start_time + _time - _node->in_time;

            if( offset < 0.f )
            {
                offset = 0.f;
            }
            else if( offset > layer->composition_data->duration )
            {
                offset = layer->composition_data->duration;
            }

            callbackData.offset = AE_TIME_OUTSCALE( offset );

            (*_composition->providers.node_update)(&callbackData, _composition->provider_userdata);
        }
        else
        {
            _node->animate = AE_MOVIE_NODE_ANIMATE_PROCESS;

            callbackData.state = AE_MOVIE_STATE_UPDATE_PROCESS;
            callbackData.offset = AE_TIME_OUTSCALE( 0.f );

            (*_composition->providers.node_update)(&callbackData, _composition->provider_userdata);
        }
    }
    else
    {
        if( _node->animate == AE_MOVIE_NODE_ANIMATE_PROCESS || _node->animate == AE_MOVIE_NODE_ANIMATE_BEGIN )
        {
            _node->animate = AE_MOVIE_NODE_ANIMATE_END;

            callbackData.state = AE_MOVIE_STATE_UPDATE_END;

            callbackData.offset = AE_TIME_OUTSCALE( 0.f );

            (*_composition->providers.node_update)(&callbackData, _composition->provider_userdata);
        }
        else
        {
            _node->animate = AE_MOVIE_NODE_ANIMATE_STATIC;

            callbackData.state = AE_MOVIE_STATE_UPDATE_SKIP;

            (*_composition->providers.node_update)(&callbackData, _composition->provider_userdata);
        }
    }
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __update_movie_composition_node_track_matte_state( const aeMovieComposition * _composition, const aeMovieCompositionAnimation * _animation, aeMovieNode * _node, ae_uint32_t _index, ae_bool_t _loop, ae_bool_t _begin, ae_float_t _time, ae_bool_t _interpolate )
{
    const aeMovieLayerData * layer = _node->layer;

    aeMovieLayerTypeEnum layer_type = layer->type;

    switch( layer_type )
    {
    case AE_MOVIE_LAYER_TYPE_MOVIE:
        {
            return;
        }break;
    default:
        {
        }break;
    }

    aeMovieRenderMesh mesh;
    __compute_movie_render_mesh( _composition, _node, &mesh, _interpolate, AE_TRUE );

    aeMovieTrackMatteUpdateCallbackData callbackData;
    callbackData.index = _index;
    callbackData.element_userdata = _node->element_userdata;
    callbackData.layer = layer;
    callbackData.interrupt = _animation->interrupt;
    callbackData.loop = _loop;

    ae_float_t offset = layer->start_time + _time - _node->in_time;

    if( offset < 0.f )
    {
        offset = 0.f;
    }
    else if( offset > layer->composition_data->duration )
    {
        offset = layer->composition_data->duration;
    }

    callbackData.offset = AE_TIME_OUTSCALE( offset );
    callbackData.immutable_matrix = _node->immutable_matrix;
    callbackData.matrix = _node->matrix;
    callbackData.immutable_color = _node->immutable_color;
    callbackData.color = _node->color;
    callbackData.opacity = 0.f;
    callbackData.mesh = &mesh;
    callbackData.track_matte_userdata = _node->track_matte_userdata;

    if( _begin == AE_TRUE )
    {
        if( _node->animate == AE_MOVIE_NODE_ANIMATE_STATIC || _node->animate == AE_MOVIE_NODE_ANIMATE_END )
        {
            _node->animate = AE_MOVIE_NODE_ANIMATE_BEGIN;

            callbackData.state = AE_MOVIE_STATE_UPDATE_BEGIN;

            (*_composition->providers.track_matte_update)(&callbackData, _composition->provider_userdata);
        }
        else
        {
            _node->animate = AE_MOVIE_NODE_ANIMATE_PROCESS;

            callbackData.state = AE_MOVIE_STATE_UPDATE_PROCESS;
			
            (*_composition->providers.track_matte_update)(&callbackData, _composition->provider_userdata);
        }
    }
    else
    {
        if( _node->animate == AE_MOVIE_NODE_ANIMATE_PROCESS || _node->animate == AE_MOVIE_NODE_ANIMATE_BEGIN )
        {
            _node->animate = AE_MOVIE_NODE_ANIMATE_END;

            callbackData.state = AE_MOVIE_STATE_UPDATE_END;
            
            (*_composition->providers.track_matte_update)(&callbackData, _composition->provider_userdata);
        }
        else
        {
            _node->animate = AE_MOVIE_NODE_ANIMATE_STATIC;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __update_movie_composition_node_state( const aeMovieComposition * _composition, const aeMovieCompositionAnimation * _animation, aeMovieNode * _node, ae_uint32_t _index, ae_bool_t _loop, ae_bool_t _begin, ae_float_t _time, ae_bool_t _interpolate )
{
    if( _node->layer->is_track_matte == AE_TRUE )
    {
        __update_movie_composition_node_track_matte_state( _composition, _animation, _node, _index, _loop, _begin, _time, _interpolate );
    }
    else
    {
        __update_movie_composition_node_normal_state( _composition, _animation, _node, _index, _loop, _begin, _time );
    }
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __update_node( const aeMovieComposition * _composition, const aeMovieCompositionData * _compositionData, const aeMovieCompositionAnimation * _animation, const aeMovieSubComposition * _subcomposition, aeMovieNode * _node, ae_uint32_t _index, ae_uint32_t _revision, ae_float_t _time, ae_uint32_t _frameId, ae_float_t _t, ae_bool_t _loop, ae_bool_t _interpolate, ae_bool_t _begin )
{
    const aeMovieLayerData * node_layer = _node->layer;

#ifdef AE_MOVIE_DEBUG	
    if( __test_error_composition_layer_frame( _composition->movie_data->instance
        , _compositionData
        , node_layer
        , _frameId
        , "__update_node frame id out count"
    ) == AE_FALSE )
    {
        return;
    }
#endif

#ifdef AE_MOVIE_SAFE
    if( _frameId >= node_layer->frame_count )
    {
        return;
    }
#endif    

    ae_bool_t node_interpolate = (_frameId + 1 == node_layer->frame_count) ? AE_FALSE : _interpolate;

    __update_movie_composition_node_matrix( _node, _revision, _composition, _compositionData, _animation, _subcomposition, _frameId, node_interpolate, _t );

    if( _node->shader_userdata != AE_NULLPTR )
    {
        __update_movie_composition_node_shader( _node, _revision, _composition, _compositionData, _frameId, node_interpolate, _t );
    }

    __update_movie_composition_node_state( _composition, _animation, _node, _index, _loop, _begin, _time, node_interpolate );

    _node->update_revision = _revision;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __update_movie_scene_effect( const aeMovieComposition * _composition, const aeMovieCompositionAnimation * _animation )
{
    aeMovieNode * scene_effect_node = _composition->scene_effect_node;

    if( scene_effect_node == AE_NULLPTR )
    {
        return;
    }

    const aeMovieLayerData * layer = scene_effect_node->layer;

    const aeMovieLayerTransformation2D * transformation2d = (const aeMovieLayerTransformation2D *)layer->transformation;

    ae_float_t frameDurationInv = layer->composition_data->frameDurationInv;

    ae_float_t current_time = _animation->time - scene_effect_node->in_time + scene_effect_node->start_time;
    ae_float_t frame_time = current_time * scene_effect_node->stretchInv * frameDurationInv;

    ae_uint32_t frameId = (ae_uint32_t)frame_time;

    aeMovieCompositionSceneEffectUpdateCallbackData callbackData;
    callbackData.element_userdata = scene_effect_node->element_userdata;

    ae_bool_t composition_interpolate = _composition->interpolate;

    if( composition_interpolate == AE_TRUE )
    {
        ae_float_t t = ae_fractional_f( frame_time );

        ae_movie_make_layer_transformation2d_interpolate( callbackData.anchor_point, callbackData.position, callbackData.scale, callbackData.quaternion, callbackData.skew, transformation2d, frameId, t );
        ae_movie_make_layer_transformation_color_interpolate( &callbackData.color, &callbackData.opacity, layer->transformation, frameId, t );
    }
    else
    {
        ae_movie_make_layer_transformation2d_fixed( callbackData.anchor_point, callbackData.position, callbackData.scale, callbackData.quaternion, callbackData.skew, transformation2d, frameId );
        ae_movie_make_layer_transformation_color_fixed( &callbackData.color, &callbackData.opacity, layer->transformation, frameId );
    }

    callbackData.scene_effect_userdata = _composition->scene_effect_userdata;

    (*_composition->providers.scene_effect_update)(&callbackData, _composition->provider_userdata);
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __refresh_movie_composition_matrix( const aeMovieComposition * _composition, const aeMovieCompositionData * _compositionData, const aeMovieCompositionAnimation * _animation, const aeMovieSubComposition * _subcomposition )
{
    ae_uint32_t update_revision = __get_composition_update_revision( _composition );

    ae_bool_t composition_interpolate = _composition->interpolate;

    aeMovieNode *it_node = _composition->nodes;
    aeMovieNode *it_node_end = _composition->nodes + _composition->node_count;
    for( ; it_node != it_node_end; ++it_node )
    {
        aeMovieNode * node = it_node;

        if( node->ignore == AE_TRUE )
        {
            continue;
        }

        if( node->active == AE_FALSE )
        {
            continue;
        }

        if( node->subcomposition != _subcomposition )
        {
            continue;
        }

        const aeMovieLayerData * node_layer = node->layer;

        if( node_layer->type == AE_MOVIE_LAYER_TYPE_EVENT )
        {
            continue;
        }

        ae_float_t frameDurationInv = node_layer->composition_data->frameDurationInv;

        ae_float_t frame_time = node->current_time * node->stretchInv * frameDurationInv;

        ae_uint32_t frameId = (ae_uint32_t)frame_time;

        ae_float_t t = 0.f;
        if( composition_interpolate == AE_TRUE )
        {
            t = ae_fractional_f( frame_time );
        }

        __update_movie_composition_node_matrix( node, update_revision, _composition, _compositionData, _animation, _subcomposition, frameId, composition_interpolate, t );
    }
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __update_movie_composition_node( const aeMovieComposition * _composition, const aeMovieCompositionData * _compositionData, const aeMovieCompositionAnimation * _animation, const aeMovieSubComposition * _subcomposition, ae_uint32_t _revision, ae_float_t _beginTime, ae_bool_t _end )
{
    ae_bool_t composition_interpolate = _composition->interpolate;

    ae_float_t animation_time = _animation->time;
    ae_bool_t animation_interrupt = _animation->interrupt;
    ae_bool_t animation_loop = _animation->loop;

    ae_float_t loopBegin = __get_animation_loop_work_begin( _animation );
    ae_float_t loopEnd = __get_animation_loop_work_end( _animation );

    ae_uint32_t enumerator = 0U;

    aeMovieNode *it_node = _composition->nodes;
    aeMovieNode *it_node_end = _composition->nodes + _composition->node_count;
    for( ; it_node != it_node_end; ++it_node, ++enumerator )
    {
        aeMovieNode * node = it_node;

        if( node->ignore == AE_TRUE )
        {
            continue;
        }

        if( node->subcomposition != _subcomposition )
        {
            continue;
        }

        const aeMovieLayerData * node_layer = node->layer;

        ae_float_t frameDurationInv = node_layer->composition_data->frameDurationInv;

        ae_bool_t test_time = (_beginTime >= loopBegin
            && animation_time < loopEnd
            && animation_interrupt == AE_FALSE
            && animation_loop == AE_TRUE
            && node_layer->type != AE_MOVIE_LAYER_TYPE_EVENT);

        ae_float_t in_time = (test_time == AE_TRUE && node->in_time <= loopBegin) ? loopBegin : node->in_time;
        ae_float_t out_time = (test_time == AE_TRUE && node->out_time >= loopEnd) ? loopEnd : node->out_time;

        ae_float_t beginFrameF = _beginTime;
        ae_float_t endFrameF = animation_time;
        ae_float_t indexInF = in_time;
        ae_float_t indexOutF = out_time;

        ae_uint32_t beginFrame = (ae_uint32_t)(beginFrameF * frameDurationInv + AE_MOVIE_FRAME_EPSILON);
        ae_uint32_t endFrame = (ae_uint32_t)(endFrameF * frameDurationInv + AE_MOVIE_FRAME_EPSILON);
        ae_uint32_t indexIn = (ae_uint32_t)(indexInF * frameDurationInv + AE_MOVIE_FRAME_EPSILON);
        ae_uint32_t indexOut = (ae_uint32_t)(indexOutF * frameDurationInv + AE_MOVIE_FRAME_EPSILON);

        ae_float_t current_time = (endFrameF >= indexOutF) ? out_time - node->in_time + node->start_time : animation_time - node->in_time + node->start_time;
        ae_float_t stretch_time = current_time * node->stretchInv;
        ae_float_t frame_time = stretch_time * frameDurationInv;

        ae_uint32_t frameId2 = (ae_uint32_t)frame_time;
        ae_uint32_t frameId = (ae_uint32_t)(frame_time + AE_MOVIE_FRAME_EPSILON);

        ae_float_t frame_time_for_fractional = frame_time;
        if( frameId != frameId2 )
        {
            frame_time_for_fractional = 0.f;
        }

        if( node_layer->type == AE_MOVIE_LAYER_TYPE_EVENT )
        {
            node->current_time = stretch_time;
            node->current_frame = frameId;
            node->current_frame_t = 0.f;

            if( beginFrameF < indexInF && endFrameF > indexInF )
            {
                __update_movie_composition_node_matrix( node, _revision, _composition, _compositionData, _animation, _subcomposition, frameId, AE_FALSE, 0.f );

                aeMovieCompositionEventCallbackData callbackData;
                callbackData.index = enumerator;
                callbackData.element_userdata = node->element_userdata;
                callbackData.name = node_layer->name;
                callbackData.immutable_matrix = node->immutable_matrix;
                callbackData.matrix = node->matrix;
                callbackData.immutable_color = node->immutable_color;
                callbackData.color = node->color;
                callbackData.opacity = node->opacity;
                callbackData.begin = AE_TRUE;

                (*_composition->providers.composition_event)(&callbackData, _composition->provider_userdata);
            }

            if( beginFrameF < indexOutF && endFrameF > indexOutF )
            {
                __update_movie_composition_node_matrix( node, _revision, _composition, _compositionData, _animation, _subcomposition, frameId, AE_FALSE, 0.f );

                aeMovieCompositionEventCallbackData callbackData;
                callbackData.index = enumerator;
                callbackData.element_userdata = node->element_userdata;
                callbackData.name = node_layer->name;
                callbackData.immutable_matrix = node->immutable_matrix;
                callbackData.matrix = node->matrix;
                callbackData.immutable_color = node->immutable_color;
                callbackData.color = node->color;
                callbackData.opacity = node->opacity;
                callbackData.begin = AE_FALSE;

                (*_composition->providers.composition_event)(&callbackData, _composition->provider_userdata);
            }

            node->update_revision = _revision;

            continue;
        }

        if( indexInF > endFrameF || indexOutF < beginFrameF )
        {
            if( node->incessantly == AE_TRUE )
            {
                node->current_time = 0.f;
                node->current_frame = 0U;
                node->current_frame_t = 0.f;

                __update_movie_composition_node_matrix( node, _revision, _composition, _compositionData, _animation, _subcomposition, 0, AE_FALSE, 0.f );

                __update_movie_composition_node_state( _composition, _animation, node, enumerator, AE_TRUE, AE_TRUE, 0.f, composition_interpolate );

                node->update_revision = _revision;
                node->active = AE_TRUE;
            }
            else
            {
                node->active = AE_FALSE;
            }

            continue;
        }
        else if( indexInF > beginFrameF && indexOutF < endFrameF )
        {
            if( node->incessantly == AE_TRUE )
            {
                node->current_time = stretch_time;
                node->current_frame = frameId;
                node->current_frame_t = 0.f;

                __update_movie_composition_node_matrix( node, _revision, _composition, _compositionData, _animation, _subcomposition, frameId, AE_FALSE, 0.f );

                __update_movie_composition_node_state( _composition, _animation, node, enumerator, AE_TRUE, AE_TRUE, stretch_time, composition_interpolate );

                node->update_revision = _revision;
                node->active = AE_TRUE;
            }
            else
            {
                node->active = AE_FALSE;
            }

            continue;
        }

        node->current_time = stretch_time;
        node->current_frame = frameId;

        ae_bool_t node_loop = ((animation_loop == AE_TRUE && animation_interrupt == AE_FALSE && loopBegin >= node->in_time && node->out_time >= loopEnd) || node_layer->incessantly == AE_TRUE);

        if( beginFrame < indexIn && endFrame >= indexIn && endFrame < indexOut )
        {
            node->active = AE_TRUE;

            ae_bool_t node_interpolate = composition_interpolate ? ((endFrame + 1) < indexOut) : AE_FALSE;

            if( node_interpolate == AE_TRUE )
            {
                node->current_frame_t = ae_fractional_f( frame_time_for_fractional );
            }

            __update_node( _composition, _compositionData, _animation, _subcomposition, node, enumerator, _revision, animation_time, node->current_frame, node->current_frame_t, node_loop, node_interpolate, AE_TRUE );
        }
        else if( endFrame >= indexOut && beginFrame >= indexIn && beginFrame < indexOut )
        {
            ae_bool_t node_active = (node_loop == AE_TRUE || node->incessantly == AE_TRUE) ? AE_TRUE : AE_FALSE;

            node->active = node_active;

            ae_uint32_t frameEnd = indexOut - indexIn;

            node->current_frame = frameEnd;
            node->current_frame_t = 0.f;

            ae_bool_t begin = (node->incessantly == AE_TRUE) ? AE_TRUE : AE_FALSE;

            __update_node( _composition, _compositionData, _animation, _subcomposition, node, enumerator, _revision, animation_time, node->current_frame, node->current_frame_t, node_loop, AE_FALSE, begin );
        }
        else if( beginFrame >= indexIn && endFrame >= indexIn && endFrame < indexOut )
        {
            node->active = AE_TRUE;

            ae_bool_t node_interpolate = (composition_interpolate == AE_TRUE) ? (endFrame + 1) < indexOut : AE_FALSE;

            if( node_interpolate == AE_TRUE )
            {
                node->current_frame_t = ae_fractional_f( frame_time_for_fractional );
            }

            ae_bool_t begin = (_end == AE_TRUE) ? AE_FALSE : AE_TRUE;

            __update_node( _composition, _compositionData, _animation, _subcomposition, node, enumerator, _revision, animation_time, node->current_frame, node->current_frame_t, node_loop, node_interpolate, begin );
        }
    }
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __update_movie_camera( const aeMovieComposition * _composition, const aeMovieCompositionAnimation * _animation )
{
    if( _composition->camera_userdata == AE_NULLPTR )
    {
        return;
    }

    const aeMovieCompositionData * composition_data = _composition->composition_data;
    const aeMovieCompositionCamera * camera = composition_data->camera;

    ae_bool_t composition_interpolate = _composition->interpolate;

    aeMovieCameraUpdateCallbackData callbackData;
    callbackData.camera_userdata = _composition->camera_userdata;
    callbackData.name = camera->name;

    ae_float_t frameDurationInv = composition_data->frameDurationInv;

    ae_float_t frame_time = _animation->time * frameDurationInv;

    ae_uint32_t frame_id = (ae_uint32_t)frame_time;

    if( composition_interpolate == AE_FALSE )
    {
        ae_movie_make_camera_transformation( callbackData.target, callbackData.position, callbackData.quaternion, composition_data->camera, frame_id, AE_FALSE, 0.f );
    }
    else
    {
        ae_float_t t = ae_fractional_f( frame_time );

        ae_movie_make_camera_transformation( callbackData.target, callbackData.position, callbackData.quaternion, composition_data->camera, frame_id, AE_TRUE, t );
    }

    (*_composition->providers.camera_update)(&callbackData, _composition->provider_userdata);
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __skip_movie_composition_node( const aeMovieComposition * _composition, const aeMovieCompositionData * _compositionData, aeMovieCompositionAnimation * _animation, const aeMovieSubComposition * _subcomposition, ae_uint32_t _revision, ae_float_t _beginTime, ae_float_t _endTime )
{
    ae_uint32_t enumerator = 0U;

    aeMovieNode	*it_node = _composition->nodes;
    aeMovieNode *it_node_end = _composition->nodes + _composition->node_count;
    for( ; it_node != it_node_end; ++it_node, ++enumerator )
    {
        aeMovieNode * node = it_node;

        if( node->ignore == AE_TRUE )
        {
            continue;
        }

        if( node->subcomposition != _subcomposition )
        {
            continue;
        }

        const aeMovieLayerData * layer = node->layer;

        ae_float_t frameDurationInv = layer->composition_data->frameDurationInv;

        ae_float_t in_time = node->in_time;
        ae_float_t out_time = node->out_time;

        ae_float_t beginFrameF = _beginTime;
        ae_float_t endFrameF = _endTime;
        ae_float_t indexInF = in_time;
        ae_float_t indexOutF = out_time;

        ae_uint32_t beginFrame = (ae_uint32_t)(beginFrameF * frameDurationInv + AE_MOVIE_FRAME_EPSILON);
        ae_uint32_t endFrame = (ae_uint32_t)(endFrameF * frameDurationInv + AE_MOVIE_FRAME_EPSILON);
        ae_uint32_t indexIn = (ae_uint32_t)(indexInF * frameDurationInv + AE_MOVIE_FRAME_EPSILON);
        ae_uint32_t indexOut = (ae_uint32_t)(indexOutF * frameDurationInv + AE_MOVIE_FRAME_EPSILON);

        if( indexInF > endFrameF || indexOutF < beginFrameF )
        {
            node->active = AE_FALSE;

            continue;
        }

        if( indexInF > beginFrameF && indexOutF < endFrameF )
        {
            node->active = AE_FALSE;

            continue;
        }

        ae_float_t current_time = (endFrame >= indexOut) ? out_time - node->in_time + node->start_time : _animation->time - node->in_time + node->start_time;
        ae_float_t frame_time = current_time * node->stretchInv * frameDurationInv;

        ae_uint32_t frameId = (ae_uint32_t)frame_time;

        node->current_time = current_time;
        node->current_frame = frameId;
        node->current_frame_t = 0.f;

        if( layer->type == AE_MOVIE_LAYER_TYPE_EVENT )
        {
        }
        else
        {
            if( beginFrame < indexIn && endFrame >= indexIn && endFrame < indexOut )
            {
            }
            else if( endFrame >= indexOut && beginFrame >= indexIn && beginFrame < indexOut )
            {
                node->active = AE_FALSE;

                ae_bool_t node_interpolate = (endFrame + 1) < indexOut;

                if( node_interpolate == AE_TRUE )
                {
                    node->current_frame_t = ae_fractional_f( frame_time );
                }

                __update_node( _composition, _compositionData, _animation, _subcomposition, node, enumerator, _revision, _endTime, node->current_frame, node->current_frame_t, AE_FALSE, node_interpolate, AE_FALSE );
            }
            else if( beginFrame >= indexIn && endFrame >= indexIn && endFrame < indexOut )
            {
            }
        }

        node->animate = AE_MOVIE_NODE_ANIMATE_STATIC;
    }
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_bool_t __update_movie_subcomposition( const aeMovieComposition * _composition, const aeMovieCompositionData * _compositionData, ae_float_t _timing, aeMovieCompositionAnimation * _animation, const aeMovieSubComposition * _subcomposition )
{
    ae_uint32_t update_revision = __get_composition_update_revision( _composition );

    ae_float_t prev_time = _animation->time;

    ae_float_t begin_time = prev_time;

    if( _animation->play == AE_TRUE )
    {
        ae_float_t current_time = _animation->time;
        ae_float_t duration = _animation->work_area_end - _animation->work_area_begin;

        ae_float_t frameDuration = _composition->composition_data->frameDuration;

        if( _animation->loop == AE_FALSE || _animation->interrupt == AE_TRUE )
        {
            ae_float_t last_time = duration - frameDuration;

            if( current_time + _timing >= last_time )
            {
                _animation->time = last_time;

                __update_movie_composition_node( _composition, _compositionData, _animation, _subcomposition, update_revision, begin_time, AE_TRUE );

                __inc_composition_update_revision( _composition );

                if( _animation->interrupt == AE_TRUE )
                {
                    aeMovieCompositionExtraInterruptCallbackData callbackData;
                    callbackData.dummy = 0;

                    ae_bool_t extra_interrupt = (*_composition->providers.composition_extra_interrupt)(&callbackData, _composition->provider_userdata);

                    if( extra_interrupt == AE_FALSE )
                    {
                        return AE_FALSE;
                    }
                }

                _animation->play = AE_FALSE;
                _animation->pause = AE_FALSE;
                _animation->interrupt = AE_FALSE;

                return AE_TRUE;
            }
            else
            {
                _animation->time += _timing;
            }
        }
        else
        {
            ae_float_t loopBegin = ae_max_f_f( _animation->loop_segment_begin, _animation->work_area_begin );
            ae_float_t loopEnd = ae_min_f_f( _animation->loop_segment_end, _animation->work_area_end );

            ae_float_t last_time = loopEnd - frameDuration;

            if( current_time + _timing >= last_time )
            {
                ae_float_t new_composition_time = current_time + _timing - last_time + loopBegin;

                ae_uint32_t loop_count = 1;
                while( new_composition_time >= last_time )
                {
                    new_composition_time -= last_time;
                    new_composition_time += loopBegin;

                    ++loop_count;
                }

                _animation->time = last_time;

                __update_movie_composition_node( _composition, _compositionData, _animation, _subcomposition, update_revision, begin_time, AE_TRUE );

                update_revision = __inc_composition_update_revision( _composition );

                ae_float_t new_begin_time = loopBegin;

                _animation->time = new_composition_time;

                __update_movie_composition_node( _composition, _compositionData, _animation, _subcomposition, update_revision, new_begin_time, AE_FALSE );

                __inc_composition_update_revision( _composition );

                uint32_t loop_iterator = 0U;
                for( ; loop_iterator != loop_count; ++loop_iterator )
                {
                    if( _subcomposition == AE_NULLPTR )
                    {
                        aeMovieCompositionStateCallbackData callbackData;
                        callbackData.state = AE_MOVIE_COMPOSITION_LOOP_END;

                        (*_composition->providers.composition_state)(&callbackData, _composition->provider_userdata);
                    }
                    else
                    {
                        aeMovieSubCompositionStateCallbackData callbackData;
                        callbackData.state = AE_MOVIE_COMPOSITION_LOOP_END;
                        callbackData.subcomposition_userdata = _subcomposition->subcomposition_userdata;

                        (*_composition->providers.subcomposition_state)(&callbackData, _composition->provider_userdata);                
                    }
                }

                return AE_FALSE;
            }
            else
            {
                _animation->time += _timing;
            }
        }
    }

    __update_movie_composition_node( _composition, _compositionData, _animation, _subcomposition, update_revision, begin_time, AE_FALSE );

    return AE_FALSE;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_update_movie_composition( const aeMovieComposition * _composition, ae_time_t _timing )
{
    ae_time_t timescale_timing = AE_TIME_INSCALE( _timing );

    __inc_composition_update_revision( _composition );

    aeMovieCompositionAnimation * animation = _composition->animation;
    const aeMovieCompositionData * composition_data = _composition->composition_data;

    ae_bool_t animation_play = animation->play;

    ae_bool_t composition_end = AE_FALSE;
    if( animation_play == AE_TRUE && animation->pause == AE_FALSE )
    {
        composition_end = __update_movie_subcomposition( _composition, composition_data, timescale_timing, animation, AE_NULLPTR );
        __update_movie_scene_effect( _composition, animation );
        __update_movie_camera( _composition, animation );
    }

    const aeMovieSubComposition *it_subcomposition = _composition->subcompositions;
    const aeMovieSubComposition *it_subcomposition_end = _composition->subcompositions + _composition->subcomposition_count;
    for( ; it_subcomposition != it_subcomposition_end; ++it_subcomposition )
    {
        const aeMovieSubComposition * subcomposition = it_subcomposition;

        aeMovieCompositionAnimation * subcomposition_animation = subcomposition->animation;

        ae_float_t subcomposition_timing = timescale_timing;

        if( subcomposition_animation->play == AE_FALSE || subcomposition_animation->pause == AE_TRUE )
        {
            if( animation_play == AE_FALSE )
            {
                continue;
            }

            subcomposition_timing = 0.f;
        }

        ae_bool_t subcomposition_end = __update_movie_subcomposition( _composition, subcomposition->composition_data, subcomposition_timing, subcomposition_animation, subcomposition );

        if( subcomposition_end == AE_TRUE )
        {
            aeMovieSubCompositionStateCallbackData callbackData;
            callbackData.state = AE_MOVIE_COMPOSITION_END;
            callbackData.subcomposition_userdata = subcomposition->subcomposition_userdata;

            (*_composition->providers.subcomposition_state)(&callbackData, _composition->provider_userdata);
        }
    }

    if( composition_end == AE_TRUE )
    {
        aeMovieCompositionStateCallbackData callbackData;
        callbackData.state = AE_MOVIE_COMPOSITION_END;

        (*_composition->providers.composition_state)(&callbackData, _composition->provider_userdata);
    }

    return composition_end;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __set_movie_composition_time( const aeMovieComposition * _composition, const aeMovieCompositionData * _compositionData, aeMovieCompositionAnimation * _animation, ae_float_t _time, const aeMovieSubComposition * _subcomposition )
{
    ae_float_t duration = _compositionData->duration;

    if( _time < 0.f )
    {
        _time = 0.f;
    }

    ae_float_t lastFrame = duration - _compositionData->frameDuration;

    ae_bool_t animation_end = AE_FALSE;

    if( _time + f_neps >= lastFrame )
    {
        _time = lastFrame + f_neps;

        animation_end = AE_TRUE;
    }

    ae_float_t current_animation_time = _animation->time;

    if( ae_equal_f_f( current_animation_time, _time ) == AE_TRUE )
    {
        return;
    }

    ae_uint32_t update_revision = __inc_composition_update_revision( _composition );

    if( current_animation_time > _time )
    {
        __skip_movie_composition_node( _composition, _compositionData, _animation, _subcomposition, update_revision, current_animation_time, _composition->composition_data->duration );

        update_revision = __inc_composition_update_revision( _composition );

        _animation->time = _time;

        __update_movie_composition_node( _composition, _compositionData, _animation, _subcomposition, update_revision, 0.f, animation_end );
    }
    else
    {
        _animation->time = _time;

        __update_movie_composition_node( _composition, _compositionData, _animation, _subcomposition, update_revision, current_animation_time, animation_end );
    }
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_set_movie_composition_work_area( const aeMovieComposition * _composition, ae_time_t _begin, ae_time_t _end )
{
    ae_time_t timescale_begin = AE_TIME_INSCALE( _begin );
    ae_time_t timescale_end = AE_TIME_INSCALE( _end );

    const aeMovieCompositionData * composition_data = _composition->composition_data;
    ae_time_t duration = composition_data->duration;

    if( timescale_begin < 0.f || timescale_end < 0.f || timescale_begin > duration || timescale_end > duration || timescale_begin > timescale_end )
    {
        return AE_FALSE;
    }

    aeMovieCompositionAnimation * animation = _composition->animation;

    animation->work_area_begin = timescale_begin;
    animation->work_area_end = timescale_end;

    if( animation->time < timescale_begin || animation->time >= timescale_end )
    {
        __set_movie_composition_time( _composition, composition_data, animation, timescale_begin, AE_NULLPTR );
    }

    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_remove_movie_composition_work_area( const  aeMovieComposition * _composition )
{
    const aeMovieCompositionData * composition_data = _composition->composition_data;
    aeMovieCompositionAnimation * animation = _composition->animation;

    animation->work_area_begin = 0.f;
    animation->work_area_end = composition_data->duration;

    __set_movie_composition_time( _composition, composition_data, animation, 0.f, AE_NULLPTR );
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_play_movie_composition( const aeMovieComposition * _composition, ae_time_t _time )
{
    ae_time_t timescale_time = AE_TIME_INSCALE( _time );

    aeMovieCompositionAnimation * animation = _composition->animation;

    if( animation->play == AE_TRUE )
    {
        animation->interrupt = AE_FALSE;

        return;
    }

    if( timescale_time >= 0.f )
    {
        ae_time_t work_time = ae_minimax_f_f( timescale_time, animation->work_area_begin, animation->work_area_end );

        const aeMovieCompositionData * composition_data = _composition->composition_data;

        __set_movie_composition_time( _composition, composition_data, animation, work_time, AE_NULLPTR );
    }

    animation->play = AE_TRUE;
    animation->pause = AE_FALSE;
    animation->interrupt = AE_FALSE;

    aeMovieCompositionStateCallbackData callbackData;
    callbackData.state = AE_MOVIE_COMPOSITION_PLAY;

    (*_composition->providers.composition_state)(&callbackData, _composition->provider_userdata);
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_stop_movie_composition( const aeMovieComposition * _composition )
{
    aeMovieCompositionAnimation * animation = _composition->animation;

    if( animation->play == AE_FALSE )
    {
        animation->interrupt = AE_FALSE;

        return;
    }

    const aeMovieCompositionData * composition_data = _composition->composition_data;

    __set_movie_composition_time( _composition, composition_data, animation, animation->work_area_begin, AE_NULLPTR );

    animation->play = AE_FALSE;
    animation->pause = AE_FALSE;
    animation->interrupt = AE_FALSE;

    __notify_stop_nodies2( _composition );

    aeMovieCompositionStateCallbackData callbackData;
    callbackData.state = AE_MOVIE_COMPOSITION_STOP;

    (*_composition->providers.composition_state)(&callbackData, _composition->provider_userdata);
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_interrupt_movie_composition( const aeMovieComposition * _composition, ae_bool_t _skip )
{
    aeMovieCompositionAnimation * animation = _composition->animation;

    if( animation->play == AE_FALSE )
    {
        return;
    }

    if( animation->pause == AE_TRUE )
    {
        return;
    }

    if( animation->interrupt == AE_TRUE )
    {
        return;
    }

    animation->interrupt = AE_TRUE;

    if( _skip == AE_TRUE )
    {
        ae_float_t loop_work_end = __get_animation_loop_work_end( animation );

        if( animation->time < loop_work_end )
        {
            const aeMovieCompositionData * composition_data = _composition->composition_data;

            __set_movie_composition_time( _composition, composition_data, animation, loop_work_end, AE_NULLPTR );
        }
    }

    aeMovieCompositionStateCallbackData callbackData;
    callbackData.state = AE_MOVIE_COMPOSITION_INTERRUPT;

    (*_composition->providers.composition_state)(&callbackData, _composition->provider_userdata);
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_set_movie_composition_time( const aeMovieComposition * _composition, ae_time_t _time )
{
    ae_time_t timescale_time = AE_TIME_INSCALE( _time );

    const aeMovieCompositionData * composition_data = _composition->composition_data;
    aeMovieCompositionAnimation * animation = _composition->animation;

    __set_movie_composition_time( _composition, composition_data, animation, timescale_time, AE_NULLPTR );

    const aeMovieSubComposition *it_subcomposition = _composition->subcompositions;
    const aeMovieSubComposition *it_subcomposition_end = _composition->subcompositions + _composition->subcomposition_count;
    for( ; it_subcomposition != it_subcomposition_end; ++it_subcomposition )
    {
        const aeMovieSubComposition * subcomposition = it_subcomposition;

        aeMovieCompositionAnimation * subcomposition_animation = subcomposition->animation;
        const aeMovieCompositionData * subcomposition_composition_data = subcomposition->composition_data;

        __refresh_movie_composition_matrix( _composition, subcomposition_composition_data, subcomposition_animation, subcomposition );
    }
}
//////////////////////////////////////////////////////////////////////////
const ae_char_t * ae_get_movie_composition_name( const aeMovieComposition * _composition )
{
    const aeMovieCompositionData * composition_data = _composition->composition_data;

    const ae_char_t * name = ae_get_movie_composition_data_name( composition_data );

    return name;
}
//////////////////////////////////////////////////////////////////////////
ae_time_t ae_get_movie_composition_time( const aeMovieComposition * _composition )
{
    const aeMovieCompositionAnimation * animation = _composition->animation;

    ae_time_t time = animation->time;

    return AE_TIME_OUTSCALE( time );
}
//////////////////////////////////////////////////////////////////////////
ae_time_t ae_get_movie_composition_duration( const aeMovieComposition * _composition )
{
    const aeMovieCompositionData * composition_data = _composition->composition_data;

    ae_time_t duration = composition_data->duration;

    return AE_TIME_OUTSCALE( duration );
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_get_movie_composition_in_out_loop( const aeMovieComposition * _composition, ae_time_t * _in, ae_time_t * _out )
{
    aeMovieCompositionAnimation * animation = _composition->animation;
    ae_float_t work_begin = ae_max_f_f( animation->loop_segment_begin, animation->work_area_begin );
    ae_float_t work_end = ae_min_f_f( animation->loop_segment_end, animation->work_area_end );

    *_in = AE_TIME_OUTSCALE( work_begin );
    *_out = AE_TIME_OUTSCALE( work_end );
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_set_movie_composition_slot_userdata( const aeMovieComposition * _composition, const ae_char_t * _name, ae_voidptr_t _userdata )
{
    const aeMovieInstance * instance = _composition->movie_data->instance;

    aeMovieNode *it_node = _composition->nodes;
    aeMovieNode *it_node_end = _composition->nodes + _composition->node_count;
    for( ; it_node != it_node_end; ++it_node )
    {
        aeMovieNode * node = it_node;

        const aeMovieLayerData * layer = node->layer;

        if( layer->type != AE_MOVIE_LAYER_TYPE_SLOT )
        {
            continue;
        }

        if( AE_STRNCMP( instance, layer->name, _name, AE_MOVIE_MAX_LAYER_NAME ) != 0 )
        {
            continue;
        }

        node->element_userdata = _userdata;

        return AE_TRUE;
    }

    return AE_FALSE;
}
//////////////////////////////////////////////////////////////////////////
ae_userdata_t ae_get_movie_composition_slot_userdata( const aeMovieComposition * _composition, const ae_char_t * _name )
{
    const aeMovieInstance * instance = _composition->movie_data->instance;

    const aeMovieNode *it_node = _composition->nodes;
    const aeMovieNode *it_node_end = _composition->nodes + _composition->node_count;
    for( ; it_node != it_node_end; ++it_node )
    {
        const aeMovieNode * node = it_node;

        const aeMovieLayerData * layer = node->layer;

        if( layer->type != AE_MOVIE_LAYER_TYPE_SLOT )
        {
            continue;
        }

        if( AE_STRNCMP( instance, layer->name, _name, AE_MOVIE_MAX_LAYER_NAME ) != 0 )
        {
            continue;
        }

        return node->element_userdata;
    }

    return AE_USERDATA_NULL;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_has_movie_composition_slot( const aeMovieComposition * _composition, const ae_char_t * _name )
{
    const aeMovieInstance * instance = _composition->movie_data->instance;

    const aeMovieNode *it_node = _composition->nodes;
    const aeMovieNode *it_node_end = _composition->nodes + _composition->node_count;
    for( ; it_node != it_node_end; ++it_node )
    {
        const aeMovieNode * node = it_node;

        const aeMovieLayerData * layer = node->layer;

        if( layer->type != AE_MOVIE_LAYER_TYPE_SLOT )
        {
            continue;
        }

        if( AE_STRNCMP( instance, layer->name, _name, AE_MOVIE_MAX_LAYER_NAME ) != 0 )
        {
            continue;
        }

        return AE_TRUE;
    }

    return AE_FALSE;
}
//////////////////////////////////////////////////////////////////////////
ae_userdata_t ae_remove_movie_composition_slot( const aeMovieComposition * _composition, const ae_char_t * _name )
{
    const aeMovieInstance * instance = _composition->movie_data->instance;

    aeMovieNode *it_node = _composition->nodes;
    aeMovieNode *it_node_end = _composition->nodes + _composition->node_count;
    for( ; it_node != it_node_end; ++it_node )
    {
        aeMovieNode * node = it_node;

        const aeMovieLayerData * layer = node->layer;

        if( layer->type != AE_MOVIE_LAYER_TYPE_SLOT )
        {
            continue;
        }

        if( AE_STRNCMP( instance, layer->name, _name, AE_MOVIE_MAX_LAYER_NAME ) != 0 )
        {
            continue;
        }

        ae_userdata_t prev_element_userdata = node->element_userdata;

        node->element_userdata = AE_NULLPTR;

        return prev_element_userdata;
    }

    return AE_USERDATA_NULL;
}
//////////////////////////////////////////////////////////////////////////
ae_userdata_t ae_get_movie_composition_camera_userdata( const aeMovieComposition * _composition )
{
    return _composition->camera_userdata;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_get_movie_composition_socket( const aeMovieComposition * _composition, const ae_char_t * _slotName, const ae_polygon_t ** _polygon )
{
    const aeMovieInstance * instance = _composition->movie_data->instance;

    const aeMovieNode *it_node = _composition->nodes;
    const aeMovieNode *it_node_end = _composition->nodes + _composition->node_count;
    for( ; it_node != it_node_end; ++it_node )
    {
        const aeMovieNode * node = it_node;

        const aeMovieLayerData * layer = node->layer;

        if( layer->type != AE_MOVIE_LAYER_TYPE_SOCKET )
        {
            continue;
        }

        if( AE_STRNCMP( instance, layer->name, _slotName, AE_MOVIE_MAX_LAYER_NAME ) != 0 )
        {
            continue;
        }

        const aeMovieLayerExtensionPolygon * polygon = layer->extensions->polygon;

        if( polygon->immutable == AE_TRUE )
        {
            *_polygon = &polygon->immutable_polygon;
        }
        else
        {
            ae_float_t t;
            ae_uint32_t frame = __compute_movie_node_frame( node, &t );

            *_polygon = polygon->polygons + frame;
        }

        return AE_TRUE;
    }

    return AE_FALSE;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_compute_movie_mesh( const aeMovieComposition * _composition, ae_uint32_t * _iterator, aeMovieRenderMesh * _render )
{
    ae_bool_t composition_interpolate = _composition->interpolate;

    ae_uint32_t render_node_index = *_iterator;
    ae_uint32_t render_node_max_count = _composition->node_count;

    ae_uint32_t iterator = render_node_index;
    for( ; iterator != render_node_max_count; ++iterator )
    {
        const aeMovieNode * node = _composition->nodes + iterator;

        if( node->active == AE_FALSE )
        {
            continue;
        }

        if( node->enable == AE_FALSE )
        {
            continue;
        }

        if( node->transparent == AE_TRUE )
        {
            continue;
        }

        if( node->track_matte_node != AE_NULLPTR && node->track_matte_node->active == AE_FALSE )
        {
            continue;
        }

        if( node->subcomposition != AE_NULLPTR && node->subcomposition->animation->enable == AE_FALSE )
        {
            continue;
        }

        const aeMovieLayerData * layer = node->layer;

        if( layer->renderable == AE_FALSE )
        {
            continue;
        }

        *_iterator = iterator + 1U;

        __compute_movie_render_mesh( _composition, node, _render, composition_interpolate, AE_FALSE );

        return AE_TRUE;
    }

    return AE_FALSE;
}
//////////////////////////////////////////////////////////////////////////
ae_uint32_t ae_get_movie_render_mesh_count( const aeMovieComposition * _composition )
{
    ae_uint32_t count = 0;

    ae_uint32_t render_node_max_count = _composition->node_count;

    ae_uint32_t iterator = 0U;
    for( ; iterator != render_node_max_count; ++iterator )
    {
        const aeMovieNode * node = _composition->nodes + iterator;

        if( node->active == AE_FALSE )
        {
            continue;
        }

        const aeMovieLayerData * layer = node->layer;

        if( layer->renderable == AE_FALSE )
        {
            continue;
        }

        if( node->track_matte_node != AE_NULLPTR && node->track_matte_node->active == AE_FALSE )
        {
            continue;
        }

        ++count;
    }

    return count;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_has_movie_composition_node( const aeMovieComposition * _composition, const ae_char_t * _layerName, aeMovieLayerTypeEnum _type )
{
    const aeMovieInstance * instance = _composition->movie_data->instance;

    const aeMovieNode *it_node = _composition->nodes;
    const aeMovieNode *it_node_end = _composition->nodes + _composition->node_count;
    for( ; it_node != it_node_end; ++it_node )
    {
        const aeMovieNode * node = it_node;

        const aeMovieLayerData * layer = node->layer;

        if( layer->type != _type )
        {
            continue;
        }

        if( AE_STRNCMP( instance, layer->name, _layerName, AE_MOVIE_MAX_LAYER_NAME ) != 0 )
        {
            continue;
        }

        return AE_TRUE;
    }

    return AE_FALSE;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_get_movie_composition_node_in_out_time( const aeMovieComposition * _composition, const ae_char_t * _layerName, aeMovieLayerTypeEnum _type, ae_time_t * _in, ae_time_t * _out )
{
    const aeMovieInstance * instance = _composition->movie_data->instance;

    const aeMovieNode *it_node = _composition->nodes;
    const aeMovieNode *it_node_end = _composition->nodes + _composition->node_count;
    for( ; it_node != it_node_end; ++it_node )
    {
        const aeMovieNode * node = it_node;

        const aeMovieLayerData * layer = node->layer;

        if( layer->type != _type )
        {
            continue;
        }

        if( AE_STRNCMP( instance, layer->name, _layerName, AE_MOVIE_MAX_LAYER_NAME ) != 0 )
        {
            continue;
        }

        *_in = AE_TIME_OUTSCALE( node->in_time );
        *_out = AE_TIME_OUTSCALE( node->out_time );

        return AE_TRUE;
    }

    return AE_FALSE;
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_set_movie_composition_nodes_enable( const aeMovieComposition * _composition, const ae_char_t * _layerName, aeMovieLayerTypeEnum _type, ae_bool_t _enable )
{
    const aeMovieInstance * instance = _composition->movie_data->instance;

    aeMovieNode *it_node = _composition->nodes;
    aeMovieNode *it_node_end = _composition->nodes + _composition->node_count;
    for( ; it_node != it_node_end; ++it_node )
    {
        aeMovieNode * node = it_node;

        const aeMovieLayerData * layer = node->layer;

        if( layer->type != _type )
        {
            continue;
        }

        if( AE_STRNCMP( instance, layer->name, _layerName, AE_MOVIE_MAX_LAYER_NAME ) != 0 )
        {
            continue;
        }

        node->enable = _enable;
    }
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_set_movie_composition_node_enable( const aeMovieComposition * _composition, const ae_char_t * _layerName, aeMovieLayerTypeEnum _type, ae_bool_t _enable )
{
    const aeMovieInstance * instance = _composition->movie_data->instance;

    aeMovieNode *it_node = _composition->nodes;
    aeMovieNode *it_node_end = _composition->nodes + _composition->node_count;
    for( ; it_node != it_node_end; ++it_node )
    {
        aeMovieNode * node = it_node;

        const aeMovieLayerData * layer = node->layer;

        if( layer->type != _type )
        {
            continue;
        }

        if( AE_STRNCMP( instance, layer->name, _layerName, AE_MOVIE_MAX_LAYER_NAME ) != 0 )
        {
            continue;
        }

        node->enable = _enable;

        return AE_TRUE;
    }

    return AE_FALSE;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_get_movie_composition_node_enable( const aeMovieComposition * _composition, const ae_char_t * _layerName, aeMovieLayerTypeEnum _type, ae_bool_t * _enable )
{
    const aeMovieInstance * instance = _composition->movie_data->instance;

    const aeMovieNode *it_node = _composition->nodes;
    const aeMovieNode *it_node_end = _composition->nodes + _composition->node_count;
    for( ; it_node != it_node_end; ++it_node )
    {
        const aeMovieNode * node = it_node;

        const aeMovieLayerData * layer = node->layer;

        if( layer->type != _type )
        {
            continue;
        }

        if( AE_STRNCMP( instance, layer->name, _layerName, AE_MOVIE_MAX_LAYER_NAME ) != 0 )
        {
            continue;
        }

        *_enable = node->enable;

        return AE_TRUE;
    }

    return AE_FALSE;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_has_movie_composition_node_any( const aeMovieComposition * _composition, const ae_char_t * _layerName )
{
    const aeMovieInstance * instance = _composition->movie_data->instance;

    const aeMovieNode *it_node = _composition->nodes;
    const aeMovieNode *it_node_end = _composition->nodes + _composition->node_count;
    for( ; it_node != it_node_end; ++it_node )
    {
        const aeMovieNode * node = it_node;

        const aeMovieLayerData * layer = node->layer;

        if( AE_STRNCMP( instance, layer->name, _layerName, AE_MOVIE_MAX_LAYER_NAME ) != 0 )
        {
            continue;
        }

        return AE_TRUE;
    }

    return AE_FALSE;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_get_movie_composition_node_in_out_time_any( const aeMovieComposition * _composition, const ae_char_t * _layerName, ae_time_t * _in, ae_time_t * _out )
{
    const aeMovieInstance * instance = _composition->movie_data->instance;

    const aeMovieNode *it_node = _composition->nodes;
    const aeMovieNode *it_node_end = _composition->nodes + _composition->node_count;
    for( ; it_node != it_node_end; ++it_node )
    {
        const aeMovieNode * node = it_node;

        const aeMovieLayerData * layer = node->layer;

        if( AE_STRNCMP( instance, layer->name, _layerName, AE_MOVIE_MAX_LAYER_NAME ) != 0 )
        {
            continue;
        }

        *_in = AE_TIME_OUTSCALE( node->in_time );
        *_out = AE_TIME_OUTSCALE( node->out_time );

        return AE_TRUE;
    }

    return AE_FALSE;
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_set_movie_composition_nodes_enable_any( const aeMovieComposition * _composition, const ae_char_t * _layerName, ae_bool_t _enable )
{
    const aeMovieInstance * instance = _composition->movie_data->instance;

    aeMovieNode *it_node = _composition->nodes;
    aeMovieNode *it_node_end = _composition->nodes + _composition->node_count;
    for( ; it_node != it_node_end; ++it_node )
    {
        aeMovieNode * node = it_node;

        const aeMovieLayerData * layer = node->layer;

        if( AE_STRNCMP( instance, layer->name, _layerName, AE_MOVIE_MAX_LAYER_NAME ) != 0 )
        {
            continue;
        }

        node->enable = _enable;
    }
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_set_movie_composition_node_enable_any( const aeMovieComposition * _composition, const ae_char_t * _layerName, ae_bool_t _enable )
{
    const aeMovieInstance * instance = _composition->movie_data->instance;

    aeMovieNode *it_node = _composition->nodes;
    aeMovieNode *it_node_end = _composition->nodes + _composition->node_count;
    for( ; it_node != it_node_end; ++it_node )
    {
        aeMovieNode * node = it_node;

        const aeMovieLayerData * layer = node->layer;

        if( AE_STRNCMP( instance, layer->name, _layerName, AE_MOVIE_MAX_LAYER_NAME ) != 0 )
        {
            continue;
        }

        node->enable = _enable;

        return AE_TRUE;
    }

    return AE_FALSE;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_get_movie_composition_node_enable_any( const aeMovieComposition * _composition, const ae_char_t * _layerName, ae_bool_t * _enable )
{
    const aeMovieInstance * instance = _composition->movie_data->instance;

    const aeMovieNode *it_node = _composition->nodes;
    const aeMovieNode *it_node_end = _composition->nodes + _composition->node_count;
    for( ; it_node != it_node_end; ++it_node )
    {
        const aeMovieNode * node = it_node;

        const aeMovieLayerData * layer = node->layer;

        if( AE_STRNCMP( instance, layer->name, _layerName, AE_MOVIE_MAX_LAYER_NAME ) != 0 )
        {
            continue;
        }

        *_enable = node->enable;

        return AE_TRUE;
    }

    return AE_FALSE;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_has_movie_sub_composition( const aeMovieComposition * _composition, const ae_char_t * _name )
{
    const aeMovieInstance * instance = _composition->movie_data->instance;

    const aeMovieSubComposition *it_subcomposition = _composition->subcompositions;
    const aeMovieSubComposition *it_subcomposition_end = _composition->subcompositions + _composition->subcomposition_count;
    for( ; it_subcomposition != it_subcomposition_end; ++it_subcomposition )
    {
        const aeMovieSubComposition * subcomposition = it_subcomposition;

        const aeMovieLayerData * layer = subcomposition->layer;

        if( AE_STRNCMP( instance, layer->name, _name, AE_MOVIE_MAX_LAYER_NAME ) != 0 )
        {
            continue;
        }

        return AE_TRUE;
    }

    return AE_FALSE;
}
//////////////////////////////////////////////////////////////////////////
const aeMovieSubComposition * ae_get_movie_sub_composition( const aeMovieComposition * _composition, const ae_char_t * _name )
{
    const aeMovieInstance * instance = _composition->movie_data->instance;

    const aeMovieSubComposition *it_subcomposition = _composition->subcompositions;
    const aeMovieSubComposition *it_subcomposition_end = _composition->subcompositions + _composition->subcomposition_count;
    for( ; it_subcomposition != it_subcomposition_end; ++it_subcomposition )
    {
        const aeMovieSubComposition * subcomposition = it_subcomposition;

        const aeMovieLayerData * layer = subcomposition->layer;

        if( AE_STRNCMP( instance, layer->name, _name, AE_MOVIE_MAX_LAYER_NAME ) != 0 )
        {
            continue;
        }

        return subcomposition;
    }

    return AE_NULLPTR;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_visit_movie_sub_composition( const aeMovieComposition * _composition, ae_movie_sub_composition_visitor_t _visitor, ae_userdata_t _ud )
{
    ae_uint32_t subcomposition_iterator = 0U;

    const aeMovieSubComposition *it_subcomposition = _composition->subcompositions;
    const aeMovieSubComposition *it_subcomposition_end = _composition->subcompositions + _composition->subcomposition_count;
    for( ; it_subcomposition != it_subcomposition_end; ++it_subcomposition )
    {
        const aeMovieSubComposition * subcomposition = it_subcomposition;

        const aeMovieLayerData * layer = subcomposition->layer;

        if( (*_visitor)(_composition, subcomposition_iterator, layer->name, subcomposition, _ud) == AE_FALSE )
        {
            return AE_FALSE;
        }

        ++subcomposition_iterator;
    }

    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
const ae_char_t * ae_get_movie_sub_composition_name( const aeMovieSubComposition * _subcomposition )
{
    const ae_char_t * name = _subcomposition->layer->name;

    return name;
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_get_movie_sub_composition_in_out_loop( const aeMovieSubComposition * _subcomposition, ae_time_t * _in, ae_time_t * _out )
{
    aeMovieCompositionAnimation * animation = _subcomposition->animation;

    ae_float_t work_begin = ae_max_f_f( animation->loop_segment_begin, animation->work_area_begin );
    ae_float_t work_end = ae_min_f_f( animation->loop_segment_end, animation->work_area_end );

    *_in = AE_TIME_OUTSCALE( work_begin );
    *_out = AE_TIME_OUTSCALE( work_end );
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_play_movie_sub_composition( const aeMovieComposition * _composition, const aeMovieSubComposition * _subcomposition, ae_time_t _time )
{
    ae_time_t timescale_time = AE_TIME_INSCALE( _time );

    const aeMovieCompositionData * composition_data = _subcomposition->composition_data;
    aeMovieCompositionAnimation * animation = _subcomposition->animation;

    if( animation->play == AE_TRUE )
    {
        animation->interrupt = AE_FALSE;

        return AE_TRUE;
    }

    if( timescale_time >= 0.f )
    {
        ae_float_t work_time = ae_minimax_f_f( timescale_time, animation->work_area_begin, animation->work_area_end );

        __set_movie_composition_time( _composition, composition_data, animation, work_time, _subcomposition );
    }

    animation->play = AE_TRUE;
    animation->pause = AE_FALSE;
    animation->interrupt = AE_FALSE;

    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_stop_movie_sub_composition( const aeMovieComposition * _composition, const aeMovieSubComposition * _subcomposition )
{
    aeMovieCompositionAnimation * animation = _subcomposition->animation;

    if( animation->play == AE_FALSE )
    {
        return AE_TRUE;
    }

    __notify_stop_nodies( _composition, _subcomposition->composition_data, animation, _subcomposition );

    const aeMovieCompositionData * composition_data = _subcomposition->composition_data;

    __set_movie_composition_time( _composition, composition_data, animation, animation->work_area_begin, _subcomposition );

    animation->play = AE_FALSE;
    animation->pause = AE_FALSE;
    animation->interrupt = AE_FALSE;

    aeMovieSubCompositionStateCallbackData callbackData;

    callbackData.state = AE_MOVIE_COMPOSITION_STOP;
    callbackData.subcomposition_userdata = _subcomposition->subcomposition_userdata;

    (*_composition->providers.subcomposition_state)(&callbackData, _composition->provider_userdata);

    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_pause_movie_sub_composition( const aeMovieComposition * _composition, const aeMovieSubComposition * _subcomposition )
{
    aeMovieCompositionAnimation * animation = _subcomposition->animation;

    if( animation->play == AE_FALSE )
    {
        return AE_TRUE;
    }

    if( animation->pause == AE_TRUE )
    {
        return AE_TRUE;
    }

    __notify_pause_nodies( _composition, _subcomposition->composition_data, animation, _subcomposition );

    animation->pause = AE_TRUE;

    aeMovieSubCompositionStateCallbackData callbackData;

    callbackData.state = AE_MOVIE_COMPOSITION_PAUSE;
    callbackData.subcomposition_userdata = _subcomposition->subcomposition_userdata;

    (*_composition->providers.subcomposition_state)(&callbackData, _composition->provider_userdata);

    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_resume_movie_sub_composition( const aeMovieComposition * _composition, const aeMovieSubComposition * _subcomposition )
{
    aeMovieCompositionAnimation * animation = _subcomposition->animation;

    if( animation->play == AE_FALSE )
    {
        return AE_TRUE;
    }

    if( animation->pause == AE_FALSE )
    {
        return AE_TRUE;
    }

    __notify_resume_nodies( _composition, _subcomposition->composition_data, animation, _subcomposition );

    animation->pause = AE_FALSE;

    aeMovieSubCompositionStateCallbackData callbackData;

    callbackData.state = AE_MOVIE_COMPOSITION_RESUME;
    callbackData.subcomposition_userdata = _subcomposition->subcomposition_userdata;

    (*_composition->providers.subcomposition_state)(&callbackData, _composition->provider_userdata);

    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_interrupt_movie_sub_composition( const aeMovieComposition * _composition, const aeMovieSubComposition * _subcomposition, ae_bool_t _skip )
{
    const aeMovieCompositionData * composition_data = _subcomposition->composition_data;
    aeMovieCompositionAnimation * animation = _subcomposition->animation;

    if( animation->play == AE_FALSE )
    {
        return;
    }

    if( animation->pause == AE_TRUE )
    {
        return;
    }

    if( animation->interrupt == AE_TRUE )
    {
        return;
    }

    animation->interrupt = AE_TRUE;

    if( _skip == AE_TRUE )
    {
        ae_float_t loop_work_end = __get_animation_loop_work_end( animation );

        if( animation->time < loop_work_end )
        {
            __set_movie_composition_time( _composition, composition_data, animation, loop_work_end, _subcomposition );
        }
    }

    aeMovieSubCompositionStateCallbackData callbackData;

    callbackData.state = AE_MOVIE_COMPOSITION_INTERRUPT;
    callbackData.subcomposition_userdata = _subcomposition->subcomposition_userdata;

    (*_composition->providers.subcomposition_state)(&callbackData, _composition->provider_userdata);
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_is_play_movie_sub_composition( const aeMovieSubComposition * _subcomposition )
{
    const aeMovieCompositionAnimation * animation = _subcomposition->animation;

    ae_bool_t play = animation->play;

    return play;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_is_pause_movie_sub_composition( const aeMovieSubComposition * _subcomposition )
{
    const aeMovieCompositionAnimation * animation = _subcomposition->animation;

    ae_bool_t pause = animation->pause;

    return pause;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_is_interrupt_movie_sub_composition( const aeMovieSubComposition * _subcomposition )
{
    const aeMovieCompositionAnimation * animation = _subcomposition->animation;

    ae_bool_t interrupt = animation->interrupt;

    return interrupt;
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_set_movie_sub_composition_time( const aeMovieComposition * _composition, const aeMovieSubComposition * _subcomposition, ae_time_t _time )
{
    ae_time_t timescale_time = AE_TIME_INSCALE( _time );

    const aeMovieCompositionData * composition_data = _subcomposition->composition_data;
    aeMovieCompositionAnimation * animation = _subcomposition->animation;

    __set_movie_composition_time( _composition, composition_data, animation, timescale_time, _subcomposition );
}
//////////////////////////////////////////////////////////////////////////
ae_time_t ae_get_movie_sub_composition_time( const aeMovieSubComposition * _subcomposition )
{
    const aeMovieCompositionAnimation * animation = _subcomposition->animation;

    ae_time_t time = animation->time;

    return AE_TIME_OUTSCALE( time );
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_set_movie_sub_composition_loop( const aeMovieSubComposition * _subcomposition, ae_bool_t _loop )
{
    aeMovieCompositionAnimation * animation = _subcomposition->animation;

    animation->loop = _loop;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_get_movie_sub_composition_loop( const aeMovieSubComposition * _subcomposition )
{
    const aeMovieCompositionAnimation * animation = _subcomposition->animation;

    ae_bool_t loop = animation->loop;

    return loop;
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_set_movie_sub_composition_enable( const aeMovieSubComposition * _subcomposition, ae_bool_t _enable )
{
    aeMovieCompositionAnimation * animation = _subcomposition->animation;

    animation->enable = _enable;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_get_movie_sub_composition_enable( const aeMovieSubComposition * _subcomposition )
{
    const aeMovieCompositionAnimation * animation = _subcomposition->animation;

    ae_bool_t enable = animation->enable;

    return enable;
}
//////////////////////////////////////////////////////////////////////////
ae_bool_t ae_set_movie_sub_composition_work_area( const aeMovieComposition * _composition, const aeMovieSubComposition * _subcomposition, ae_time_t _begin, ae_time_t _end )
{
    ae_time_t timescale_begin = AE_TIME_INSCALE( _begin );
    ae_time_t timescale_end = AE_TIME_INSCALE( _end );

    ae_float_t duration = _composition->composition_data->duration;

    if( timescale_begin < 0.f || timescale_end < 0.f || timescale_begin > duration || timescale_end > duration || timescale_begin > timescale_end )
    {
        return AE_FALSE;
    }

    const aeMovieCompositionData * composition_data = _subcomposition->composition_data;
    aeMovieCompositionAnimation * animation = _subcomposition->animation;

    animation->work_area_begin = timescale_begin;
    animation->work_area_end = timescale_end;

    if( animation->time < timescale_begin || animation->time >= timescale_end )
    {
        __set_movie_composition_time( _composition, composition_data, animation, timescale_begin, _subcomposition );
    }

    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_remove_movie_sub_composition_work_area( const aeMovieComposition * _composition, const aeMovieSubComposition * _subcomposition )
{
    const aeMovieCompositionData * composition_data = _subcomposition->composition_data;
    aeMovieCompositionAnimation * animation = _subcomposition->animation;

    animation->work_area_begin = 0.f;
    animation->work_area_end = _composition->composition_data->duration;

    __set_movie_composition_time( _composition, composition_data, animation, 0.f, _subcomposition );
}
//////////////////////////////////////////////////////////////////////////
const aeMovieCompositionData * ae_get_movie_sub_composition_composition_data( const aeMovieSubComposition * _subcomposition )
{
    const aeMovieCompositionData * composition_data = _subcomposition->subcomposition_data;

    return composition_data;
}
//////////////////////////////////////////////////////////////////////////