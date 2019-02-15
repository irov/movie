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

#ifndef MOVIE_MATH_H_
#define MOVIE_MATH_H_

#include "movie/movie_type.h"

//////////////////////////////////////////////////////////////////////////
static const ae_float_t f_eps = 0.00001f;
static const ae_float_t f_neps = -0.00001f;
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_bool_t ae_equal_f_z( ae_float_t _a )
{
    return ((_a >= 0.f + f_neps) && (_a <= 0.f + f_eps)) ? AE_TRUE : AE_FALSE;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_bool_t ae_equal_f_f( ae_float_t _a, ae_float_t _b )
{
    return ((_a >= _b + f_neps) && (_a <= _b + f_eps)) ? AE_TRUE : AE_FALSE;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_float_t ae_min_f_f( ae_float_t _a, ae_float_t _b )
{
    return (_a > _b) ? _b : _a;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_float_t ae_max_f_f( ae_float_t _a, ae_float_t _b )
{
    return (_a > _b) ? _a : _b;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_float_t ae_minimax_f_f( ae_float_t _v, ae_float_t _min, ae_float_t _max )
{
    return (_v > _min) ? ((_v < _max) ? _v : _max) : _min;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t ae_copy_v2( ae_vector2_t _out, const ae_vector2_t _in )
{
    _out[0] = _in[0];
    _out[1] = _in[1];
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_uint32_t ae_min_u_u( ae_uint32_t _a, ae_uint32_t _b )
{
    return (_a > _b) ? _b : _a;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_uint32_t ae_max_u_u( ae_uint32_t _a, ae_uint32_t _b )
{
    return (_a > _b) ? _a : _b;
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_mul_v3_xy_m34( ae_vector3_t _out, ae_float_t _x, ae_float_t _y, const ae_matrix34_t _b );
ae_void_t ae_mul_v3_v2_m34( ae_vector3_t _out, const ae_vector2_t _a, const ae_matrix34_t _b );
ae_void_t ae_mul_m34_m34_r( ae_matrix34_t _out, const ae_matrix34_t _a, const ae_matrix34_t _b );
ae_void_t ae_mul_m34_m34( ae_matrix34_t _out, const ae_matrix34_t _a, const ae_matrix34_t _b );
ae_void_t ae_ident_m34( ae_matrix34_t _out );
ae_void_t ae_copy_m34( ae_matrix34_t _out, const ae_matrix34_t _in );
ae_void_t ae_movie_make_transformation3d_m34( ae_matrix34_t _out, const ae_vector3_t _position, const ae_vector3_t _anchor, const ae_vector3_t _scale, const ae_quaternion_t _quaternion, const ae_skew_t _skew );
ae_void_t ae_movie_make_transformation3d_m34wq( ae_matrix34_t _out, const ae_vector3_t _position, const ae_vector3_t _anchor, const ae_vector3_t _scale, const ae_skew_t _skew );
ae_void_t ae_movie_make_transformation3d_m34wsk( ae_matrix34_t _out, const ae_vector3_t _position, const ae_vector3_t _anchor, const ae_vector3_t _scale, const ae_quaternion_t _quaternion );
ae_void_t ae_movie_make_transformation3d_m34wskq( ae_matrix34_t _out, const ae_vector3_t _position, const ae_vector3_t _anchor, const ae_vector3_t _scale );
ae_void_t ae_movie_make_transformation2d_m34( ae_matrix34_t _out, const ae_vector2_t _position, const ae_vector2_t _anchor, const ae_vector2_t _scale, const ae_quaternionzw_t _quaternion, const ae_skew_t _skew );
ae_void_t ae_movie_make_transformation2d_m34wq( ae_matrix34_t _out, const ae_vector2_t _position, const ae_vector2_t _anchor, const ae_vector2_t _scale, const ae_skew_t _skew );
ae_void_t ae_movie_make_transformation2d_m34wsk( ae_matrix34_t _out, const ae_vector2_t _position, const ae_vector2_t _anchor, const ae_vector2_t _scale, const ae_quaternionzw_t _quaternion );
ae_void_t ae_movie_make_transformation2d_m34wskq( ae_matrix34_t _out, const ae_vector2_t _position, const ae_vector2_t _anchor, const ae_vector2_t _scale );
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_float_t ae_linerp_f1( ae_float_t _in1, ae_float_t _in2, ae_float_t _t )
{
    return _in1 * (1.f - _t) + _in2 * _t;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t ae_linerp_f2( ae_vector2_t _out, const ae_vector2_t _in1, const ae_vector2_t _in2, ae_float_t _t )
{
    _out[0] = ae_linerp_f1( _in1[0], _in2[0], _t );
    _out[1] = ae_linerp_f1( _in1[1], _in2[1], _t );
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_float_t ae_fractional_f( ae_float_t value )
{
    ae_float_t ff = value + 16777215.f;
    ff -= 16777215.f;

    if( ff > value )
    {
        ff -= 1.f;
    }

    ae_float_t fractional = value - ff;

    if( fractional >= 1.f )
    {
        return 0.f;
    }

    return fractional;
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_linerp_q( ae_quaternion_t _q, const ae_quaternion_t _q1, const ae_quaternion_t _q2, ae_float_t _t );
ae_void_t ae_linerp_qzw( ae_quaternionzw_t _q, const ae_quaternionzw_t _q1, const ae_quaternionzw_t _q2, ae_float_t _t );
//////////////////////////////////////////////////////////////////////////
#endif