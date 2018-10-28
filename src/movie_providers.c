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
ae_void_t ae_clear_movie_composition_providers( aeMovieCompositionProviders * _providers )
{
    _providers->node_provider = 0;
    _providers->node_deleter = 0;
    _providers->node_update = 0;
    _providers->camera_provider = 0;
    _providers->camera_deleter = 0;
    _providers->camera_update = 0;
    _providers->track_matte_provider = 0;
    _providers->track_matte_update = 0;
    _providers->track_matte_deleter = 0;
    _providers->shader_provider = 0;
    _providers->shader_deleter = 0;
    _providers->shader_property_update = 0;
    _providers->composition_event = 0;
    _providers->composition_state = 0;
    _providers->composition_extra_interrupt = 0;
    _providers->scene_effect_provider = 0;
    _providers->scene_effect_deleter = 0;
    _providers->scene_effect_update = 0;
    _providers->subcomposition_provider = 0;
    _providers->subcomposition_deleter = 0;
    _providers->subcomposition_state = 0;
}