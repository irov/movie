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

#ifndef __AEMovieEx_H__
#define __AEMovieEx_H__

#include "AEConfig.h"

#include "AESound.h"
#include "AESoundNode.h"
#include "AESlotNode.h"
#include "AERenderData.h"
#include "AETrackMatteData.h"
#include "AETrackMatteNode.h"
#include "AEMovieData.h"
#include "AEMovieCache.h"

NS_CC_EXT_BEGIN;

#define AE_MOVIE_DEBUG_DRAW 0

//
// the movie player
//

class CC_DLL AEMovie : public cocos2d::Node
{
public:
    // movie state report
    enum class EventType
    {
        PLAYING = 0,
        PAUSED,
        STOPPED,
        TIME_CHANGED
    };

    // the only function to create new nodes
	static AEMovie * create( const std::string & filePath );
	
	static AEMovie * createWithFramesFolder( const std::string & filePath, const std::string & framesFoldes );
	
	static AEMovie * createWithPlist( const std::string & filePath, const std::string & plistPath );
	
    // select composition inside the movie
    void setComposition( const std::string & name );

    // set playback to the specified time
    void setTime( float t );
    float getTime() const { return _time; }

    float getDuration() const { return _duration; }

    // playback controls
    void play();
    void stop();
    void interrupt( bool skip );

    virtual void pause() override;
    virtual void resume() override;

    // returns node for attachments, if it exists by the given name
    AESlotNode * getSlotNode( const std::string & name );

    // sets _eventCallback
    typedef std::function<void( Ref *, EventType )> TMovieCallback;
    virtual void addEventListener( const AEMovie::TMovieCallback & callback );

    // calls _eventCallback
    virtual void onPlayEvent( int event );

CC_CONSTRUCTOR_ACCESS:
    AEMovie();
    virtual ~AEMovie();

    virtual bool initWithFile( const std::string & filePath );
	virtual bool initWithFileAndFramesFolder( const std::string & filePath, const std::string & framesFoldes );
	virtual bool initWithPlist( const std::string & filePath, const std::string & plistPath );
    virtual bool initWithData( const AEMovieData * data );

protected:
    static ae_voidptr_t callbackCameraProvider( const aeMovieCameraProviderCallbackData * _callbackData, ae_voidptr_t _data );
    static ae_voidptr_t callbackNodeProvider( const aeMovieNodeProviderCallbackData * _callbackData, ae_voidptr_t _data );
    static void callbackNodeDeleter( const aeMovieNodeDeleterCallbackData * _callbackData, ae_voidptr_t _data );
    static void callbackNodeUpdate( const aeMovieNodeUpdateCallbackData * _callbackData, ae_voidptr_t _data );
    static ae_voidptr_t callbackCompositionTrackMatteProvider( const aeMovieTrackMatteProviderCallbackData * _callbackData, ae_voidptr_t _data );
    static void callbackCompositionTrackMatteUpdate( const aeMovieTrackMatteUpdateCallbackData * _callbackData, ae_voidptr_t _data );
    static void callbackCompositionEvent( const aeMovieCompositionEventCallbackData * _callbackData, ae_voidptr_t _data );
    static void callbackCompositionState( const aeMovieCompositionStateCallbackData * _callbackData, ae_voidptr_t _data );

protected:
    void addCamera( const std::string & name, cocos2d::Camera * camera );
    cocos2d::Camera * getCamera( const std::string & name );

    void addSoundNode( AESoundNode * node );

    void addSlotNode( const std::string & name, AESlotNode * node );

    void addTrackMatteNode( AETrackMatteNode * node );
    void addTrackMatteData( AETrackMatteData * data );

    // this is scheduled to update every frame after a composition is loaded
    virtual void update( float delta ) override;

	void convertTextCoords(AERenderData& renderData, cocos2d::SpriteFrame* spriteFrame);

	// renders the node
    virtual void draw( cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags ) override;

    cocos2d::GLProgramState * _normalGPS;
    cocos2d::GLProgramState * _trackMatteGPS;
	
#if AE_MOVIE_DEBUG_DRAW > 0
    cocos2d::DrawNode *_debugDrawNode;
#endif

    // the movie resource being played
    const AEMovieData * _movieData;

    // current composition info
    aeMovieComposition * _composition;

    //	bool _isPlayingSubComposition;

    float _time;
    float _duration;

    // this is called in onPlayEvent()
    TMovieCallback _eventCallback;

    // various data arrays
    // TODO: use cocos Map & Vector classes
    TVectorRenderData _renderDatas;

    typedef std::map<std::string, cocos2d::Camera *> TMapCamera;
    TMapCamera _cameras;

    typedef std::vector<AESoundNode *> TVectorSoundNode;
    TVectorSoundNode _soundNodes;

    // FIXME: rework to use cocos2d::Node::_name and getChildByName()
    typedef std::unordered_map<std::string, AESlotNode *> TMapSlotNode;
    TMapSlotNode _slotNodes;

    //	typedef std::unordered_map<std::string, AETrackMatteNode *> TMapTrackMatteNode;
    //	TMapTrackMatteNode _trackMatteNodes;
    typedef std::vector<AETrackMatteNode *> TVectorTrackMatteNode;
    TVectorTrackMatteNode _trackMatteNodes;

    typedef std::vector<AETrackMatteData *> TVectorTrackMatteData;
    TVectorTrackMatteData _trackMatteDatas;

private:
    CC_DISALLOW_COPY_AND_ASSIGN( AEMovie );
};

NS_CC_EXT_END;

#endif


