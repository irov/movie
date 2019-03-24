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

#include "movie_transformation.h"

#include "movie_stream.h"
#include "movie_math.h"

//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __unhash_movie_layer_transformation_timeline( const aeMovieInstance * _instance, ae_uint32_t _iterator, ae_voidptr_t _timeline, ae_uint32_t _size )
{
    const ae_uint32_t * hashmask = _instance->hashmask;

    ae_uint32_t * it_timeline = (ae_uint32_t *)_timeline;
    ae_uint32_t * it_timeline_end = (ae_uint32_t *)_timeline + (_size >> 2);
    for( ; it_timeline != it_timeline_end; ++it_timeline )
    {
        ae_uint32_t hashmask_index = (_iterator++) % 5U;

        ae_uint32_t hash = hashmask[hashmask_index];

        *it_timeline ^= hash;
    }
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_constvoidptr_t __load_movie_layer_transformation_timeline( aeMovieStream * _stream, const ae_char_t * _doc )
{
    AE_UNUSED( _doc );

    ae_uint32_t size;
    AE_READ( _stream, size );

    ae_uint32_t hashmask_iterator = AE_READ8( _stream );

    ae_voidptr_t timeline = AE_NEWV( _stream->instance, size, _doc );

    AE_MOVIE_PANIC_MEMORY( timeline, AE_NULLPTR );

    AE_READV( _stream, timeline, (ae_size_t)size );

    if( _stream->instance->use_hash == AE_TRUE )
    {
        __unhash_movie_layer_transformation_timeline( _stream->instance, hashmask_iterator, timeline, size );
    }

    return timeline;
}
//////////////////////////////////////////////////////////////////////////
static const ae_float_t one_div_index256[] =
{0.f, 1.f / 1.f, 1.f / 2.f, 1.f / 3.f, 1.f / 4.f, 1.f / 5.f, 1.f / 6.f, 1.f / 7.f, 1.f / 8.f, 1.f / 9.f
, 1.f / 10.f, 1.f / 11.f, 1.f / 12.f, 1.f / 13.f, 1.f / 14.f, 1.f / 15.f, 1.f / 16.f, 1.f / 17.f, 1.f / 18.f, 1.f / 19.f
, 1.f / 20.f, 1.f / 21.f, 1.f / 22.f, 1.f / 23.f, 1.f / 24.f, 1.f / 25.f, 1.f / 26.f, 1.f / 27.f, 1.f / 28.f, 1.f / 29.f
, 1.f / 30.f, 1.f / 31.f, 1.f / 32.f, 1.f / 33.f, 1.f / 34.f, 1.f / 35.f, 1.f / 36.f, 1.f / 37.f, 1.f / 38.f, 1.f / 39.f
, 1.f / 40.f, 1.f / 41.f, 1.f / 42.f, 1.f / 43.f, 1.f / 44.f, 1.f / 45.f, 1.f / 46.f, 1.f / 47.f, 1.f / 48.f, 1.f / 49.f
, 1.f / 50.f, 1.f / 51.f, 1.f / 52.f, 1.f / 53.f, 1.f / 54.f, 1.f / 55.f, 1.f / 56.f, 1.f / 57.f, 1.f / 58.f, 1.f / 59.f
, 1.f / 60.f, 1.f / 61.f, 1.f / 62.f, 1.f / 63.f, 1.f / 64.f, 1.f / 65.f, 1.f / 66.f, 1.f / 67.f, 1.f / 68.f, 1.f / 69.f
, 1.f / 70.f, 1.f / 71.f, 1.f / 72.f, 1.f / 73.f, 1.f / 74.f, 1.f / 75.f, 1.f / 76.f, 1.f / 77.f, 1.f / 78.f, 1.f / 79.f
, 1.f / 80.f, 1.f / 81.f, 1.f / 82.f, 1.f / 83.f, 1.f / 84.f, 1.f / 85.f, 1.f / 86.f, 1.f / 87.f, 1.f / 88.f, 1.f / 89.f
, 1.f / 90.f, 1.f / 91.f, 1.f / 92.f, 1.f / 93.f, 1.f / 94.f, 1.f / 95.f, 1.f / 96.f, 1.f / 97.f, 1.f / 98.f, 1.f / 99.f
, 1.f / 100.f, 1.f / 101.f, 1.f / 102.f, 1.f / 103.f, 1.f / 104, 1.f / 105.f, 1.f / 106.f, 1.f / 107.f, 1.f / 108.f, 1.f / 109.f
, 1.f / 110.f, 1.f / 111.f, 1.f / 112.f, 1.f / 113.f, 1.f / 114, 1.f / 115.f, 1.f / 116.f, 1.f / 117.f, 1.f / 118.f, 1.f / 119.f
, 1.f / 120.f, 1.f / 121.f, 1.f / 122.f, 1.f / 123.f, 1.f / 124, 1.f / 125.f, 1.f / 126.f, 1.f / 127.f, 1.f / 128.f, 1.f / 129.f
, 1.f / 130.f, 1.f / 131.f, 1.f / 132.f, 1.f / 133.f, 1.f / 134, 1.f / 135.f, 1.f / 136.f, 1.f / 137.f, 1.f / 138.f, 1.f / 139.f
, 1.f / 140.f, 1.f / 141.f, 1.f / 142.f, 1.f / 143.f, 1.f / 144, 1.f / 145.f, 1.f / 146.f, 1.f / 147.f, 1.f / 148.f, 1.f / 149.f
, 1.f / 150.f, 1.f / 151.f, 1.f / 152.f, 1.f / 153.f, 1.f / 154, 1.f / 155.f, 1.f / 156.f, 1.f / 157.f, 1.f / 158.f, 1.f / 159.f
, 1.f / 160.f, 1.f / 161.f, 1.f / 162.f, 1.f / 163.f, 1.f / 164, 1.f / 165.f, 1.f / 166.f, 1.f / 167.f, 1.f / 168.f, 1.f / 169.f
, 1.f / 170.f, 1.f / 171.f, 1.f / 172.f, 1.f / 173.f, 1.f / 174, 1.f / 175.f, 1.f / 176.f, 1.f / 177.f, 1.f / 178.f, 1.f / 179.f
, 1.f / 180.f, 1.f / 181.f, 1.f / 182.f, 1.f / 183.f, 1.f / 184, 1.f / 185.f, 1.f / 186.f, 1.f / 187.f, 1.f / 188.f, 1.f / 189.f
, 1.f / 190.f, 1.f / 191.f, 1.f / 192.f, 1.f / 193.f, 1.f / 194, 1.f / 195.f, 1.f / 196.f, 1.f / 197.f, 1.f / 198.f, 1.f / 199.f
, 1.f / 200.f, 1.f / 201.f, 1.f / 202.f, 1.f / 203.f, 1.f / 204, 1.f / 205.f, 1.f / 206.f, 1.f / 207.f, 1.f / 208.f, 1.f / 209.f
, 1.f / 210.f, 1.f / 211.f, 1.f / 212.f, 1.f / 213.f, 1.f / 214, 1.f / 215.f, 1.f / 216.f, 1.f / 217.f, 1.f / 218.f, 1.f / 219.f
, 1.f / 220.f, 1.f / 221.f, 1.f / 222.f, 1.f / 223.f, 1.f / 224, 1.f / 225.f, 1.f / 226.f, 1.f / 227.f, 1.f / 228.f, 1.f / 229.f
, 1.f / 230.f, 1.f / 231.f, 1.f / 232.f, 1.f / 233.f, 1.f / 234, 1.f / 235.f, 1.f / 236.f, 1.f / 237.f, 1.f / 238.f, 1.f / 239.f
, 1.f / 240.f, 1.f / 241.f, 1.f / 242.f, 1.f / 243.f, 1.f / 244, 1.f / 245.f, 1.f / 246.f, 1.f / 247.f, 1.f / 248.f, 1.f / 249.f
, 1.f / 250.f, 1.f / 251.f, 1.f / 252.f, 1.f / 253.f, 1.f / 254, 1.f / 255.f, 1.f / 256.f, 1.f / 257.f, 1.f / 258.f, 1.f / 259.f};
//////////////////////////////////////////////////////////////////////////
static const ae_float_t index256_to_float[] =
{0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f
, 10.f, 11.f, 12.f, 13.f, 14.f, 15.f, 16.f, 17.f, 18.f, 19.f
, 20.f, 21.f, 22.f, 23.f, 24.f, 25.f, 26.f, 27.f, 28.f, 29.f
, 30.f, 31.f, 32.f, 33.f, 34.f, 35.f, 36.f, 37.f, 38.f, 39.f
, 40.f, 41.f, 42.f, 43.f, 44.f, 45.f, 46.f, 47.f, 48.f, 49.f
, 50.f, 51.f, 52.f, 53.f, 54.f, 55.f, 56.f, 57.f, 58.f, 59.f
, 60.f, 61.f, 62.f, 63.f, 64.f, 65.f, 66.f, 67.f, 68.f, 69.f
, 70.f, 71.f, 72.f, 73.f, 74.f, 75.f, 76.f, 77.f, 78.f, 79.f
, 80.f, 81.f, 82.f, 83.f, 84.f, 85.f, 86.f, 87.f, 88.f, 89.f
, 90.f, 91.f, 92.f, 93.f, 94.f, 95.f, 96.f, 97.f, 98.f, 99.f
, 100.f, 101.f, 102.f, 103.f, 104.f, 105.f, 106.f, 107.f, 108.f, 109.f
, 110.f, 111.f, 112.f, 113.f, 114.f, 115.f, 116.f, 117.f, 118.f, 119.f
, 120.f, 121.f, 122.f, 123.f, 124.f, 125.f, 126.f, 127.f, 128.f, 129.f
, 130.f, 131.f, 132.f, 133.f, 134.f, 135.f, 136.f, 137.f, 138.f, 139.f
, 140.f, 141.f, 142.f, 143.f, 144.f, 145.f, 146.f, 147.f, 148.f, 149.f
, 150.f, 151.f, 152.f, 153.f, 154.f, 155.f, 156.f, 157.f, 158.f, 159.f
, 160.f, 161.f, 162.f, 163.f, 164.f, 165.f, 166.f, 167.f, 168.f, 169.f
, 170.f, 171.f, 172.f, 173.f, 174.f, 175.f, 176.f, 177.f, 178.f, 179.f
, 180.f, 181.f, 182.f, 183.f, 184.f, 185.f, 186.f, 187.f, 188.f, 189.f
, 190.f, 191.f, 192.f, 193.f, 194.f, 195.f, 196.f, 197.f, 198.f, 199.f
, 200.f, 201.f, 202.f, 203.f, 204.f, 205.f, 206.f, 207.f, 208.f, 209.f
, 210.f, 211.f, 212.f, 213.f, 214.f, 215.f, 216.f, 217.f, 218.f, 219.f
, 220.f, 221.f, 222.f, 223.f, 224.f, 225.f, 226.f, 227.f, 228.f, 229.f
, 230.f, 231.f, 232.f, 233.f, 234.f, 235.f, 236.f, 237.f, 238.f, 239.f
, 240.f, 241.f, 242.f, 243.f, 244.f, 245.f, 246.f, 247.f, 248.f, 249.f
, 250.f, 251.f, 252.f, 253.f, 254.f, 255.f, 256.f, 257.f, 258.f, 259.f};
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_float_t __get_movie_layer_transformation_property_fixed( ae_constvoidptr_t _property, ae_uint32_t _index )
{
    ae_uint32_t property_block_offset[4] = {1U, 3U, 2U, 0U};

    ae_uint32_t property_index = 0U;

    const ae_uint32_t * property_ae_uint32_t = (const ae_uint32_t *)_property;

    ae_uint32_t zp_block_type_count_data = *(property_ae_uint32_t++);

    ae_uint32_t zp_block_type = zp_block_type_count_data >> 24U;
    ae_uint32_t zp_block_count = zp_block_type_count_data & 0x00FFFFFF;

    while( property_index + zp_block_count <= _index )
    {
        property_block_offset[3] = zp_block_count;

        property_ae_uint32_t += property_block_offset[zp_block_type];

        property_index += zp_block_count;

        ae_uint32_t zp_block_type_count_data_next = *(property_ae_uint32_t++);

        zp_block_type = zp_block_type_count_data_next >> 24U;
        zp_block_count = zp_block_type_count_data_next & 0x00FFFFFF;
    }

    const ae_float_t * property_ae_float_t = (const ae_float_t *)(ae_constvoidptr_t)(property_ae_uint32_t);

    switch( zp_block_type )
    {
    case 0:
        {
            ae_float_t block_value = property_ae_float_t[0];

            return block_value;
        }break;
    case 1:
        {
            ae_float_t block_inv = property_ae_float_t[0];
            ae_float_t block_begin = property_ae_float_t[1];
            ae_float_t block_end = property_ae_float_t[2];

            ae_uint32_t block_index = _index - property_index;
            ae_float_t block_index_f = (ae_float_t)block_index;

            ae_float_t block_t = block_index_f * block_inv;

            ae_float_t block_value = block_begin + (block_end - block_begin) * block_t;

            return block_value;
        }break;
    case 2:
        {
            const ae_float_t block_inv = one_div_index256[zp_block_count - 1];
            ae_float_t block_begin = property_ae_float_t[0];
            ae_float_t block_end = property_ae_float_t[1];

            ae_uint32_t block_index = _index - property_index;
            const ae_float_t block_index_f = index256_to_float[block_index];

            ae_float_t block_t = block_index_f * block_inv;

            ae_float_t block_value = block_begin + (block_end - block_begin) * block_t;

            return block_value;
        }break;
    case 3:
        {
            ae_uint32_t block_index = _index - property_index;

            ae_float_t block_value = property_ae_float_t[block_index];

            return block_value;
        }break;
    default:
        {
            //Error
        }break;
    }

#ifdef AE_MOVIE_DEBUG
    __movie_break_point();
#endif

    return 0.f;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_float_t __get_movie_layer_transformation_property_initial( ae_constvoidptr_t _property )
{
    const ae_uint32_t * property_ae_uint32_t = (const ae_uint32_t *)_property;

    ae_uint32_t zp_block_type_count_data = *(property_ae_uint32_t++);

    ae_uint32_t zp_block_type = zp_block_type_count_data >> 24U;

    const ae_float_t * property_ae_float_t = (const ae_float_t *)(ae_constvoidptr_t)(property_ae_uint32_t);

    switch( zp_block_type )
    {
    case 0:
        {
            ae_float_t block_value = property_ae_float_t[0];

            return block_value;
        }break;
    case 1:
        {
            ae_float_t block_begin = property_ae_float_t[1];
                        
            ae_float_t block_value = block_begin;

            return block_value;
        }break;
    case 2:
        {
            ae_float_t block_begin = property_ae_float_t[0];

            ae_float_t block_value = block_begin;

            return block_value;
        }break;
    case 3:
        {
            ae_float_t block_value = property_ae_float_t[0];

            return block_value;
        }break;
    default:
        {
            //Error
        }break;
    }

#ifdef AE_MOVIE_DEBUG
    __movie_break_point();
#endif

    return 0.f;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_float_t __get_movie_layer_transformation_property_interpolate( ae_constvoidptr_t _property, ae_uint32_t _index, ae_float_t _t )
{
    ae_float_t data_0 = __get_movie_layer_transformation_property_fixed( _property, _index + 0 );
    ae_float_t data_1 = __get_movie_layer_transformation_property_fixed( _property, _index + 1 );

    ae_float_t data = ae_linerp_f1( data_0, data_1, _t );

    return data;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __make_movie_layer_transformation2d_immutable( ae_matrix34_t _out, const aeMovieLayerTransformation2D * _transformation )
{
    ae_vector2_t anchor_point;
    anchor_point[0] = _transformation->immutable.anchor_point_x;
    anchor_point[1] = _transformation->immutable.anchor_point_y;

    ae_vector2_t position;
    position[0] = _transformation->immutable.position_x;
    position[1] = _transformation->immutable.position_y;

    ae_vector2_t scale;
    scale[0] = _transformation->immutable.scale_x;
    scale[1] = _transformation->immutable.scale_y;

    ae_quaternionzw_t quaternionzw;
    quaternionzw[0] = _transformation->immutable.quaternion_z;
    quaternionzw[1] = _transformation->immutable.quaternion_w;

    ae_skew_t skew;
    skew[0] = _transformation->immutable.skew;
    skew[1] = _transformation->immutable.skew_quaternion_z;
    skew[2] = _transformation->immutable.skew_quaternion_w;

    ae_movie_make_transformation2d_m34( _out, position, anchor_point, scale, quaternionzw, skew );
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __make_movie_layer_transformation3d_immutable( ae_matrix34_t _out, const aeMovieLayerTransformation3D * _transformation )
{
    ae_vector3_t anchor_point;
    anchor_point[0] = _transformation->immutable.anchor_point_x;
    anchor_point[1] = _transformation->immutable.anchor_point_y;
    anchor_point[2] = _transformation->immutable.anchor_point_z;

    ae_vector3_t position;
    position[0] = _transformation->immutable.position_x;
    position[1] = _transformation->immutable.position_y;
    position[2] = _transformation->immutable.position_z;

    ae_vector3_t scale;
    scale[0] = _transformation->immutable.scale_x;
    scale[1] = _transformation->immutable.scale_y;
    scale[2] = _transformation->immutable.scale_z;

    ae_quaternion_t quaternion;
    quaternion[0] = _transformation->immutable.quaternion_x;
    quaternion[1] = _transformation->immutable.quaternion_y;
    quaternion[2] = _transformation->immutable.quaternion_z;
    quaternion[3] = _transformation->immutable.quaternion_w;

    ae_skew_t skew;
    skew[0] = _transformation->immutable.skew;
    skew[1] = _transformation->immutable.skew_quaternion_z;
    skew[2] = _transformation->immutable.skew_quaternion_w;

    ae_movie_make_transformation3d_m34( _out, position, anchor_point, scale, quaternion, skew );
}
//////////////////////////////////////////////////////////////////////////
#define AE_STREAM_PROPERTY(Mask, Name, Default)\
    if( identity_property_mask & Mask )\
    {\
        _transformation->immutable.Name = Default;\
        if( _transformation->timeline != AE_NULLPTR ) {_transformation->timeline->Name = AE_NULLPTR;}\
    }\
	else if( immutable_property_mask & Mask )\
	{\
		AE_READ( _stream, _transformation->immutable.Name );\
		if( _transformation->timeline != AE_NULLPTR ) {_transformation->timeline->Name = AE_NULLPTR;}\
	}\
	else\
	{\
		_transformation->immutable.Name = 0.f;\
		_transformation->timeline->Name = __load_movie_layer_transformation_timeline(_stream, #Name);\
        AE_RESULT_PANIC_MEMORY(_transformation->timeline->Name);\
	}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __load_movie_layer_transformation2d( aeMovieStream * _stream, aeMovieLayerTransformation2D * _transformation )
{
    ae_uint32_t immutable_property_mask = _transformation->immutable_property_mask;
    ae_uint32_t identity_property_mask = _transformation->identity_property_mask;

    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_ANCHOR_POINT_X, anchor_point_x, 0.f );
    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_ANCHOR_POINT_Y, anchor_point_y, 0.f );

    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_POSITION_X, position_x, 0.f );
    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_POSITION_Y, position_y, 0.f );

    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_SCALE_X, scale_x, 1.f );
    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_SCALE_Y, scale_y, 1.f );

    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_QUATERNION_Z, quaternion_z, 0.f );
    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_QUATERNION_W, quaternion_w, 1.f );

    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_SKEW, skew, 0.f );
    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_SKEW_QUATERNION_Z, skew_quaternion_z, 0.f );
    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_SKEW_QUATERNION_W, skew_quaternion_w, 1.f );

    if( (_transformation->immutable_property_mask & AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL) == AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL )
    {
        ae_matrix34_t * immutable_matrix = AE_NEW( _stream->instance, ae_matrix34_t );

        AE_MOVIE_PANIC_MEMORY( immutable_matrix, AE_RESULT_INVALID_MEMORY );

        __make_movie_layer_transformation2d_immutable( *immutable_matrix, _transformation );

        _transformation->immutable_matrix = immutable_matrix;
    }
    else
    {
        _transformation->immutable_matrix = AE_NULLPTR;
    }

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __load_movie_layer_transformation3d( aeMovieStream * _stream, aeMovieLayerTransformation3D * _transformation )
{
    ae_uint32_t immutable_property_mask = _transformation->immutable_property_mask;
    ae_uint32_t identity_property_mask = _transformation->identity_property_mask;

    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_ANCHOR_POINT_X, anchor_point_x, 0.f );
    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_ANCHOR_POINT_Y, anchor_point_y, 0.f );
    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_ANCHOR_POINT_Z, anchor_point_z, 0.f );

    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_POSITION_X, position_x, 0.f );
    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_POSITION_Y, position_y, 0.f );
    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_POSITION_Z, position_z, 0.f );

    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_SCALE_X, scale_x, 1.f );
    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_SCALE_Y, scale_y, 1.f );
    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_SCALE_Z, scale_z, 1.f );

    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_QUATERNION_X, quaternion_x, 0.f );
    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_QUATERNION_Y, quaternion_y, 0.f );
    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_QUATERNION_Z, quaternion_z, 0.f );
    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_QUATERNION_W, quaternion_w, 1.f );

    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_SKEW, skew, 0.f );
    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_SKEW_QUATERNION_Z, skew_quaternion_z, 0.f );
    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_SKEW_QUATERNION_W, skew_quaternion_w, 1.f );

    if( (_transformation->immutable_property_mask & AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL) == AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL )
    {
        ae_matrix34_t * immutable_matrix = AE_NEW( _stream->instance, ae_matrix34_t );

        AE_MOVIE_PANIC_MEMORY( immutable_matrix, AE_RESULT_INVALID_MEMORY );

        __make_movie_layer_transformation3d_immutable( *immutable_matrix, _transformation );

        _transformation->immutable_matrix = immutable_matrix;
    }
    else
    {
        _transformation->immutable_matrix = AE_NULLPTR;
    }

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
AE_CALLBACK ae_void_t __make_layer_transformation_interpolate_identity( ae_matrix34_t _out, const aeMovieLayerTransformation * _transformation, ae_uint32_t _index, ae_float_t _t )
{
    AE_UNUSED( _out );
    AE_UNUSED( _transformation );
    AE_UNUSED( _index );
    AE_UNUSED( _t );
}
//////////////////////////////////////////////////////////////////////////
AE_CALLBACK ae_void_t __make_layer_transformation_fixed_identity( ae_matrix34_t _out, const aeMovieLayerTransformation * _transformation, ae_uint32_t _index )
{
    AE_UNUSED( _out );
    AE_UNUSED( _transformation );
    AE_UNUSED( _index );
}
//////////////////////////////////////////////////////////////////////////
AE_CALLBACK ae_void_t __make_layer_transformation_interpolate_immutable( ae_matrix34_t _out, const aeMovieLayerTransformation * _transformation, ae_uint32_t _index, ae_float_t _t )
{
    AE_UNUSED( _index );
    AE_UNUSED( _t );

    ae_copy_m34( _out, *_transformation->immutable_matrix );
}
//////////////////////////////////////////////////////////////////////////
AE_CALLBACK ae_void_t __make_layer_transformation_fixed_immutable( ae_matrix34_t _out, const aeMovieLayerTransformation * _transformation, ae_uint32_t _index )
{
    AE_UNUSED( _index );

    ae_copy_m34( _out, *_transformation->immutable_matrix );
}
//////////////////////////////////////////////////////////////////////////
#define AE_INTERPOLATE_PROPERTY( Transformation, Name, OutName )\
	OutName = (Transformation->timeline == AE_NULLPTR || Transformation->timeline->Name == AE_NULLPTR) ? Transformation->immutable.Name : __get_movie_layer_transformation_property_interpolate(\
		Transformation->timeline->Name,\
		_index, _t )
