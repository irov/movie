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

#include "AEMovie.h"

NS_CC_EXT_BEGIN;

USING_NS_CC;

AEMovieCache * AEMovieCache::s_sharedInstance = nullptr;

//===============================================

static ae_voidptr_t stdlib_movie_alloc( ae_voidptr_t _data, ae_size_t _size ) {
    (void)_data;
    return malloc( _size );
}

static ae_voidptr_t stdlib_movie_alloc_n( ae_voidptr_t _data, ae_size_t _size, ae_size_t _count ) {
    (void)_data;
    ae_size_t total = _size * _count;
    return malloc( total );
}

static void stdlib_movie_free( ae_voidptr_t _data, ae_constvoidptr_t _ptr ) {
    (void)_data;
    free( (void *)_ptr );
}

static void stdlib_movie_free_n( ae_voidptr_t _data, ae_constvoidptr_t _ptr ) {
    (void)_data;
    free( (void *)_ptr );
}

static void stdlib_movie_logerror( ae_voidptr_t _data, aeMovieErrorCode _code, const ae_char_t * _format, ... ) {
    char dst[2048];
    (void)_data;
    (void)_code;

    va_list argList;

    va_start( argList, _format );
    //	vprintf( _format, argList );
    vsnprintf( (char *)dst, sizeof( dst ), _format, argList );
    va_end( argList );

    CCLOG( "%.2048s", dst );
}

//===============================================

AEMovieCache::AEMovieCache() {
    _instance = nullptr;
}

AEMovieCache::~AEMovieCache() {
    if( _instance != nullptr ) {
        CCLOG( "Deleting movie instance." );
        ae_delete_movie_instance( _instance );
    }
}

bool AEMovieCache::initialize( const char * _hash )
{
    aeMovieInstance * instance = ae_create_movie_instance( _hash
        , &stdlib_movie_alloc
        , &stdlib_movie_alloc_n
        , &stdlib_movie_free
        , &stdlib_movie_free_n
        , (ae_movie_strncmp_t)AE_NULL
        , &stdlib_movie_logerror
        , AE_NULL );

    if( instance == AE_NULL )
    {
        return false;
    }

    _instance = instance;

    return true;
}

AEMovieData * AEMovieCache::addMovie( const std::string & filepath, const std::string & framesFoldes ) {
    MapAEMovieData::iterator it = _movies.find( filepath );

    if( it != _movies.end() ) {
        return it->second;
    }

    AEMovieData * data = new (std::nothrow) AEMovieData();

    if( data && data->initWithFileAndFramesFolder( _instance, filepath, framesFoldes ) ) {
        _movies.insert( filepath, data );
        data->release();
        return data;
    }

    return nullptr;
}

AEMovieData * AEMovieCache::addMovieWithPlist( const std::string & filePath, const std::string & plistPath )
{
	if(FileUtils::getInstance()->isFileExist(plistPath))
	{
		cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plistPath);
		return addMovie(filePath, "");
	}
	return nullptr;
}

void AEMovieCache::removeUnusedMovies() {
    for( MapAEMovieData::iterator
        it = _movies.begin(),
        it_end = _movies.end();
        it != it_end; ) {
        AEMovieData *data = it->second;

        if( data->getReferenceCount() == 1 ) {
            CCLOG( "AEMovieCache::removeUnusedMovies(): '%s'", it->first.c_str() );
            it = _movies.erase( it );
        }
        else {
            it++;
        }
    }
}

AEMovieCache * AEMovieCache::getInstance() {
    if( !s_sharedInstance ) {
        s_sharedInstance = new (std::nothrow) AEMovieCache();
        CCASSERT( s_sharedInstance, "FATAL: Not enough memory to create movie cache." );
    }

    return s_sharedInstance;
}

void AEMovieCache::destroyInstance() {
    CC_SAFE_RELEASE_NULL( s_sharedInstance );
}

NS_CC_EXT_END;
