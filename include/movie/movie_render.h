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

#ifndef MOVIE_RENDER_H_
#define MOVIE_RENDER_H_

#include "movie_type.h"
#include "movie_data.h"

/**
@addtogroup render
@{
*/

/**
@brief Mesh used in render loop.

Can be provided:
<ul>
<li>By ae_compute_movie_mesh().</li>
<li>As a part of track matte data structure,
which is in turn created and updated inside
user-provided callbacks (see \link #ae_movie_callback_node_provider_t node provider\endlink etc.).</li>
</ul>
*/
typedef struct aeMovieRenderMesh
{
    /**
    @brief Renderable type.
    */
    aeMovieLayerTypeEnum layer_type;

    /**
    @brief One of AE blend modes.
    */
    ae_blend_mode_t blend_mode;

    /**
    @brief Type of the resource associated with the mesh.
    */
    aeMovieResourceTypeEnum resource_type;

    /**
    @brief Pointer to the resource data.
    */
    ae_userdata_t resource_userdata;

    /**
    @brief Number of vertices.
    */
    ae_uint32_t vertexCount;

    /**
    @brief Number of indices.
    */
    ae_uint32_t indexCount;

    /**
    @brief Vertex positions.
    */
    ae_vector3_t position[AE_MOVIE_MAX_VERTICES];

    /**
    @brief Texture coordinates.
    */
    ae_vector2_t uv[AE_MOVIE_MAX_VERTICES];

    ae_userdata_t uv_cache_userdata;

    /**
    @brief Triangle indices.
    */
    const ae_uint16_t * indices;

    /**
    @name Color
    @brief Usually used as a multiplier.
    @{
    */
    ae_color_t color;
    ae_color_channel_t opacity;
    /// @}

    /**
    @brief Pointer to a user-provided camera structure.
    */
    ae_userdata_t camera_userdata;

    ae_track_matte_mode_t track_matte_mode;

    /**
    @brief Pointer to a user-provided track matte structure.

    Contains track matte layer data.
    */
    ae_userdata_t track_matte_userdata;

    const ae_viewport_t * viewport;

    /**
    @brief Pointer to a user-provided shader structure.

    Contains shader layer data.
    */
    ae_userdata_t shader_userdata;

    /**
    @brief Pointer to a user-provided node structure.

    Contains data for track matte, slot, socket etc.
    */
    ae_userdata_t element_userdata;
} aeMovieRenderMesh;

// render
/// @}

#endif