//////////////////////////////////////////////////////////////////////////
#define AE_FIXED_PROPERTY( Transformation, Name, Index, OutName)\
	OutName = (Transformation->timeline == AE_NULLPTR || Transformation->timeline->Name == AE_NULLPTR) ? Transformation->immutable.Name : __get_movie_layer_transformation_property_fixed(\
		Transformation->timeline->Name,\
		_index + Index )
//////////////////////////////////////////////////////////////////////////
AE_CALLBACK ae_void_t __make_layer_transformation2d_interpolate( ae_matrix34_t _out, const aeMovieLayerTransformation * _transformation, ae_uint32_t _index, ae_float_t _t )
{
    const aeMovieLayerTransformation2D * transformation2d = (const aeMovieLayerTransformation2D *)_transformation;

    ae_vector2_t anchor_point;
    ae_vector2_t position;
    ae_vector2_t scale;
    ae_quaternionzw_t quaternion;
    ae_skew_t skew;

    AE_INTERPOLATE_PROPERTY( transformation2d, anchor_point_x, anchor_point[0] );
    AE_INTERPOLATE_PROPERTY( transformation2d, anchor_point_y, anchor_point[1] );

    AE_INTERPOLATE_PROPERTY( transformation2d, position_x, position[0] );
    AE_INTERPOLATE_PROPERTY( transformation2d, position_y, position[1] );

    AE_INTERPOLATE_PROPERTY( transformation2d, scale_x, scale[0] );
    AE_INTERPOLATE_PROPERTY( transformation2d, scale_y, scale[1] );

    ae_quaternionzw_t q1;
    AE_FIXED_PROPERTY( transformation2d, quaternion_z, 0, q1[0] );
    AE_FIXED_PROPERTY( transformation2d, quaternion_w, 0, q1[1] );

    ae_quaternionzw_t q2;
    AE_FIXED_PROPERTY( transformation2d, quaternion_z, 1, q2[0] );
    AE_FIXED_PROPERTY( transformation2d, quaternion_w, 1, q2[1] );

    ae_linerp_qzw( quaternion, q1, q2, _t );

    AE_INTERPOLATE_PROPERTY( transformation2d, skew, skew[0] );
    AE_INTERPOLATE_PROPERTY( transformation2d, skew_quaternion_z, skew[1] );
    AE_INTERPOLATE_PROPERTY( transformation2d, skew_quaternion_w, skew[2] );

    ae_movie_make_transformation2d_m34( _out, position, anchor_point, scale, quaternion, skew );
}
//////////////////////////////////////////////////////////////////////////
AE_CALLBACK ae_void_t __make_layer_transformation2d_interpolate_fq( ae_matrix34_t _out, const aeMovieLayerTransformation * _transformation, ae_uint32_t _index, ae_float_t _t )
{
    const aeMovieLayerTransformation2D * transformation2d = (const aeMovieLayerTransformation2D *)_transformation;

    ae_vector2_t anchor_point;
    ae_vector2_t position;
    ae_vector2_t scale;
    ae_quaternionzw_t quaternion;
    ae_skew_t skew;

    AE_INTERPOLATE_PROPERTY( transformation2d, anchor_point_x, anchor_point[0] );
    AE_INTERPOLATE_PROPERTY( transformation2d, anchor_point_y, anchor_point[1] );

    AE_INTERPOLATE_PROPERTY( transformation2d, position_x, position[0] );
    AE_INTERPOLATE_PROPERTY( transformation2d, position_y, position[1] );

    AE_INTERPOLATE_PROPERTY( transformation2d, scale_x, scale[0] );
    AE_INTERPOLATE_PROPERTY( transformation2d, scale_y, scale[1] );

    AE_FIXED_PROPERTY( transformation2d, quaternion_z, 0, quaternion[0] );
    AE_FIXED_PROPERTY( transformation2d, quaternion_w, 0, quaternion[1] );

    AE_INTERPOLATE_PROPERTY( transformation2d, skew, skew[0] );
    AE_INTERPOLATE_PROPERTY( transformation2d, skew_quaternion_z, skew[1] );
    AE_INTERPOLATE_PROPERTY( transformation2d, skew_quaternion_w, skew[2] );

    ae_movie_make_transformation2d_m34( _out, position, anchor_point, scale, quaternion, skew );
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __make_layer_transformation2d_interpolate_wskfq( ae_matrix34_t _out, const aeMovieLayerTransformation * _transformation, ae_uint32_t _index, ae_float_t _t )
{
    const aeMovieLayerTransformation2D * transformation2d = (const aeMovieLayerTransformation2D *)_transformation;

    ae_vector2_t anchor_point;
    ae_vector2_t position;
    ae_vector2_t scale;
    ae_quaternionzw_t quaternion;

    AE_INTERPOLATE_PROPERTY( transformation2d, anchor_point_x, anchor_point[0] );
    AE_INTERPOLATE_PROPERTY( transformation2d, anchor_point_y, anchor_point[1] );

    AE_INTERPOLATE_PROPERTY( transformation2d, position_x, position[0] );
    AE_INTERPOLATE_PROPERTY( transformation2d, position_y, position[1] );

    AE_INTERPOLATE_PROPERTY( transformation2d, scale_x, scale[0] );
    AE_INTERPOLATE_PROPERTY( transformation2d, scale_y, scale[1] );

    AE_FIXED_PROPERTY( transformation2d, quaternion_z, 0, quaternion[0] );
    AE_FIXED_PROPERTY( transformation2d, quaternion_w, 0, quaternion[1] );

    ae_movie_make_transformation2d_m34wsk( _out, position, anchor_point, scale, quaternion );
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __make_layer_transformation2d_fixed( ae_matrix34_t _out, const aeMovieLayerTransformation * _transformation, ae_uint32_t _index )
{
    const aeMovieLayerTransformation2D * transformation2d = (const aeMovieLayerTransformation2D *)_transformation;

    ae_vector2_t anchor_point;
    ae_vector2_t position;
    ae_vector2_t scale;
    ae_quaternionzw_t quaternion;
    ae_skew_t skew;

    AE_FIXED_PROPERTY( transformation2d, anchor_point_x, 0, anchor_point[0] );
    AE_FIXED_PROPERTY( transformation2d, anchor_point_y, 0, anchor_point[1] );

    AE_FIXED_PROPERTY( transformation2d, position_x, 0, position[0] );
    AE_FIXED_PROPERTY( transformation2d, position_y, 0, position[1] );

    AE_FIXED_PROPERTY( transformation2d, scale_x, 0, scale[0] );
    AE_FIXED_PROPERTY( transformation2d, scale_y, 0, scale[1] );

    AE_FIXED_PROPERTY( transformation2d, quaternion_z, 0, quaternion[0] );
    AE_FIXED_PROPERTY( transformation2d, quaternion_w, 0, quaternion[1] );

    AE_FIXED_PROPERTY( transformation2d, skew, 0, skew[0] );
    AE_FIXED_PROPERTY( transformation2d, skew_quaternion_z, 0, skew[1] );
    AE_FIXED_PROPERTY( transformation2d, skew_quaternion_w, 0, skew[2] );

    ae_movie_make_transformation2d_m34( _out, position, anchor_point, scale, quaternion, skew );
}
//////////////////////////////////////////////////////////////////////////
AE_CALLBACK ae_void_t __make_layer_transformation2d_interpolate_wq( ae_matrix34_t _out, const aeMovieLayerTransformation * _transformation, ae_uint32_t _index, ae_float_t _t )
{
    const aeMovieLayerTransformation2D * transformation2d = (const aeMovieLayerTransformation2D *)_transformation;

    ae_vector2_t anchor_point;
    ae_vector2_t position;
    ae_vector2_t scale;
    ae_skew_t skew;

    AE_INTERPOLATE_PROPERTY( transformation2d, anchor_point_x, anchor_point[0] );
    AE_INTERPOLATE_PROPERTY( transformation2d, anchor_point_y, anchor_point[1] );

    AE_INTERPOLATE_PROPERTY( transformation2d, position_x, position[0] );
    AE_INTERPOLATE_PROPERTY( transformation2d, position_y, position[1] );

    AE_INTERPOLATE_PROPERTY( transformation2d, scale_x, scale[0] );
    AE_INTERPOLATE_PROPERTY( transformation2d, scale_y, scale[1] );

    AE_INTERPOLATE_PROPERTY( transformation2d, skew, skew[0] );
    AE_INTERPOLATE_PROPERTY( transformation2d, skew_quaternion_z, skew[1] );
    AE_INTERPOLATE_PROPERTY( transformation2d, skew_quaternion_w, skew[2] );

    ae_movie_make_transformation2d_m34wq( _out, position, anchor_point, scale, skew );
}
//////////////////////////////////////////////////////////////////////////
AE_CALLBACK ae_void_t __make_layer_transformation2d_fixed_wq( ae_matrix34_t _out, const aeMovieLayerTransformation * _transformation, ae_uint32_t _index )
{
    const aeMovieLayerTransformation2D * transformation2d = (const aeMovieLayerTransformation2D *)_transformation;

    ae_vector2_t anchor_point;
    ae_vector2_t position;
    ae_vector2_t scale;
    ae_skew_t skew;

    AE_FIXED_PROPERTY( transformation2d, anchor_point_x, 0, anchor_point[0] );
    AE_FIXED_PROPERTY( transformation2d, anchor_point_y, 0, anchor_point[1] );

    AE_FIXED_PROPERTY( transformation2d, position_x, 0, position[0] );
    AE_FIXED_PROPERTY( transformation2d, position_y, 0, position[1] );

    AE_FIXED_PROPERTY( transformation2d, scale_x, 0, scale[0] );
    AE_FIXED_PROPERTY( transformation2d, scale_y, 0, scale[1] );

    AE_FIXED_PROPERTY( transformation2d, skew, 0, skew[0] );
    AE_FIXED_PROPERTY( transformation2d, skew_quaternion_z, 0, skew[1] );
    AE_FIXED_PROPERTY( transformation2d, skew_quaternion_w, 0, skew[2] );

    ae_movie_make_transformation2d_m34wq( _out, position, anchor_point, scale, skew );
}
//////////////////////////////////////////////////////////////////////////
AE_CALLBACK ae_void_t __make_layer_transformation2d_interpolate_wskq( ae_matrix34_t _out, const aeMovieLayerTransformation * _transformation, ae_uint32_t _index, ae_float_t _t )
{
    const aeMovieLayerTransformation2D * transformation2d = (const aeMovieLayerTransformation2D *)_transformation;

    ae_vector2_t anchor_point;
    ae_vector2_t position;
    ae_vector2_t scale;

    AE_INTERPOLATE_PROPERTY( transformation2d, anchor_point_x, anchor_point[0] );
    AE_INTERPOLATE_PROPERTY( transformation2d, anchor_point_y, anchor_point[1] );

    AE_INTERPOLATE_PROPERTY( transformation2d, position_x, position[0] );
    AE_INTERPOLATE_PROPERTY( transformation2d, position_y, position[1] );

    AE_INTERPOLATE_PROPERTY( transformation2d, scale_x, scale[0] );
    AE_INTERPOLATE_PROPERTY( transformation2d, scale_y, scale[1] );

    ae_movie_make_transformation2d_m34wskq( _out, position, anchor_point, scale );
}
//////////////////////////////////////////////////////////////////////////
AE_CALLBACK ae_void_t __make_layer_transformation2d_fixed_wskq( ae_matrix34_t _out, const aeMovieLayerTransformation * _transformation, ae_uint32_t _index )
{
    const aeMovieLayerTransformation2D * transformation2d = (const aeMovieLayerTransformation2D *)_transformation;

    ae_vector2_t anchor_point;
    ae_vector2_t position;
    ae_vector2_t scale;

    AE_FIXED_PROPERTY( transformation2d, anchor_point_x, 0, anchor_point[0] );
    AE_FIXED_PROPERTY( transformation2d, anchor_point_y, 0, anchor_point[1] );

    AE_FIXED_PROPERTY( transformation2d, position_x, 0, position[0] );
    AE_FIXED_PROPERTY( transformation2d, position_y, 0, position[1] );

    AE_FIXED_PROPERTY( transformation2d, scale_x, 0, scale[0] );
    AE_FIXED_PROPERTY( transformation2d, scale_y, 0, scale[1] );

    ae_movie_make_transformation2d_m34wskq( _out, position, anchor_point, scale );
}
//////////////////////////////////////////////////////////////////////////
AE_CALLBACK ae_void_t __make_layer_transformation2d_interpolate_wsk( ae_matrix34_t _out, const aeMovieLayerTransformation * _transformation, ae_uint32_t _index, ae_float_t _t )
{
    const aeMovieLayerTransformation2D * transformation2d = (const aeMovieLayerTransformation2D *)_transformation;

    ae_vector2_t anchor_point;
    ae_vector2_t position;
    ae_vector2_t scale;
    ae_quaternionzw_t quaternion;

    AE_INTERPOLATE_PROPERTY( transformation2d, anchor_point_x, anchor_point[0] );
    AE_INTERPOLATE_PROPERTY( transformation2d, anchor_point_y, anchor_point[1] );

    AE_INTERPOLATE_PROPERTY( transformation2d, position_x, position[0] );
    AE_INTERPOLATE_PROPERTY( transformation2d, position_y, position[1] );

    AE_INTERPOLATE_PROPERTY( transformation2d, scale_x, scale[0] );
    AE_INTERPOLATE_PROPERTY( transformation2d, scale_y, scale[1] );

    ae_quaternionzw_t q1;
    AE_FIXED_PROPERTY( transformation2d, quaternion_z, 0, q1[0] );
    AE_FIXED_PROPERTY( transformation2d, quaternion_w, 0, q1[1] );

    ae_quaternionzw_t q2;
    AE_FIXED_PROPERTY( transformation2d, quaternion_z, 1, q2[0] );
    AE_FIXED_PROPERTY( transformation2d, quaternion_w, 1, q2[1] );

    ae_linerp_qzw( quaternion, q1, q2, _t );

    ae_movie_make_transformation2d_m34wsk( _out, position, anchor_point, scale, quaternion );
}
//////////////////////////////////////////////////////////////////////////
AE_CALLBACK ae_void_t __make_layer_transformation2d_fixed_wsk( ae_matrix34_t _out, const aeMovieLayerTransformation * _transformation, ae_uint32_t _index )
{
    const aeMovieLayerTransformation2D * transformation2d = (const aeMovieLayerTransformation2D *)_transformation;

    ae_vector2_t anchor_point;
    ae_vector2_t position;
    ae_vector2_t scale;
    ae_quaternionzw_t quaternion;

    AE_FIXED_PROPERTY( transformation2d, anchor_point_x, 0, anchor_point[0] );
    AE_FIXED_PROPERTY( transformation2d, anchor_point_y, 0, anchor_point[1] );

    AE_FIXED_PROPERTY( transformation2d, position_x, 0, position[0] );
    AE_FIXED_PROPERTY( transformation2d, position_y, 0, position[1] );

    AE_FIXED_PROPERTY( transformation2d, scale_x, 0, scale[0] );
    AE_FIXED_PROPERTY( transformation2d, scale_y, 0, scale[1] );

    AE_FIXED_PROPERTY( transformation2d, quaternion_z, 0, quaternion[0] );
    AE_FIXED_PROPERTY( transformation2d, quaternion_w, 0, quaternion[1] );

    ae_movie_make_transformation2d_m34wsk( _out, position, anchor_point, scale, quaternion );
}
//////////////////////////////////////////////////////////////////////////
AE_CALLBACK ae_void_t __make_layer_transformation3d_fixed( ae_matrix34_t _out, const aeMovieLayerTransformation * _transformation, ae_uint32_t _index )
{
    const aeMovieLayerTransformation3D * transformation3d = (const aeMovieLayerTransformation3D *)_transformation;

    ae_vector3_t anchor_point;
    ae_vector3_t position;
    ae_vector3_t scale;
    ae_quaternion_t quaternion;
    ae_skew_t skew;

    AE_FIXED_PROPERTY( transformation3d, anchor_point_x, 0, anchor_point[0] );
    AE_FIXED_PROPERTY( transformation3d, anchor_point_y, 0, anchor_point[1] );
    AE_FIXED_PROPERTY( transformation3d, anchor_point_z, 0, anchor_point[2] );

    AE_FIXED_PROPERTY( transformation3d, position_x, 0, position[0] );
    AE_FIXED_PROPERTY( transformation3d, position_y, 0, position[1] );
    AE_FIXED_PROPERTY( transformation3d, position_z, 0, position[2] );

    AE_FIXED_PROPERTY( transformation3d, scale_x, 0, scale[0] );
    AE_FIXED_PROPERTY( transformation3d, scale_y, 0, scale[1] );
    AE_FIXED_PROPERTY( transformation3d, scale_z, 0, scale[2] );

    AE_FIXED_PROPERTY( transformation3d, quaternion_x, 0, quaternion[0] );
    AE_FIXED_PROPERTY( transformation3d, quaternion_y, 0, quaternion[1] );
    AE_FIXED_PROPERTY( transformation3d, quaternion_z, 0, quaternion[2] );
    AE_FIXED_PROPERTY( transformation3d, quaternion_w, 0, quaternion[3] );

    AE_FIXED_PROPERTY( transformation3d, skew, 0, skew[0] );
    AE_FIXED_PROPERTY( transformation3d, skew_quaternion_z, 0, skew[1] );
    AE_FIXED_PROPERTY( transformation3d, skew_quaternion_w, 0, skew[2] );

    ae_movie_make_transformation3d_m34( _out, position, anchor_point, scale, quaternion, skew );
}
//////////////////////////////////////////////////////////////////////////
AE_CALLBACK ae_void_t __make_layer_transformation3d_fixed_wq( ae_matrix34_t _out, const aeMovieLayerTransformation * _transformation, ae_uint32_t _index )
{
    const aeMovieLayerTransformation3D * transformation3d = (const aeMovieLayerTransformation3D *)_transformation;

    ae_vector3_t anchor_point;
    ae_vector3_t position;
    ae_vector3_t scale;
    ae_skew_t skew;

    AE_FIXED_PROPERTY( transformation3d, anchor_point_x, 0, anchor_point[0] );
    AE_FIXED_PROPERTY( transformation3d, anchor_point_y, 0, anchor_point[1] );
    AE_FIXED_PROPERTY( transformation3d, anchor_point_z, 0, anchor_point[2] );

    AE_FIXED_PROPERTY( transformation3d, position_x, 0, position[0] );
    AE_FIXED_PROPERTY( transformation3d, position_y, 0, position[1] );
    AE_FIXED_PROPERTY( transformation3d, position_z, 0, position[2] );

    AE_FIXED_PROPERTY( transformation3d, scale_x, 0, scale[0] );
    AE_FIXED_PROPERTY( transformation3d, scale_y, 0, scale[1] );
    AE_FIXED_PROPERTY( transformation3d, scale_z, 0, scale[2] );

    AE_FIXED_PROPERTY( transformation3d, skew, 0, skew[0] );
    AE_FIXED_PROPERTY( transformation3d, skew_quaternion_z, 0, skew[1] );
    AE_FIXED_PROPERTY( transformation3d, skew_quaternion_w, 0, skew[2] );

    ae_movie_make_transformation3d_m34wq( _out, position, anchor_point, scale, skew );
}
//////////////////////////////////////////////////////////////////////////
AE_CALLBACK ae_void_t __make_layer_transformation3d_interpolate_wq( ae_matrix34_t _out, const aeMovieLayerTransformation * _transformation, ae_uint32_t _index, ae_float_t _t )
{
    const aeMovieLayerTransformation3D * transformation3d = (const aeMovieLayerTransformation3D *)_transformation;

    ae_vector3_t anchor_point;
    ae_vector3_t position;
    ae_vector3_t scale;
    ae_skew_t skew;

    AE_INTERPOLATE_PROPERTY( transformation3d, anchor_point_x, anchor_point[0] );
    AE_INTERPOLATE_PROPERTY( transformation3d, anchor_point_y, anchor_point[1] );
    AE_INTERPOLATE_PROPERTY( transformation3d, anchor_point_z, anchor_point[2] );

    AE_INTERPOLATE_PROPERTY( transformation3d, position_x, position[0] );
    AE_INTERPOLATE_PROPERTY( transformation3d, position_y, position[1] );
    AE_INTERPOLATE_PROPERTY( transformation3d, position_z, position[2] );

    AE_INTERPOLATE_PROPERTY( transformation3d, scale_x, scale[0] );
    AE_INTERPOLATE_PROPERTY( transformation3d, scale_y, scale[1] );
    AE_INTERPOLATE_PROPERTY( transformation3d, scale_z, scale[2] );

    AE_INTERPOLATE_PROPERTY( transformation3d, skew, skew[0] );
    AE_INTERPOLATE_PROPERTY( transformation3d, skew_quaternion_z, skew[1] );
    AE_INTERPOLATE_PROPERTY( transformation3d, skew_quaternion_w, skew[2] );

    ae_movie_make_transformation3d_m34wq( _out, position, anchor_point, scale, skew );
}
//////////////////////////////////////////////////////////////////////////
AE_CALLBACK ae_void_t __make_layer_transformation3d_interpolate_wskq( ae_matrix34_t _out, const aeMovieLayerTransformation * _transformation, ae_uint32_t _index, ae_float_t _t )
{
    const aeMovieLayerTransformation3D * transformation3d = (const aeMovieLayerTransformation3D *)_transformation;

    ae_vector3_t anchor_point;
    ae_vector3_t position;
    ae_vector3_t scale;

    AE_INTERPOLATE_PROPERTY( transformation3d, anchor_point_x, anchor_point[0] );
    AE_INTERPOLATE_PROPERTY( transformation3d, anchor_point_y, anchor_point[1] );
    AE_INTERPOLATE_PROPERTY( transformation3d, anchor_point_z, anchor_point[2] );

    AE_INTERPOLATE_PROPERTY( transformation3d, position_x, position[0] );
    AE_INTERPOLATE_PROPERTY( transformation3d, position_y, position[1] );
    AE_INTERPOLATE_PROPERTY( transformation3d, position_z, position[2] );

    AE_INTERPOLATE_PROPERTY( transformation3d, scale_x, scale[0] );
    AE_INTERPOLATE_PROPERTY( transformation3d, scale_y, scale[1] );
    AE_INTERPOLATE_PROPERTY( transformation3d, scale_z, scale[2] );

    ae_movie_make_transformation3d_m34wskq( _out, position, anchor_point, scale );
}
//////////////////////////////////////////////////////////////////////////
AE_CALLBACK ae_void_t __make_layer_transformation3d_fixed_wskq( ae_matrix34_t _out, const aeMovieLayerTransformation * _transformation, ae_uint32_t _index )
{
    const aeMovieLayerTransformation3D * transformation3d = (const aeMovieLayerTransformation3D *)_transformation;

    ae_vector3_t anchor_point;
    ae_vector3_t position;
    ae_vector3_t scale;

    AE_FIXED_PROPERTY( transformation3d, anchor_point_x, 0, anchor_point[0] );
    AE_FIXED_PROPERTY( transformation3d, anchor_point_y, 0, anchor_point[1] );
    AE_FIXED_PROPERTY( transformation3d, anchor_point_z, 0, anchor_point[2] );

    AE_FIXED_PROPERTY( transformation3d, position_x, 0, position[0] );
    AE_FIXED_PROPERTY( transformation3d, position_y, 0, position[1] );
    AE_FIXED_PROPERTY( transformation3d, position_z, 0, position[2] );

    AE_FIXED_PROPERTY( transformation3d, scale_x, 0, scale[0] );
    AE_FIXED_PROPERTY( transformation3d, scale_y, 0, scale[1] );
    AE_FIXED_PROPERTY( transformation3d, scale_z, 0, scale[2] );

    ae_movie_make_transformation3d_m34wskq( _out, position, anchor_point, scale );
}
//////////////////////////////////////////////////////////////////////////
AE_CALLBACK ae_void_t __make_layer_transformation3d_interpolate_wsk( ae_matrix34_t _out, const aeMovieLayerTransformation * _transformation, ae_uint32_t _index, ae_float_t _t )
{
    const aeMovieLayerTransformation3D * transformation3d = (const aeMovieLayerTransformation3D *)_transformation;

    ae_vector3_t anchor_point;
    ae_vector3_t position;
    ae_vector3_t scale;
    ae_quaternion_t quaternion;

    AE_INTERPOLATE_PROPERTY( transformation3d, anchor_point_x, anchor_point[0] );
    AE_INTERPOLATE_PROPERTY( transformation3d, anchor_point_y, anchor_point[1] );
    AE_INTERPOLATE_PROPERTY( transformation3d, anchor_point_z, anchor_point[2] );

    AE_INTERPOLATE_PROPERTY( transformation3d, position_x, position[0] );
    AE_INTERPOLATE_PROPERTY( transformation3d, position_y, position[1] );
    AE_INTERPOLATE_PROPERTY( transformation3d, position_z, position[2] );

    AE_INTERPOLATE_PROPERTY( transformation3d, scale_x, scale[0] );
    AE_INTERPOLATE_PROPERTY( transformation3d, scale_y, scale[1] );
    AE_INTERPOLATE_PROPERTY( transformation3d, scale_z, scale[2] );

    ae_quaternion_t q1;
    AE_FIXED_PROPERTY( transformation3d, quaternion_x, 0, q1[0] );
    AE_FIXED_PROPERTY( transformation3d, quaternion_y, 0, q1[1] );
    AE_FIXED_PROPERTY( transformation3d, quaternion_z, 0, q1[2] );
    AE_FIXED_PROPERTY( transformation3d, quaternion_w, 0, q1[3] );

    ae_quaternion_t q2;
    AE_FIXED_PROPERTY( transformation3d, quaternion_x, 1, q2[0] );
    AE_FIXED_PROPERTY( transformation3d, quaternion_y, 1, q2[1] );
    AE_FIXED_PROPERTY( transformation3d, quaternion_z, 1, q2[2] );
    AE_FIXED_PROPERTY( transformation3d, quaternion_w, 1, q2[3] );

    ae_linerp_q( quaternion, q1, q2, _t );

    ae_movie_make_transformation3d_m34wsk( _out, position, anchor_point, scale, quaternion );
}
//////////////////////////////////////////////////////////////////////////
AE_CALLBACK ae_void_t __make_layer_transformation3d_interpolate_wskfq( ae_matrix34_t _out, const aeMovieLayerTransformation * _transformation, ae_uint32_t _index, ae_float_t _t )
{
    const aeMovieLayerTransformation3D * transformation3d = (const aeMovieLayerTransformation3D *)_transformation;

    ae_vector3_t anchor_point;
    ae_vector3_t position;
    ae_vector3_t scale;
    ae_quaternion_t quaternion;

    AE_INTERPOLATE_PROPERTY( transformation3d, anchor_point_x, anchor_point[0] );
    AE_INTERPOLATE_PROPERTY( transformation3d, anchor_point_y, anchor_point[1] );
    AE_INTERPOLATE_PROPERTY( transformation3d, anchor_point_z, anchor_point[2] );

    AE_INTERPOLATE_PROPERTY( transformation3d, position_x, position[0] );
    AE_INTERPOLATE_PROPERTY( transformation3d, position_y, position[1] );
    AE_INTERPOLATE_PROPERTY( transformation3d, position_z, position[2] );

    AE_INTERPOLATE_PROPERTY( transformation3d, scale_x, scale[0] );
    AE_INTERPOLATE_PROPERTY( transformation3d, scale_y, scale[1] );
    AE_INTERPOLATE_PROPERTY( transformation3d, scale_z, scale[2] );

    AE_FIXED_PROPERTY( transformation3d, quaternion_x, 0, quaternion[0] );
    AE_FIXED_PROPERTY( transformation3d, quaternion_y, 0, quaternion[1] );
    AE_FIXED_PROPERTY( transformation3d, quaternion_z, 0, quaternion[2] );
    AE_FIXED_PROPERTY( transformation3d, quaternion_w, 0, quaternion[3] );

    ae_movie_make_transformation3d_m34wsk( _out, position, anchor_point, scale, quaternion );
}
//////////////////////////////////////////////////////////////////////////
AE_CALLBACK ae_void_t __make_layer_transformation3d_fixed_wsk( ae_matrix34_t _out, const aeMovieLayerTransformation * _transformation, ae_uint32_t _index )
{
    const aeMovieLayerTransformation3D * transformation3d = (const aeMovieLayerTransformation3D *)_transformation;

    ae_vector3_t anchor_point;
    ae_vector3_t position;
    ae_vector3_t scale;
    ae_quaternion_t quaternion;

    AE_FIXED_PROPERTY( transformation3d, anchor_point_x, 0, anchor_point[0] );
    AE_FIXED_PROPERTY( transformation3d, anchor_point_y, 0, anchor_point[1] );
    AE_FIXED_PROPERTY( transformation3d, anchor_point_z, 0, anchor_point[2] );

    AE_FIXED_PROPERTY( transformation3d, position_x, 0, position[0] );
    AE_FIXED_PROPERTY( transformation3d, position_y, 0, position[1] );
    AE_FIXED_PROPERTY( transformation3d, position_z, 0, position[2] );

    AE_FIXED_PROPERTY( transformation3d, scale_x, 0, scale[0] );
    AE_FIXED_PROPERTY( transformation3d, scale_y, 0, scale[1] );
    AE_FIXED_PROPERTY( transformation3d, scale_z, 0, scale[2] );

    AE_FIXED_PROPERTY( transformation3d, quaternion_x, 0, quaternion[0] );
    AE_FIXED_PROPERTY( transformation3d, quaternion_y, 0, quaternion[1] );
    AE_FIXED_PROPERTY( transformation3d, quaternion_z, 0, quaternion[2] );
    AE_FIXED_PROPERTY( transformation3d, quaternion_w, 0, quaternion[3] );

    ae_movie_make_transformation3d_m34wsk( _out, position, anchor_point, scale, quaternion );
}
//////////////////////////////////////////////////////////////////////////
AE_CALLBACK ae_void_t __make_layer_transformation3d_interpolate_fq( ae_matrix34_t _out, const aeMovieLayerTransformation * _transformation, ae_uint32_t _index, ae_float_t _t )
{
    const aeMovieLayerTransformation3D * transformation3d = (const aeMovieLayerTransformation3D *)_transformation;

    ae_vector3_t anchor_point;
    ae_vector3_t position;
    ae_vector3_t scale;
    ae_quaternion_t quaternion;
    ae_skew_t skew;

    AE_INTERPOLATE_PROPERTY( transformation3d, anchor_point_x, anchor_point[0] );
    AE_INTERPOLATE_PROPERTY( transformation3d, anchor_point_y, anchor_point[1] );
    AE_INTERPOLATE_PROPERTY( transformation3d, anchor_point_z, anchor_point[2] );

    AE_INTERPOLATE_PROPERTY( transformation3d, position_x, position[0] );
    AE_INTERPOLATE_PROPERTY( transformation3d, position_y, position[1] );
    AE_INTERPOLATE_PROPERTY( transformation3d, position_z, position[2] );

    AE_INTERPOLATE_PROPERTY( transformation3d, scale_x, scale[0] );
    AE_INTERPOLATE_PROPERTY( transformation3d, scale_y, scale[1] );
    AE_INTERPOLATE_PROPERTY( transformation3d, scale_z, scale[2] );

    AE_FIXED_PROPERTY( transformation3d, quaternion_x, 0, quaternion[0] );
    AE_FIXED_PROPERTY( transformation3d, quaternion_y, 0, quaternion[1] );
    AE_FIXED_PROPERTY( transformation3d, quaternion_z, 0, quaternion[2] );
    AE_FIXED_PROPERTY( transformation3d, quaternion_w, 0, quaternion[3] );

    AE_INTERPOLATE_PROPERTY( transformation3d, skew, skew[0] );
    AE_INTERPOLATE_PROPERTY( transformation3d, skew_quaternion_z, skew[1] );
    AE_INTERPOLATE_PROPERTY( transformation3d, skew_quaternion_w, skew[2] );

    ae_movie_make_transformation3d_m34( _out, position, anchor_point, scale, quaternion, skew );
}
//////////////////////////////////////////////////////////////////////////
AE_CALLBACK ae_void_t __make_layer_transformation3d_interpolate( ae_matrix34_t _out, const aeMovieLayerTransformation * _transformation, ae_uint32_t _index, ae_float_t _t )
{
    const aeMovieLayerTransformation3D * transformation3d = (const aeMovieLayerTransformation3D *)_transformation;

    ae_vector3_t anchor_point;
    ae_vector3_t position;
    ae_vector3_t scale;
    ae_quaternion_t quaternion;
    ae_skew_t skew;

    AE_INTERPOLATE_PROPERTY( transformation3d, anchor_point_x, anchor_point[0] );
    AE_INTERPOLATE_PROPERTY( transformation3d, anchor_point_y, anchor_point[1] );
    AE_INTERPOLATE_PROPERTY( transformation3d, anchor_point_z, anchor_point[2] );

    AE_INTERPOLATE_PROPERTY( transformation3d, position_x, position[0] );
    AE_INTERPOLATE_PROPERTY( transformation3d, position_y, position[1] );
    AE_INTERPOLATE_PROPERTY( transformation3d, position_z, position[2] );

    AE_INTERPOLATE_PROPERTY( transformation3d, scale_x, scale[0] );
    AE_INTERPOLATE_PROPERTY( transformation3d, scale_y, scale[1] );
    AE_INTERPOLATE_PROPERTY( transformation3d, scale_z, scale[2] );

    ae_quaternion_t q1;
    AE_FIXED_PROPERTY( transformation3d, quaternion_x, 0, q1[0] );
    AE_FIXED_PROPERTY( transformation3d, quaternion_y, 0, q1[1] );
    AE_FIXED_PROPERTY( transformation3d, quaternion_z, 0, q1[2] );
    AE_FIXED_PROPERTY( transformation3d, quaternion_w, 0, q1[3] );

    ae_quaternion_t q2;
    AE_FIXED_PROPERTY( transformation3d, quaternion_x, 1, q2[0] );
    AE_FIXED_PROPERTY( transformation3d, quaternion_y, 1, q2[1] );
    AE_FIXED_PROPERTY( transformation3d, quaternion_z, 1, q2[2] );
    AE_FIXED_PROPERTY( transformation3d, quaternion_w, 1, q2[3] );

    ae_linerp_q( quaternion, q1, q2, _t );

    AE_INTERPOLATE_PROPERTY( transformation3d, skew, skew[0] );
    AE_INTERPOLATE_PROPERTY( transformation3d, skew_quaternion_z, skew[1] );
    AE_INTERPOLATE_PROPERTY( transformation3d, skew_quaternion_w, skew[2] );

    ae_movie_make_transformation3d_m34( _out, position, anchor_point, scale, quaternion, skew );
}
//////////////////////////////////////////////////////////////////////////
ae_result_t ae_movie_load_layer_transformation( aeMovieStream * _stream, aeMovieLayerTransformation * _transformation, ae_bool_t _threeD )
{
    ae_uint32_t immutable_property_mask;
    AE_READ( _stream, immutable_property_mask );

    _transformation->immutable_property_mask = immutable_property_mask;

    ae_uint32_t identity_property_mask;
    AE_READ( _stream, identity_property_mask );

    _transformation->identity_property_mask = identity_property_mask;

    if( _threeD == AE_FALSE )
    {
        aeMovieLayerTransformation2D * transformation2d = (aeMovieLayerTransformation2D *)_transformation;

        aeMovieLayerTransformation2DTimeline * timeline = AE_NULLPTR;

        if( (immutable_property_mask & AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL) != AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL )
        {
            timeline = AE_NEW( _stream->instance, aeMovieLayerTransformation2DTimeline );

            AE_RESULT_PANIC_MEMORY( timeline );
        }

        transformation2d->timeline = timeline;

        AE_RESULT( __load_movie_layer_transformation2d, (_stream, transformation2d) );

        if( (identity_property_mask & AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL) == AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL )
        {
            _transformation->transforamtion_interpolate_matrix = &__make_layer_transformation_interpolate_identity;
            _transformation->transforamtion_fixed_matrix = &__make_layer_transformation_fixed_identity;
        }
        else if( (immutable_property_mask & AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL) == AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL )
        {
            _transformation->transforamtion_interpolate_matrix = &__make_layer_transformation_interpolate_immutable;
            _transformation->transforamtion_fixed_matrix = &__make_layer_transformation_fixed_immutable;
        }
        else
        {
            ae_uint32_t fixed_transformation = 0;

            if( (identity_property_mask & AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL_SKEW) == AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL_SKEW )
            {
                fixed_transformation |= 1;
            }

            if( (identity_property_mask & AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL_QUATERNION) == AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL_QUATERNION )
            {
                fixed_transformation |= 2;
            }
            else if( (immutable_property_mask & AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL_QUATERNION) == AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL_QUATERNION )
            {
                fixed_transformation |= 4;
            }

            ae_movie_make_layer_transformation_intepolate_t transforamtion_interpolate_matrix[6] = {
                &__make_layer_transformation2d_interpolate
                , &__make_layer_transformation2d_interpolate_wsk
                , &__make_layer_transformation2d_interpolate_wq
                , &__make_layer_transformation2d_interpolate_wskq
                , &__make_layer_transformation2d_interpolate_fq
                , &__make_layer_transformation2d_interpolate_wskfq
            };

            ae_movie_make_layer_transformation_fixed_t transforamtion_fixed_matrix[6] = {
                &__make_layer_transformation2d_fixed
                , &__make_layer_transformation2d_fixed_wsk
                , &__make_layer_transformation2d_fixed_wq
                , &__make_layer_transformation2d_fixed_wskq
                , &__make_layer_transformation2d_fixed
                , &__make_layer_transformation2d_fixed_wsk
            };

            if( fixed_transformation > 5 )
            {
                return AE_RESULT_INTERNAL_ERROR;
            }

            _transformation->transforamtion_interpolate_matrix = transforamtion_interpolate_matrix[fixed_transformation];
            _transformation->transforamtion_fixed_matrix = transforamtion_fixed_matrix[fixed_transformation];
        }
    }
    else
    {
        aeMovieLayerTransformation3D * transformation3d = (aeMovieLayerTransformation3D *)_transformation;

        aeMovieLayerTransformation3DTimeline * timeline = AE_NULLPTR;

        if( (immutable_property_mask & AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL) != AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL )
        {
            timeline = AE_NEW( _stream->instance, aeMovieLayerTransformation3DTimeline );

            AE_RESULT_PANIC_MEMORY( timeline );
        }

        transformation3d->timeline = timeline;

        AE_RESULT( __load_movie_layer_transformation3d, (_stream, transformation3d) );

        if( (identity_property_mask & AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL) == AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL )
        {
            _transformation->transforamtion_interpolate_matrix = &__make_layer_transformation_interpolate_identity;
            _transformation->transforamtion_fixed_matrix = &__make_layer_transformation_fixed_identity;
        }
        else if( (immutable_property_mask & AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL) == AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL )
        {
            _transformation->transforamtion_interpolate_matrix = &__make_layer_transformation_interpolate_immutable;
            _transformation->transforamtion_fixed_matrix = &__make_layer_transformation_fixed_immutable;
        }
        else
        {
            ae_uint32_t fixed_transformation = 0;

            if( (identity_property_mask & AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL_SKEW) == AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL_SKEW )
            {
                fixed_transformation |= 1;
            }

            if( (identity_property_mask & AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL_QUATERNION) == AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL_QUATERNION )
            {
                fixed_transformation |= 2;
            }
            else if( (immutable_property_mask & AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL_QUATERNION) == AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL_QUATERNION )
            {
                fixed_transformation |= 4;
            }

            ae_movie_make_layer_transformation_intepolate_t transforamtion_interpolate_matrix[6] = { 
                &__make_layer_transformation3d_interpolate
                , &__make_layer_transformation3d_interpolate_wsk
                , &__make_layer_transformation3d_interpolate_wq
                , &__make_layer_transformation3d_interpolate_wskq
                , &__make_layer_transformation3d_interpolate_fq
                , &__make_layer_transformation3d_interpolate_wskfq 
            };
            
            ae_movie_make_layer_transformation_fixed_t transforamtion_fixed_matrix[6] = {
                &__make_layer_transformation3d_fixed
                , &__make_layer_transformation3d_fixed_wsk
                , &__make_layer_transformation3d_fixed_wq
                , &__make_layer_transformation3d_fixed_wskq
                , &__make_layer_transformation3d_fixed
                , &__make_layer_transformation3d_fixed_wsk
            };

            if( fixed_transformation > 5 )
            {
                return AE_RESULT_INTERNAL_ERROR;
            }

            _transformation->transforamtion_interpolate_matrix = transforamtion_interpolate_matrix[fixed_transformation];
            _transformation->transforamtion_fixed_matrix = transforamtion_fixed_matrix[fixed_transformation];
        }
    }

    if( identity_property_mask & AE_MOVIE_PROPERTY_COLOR_R )
    {
        _transformation->immutable_color.color_r = 1.f;
        _transformation->timeline_color.color_r = AE_NULLPTR;
    }
    else if( immutable_property_mask & AE_MOVIE_PROPERTY_COLOR_R )
    {
        AE_READF( _stream, _transformation->immutable_color.color_r );
        _transformation->timeline_color.color_r = AE_NULLPTR;
    }
    else
    {
        _transformation->timeline_color.color_r = __load_movie_layer_transformation_timeline( _stream, "immutable_color_r" );
        _transformation->initial_color.color_r = __get_movie_layer_transformation_property_initial( _transformation->timeline_color.color_r );
    }

    if( identity_property_mask & AE_MOVIE_PROPERTY_COLOR_G )
    {
        _transformation->immutable_color.color_g = 1.f;
        _transformation->timeline_color.color_g = AE_NULLPTR;
    }
    else if( immutable_property_mask & AE_MOVIE_PROPERTY_COLOR_G )
    {
        AE_READF( _stream, _transformation->immutable_color.color_g );
        _transformation->timeline_color.color_g = AE_NULLPTR;
    }
    else
    {
        _transformation->timeline_color.color_g = __load_movie_layer_transformation_timeline( _stream, "immutable_color_g" );
        _transformation->initial_color.color_g = __get_movie_layer_transformation_property_initial( _transformation->timeline_color.color_g );
    }

    if( identity_property_mask & AE_MOVIE_PROPERTY_COLOR_B )
    {
        _transformation->immutable_color.color_b = 1.f;
        _transformation->timeline_color.color_b = AE_NULLPTR;
    }
    else if( immutable_property_mask & AE_MOVIE_PROPERTY_COLOR_B )
    {
        AE_READF( _stream, _transformation->immutable_color.color_b );
        _transformation->timeline_color.color_b = AE_NULLPTR;
    }
    else
    {
        _transformation->timeline_color.color_b = __load_movie_layer_transformation_timeline( _stream, "immutable_color_b" );
        _transformation->initial_color.color_b = __get_movie_layer_transformation_property_initial( _transformation->timeline_color.color_b );
    }

    if( identity_property_mask & AE_MOVIE_PROPERTY_OPACITY )
    {
        _transformation->immutable_opacity = 1.f;
        _transformation->timeline_opacity = AE_NULLPTR;
    }
    else if( immutable_property_mask & AE_MOVIE_PROPERTY_OPACITY )
    {
        AE_READF( _stream, _transformation->immutable_opacity );
        _transformation->timeline_opacity = AE_NULLPTR;
    }
    else
    {
        _transformation->timeline_opacity = __load_movie_layer_transformation_timeline( _stream, "immutable_opacity" );
        _transformation->initial_opacity = __get_movie_layer_transformation_property_initial( _transformation->timeline_opacity );
    }

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_result_t __load_camera_transformation_property( aeMovieStream * _stream, aeMovieCompositionCamera * _transformation )
{
    ae_uint32_t immutable_property_mask = _transformation->immutable_property_mask;
    ae_uint32_t identity_property_mask = _transformation->identity_property_mask;

    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_ANCHOR_POINT_X, target_x, 0.f );
    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_ANCHOR_POINT_Y, target_y, 0.f );
    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_ANCHOR_POINT_Z, target_z, 1.f );

    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_POSITION_X, position_x, 0.f );
    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_POSITION_Y, position_y, 0.f );
    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_POSITION_Z, position_z, 0.f );

    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_QUATERNION_X, quaternion_x, 0.f );
    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_QUATERNION_Y, quaternion_y, 0.f );
    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_QUATERNION_Z, quaternion_z, 0.f );
    AE_STREAM_PROPERTY( AE_MOVIE_PROPERTY_QUATERNION_W, quaternion_w, 1.f );

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
ae_result_t ae_movie_load_camera_transformation( aeMovieStream * _stream, aeMovieCompositionCamera * _camera )
{
    ae_uint32_t immutable_property_mask;
    AE_READ( _stream, immutable_property_mask );

    _camera->immutable_property_mask = immutable_property_mask;

    ae_uint32_t identity_property_mask;
    AE_READ( _stream, identity_property_mask );

    _camera->identity_property_mask = identity_property_mask;

    aeMovieCompositionCameraTimeline * timeline = AE_NULLPTR;

    if( (immutable_property_mask & AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL_CAMERA) != AE_MOVIE_PROPERTY_TRANSFORM_SUPER_ALL_CAMERA )
    {
        timeline = AE_NEW( _stream->instance, aeMovieCompositionCameraTimeline );

        AE_RESULT_PANIC_MEMORY( timeline );
    }

    _camera->timeline = timeline;

    AE_RESULT( __load_camera_transformation_property, (_stream, _camera) );

    return AE_RESULT_SUCCESSFUL;
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __delete_layer_transformation2d( const aeMovieInstance * _instance, const aeMovieLayerTransformation2D * _transformation )
{
    if( _transformation->timeline != AE_NULLPTR )
    {
        aeMovieLayerTransformation2DTimeline * timeline = _transformation->timeline;

        AE_DELETE( _instance, timeline->anchor_point_x );
        AE_DELETE( _instance, timeline->anchor_point_y );
        AE_DELETE( _instance, timeline->position_x );
        AE_DELETE( _instance, timeline->position_y );
        AE_DELETE( _instance, timeline->scale_x );
        AE_DELETE( _instance, timeline->scale_y );
        AE_DELETE( _instance, timeline->quaternion_z );
        AE_DELETE( _instance, timeline->quaternion_w );
        AE_DELETE( _instance, timeline->skew );
        AE_DELETE( _instance, timeline->skew_quaternion_z );
        AE_DELETE( _instance, timeline->skew_quaternion_w );

        AE_DELETE( _instance, _transformation->timeline );
    }
}
//////////////////////////////////////////////////////////////////////////
AE_INTERNAL ae_void_t __delete_layer_transformation3d( const aeMovieInstance * _instance, const aeMovieLayerTransformation3D * _transformation )
{
    if( _transformation->timeline != AE_NULLPTR )
    {
        aeMovieLayerTransformation3DTimeline * timeline = _transformation->timeline;

        AE_DELETE( _instance, timeline->anchor_point_x );
        AE_DELETE( _instance, timeline->anchor_point_y );
        AE_DELETE( _instance, timeline->anchor_point_z );
        AE_DELETE( _instance, timeline->position_x );
        AE_DELETE( _instance, timeline->position_y );
        AE_DELETE( _instance, timeline->position_z );
        AE_DELETE( _instance, timeline->scale_x );
        AE_DELETE( _instance, timeline->scale_y );
        AE_DELETE( _instance, timeline->scale_z );
        AE_DELETE( _instance, timeline->quaternion_x );
        AE_DELETE( _instance, timeline->quaternion_y );
        AE_DELETE( _instance, timeline->quaternion_z );
        AE_DELETE( _instance, timeline->quaternion_w );
        AE_DELETE( _instance, timeline->skew );
        AE_DELETE( _instance, timeline->skew_quaternion_z );
        AE_DELETE( _instance, timeline->skew_quaternion_w );

        AE_DELETE( _instance, _transformation->timeline );
    }
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_movie_delete_layer_transformation( const aeMovieInstance * _instance, const aeMovieLayerTransformation * _transformation, ae_bool_t _threeD )
{
    AE_DELETE( _instance, _transformation->timeline_color.color_r );
    AE_DELETE( _instance, _transformation->timeline_color.color_g );
    AE_DELETE( _instance, _transformation->timeline_color.color_b );
    AE_DELETE( _instance, _transformation->timeline_opacity );

    if( _threeD == AE_FALSE )
    {
        __delete_layer_transformation2d( _instance, (const aeMovieLayerTransformation2D *)_transformation );
    }
    else
    {
        __delete_layer_transformation3d( _instance, (const aeMovieLayerTransformation3D *)_transformation );
    }

    if( _transformation->immutable_matrix != AE_NULLPTR )
    {
        AE_DELETE( _instance, _transformation->immutable_matrix );
    }
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_movie_make_layer_matrix( ae_matrix34_t _out, const aeMovieLayerTransformation * _transformation, ae_bool_t _interpolate, ae_uint32_t _index, ae_float_t _t )
{
    if( _interpolate == AE_TRUE )
    {
        (*_transformation->transforamtion_interpolate_matrix)(_out, _transformation, _index, _t);
    }
    else
    {
        (*_transformation->transforamtion_fixed_matrix)(_out, _transformation, _index);
    }
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_movie_make_camera_transformation( ae_vector3_t _target, ae_vector3_t _position, ae_quaternion_t _quaternion, const aeMovieCompositionCamera * _camera, ae_uint32_t _index, ae_bool_t _interpolate, ae_float_t _t )
{
    if( _interpolate == AE_TRUE )
    {
        AE_INTERPOLATE_PROPERTY( _camera, target_x, _target[0] );
        AE_INTERPOLATE_PROPERTY( _camera, target_y, _target[1] );
        AE_INTERPOLATE_PROPERTY( _camera, target_z, _target[2] );

        AE_INTERPOLATE_PROPERTY( _camera, position_x, _position[0] );
        AE_INTERPOLATE_PROPERTY( _camera, position_y, _position[1] );
        AE_INTERPOLATE_PROPERTY( _camera, position_z, _position[2] );

        ae_quaternion_t q1;
        AE_FIXED_PROPERTY( _camera, quaternion_x, 0, q1[0] );
        AE_FIXED_PROPERTY( _camera, quaternion_y, 0, q1[1] );
        AE_FIXED_PROPERTY( _camera, quaternion_z, 0, q1[2] );
        AE_FIXED_PROPERTY( _camera, quaternion_w, 0, q1[3] );

        ae_quaternion_t q2;
        AE_FIXED_PROPERTY( _camera, quaternion_x, 1, q2[0] );
        AE_FIXED_PROPERTY( _camera, quaternion_y, 1, q2[1] );
        AE_FIXED_PROPERTY( _camera, quaternion_z, 1, q2[2] );
        AE_FIXED_PROPERTY( _camera, quaternion_w, 1, q2[3] );

        ae_linerp_q( _quaternion, q1, q2, _t );
    }
    else
    {
        AE_FIXED_PROPERTY( _camera, target_x, 0, _target[0] );
        AE_FIXED_PROPERTY( _camera, target_y, 0, _target[1] );
        AE_FIXED_PROPERTY( _camera, target_z, 0, _target[2] );

        AE_FIXED_PROPERTY( _camera, position_x, 0, _position[0] );
        AE_FIXED_PROPERTY( _camera, position_y, 0, _position[1] );
        AE_FIXED_PROPERTY( _camera, position_z, 0, _position[2] );

        AE_FIXED_PROPERTY( _camera, quaternion_x, 0, _quaternion[0] );
        AE_FIXED_PROPERTY( _camera, quaternion_y, 0, _quaternion[1] );
        AE_FIXED_PROPERTY( _camera, quaternion_z, 0, _quaternion[2] );
        AE_FIXED_PROPERTY( _camera, quaternion_w, 0, _quaternion[3] );
    }
}
//////////////////////////////////////////////////////////////////////////
ae_color_channel_t ae_movie_make_layer_color_r( const aeMovieLayerTransformation * _transformation, ae_uint32_t _index, ae_bool_t _interpolate, ae_float_t _t )
{
    if( _transformation->timeline_color.color_r == AE_NULLPTR )
    {
        return _transformation->immutable_color.color_r;
    }

    ae_color_channel_t value;

    if( _interpolate == AE_TRUE )
    {
        value = __get_movie_layer_transformation_property_interpolate( _transformation->timeline_color.color_r, _index, _t );
    }
    else
    {
        if( _index == 0 )
        {
            value = _transformation->initial_color.color_r;
        }
        else
        {
            value = __get_movie_layer_transformation_property_fixed( _transformation->timeline_color.color_r, _index );
        }
    }

    return value;
}
//////////////////////////////////////////////////////////////////////////
ae_color_channel_t ae_movie_make_layer_color_g( const aeMovieLayerTransformation * _transformation, ae_uint32_t _index, ae_bool_t _interpolate, ae_float_t _t )
{
    if( _transformation->timeline_color.color_g == AE_NULLPTR )
    {
        return _transformation->immutable_color.color_g;
    }

    ae_color_channel_t value;

    if( _interpolate == AE_TRUE )
    {
        value = __get_movie_layer_transformation_property_interpolate( _transformation->timeline_color.color_g, _index, _t );
    }
    else
    {
        if( _index == 0 )
        {
            value = _transformation->initial_color.color_g;
        }
        else
        {
            value = __get_movie_layer_transformation_property_fixed( _transformation->timeline_color.color_g, _index );
        }
    }

    return value;
}
//////////////////////////////////////////////////////////////////////////
ae_color_channel_t ae_movie_make_layer_color_b( const aeMovieLayerTransformation * _transformation, ae_uint32_t _index, ae_bool_t _interpolate, ae_float_t _t )
{
    if( _transformation->timeline_color.color_b == AE_NULLPTR )
    {
        return _transformation->immutable_color.color_b;
    }

    ae_color_channel_t value;

    if( _interpolate == AE_TRUE )
    {
        value = __get_movie_layer_transformation_property_interpolate( _transformation->timeline_color.color_b, _index, _t );
    }
    else
    {
        if( _index == 0 )
        {
            value = _transformation->initial_color.color_b;
        }
        else
        {
            value = __get_movie_layer_transformation_property_fixed( _transformation->timeline_color.color_b, _index );
        }
    }

    return value;
}
//////////////////////////////////////////////////////////////////////////
ae_color_channel_t ae_movie_make_layer_opacity( const aeMovieLayerTransformation * _transformation, ae_uint32_t _index, ae_bool_t _interpolate, ae_float_t _t )
{
    if( _transformation->timeline_opacity == AE_NULLPTR )
    {
        return _transformation->immutable_opacity;
    }

    ae_color_channel_t value;

    if( _interpolate == AE_TRUE )
    {
        value = __get_movie_layer_transformation_property_interpolate( _transformation->timeline_opacity, _index, _t );
    }
    else
    {
        if( _index == 0 )
        {
            value = _transformation->initial_opacity;
        }
        else
        {
            value = __get_movie_layer_transformation_property_fixed( _transformation->timeline_opacity, _index );
        }
    }

    return value;
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_movie_make_layer_transformation2d_interpolate( ae_vector2_t _anchor_point, ae_vector2_t _position, ae_vector2_t _scale, ae_quaternionzw_t _quaternion, ae_skew_t _skew, const aeMovieLayerTransformation2D * _transformation2d, ae_uint32_t _index, ae_float_t _t )
{
    AE_INTERPOLATE_PROPERTY( _transformation2d, anchor_point_x, _anchor_point[0] );
    AE_INTERPOLATE_PROPERTY( _transformation2d, anchor_point_y, _anchor_point[1] );

    AE_INTERPOLATE_PROPERTY( _transformation2d, position_x, _position[0] );
    AE_INTERPOLATE_PROPERTY( _transformation2d, position_y, _position[1] );

    AE_INTERPOLATE_PROPERTY( _transformation2d, scale_x, _scale[0] );
    AE_INTERPOLATE_PROPERTY( _transformation2d, scale_y, _scale[1] );

    ae_quaternionzw_t q1;
    AE_FIXED_PROPERTY( _transformation2d, quaternion_z, 0, q1[0] );
    AE_FIXED_PROPERTY( _transformation2d, quaternion_w, 0, q1[1] );

    ae_quaternionzw_t q2;
    AE_FIXED_PROPERTY( _transformation2d, quaternion_z, 1, q2[0] );
    AE_FIXED_PROPERTY( _transformation2d, quaternion_w, 1, q2[1] );

    ae_linerp_qzw( _quaternion, q1, q2, _t );

    AE_INTERPOLATE_PROPERTY( _transformation2d, skew, _skew[0] );
    AE_INTERPOLATE_PROPERTY( _transformation2d, skew_quaternion_z, _skew[1] );
    AE_INTERPOLATE_PROPERTY( _transformation2d, skew_quaternion_w, _skew[2] );
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_movie_make_layer_transformation2d_fixed( ae_vector2_t _anchor_point, ae_vector2_t _position, ae_vector2_t _scale, ae_quaternionzw_t _quaternion, ae_skew_t _skew, const aeMovieLayerTransformation2D * _transformation2d, ae_uint32_t _index )
{
    AE_FIXED_PROPERTY( _transformation2d, anchor_point_x, 0, _anchor_point[0] );
    AE_FIXED_PROPERTY( _transformation2d, anchor_point_y, 0, _anchor_point[1] );

    AE_FIXED_PROPERTY( _transformation2d, position_x, 0, _position[0] );
    AE_FIXED_PROPERTY( _transformation2d, position_y, 0, _position[1] );

    AE_FIXED_PROPERTY( _transformation2d, scale_x, 0, _scale[0] );
    AE_FIXED_PROPERTY( _transformation2d, scale_y, 0, _scale[1] );

    AE_FIXED_PROPERTY( _transformation2d, quaternion_z, 0, _quaternion[0] );
    AE_FIXED_PROPERTY( _transformation2d, quaternion_w, 0, _quaternion[1] );

    AE_FIXED_PROPERTY( _transformation2d, skew, 0, _skew[0] );
    AE_FIXED_PROPERTY( _transformation2d, skew_quaternion_z, 0, _skew[1] );
    AE_FIXED_PROPERTY( _transformation2d, skew_quaternion_w, 0, _skew[2] );
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_movie_make_layer_transformation_color_interpolate( ae_color_t * _color, ae_color_channel_t * _opacity, const aeMovieLayerTransformation * _transformation, ae_uint32_t _index, ae_float_t _t )
{
    _color->r = (_transformation->timeline_color.color_r == AE_NULLPTR) ?
        _transformation->immutable_color.color_r : __get_movie_layer_transformation_property_interpolate( _transformation->timeline_color.color_r, _index, _t );

    _color->g = (_transformation->timeline_color.color_g == AE_NULLPTR) ?
        _transformation->immutable_color.color_g : __get_movie_layer_transformation_property_interpolate( _transformation->timeline_color.color_g, _index, _t );

    _color->b = (_transformation->timeline_color.color_b == AE_NULLPTR) ?
        _transformation->immutable_color.color_b : __get_movie_layer_transformation_property_interpolate( _transformation->timeline_color.color_b, _index, _t );

    *_opacity = (_transformation->timeline_opacity == AE_NULLPTR) ?
        _transformation->immutable_opacity : __get_movie_layer_transformation_property_interpolate( _transformation->timeline_opacity, _index, _t );
}
//////////////////////////////////////////////////////////////////////////
ae_void_t ae_movie_make_layer_transformation_color_fixed( ae_color_t * _color, ae_color_channel_t * _opacity, const aeMovieLayerTransformation * _transformation, ae_uint32_t _index )
{
    _color->r = (_transformation->timeline_color.color_r == AE_NULLPTR) ?
        _transformation->immutable_color.color_r : __get_movie_layer_transformation_property_fixed( _transformation->timeline_color.color_r, _index );

    _color->g = (_transformation->timeline_color.color_g == AE_NULLPTR) ?
        _transformation->immutable_color.color_g : __get_movie_layer_transformation_property_fixed( _transformation->timeline_color.color_g, _index );

    _color->b = (_transformation->timeline_color.color_b == AE_NULLPTR) ?
        _transformation->immutable_color.color_b : __get_movie_layer_transformation_property_fixed( _transformation->timeline_color.color_b, _index );

    *_opacity = (_transformation->timeline_opacity == AE_NULLPTR) ?
        _transformation->immutable_opacity : __get_movie_layer_transformation_property_fixed( _transformation->timeline_opacity, _index );
}
