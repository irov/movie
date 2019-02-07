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

#ifndef EM_MATH_H_
#define EM_MATH_H_

#include <math.h>

//////////////////////////////////////////////////////////////////////////
static float __sqrlength_v3( const float * _v )
{
    return	_v[0] * _v[0] + _v[1] * _v[1] + _v[2] * _v[2];
}
//////////////////////////////////////////////////////////////////////////
static float __length_v3( const float * _v )
{
    float sqrlen = __sqrlength_v3( _v );
    float len = sqrtf( sqrlen );

    return len;
}
//////////////////////////////////////////////////////////////////////////
static void __normalize_v3( float * _out, const float * _v )
{
    float len = __length_v3( _v );

    float inv_len = 1.f / len;

    _out[0] = _v[0] * inv_len;
    _out[1] = _v[1] * inv_len;
    _out[2] = _v[2] * inv_len;
}
//////////////////////////////////////////////////////////////////////////
static void __negative_v3( float * _out, const float * _v )
{
    _out[0] = -_v[0];
    _out[1] = -_v[1];
    _out[2] = -_v[2];
}
//////////////////////////////////////////////////////////////////////////
static void __cross_v3( float * _out, const float * _a, const float * _b )
{
    _out[0] = _a[1] * _b[2] - _a[2] * _b[1];
    _out[1] = _a[2] * _b[0] - _a[0] * _b[2];
    _out[2] = _a[0] * _b[1] - _a[1] * _b[0];
}
//////////////////////////////////////////////////////////////////////////
static float __dot_v3( const float * _a, const float * _b )
{
    return _a[0] * _b[0] + _a[1] * _b[1] + _a[2] * _b[2];
}
//////////////////////////////////////////////////////////////////////////
static void __make_lookat_m4( float * _out, const float * _position, const float * _target )
{
    ae_vector3_t direction;
    direction[0] = _target[0] - _position[0];
    direction[1] = _target[1] - _position[1];
    direction[2] = _target[2] - _position[2];

    ae_vector3_t zaxis;
    __normalize_v3( zaxis, direction );

    ae_vector3_t up = { 0.f, -1.f, 0.f };

    ae_vector3_t xaxis;
    __cross_v3( xaxis, up, zaxis );
    __normalize_v3( xaxis, xaxis );

    ae_vector3_t yaxis;
    __cross_v3( yaxis, zaxis, xaxis );

    __negative_v3( xaxis, xaxis );

    _out[0] = xaxis[0];
    _out[1] = yaxis[0];
    _out[2] = zaxis[0];
    _out[3] = 0.f;

    _out[4] = xaxis[1];
    _out[5] = yaxis[1];
    _out[6] = zaxis[1];
    _out[7] = 0.f;

    _out[8] = xaxis[2];
    _out[9] = yaxis[2];
    _out[10] = zaxis[2];
    _out[11] = 0.f;

    _out[12] = -__dot_v3( xaxis, _position );
    _out[13] = -__dot_v3( yaxis, _position );
    _out[14] = -__dot_v3( zaxis, _position );
    _out[15] = 1.f;
}
//////////////////////////////////////////////////////////////////////////
static void __make_projection_fov_m4( float * _out, float _width, float _height, float _fov )
{
    float aspect = _width / _height;

    float zn = 1.f;
    float zf = 10000.f;

    float h = 1.f / tanf( _fov * 0.5f );
    float w = h / aspect;

    float a = zf / (zf - zn);
    float b = -zn * zf / (zf - zn);

    _out[0] = w;
    _out[1] = 0.f;
    _out[2] = 0.f;
    _out[3] = 0.f;

    _out[4] = 0.f;
    _out[5] = h;
    _out[6] = 0.f;
    _out[7] = 0.f;

    _out[8] = 0.f;
    _out[9] = 0.f;
    _out[10] = a;
    _out[11] = 1.f;

    _out[12] = 0.f;
    _out[13] = 0.f;
    _out[14] = b;
    _out[15] = 0.f;
}
//////////////////////////////////////////////////////////////////////////
static void __make_orthogonal_m4( float * _out, float _left, float _right, float _top, float _bottom, float _near, float _far )
{
    _out[0 * 4 + 0] = 2.f / (_right - _left);
    _out[0 * 4 + 1] = 0.f;
    _out[0 * 4 + 2] = 0.f;
    _out[0 * 4 + 3] = 0.f;

    _out[1 * 4 + 0] = 0.f;
    _out[1 * 4 + 1] = 2.f / (_top - _bottom);
    _out[1 * 4 + 2] = 0.f;
    _out[1 * 4 + 3] = 0.f;

    _out[2 * 4 + 0] = 0.f;
    _out[2 * 4 + 1] = 0.f;
    _out[2 * 4 + 2] = 1.f / (_far - _near);
    _out[2 * 4 + 3] = 0.f;

    _out[3 * 4 + 0] = (_left + _right) / (_left - _right);
    _out[3 * 4 + 1] = (_bottom + _top) / (_bottom - _top);
    _out[3 * 4 + 2] = _near / (_near - _far);
    _out[3 * 4 + 3] = 1.f;
}
//////////////////////////////////////////////////////////////////////////
static void __identity_m4( float * _out )
{
    _out[0] = 1.f;
    _out[1] = 0.f;
    _out[2] = 0.f;
    _out[3] = 0.f;

    _out[4] = 0.f;
    _out[5] = 1.f;
    _out[6] = 0.f;
    _out[7] = 0.f;

    _out[8] = 0.f;
    _out[9] = 0.f;
    _out[10] = 1.f;
    _out[11] = 0.f;

    _out[12] = 0.f;
    _out[13] = 0.f;
    _out[14] = 0.f;
    _out[15] = 1.f;
}
//////////////////////////////////////////////////////////////////////////
static void __mul_v4_m4( float * _out, const float * _a, const float * _b )
{
    _out[0] = _a[0] * _b[0 * 4 + 0] + _a[1] * _b[1 * 4 + 0] + _a[2] * _b[2 * 4 + 0] + _a[3] * _b[3 * 4 + 0];
    _out[1] = _a[0] * _b[0 * 4 + 1] + _a[1] * _b[1 * 4 + 1] + _a[2] * _b[2 * 4 + 1] + _a[3] * _b[3 * 4 + 1];
    _out[2] = _a[0] * _b[0 * 4 + 2] + _a[1] * _b[1 * 4 + 2] + _a[2] * _b[2 * 4 + 2] + _a[3] * _b[3 * 4 + 2];
    _out[3] = _a[0] * _b[0 * 4 + 3] + _a[1] * _b[1 * 4 + 3] + _a[2] * _b[2 * 4 + 3] + _a[3] * _b[3 * 4 + 3];
}
//////////////////////////////////////////////////////////////////////////
static void __mul_m4_m4( float * _out, const float * _a, const float * _b )
{
    __mul_v4_m4( _out + 0, _a + 0, _b );
    __mul_v4_m4( _out + 4, _a + 4, _b );
    __mul_v4_m4( _out + 8, _a + 8, _b );
    __mul_v4_m4( _out + 12, _a + 12, _b );
}
//////////////////////////////////////////////////////////////////////////
static void __mul_v3_v3_m4_r( float * _out, const float * _v, const float * _m )
{
    _out[0] = _m[0 * 4 + 0] * _v[0] + _m[1 * 4 + 0] * _v[1] + _m[2 * 4 + 0] * _v[2];
    _out[1] = _m[0 * 4 + 1] * _v[0] + _m[1 * 4 + 1] * _v[1] + _m[2 * 4 + 1] * _v[2];
    _out[2] = _m[0 * 4 + 2] * _v[0] + _m[1 * 4 + 2] * _v[1] + _m[2 * 4 + 2] * _v[2];
}
//////////////////////////////////////////////////////////////////////////
static void __copy_v3( float * _out, const float * _v )
{
    _out[0] = _v[0];
    _out[1] = _v[1];
    _out[2] = _v[2];
}
//////////////////////////////////////////////////////////////////////////
static void __v3_v3_m4( float * _out, const float * _v, const float * _m )
{
    __mul_v3_v3_m4_r( _out, _v, _m );

    _out[0] += _m[3 * 4 + 0];
    _out[1] += _m[3 * 4 + 1];
    _out[2] += _m[3 * 4 + 2];
}
//////////////////////////////////////////////////////////////////////////
static void __copy_m4( float * _out, const float * _in )
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

    *_out++ = *_in++;
    *_out++ = *_in++;
    *_out++ = *_in++;
    *_out++ = *_in++;
}
//////////////////////////////////////////////////////////////////////////
static uint32_t __make_argb( float _r, float _g, float _b, float _a )
{
    const float rgba_255 = 255.f;

    uint8_t a8 = (uint8_t)(_a * rgba_255);
    uint8_t b8 = (uint8_t)(_r * rgba_255);
    uint8_t g8 = (uint8_t)(_g * rgba_255);
    uint8_t r8 = (uint8_t)(_b * rgba_255);

    uint32_t argb = (a8 << 24) | (r8 << 16) | (g8 << 8) | (b8 << 0);

    return argb;
}
//////////////////////////////////////////////////////////////////////////
static void __minus_v2( float * _out, const float * _a, const float * _b )
{
    _out[0] = _a[0] - _b[0];
    _out[1] = _a[1] - _b[1];
}
//////////////////////////////////////////////////////////////////////////
static void __mul_v2_f( float * _out, float _v )
{
    _out[0] *= _v;
    _out[1] *= _v;
}
//////////////////////////////////////////////////////////////////////////
static void __calc_point_uv( float * _out, const float * _a, const float * _b, const float * _c, const float * _auv, const float * _buv, const float * _cuv, const float * _point )
{    
    float _dAB[2];
    __minus_v2( _dAB, _b, _a );
    
    float _dAC[2];
    __minus_v2( _dAC, _c, _a );

    float inv_v = 1.f / (_dAB[0] * _dAC[1] - _dAB[1] * _dAC[0]);
    __mul_v2_f( _dAB, inv_v );
    __mul_v2_f( _dAC, inv_v );

    float _dac[2];
    _dac[0] = _dAC[0] * _a[1] - _dAC[1] * _a[0];
    _dac[1] = _dAB[1] * _a[0] - _dAB[0] * _a[1];

    float _duvAB[2];
    __minus_v2( _duvAB, _buv, _auv );

    float _duvAC[2];
    __minus_v2( _duvAC, _cuv, _auv );

    float pv[3] = { 1.f, _point[0], _point[1] };
    float av[3] = { _dac[0], _dAC[1], -_dAC[0] };
    float bv[3] = { _dac[1], -_dAB[1], _dAB[0] };

    float a = __dot_v3( av, pv );
    float b = __dot_v3( bv, pv );

    float abv[3] = { 1.f, a, b };
    float uv[3] = { _auv[0], _duvAB[0], _duvAC[0] };
    float vv[3] = { _auv[1], _duvAB[1], _duvAC[1] };

    float u = __dot_v3( uv, abv );
    float v = __dot_v3( vv, abv );

    _out[0] = u;
    _out[1] = v;
}

#endif