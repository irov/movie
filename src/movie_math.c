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

#include "movie_math.h"

//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __mul_v3_m34_r( ae_vector3_t _out, const ae_vector3_t _a, const ae_matrix34_t _b )
{
    _out[0] = _a[0] * _b[0 * 3 + 0] + _a[1] * _b[1 * 3 + 0] + _a[2] * _b[2 * 3 + 0];
    _out[1] = _a[0] * _b[0 * 3 + 1] + _a[1] * _b[1 * 3 + 1] + _a[2] * _b[2 * 3 + 1];
    _out[2] = _a[0] * _b[0 * 3 + 2] + _a[1] * _b[1 * 3 + 2] + _a[2] * _b[2 * 3 + 2];
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __mul_v3_m34_rr( ae_vector3_t _out, const ae_vector3_t _a, const ae_matrix34_t _b )
{
    _out[0] = _a[0] * _b[0 * 3 + 0] + _a[1] * _b[1 * 3 + 0] + _a[2] * _b[2 * 3 + 0] + _b[3 * 3 + 0];
    _out[1] = _a[0] * _b[0 * 3 + 1] + _a[1] * _b[1 * 3 + 1] + _a[2] * _b[2 * 3 + 1] + _b[3 * 3 + 1];
    _out[2] = _a[0] * _b[0 * 3 + 2] + _a[1] * _b[1 * 3 + 2] + _a[2] * _b[2 * 3 + 2] + _b[3 * 3 + 2];
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_mul_v3_xy_m34( ae_vector3_t _out, ae_float_t _x, ae_float_t _y, const ae_matrix34_t _b )
{
    _out[0] = _x * _b[0 * 3 + 0] + _y * _b[1 * 3 + 0] + _b[3 * 3 + 0];
    _out[1] = _x * _b[0 * 3 + 1] + _y * _b[1 * 3 + 1] + _b[3 * 3 + 1];
    _out[2] = _x * _b[0 * 3 + 2] + _y * _b[1 * 3 + 2] + _b[3 * 3 + 2];
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_mul_v3_v2_m34( ae_vector3_t _out, const ae_vector2_t _a, const ae_matrix34_t _b )
{
    _out[0] = _a[0] * _b[0 * 3 + 0] + _a[1] * _b[1 * 3 + 0] + _b[3 * 3 + 0];
    _out[1] = _a[0] * _b[0 * 3 + 1] + _a[1] * _b[1 * 3 + 1] + _b[3 * 3 + 1];
    _out[2] = _a[0] * _b[0 * 3 + 2] + _a[1] * _b[1 * 3 + 2] + _b[3 * 3 + 2];
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_mul_m34_m34_r( ae_matrix34_t _out, const ae_matrix34_t _a, const ae_matrix34_t _b )
{
    __mul_v3_m34_r( _out + 0, _a + 0, _b );
    __mul_v3_m34_r( _out + 3, _a + 3, _b );
    __mul_v3_m34_r( _out + 6, _a + 6, _b );
    __mul_v3_m34_rr( _out + 9, _a + 9, _b );
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __mul_v3_m34( ae_vector3_t _out, const ae_vector3_t _a, const ae_matrix34_t _b )
{
    _out[0] = _a[0] * _b[0 * 3 + 0] + _a[1] * _b[1 * 3 + 0] + _a[2] * _b[2 * 3 + 0] + _b[3 * 3 + 0];
    _out[1] = _a[0] * _b[0 * 3 + 1] + _a[1] * _b[1 * 3 + 1] + _a[2] * _b[2 * 3 + 1] + _b[3 * 3 + 1];
    _out[2] = _a[0] * _b[0 * 3 + 2] + _a[1] * _b[1 * 3 + 2] + _a[2] * _b[2 * 3 + 2] + _b[3 * 3 + 2];
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_mul_m34_m34( ae_matrix34_t _out, const ae_matrix34_t _a, const ae_matrix34_t _b )
{
    __mul_v3_m34( _out + 0, _a + 0, _b );
    __mul_v3_m34( _out + 3, _a + 3, _b );
    __mul_v3_m34( _out + 6, _a + 6, _b );
    __mul_v3_m34( _out + 9, _a + 9, _b );
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_ident_m34( ae_matrix34_t _out )
{
    _out[0 * 3 + 0] = 1.f;
    _out[0 * 3 + 1] = 0.f;
    _out[0 * 3 + 2] = 0.f;

    _out[1 * 3 + 0] = 0.f;
    _out[1 * 3 + 1] = 1.f;
    _out[1 * 3 + 2] = 0.f;

    _out[2 * 3 + 0] = 0.f;
    _out[2 * 3 + 1] = 0.f;
    _out[2 * 3 + 2] = 1.f;

    _out[3 * 3 + 0] = 0.f;
    _out[3 * 3 + 1] = 0.f;
    _out[3 * 3 + 2] = 0.f;
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_copy_m34( ae_matrix34_t _out, const ae_matrix34_t _in )
{
    *_out++ = *_in++;
    *_out++ = *_in++;
    *_out++ = *_in++;

    *_out++ = *_in++;
    *_out++ = *_in++;
    *_out++ = *_in++;

    *_out++ = *_in++;
    *_out++ = *_in++;
    *_out++ = *_in++;

    *_out++ = *_in++;
    *_out++ = *_in++;
    *_out++ = *_in++;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __make_quaternion_m34( ae_matrix34_t _m, const ae_quaternion_t _quaternion )
{
    ae_float_t x = _quaternion[0];
    ae_float_t y = _quaternion[1];
    ae_float_t z = _quaternion[2];
    ae_float_t w = _quaternion[3];

    ae_float_t x2 = x + x;
    ae_float_t y2 = y + y;
    ae_float_t z2 = z + z;
    ae_float_t xx = x * x2;
    ae_float_t xy = x * y2;
    ae_float_t xz = x * z2;
    ae_float_t yy = y * y2;
    ae_float_t yz = y * z2;
    ae_float_t zz = z * z2;
    ae_float_t wx = w * x2;
    ae_float_t wy = w * y2;
    ae_float_t wz = w * z2;

    _m[0 * 3 + 0] = 1.f - (yy + zz);
    _m[0 * 3 + 1] = xy - wz;
    _m[0 * 3 + 2] = xz + wy;

    _m[1 * 3 + 0] = xy + wz;
    _m[1 * 3 + 1] = 1.f - (xx + zz);
    _m[1 * 3 + 2] = yz - wx;

    _m[2 * 3 + 0] = xz - wy;
    _m[2 * 3 + 1] = yz + wx;
    _m[2 * 3 + 2] = 1.f - (xx + yy);

    _m[3 * 3 + 0] = 0.f;
    _m[3 * 3 + 1] = 0.f;
    _m[3 * 3 + 2] = 0.f;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __make_quaternionzw_m34( ae_matrix34_t _m, const ae_quaternionzw_t _quaternion )
{
    ae_float_t z = _quaternion[0];
    ae_float_t w = _quaternion[1];

    ae_float_t z2 = z + z;
    ae_float_t zz = z * z2;
    ae_float_t wz = w * z2;

    _m[0 * 3 + 0] = 1.f - zz;
    _m[0 * 3 + 1] = -wz;
    _m[0 * 3 + 2] = 0.f;

    _m[1 * 3 + 0] = +wz;
    _m[1 * 3 + 1] = 1.f - zz;
    _m[1 * 3 + 2] = 0.f;

    _m[2 * 3 + 0] = 0.f;
    _m[2 * 3 + 1] = 0.f;
    _m[2 * 3 + 2] = 1.f;

    _m[3 * 3 + 0] = 0.f;
    _m[3 * 3 + 1] = 0.f;
    _m[3 * 3 + 2] = 0.f;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __make_transformation2d_anchor_scale_m34( ae_matrix34_t _m, const ae_vector2_t _anchor, const ae_vector2_t _scale )
{
    _m[0 * 3 + 0] = _scale[0];
    _m[0 * 3 + 1] = 0.f;
    _m[0 * 3 + 2] = 0.f;

    _m[1 * 3 + 0] = 0.f;
    _m[1 * 3 + 1] = _scale[1];
    _m[1 * 3 + 2] = 0.f;

    _m[2 * 3 + 0] = 0.f;
    _m[2 * 3 + 1] = 0.f;
    _m[2 * 3 + 2] = 1.f;

    _m[3 * 3 + 0] = -_anchor[0] * _scale[0];
    _m[3 * 3 + 1] = -_anchor[1] * _scale[1];
    _m[3 * 3 + 2] = 0.f;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __make_transformation3d_anchor_scale_m34( ae_matrix34_t _m, const ae_vector3_t _anchor, const ae_vector3_t _scale )
{
    _m[0 * 3 + 0] = _scale[0];
    _m[0 * 3 + 1] = 0.f;
    _m[0 * 3 + 2] = 0.f;

    _m[1 * 3 + 0] = 0.f;
    _m[1 * 3 + 1] = _scale[1];
    _m[1 * 3 + 2] = 0.f;

    _m[2 * 3 + 0] = 0.f;
    _m[2 * 3 + 1] = 0.f;
    _m[2 * 3 + 2] = _scale[2];

    _m[3 * 3 + 0] = -_anchor[0] * _scale[0];
    _m[3 * 3 + 1] = -_anchor[1] * _scale[1];
    _m[3 * 3 + 2] = -_anchor[2] * _scale[2];
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __make_transformation_skew_m34( ae_matrix34_t _m, const ae_float_t _skew )
{
    _m[0 * 3 + 0] = 1.f;
    _m[0 * 3 + 1] = _skew;
    _m[0 * 3 + 2] = 0.f;

    _m[1 * 3 + 0] = 0.f;
    _m[1 * 3 + 1] = 1.f;
    _m[1 * 3 + 2] = 0.f;

    _m[2 * 3 + 0] = 0.f;
    _m[2 * 3 + 1] = 0.f;
    _m[2 * 3 + 2] = 1.f;

    _m[3 * 3 + 0] = 0.f;
    _m[3 * 3 + 1] = 0.f;
    _m[3 * 3 + 2] = 0.f;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __apply_transformation_skew_m34( ae_matrix34_t _out, const ae_matrix34_t _in, const ae_skew_t _skew )
{
    ae_quaternionzw_t qzw_skew_rotate_inv;
    qzw_skew_rotate_inv[0] = -_skew[1];
    qzw_skew_rotate_inv[1] = _skew[2];

    ae_matrix34_t m4_skew_rotate_inv;
    __make_quaternionzw_m34( m4_skew_rotate_inv, qzw_skew_rotate_inv );

    ae_matrix34_t m4_skew;
    __make_transformation_skew_m34( m4_skew, _skew[0] );

    ae_quaternionzw_t qzw_skew_rotate;
    qzw_skew_rotate[0] = _skew[1];
    qzw_skew_rotate[1] = _skew[2];

    ae_matrix34_t m4_skew_rotate;
    __make_quaternionzw_m34( m4_skew_rotate, qzw_skew_rotate );

    ae_matrix34_t m4_skew_1;
    ae_mul_m34_m34_r( m4_skew_1, _in, m4_skew_rotate_inv );

    ae_matrix34_t m4_skew_2;
    ae_mul_m34_m34_r( m4_skew_2, m4_skew_1, m4_skew );

    ae_mul_m34_m34_r( _out, m4_skew_2, m4_skew_rotate );
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __apply_transformation3d_rotate_m34( ae_matrix34_t _out, const ae_matrix34_t _in, const ae_quaternion_t _quaternion )
{
    ae_matrix34_t m4_rotate;
    __make_quaternion_m34( m4_rotate, _quaternion );

    ae_mul_m34_m34_r( _out, _in, m4_rotate );
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __apply_transformation2d_rotate_m34( ae_matrix34_t _out, const ae_matrix34_t _in, const ae_quaternionzw_t _quaternion )
{
    ae_matrix34_t m4_rotate;
    __make_quaternionzw_m34( m4_rotate, _quaternion );

    ae_mul_m34_m34_r( _out, _in, m4_rotate );
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_movie_make_transformation3d_m34( ae_matrix34_t _out, const ae_vector3_t _position, const ae_vector3_t _anchor, const ae_vector3_t _scale, const ae_quaternion_t _quaternion, const ae_skew_t _skew )
{
    ae_matrix34_t m4_anchor_scale;
    __make_transformation3d_anchor_scale_m34( m4_anchor_scale, _anchor, _scale );

    ae_matrix34_t m4_anchor_scale_rotate;
    __apply_transformation3d_rotate_m34( m4_anchor_scale_rotate, m4_anchor_scale, _quaternion );

    __apply_transformation_skew_m34( _out, m4_anchor_scale_rotate, _skew );

    _out[3 * 3 + 0] += _position[0];
    _out[3 * 3 + 1] += _position[1];
    _out[3 * 3 + 2] += _position[2];
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_movie_make_transformation3d_m34wq( ae_matrix34_t _out, const ae_vector3_t _position, const ae_vector3_t _anchor, const ae_vector3_t _scale, const ae_skew_t _skew )
{
    ae_matrix34_t m4_anchor_scale;
    __make_transformation3d_anchor_scale_m34( m4_anchor_scale, _anchor, _scale );

    __apply_transformation_skew_m34( _out, m4_anchor_scale, _skew );

    _out[3 * 3 + 0] += _position[0];
    _out[3 * 3 + 1] += _position[1];
    _out[3 * 3 + 2] += _position[2];
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_movie_make_transformation3d_m34wsk( ae_matrix34_t _out, const ae_vector3_t _position, const ae_vector3_t _anchor, const ae_vector3_t _scale, const ae_quaternion_t _quaternion )
{
    ae_matrix34_t m4_anchor_scale;
    __make_transformation3d_anchor_scale_m34( m4_anchor_scale, _anchor, _scale );

    __apply_transformation3d_rotate_m34( _out, m4_anchor_scale, _quaternion );

    _out[3 * 3 + 0] += _position[0];
    _out[3 * 3 + 1] += _position[1];
    _out[3 * 3 + 2] += _position[2];
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_movie_make_transformation3d_m34wskq( ae_matrix34_t _out, const ae_vector3_t _position, const ae_vector3_t _anchor, const ae_vector3_t _scale )
{
    __make_transformation3d_anchor_scale_m34( _out, _anchor, _scale );

    _out[3 * 3 + 0] += _position[0];
    _out[3 * 3 + 1] += _position[1];
    _out[3 * 3 + 2] += _position[2];
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_movie_make_transformation2d_m34wq( ae_matrix34_t _out, const ae_vector2_t _position, const ae_vector2_t _anchor, const ae_vector2_t _scale, const ae_skew_t _skew )
{
    ae_matrix34_t m4_anchor_scale;
    __make_transformation2d_anchor_scale_m34( m4_anchor_scale, _anchor, _scale );

    __apply_transformation_skew_m34( _out, m4_anchor_scale, _skew );

    _out[3 * 3 + 0] += _position[0];
    _out[3 * 3 + 1] += _position[1];
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_movie_make_transformation2d_m34wsk( ae_matrix34_t _out, const ae_vector2_t _position, const ae_vector2_t _anchor, const ae_vector2_t _scale, const ae_quaternionzw_t _quaternion )
{
    ae_matrix34_t m4_anchor_scale;
    __make_transformation2d_anchor_scale_m34( m4_anchor_scale, _anchor, _scale );

    __apply_transformation2d_rotate_m34( _out, m4_anchor_scale, _quaternion );

    _out[3 * 3 + 0] += _position[0];
    _out[3 * 3 + 1] += _position[1];
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_movie_make_transformation2d_m34wskq( ae_matrix34_t _out, const ae_vector2_t _position, const ae_vector2_t _anchor, const ae_vector2_t _scale )
{
    __make_transformation2d_anchor_scale_m34( _out, _anchor, _scale );

    _out[3 * 3 + 0] += _position[0];
    _out[3 * 3 + 1] += _position[1];
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_movie_make_transformation2d_m34( ae_matrix34_t _out, const ae_vector2_t _position, const ae_vector2_t _anchor, const ae_vector2_t _scale, const ae_quaternionzw_t _quaternion, const ae_skew_t _skew )
{
    ae_matrix34_t m4_anchor_scale;
    __make_transformation2d_anchor_scale_m34( m4_anchor_scale, _anchor, _scale );

    ae_matrix34_t m4_anchor_scale_rotate;
    __apply_transformation2d_rotate_m34( m4_anchor_scale_rotate, m4_anchor_scale, _quaternion );

    __apply_transformation_skew_m34( _out, m4_anchor_scale_rotate, _skew );

    _out[3 * 3 + 0] += _position[0];
    _out[3 * 3 + 1] += _position[1];
}
//////////////////////////////////////////////////////////////////////////
#ifdef LIBMOVIE_EXTERNAL_INVERSE_SQRTF
extern ae_float_t libmovie_external_inverse_sqrtf( ae_float_t );

#define __inverse_sqrtf libmovie_external_inverse_sqrtf
#else
//////////////////////////////////////////////////////////////////////////
union __inverse_sqrtf_alias_cast_t
{
    ae_float_t raw;
    ae_uint32_t data;
};
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_float_t __inverse_sqrtf( ae_float_t _number )
{
    ae_float_t x2 = _number * 0.5f;

    union __inverse_sqrtf_alias_cast_t i_cast;
    i_cast.raw = _number;
    ae_uint32_t i = i_cast.data;

    i = 0x5F3759DF - (i >> 1);

    union __inverse_sqrtf_alias_cast_t f_cast;
    f_cast.data = i;

    ae_float_t y = f_cast.raw;

    y = y * (1.5f - (x2 * y * y));
    y = y * (1.5f - (x2 * y * y));

    return y;
}
#endif
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_float_t __dot_q( const ae_quaternion_t _q1, const ae_quaternion_t _q2 )
{
    return _q1[0] * _q2[0] + _q1[1] * _q2[1] + _q1[2] * _q2[2] + _q1[3] * _q2[3];
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_linerp_q( ae_quaternion_t _q, const ae_quaternion_t _q1, const ae_quaternion_t _q2, ae_float_t _t )
{
    ae_float_t inv_t = 1.f - _t;

    ae_float_t dot = __dot_q( _q1, _q2 );

    ae_float_t x;
    ae_float_t y;
    ae_float_t z;
    ae_float_t w;

    if( dot < 0.f )
    {
        x = _q1[0] * inv_t - _q2[0] * _t;
        y = _q1[1] * inv_t - _q2[1] * _t;
        z = _q1[2] * inv_t - _q2[2] * _t;
        w = _q1[3] * inv_t - _q2[3] * _t;
    }
    else
    {
        x = _q1[0] * inv_t + _q2[0] * _t;
        y = _q1[1] * inv_t + _q2[1] * _t;
        z = _q1[2] * inv_t + _q2[2] * _t;
        w = _q1[3] * inv_t + _q2[3] * _t;
    }

    ae_float_t q_dot = x * x + y * y + z * z + w * w;
    ae_float_t inv_length = __inverse_sqrtf( q_dot );

    _q[0] = x * inv_length;
    _q[1] = y * inv_length;
    _q[2] = z * inv_length;
    _q[3] = w * inv_length;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_float_t __dot_qzw( const ae_quaternionzw_t _q1, const ae_quaternionzw_t _q2 )
{
    return _q1[0] * _q2[0] + _q1[1] * _q2[1];
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_linerp_qzw( ae_quaternionzw_t _q, const ae_quaternionzw_t _q1, const ae_quaternionzw_t _q2, ae_float_t _t )
{
    ae_float_t inv_t = 1.f - _t;

    ae_float_t dot = __dot_qzw( _q1, _q2 );

    ae_float_t z;
    ae_float_t w;

    if( dot < 0.f )
    {
        z = _q1[0] * inv_t - _q2[0] * _t;
        w = _q1[1] * inv_t - _q2[1] * _t;
    }
    else
    {
        z = _q1[0] * inv_t + _q2[0] * _t;
        w = _q1[1] * inv_t + _q2[1] * _t;
    }

    ae_float_t q_dot = z * z + w * w;
    ae_float_t inv_length = __inverse_sqrtf( q_dot );

    _q[0] = z * inv_length;
    _q[1] = w * inv_length;
}

