#include "Composition.h"
#include "ResourcesManager.h"
#include "Sound.h"

#include "Logger.h"

#include <math.h>
#include <string.h>

//////////////////////////////////////////////////////////////////////////
static void MakeOrtho2DMat( float left, float right, float top, float bottom, float zNear, float zFar, float mat[16] )
{
    mat[0] = 2.f / (right - left);
    mat[5] = 2.f / (top - bottom);
    mat[10] = -2.f / (zFar - zNear);
    mat[12] = -(right + left) / (right - left);
    mat[13] = -(top + bottom) / (top - bottom);
    mat[14] = -(zFar + zNear) / (zFar - zNear);
    mat[15] = 1.f;

    mat[1] = mat[2] = mat[3] = mat[4] = mat[6] = mat[7] = mat[8] = mat[9] = mat[11] = 0.f;
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
static float __dot_v3( const float * _a, const float * _b )
{
    return _a[0] * _b[0] + _a[1] * _b[1] + _a[2] * _b[2];
}
//////////////////////////////////////////////////////////////////////////
static void CalcPointUV( float * _out, const float * _a, const float * _b, const float * _c, const float * _auv, const float * _buv, const float * _cuv, const float * _point )
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
//////////////////////////////////////////////////////////////////////////
#define _GL_OFFSET(s, m) reinterpret_cast<const GLvoid*>(&(((s*)0)->m))
#define _GL_SIZE(s) static_cast<GLsizei>(sizeof( s ))
//////////////////////////////////////////////////////////////////////////
static const uint32_t kMaxVerticesToDraw = 4 * 1024;
static const uint32_t kMaxIndicesToDraw = 6 * 1024;
static const float    kSomeSmallFloat = 0.000001f;
//////////////////////////////////////////////////////////////////////////
struct DrawVertex
{
    float pos[3];
    float uv0[2];
    float uv1[2];
    uint32_t color;
};
//////////////////////////////////////////////////////////////////////////
static const GLuint kVertexPosAttribIdx = 0;
static const GLuint kVertexUV0AttribIdx = 1;
static const GLuint kVertexUV1AttribIdx = 2;
static const GLuint kVertexColorAttribIdx = 3;
//////////////////////////////////////////////////////////////////////////
static const GLint  kTextureRGBSlot = 0;
static const GLint  kTextureASlot = 1;
//////////////////////////////////////////////////////////////////////////
static const char* sVertexShader = "#version 330       \n\
layout(location = 0) in vec3 inPos;                    \n\
layout(location = 1) in vec2 inUV0;                    \n\
layout(location = 2) in vec2 inUV1;                    \n\
layout(location = 3) in vec4 inColor;                  \n\
uniform mat4 uWVP;                                     \n\
uniform float uScale;                                  \n\
uniform vec2 uOffset;                                  \n\
out vec2 v2fUV0;                                       \n\
out vec2 v2fUV1;                                       \n\
out vec4 v2fColor;                                     \n\
void main() {                                          \n\
    vec3 p = vec3(inPos.xy + uOffset, 0.0) * uScale;   \n\
    gl_Position = uWVP * vec4(p, 1.0);                 \n\
    v2fUV0 = inUV0;                                    \n\
    v2fUV1 = inUV1;                                    \n\
    v2fColor = inColor;                                \n\
}                                                      \n";
//////////////////////////////////////////////////////////////////////////
static const char* sFragmentShader = "#version 330     \n\
uniform sampler2D uTextureRGB;                         \n\
uniform sampler2D uTextureA;                           \n\
uniform bool uIsPremultAlpha;                          \n\
in vec2 v2fUV0;                                        \n\
in vec2 v2fUV1;                                        \n\
in vec4 v2fColor;                                      \n\
out vec4 oColor;                                       \n\
void main() {                                          \n\
    vec4 texColor = texture(uTextureRGB, v2fUV0);      \n\
    vec4 texAlpha = texture(uTextureA, v2fUV1);        \n\
    oColor = texColor * v2fColor;                      \n\
    if (uIsPremultAlpha) {                             \n\
        oColor.rgb *= texAlpha.a * v2fColor.a;         \n\
        oColor.a *= texAlpha.a;                        \n\
    } else {                                           \n\
        oColor.a *= texAlpha.a * v2fColor.a;           \n\
    }                                                  \n\
}                                                      \n";
//////////////////////////////////////////////////////////////////////////
static const char* sWireVertexShader = "#version 330   \n\
layout(location = 0) in vec3 inPos;                    \n\
layout(location = 3) in vec4 inColor;                  \n\
uniform mat4 uWVP;                                     \n\
uniform float uScale;                                  \n\
uniform vec2 uOffset;                                  \n\
out vec4 v2fColor;                                     \n\
void main() {                                          \n\
    vec3 p = vec3(inPos.xy + uOffset, 0.0) * uScale;   \n\
    gl_Position = uWVP * vec4(p, 1.0);                 \n\
    v2fColor = inColor;                                \n\
}                                                      \n";
//////////////////////////////////////////////////////////////////////////
static const char* sWireFragmentShader = "#version 330 \n\
in vec4 v2fColor;                                      \n\
out vec4 oColor;                                       \n\
void main() {                                          \n\
    oColor = v2fColor;                                 \n\
}                                                      \n";
//////////////////////////////////////////////////////////////////////////
static uint32_t FloatColorToUint( ae_color_t color, ae_color_channel_t alpha )
{
    uint32_t r = static_cast<uint32_t>(::floorf( color.r * 255.5f ));
    uint32_t g = static_cast<uint32_t>(::floorf( color.g * 255.5f ));
    uint32_t b = static_cast<uint32_t>(::floorf( color.b * 255.5f ));
    uint32_t a = static_cast<uint32_t>(::floorf( alpha * 255.5f ));

    return (a << 24) | (b << 16) | (g << 8) | r;
}
//////////////////////////////////////////////////////////////////////////
struct TrackMatteDesc
{
    float matrix[16];
    aeMovieRenderMesh mesh;
    ae_track_matte_mode_t mode;
};
//////////////////////////////////////////////////////////////////////////
Composition::Composition()
    : mComposition( nullptr )
    , mShader( 0 )
    , mWireShader( 0 )
    , mVAO( 0 )
    , mVB( 0 )
    , mIB( 0 )
    , mCurrentTextureRGB( 0 )
    , mCurrentTextureA( 0 )
    , mCurrentBlendMode( BlendMode::Normal )
    , mPremultipliedAlpha( false )
    , mNumVertices( 0 )
    , mNumIndices( 0 )
    , mVerticesData( nullptr )
    , mIndicesData( nullptr )
    , mDrawMode( DrawMode::Solid )
    , mViewportWidth( 1.f )
    , mViewportHeight(1.f )
    , mContentScale( 1.f )
    , mContentOffX( 0.f )
    , mContentOffY( 0.f )
{
}
//////////////////////////////////////////////////////////////////////////
Composition::~Composition()
{
    if( mComposition != nullptr )
    {
        ae_delete_movie_composition( mComposition );
        mComposition = nullptr;
    }

    this->DestroyDrawingData();
}
//////////////////////////////////////////////////////////////////////////
void Composition::SetViewportSize( const float width, const float height )
{
    mViewportWidth = width < kSomeSmallFloat ? kSomeSmallFloat : width;
    mViewportHeight = height < kSomeSmallFloat ? kSomeSmallFloat : height;

    const float left = 0.f;
    const float right = mViewportWidth;
    const float bottom = mViewportHeight;
    const float top = 0.f;
    const float zNear = -1.f;
    const float zFar = 1.f;

    float projOrtho[16];
    MakeOrtho2DMat( left, right, top, bottom, zNear, zFar, projOrtho );

    if( mShader )
    {
        glUseProgram( mShader );
        GLint mvpLoc = glGetUniformLocation( mShader, "uWVP" );
        if( mvpLoc >= 0 )
        {
            glUniformMatrix4fv( mvpLoc, 1, GL_FALSE, projOrtho );
        }
    }

    if( mWireShader )
    {
        glUseProgram( mWireShader );
        GLint mvpLoc = glGetUniformLocation( mWireShader, "uWVP" );
        if( mvpLoc >= 0 )
        {
            glUniformMatrix4fv( mvpLoc, 1, GL_FALSE, projOrtho );
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void Composition::SetContentScale( const float scale )
{
    mContentScale = scale < kSomeSmallFloat ? kSomeSmallFloat : scale;

    if( mShader )
    {
        glUseProgram( mShader );
        GLint scaleLoc = glGetUniformLocation( mShader, "uScale" );
        if( scaleLoc >= 0 )
        {
            glUniform1f( scaleLoc, mContentScale );
        }
    }

    if( mWireShader )
    {
        glUseProgram( mWireShader );
        GLint scaleLoc = glGetUniformLocation( mWireShader, "uScale" );
        if( scaleLoc >= 0 )
        {
            glUniform1f( scaleLoc, mContentScale );
        }
    }
}
//////////////////////////////////////////////////////////////////////////
float Composition::GetContentScale() const
{
    return mContentScale;
}
//////////////////////////////////////////////////////////////////////////
void Composition::SetContentOffset( const float offX, const float offY )
{
    mContentOffX = offX;
    mContentOffY = offY;

    if( mShader )
    {
        glUseProgram( mShader );
        GLint offLoc = glGetUniformLocation( mShader, "uOffset" );
        if( offLoc >= 0 )
        {
            glUniform2f( offLoc, mContentOffX, mContentOffY );
        }
    }

    if( mWireShader )
    {
        glUseProgram( mWireShader );
        GLint offLoc = glGetUniformLocation( mWireShader, "uOffset" );
        if( offLoc >= 0 )
        {
            glUniform2f( offLoc, mContentOffX, mContentOffY );
        }
    }
}
//////////////////////////////////////////////////////////////////////////
float Composition::GetWidth() const
{
    if( mComposition )
    {
        const aeMovieCompositionData* data = ae_get_movie_composition_composition_data( mComposition );
        return ae_get_movie_composition_data_width( data );
    }
    else
    {
        return 0.f;
    }
}
//////////////////////////////////////////////////////////////////////////
float Composition::GetHeight() const
{
    if( mComposition )
    {
        const aeMovieCompositionData* data = ae_get_movie_composition_composition_data( mComposition );
        return ae_get_movie_composition_data_height( data );
    }
    else
    {
        return 0.f;
    }
}
//////////////////////////////////////////////////////////////////////////
const std::string & Composition::GetName() const
{
    return mName;
}
//////////////////////////////////////////////////////////////////////////
float Composition::GetDuration() const
{
    if( mComposition == nullptr )
    {
        return 0.f;
    }

    return ae_get_movie_composition_duration( mComposition );
}
//////////////////////////////////////////////////////////////////////////
float Composition::GetCurrentPlayTime() const
{
    if( mComposition == nullptr )
    {
        return 0.f;
    }

    return ae_get_movie_composition_time( mComposition );
}
//////////////////////////////////////////////////////////////////////////
void Composition::SetCurrentPlayTime( float time ) const 
{
    if( mComposition == nullptr )
    {
        return;
    }

    ae_set_movie_composition_time( mComposition, time );
}
//////////////////////////////////////////////////////////////////////////
bool Composition::IsPlaying() const 
{
    if( mComposition == nullptr )
    {
        return false;
    }

    if( ae_is_play_movie_composition( mComposition ) == AE_FALSE )
    {
        return false;
    }

    return true;
}
//////////////////////////////////////////////////////////////////////////
void Composition::Play( const float startTime )
{
    if( mComposition == nullptr )
    {
        return;
    }

    if( this->IsPaused() == true )
    {
        ae_resume_movie_composition( mComposition );
    }
    else if( this->IsPlaying() == false )
    {
        ae_play_movie_composition( mComposition, startTime );
    }
}
//////////////////////////////////////////////////////////////////////////
void Composition::Pause()
{
    if( mComposition == nullptr )
    {
        return;
    }

    ae_pause_movie_composition( mComposition );
}
//////////////////////////////////////////////////////////////////////////
bool Composition::IsPaused() const
{
    if( mComposition == nullptr )
    {
        return false;
    }

    if( ae_is_pause_movie_composition( mComposition ) == AE_FALSE )
    {
        return false;
    }

    return true;
}
//////////////////////////////////////////////////////////////////////////
void Composition::Stop()
{
    if( mComposition == nullptr )
    {
        return;
    }

    ae_stop_movie_composition( mComposition );
}
//////////////////////////////////////////////////////////////////////////
void Composition::SetLoop( bool toLoop )
{
    if( mComposition == nullptr )
    {
        return;
    }

    ae_set_movie_composition_loop( mComposition, toLoop ? AE_TRUE : AE_FALSE );
}
//////////////////////////////////////////////////////////////////////////
bool Composition::IsLooped() const
{
    if( mComposition == nullptr )
    {
        return false;
    }

    if( ae_get_movie_composition_loop( mComposition ) == AE_FALSE )
    {
        return false;
    }

    return true;
}
//////////////////////////////////////////////////////////////////////////
bool Composition::IsEndedPlay() const
{
    if( mComposition == nullptr )
    {
        return true;
    }

    if( this->IsLooped() == true )
    {
        return false;
    }

    if( this->IsPlaying() || this->IsPaused() )
    {
        return false;
    }

    return true;
}
//////////////////////////////////////////////////////////////////////////
void Composition::Update( float deltaTime )
{
    if( mComposition == nullptr )
    {
        return;
    }

    ae_update_movie_composition( mComposition, deltaTime );
}
//////////////////////////////////////////////////////////////////////////
void Composition::Draw( const DrawMode mode )
{
    if( mComposition == nullptr )
    {
        return;
    }

    mDrawMode = mode;

    float alternativeUV[1024];

    this->BeginDraw();

    ae_uint32_t render_mesh_it = 0;

    aeMovieRenderMesh render_mesh;
    while( ae_compute_movie_mesh( mComposition, &render_mesh_it, &render_mesh ) == AE_TRUE )
    {
        if( render_mesh.track_matte_userdata == AE_NULLPTR )
        {
            switch( render_mesh.layer_type )
            {
            case AE_MOVIE_LAYER_TYPE_SHAPE:
            case AE_MOVIE_LAYER_TYPE_SOLID:
                {
                    if( render_mesh.vertexCount && render_mesh.indexCount )
                    {
                        this->DrawMesh( &render_mesh, nullptr, nullptr, nullptr );
                    }
                } break;
            case AE_MOVIE_LAYER_TYPE_SEQUENCE:
            case AE_MOVIE_LAYER_TYPE_IMAGE:
                {
                    if( render_mesh.vertexCount && render_mesh.indexCount )
                    {
                        ResourceImage* imageRes = reinterpret_cast<ResourceImage*>(render_mesh.resource_userdata);
                        this->DrawMesh( &render_mesh, imageRes, nullptr, nullptr );
                    }
                } break;
            default:
                {
                }break;
            }
        }
        else
        {
            switch( render_mesh.layer_type )
            {
            case AE_MOVIE_LAYER_TYPE_SEQUENCE:
            case AE_MOVIE_LAYER_TYPE_IMAGE:
                {
                    if( render_mesh.element_userdata && render_mesh.vertexCount )
                    {
                        const TrackMatteDesc* track_matte_desc = reinterpret_cast<const TrackMatteDesc*>(render_mesh.track_matte_userdata);
                        const aeMovieRenderMesh& track_matte_mesh = track_matte_desc->mesh;

                        ResourceImage* matteImageRes = reinterpret_cast<ResourceImage*>(render_mesh.element_userdata);
                        ResourceImage* imageRes = reinterpret_cast<ResourceImage*>(render_mesh.resource_userdata);

                        for( ae_uint32_t i = 0; i != track_matte_mesh.vertexCount; ++i )
                        {
                            const float* mesh_position = track_matte_mesh.position[i];

                            CalcPointUV( &alternativeUV[i * 2],
                                render_mesh.position[0],
                                render_mesh.position[1],
                                render_mesh.position[2],
                                render_mesh.uv[0],
                                render_mesh.uv[1],
                                render_mesh.uv[2],
                                mesh_position );
                        }

                        this->DrawMesh( &track_matte_mesh, matteImageRes, imageRes, alternativeUV );
                    }
                } break;
            default:
                {
                }break;
            }
        }
    }

    EndDraw();
}
//////////////////////////////////////////////////////////////////////////
uint32_t Composition::GetNumSubCompositions() const
{
    uint32_t size = static_cast<uint32_t>(mSubCompositions.size());

    return size;
}
//////////////////////////////////////////////////////////////////////////
const char * Composition::GetSubCompositionName( uint32_t idx ) const
{
    const aeMovieSubComposition* subcomposition = mSubCompositions[idx];

    const char * name = ae_get_movie_sub_composition_name( subcomposition );

    return name;
}
//////////////////////////////////////////////////////////////////////////
void Composition::PlaySubComposition( uint32_t idx )
{
    VectorMovieSubComposition::size_type size = mSubCompositions.size();

    if( idx >= size )
    {
        return;
    }

    const aeMovieSubComposition* subComposition = mSubCompositions[idx];

    if( ae_is_pause_movie_sub_composition( subComposition ) == AE_TRUE )
    {
        ae_resume_movie_sub_composition( mComposition, subComposition );
    }
    else
    {
        ae_play_movie_sub_composition( mComposition, subComposition, 0.f );
    }
}
//////////////////////////////////////////////////////////////////////////
void Composition::PauseSubComposition( uint32_t idx )
{
    VectorMovieSubComposition::size_type size = mSubCompositions.size();

    if( idx >= size )
    {
        return;
    }

    const aeMovieSubComposition* subComposition = mSubCompositions[idx];

    ae_pause_movie_sub_composition( mComposition, subComposition );
}
//////////////////////////////////////////////////////////////////////////
void Composition::StopSubComposition( uint32_t idx )
{
    VectorMovieSubComposition::size_type size = mSubCompositions.size();

    if( idx >= size )
    {
        return;
    }

    const aeMovieSubComposition* subComposition = mSubCompositions[idx];

    ae_stop_movie_sub_composition( mComposition, subComposition );
}
//////////////////////////////////////////////////////////////////////////
void Composition::SetTimeSubComposition( uint32_t idx, float time )
{
    VectorMovieSubComposition::size_type size = mSubCompositions.size();

    if( idx >= size )
    {
        return;
    }

    const aeMovieSubComposition* subComposition = mSubCompositions[idx];

    ae_set_movie_sub_composition_time( mComposition, subComposition, static_cast<ae_time_t>(time) );
}
//////////////////////////////////////////////////////////////////////////
void Composition::SetLoopSubComposition( uint32_t idx, bool toLoop ) {
    VectorMovieSubComposition::size_type size = mSubCompositions.size();

    if( idx >= size )
    {
        return;
    }

    const aeMovieSubComposition* subComposition = mSubCompositions[idx];

    ae_set_movie_sub_composition_loop( subComposition, toLoop ? AE_TRUE : AE_FALSE );
}
//////////////////////////////////////////////////////////////////////////
bool Composition::IsLoopedSubComposition( uint32_t idx ) const
{
    VectorMovieSubComposition::size_type size = mSubCompositions.size();

    if( idx >= size )
    {
        return false;
    }

    const aeMovieSubComposition* subComposition = mSubCompositions[idx];

    if( ae_get_movie_sub_composition_loop( subComposition ) == AE_FALSE )
    {
        return false;
    }

    return true;
}
//////////////////////////////////////////////////////////////////////////
bool Composition::Create( const aeMovieData* moviewData, const aeMovieCompositionData* compData )
{
    aeMovieCompositionProviders providers;
    ae_clear_movie_composition_providers( &providers );

    providers.node_provider = []( const aeMovieNodeProviderCallbackData* _callbackData, ae_voidptrptr_t _nd, ae_voidptr_t _ud ) -> ae_bool_t
    {
        Composition* _this = reinterpret_cast<Composition*>(_ud);

        if( _this == nullptr )
        {
            return AE_FALSE;
        }

        if( _this->OnProvideNode( _callbackData, _nd ) == false )
        {
            return AE_FALSE;
        }

        return AE_TRUE;
    };

    providers.node_deleter = []( const aeMovieNodeDeleterCallbackData* _callbackData, ae_voidptr_t _ud )
    {
        Composition* _this = reinterpret_cast<Composition*>(_ud);

        if( _this == nullptr )
        {
            return;
        }

        _this->OnDeleteNode( _callbackData );
    };

    providers.node_update = []( const aeMovieNodeUpdateCallbackData* _callbackData, ae_voidptr_t _ud )
    {
        Composition* _this = reinterpret_cast<Composition*>(_ud);

        if( _this == nullptr )
        {
            return;
        }

        _this->OnUpdateNode( _callbackData );
    };

    providers.camera_provider = []( const aeMovieCameraProviderCallbackData* _callbackData, ae_voidptrptr_t _cd, ae_voidptr_t _ud ) -> ae_bool_t
    {
        Composition* _this = reinterpret_cast<Composition*>(_ud);

        if( _this == nullptr )
        {
            return AE_FALSE;
        }

        if( _this->OnProvideCamera( _callbackData, _cd ) == false )
        {
            return AE_FALSE;
        }

        return AE_TRUE;
    };

    providers.track_matte_provider = []( const aeMovieTrackMatteProviderCallbackData * _callbackData, ae_voidptrptr_t _tmd, ae_voidptr_t _ud ) -> ae_bool_t
    {
        Composition* _this = reinterpret_cast<Composition*>(_ud);

        if( _this == nullptr )
        {
            return AE_FALSE;
        }

        if( _this->OnProvideTrackMatte( _callbackData, _tmd ) == false )
        {
            return AE_FALSE;
        }

        return AE_TRUE;
    };

    providers.track_matte_update = []( const aeMovieTrackMatteUpdateCallbackData* _callbackData, ae_voidptr_t _ud )
    {
        Composition* _this = reinterpret_cast<Composition*>(_ud);

        if( _this == nullptr )
        {
            return;
        }

        _this->OnUpdateTrackMatte( _callbackData );
    };

    providers.track_matte_deleter = []( const aeMovieTrackMatteDeleterCallbackData * _callbackData, ae_voidptr_t _ud )
    {
        Composition* _this = reinterpret_cast<Composition*>(_ud);

        if( _this == nullptr )
        {
            return;
        }

        _this->OnDeleteTrackMatte( _callbackData );
    };

    providers.composition_event = []( const aeMovieCompositionEventCallbackData* _callbackData, ae_voidptr_t _ud )
    {
        Composition* _this = reinterpret_cast<Composition*>(_ud);

        if( _this == nullptr )
        {
            return;
        }

        _this->OnCompositionEffect( _callbackData );
    };

    providers.composition_state = []( const aeMovieCompositionStateCallbackData* _callbackData, ae_voidptr_t _ud )
    {
        Composition* _this = reinterpret_cast<Composition*>(_ud);

        if( _this == nullptr )
        {
            return;
        }

        _this->OnCompositionState( _callbackData );
    };

    mComposition = ae_create_movie_composition( moviewData, compData, AE_TRUE, &providers, this );

    if( mComposition == AE_NULLPTR )
    {
        return false;
    }

    ae_visit_movie_sub_composition( mComposition
        , []( const aeMovieComposition*, ae_uint32_t, const ae_char_t*, const aeMovieSubComposition* _subcomposition, ae_voidptr_t _ud ) -> ae_bool_t
    {
        Composition* _this = reinterpret_cast<Composition*>(_ud);

        _this->AddSubComposition( _subcomposition );

        return AE_TRUE;
    }, this );

    const ae_char_t * name = ae_get_movie_composition_name( mComposition );

    mName = name;

    if( this->CreateDrawingData() == false )
    {
        return false;
    }

    return true;
}
//////////////////////////////////////////////////////////////////////////
void Composition::AddSubComposition( const aeMovieSubComposition* subComposition )
{
    mSubCompositions.push_back( subComposition );
}
//////////////////////////////////////////////////////////////////////////
static bool CompileShader( const char* src, const GLenum type, GLuint * out, std::string * log )
{
    GLuint shader = glCreateShader( type );

    if( shader == 0 )
    {
        return false;
    }

    GLint status = 0;
    glShaderSource( shader, 1, &src, 0 );
    glCompileShader( shader );
    glGetShaderiv( shader, GL_COMPILE_STATUS, &status );

    // we grab the log anyway
    GLint infoLen = 0;
    glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &infoLen );

    if( infoLen > 1 && infoLen < 2048 )
    {
        char info[2048] = { 0 };
        glGetShaderInfoLog( shader, infoLen, nullptr, info ); // non-const .data() in c++17

        *log = info;
    }

    if( !status )
    {
        glDeleteShader( shader );

        return false;
    }

    *out = shader;

    return true;
}
//////////////////////////////////////////////////////////////////////////
static bool CreateShader( const char* vs, const char* fs, GLuint * out )
{
    std::string vsLog;
    GLuint vertexShader;
    if( CompileShader( vs, GL_VERTEX_SHADER, &vertexShader, &vsLog ) == false )
    {
        ViewerLogger << "Vertex shader compilation:" << std::endl << vsLog << std::endl;

        return false;
    }

    std::string fsLog;
    GLuint fragmentShader;
    if( CompileShader( fs, GL_FRAGMENT_SHADER, &fragmentShader, &fsLog ) == false )
    {
        ViewerLogger << "Fragment shader compilation:" << std::endl << fsLog << std::endl;

        return false;
    }

    GLuint program = glCreateProgram();

    if( program == 0 )
    {
        return false;
    }

    glAttachShader( program, vertexShader );
    glAttachShader( program, fragmentShader );
    glLinkProgram( program );

    // we don't need our shader objects anymore
    glDeleteShader( vertexShader );
    glDeleteShader( fragmentShader );

    // grab the log
    GLint infoLen = 0;
    glGetProgramiv( program, GL_INFO_LOG_LENGTH, &infoLen );

    if( infoLen > 1 && infoLen < 2048 ) {
        char info[2048] = { 0 };
        glGetProgramInfoLog( program, infoLen, nullptr, info ); // non-const .data() in c++17

        ViewerLogger << "Shader link:" << std::endl << info << std::endl;
    }

    GLint status = 0;
    glGetProgramiv( program, GL_LINK_STATUS, &status );

    if( status == 0 )
    {
        glDeleteProgram( program );

        return false;
    }

    glUseProgram( program );

    *out = program;

    return true;
}
//////////////////////////////////////////////////////////////////////////
bool Composition::CreateDrawingData()
{
    const GLsizeiptr vbSize = static_cast<GLsizeiptr>(kMaxVerticesToDraw * sizeof( DrawVertex ));
    const GLsizeiptr ibSize = static_cast<GLsizeiptr>(kMaxIndicesToDraw * sizeof( uint16_t ));

    // create shader program
    if( CreateShader( sVertexShader, sFragmentShader, &mShader ) == false )
    {
        return false;
    }

    if( CreateShader( sWireVertexShader, sWireFragmentShader, &mWireShader ) == false )
    {
        return false;
    }

    const aeMovieCompositionData * data = ae_get_movie_composition_composition_data( mComposition );

    if( data == AE_NULLPTR )
    {
        return false;
    }

    // Set initial ortho matrix, scale & offset
    SetViewportSize( mViewportWidth, mViewportHeight );
    SetContentScale( mContentScale );
    SetContentOffset( mContentOffX, mContentOffY );

    glUseProgram( mShader );

    mIsPremultAlphaUniform = glGetUniformLocation( mShader, "uIsPremultAlpha" );
    if( mIsPremultAlphaUniform >= 0 )
    {
        glUniform1i( mIsPremultAlphaUniform, GL_FALSE );
    }

    GLint texLocRGB = glGetUniformLocation( mShader, "uTextureRGB" );
    if( texLocRGB >= 0 )
    {
        glUniform1i( texLocRGB, kTextureRGBSlot );
    }

    GLint texLocA = glGetUniformLocation( mShader, "uTextureA" );
    if( texLocA >= 0 )
    {
        glUniform1i( texLocA, kTextureASlot );
    }

    // create vertex buffer
    glGenBuffers( 1, &mVB );
    glBindBuffer( GL_ARRAY_BUFFER, mVB );
    glBufferData( GL_ARRAY_BUFFER, vbSize, nullptr, GL_DYNAMIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    // create index buffer
    glGenBuffers( 1, &mIB );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIB );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, ibSize, nullptr, GL_DYNAMIC_DRAW );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    // create vao to hold vertex attribs bindings
    glGenVertexArrays( 1, &mVAO );
    glBindVertexArray( mVAO );

    // attach vb
    glBindBuffer( GL_ARRAY_BUFFER, mVB );

    // enable our attributes
    glEnableVertexAttribArray( kVertexPosAttribIdx );
    glEnableVertexAttribArray( kVertexUV0AttribIdx );
    glEnableVertexAttribArray( kVertexUV1AttribIdx );
    glEnableVertexAttribArray( kVertexColorAttribIdx );

    // bind our attributes
    glVertexAttribPointer( kVertexPosAttribIdx, 3, GL_FLOAT, GL_FALSE, _GL_SIZE( DrawVertex ), _GL_OFFSET( DrawVertex, pos ) );
    glVertexAttribPointer( kVertexUV0AttribIdx, 2, GL_FLOAT, GL_FALSE, _GL_SIZE( DrawVertex ), _GL_OFFSET( DrawVertex, uv0 ) );
    glVertexAttribPointer( kVertexUV1AttribIdx, 2, GL_FLOAT, GL_FALSE, _GL_SIZE( DrawVertex ), _GL_OFFSET( DrawVertex, uv1 ) );
    glVertexAttribPointer( kVertexColorAttribIdx, 4, GL_UNSIGNED_BYTE, GL_TRUE, _GL_SIZE( DrawVertex ), _GL_OFFSET( DrawVertex, color ) );

    // attach ib
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIB );

    return true;
}
//////////////////////////////////////////////////////////////////////////
void Composition::DestroyDrawingData()
{
    glBindVertexArray( mVAO );
    glBindBuffer( GL_ARRAY_BUFFER, mVB );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIB );
    glUnmapBuffer( GL_ARRAY_BUFFER );
    glUnmapBuffer( GL_ELEMENT_ARRAY_BUFFER );

    glDeleteBuffers( 1, &mVB );
    glDeleteBuffers( 1, &mIB );
    glDeleteVertexArrays( 1, &mVAO );

    glDeleteShader( mShader );
    glDeleteShader( mWireShader );
}
//////////////////////////////////////////////////////////////////////////
void Composition::BeginDraw()
{
    glBindVertexArray( mVAO );
    glBindBuffer( GL_ARRAY_BUFFER, mVB );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIB );
    mVerticesData = glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY );
    mIndicesData = glMapBuffer( GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY );
}
//////////////////////////////////////////////////////////////////////////
void Composition::EndDraw()
{
    this->FlushDraw();

    glUnmapBuffer( GL_ARRAY_BUFFER );
    glUnmapBuffer( GL_ELEMENT_ARRAY_BUFFER );
}
//////////////////////////////////////////////////////////////////////////
void Composition::DrawMesh( const aeMovieRenderMesh* mesh, const ResourceImage* imageRGB, const ResourceImage* imageA, const float* alternativeUV )
{
    uint32_t verticesLeft = kMaxVerticesToDraw - mNumVertices;
    uint32_t indicesLeft = kMaxIndicesToDraw - mNumIndices;

    GLuint newTextureRGB = ResourcesManager::Instance().GetWhiteTexture();
    if( imageRGB != nullptr && imageRGB->textureRes != nullptr )
    {
        newTextureRGB = imageRGB->textureRes->texture;
    }

    GLuint newTextureA = ResourcesManager::Instance().GetWhiteTexture();
    if( imageA != nullptr && imageA->textureRes != nullptr )
    {
        newTextureA = imageA->textureRes->texture;
    }

    bool isPremultAlpha = (imageRGB && imageRGB->premultAlpha);
    const BlendMode newBlendMode = (mesh->blend_mode == AE_MOVIE_BLEND_ADD) ? BlendMode::Add : BlendMode::Normal;

    if( mesh->vertexCount > verticesLeft ||
        mesh->indexCount > indicesLeft ||
        newTextureRGB != mCurrentTextureRGB ||
        newTextureA != mCurrentTextureA ||
        isPremultAlpha != mPremultipliedAlpha ||
        newBlendMode != mCurrentBlendMode )
    {
        this->FlushDraw();
    }

    mCurrentTextureRGB = newTextureRGB;
    mCurrentTextureA = newTextureA;
    mCurrentBlendMode = newBlendMode;
    mPremultipliedAlpha = isPremultAlpha;

    DrawVertex* vertices = reinterpret_cast<DrawVertex*>(mVerticesData) + mNumVertices;
    uint16_t* indices = reinterpret_cast<uint16_t*>(mIndicesData) + mNumIndices;

    for( uint32_t i = 0; i < mesh->vertexCount; ++i, ++vertices )
    {
        vertices->pos[0] = mesh->position[i][0];
        vertices->pos[1] = mesh->position[i][1];
        vertices->pos[2] = mesh->position[i][2];

        if( alternativeUV )
        {
            vertices->uv0[0] = alternativeUV[i * 2 + 0];
            vertices->uv0[1] = alternativeUV[i * 2 + 1];
            vertices->uv1[0] = mesh->uv[i][0];
            vertices->uv1[1] = mesh->uv[i][1];
        }
        else
        {
            vertices->uv1[0] = vertices->uv0[0] = mesh->uv[i][0];
            vertices->uv1[1] = vertices->uv0[1] = mesh->uv[i][1];
        }

        vertices->color = FloatColorToUint( mesh->color, mesh->opacity );
    }

    for( uint32_t i = 0; i < mesh->indexCount; ++i )
    {
        indices[i] = static_cast<uint16_t>((mesh->indices[i] + mNumVertices) & 0xffff);
    }

    mNumVertices += mesh->vertexCount;
    mNumIndices += mesh->indexCount;
}
//////////////////////////////////////////////////////////////////////////
void Composition::FlushDraw()
{
    if( mNumIndices )
    {
        bool drawSolid = (mDrawMode == DrawMode::Solid || mDrawMode == DrawMode::SolidWithWireOverlay);
        bool drawWire = (mDrawMode == DrawMode::Wireframe || mDrawMode == DrawMode::SolidWithWireOverlay);

        GLint isPremultAlpha = GL_FALSE;

        switch( mCurrentBlendMode )
        {
        case BlendMode::Normal:
            {
                if( mPremultipliedAlpha )
                {
                    glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
                }
                else
                {
                    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
                }
            } break;
        case BlendMode::Add:
            {
                if( mPremultipliedAlpha )
                {
                    glBlendFunc( GL_SRC_ALPHA, GL_ONE );
                }
                else
                {
                    glBlendFunc( GL_ONE, GL_ONE );
                }
            } break;
        }

        glUnmapBuffer( GL_ARRAY_BUFFER );
        glUnmapBuffer( GL_ELEMENT_ARRAY_BUFFER );

        glActiveTexture( GL_TEXTURE0 + kTextureRGBSlot );
        glBindTexture( GL_TEXTURE_2D, mCurrentTextureRGB );
        glActiveTexture( GL_TEXTURE0 + kTextureASlot );
        glBindTexture( GL_TEXTURE_2D, mCurrentTextureA );

        if( drawSolid )
        {
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
            glUseProgram( mShader );
            glUniform1i( mIsPremultAlphaUniform, mPremultipliedAlpha ? GL_TRUE : GL_FALSE );
            glDrawElements( GL_TRIANGLES, static_cast<GLsizei>(mNumIndices), GL_UNSIGNED_SHORT, nullptr );
        }

        if( drawWire )
        {
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
            glUseProgram( mWireShader );
            glDrawElements( GL_TRIANGLES, static_cast<GLsizei>(mNumIndices), GL_UNSIGNED_SHORT, nullptr );
        }

        mVerticesData = glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY );
        mIndicesData = glMapBuffer( GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY );
    }

    mNumVertices = 0;
    mNumIndices = 0;
}
//////////////////////////////////////////////////////////////////////////
bool Composition::OnProvideNode( const aeMovieNodeProviderCallbackData* _callbackData, void** _nd )
{
    AE_UNUSED( _nd );

    ViewerLogger << "Node provider callback" << std::endl;

    if( ae_is_movie_layer_data_track_mate( _callbackData->layer ) == AE_TRUE )
    {
        ViewerLogger << " Is track matte layer" << std::endl;
        return true;
    }

    aeMovieLayerTypeEnum layerType = ae_get_movie_layer_data_type( _callbackData->layer );

    ViewerLogger << " Layer: '" << ae_get_movie_layer_data_name( _callbackData->layer ) << std::endl;

    if( _callbackData->track_matte_layer == nullptr )
    {
        ViewerLogger << " Has track matte: no" << std::endl;
        ViewerLogger << " Type:";

        switch( layerType )
        {
        case AE_MOVIE_LAYER_TYPE_SLOT:
            {
                ViewerLogger << " slot" << std::endl;
            }break;
        case AE_MOVIE_LAYER_TYPE_VIDEO:
            {
                ViewerLogger << " video" << std::endl;
            }break;
        case AE_MOVIE_LAYER_TYPE_SOUND:
            {
                ViewerLogger << " sound" << std::endl;

                ae_voidptr_t rd = ae_get_movie_layer_data_resource_userdata( _callbackData->layer );

                ResourceSound * resourceSound = reinterpret_cast<ResourceSound*>( rd );

                *_nd = reinterpret_cast<ae_voidptr_t>( resourceSound );
            }break;
        case AE_MOVIE_LAYER_TYPE_IMAGE:
            {
                ViewerLogger << " image" << std::endl;

                ae_voidptr_t rd = ae_get_movie_layer_data_resource_userdata( _callbackData->layer );

                ResourceImage* resourceImage = reinterpret_cast<ResourceImage*>( rd );

                *_nd = reinterpret_cast<ae_voidptr_t>( resourceImage );
            }break;
        default:
            {
                ViewerLogger << " other" << std::endl;
            }break;
        }
    }
    else
    {
        ViewerLogger << " Has track matte: yes" << std::endl;
        ViewerLogger << " Type:";

        switch( layerType )
        {
        case AE_MOVIE_LAYER_TYPE_SHAPE:
            {
                ViewerLogger << " shape" << std::endl; break;
            }break;
        case AE_MOVIE_LAYER_TYPE_IMAGE:
            {
                ViewerLogger << " image" << std::endl;

                ae_voidptr_t rd = ae_get_movie_layer_data_resource_userdata( _callbackData->track_matte_layer );

                ResourceImage* resourceTrackMatteImage = reinterpret_cast<ResourceImage*>(rd);

                *_nd = reinterpret_cast<ae_voidptr_t>(resourceTrackMatteImage);
            }break;
        default:
            {
                ViewerLogger << " other" << std::endl;
            }break;
        }
    }

    return true;
}
//////////////////////////////////////////////////////////////////////////
void Composition::OnDeleteNode( const aeMovieNodeDeleterCallbackData* _callbackData )
{
    ViewerLogger << "Node destroyer callback." << std::endl;
    aeMovieLayerTypeEnum layerType = ae_get_movie_layer_data_type( _callbackData->layer );
    ViewerLogger << " Layer type: " << layerType << std::endl;
}
//////////////////////////////////////////////////////////////////////////
void Composition::OnUpdateNode( const aeMovieNodeUpdateCallbackData* _callbackData )
{
    AE_UNUSED( _callbackData );

    aeMovieLayerTypeEnum layerType = ae_get_movie_layer_data_type( _callbackData->layer );

    if( AE_MOVIE_LAYER_TYPE_SOUND == layerType )
    {
        ae_voidptr_t rd = ae_get_movie_layer_data_resource_userdata( _callbackData->layer );

        ResourceSound * resourceSound = reinterpret_cast<ResourceSound*>( rd );

        if( resourceSound && resourceSound->sound )
        {
            switch( _callbackData->state )
            {
            case AE_MOVIE_STATE_UPDATE_BEGIN:
            case AE_MOVIE_STATE_UPDATE_RESUME:
                {
                    resourceSound->sound->Play();
                }break;

            case AE_MOVIE_STATE_UPDATE_PAUSE:
                {
                    resourceSound->sound->Pause();
                }break;

            case AE_MOVIE_STATE_UPDATE_END:
                {
                    resourceSound->sound->Stop();
                }break;

            case AE_MOVIE_STATE_UPDATE_PROCESS:
            case AE_MOVIE_STATE_UPDATE_SKIP:
                //#NOTE_SK: should I do something here ???
                break;
            }
        }
    }
}
//////////////////////////////////////////////////////////////////////////
bool Composition::OnProvideCamera( const aeMovieCameraProviderCallbackData* _callbackData, void** _cd )
{
    AE_UNUSED( _callbackData );
    AE_UNUSED( _cd );

    ViewerLogger << "Camera provider callback." << std::endl;

    return true;
}
//////////////////////////////////////////////////////////////////////////
static void __copy_m4_m34( float * _m4, const float * _m34 )
{
    for( uint32_t index = 0; index != 4; ++index )
    {
        _m4[index * 4 + 0] = _m34[index * 3 + 0];
        _m4[index * 4 + 1] = _m34[index * 3 + 1];
        _m4[index * 4 + 2] = _m34[index * 3 + 2];
    }

    _m4[3] = 0.f;
    _m4[7] = 0.f;
    _m4[11] = 0.f;
    _m4[15] = 1.f;
}
//////////////////////////////////////////////////////////////////////////
bool Composition::OnProvideTrackMatte( const aeMovieTrackMatteProviderCallbackData * _callbackData, void** _tmd )
{
    TrackMatteDesc* desc = new TrackMatteDesc();

    __copy_m4_m34( desc->matrix, _callbackData->matrix );

    desc->mesh = _callbackData->mesh[0];
    desc->mode = _callbackData->track_matte_mode;

    *_tmd = desc;

    return true;
}
//////////////////////////////////////////////////////////////////////////
void Composition::OnUpdateTrackMatte( const aeMovieTrackMatteUpdateCallbackData* _callbackData )
{
    switch( _callbackData->state )
    {
    case AE_MOVIE_STATE_UPDATE_BEGIN:
        {
            TrackMatteDesc* desc = reinterpret_cast<TrackMatteDesc *>(_callbackData->track_matte_userdata);

            if( desc != nullptr )
            {
                __copy_m4_m34( desc->matrix, _callbackData->matrix );                
                desc->mesh = _callbackData->mesh[0];
            }
        }break;
    case AE_MOVIE_STATE_UPDATE_PROCESS:
        {
            TrackMatteDesc* desc = reinterpret_cast<TrackMatteDesc *>(_callbackData->track_matte_userdata);

            if( desc != nullptr )
            {
                __copy_m4_m34( desc->matrix, _callbackData->matrix );
                desc->mesh = _callbackData->mesh[0];
            }
        }break;
    default:
        {
        }break;
    }
}
//////////////////////////////////////////////////////////////////////////
void Composition::OnDeleteTrackMatte( const aeMovieTrackMatteDeleterCallbackData* _callbackData )
{
    TrackMatteDesc* desc = reinterpret_cast<TrackMatteDesc *>(_callbackData->track_matte_userdata);

    if( desc != nullptr )
    {
        delete desc;
    }
}
//////////////////////////////////////////////////////////////////////////
void Composition::OnCompositionEffect( const aeMovieCompositionEventCallbackData* _callbackData )
{
    ViewerLogger << "Composition event callback." << std::endl;
}
//////////////////////////////////////////////////////////////////////////
void Composition::OnCompositionState( const aeMovieCompositionStateCallbackData* _callbackData )
{
    AE_UNUSED( _callbackData );
}
