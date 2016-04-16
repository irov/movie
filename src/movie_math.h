#	ifndef MOVIE_MATRIX_H_
#	define MOVIE_MATRIX_H_

#	include <movie/movie_type.h>

void mul_v2_v2_m4( ae_vector2_t _out, const ae_vector2_t _a, const ae_matrix4_t _b );
void mul_v3_v2_m4( ae_vector3_t _out, const ae_vector2_t _a, const ae_matrix4_t _b );
void mul_v4_m4( ae_vector4_t _out, const ae_vector4_t _a, const ae_matrix4_t _b );
void mul_m4_m4( ae_matrix4_t _out, const ae_matrix4_t _a, const ae_matrix4_t _b );
void ident_m4( ae_matrix4_t _out );
void make_rotate_z_axis_m4( ae_matrix4_t _out, float _angle );
void make_rotate_m4_euler( ae_matrix4_t _out, float _x, float _y, float _z );
void make_transformation_m4( ae_matrix4_t _lm, const ae_vector3_t _position, const ae_vector3_t _origin, const ae_vector3_t _scale, const ae_vector3_t _orientation );
void norm_v3_v3( ae_vector3_t _out, const ae_vector3_t _rhs );
void cross_v3_v3( ae_vector3_t _out, const ae_vector3_t _a, const ae_vector3_t _b );
void cross_v3_v3_norm( ae_vector3_t _out, const ae_vector3_t _a, const ae_vector3_t _b );
float dot_v3_v3( const ae_vector3_t a, const ae_vector3_t b );
void make_lookat_m4( ae_matrix4_t _out, const ae_vector3_t _eye, const ae_vector3_t _dir, const ae_vector3_t _up );
void make_projection_fov_m4( ae_matrix4_t _out, float fovy, float aspect, float zn, float zf );
void mul_v3_v3_m4_homogenize( ae_vector3_t _out, const ae_vector3_t _a, const ae_matrix4_t _b );

float angle_norm( float _angle );
float angle_correct_interpolate_from_to( float _from, float _to );

float linerp_f1( float _in1, float _in2, float _scale );

float make_camera_fov( float _height, float _zoom );

#	endif