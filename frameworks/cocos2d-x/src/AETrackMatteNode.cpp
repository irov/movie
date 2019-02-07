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
#include "AETrackMatteNode.h"
#include "AEMovie.h"

NS_CC_EXT_BEGIN;

USING_NS_CC;

AETrackMatteNode * AETrackMatteNode::createFromTexture( Texture2D * texture ) {
    AETrackMatteNode * ret = new (std::nothrow) AETrackMatteNode();

    if( ret ) {
        ret->autorelease();
        ret->_texture = texture;
        return ret;
    }

    CC_SAFE_DELETE( ret );

    return nullptr;
}

AETrackMatteNode::AETrackMatteNode()
    : _texture( nullptr )
{
    CCLOG( "AETrackMatteNode::AETrackMatteNode()" );

    //#ifdef AE_SLOTNODE_DEBUG_DRAW
    //	_debugDrawNode = DrawNode::create();
    //	addChild(_debugDrawNode);
    //#endif
}

AETrackMatteNode::~AETrackMatteNode() {
    CCLOG( "AETrackMatteNode::~AETrackMatteNode()" );
}

/*
void AETrackMatteNode::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) {
#ifdef AE_SLOTNODE_DEBUG_DRAW
        _debugDrawNode->clear();
    _debugDrawNode->drawRect(Vec2(0.f, 0.f), Vec2(100.f, 100.f), Color4F::WHITE);
#endif
}
*/

NS_CC_EXT_END;
