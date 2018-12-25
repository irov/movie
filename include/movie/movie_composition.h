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

#ifndef MOVIE_NODE_H_
#define MOVIE_NODE_H_

#include "movie_type.h"
#include "movie_typedef.h"
#include "movie_instance.h"
#include "movie_data.h"
#include "movie_render.h"
#include "movie_providers.h"

/**
@addtogroup compositions
@{
*/

/**
@brief Allocate a composition in memory.
@param [in] _movieData Resource holding movie data.
@param [in] _compositionData Resource holding composition data.
@param [in] _interpolate If TRUE, composition will be interpolated.
@param [in] _providers Set of callbacks which provide data used during playback.
@param [in] _userdata Link to the object that will hold the data providers give.
@return A composition or AE_NULL if failed.
*/
const aeMovieComposition * ae_create_movie_composition( const aeMovieData * _movieData, const aeMovieCompositionData * _compositionData, ae_bool_t _interpolate, const aeMovieCompositionProviders * _providers, ae_userdata_t _userdata );

/**
@brief Release a composition from memory.
@param [in] _composition Composition.
*/
ae_void_t ae_delete_movie_composition( const aeMovieComposition * _composition );

/**
@brief get composition data from composition.
@param [in] _composition Composition.
*/
const aeMovieCompositionData * ae_get_movie_composition_composition_data( const aeMovieComposition * _composition );

/**
@brief Get composition anchor point.
@param [in] _composition Composition.
@param [out] _point point.
@return TRUE if successful
*/
ae_bool_t ae_get_movie_composition_anchor_point( const aeMovieComposition * _composition, ae_vector3_t _point );

typedef struct aeMovieCompositionRenderInfo
{
    ae_uint32_t max_render_node;
    ae_uint32_t max_vertex_count;
    ae_uint32_t max_index_count;
} aeMovieCompositionRenderInfo;

/**
@param [in] _composition Composition.
@return Maximum number of render meshes you will ever get during a full ae_compute_movie_mesh() cycle.
*/
ae_void_t ae_calculate_movie_composition_render_info( const aeMovieComposition * _composition, aeMovieCompositionRenderInfo * _info );

/**
@brief Set whether a composition is looped or not.
@param [in] _composition Composition.
@param [in] _loop If TRUE, playback will be looped.
*/
ae_void_t ae_set_movie_composition_loop( const aeMovieComposition * _composition, ae_bool_t _loop );

/**
@param [in] _composition Composition.
@return TRUE if composition playback is looped.
*/
ae_bool_t ae_get_movie_composition_loop( const aeMovieComposition * _composition );

/**
@param [in] _composition Composition.
@return TRUE if composition playback is interpolated.
*/
ae_bool_t ae_get_movie_composition_interpolate( const aeMovieComposition * _composition );

/**
@brief Set playback area of a composition in milliseconds.
@param [in] _composition Composition.
@param [in] _begin Begin time.
@param [in] _end End time.
@return TRUE if successful.
*/
ae_bool_t ae_set_movie_composition_work_area( const aeMovieComposition * _composition, ae_time_t _begin, ae_time_t _end );

/**
@brief Restore playback area of a composition to whole-length.
@param [in] _composition Composition.
*/
ae_void_t ae_remove_movie_composition_work_area( const aeMovieComposition * _composition );

/**
@brief Start playback.
@param [in] _composition Composition.
@param [in] _time Starting time.
*/
ae_void_t ae_play_movie_composition( const aeMovieComposition * _composition, ae_time_t _time );

/**
@brief Stop playback.
@param [in] _composition Composition.
*/
ae_void_t ae_stop_movie_composition( const aeMovieComposition * _composition );

/**
@brief Pause playback.
@param [in] _composition Composition.
*/
ae_void_t ae_pause_movie_composition( const aeMovieComposition * _composition );

/**
@brief Resume playback.
@param [in] _composition Composition.
*/
ae_void_t ae_resume_movie_composition( const aeMovieComposition * _composition );

/**
@brief Play till the end of the loop area and stop, even if playback is looped.
@param [in] _composition Composition.
@param [in] _skip If TRUE, skip to the end of current animation loop.
*/
ae_void_t ae_interrupt_movie_composition( const aeMovieComposition * _composition, ae_bool_t _skip );

/**
@param [in] _composition Composition.
@return TRUE if the composition is playing.
*/
ae_bool_t ae_is_play_movie_composition( const aeMovieComposition * _composition );

/**
@param [in] _composition Composition.
@return TRUE if the composition is paused.
*/
ae_bool_t ae_is_pause_movie_composition( const aeMovieComposition * _composition );

/**
@param [in] _composition Composition.
@return TRUE if the composition has been interrupted.
*/
ae_bool_t ae_is_interrupt_movie_composition( const aeMovieComposition * _composition );

