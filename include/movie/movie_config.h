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

#ifndef MOVIE_CONFIG_H_
#define MOVIE_CONFIG_H_

#ifndef NDEBUG
#   ifndef AE_MOVIE_NO_DEBUG
#       define AE_MOVIE_DEBUG
#   endif
#endif

#ifndef AE_TIME_DEFINE
#   define AE_TIME_USE_SECOND
#endif

#ifdef AE_TIME_USE_SECOND
#   define AE_TIME_OUTSCALE( T ) (T)
#   define AE_TIME_INSCALE( T ) (T)
#   define AE_TIME_SECOND(T) (T)
#   define AE_TIME_MILLISECOND(T) ((T) * 1000.f)
#elif AE_TIME_USE_MILLISECOND
#   define AE_TIME_OUTSCALE( T ) ((T) * 1000.f)
#   define AE_TIME_INSCALE( T ) ((T) * 0.001f)
#   define AE_TIME_SECOND(T) ((T) * 0.001f)
#   define AE_TIME_MILLISECOND(T) (T)
#endif

#define AE_CALLBACK static
#define AE_INTERNAL inline static

#define AE_UNUSED(Var) ((ae_void_t)Var)

#ifndef AE_MOVIE_BEZIER_MAX_QUALITY
#   define AE_MOVIE_BEZIER_MAX_QUALITY (10U)
#endif

#ifndef AE_MOVIE_BEZIER_WARP_BASE_GRID
#   define AE_MOVIE_BEZIER_WARP_BASE_GRID (7U)
#endif 


#ifndef AE_MOVIE_LAYER_MAX_OPTIONS
#   define AE_MOVIE_LAYER_MAX_OPTIONS (8U)
#endif

#endif