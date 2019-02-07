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
//#include "audio/include/AudioEngine.h"
#include "AESoundNode.h"
#include "AEMovie.h"

NS_CC_EXT_BEGIN;

USING_NS_CC;
using namespace cocos2d::experimental;

AESoundNode * AESoundNode::create( AESound *sound ) {
    AESoundNode * ret = new (std::nothrow) AESoundNode();

    if( ret ) {
        ret->autorelease();
        ret->_sound = sound;
        CCLOG( "  sound ptr: %p", ret->getSound() );
        CCLOG( " Sound: '%s'", ret->getSound()->getPath().c_str() );

        return ret;
    }

    CC_SAFE_DELETE( ret );

    return nullptr;
}

AESoundNode::AESoundNode()
    : _sound( nullptr )
    , _audioId( AudioEngine::INVALID_AUDIO_ID )
{
    CCLOG( "AESoundNode::AESoundNode()" );
}

AESoundNode::~AESoundNode() {
    CCLOG( "AESoundNode::~AESoundNode()" );
    AudioEngine::stop( _audioId );
}

void AESoundNode::setTime( float t ) {
    if( _audioId == AudioEngine::INVALID_AUDIO_ID )
        return;

    AudioEngine::AudioState state = AudioEngine::getState( _audioId );

    Scheduler* const scheduler( Director::getInstance()->getScheduler() );

    if( state >= AudioEngine::AudioState::PLAYING ) {
        scheduler->unschedule( SEL_SCHEDULE( &AESoundNode::updateTimeCallback ), this );
        AudioEngine::setCurrentTime( _audioId, t );
    }
    else {
        _updateTime = t;
        scheduler->schedule( SEL_SCHEDULE( &AESoundNode::updateTimeCallback ), this, 0.f, CC_REPEAT_FOREVER, 0.f, false );
    }
}

void AESoundNode::updateTimeCallback( float dt ) {
    AE_UNUSED( dt );

    Scheduler* const scheduler( Director::getInstance()->getScheduler() );

    if( _audioId == AudioEngine::INVALID_AUDIO_ID ) {
        scheduler->unschedule( SEL_SCHEDULE( &AESoundNode::updateTimeCallback ), this );
        return;
    }

    AudioEngine::AudioState state = AudioEngine::getState( _audioId );

    if( state >= AudioEngine::AudioState::PLAYING ) {
        AudioEngine::setCurrentTime( _audioId, _updateTime );
        scheduler->unschedule( SEL_SCHEDULE( &AESoundNode::updateTimeCallback ), this );
    }
}

NS_CC_EXT_END;
