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

//#include "cocos2d.h"
//#include "extensions/cocos-ext.h"
//#include "audio/include/AudioEngine.h"
#include "AEMovieData.h"
#include "AEMovie.h"

NS_CC_EXT_BEGIN;

USING_NS_CC;
using namespace cocos2d::experimental;

//===============================================

namespace Detail
{
	static ae_size_t read_file( ae_voidptr_t _data, ae_voidptr_t _buff, ae_size_t _carriage, ae_size_t _size ) {
		(void)_carriage;
		BundleReader * reader = static_cast<BundleReader *>(_data);
		ssize_t rs = reader->read( _buff, 1, _size );

		return (ae_size_t)rs;
	}

	static void memory_copy( ae_voidptr_t _data, ae_constvoidptr_t _src, ae_voidptr_t _dst, ae_size_t _size ) {
		(void)_data;
		memcpy( _dst, _src, _size );
	}
}

//===============================================

void *AEMovieData::callbackResourceProvider( const aeMovieResource * _resource, ae_voidptr_t _data ) {
    CCLOG( "CALL: resource provider" );

    AEMovieData *data = static_cast<AEMovieData *>(_data);

    switch( _resource->type )
    {
    case AE_MOVIE_RESOURCE_IMAGE:
        {
            const aeMovieResourceImage * r = (const aeMovieResourceImage *)_resource;

            CCLOG( "Resource type: image." );
            CCLOG( " path        = '%s'", r->path );
            CCLOG( " trim_width  = %i", (int)r->trim_width );
            CCLOG( " trim_height = %i", (int)r->trim_height );

            Ref * ref = data->createImage( r->path, (int)r->trim_width, (int)r->trim_height );
            return ref;
        }
    case AE_MOVIE_RESOURCE_VIDEO:
        {
            //			const aeMovieResourceVideo * r = (const aeMovieResourceVideo *)_resource;

            CCLOG( "Resource type: video." );

            //			Ref * rr = data->createResourceVideo( r );
            //			return rr;
            break;
        }
    case AE_MOVIE_RESOURCE_SOUND:
        {
            const aeMovieResourceSound * r = (const aeMovieResourceSound *)_resource;

            CCLOG( "Resource type: sound." );
            CCLOG( " path        = '%s'", r->path );

            AESound *sound = data->createSound( r->path );
            return sound;
        }
    case AE_MOVIE_RESOURCE_SLOT:
        {
            const aeMovieResourceSlot * r = (const aeMovieResourceSlot *)_resource;
            AE_UNUSED(r);
            
            CCLOG( "Resource type: slot." );
            CCLOG( " width  = %f", r->width );
            CCLOG( " height = %f", r->height );
            break;
        }
    default:
        {
            CCLOG( "Resource type: other." );
            break;
        }
    }

    return nullptr;
}

void AEMovieData::callbackResourceDeleter( aeMovieResourceTypeEnum _type, ae_voidptr_t _data, ae_voidptr_t _ud )
{
    (void)_type;
    (void)_data;
    (void)_ud;
}

//===============================================

AEMovieData::AEMovieData()
    : _data( nullptr )
{
}

AEMovieData::~AEMovieData()
{
    if( _data ) {
        CCLOG( "Deleting movie data." );
        ae_delete_movie_data( _data );
        //		_data = nullptr;
    }

    //	_path.clear();

    for( TVectorSound::iterator soundIt = _sounds.begin(); soundIt != _sounds.end(); soundIt++ )
        CC_SAFE_RELEASE( *soundIt );
}

bool AEMovieData::initWithFileAndFramesFolder( aeMovieInstance * instance, const std::string & filepath, const std::string & framesFolder )
{
	if( filepath.empty() )
	{
		CCLOG( "AEMovieData::initWithFile(): blank resource filename." );
		return false;
	}
	
	std::string fullPath = FileUtils::getInstance()->fullPathForFilename( filepath );
	
	CCLOG( "Initializing with file '%s'.", fullPath.c_str() );
	
	Data data = FileUtils::getInstance()->getDataFromFile( fullPath );
	
	if( data.isNull() )
	{
		CCLOG( "'%s' not found.", fullPath.c_str() );
		return false;
	}
	
	_path = framesFolder;
	
	BundleReader reader;
	reader.init( (char *)(data.getBytes()), data.getSize() );
	
	_data = ae_create_movie_data( instance, &AEMovieData::callbackResourceProvider, &AEMovieData::callbackResourceDeleter, this );
	aeMovieStream * stream = ae_create_movie_stream( instance, &Detail::read_file, &Detail::memory_copy, &reader );
    ae_uint32_t version;
    ae_result_t r = ae_load_movie_data( _data, stream, &version );
	ae_delete_movie_stream( stream );
	
	//	data.clear();
	
	if( r != AE_RESULT_SUCCESSFUL )
	{
		CCLOG( "Failed to load movie data." );
		return false;
	}
	
	return true;
}

Ref *AEMovieData::createImage( const std::string & path, int width, int height ) {
    AE_UNUSED( width );
    AE_UNUSED( height );

    std::string fileName = _path + path;
    std::replace( fileName.begin(), fileName.end(), '\\', '/' );
    CCLOG( "createImage fileName = %s", fileName.c_str() );

	SpriteFrame* frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(fileName);
	if(frame)
	{
		return frame;
	}
    else
	{
		auto texture = Director::getInstance()->getTextureCache()->addImage(fileName);
		if(texture)
		{
			Rect rect;
			rect.size = texture->getContentSizeInPixels();
			frame = SpriteFrame::createWithTexture(texture, rect);
			SpriteFrameCache::getInstance()->addSpriteFrame(frame, fileName);
			
			Texture2D::TexParams tp;
			tp.magFilter = GL_LINEAR;
			tp.minFilter = GL_LINEAR;
			tp.wrapS = GL_CLAMP_TO_EDGE;
			tp.wrapT = GL_CLAMP_TO_EDGE;
			texture->setTexParameters( tp );
		}
	}

    return frame;
}

AESound *AEMovieData::createSound( const std::string & path ) {
    std::string fileName = _path + path;
    std::replace( fileName.begin(), fileName.end(), '\\', '/' );

    //	AudioEngine::preload(path);

    AESound * sound = AESound::create( fileName );
    sound->retain();

    _sounds.push_back( sound );

    return sound;
}

NS_CC_EXT_END;
