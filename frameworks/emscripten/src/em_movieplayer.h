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

#ifndef EM_MOVIEPLAYER_H_
#define EM_MOVIEPLAYER_H_

#include <emscripten.h>

#include <stdint.h>

typedef struct em_player_t em_player_t;
typedef struct em_movie_data_t em_movie_data_t;
typedef struct em_movie_composition_t em_movie_composition_t;

EMSCRIPTEN_KEEPALIVE em_player_t * em_create_player( const char * _hashkey, const char * _folder, uint32_t _width, uint32_t _height, uint32_t _ud );
EMSCRIPTEN_KEEPALIVE void em_delete_player( em_player_t * _instance );

EMSCRIPTEN_KEEPALIVE em_movie_data_t * em_create_movie_data( em_player_t * _player, const uint8_t * _data );
EMSCRIPTEN_KEEPALIVE void em_delete_movie_data( em_movie_data_t * _movieData );

EMSCRIPTEN_KEEPALIVE em_movie_composition_t * em_create_movie_composition( em_player_t * _player, em_movie_data_t * _movieData, const char * _name );
EMSCRIPTEN_KEEPALIVE void em_delete_movie_composition( em_movie_composition_t * _composition );

EMSCRIPTEN_KEEPALIVE void em_set_movie_composition_loop( em_movie_composition_t * _composition, uint32_t _loop );
EMSCRIPTEN_KEEPALIVE uint32_t em_get_movie_composition_loop( em_movie_composition_t * _composition );

EMSCRIPTEN_KEEPALIVE void em_set_movie_composition_work_area( em_movie_composition_t * _composition, float _begin, float _end );
EMSCRIPTEN_KEEPALIVE void em_remove_movie_composition_work_area( em_movie_composition_t * _composition );

EMSCRIPTEN_KEEPALIVE void em_play_movie_composition( em_movie_composition_t * _composition, float _time );
EMSCRIPTEN_KEEPALIVE void em_stop_movie_composition( em_movie_composition_t * _composition );
EMSCRIPTEN_KEEPALIVE void em_pause_movie_composition( em_movie_composition_t * _composition );
EMSCRIPTEN_KEEPALIVE void em_resume_movie_composition( em_movie_composition_t * _composition );
EMSCRIPTEN_KEEPALIVE void em_interrupt_movie_composition( em_movie_composition_t * _composition, uint32_t _skip );

EMSCRIPTEN_KEEPALIVE void em_set_movie_composition_time( em_movie_composition_t * _composition, float _time );
EMSCRIPTEN_KEEPALIVE float em_get_movie_composition_time( em_movie_composition_t * _composition );

EMSCRIPTEN_KEEPALIVE float em_get_movie_composition_duration( em_movie_composition_t * _composition );
EMSCRIPTEN_KEEPALIVE float em_get_movie_composition_width( em_movie_composition_t * _composition );
EMSCRIPTEN_KEEPALIVE float em_get_movie_composition_height( em_movie_composition_t * _composition );
EMSCRIPTEN_KEEPALIVE uint32_t em_get_movie_composition_frame_count( em_movie_composition_t * _composition );

EMSCRIPTEN_KEEPALIVE float em_get_movie_composition_in_loop( em_movie_composition_t * _composition );
EMSCRIPTEN_KEEPALIVE float em_get_movie_composition_out_loop( em_movie_composition_t * _composition );

EMSCRIPTEN_KEEPALIVE uint32_t em_has_movie_composition_node( em_movie_composition_t * _composition, const char * _layer );

EMSCRIPTEN_KEEPALIVE float em_get_movie_composition_node_in_time( em_movie_composition_t * _composition, const char * _layer );
EMSCRIPTEN_KEEPALIVE float em_get_movie_composition_node_out_time( em_movie_composition_t * _composition, const char * _layer );

EMSCRIPTEN_KEEPALIVE void em_set_movie_composition_node_enable( em_movie_composition_t * _composition, const char * _layer, uint32_t _enable );
EMSCRIPTEN_KEEPALIVE uint32_t em_get_movie_composition_node_enable( em_movie_composition_t * _composition, const char * _layer );

EMSCRIPTEN_KEEPALIVE void em_update_movie_composition( em_player_t * _player, em_movie_composition_t * _composition, float _time );
EMSCRIPTEN_KEEPALIVE void em_render_movie_composition( em_player_t * _player, em_movie_composition_t * _composition );

EMSCRIPTEN_KEEPALIVE void em_utils_opengl_create_texture( uint32_t _id, uint32_t _width, uint32_t _height, const uint8_t * _data );
EMSCRIPTEN_KEEPALIVE void em_set_movie_composition_wm( em_movie_composition_t * _composition, float _px, float _py, float _ox, float _oy, float _sx, float _sy, float _angle );

#endif