/**
@brief Set composition playback to the specified position in milliseconds.
@param [in] _composition Composition.
@param [in] _timing Position.
*/
ae_void_t ae_set_movie_composition_time( const aeMovieComposition * _composition, ae_time_t _timing );


const ae_char_t * ae_get_movie_composition_name( const aeMovieComposition * _composition );

/**
@brief Get composition current playback position in milliseconds.
@param [in] _composition Composition.
@return Position.
*/
ae_time_t ae_get_movie_composition_time( const aeMovieComposition * _composition );

/**
@brief Get composition duration in milliseconds.
@param [in] _composition Composition.
@return Duration.
*/
ae_time_t ae_get_movie_composition_duration( const aeMovieComposition * _composition );

/**
@brief Get composition loop area in milliseconds.
@param [in] _composition Composition.
@param [out] _in Begin time.
@param [out] _out End time.
*/
ae_void_t ae_get_movie_composition_in_out_loop( const aeMovieComposition * _composition, ae_time_t * _in, ae_time_t * _out );

/**
@brief Update composition nodes and state. Call this from your application update() loop.
@param [in] _composition Composition.
@param [in] _timing Time offset since the last update in milliseconds.
*/
ae_bool_t ae_update_movie_composition( const aeMovieComposition * _composition, ae_time_t _timing );

// compositions
/// @}

/**
@addtogroup slots
@{
*/

/**
@brief Set user data for a slot.
@param [in] _composition Composition.
@param [in] _name Slot name.
@param [in] _userdata Data pointer.
@return TRUE if successful.
*/
ae_bool_t ae_set_movie_composition_slot_userdata( const aeMovieComposition * _composition, const ae_char_t * _name, ae_userdata_t _userdata );

/**
@brief Search for a slot by the given name.
@param [in] _composition Composition.
@param [in] _slotName Slot name.
@return Pointer to the slot or AE_NULL if not found.
*/
ae_userdata_t ae_get_movie_composition_slot_userdata( const aeMovieComposition * _composition, const ae_char_t * _name );

/**
@brief Check for a slot by the given name.
@param [in] _composition Composition.
@param [in] _name Slot name.
@return TRUE if found.
*/
ae_bool_t ae_has_movie_composition_slot( const aeMovieComposition * _composition, const ae_char_t * _name );

/**
@brief Remove user data from the slot.
@param [in] _composition Composition.
@param [in] _name Slot name.
@return Pointer to data that was assigned to this slot prior to removal.
*/
ae_userdata_t ae_remove_movie_composition_slot( const aeMovieComposition * _composition, const ae_char_t * _name );

// slots
/// @}

/**
@brief get camera user data from composition.
@param [in] _composition Composition.
@return Pointer to the data referenced by the camera linked to the composition.
*/
ae_userdata_t ae_get_movie_composition_camera_userdata( const aeMovieComposition * _composition );

// slots
/// @}


/**
@addtogroup sockets
@{
*/

/**
@brief Search for a socket by the given name.
@param [in] _composition Composition.
@param [in] _slotName Socket name.
@param [out] _polygon Socket shape.
@return TRUE if found.
*/
ae_bool_t ae_get_movie_composition_socket( const aeMovieComposition * _composition, const ae_char_t * _slotName, const ae_polygon_t ** _polygon );

// sockets
/// @}



/**
@addtogroup compositions
@{
*/

/**
@brief Compute rendering data at the current playback time.
@param [in] _composition Composition.
@param [in] _iterator Index of the render mesh to update, can start from zero, will increase automatically inside.
@param [out] _vertices Pointer to render mesh.
@return TRUE if there are still meshes to compute in the next iteration, FALSE if finished.
*/
ae_bool_t ae_compute_movie_mesh( const aeMovieComposition * _composition, ae_uint32_t * _iterator, aeMovieRenderMesh * _vertices );

/**
@param [in] _composition Composition.
@return Number of meshes at the current playback time.
*/
uint32_t ae_get_movie_render_mesh_count( const aeMovieComposition * _composition );


/**
@brief test exist node.
@param [in] _composition Composition.
@param [in] _layerName Node name.
@param [in] _type Node type.
@return TRUE if the node is found.
*/
ae_bool_t ae_has_movie_composition_node( const aeMovieComposition * _composition, const ae_char_t * _layerName, aeMovieLayerTypeEnum _type );

/**
@brief Get node active time range in milliseconds.
Returns interval on which node is active, i.e. being played, rendered, etc.
@param [in] _composition Composition.
@param [in] _layerName Node name.
@param [in] _type Node type.
@param [out] _in Begin time.
@param [out] _out End time.
@return TRUE if the node is found.
*/
ae_bool_t ae_get_movie_composition_node_in_out_time( const aeMovieComposition * _composition, const ae_char_t * _layerName, aeMovieLayerTypeEnum _type, ae_time_t * _in, ae_time_t * _out );


