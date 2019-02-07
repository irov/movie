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

#include "movie/movie_skeleton.h"

#include "movie_memory.h"
#include "movie_math.h"
#include "movie_struct.h"
#include "movie_debug.h"

//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_bool_t __test_movie_skeleton_base( aeMovieComposition * _base )
{
    ae_float_t duration = _base->composition_data->duration;

    const aeMovieNode * it = _base->nodes;
    const aeMovieNode * it_end = _base->nodes + _base->node_count;

    for( ;
        it != it_end;
        ++it )
    {
        const aeMovieNode * node = it;

        if( ae_equal_f_z( node->in_time ) == AE_FALSE )
        {
            return AE_FALSE;
        }

        if( ae_equal_f_f( node->out_time, duration ) == AE_FALSE )
        {
            return AE_FALSE;
        }

        if( ae_equal_f_z( node->start_time ) == AE_FALSE )
        {
            return AE_FALSE;
        }
    }

    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
aeMovieSkeleton * ae_movie_create_skeleton( aeMovieComposition * _base )
{
    if( _base == AE_NULLPTR )
    {
        return AE_NULLPTR;
    }

    if( __test_movie_skeleton_base( _base ) == AE_FALSE )
    {
        return AE_NULLPTR;
    }

    const aeMovieInstance * instance = _base->movie_data->instance;

    aeMovieSkeleton * skeleton = AE_NEW( instance, aeMovieSkeleton );

    AE_MOVIE_PANIC_MEMORY( skeleton, AE_NULLPTR );

    skeleton->base = _base;

    ae_uint32_t i = 0;
    for( ; i != 8; ++i )
    {
        skeleton->animations[i] = AE_NULLPTR;
    }

    return skeleton;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_bool_t __test_movie_skeleton_animation( aeMovieComposition * _base, aeMovieComposition * _animation )
{
    if( _base->node_count != _animation->node_count )
    {
        return AE_FALSE;
    }

    const aeMovieInstance * instance = _base->movie_data->instance;

    const aeMovieNode * it_base = _base->nodes;
    const aeMovieNode * it_base_end = _base->nodes + _base->node_count;
    const aeMovieNode * it_animation = _animation->nodes;
    //const aeMovieNode * it_animation_end = _animation->nodes + _animation->node_count;

    for( ;
        it_base != it_base_end;
        ++it_base, ++it_animation )
    {
        const aeMovieNode * base_node = it_base;
        const aeMovieNode * animation_node = it_animation;

        if( base_node->layer->type != animation_node->layer->type )
        {
            return AE_FALSE;
        }

        if( instance->strncmp( instance->instance_userdata, base_node->layer->name, animation_node->layer->name, AE_MOVIE_MAX_LAYER_NAME ) != 0 )
        {
            return AE_FALSE;
        }
    }

    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_uint32_t __movie_skeleton_find_free_animation_place( aeMovieSkeleton * _skeleton )
{
    ae_uint32_t i = 0;
    for( ; i != 8; ++i )
    {
        if( _skeleton->animations[i] != AE_NULLPTR )
        {
            continue;
        }

        return i;
    }

    return (ae_uint32_t)-1;
}

ae_bool_t ae_movie_skeleton_add_animation( aeMovieSkeleton * _skeleton, aeMovieComposition * _animation )
{
    if( __test_movie_skeleton_base( _animation ) == AE_FALSE )
    {
        return AE_FALSE;
    }

    aeMovieComposition * base = _skeleton->base;

    if( __test_movie_skeleton_animation( base, _animation ) == AE_FALSE )
    {
        return AE_FALSE;
    }

    ae_uint32_t index = __movie_skeleton_find_free_animation_place( _skeleton );

    if( index == (ae_uint32_t)-1 )
    {
        return AE_FALSE;
    }

    _skeleton->animations[index] = _animation;

    return AE_TRUE;
}

ae_void_t ae_movie_destroy_skeleton( const aeMovieSkeleton * _skeleton )
{
    AE_DELETE( _skeleton->base->movie_data->instance, _skeleton );
}
