#include "em_movieplayer.h"

#include "movie/movie.h"

#include <stdint.h>
#include <string.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <SDL/SDL_image.h>

#include "em_typedef.h"
#include "em_memory.h"
#include "em_math.h"
#include "em_opengles.h"

//////////////////////////////////////////////////////////////////////////
typedef struct em_blend_render_vertex_t
{
    float position[3];
    uint32_t color;
    float uv[2];
} em_blend_render_vertex_t;
//////////////////////////////////////////////////////////////////////////
typedef struct em_blend_shader_t
{
    GLuint program_id;

    GLint positionLocation;
    GLint colorLocation;
    GLint texcoordLocation;
    GLint mvpMatrixLocation;
    GLint tex0Location;
} em_blend_shader_t;
//////////////////////////////////////////////////////////////////////////
typedef struct em_track_matte_render_vertex_t
{
    float position[3];
    uint32_t color;
    float uv0[2];
    float uv1[2];
} em_track_matte_render_vertex_t;
//////////////////////////////////////////////////////////////////////////
typedef struct em_track_matte_shader_t
{
    GLuint program_id;

    GLint positionLocation;
    GLint colorLocation;
    GLint texcoord0Location;
    GLint texcoord1Location;
    GLint mvpMatrixLocation;
    GLint tex0Location;
    GLint tex1Location;
} em_track_matte_shader_t;
//////////////////////////////////////////////////////////////////////////
typedef struct em_player_t
{
    aeMovieInstance * instance;

    float width;
    float height;

    const em_blend_shader_t * blend_shader;
    const em_track_matte_shader_t * track_matte_shader;

    em_blend_render_vertex_t blend_vertices[1024];
    em_track_matte_render_vertex_t track_matte_vertices[1024];

} em_player_t;
//////////////////////////////////////////////////////////////////////////
static void * __instance_alloc( void * _data, size_t _size )
{
    (void)_data;

    return EM_MALLOC( _size );
}
//////////////////////////////////////////////////////////////////////////
static void * __instance_alloc_n( void * _data, size_t _size, size_t _count )
{
    (void)_data;

    size_t total = _size * _count;

    return EM_MALLOC( total );
}
//////////////////////////////////////////////////////////////////////////
static void __instance_free( void * _data, const void * _ptr )
{
    (void)_data;

    EM_FREE( (void *)_ptr );
}
//////////////////////////////////////////////////////////////////////////
static void __instance_free_n( void * _data, const void * _ptr )
{
    (void)_data;

    EM_FREE( (void *)_ptr );
}
//////////////////////////////////////////////////////////////////////////
static void __instance_logerror( void * _data, aeMovieErrorCode _code, const char * _format, ... )
{
    (void)_data;
    (void)_code;
    (void)_format;
}
//////////////////////////////////////////////////////////////////////////
static em_blend_shader_t * __make_blend_shader()
{
    const char * vertex_shader_source =
        "uniform highp mat4 g_mvpMatrix;\n"
        "attribute highp vec4 a_position;\n"
        "attribute lowp vec4 a_color;\n"
        "attribute mediump vec2 a_texcoord;\n"
        "varying lowp vec4 v_color;\n"
        "varying mediump vec2 v_texcoord;\n"
        "void main( void )\n"
        "{\n"
        "   gl_Position = g_mvpMatrix * a_position;\n"
        "   v_color = a_color;\n"
        "   v_texcoord = a_texcoord;\n"
        "}\n";

    const char * fragment_shader_source =
        "uniform sampler2D g_tex0;\n"
        "varying lowp vec4 v_color;\n"
        "varying mediump vec2 v_texcoord;\n"
        "void main( void ) {\n"
        "mediump vec4 c = texture2D( g_tex0, v_texcoord );\n"
        "gl_FragColor = v_color * c;\n"
        "}\n";

    GLuint program_id = __make_opengl_program( "__blend__", 100, vertex_shader_source, fragment_shader_source );

    if( program_id == 0U )
    {
        return em_nullptr;
    }

    int positionLocation;
    GLCALLR( positionLocation, glGetAttribLocation, (program_id, "a_position") );

    emscripten_log( EM_LOG_CONSOLE, "opengl attrib a_position '%d'\n"
        , positionLocation
    );

    int colorLocation;
    GLCALLR( colorLocation, glGetAttribLocation, (program_id, "a_color") );

    emscripten_log( EM_LOG_CONSOLE, "opengl attrib a_color '%d'\n"
        , colorLocation
    );

    int texcoordLocation;
    GLCALLR( texcoordLocation, glGetAttribLocation, (program_id, "a_texcoord") );

    emscripten_log( EM_LOG_CONSOLE, "opengl attrib a_texcoord '%d'\n"
        , texcoordLocation
    );

    int mvpMatrixLocation;
    GLCALLR( mvpMatrixLocation, glGetUniformLocation, (program_id, "g_mvpMatrix") );

    emscripten_log( EM_LOG_CONSOLE, "opengl uniform g_mvpMatrix '%d'\n"
        , mvpMatrixLocation
    );

    int tex0Location;
    GLCALLR( tex0Location, glGetUniformLocation, (program_id, "g_tex0") );

    emscripten_log( EM_LOG_CONSOLE, "opengl uniform g_tex0 '%d'\n"
        , tex0Location
    );

    em_blend_shader_t * blend_shader = EM_NEW( em_blend_shader_t );

    blend_shader->program_id = program_id;
    blend_shader->positionLocation = positionLocation;
    blend_shader->colorLocation = colorLocation;
    blend_shader->texcoordLocation = texcoordLocation;
    blend_shader->mvpMatrixLocation = mvpMatrixLocation;
    blend_shader->tex0Location = tex0Location;

    emscripten_log( EM_LOG_CONSOLE, "opengl create program '__blend__' id '%d'\n"
        , program_id
    );

    return blend_shader;
}
//////////////////////////////////////////////////////////////////////////
static em_track_matte_shader_t * __make_track_matte_shader()
{
    const char * vertex_shader_source =
        "uniform highp mat4 g_mvpMatrix;\n"
        "attribute highp vec4 a_position;\n"
        "attribute lowp vec4 a_color;\n"
        "attribute mediump vec2 a_texcoord0;\n"
        "attribute mediump vec2 a_texcoord1;\n"
        "varying lowp vec4 v_color;\n"
        "varying mediump vec2 v_texcoord0;\n"
        "varying mediump vec2 v_texcoord1;\n"
        "void main( void )\n"
        "{\n"
        "   gl_Position = g_mvpMatrix * a_position;\n"
        "   v_color = a_color;\n"
        "   v_texcoord0 = a_texcoord0;\n"
        "   v_texcoord1 = a_texcoord1;\n"
        "}\n";

    const char * fragment_shader_source =
        "uniform sampler2D g_tex0;\n"
        "uniform sampler2D g_tex1;\n"
        "varying lowp vec4 v_color;\n"
        "varying mediump vec2 v_texcoord0;\n"
        "varying mediump vec2 v_texcoord1;\n"
        "void main( void ) {\n"
        "mediump vec4 c0 = texture2D( g_tex0, v_texcoord0 );\n"
        "mediump vec4 c1 = texture2D( g_tex1, v_texcoord1 );\n"
        "gl_FragColor = v_color * c0 * vec4(1.0, 1.0, 1.0, c1.a);\n"
        "}\n";

    GLuint program_id = __make_opengl_program( "__track_matte__", 100, vertex_shader_source, fragment_shader_source );

    if( program_id == 0U )
    {
        return em_nullptr;
    }

    int positionLocation;
    GLCALLR( positionLocation, glGetAttribLocation, (program_id, "a_position") );

    emscripten_log( EM_LOG_CONSOLE, "opengl attrib a_position '%d'\n"
        , positionLocation
    );

    int colorLocation;
    GLCALLR( colorLocation, glGetAttribLocation, (program_id, "a_color") );

    emscripten_log( EM_LOG_CONSOLE, "opengl attrib a_color '%d'\n"
        , colorLocation
    );

    int texcoord0Location;
    GLCALLR( texcoord0Location, glGetAttribLocation, (program_id, "a_texcoord0") );

    emscripten_log( EM_LOG_CONSOLE, "opengl attrib a_texcoord0 '%d'\n"
        , texcoord0Location
    );

    int texcoord1Location;
    GLCALLR( texcoord1Location, glGetAttribLocation, (program_id, "a_texcoord1") );

    emscripten_log( EM_LOG_CONSOLE, "opengl attrib a_texcoord1 '%d'\n"
        , texcoord1Location
    );

    int mvpMatrixLocation;
    GLCALLR( mvpMatrixLocation, glGetUniformLocation, (program_id, "g_mvpMatrix") );

    emscripten_log( EM_LOG_CONSOLE, "opengl uniform g_mvpMatrix '%d'\n"
        , mvpMatrixLocation
    );

    int tex0Location;
    GLCALLR( tex0Location, glGetUniformLocation, (program_id, "g_tex0") );

    emscripten_log( EM_LOG_CONSOLE, "opengl uniform g_tex0 '%d'\n"
        , tex0Location
    );

    int tex1Location;
    GLCALLR( tex1Location, glGetUniformLocation, (program_id, "g_tex1") );

    emscripten_log( EM_LOG_CONSOLE, "opengl uniform g_tex1 '%d'\n"
        , tex1Location
    );

    em_track_matte_shader_t * track_matte_shader = EM_NEW( em_track_matte_shader_t );

    track_matte_shader->program_id = program_id;
    track_matte_shader->positionLocation = positionLocation;
    track_matte_shader->colorLocation = colorLocation;
    track_matte_shader->texcoord0Location = texcoord0Location;
    track_matte_shader->texcoord1Location = texcoord1Location;
    track_matte_shader->mvpMatrixLocation = mvpMatrixLocation;
    track_matte_shader->tex0Location = tex0Location;
    track_matte_shader->tex1Location = tex1Location;

    emscripten_log( EM_LOG_CONSOLE, "opengl create program '__track_mate__' id '%d'\n"
        , program_id
    );

    return track_matte_shader;
}
//////////////////////////////////////////////////////////////////////////
em_player_handle_t em_create_player( const char * _hashkey, float _width, float _height )
{
    em_player_t * em_player = EM_NEW( em_player_t );

    aeMovieInstance * ae_instance = ae_create_movie_instance( _hashkey
        , &__instance_alloc
        , &__instance_alloc_n
        , &__instance_free
        , &__instance_free_n
        , AE_NULL
        , &__instance_logerror
        , AE_NULL );

    em_player->instance = ae_instance;

    em_player->width = _width;
    em_player->height = _height;

    IMG_Init( IMG_INIT_PNG );


    em_blend_shader_t * blend_shader = __make_blend_shader();

    if( blend_shader == em_nullptr )
    {
        return em_nullptr;
    }

    em_player->blend_shader = blend_shader;

    em_track_matte_shader_t * track_matte_shader = __make_track_matte_shader();

    if( track_matte_shader == em_nullptr )
    {
        return em_nullptr;
    }

    em_player->track_matte_shader = track_matte_shader;

    emscripten_log( EM_LOG_CONSOLE, "successful create player hashkey '%s' width '%f' height '%f'"
        , _hashkey
        , _width
        , _height
    );

    return em_player;
}
//////////////////////////////////////////////////////////////////////////
void em_delete_player( em_player_handle_t _player )
{
    em_player_t * player = (em_player_t *)_player;

    free( (em_blend_shader_t *)player->blend_shader );

    ae_delete_movie_instance( player->instance );

    free( player );

    emscripten_log( EM_LOG_CONSOLE, "successful delete movie instance" );
}
//////////////////////////////////////////////////////////////////////////
static ae_size_t __read_file( ae_voidptr_t _data, ae_voidptr_t _buff, ae_uint32_t _carriage, ae_uint32_t _size )
{
    (void)_carriage;

    FILE * f = (FILE *)_data;

    ae_size_t s = fread( _buff, 1, _size, f );

    //emscripten_log( EM_LOG_CONSOLE, "read_file %u", _size );

    return s;
}
//////////////////////////////////////////////////////////////////////////
static void __memory_copy( ae_voidptr_t _data, ae_constvoidptr_t _src, ae_voidptr_t _dst, ae_size_t _size )
{
    (void)_data;
    memcpy( _dst, _src, _size );

    //emscripten_log( EM_LOG_CONSOLE, "memory_copy %u", _size );
}
//////////////////////////////////////////////////////////////////////////
typedef struct em_resource_image_t
{
    GLuint texture_id;
} em_resource_image_t;
//////////////////////////////////////////////////////////////////////////
static ae_voidptr_t __ae_movie_data_resource_provider( const aeMovieResource * _resource, ae_voidptr_t _ud )
{
    (void)_resource;
    (void)_ud;

    switch( _resource->type )
    {
    case AE_MOVIE_RESOURCE_IMAGE:
        {
            const aeMovieResourceImage * r = (const aeMovieResourceImage *)_resource;

            emscripten_log( EM_LOG_CONSOLE, "Resource type: image.\n" );
            emscripten_log( EM_LOG_CONSOLE, " path        = '%s'\n", r->path );
            emscripten_log( EM_LOG_CONSOLE, " trim_width  = %f\n", r->trim_width );
            emscripten_log( EM_LOG_CONSOLE, " trim_height = %f\n", r->trim_height );

            em_resource_image_t * resource_image = EM_NEW( em_resource_image_t );

            SDL_Surface * surface = IMG_Load( r->path );

            if( surface == NULL )
            {
                emscripten_log( EM_LOG_CONSOLE, "bad load image '%s'\n", r->path );
            }

            GLuint texture_id = __make_opengl_texture( surface->w, surface->h, surface->pixels );

            SDL_FreeSurface( surface );

            resource_image->texture_id = texture_id;

            emscripten_log( EM_LOG_CONSOLE, "create texture %ux%u (%u) id '%d'\n"
                , surface->w
                , surface->h
                , surface->format->BytesPerPixel
                , texture_id
            );

            return resource_image;

            break;
        }
    case AE_MOVIE_RESOURCE_SEQUENCE:
        {
            emscripten_log( EM_LOG_CONSOLE, "Resource type: image sequence.\n" );

            break;
        }
    case AE_MOVIE_RESOURCE_VIDEO:
        {
            //			const aeMovieResourceVideo * r = (const aeMovieResourceVideo *)_resource;

            emscripten_log( EM_LOG_CONSOLE, "Resource type: video.\n" );

            break;
        }
    case AE_MOVIE_RESOURCE_SOUND:
        {
            const aeMovieResourceSound * r = (const aeMovieResourceSound *)_resource;

            emscripten_log( EM_LOG_CONSOLE, "Resource type: sound.\n" );
            emscripten_log( EM_LOG_CONSOLE, " path        = '%s'", r->path );

            break;
        }
    case AE_MOVIE_RESOURCE_SLOT:
        {
            const aeMovieResourceSlot * r = (const aeMovieResourceSlot *)_resource;

            emscripten_log( EM_LOG_CONSOLE, "Resource type: slot.\n" );
            emscripten_log( EM_LOG_CONSOLE, " width  = %.2f\n", r->width );
            emscripten_log( EM_LOG_CONSOLE, " height = %.2f\n", r->height );

            break;
        }
    default:
        {
            emscripten_log( EM_LOG_CONSOLE, "Resource type: other (%i).\n", _resource->type );
            break;
        }
    }

    return AE_NULL;
}
//////////////////////////////////////////////////////////////////////////
static void __ae_movie_data_resource_deleter( aeMovieResourceTypeEnum _type, const ae_voidptr_t * _data, ae_voidptr_t _ud )
{
    (void)_data;
    (void)_ud;

    switch( _type )
    {
    case AE_MOVIE_RESOURCE_IMAGE:
        {
            em_resource_image_t * image_data = (em_resource_image_t *)_data;

            glDeleteTextures( 1, &image_data->texture_id );

            free( (ae_voidptr_t *)_data );

            break;
        }
    case AE_MOVIE_RESOURCE_SEQUENCE:
        {
            break;
        }
    case AE_MOVIE_RESOURCE_VIDEO:
        {
            break;
        }
    case AE_MOVIE_RESOURCE_SOUND:
        {
            break;
        }
    case AE_MOVIE_RESOURCE_SLOT:
        {
            break;
        }
    default:
        {
            break;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
em_movie_data_handle_t em_create_movie_data( em_player_handle_t _player, const char * _path )
{
    em_player_t * em_instance = (em_player_t *)_player;

    aeMovieData * ae_movie_data = ae_create_movie_data( em_instance->instance, &__ae_movie_data_resource_provider, &__ae_movie_data_resource_deleter, AE_NULL );

    FILE * f = fopen( _path, "rb" );

    if( f == NULL )
    {
        emscripten_log( EM_LOG_CONSOLE, "invalid open file (%s)", _path );

        return AE_NULL;
    }

    fseek( f, 0L, SEEK_END );
    size_t sz = ftell( f );
    rewind( f );

    ae_voidptr_t * buffer = EM_MALLOC( sz );
    fread( buffer, sz, 1, f );
    fclose( f );

    aeMovieStream * ae_stream = ae_create_movie_stream_memory( em_instance->instance, buffer, &__memory_copy, AE_NULL );

    if( ae_load_movie_data( ae_movie_data, ae_stream ) == AE_MOVIE_FAILED )
    {
        emscripten_log( EM_LOG_CONSOLE, "WRONG LOAD (%s)", _path );
    }

    ae_delete_movie_stream( ae_stream );

    free( buffer );

    emscripten_log( EM_LOG_CONSOLE, "successful create movie data from file '%s'", _path );

    return ae_movie_data;
}
//////////////////////////////////////////////////////////////////////////
void em_delete_movie_data( em_movie_data_handle_t _movieData )
{
    aeMovieData * ae_movie_data = (aeMovieData *)_movieData;

    ae_delete_movie_data( ae_movie_data );

    emscripten_log( EM_LOG_CONSOLE, "successful delete movie data" );
}
//////////////////////////////////////////////////////////////////////////
typedef struct em_node_track_matte_t
{
    em_resource_image_t * base_image;
    em_resource_image_t * track_matte_image;
} em_node_track_matte_t;
//////////////////////////////////////////////////////////////////////////
static ae_voidptr_t __ae_movie_composition_node_provider( const aeMovieNodeProviderCallbackData * _callbackData, void * _data )
{
    (void)_data;

    const aeMovieLayerData * layer = _callbackData->layer;

    ae_bool_t is_track_matte = ae_is_movie_layer_data_track_mate( layer );

    if( is_track_matte == AE_TRUE )
    {
        return AE_NULL;
    }

    aeMovieLayerTypeEnum type = ae_get_movie_layer_data_type( layer );

    if( _callbackData->track_matte_layer != AE_NULL )
    {
        switch( type )
        {
        case AE_MOVIE_LAYER_TYPE_IMAGE:
            {
                em_node_track_matte_t * node_track_matte = EM_NEW( em_node_track_matte_t );

                em_resource_image_t * base_image = (em_resource_image_t *)ae_get_movie_layer_data_resource_data( _callbackData->layer );
                em_resource_image_t * track_matte_image = (em_resource_image_t *)ae_get_movie_layer_data_resource_data( _callbackData->track_matte_layer );

                node_track_matte->base_image = base_image;
                node_track_matte->track_matte_image = track_matte_image;

                return node_track_matte;
            }break;
        default:
            {
            }break;
        }
    }
    else
    {
        switch( type )
        {
        case AE_MOVIE_LAYER_TYPE_VIDEO:
            {
                //Empty

                return AE_NULL;
            }break;
        case AE_MOVIE_LAYER_TYPE_SOUND:
            {
                //Empty

                return AE_NULL;
            }break;
        default:
            {
            }break;
        }
    }

    return AE_NULL;
}
//////////////////////////////////////////////////////////////////////////
static void __ae_movie_composition_node_deleter( const aeMovieNodeDeleterCallbackData * _callbackData, void * _data )
{
    (void)_data;

    const aeMovieLayerData * layer = _callbackData->layer;

    ae_bool_t is_track_matte = ae_is_movie_layer_data_track_mate( layer );

    if( is_track_matte == AE_TRUE )
    {
        return;
    }

    aeMovieLayerTypeEnum type = ae_get_movie_layer_data_type( layer );

    if( _callbackData->track_matte_layer != AE_NULL )
    {
        switch( type )
        {
        case AE_MOVIE_LAYER_TYPE_IMAGE:
            {                
                em_node_track_matte_t * node_track_matte = (em_node_track_matte_t *)_callbackData->element;

                EM_DELETE( node_track_matte );
            }break;
        default:
            {
            }break;
        }
    }
    else
    {
        switch( type )
        {
        case AE_MOVIE_LAYER_TYPE_VIDEO:
            {
            }break;
        case AE_MOVIE_LAYER_TYPE_SOUND:
            {
            }break;
        default:
            {
            }break;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
typedef struct ae_camera_t
{
    float view[16];
    float projection[16];
    float width;
    float height;
} ae_camera_t;
//////////////////////////////////////////////////////////////////////////
static void * __ae_movie_callback_camera_provider( const aeMovieCameraProviderCallbackData * _callbackData, void * _data )
{
    (void)_data;

    ae_camera_t * camera = EM_NEW( ae_camera_t );

    __make_lookat_m4( camera->view, _callbackData->position, _callbackData->target );
    __make_projection_fov_m4( camera->projection, _callbackData->width, _callbackData->height, _callbackData->fov );

    camera->width = _callbackData->width;
    camera->height = _callbackData->height;

    return camera;
}
//////////////////////////////////////////////////////////////////////////
static void __ae_movie_callback_camera_deleter( const aeMovieCameraDeleterCallbackData * _callbackData, ae_voidptr_t _data )
{
    (void)_data;

    ae_camera_t * camera = (ae_camera_t *)_callbackData->element;

    free( camera );
}
//////////////////////////////////////////////////////////////////////////
static void __ae_movie_callback_camera_update( const aeMovieCameraUpdateCallbackData * _callbackData, void * _data )
{
    (void)_data;

    ae_camera_t * camera = (ae_camera_t *)_callbackData->element;

    __make_lookat_m4( camera->view, _callbackData->position, _callbackData->target );
}
//////////////////////////////////////////////////////////////////////////
typedef struct em_parameter_color_t
{
    GLfloat r;
    GLfloat g;
    GLfloat b;
} em_parameter_color_t;
//////////////////////////////////////////////////////////////////////////
typedef struct em_custom_shader_t
{
    GLuint program_id;

    uint8_t parameter_count;
    int8_t parameter_types[32];
    GLint parameter_locations[32];

    GLfloat parameter_values[32];
    em_parameter_color_t parameter_colors[32];

    GLint positionLocation;
    GLint colorLocation;
    GLint texcoordLocation;

    GLint mvpMatrixLocation;
    GLint tex0Location;
} em_custom_shader_t;
//////////////////////////////////////////////////////////////////////////
static ae_voidptr_t __ae_movie_callback_shader_provider( const aeMovieShaderProviderCallbackData * _callbackData, ae_voidptr_t _data )
{
    em_custom_shader_t * shader = EM_NEW( em_custom_shader_t );

    GLuint program_id = __make_opengl_program( _callbackData->name, _callbackData->version, _callbackData->shader_vertex, _callbackData->shader_fragment );

    if( program_id == 0U )
    {
        return em_nullptr;
    }

    shader->program_id = program_id;

    int positionLocation;
    GLCALLR( positionLocation, glGetAttribLocation, (program_id, "a_position") );

    emscripten_log( EM_LOG_CONSOLE, "opengl attrib inVert '%d'\n"
        , positionLocation
    );

    shader->positionLocation = positionLocation;

    int colorLocation;
    GLCALLR( colorLocation, glGetAttribLocation, (program_id, "a_color") );

    emscripten_log( EM_LOG_CONSOLE, "opengl attrib inCol '%d'\n"
        , colorLocation
    );

    shader->colorLocation = colorLocation;

    int texcoordLocation;
    GLCALLR( texcoordLocation, glGetAttribLocation, (program_id, "a_texcoord") );

    emscripten_log( EM_LOG_CONSOLE, "opengl attrib inUV '%d'\n"
        , texcoordLocation
    );

    shader->texcoordLocation = texcoordLocation;

    shader->parameter_count = _callbackData->parameter_count;

    for( uint32_t i = 0; i != _callbackData->parameter_count; ++i )
    {
        const char * parameter_name = _callbackData->parameter_names[i];
        const char * parameter_uniform = _callbackData->parameter_uniforms[i];
        uint8_t parameter_type = _callbackData->parameter_types[i];

        shader->parameter_types[i] = parameter_type;

        GLint parameter_location;
        GLCALLR( parameter_location, glGetUniformLocation, (program_id, parameter_uniform) );

        shader->parameter_locations[i] = parameter_location;

        emscripten_log( EM_LOG_CONSOLE, "opengl attrib '%s' uniform '%s' location '%d'\n"
            , parameter_name
            , parameter_uniform
            , parameter_location
        );
    }

    int mvpMatrixLocation;
    GLCALLR( mvpMatrixLocation, glGetUniformLocation, (program_id, "g_mvpMatrix") );

    emscripten_log( EM_LOG_CONSOLE, "opengl uniform mvpMatrix '%d'\n"
        , mvpMatrixLocation
    );

    shader->mvpMatrixLocation = mvpMatrixLocation;

    int tex0Location;
    GLCALLR( tex0Location, glGetUniformLocation, (program_id, "g_tex0") );

    emscripten_log( EM_LOG_CONSOLE, "opengl uniform tex0 '%d'\n"
        , tex0Location
    );

    shader->tex0Location = tex0Location;

    emscripten_log( EM_LOG_CONSOLE, "opengl create program '%s' version '%d' id '%d'\n"
        , _callbackData->name
        , _callbackData->version
        , program_id
    );

    return shader;
}
//////////////////////////////////////////////////////////////////////////
static void __ae_movie_callback_shader_property_update( const aeMovieShaderPropertyUpdateCallbackData * _callbackData, ae_voidptr_t _data )
{
    em_custom_shader_t * shader = (em_custom_shader_t *)_callbackData->element;

    uint8_t index = _callbackData->index;

    GLint location = shader->parameter_locations[index];

    switch( _callbackData->type )
    {
    case 3:
        {
            shader->parameter_values[index] = _callbackData->value;
        }break;
    case 5:
        {
            em_parameter_color_t c;
            c.r = _callbackData->color_r;
            c.g = _callbackData->color_g;
            c.b = _callbackData->color_b;

            shader->parameter_colors[index] = c;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
static void __ae_movie_callback_shader_deleter( const aeMovieShaderDeleterCallbackData * _callbackData, ae_voidptr_t _data )
{
    free( _callbackData->element );
}
//////////////////////////////////////////////////////////////////////////
typedef struct em_track_matte_t
{
    float matrix[16];
    aeMovieRenderMesh mesh;
} em_track_matte_t;
//////////////////////////////////////////////////////////////////////////
static void * __ae_movie_callback_track_matte_provider( const aeMovieTrackMatteProviderCallbackData * _callbackData, void * _data )
{
    (void)_data;

    em_track_matte_t * track_matte = EM_NEW( em_track_matte_t );

    __copy_m4( track_matte->matrix, _callbackData->matrix );
    track_matte->mesh = *_callbackData->mesh;

    return track_matte;
}
//////////////////////////////////////////////////////////////////////////
static void __ae_movie_callback_track_matte_update( const aeMovieTrackMatteUpdateCallbackData * _callbackData, void * _data )
{
    (void)_data;

    em_track_matte_t * track_matte = (em_track_matte_t *)_callbackData->track_matte_data;

    switch( _callbackData->state )
    {
    case AE_MOVIE_NODE_UPDATE_BEGIN:
        {
            __copy_m4( track_matte->matrix, _callbackData->matrix );
            track_matte->mesh = *_callbackData->mesh;
        }break;
    case AE_MOVIE_NODE_UPDATE_UPDATE:
        {
            __copy_m4( track_matte->matrix, _callbackData->matrix );
            track_matte->mesh = *_callbackData->mesh;
        }break;
    }
}
//////////////////////////////////////////////////////////////////////////
static void __ae_movie_callback_track_matte_deleter( const aeMovieTrackMatteDeleterCallbackData * _callbackData, void * _data )
{
    (void)_data;

    em_track_matte_t * track_matte = (em_track_matte_t *)_callbackData->element;

    free( track_matte );
}
//////////////////////////////////////////////////////////////////////////
em_movie_composition_handle_t em_create_movie_composition( em_movie_data_handle_t _movieData, const char * _name )
{
    aeMovieData * ae_movie_data = (aeMovieData *)_movieData;

    const ae_char_t * movieName = ae_get_movie_name( ae_movie_data );

    const aeMovieCompositionData * movieCompositionData = ae_get_movie_composition_data( ae_movie_data, _name );

    if( movieCompositionData == AE_NULL )
    {
        emscripten_log( EM_LOG_CONSOLE, "error get movie '%s' composition data '%s'"
            , movieName
            , _name
        );

        return AE_NULL;
    }

    aeMovieCompositionProviders providers;
    ae_initialize_movie_composition_providers( &providers );

    providers.node_provider = &__ae_movie_composition_node_provider;
    providers.node_deleter = &__ae_movie_composition_node_deleter;
    //providers.node_update = &ae_movie_composition_node_update;

    providers.camera_provider = &__ae_movie_callback_camera_provider;
    providers.camera_deleter = &__ae_movie_callback_camera_deleter;
    providers.camera_update = &__ae_movie_callback_camera_update;

    providers.shader_provider = &__ae_movie_callback_shader_provider;
    providers.shader_deleter = &__ae_movie_callback_shader_deleter;
    providers.shader_property_update = &__ae_movie_callback_shader_property_update;

    providers.track_matte_provider = &__ae_movie_callback_track_matte_provider;
    providers.track_matte_update = &__ae_movie_callback_track_matte_update;
    providers.track_matte_deleter = &__ae_movie_callback_track_matte_deleter;

    aeMovieComposition * movieComposition = ae_create_movie_composition( ae_movie_data, movieCompositionData, AE_TRUE, &providers, AE_NULL );

    if( movieComposition == AE_NULL )
    {
        emscripten_log( EM_LOG_CONSOLE, "error create movie '%s' composition '%s'"
            , movieName
            , _name
        );

        return AE_NULL;
    }

    emscripten_log( EM_LOG_CONSOLE, "successful create movie '%s' composition '%s'"
        , movieName
        , _name
    );

    return movieComposition;
}
//////////////////////////////////////////////////////////////////////////
void em_delete_movie_composition( em_movie_composition_handle_t _movieComposition )
{
    const aeMovieComposition * ae_movie_composition = (const aeMovieComposition *)_movieComposition;

    const ae_char_t * movieName = ae_get_movie_composition_name( ae_movie_composition );

    emscripten_log( EM_LOG_CONSOLE, "successful delete movie composition '%s'"
        , movieName
    );

    ae_delete_movie_composition( ae_movie_composition );

}
//////////////////////////////////////////////////////////////////////////
void em_set_movie_composition_loop( em_movie_composition_handle_t _movieComposition, unsigned int _loop )
{
    const aeMovieComposition * ae_movie_composition = (const aeMovieComposition *)_movieComposition;

    ae_set_movie_composition_loop( ae_movie_composition, _loop );

    const ae_char_t * movieName = ae_get_movie_composition_name( ae_movie_composition );

    emscripten_log( EM_LOG_CONSOLE, "successful set movie composition '%s' loop '%u'"
        , movieName
        , _loop
    );
}
//////////////////////////////////////////////////////////////////////////
void em_play_movie_composition( em_movie_composition_handle_t _movieComposition, float _time )
{
    aeMovieComposition * ae_movie_composition = (aeMovieComposition *)_movieComposition;

    ae_play_movie_composition( ae_movie_composition, _time );

    const ae_char_t * movieName = ae_get_movie_composition_name( ae_movie_composition );

    emscripten_log( EM_LOG_CONSOLE, "successful play movie composition '%s' time '%f'"
        , movieName
        , _time
    );
}
//////////////////////////////////////////////////////////////////////////
void em_update_movie_composition( em_movie_composition_handle_t _movieComposition, float _time )
{
    aeMovieComposition * ae_movie_composition = (aeMovieComposition *)_movieComposition;

    ae_update_movie_composition( ae_movie_composition, _time * 1000.f );

    const ae_char_t * movieName = ae_get_movie_composition_name( ae_movie_composition );

    //emscripten_log( EM_LOG_CONSOLE, "successful update movie composition '%s' time '%f'"
    //    , movieName
    //    , _time
    //);
}
//////////////////////////////////////////////////////////////////////////
void em_render_movie_composition( em_player_handle_t _player, em_movie_composition_handle_t _movieComposition )
{
    em_player_t * player = (em_player_t *)_player;

    const aeMovieComposition * ae_movie_composition = (const aeMovieComposition *)_movieComposition;

    GLCALL( glDisable, (GL_CULL_FACE) );
    GLCALL( glEnable, (GL_BLEND) );

    uint32_t mesh_iterator = 0;

    aeMovieRenderMesh mesh;
    while( ae_compute_movie_mesh( ae_movie_composition, &mesh_iterator, &mesh ) == AE_TRUE )
    {
        switch( mesh.blend_mode )
        {
        case AE_MOVIE_BLEND_ADD:
            {
                GLCALL( glBlendFunc, (GL_SRC_ALPHA, GL_ONE) );
            }break;
        case AE_MOVIE_BLEND_NORMAL:
            {
                GLCALL( glBlendFunc, (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) );
            }break;
        case AE_MOVIE_BLEND_ALPHA_ADD:
        case AE_MOVIE_BLEND_CLASSIC_COLOR_BURN:
        case AE_MOVIE_BLEND_CLASSIC_COLOR_DODGE:
        case AE_MOVIE_BLEND_CLASSIC_DIFFERENCE:
        case AE_MOVIE_BLEND_COLOR:
        case AE_MOVIE_BLEND_COLOR_BURN:
        case AE_MOVIE_BLEND_COLOR_DODGE:
        case AE_MOVIE_BLEND_DANCING_DISSOLVE:
        case AE_MOVIE_BLEND_DARKEN:
        case AE_MOVIE_BLEND_DARKER_COLOR:
        case AE_MOVIE_BLEND_DIFFERENCE:
        case AE_MOVIE_BLEND_DISSOLVE:
        case AE_MOVIE_BLEND_EXCLUSION:
        case AE_MOVIE_BLEND_HARD_LIGHT:
        case AE_MOVIE_BLEND_HARD_MIX:
        case AE_MOVIE_BLEND_HUE:
        case AE_MOVIE_BLEND_LIGHTEN:
        case AE_MOVIE_BLEND_LIGHTER_COLOR:
        case AE_MOVIE_BLEND_LINEAR_BURN:
        case AE_MOVIE_BLEND_LINEAR_DODGE:
        case AE_MOVIE_BLEND_LINEAR_LIGHT:
        case AE_MOVIE_BLEND_LUMINESCENT_PREMUL:
        case AE_MOVIE_BLEND_LUMINOSITY:
        case AE_MOVIE_BLEND_MULTIPLY:
        case AE_MOVIE_BLEND_OVERLAY:
        case AE_MOVIE_BLEND_PIN_LIGHT:
        case AE_MOVIE_BLEND_SATURATION:
        case AE_MOVIE_BLEND_SCREEN:
        case AE_MOVIE_BLEND_SILHOUETE_ALPHA:
        case AE_MOVIE_BLEND_SILHOUETTE_LUMA:
        case AE_MOVIE_BLEND_SOFT_LIGHT:
        case AE_MOVIE_BLEND_STENCIL_ALPHA:
        case AE_MOVIE_BLEND_STENCIL_LUMA:
        case AE_MOVIE_BLEND_VIVID_LIGHT:
            {

            }break;
        }

        if( mesh.track_matte_data == em_nullptr )
        {
            size_t opengl_indices_buffer_size = mesh.indexCount * sizeof( em_render_index_t );
            size_t opengl_vertex_buffer_size = mesh.vertexCount * sizeof( em_blend_render_vertex_t );

            GLuint texture_id = 0U;

            em_blend_render_vertex_t * vertices = player->blend_vertices;
            const ae_uint16_t * indices = mesh.indices;

            switch( mesh.layer_type )
            {
            case AE_MOVIE_LAYER_TYPE_SHAPE:
            case AE_MOVIE_LAYER_TYPE_SOLID:
                {
                    uint32_t color = __make_argb( mesh.r, mesh.g, mesh.b, mesh.a );

                    for( uint32_t index = 0; index != mesh.vertexCount; ++index )
                    {
                        em_blend_render_vertex_t * v = vertices + index;

                        const float * mesh_position = mesh.position[index];

                        v->position[0] = mesh_position[0];
                        v->position[1] = mesh_position[1];
                        v->position[2] = mesh_position[2];

                        v->color = color;

                        const float * mesh_uv = mesh.uv[index];

                        v->uv[0] = mesh_uv[0];
                        v->uv[1] = mesh_uv[1];
                    }
                }break;
            case AE_MOVIE_LAYER_TYPE_SEQUENCE:
            case AE_MOVIE_LAYER_TYPE_IMAGE:
                {
                    const em_resource_image_t * resource_image = (const em_resource_image_t *)mesh.resource_data;

                    texture_id = resource_image->texture_id;

                    uint32_t color = __make_argb( mesh.r, mesh.g, mesh.b, mesh.a );

                    for( uint32_t index = 0; index != mesh.vertexCount; ++index )
                    {
                        em_blend_render_vertex_t * v = vertices + index;

                        const float * mesh_position = mesh.position[index];

                        v->position[0] = mesh_position[0];
                        v->position[1] = mesh_position[1];
                        v->position[2] = mesh_position[2];

                        v->color = color;

                        const float * mesh_uv = mesh.uv[index];

                        v->uv[0] = mesh_uv[0];
                        v->uv[1] = mesh_uv[1];
                    }
                }break;
            case AE_MOVIE_LAYER_TYPE_VIDEO:
                {
                }break;
            }

            GLuint opengl_vertex_buffer_id = 0;
            GLCALL( glGenBuffers, (1, &opengl_vertex_buffer_id) );
            GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, opengl_vertex_buffer_id) );
            GLCALL( glBufferData, (GL_ARRAY_BUFFER, opengl_vertex_buffer_size, vertices, GL_STREAM_DRAW) );

            GLint positionLocation;
            GLint colorLocation;
            GLint texcoordLocation;
            GLint mvpMatrixLocation;
            GLint tex0Location;

            GLuint program_id;

            if( mesh.shader_data == AE_NULL )
            {
                const em_blend_shader_t * blend_shader = player->blend_shader;

                program_id = blend_shader->program_id;

                GLCALL( glUseProgram, (blend_shader->program_id) );

                positionLocation = blend_shader->positionLocation;
                colorLocation = blend_shader->colorLocation;
                texcoordLocation = blend_shader->texcoordLocation;
                mvpMatrixLocation = blend_shader->mvpMatrixLocation;
                tex0Location = blend_shader->tex0Location;
            }
            else
            {
                em_custom_shader_t * shader = (em_custom_shader_t *)mesh.shader_data;

                program_id = shader->program_id;

                GLCALL( glUseProgram, (shader->program_id) );

                uint8_t parameter_count = shader->parameter_count;
                for( uint8_t i = 0; i != parameter_count; ++i )
                {
                    GLint parameter_location = shader->parameter_locations[i];

                    uint8_t parameter_type = shader->parameter_types[i];

                    switch( parameter_type )
                    {
                    case 3:
                        {
                            float value = shader->parameter_values[i];
                            GLCALL( glUniform1f, (parameter_location, value) );
                        }break;
                    case 5:
                        {
                            em_parameter_color_t * c = shader->parameter_colors + i;
                            GLCALL( glUniform3f, (parameter_location, c->r, c->g, c->b) );
                        }break;
                    }
                }

                positionLocation = shader->positionLocation;
                colorLocation = shader->colorLocation;
                texcoordLocation = shader->texcoordLocation;
                mvpMatrixLocation = shader->mvpMatrixLocation;
                tex0Location = shader->tex0Location;
            }

            GLCALL( glActiveTexture, (GL_TEXTURE0) );

            if( texture_id != 0U )
            {
                GLCALL( glBindTexture, (GL_TEXTURE_2D, texture_id) );

                GLCALL( glUniform1i, (tex0Location, 0) );
            }
            else
            {
                GLCALL( glBindTexture, (GL_TEXTURE_2D, 0U) );
            }

            GLCALL( glEnableVertexAttribArray, (positionLocation) );
            GLCALL( glEnableVertexAttribArray, (colorLocation) );
            GLCALL( glEnableVertexAttribArray, (texcoordLocation) );

            GLCALL( glVertexAttribPointer, (positionLocation, 3, GL_FLOAT, GL_FALSE, sizeof( em_blend_render_vertex_t ), (const GLvoid *)offsetof( em_blend_render_vertex_t, position )) );
            GLCALL( glVertexAttribPointer, (colorLocation, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof( em_blend_render_vertex_t ), (const GLvoid *)offsetof( em_blend_render_vertex_t, color )) );
            GLCALL( glVertexAttribPointer, (texcoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof( em_blend_render_vertex_t ), (const GLvoid *)offsetof( em_blend_render_vertex_t, uv )) );

            if( mesh.camera_data == AE_NULL )
            {
                const aeMovieCompositionData * ae_movie_composition_data = ae_get_movie_composition_composition_data( ae_movie_composition );

                float viewMatrix[16];
                __identity_m4( viewMatrix );

                float composition_width = ae_get_movie_composition_data_width( ae_movie_composition_data );
                float composition_height = ae_get_movie_composition_data_width( ae_movie_composition_data );

                float projectionMatrix[16];
                __make_orthogonal_m4( projectionMatrix, composition_width, composition_height );

                float projectionViewMatrix[16];
                __mul_m4_m4( projectionViewMatrix, projectionMatrix, viewMatrix );

                GLCALL( glUniformMatrix4fv, (mvpMatrixLocation, 1, GL_FALSE, projectionViewMatrix) );

                glViewport( 0, 0, (GLsizei)composition_width, (GLsizei)composition_height );
            }
            else
            {
                ae_camera_t * camera = (ae_camera_t *)mesh.camera_data;

                float projectionViewMatrix[16];
                __mul_m4_m4( projectionViewMatrix, camera->projection, camera->view );

                GLCALL( glUniformMatrix4fv, (mvpMatrixLocation, 1, GL_FALSE, projectionViewMatrix) );

                glViewport( 0, 0, (GLsizei)camera->width, (GLsizei)camera->height );
            }

            GLuint opengl_indices_buffer_id = 0;
            GLCALL( glGenBuffers, (1, &opengl_indices_buffer_id) );
            GLCALL( glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, opengl_indices_buffer_id) );
            GLCALL( glBufferData, (GL_ELEMENT_ARRAY_BUFFER, opengl_indices_buffer_size, indices, GL_STREAM_DRAW) );

            GLCALL( glDrawElements, (GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_SHORT, 0) );

            GLCALL( glDisableVertexAttribArray, (positionLocation) );
            GLCALL( glDisableVertexAttribArray, (colorLocation) );
            GLCALL( glDisableVertexAttribArray, (texcoordLocation) );

            GLCALL( glActiveTexture, (GL_TEXTURE0) );
            GLCALL( glBindTexture, (GL_TEXTURE_2D, 0) );

            GLCALL( glUseProgram, (0) );

            GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, 0) );
            GLCALL( glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, 0) );

            GLCALL( glDeleteBuffers, (1, &opengl_indices_buffer_id) );
            GLCALL( glDeleteBuffers, (1, &opengl_vertex_buffer_id) );
        }
        else
        {
            size_t opengl_indices_buffer_size = mesh.indexCount * sizeof( em_render_index_t );
            size_t opengl_vertex_buffer_size = mesh.vertexCount * sizeof( em_track_matte_render_vertex_t );

            em_node_track_matte_t * node_track_matte = (em_node_track_matte_t *)mesh.element_data;

            GLuint base_texture_id = node_track_matte->base_image->texture_id;
            GLuint track_matte_texture_id = node_track_matte->track_matte_image->texture_id;

            em_track_matte_render_vertex_t * vertices = player->track_matte_vertices;
            const ae_uint16_t * indices = mesh.indices;

            switch( mesh.layer_type )
            {
            case AE_MOVIE_LAYER_TYPE_SEQUENCE:
            case AE_MOVIE_LAYER_TYPE_IMAGE:
                {
                    const em_track_matte_t * track_matte = (const em_track_matte_t *)mesh.track_matte_data;

                    const aeMovieRenderMesh * track_matte_mesh = &track_matte->mesh;

                    uint32_t color = __make_argb( mesh.r, mesh.g, mesh.b, mesh.a * track_matte_mesh->a );

                    for( uint32_t index = 0; index != mesh.vertexCount; ++index )
                    {
                        em_track_matte_render_vertex_t * v = vertices + index;

                        const float * mesh_position = mesh.position[index];

                        v->position[0] = mesh_position[0];
                        v->position[1] = mesh_position[1];
                        v->position[2] = mesh_position[2];

                        v->color = color;

                        const float * mesh_uv = mesh.uv[index];

                        v->uv0[0] = mesh_uv[0];
                        v->uv0[1] = mesh_uv[1];

                        float uv1[2];
                        __calc_point_uv( uv1
                            , track_matte_mesh->position[0]
                            , track_matte_mesh->position[1]
                            , track_matte_mesh->position[2]
                            , track_matte_mesh->uv[0]
                            , track_matte_mesh->uv[1]
                            , track_matte_mesh->uv[2]
                            , mesh_position );

                        v->uv1[0] = uv1[0];
                        v->uv1[1] = uv1[1];
                    }
                }break;
            case AE_MOVIE_LAYER_TYPE_VIDEO:
                {
                }break;
            }

            GLuint opengl_vertex_buffer_id = 0;
            GLCALL( glGenBuffers, (1, &opengl_vertex_buffer_id) );
            GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, opengl_vertex_buffer_id) );
            GLCALL( glBufferData, (GL_ARRAY_BUFFER, opengl_vertex_buffer_size, vertices, GL_STREAM_DRAW) );

            const em_track_matte_shader_t * track_matte_shader = player->track_matte_shader;

            GLuint program_id = track_matte_shader->program_id;

            GLCALL( glUseProgram, (program_id) );

            GLint positionLocation = track_matte_shader->positionLocation;
            GLint colorLocation = track_matte_shader->colorLocation;
            GLint texcoord0Location = track_matte_shader->texcoord0Location;
            GLint texcoord1Location = track_matte_shader->texcoord1Location;
            GLint mvpMatrixLocation = track_matte_shader->mvpMatrixLocation;
            GLint tex0Location = track_matte_shader->tex0Location;
            GLint tex1Location = track_matte_shader->tex1Location;

            GLCALL( glActiveTexture, (GL_TEXTURE0) );
            GLCALL( glBindTexture, (GL_TEXTURE_2D, base_texture_id) );
            GLCALL( glUniform1i, (tex0Location, 0) );

            GLCALL( glActiveTexture, (GL_TEXTURE1) );
            GLCALL( glBindTexture, (GL_TEXTURE_2D, track_matte_texture_id) );
            GLCALL( glUniform1i, (tex1Location, 1) );

            GLCALL( glEnableVertexAttribArray, (positionLocation) );
            GLCALL( glEnableVertexAttribArray, (colorLocation) );
            GLCALL( glEnableVertexAttribArray, (texcoord0Location) );
            GLCALL( glEnableVertexAttribArray, (texcoord1Location) );

            GLCALL( glVertexAttribPointer, (positionLocation, 3, GL_FLOAT, GL_FALSE, sizeof( em_track_matte_render_vertex_t ), (const GLvoid *)offsetof( em_track_matte_render_vertex_t, position )) );
            GLCALL( glVertexAttribPointer, (colorLocation, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof( em_track_matte_render_vertex_t ), (const GLvoid *)offsetof( em_track_matte_render_vertex_t, color )) );
            GLCALL( glVertexAttribPointer, (texcoord0Location, 2, GL_FLOAT, GL_FALSE, sizeof( em_track_matte_render_vertex_t ), (const GLvoid *)offsetof( em_track_matte_render_vertex_t, uv0 )) );
            GLCALL( glVertexAttribPointer, (texcoord1Location, 2, GL_FLOAT, GL_FALSE, sizeof( em_track_matte_render_vertex_t ), (const GLvoid *)offsetof( em_track_matte_render_vertex_t, uv1 )) );

            if( mesh.camera_data == AE_NULL )
            {
                const aeMovieCompositionData * ae_movie_composition_data = ae_get_movie_composition_composition_data( ae_movie_composition );

                float viewMatrix[16];
                __identity_m4( viewMatrix );

                float composition_width = ae_get_movie_composition_data_width( ae_movie_composition_data );
                float composition_height = ae_get_movie_composition_data_width( ae_movie_composition_data );

                float projectionMatrix[16];
                __make_orthogonal_m4( projectionMatrix, composition_width, composition_height );

                float projectionViewMatrix[16];
                __mul_m4_m4( projectionViewMatrix, projectionMatrix, viewMatrix );

                GLCALL( glUniformMatrix4fv, (mvpMatrixLocation, 1, GL_FALSE, projectionViewMatrix) );

                glViewport( 0, 0, (GLsizei)composition_width, (GLsizei)composition_height );
            }
            else
            {
                ae_camera_t * camera = (ae_camera_t *)mesh.camera_data;

                float projectionViewMatrix[16];
                __mul_m4_m4( projectionViewMatrix, camera->projection, camera->view );

                GLCALL( glUniformMatrix4fv, (mvpMatrixLocation, 1, GL_FALSE, projectionViewMatrix) );

                glViewport( 0, 0, (GLsizei)camera->width, (GLsizei)camera->height );
            }

            GLuint opengl_indices_buffer_id = 0;
            GLCALL( glGenBuffers, (1, &opengl_indices_buffer_id) );
            GLCALL( glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, opengl_indices_buffer_id) );
            GLCALL( glBufferData, (GL_ELEMENT_ARRAY_BUFFER, opengl_indices_buffer_size, indices, GL_STREAM_DRAW) );

            GLCALL( glDrawElements, (GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_SHORT, 0) );

            GLCALL( glDisableVertexAttribArray, (positionLocation) );
            GLCALL( glDisableVertexAttribArray, (colorLocation) );
            GLCALL( glDisableVertexAttribArray, (texcoord0Location) );
            GLCALL( glDisableVertexAttribArray, (texcoord1Location) );

            GLCALL( glActiveTexture, (GL_TEXTURE0) );
            GLCALL( glBindTexture, (GL_TEXTURE_2D, 0) );
            GLCALL( glActiveTexture, (GL_TEXTURE1) );
            GLCALL( glBindTexture, (GL_TEXTURE_2D, 0) );

            GLCALL( glUseProgram, (0) );

            GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, 0) );
            GLCALL( glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, 0) );

            GLCALL( glDeleteBuffers, (1, &opengl_indices_buffer_id) );
            GLCALL( glDeleteBuffers, (1, &opengl_vertex_buffer_id) );
        }

        //emscripten_log( EM_LOG_CONSOLE, "draw elements vertices '%d' indices '%d' program '%d' texture '%d' index '%d'"
        //    , mesh.vertexCount
        //    , mesh.indexCount
        //    , program_id
        //    , texture_id
        //    , mesh_iterator
        //);
    }
}