ae_void_t ae_set_movie_composition_nodes_enable( const aeMovieComposition * _composition, const ae_char_t * _layerName, aeMovieLayerTypeEnum _type, ae_bool_t _enable );

/**
@brief Toggle layer usage.
@param [in] _composition Composition.
@param [in] _layerName Node name.
@param [in] _type Node type.
@param [in] _enable If TRUE, enable usage.
@return TRUE if the node is found.
*/
ae_bool_t ae_set_movie_composition_node_enable( const aeMovieComposition * _composition, const ae_char_t * _layerName, aeMovieLayerTypeEnum _type, ae_bool_t _enable );

/**
@brief Query whether the given node is active or not.
@param [in] _composition Composition.
@param [in] _layerName Node name.
@param [in] _type Node type.
@param [out] _enable TRUE if enabled.
@return TRUE if the node is found.
*/
ae_bool_t ae_get_movie_composition_node_enable( const aeMovieComposition * _composition, const ae_char_t * _layerName, aeMovieLayerTypeEnum _type, ae_bool_t * _enable );

/**
@brief test exist node.
@param [in] _composition Composition.
@param [in] _layerName Node name.
@param [in] _type Node type.
@return TRUE if the node is found.
*/
ae_bool_t ae_has_movie_composition_node_any( const aeMovieComposition * _composition, const ae_char_t * _layerName );

/**
@brief Get node active time range in milliseconds.
Returns interval on which node is active, i.e. being played, rendered, etc.
@param [in] _composition Composition.
@param [in] _layerName Node name.
@param [in] _type Node type.
@param [out] _in Begin time.
@param [out] _out End time.
@return TRUE if the node is found.
*/
ae_bool_t ae_get_movie_composition_node_in_out_time_any( const aeMovieComposition * _composition, const ae_char_t * _layerName, ae_time_t * _in, ae_time_t * _out );


ae_void_t ae_set_movie_composition_nodes_enable_any( const aeMovieComposition * _composition, const ae_char_t * _layerName, ae_bool_t _enable );

/**
@brief Toggle layer usage.
@param [in] _composition Composition.
@param [in] _layerName Node name.
@param [in] _type Node type.
@param [in] _enable If TRUE, enable usage.
@return TRUE if the node is found.
*/
ae_bool_t ae_set_movie_composition_node_enable_any( const aeMovieComposition * _composition, const ae_char_t * _layerName, ae_bool_t _enable );

/**
@brief Query whether the given node is active or not.
@param [in] _composition Composition.
@param [in] _layerName Node name.
@param [in] _type Node type.
@param [out] _enable TRUE if enabled.
@return TRUE if the node is found.
*/
ae_bool_t ae_get_movie_composition_node_enable_any( const aeMovieComposition * _composition, const ae_char_t * _layerName, ae_bool_t * _enable );


// compositions
/// @}



/**
@addtogroup subcompositions
@{
*/

ae_bool_t ae_has_movie_sub_composition( const aeMovieComposition * _composition, const ae_char_t * _name );

/**
@brief Search for a sub-composition by name.
@param [in] _composition Composition.
@param [in] _name Name of a sub-composition to search for.
@return Pointer to the sub-composition or AE_NULL if not found.
*/
const aeMovieSubComposition * ae_get_movie_sub_composition( const aeMovieComposition * _composition, const ae_char_t * _name );


typedef ae_bool_t( *ae_movie_sub_composition_visitor_t )(const aeMovieComposition * _compositionData, ae_uint32_t _index, const ae_char_t * _name, const aeMovieSubComposition * _subcomposition, ae_userdata_t _ud);

ae_bool_t ae_visit_movie_sub_composition( const aeMovieComposition * _composition, ae_movie_sub_composition_visitor_t _visitor, ae_userdata_t _ud );

/**
@brief Get name of a sub-composition.
@param [in] _subcomposition Sub-composition.
@return Name.
*/
const ae_char_t * ae_get_movie_sub_composition_name( const aeMovieSubComposition * _subcomposition );

/**
@brief Get sub-composition loop area in milliseconds.
@param [in] _subcomposition Sub-composition.
@param [out] _in Begin time.
@param [out] _out End time.
*/
ae_void_t ae_get_movie_sub_composition_in_out_loop( const aeMovieSubComposition * _subcomposition, ae_time_t * _in, ae_time_t * _out );

/**
@brief Start playback.
@param [in] _composition Composition.
@param [in] _subcomposition Sub-composition.
@param [in] _time Start time in milliseconds.
@return TRUE if successful.
*/
ae_bool_t ae_play_movie_sub_composition( const aeMovieComposition * _composition, const aeMovieSubComposition * _subcomposition, ae_time_t _time );

/**
@brief Stop playback.
@param [in] _composition Composition.
@param [in] _subcomposition Sub-composition.
@return TRUE if successful.
*/
ae_bool_t ae_stop_movie_sub_composition( const aeMovieComposition * _composition, const aeMovieSubComposition * _subcomposition );

/**
@brief Pause playback.
@param [in] _composition Composition.
@param [in] _subcomposition Sub-composition.
@return TRUE if successful.
*/
ae_bool_t ae_pause_movie_sub_composition( const aeMovieComposition * _composition, const aeMovieSubComposition * _subcomposition );

/**
@brief Resume playback.
@param [in] _composition Composition.
@param [in] _subcomposition Sub-composition.
@return TRUE if successful.
*/
ae_bool_t ae_resume_movie_sub_composition( const aeMovieComposition * _composition, const aeMovieSubComposition * _subcomposition );

/**
@brief Play till the end of the loop area and stop, even if playback is looped.
@param [in] _composition Composition.
@param [in] _subcomposition Sub-composition.
@param [in] _skip If TRUE, skip to the end of current animation loop.
*/
ae_void_t ae_interrupt_movie_sub_composition( const aeMovieComposition * _composition, const aeMovieSubComposition * _subcomposition, ae_bool_t _skip );

/**
@param [in] _subcomposition Sub-composition.
@return TRUE if the sub-composition is playing.
*/
ae_bool_t ae_is_play_movie_sub_composition( const aeMovieSubComposition * _subcomposition );

/**
@param [in] _subcomposition Sub-composition.
@return TRUE if the sub-composition is paused.
*/
ae_bool_t ae_is_pause_movie_sub_composition( const aeMovieSubComposition * _subcomposition );

/**
@param [in] _subcomposition Sub-composition.
@return TRUE if the sub-composition has been interrupted.
*/
ae_bool_t ae_is_interrupt_movie_sub_composition( const aeMovieSubComposition * _subcomposition );

/**
@brief Set sub-composition playback to the specified position in milliseconds.
@param [in] _composition Composition.
@param [in] _subcomposition Sub-composition.
@param [in] _timing Position.
*/
ae_void_t ae_set_movie_sub_composition_time( const aeMovieComposition * _composition, const aeMovieSubComposition * _subcomposition, ae_time_t _timing );

/**
@brief Get current playback position of a sub-composition in milliseconds.
@param [in] _subcomposition Sub-composition.
@return Time.
*/
ae_time_t ae_get_movie_sub_composition_time( const aeMovieSubComposition * _subcomposition );

/**
@brief Set whether a sub-composition is looped or not.
@param [in] _subcomposition Sub-composition.
@param [in] _loop If TRUE, playback will be looped.
*/
ae_void_t ae_set_movie_sub_composition_loop( const aeMovieSubComposition * _subcomposition, ae_bool_t _loop );

/**
@param [in] _subcomposition Sub-composition.
@return TRUE if sub-composition playback is looped.
*/
ae_bool_t ae_get_movie_sub_composition_loop( const aeMovieSubComposition * _subcomposition );

/**
@brief Set whether a sub-composition is enable or not.
@param [in] _subcomposition Sub-composition.
@param [in] _loop If TRUE, playback will be looped.
*/
ae_void_t ae_set_movie_sub_composition_enable( const aeMovieSubComposition * _subcomposition, ae_bool_t _enable );

/**
@param [in] _subcomposition Sub-composition.
@return TRUE if sub-composition playback is enable.
*/
ae_bool_t ae_get_movie_sub_composition_enable( const aeMovieSubComposition * _subcomposition );

/**
@brief Set playback area of a sub-composition in milliseconds.
@param [in] _composition Composition.
@param [in] _subcomposition Sub-composition.
@param [in] _begin Begin time.
@param [in] _end End time.
@return TRUE if successful.
*/
ae_bool_t ae_set_movie_sub_composition_work_area( const aeMovieComposition * _composition, const aeMovieSubComposition * _subcomposition, ae_time_t _begin, ae_time_t _end );

/**
@brief Restore playback area of a sub-composition to whole-length.
@param [in] _composition Composition.
@param [in] _subcomposition Sub-composition.
*/
ae_void_t ae_remove_movie_sub_composition_work_area( const aeMovieComposition * _composition, const aeMovieSubComposition * _subcomposition );

/**
@brief get composition data from subcomposition.
@param [in] _subcomposition SubComposition.
*/
const aeMovieCompositionData * ae_get_movie_sub_composition_composition_data( const aeMovieSubComposition * _subcomposition );

// subcompositions
/// @}

#endif
