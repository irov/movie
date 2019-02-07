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
//#include "renderer/CCGLProgramCache.h"

NS_CC_EXT_BEGIN;

USING_NS_CC;
using namespace cocos2d::experimental;

//===============================================

// TODO
ae_voidptr_t AEMovie::callbackCameraProvider( const aeMovieCameraProviderCallbackData * _callbackData, ae_voidptr_t _data )
{
    AE_UNUSED( _callbackData );
    AE_UNUSED( _data );

	CCLOG("CALL: camera provider");
/*
	AEMovie *movie = static_cast<AEMovie *>(_data);

	Camera *c = movie->getCamera(_callbackData->name);

	if (c != nullptr) {
		CCLOG("Camera '%s' found.", _callbackData->name);
		return c;
	}

	CCLOG("Creating new camera '%s'.", _callbackData->name);
*/
/*
	Camera* camera = Camera::createPerspective(_callbackData->fov, _callbackData->width / _callbackData->height, 1.f, 1000.f);
	camera->retain();
	camera->setPosition3D(Vec3(_callbackData->position));
	camera->lookAt(Vec3(_callbackData->direction));
	camera->setCameraFlag(CameraFlag::USER1);

	movie->addChild(camera);	// add to scene graph
	movie->addCamera(_callbackData->name, camera);	// add to map

	return camera;
*/
	return nullptr;
}

ae_voidptr_t AEMovie::callbackNodeProvider( const aeMovieNodeProviderCallbackData * _callbackData, ae_voidptr_t _data ) {
	CCLOG("CALL: node provider");

    ae_bool_t is_track_matte = ae_is_movie_layer_data_track_mate( _callbackData->layer );

	if( is_track_matte == AE_TRUE )
		return nullptr;

	AEMovie *movie = static_cast<AEMovie *>(_data);

    aeMovieLayerTypeEnum layerType = ae_get_movie_layer_data_type( _callbackData->layer );
    const char * layerName = ae_get_movie_layer_data_name( _callbackData->layer );
	
	CCLOG("Layer type: %i", layerType);
	CCLOG("Layer name: '%s'", layerName);

	if( _callbackData->track_matte_layer == nullptr ) {
		CCLOG(" No track matte.");

		switch( layerType ) {
            case AE_MOVIE_LAYER_TYPE_MOVIE:
            case AE_MOVIE_LAYER_TYPE_TEXT:
            case AE_MOVIE_LAYER_TYPE_EVENT:
            case AE_MOVIE_LAYER_TYPE_SOCKET:
            case AE_MOVIE_LAYER_TYPE_SHAPE:
            case AE_MOVIE_LAYER_TYPE_NULL:
            case AE_MOVIE_LAYER_TYPE_SOLID:
            case AE_MOVIE_LAYER_TYPE_SEQUENCE:
            case AE_MOVIE_LAYER_TYPE_PARTICLE:
            case AE_MOVIE_LAYER_TYPE_IMAGE:
            case AE_MOVIE_LAYER_TYPE_SUB_MOVIE:
                break;
			case AE_MOVIE_LAYER_TYPE_SLOT:
			{
				AESlotNode * slotNode = AESlotNode::create();
				slotNode->retain();
				//slotNode->setName(layerName);
				slotNode->setAdditionalTransform(Mat4(_callbackData->matrix));

				movie->addSlotNode(layerName, slotNode);

				CCLOG(" Slot:");
				CCLOG("  node ptr: %p", slotNode);
//				movie->addChild(slotNode);

				return slotNode;
			}break;
			case AE_MOVIE_LAYER_TYPE_VIDEO: {
				CCLOG(" Video:");
				
			}break;
			case AE_MOVIE_LAYER_TYPE_SOUND: {
				AESound *sound = static_cast<AESound *>(ae_get_movie_layer_data_resource_data(_callbackData->layer));
				
				CCLOG(" Sound:");
				CCLOG("  name: '%s'", sound->getPath().c_str());
				CCLOG("  sound ptr: %p",  sound);

				// FIXME: wrap into separate method
                AESoundNode * soundNode = AESoundNode::create(sound);
				soundNode->retain();

				movie->addSoundNode(soundNode);

				CCLOG(" node ptr: %p", soundNode);

				return soundNode;
			}break;
		}
	}
	else {
		CCLOG(" Has track matte.");

		switch( layerType ) {
            case AE_MOVIE_LAYER_TYPE_MOVIE:
            case AE_MOVIE_LAYER_TYPE_TEXT:
            case AE_MOVIE_LAYER_TYPE_EVENT:
            case AE_MOVIE_LAYER_TYPE_SOCKET:
            case AE_MOVIE_LAYER_TYPE_SLOT:
            case AE_MOVIE_LAYER_TYPE_NULL:
            case AE_MOVIE_LAYER_TYPE_SOLID:
            case AE_MOVIE_LAYER_TYPE_SEQUENCE:
            case AE_MOVIE_LAYER_TYPE_VIDEO:
            case AE_MOVIE_LAYER_TYPE_SOUND:
            case AE_MOVIE_LAYER_TYPE_PARTICLE:
            case AE_MOVIE_LAYER_TYPE_SUB_MOVIE:
                break;
            case AE_MOVIE_LAYER_TYPE_SHAPE:
				CCLOG(" Shape:");
				break;
			case AE_MOVIE_LAYER_TYPE_IMAGE:
                SpriteFrame *spriteFrame = static_cast<SpriteFrame *>(ae_get_movie_layer_data_resource_data( _callbackData->track_matte_layer ));
				Texture2D * texture = spriteFrame->getTexture();

				AETrackMatteNode * trackMatteNode = AETrackMatteNode::createFromTexture(texture);
				trackMatteNode->retain();
				//trackMatteNode->setName(layerName);

				movie->addTrackMatteNode(trackMatteNode);

				CCLOG(" Image:");
				CCLOG("  node ptr: %p", trackMatteNode);

				return trackMatteNode;
		}
	}

	return nullptr;
}

void AEMovie::callbackNodeDeleter( const aeMovieNodeDeleterCallbackData * _callbackData, ae_voidptr_t _data ) {
    AE_UNUSED( _callbackData );
    AE_UNUSED( _data );
    
	CCLOG("CALL: node destroyer");
}

void AEMovie::callbackNodeUpdate( const aeMovieNodeUpdateCallbackData * _callbackData, ae_voidptr_t _data ) {
    AE_UNUSED( _data );

    CCLOG("CALL: node update");

//	AEMovie *movie = static_cast<AEMovie *>(_data);

	switch (_callbackData->state)
	{
        case AE_MOVIE_STATE_UPDATE_BEGIN:
        {
            CCLOG("AE_MOVIE_STATE_UPDATE_BEGIN");
            
            switch (_callbackData->type)
            {
                case AE_MOVIE_LAYER_TYPE_MOVIE:
                case AE_MOVIE_LAYER_TYPE_TEXT:
                case AE_MOVIE_LAYER_TYPE_EVENT:
                case AE_MOVIE_LAYER_TYPE_SOCKET:
                case AE_MOVIE_LAYER_TYPE_SHAPE:
                case AE_MOVIE_LAYER_TYPE_SLOT:
                case AE_MOVIE_LAYER_TYPE_NULL:
                case AE_MOVIE_LAYER_TYPE_SOLID:
                case AE_MOVIE_LAYER_TYPE_SEQUENCE:
                case AE_MOVIE_LAYER_TYPE_PARTICLE:
                case AE_MOVIE_LAYER_TYPE_IMAGE:
                case AE_MOVIE_LAYER_TYPE_SUB_MOVIE:
                    break;
                case AE_MOVIE_LAYER_TYPE_VIDEO:
                    CCLOG(" Video:");
                    break;
                case AE_MOVIE_LAYER_TYPE_SOUND:
                    AESoundNode *soundNode = static_cast<AESoundNode *>(_callbackData->element);
                    
                    CCLOG("  node ptr: %p", soundNode);
                    CCLOG(" Sound:");
                    CCLOG("  sound ptr: %p", soundNode->getSound());
                    CCLOG("  name: '%s'", soundNode->getSound()->getPath().c_str());
                    CCLOG("  offset: %.2f sec", _callbackData->offset);
                    
                    int id = soundNode->getAudioId();
                    
                    // FIXME: maybe don't stop, just set time instead
                    if (id != AudioEngine::INVALID_AUDIO_ID) {
                        AudioEngine::stop(id);
                    }
                    
                    id = AudioEngine::play2d(soundNode->getSound()->getPath());
                    
                    CCLOG("  node audio id: %i", id);
                    CCLOG("  node audio state: %i", AudioEngine::getState(id));
                    
                    soundNode->setAudioId(id);
                    soundNode->setTime(_callbackData->offset);
                    
                    break;
            }
        }break;
		case AE_MOVIE_STATE_UPDATE_PROCESS:
		{
			CCLOG("AE_MOVIE_STATE_UPDATE_PROCESS");

			switch (_callbackData->type) {
                case AE_MOVIE_LAYER_TYPE_MOVIE:
                case AE_MOVIE_LAYER_TYPE_TEXT:
                case AE_MOVIE_LAYER_TYPE_EVENT:
                case AE_MOVIE_LAYER_TYPE_SOCKET:
                case AE_MOVIE_LAYER_TYPE_SHAPE:
                case AE_MOVIE_LAYER_TYPE_NULL:
                case AE_MOVIE_LAYER_TYPE_SOLID:
                case AE_MOVIE_LAYER_TYPE_SEQUENCE:
                case AE_MOVIE_LAYER_TYPE_VIDEO:
                case AE_MOVIE_LAYER_TYPE_SOUND:
                case AE_MOVIE_LAYER_TYPE_IMAGE:
                case AE_MOVIE_LAYER_TYPE_SUB_MOVIE:
                    break;
				case AE_MOVIE_LAYER_TYPE_PARTICLE:
					CCLOG(" Particle:");
					break;
				case AE_MOVIE_LAYER_TYPE_SLOT:
					CCLOG(" Slot:");

					AESlotNode *slotNode = static_cast<AESlotNode *>(_callbackData->element);
					slotNode->setAdditionalTransform(Mat4(_callbackData->matrix));

					#if COCOS2D_DEBUG > 0
					const float *f = _callbackData->matrix;
					CCLOG("f0: %.2f %.2f %.2f %.2f", f[0], f[1], f[2], f[3]);
					CCLOG("f1: %.2f %.2f %.2f %.2f", f[4], f[5], f[6], f[7]);
					CCLOG("f2: %.2f %.2f %.2f %.2f", f[8], f[9], f[10], f[11]);
					CCLOG("f3: %.2f %.2f %.2f %.2f", f[12], f[13], f[14], f[15]);
					#endif

					break;
			}
		}break;
        case AE_MOVIE_STATE_UPDATE_PAUSE:
            break;
        case AE_MOVIE_STATE_UPDATE_RESUME:
            break;
		case AE_MOVIE_STATE_UPDATE_END:
		{
			CCLOG("AE_MOVIE_STATE_UPDATE_END");

			switch (_callbackData->type)
			{
                case AE_MOVIE_LAYER_TYPE_MOVIE:
                case AE_MOVIE_LAYER_TYPE_TEXT:
                case AE_MOVIE_LAYER_TYPE_EVENT:
                case AE_MOVIE_LAYER_TYPE_SOCKET:
                case AE_MOVIE_LAYER_TYPE_SHAPE:
                case AE_MOVIE_LAYER_TYPE_SLOT:
                case AE_MOVIE_LAYER_TYPE_NULL:
                case AE_MOVIE_LAYER_TYPE_SOLID:
                case AE_MOVIE_LAYER_TYPE_SEQUENCE:
                case AE_MOVIE_LAYER_TYPE_PARTICLE:
                case AE_MOVIE_LAYER_TYPE_IMAGE:
                case AE_MOVIE_LAYER_TYPE_SUB_MOVIE:
                    break;
				case AE_MOVIE_LAYER_TYPE_VIDEO:
					CCLOG(" Video:");
					break;
				case AE_MOVIE_LAYER_TYPE_SOUND:
					AESoundNode *soundNode = static_cast<AESoundNode *>(_callbackData->element);
					CCLOG("  node ptr: %p", soundNode);

					CCLOG(" Sound:");
					CCLOG("  name: '%s'", soundNode->getSound()->getPath().c_str());

                    int audioId = soundNode->getAudioId();
					AudioEngine::stop(audioId);

					// TODO: rework into soundNode->stop() etc.
					soundNode->setAudioId(AudioEngine::INVALID_AUDIO_ID);

					break;
			}
		}break;
	}
}

ae_voidptr_t AEMovie::callbackCompositionTrackMatteProvider( const aeMovieTrackMatteProviderCallbackData * _callbackData, ae_voidptr_t _data ) {
    
    AEMovie *movie = static_cast<AEMovie *>(_data);

    // FIXME: count track matte on load & use a vector of simple structs instead
    AETrackMatteData * tmData = AETrackMatteData::create();
    tmData->retain();
    tmData->setMatrix( _callbackData->matrix );
    tmData->setRenderMesh( *_callbackData->mesh );

    movie->addTrackMatteData( tmData );

    return tmData;
}

void AEMovie::callbackCompositionTrackMatteUpdate( const aeMovieTrackMatteUpdateCallbackData * _callbackData, ae_voidptr_t _data ) {
	CCLOG("CALL: composition track matte update");

	AEMovie *movie = static_cast<AEMovie *>(_data);
    AE_UNUSED(movie);

	switch (_callbackData->state) {
		case AE_MOVIE_STATE_UPDATE_BEGIN: {
			CCLOG("AE_MOVIE_STATE_UPDATE_BEGIN");
			
			AETrackMatteData * tmData = static_cast<AETrackMatteData *>(_callbackData->track_matte_data);
			tmData->setMatrix(_callbackData->matrix);
			tmData->setRenderMesh(*_callbackData->mesh);
			
		}break;
		case AE_MOVIE_STATE_UPDATE_PROCESS: {
			CCLOG("AE_MOVIE_STATE_UPDATE_PROCESS");

			AETrackMatteData * tmData = static_cast<AETrackMatteData *>(_callbackData->track_matte_data);
			tmData->setMatrix(_callbackData->matrix);
			tmData->setRenderMesh(*_callbackData->mesh);
			
		}break;
        case AE_MOVIE_STATE_UPDATE_PAUSE:
        case AE_MOVIE_STATE_UPDATE_RESUME:
            break;
		case AE_MOVIE_STATE_UPDATE_END: {
			CCLOG("NODE_UPDATE_END");
/*
			AETrackMatteData * tmData = static_cast<AETrackMatteData *>(_callbackData->track_matte_data);

			if (tmData == nullptr)
				CCLOG("WTF data NULL");

			CC_SAFE_RELEASE(tmData);
*/
		}break;
	}
}

void AEMovie::callbackCompositionEvent( const aeMovieCompositionEventCallbackData * _callbackData, ae_voidptr_t _data ) {
    AE_UNUSED( _callbackData );
    AE_UNUSED( _data );
	CCLOG("CALL: composition event");
}

void AEMovie::callbackCompositionState( const aeMovieCompositionStateCallbackData * _callbackData, ae_voidptr_t _data ) {
	CCLOG("CALL: composition state");

	if (_callbackData->subcomposition != nullptr)
		return;

	AEMovie *movie = static_cast<AEMovie *>(_data);

	if (_callbackData->state == AE_MOVIE_COMPOSITION_END) {
		movie->stop();
	}
}

void AEMovie::addCamera(const std::string & name, Camera *camera) {
	TMapCamera::iterator it = _cameras.find(name);

	if (it != _cameras.end()) {
		CCLOG("addCamera(): camera '%s' already exists.", name.c_str());
		return;
	}

	_cameras.insert(std::make_pair(name, camera));
}

Camera *AEMovie::getCamera(const std::string & name) {
	TMapCamera::iterator it = _cameras.find(name);

	if (it == _cameras.end()) {
		CCLOG("getCamera(): camera '%s' does not exist.", name.c_str());
		return nullptr;
	}

	return it->second;
}

void AEMovie::addSoundNode(AESoundNode * node) {
	_soundNodes.push_back(node);
}

void AEMovie::addSlotNode(const std::string& name, AESlotNode *node) {
	TMapSlotNode::iterator it = _slotNodes.find(name);

	if (it != _slotNodes.end()) {
		CCLOG("addSlotNode(): node '%s' already exists.", name.c_str());
		return;
	}

	_slotNodes.insert(std::make_pair(name, node));
}

AESlotNode *AEMovie::getSlotNode(const std::string & name) {
	TMapSlotNode::iterator it = _slotNodes.find(name);

	if (it == _slotNodes.end()) {
		CCLOG("getSlotNode(): node '%s' does not exist.", name.c_str());
		return nullptr;
	}

	return it->second;
}

void AEMovie::addTrackMatteNode(AETrackMatteNode * node) {
	_trackMatteNodes.push_back(node);
}

void AEMovie::addTrackMatteData(AETrackMatteData * data) {
	_trackMatteDatas.push_back(data);
}

AEMovie * AEMovie::create(const std::string & filepath) {
	AEMovie * ret = new (std::nothrow) AEMovie();

	if (ret && ret->initWithFile(filepath)) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

AEMovie * AEMovie::createWithFramesFolder( const std::string & filepath, const std::string & framesFoldes )
{
	AEMovie * ret = new (std::nothrow) AEMovie();
	
	if (ret && ret->initWithFileAndFramesFolder(filepath, framesFoldes)) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

AEMovie * AEMovie::createWithPlist( const std::string & filePath, const std::string & plistPath )
{
	AEMovie * ret = new (std::nothrow) AEMovie();
	
	if (ret && ret->initWithPlist(filePath, plistPath)) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}


AEMovie::AEMovie()
: _normalGPS(nullptr)
, _trackMatteGPS(nullptr)
, _movieData(nullptr)
, _composition(nullptr)
, _time(0.f)
, _duration(0.f)
, _eventCallback(nullptr)
{
#if AE_MOVIE_DEBUG_DRAW > 0
	_debugDrawNode = DrawNode::create();
	addChild(_debugDrawNode);
#endif
}

AEMovie::~AEMovie()
{
	if (_composition != nullptr) {
		CCLOG("Deleting movie composition.");
		ae_delete_movie_composition(_composition );
		_composition = nullptr;
	}

//	_isPlayingSubComposition = false;

	// decrease all cocos Ref counters
	for (TMapCamera::iterator cameraIt = _cameras.begin(); cameraIt != _cameras.end(); cameraIt++)
		CC_SAFE_RELEASE(cameraIt->second);
	for (TVectorSoundNode::iterator soundNodeIt = _soundNodes.begin(); soundNodeIt != _soundNodes.end(); soundNodeIt++)
		CC_SAFE_RELEASE(*soundNodeIt);
	for (TMapSlotNode::iterator slotNodeIt = _slotNodes.begin(); slotNodeIt != _slotNodes.end(); slotNodeIt++)
		CC_SAFE_RELEASE(slotNodeIt->second);
	for (TVectorTrackMatteNode::iterator tmNodeIt = _trackMatteNodes.begin(); tmNodeIt != _trackMatteNodes.end(); tmNodeIt++)
		CC_SAFE_RELEASE(*tmNodeIt);
	for (TVectorTrackMatteData::iterator tmDataIt = _trackMatteDatas.begin(); tmDataIt != _trackMatteDatas.end(); tmDataIt++)
		CC_SAFE_RELEASE(*tmDataIt);

	// clear lists
/*
	_renderDatas.clear();
	_cameras.clear();
	_soundNodes.clear();
	_slotNodes.clear();
	_trackMatteNodes.clear();
	_trackMatteDatas.clear();
*/
}

bool AEMovie::initWithFile(const std::string & filepath)
{
	int k = filepath.find_last_of('/');
	if( k != std::string::npos )
	{
		std::string framesFolder = filepath.substr(0, k + 1);
		return initWithFileAndFramesFolder(filepath, framesFolder);
	}
	else
	{
		return false;
	}
}

bool AEMovie::initWithFileAndFramesFolder(const std::string & filePath, const std::string & framesFoldes)
{
	// FIXME: move cache to the Director
	AEMovieData * data = AEMovieCache::getInstance()->addMovie(filePath, framesFoldes);
	return initWithData(data);
}

bool AEMovie::initWithPlist( const std::string & filePath, const std::string & plistPath )
{
	AEMovieData * data = AEMovieCache::getInstance()->addMovieWithPlist(filePath, plistPath);
	return initWithData(data);
}

bool AEMovie::initWithData(const AEMovieData * data)
{
/*
	_renderDatas.clear();
	_cameras.clear();
	_soundNodes.clear();
	_slotNodes.clear();
	_trackMatteNodes.clear();
	_trackMatteDatas.clear();
*/

	// get pre-cached shaders
    GLProgramCache* pc = GLProgramCache::getInstance();
    GLProgram * p1 = pc->getGLProgram("AEM_Normal");
    GLProgram * p2 = pc->getGLProgram("AEM_TrackMatte");

	// create or get cached GL states for them
	_normalGPS = GLProgramState::getOrCreateWithGLProgram(p1);
	_trackMatteGPS = GLProgramState::getOrCreateWithGLProgram(p2);

	// set data pointer
	_movieData = data;

	//
	// start update loop
	//

	scheduleUpdate();

	return true;
}

void AEMovie::setComposition(const std::string & name)
{
    if (name.empty())
    {
        CCLOG("Called AEMovie::setComposition with blank composition name.");
        return;
    }

    if (!_movieData)
    {
        CCLOG("Called AEMovie::setComposition with NULL movie data.");
        return;
    }


	// look for it
	const aeMovieCompositionData * compositionData = ae_get_movie_composition_data( _movieData->getData(), name.c_str() );

	if( compositionData == AE_NULL )
	{
		CCLOG("Composition '%s' not found.", name.c_str());
		return;
	}

    float composition_duration = ae_get_movie_composition_data_duration( compositionData );
	
    _duration = composition_duration;

	CCLOG("Composition: '%s'", name.c_str());
	CCLOG(" width: %.2f", ae_get_movie_composition_data_width(compositionData));
	CCLOG(" height: %.2f", ae_get_movie_composition_data_height(compositionData));
	CCLOG(" duration: %.2f sec", _duration);

	// free the previous one
	if (_composition) {
		ae_delete_movie_composition(_composition );
		_composition = nullptr;
	}

	// create new
	aeMovieCompositionProviders providers;

	providers.camera_provider = &callbackCameraProvider;
	providers.node_provider = &callbackNodeProvider;
	providers.node_deleter = &callbackNodeDeleter;
	providers.node_update = &callbackNodeUpdate;
    providers.track_matte_provider = &callbackCompositionTrackMatteProvider;
	providers.track_matte_update = &callbackCompositionTrackMatteUpdate;
	providers.composition_event = &callbackCompositionEvent;
	providers.composition_state = &callbackCompositionState;

	_composition = ae_create_movie_composition( _movieData->getData(), compositionData, AE_TRUE, &providers, this );

	if (_composition == nullptr)
	{
		CCLOG("Failed to create composition.");
		return;
	}

	ae_set_movie_composition_loop( _composition, AE_TRUE );

	// reserve storage
    aeMovieCompositionRenderInfo info;
    ae_calculate_movie_composition_render_info( _composition, &info );

    _renderDatas.reserve( info.max_render_node );

	// don't play from start
	stop();

	// set node original content size
    float width = ae_get_movie_composition_data_width( compositionData );
    float height = ae_get_movie_composition_data_height( compositionData );

    setContentSize( Size( width, height ) );
}

void AEMovie::setTime(float t) {
	_time = t;
	onPlayEvent((int)AEMovie::EventType::TIME_CHANGED);

	CCLOG("Set time to: %.2f sec.", _time);

	ae_stop_movie_composition( _composition );
	ae_play_movie_composition( _composition, _time );
//	onPlayEvent((int)AEMovie::EventType::PLAYING);

/*
	// TODO: make work (might need to add that to libmovie)
	for (TVectorSoundNode::iterator soundNodeIt = _soundNodes.begin(); soundNodeIt != _soundNodes.end(); soundNodeIt++) {
		AudioEngine::setCurrentTime(*soundNodeIt->getAudioId(), );
	}
*/
}

void AEMovie::play() {
	CCLOG("AEMOVIEPLAY");

	resume();

	ae_play_movie_composition( _composition, _time );

	//
	// run subcomposition (start playing it in a loop just once for test)
	//
/*
	if (!_isPlayingSubComposition) {
		const aeMovieSubComposition * subComposition = ae_get_movie_sub_composition( _composition, "1-1" );

		if( subComposition != nullptr )
		{
			ae_set_movie_sub_composition_loop( subComposition, AE_TRUE );
			ae_play_movie_sub_composition( _composition, subComposition, 0.f );
		}

		_isPlayingSubComposition = true;
	}
*/

	onPlayEvent((int)AEMovie::EventType::PLAYING);
}

void AEMovie::stop() {
	CCLOG("AEMOVIESTOP");	

	_time = 0.f;
	onPlayEvent((int)AEMovie::EventType::TIME_CHANGED);

	ae_stop_movie_composition( _composition );
	onPlayEvent((int)AEMovie::EventType::STOPPED);

	// stop all playing audio streams
	for (TVectorSoundNode::iterator soundNodeIt = _soundNodes.begin(); soundNodeIt != _soundNodes.end(); soundNodeIt++)
		AudioEngine::stop((*soundNodeIt)->getAudioId());
}

void AEMovie::interrupt(bool skip) {
	CCLOG("AEMOVIEINTERRUPT");	

    ae_interrupt_movie_composition( _composition, skip );
}

void AEMovie::pause() {
	CCLOG("AEMOVIEPAUSE");	

	Node::pause();

	onPlayEvent((int)AEMovie::EventType::PAUSED);

	// pause all playing audio streams
	for (TVectorSoundNode::iterator soundNodeIt = _soundNodes.begin(); soundNodeIt != _soundNodes.end(); soundNodeIt++)
		AudioEngine::pause((*soundNodeIt)->getAudioId());
}

void AEMovie::resume() {
	CCLOG("AEMOVIERESUME");

	Node::resume();

	// resume all playing audio streams
	for (TVectorSoundNode::iterator soundNodeIt = _soundNodes.begin(); soundNodeIt != _soundNodes.end(); soundNodeIt++)
		AudioEngine::resume((*soundNodeIt)->getAudioId());

	onPlayEvent((int)AEMovie::EventType::PLAYING);
}

void AEMovie::addEventListener(const AEMovie::TMovieCallback& callback) {
	_eventCallback = callback;
}

void AEMovie::onPlayEvent(int event) {
        AEMovie::EventType movieEvent = (AEMovie::EventType)event;

//	if (movieEvent == AEMovie::EventType::STOPPED) {
//		_isPlayingSubComposition = false;
//	}

        if (_eventCallback) {
            _eventCallback(this, movieEvent);
        }
}

void AEMovie::update(float delta) {
	Node::update(delta);

	if (_composition == nullptr)
		return;
	if (ae_is_play_movie_composition(_composition) == AE_FALSE)
		return;

	ae_update_movie_composition( _composition, delta );

//	float t = ae_get_movie_composition_time(_composition);

	_time += delta;
	onPlayEvent((int)AEMovie::EventType::TIME_CHANGED);

	CCLOG("Composition time mark is %.2f sec.", _time);
}

static void calc_uv_clip_vectors( const Vec2 & _a, const Vec2 & _b, const Vec2 & _c, const Vec2 & _auv, const Vec2 & _buv, const Vec2 & _cuv, Vec3 * _out) {
	Vec2 _uvA = _auv;
	Vec2 _dAB = _b - _a;
	Vec2 _dAC = _c - _a;

	float inv_v = 1.f / (_dAB.x * _dAC.y - _dAB.y * _dAC.x);
	_dAB *= inv_v;
	_dAC *= inv_v;

	Vec2 _dac;
	_dac.x = _dAC.x * _a.y - _dAC.y * _a.x;
	_dac.y = -(_dAB.x * _a.y - _dAB.y * _a.x);

	_dAB *= -1.f;

	_dAB.x *= -1.f;
	_dAC.x *= -1.f;

	Vec2 _duvAB = _buv - _uvA;
	Vec2 _duvAC = _cuv - _uvA;

	_out[0] = Vec3( _dac.x, _dAC.y, _dAC.x );
	_out[1] = Vec3( _dac.y, _dAB.y, _dAB.x );
	_out[2] = Vec3( _uvA.x, _duvAB.x, _duvAC.x );
	_out[3] = Vec3( _uvA.y, _duvAB.y, _duvAC.y );
}

void AEMovie::convertTextCoords(AERenderData& renderData, cocos2d::SpriteFrame* spriteFrame)
{
	float atlasWidth = spriteFrame->getTexture()->getPixelsWide();
	float atlasHeight = spriteFrame->getTexture()->getPixelsHigh();
	Rect rectInPixels = spriteFrame->getRect();
	bool rectRotated = spriteFrame->isRotated();
	
	float rw = rectInPixels.size.width;
	float rh = rectInPixels.size.height;
	if (rectRotated)
		std::swap(rw, rh);
	
	float left    = rectInPixels.origin.x / atlasWidth;
	float right   = (rectInPixels.origin.x + rw) / atlasWidth;
	float top     = rectInPixels.origin.y / atlasHeight;
	float bottom  = (rectInPixels.origin.y + rh) / atlasHeight;
	
	left += right * renderData.vertices[0].texCoords.u;
	top += bottom * renderData.vertices[0].texCoords.v;
	right -= right * (1-renderData.vertices[2].texCoords.u);
	bottom -= bottom * (1-renderData.vertices[2].texCoords.u);
	
	if (rectRotated)
	{
		renderData.vertices[0].texCoords.u = right;
		renderData.vertices[0].texCoords.v = top;
		renderData.vertices[1].texCoords.u = right;
		renderData.vertices[1].texCoords.v = bottom;
		renderData.vertices[2].texCoords.u = left;
		renderData.vertices[2].texCoords.v = bottom;
		renderData.vertices[3].texCoords.u = left;
		renderData.vertices[3].texCoords.v = top;
	}
	else
	{
		renderData.vertices[0].texCoords.u = left;
		renderData.vertices[0].texCoords.v = top;
		renderData.vertices[1].texCoords.u = right;
		renderData.vertices[1].texCoords.v = top;
		renderData.vertices[2].texCoords.u = right;
		renderData.vertices[2].texCoords.v = bottom;
		renderData.vertices[3].texCoords.u = left;
		renderData.vertices[3].texCoords.v = bottom;
	}
}

void AEMovie::draw(Renderer * renderer, const Mat4 & transform, uint32_t flags) {
#if AE_MOVIE_DEBUG_DRAW > 0
    const Size& size = getContentSize();

	_debugDrawNode->clear();
	_debugDrawNode->drawRect(Vec2(0.f, 0.f), Vec2(size.width, size.height), Color4F::WHITE);
#endif

	if (_composition == nullptr) {
		return;
	}

	_renderDatas.clear();

	uint32_t renderMeshIt = 0;
	aeMovieRenderMesh renderMesh;

	CCLOG("AEMovie::Draw(): BEGIN");

	while( ae_compute_movie_mesh( _composition, &renderMeshIt, &renderMesh ) == AE_TRUE )
	{	
		// TODO: pre-cache mesh data
		CCLOG( "Rendering mesh %i.", renderMeshIt );

		if( renderMesh.camera_data != nullptr ) {
			CCLOG("Has camera.");
		}
		else {
			CCLOG("No camera.");
		}

		if( renderMesh.track_matte_data == nullptr ) {
			//
			// layer has no track matte
			//

			CCLOG("No track matte.");

			switch( renderMesh.layer_type )
			{
                case AE_MOVIE_LAYER_TYPE_TEXT:
                case AE_MOVIE_LAYER_TYPE_EVENT:
                case AE_MOVIE_LAYER_TYPE_SOCKET:
                case AE_MOVIE_LAYER_TYPE_NULL:
                    break;
				case AE_MOVIE_LAYER_TYPE_MOVIE:
				{
					CCLOG("Layer type: movie.");
					break;
				}
				case AE_MOVIE_LAYER_TYPE_SHAPE:
				{
					CCLOG("Layer type: shape.");
					break;
				}
				case AE_MOVIE_LAYER_TYPE_SLOT:
				{
					CCLOG("Layer type: slot.");

					AESlotNode * slotNode = static_cast<AESlotNode *>(renderMesh.element_data);
					CCLOG(" node ptr: %p", slotNode);

					// render it & children
					slotNode->visit(renderer, transform, flags);

					break;
				}
				case AE_MOVIE_LAYER_TYPE_SOLID:
				{
					CCLOG("Layer type: solid.");
					break;
				}
				case AE_MOVIE_LAYER_TYPE_SEQUENCE:
				case AE_MOVIE_LAYER_TYPE_IMAGE:
				{
					if (renderMesh.layer_type == AE_MOVIE_LAYER_TYPE_SEQUENCE)
						CCLOG("Layer type: sequence.");
					else
						CCLOG("Layer type: image.");

					if (renderMesh.vertexCount == 0 || renderMesh.indexCount == 0)
						break;

					CCLOG("mesh info:");
					CCLOG(" vertex count = %i", renderMesh.vertexCount);
					CCLOG(" index count = %i", renderMesh.indexCount);
					CCLOG(" RGBA: %.2f %.2f %.2f %.2f", renderMesh.color.r, renderMesh.color.g, renderMesh.color.b, renderMesh.opacity);
					CCLOG(" blendfunc: %i", renderMesh.blend_mode);

					SpriteFrame * spriteFrame = static_cast<SpriteFrame *>(renderMesh.resource_data);
					Texture2D * texture = spriteFrame->getTexture();

					_renderDatas.push_back(AERenderData());
					AERenderData & renderData = _renderDatas.back();

					//
					// setup vertices
					//

					renderData.vertices.resize(renderMesh.vertexCount);

					const ae_float_t * p = &renderMesh.position[0][0];
					const ae_float_t * uv = renderMesh.uv[0];

                    for( ae_uint32_t i = 0; i != renderMesh.vertexCount; ++i ) 
                    {
                        V3F_C4B_T2F &v = renderData.vertices[i];

						v.vertices = Vec3(p[0], p[1], p[2]);
						v.colors = Color4B(Color4F(renderMesh.color.r, renderMesh.color.g, renderMesh.color.b, renderMesh.opacity));
						v.texCoords = Tex2F(uv[0], uv[1]);
						
						CCLOG(" vertex %i:", i);
						CCLOG("  position = %.2f %.2f %.2f", p[0], p[1], p[2]);
						CCLOG("  uv       = %.2f %.2f", uv[0], uv[1]);
						CCLOG("  color    = %.2f %.2f %.2f %.2f", renderMesh.color.r, renderMesh.color.g, renderMesh.color.b, renderMesh.opacity);

                        p += 3;
                        uv += 2;
					}
					
					convertTextCoords(renderData, spriteFrame);
					
					TrianglesCommand::Triangles triangles;
					triangles.verts = renderData.vertices.data();
					triangles.vertCount = renderMesh.vertexCount;

					//
					// setup indices
					//

					renderData.indices.assign(renderMesh.indices, renderMesh.indices + renderMesh.indexCount);

					triangles.indices = renderData.indices.data();
					triangles.indexCount = renderMesh.indexCount;

					#if COCOS2D_DEBUG > 0
					for (int i = 0; i < triangles.indexCount; i++) {
						CCLOG(" index %i = %i", i, triangles.indices[i]);
					}
					#endif

					BlendFunc blendFunc;

					switch(renderMesh.blend_mode) {
						case AE_MOVIE_BLEND_ADD:
							blendFunc.src = GL_SRC_ALPHA;
							blendFunc.dst = GL_ONE;
							break;
//						case AE_MOVIE_BLEND_NORMAL:
						default:
							blendFunc.src = GL_SRC_ALPHA;
							blendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;
							break;
					}

//					CCLOG(" resource type: %i", (int)renderMesh.resource_type);
//					CCLOG(" resource data: %i", (int)renderMesh.resource_data);

					
					

					#if COCOS2D_DEBUG > 0
					const Texture2D::PixelFormatInfo & pfi = Texture2D::getPixelFormatInfoMap().at(texture->getPixelFormat());

					CCLOG(" Texture: '%s'", texture->getDescription().c_str());
					CCLOG("  internal format: 0x%x", pfi.internalFormat);
					CCLOG("  format: 0x%x", pfi.format);
					CCLOG("  type: 0x%x", pfi.type);
					CCLOG("  bpp: %i", pfi.bpp);
					CCLOG("  compressed: %i", pfi.compressed);
					CCLOG("  alpha: %i", pfi.alpha);
					#endif

					// don't do this because cocos binds the trianglesCommand.texture as CC_Texture0
//					_normalGPS->setUniformTexture("u_texture", texture);

					setGLProgramState(_normalGPS);

///					setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP, texture));
//					setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR, texture));
//					setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_COLOR_NO_MVP));
//					setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_COLOR));

					renderData.trianglesCommand.init(
						_globalZOrder,
						texture,
						getGLProgramState(),
						blendFunc,
						triangles,
						transform,
						flags);

					renderer->addCommand(&renderData.trianglesCommand);

					break;
				}
				case AE_MOVIE_LAYER_TYPE_VIDEO:
				{
					CCLOG("Layer type: video.");
					break;
				}
				case AE_MOVIE_LAYER_TYPE_SOUND:
				{
					CCLOG("Layer type: sound.");
					break;
				}
				case AE_MOVIE_LAYER_TYPE_PARTICLE:
				{
					CCLOG("Layer type: particle.");
					break;
				}
				case AE_MOVIE_LAYER_TYPE_SUB_MOVIE:
				{
					CCLOG("Layer type: submovie.");
					break;
				}
			}
		}
		else {
			//
			// layer has track matte
			//

			CCLOG("Has track matte.");

			switch( renderMesh.layer_type )
			{
                case AE_MOVIE_LAYER_TYPE_MOVIE:
                case AE_MOVIE_LAYER_TYPE_TEXT:
                case AE_MOVIE_LAYER_TYPE_EVENT:
                case AE_MOVIE_LAYER_TYPE_SOCKET:
                case AE_MOVIE_LAYER_TYPE_SHAPE:
                case AE_MOVIE_LAYER_TYPE_SLOT:
                case AE_MOVIE_LAYER_TYPE_NULL:
                case AE_MOVIE_LAYER_TYPE_SOLID:
                case AE_MOVIE_LAYER_TYPE_SEQUENCE:
                case AE_MOVIE_LAYER_TYPE_VIDEO:
                case AE_MOVIE_LAYER_TYPE_SOUND:
                case AE_MOVIE_LAYER_TYPE_PARTICLE:
                case AE_MOVIE_LAYER_TYPE_SUB_MOVIE:
                    break;
				case AE_MOVIE_LAYER_TYPE_IMAGE:
				{
					CCLOG("Layer type: image.");

					const AETrackMatteNode * trackMatteNode = static_cast<const AETrackMatteNode *>(renderMesh.element_data);

					if (trackMatteNode == nullptr)
						break;
					if (renderMesh.vertexCount == 0 || renderMesh.indexCount == 0)
						break;

					CCLOG("mesh info:");
					CCLOG(" vertex count = %i", renderMesh.vertexCount);
					CCLOG(" index count = %i", renderMesh.indexCount);
					CCLOG(" RGBA: %.2f %.2f %.2f %.2f", renderMesh.color.r, renderMesh.color.g, renderMesh.color.b, renderMesh.opacity);
					CCLOG(" blendfunc: %i", renderMesh.blend_mode);

					_renderDatas.push_back(AERenderData());
					AERenderData & renderData = _renderDatas.back();

					AETrackMatteData * tmData = static_cast<AETrackMatteData *>(renderMesh.track_matte_data);
//					const Mat4 & tmMatrix = tmData->getMatrix();
					const aeMovieRenderMesh & tmRenderMesh = tmData->getRenderMesh();
/*
					#if COCOS2D_DEBUG > 0
					const float *f = tmMatrix.m;
					CCLOG(" tmMatrix:");
					CCLOG("  f0: %.2f %.2f %.2f %.2f", f[0], f[1], f[2], f[3]);
					CCLOG("  f1: %.2f %.2f %.2f %.2f", f[4], f[5], f[6], f[7]);
					CCLOG("  f2: %.2f %.2f %.2f %.2f", f[8], f[9], f[10], f[11]);
					CCLOG("  f3: %.2f %.2f %.2f %.2f", f[12], f[13], f[14], f[15]);
					#endif
*/
					if (tmRenderMesh.vertexCount == 0 || tmRenderMesh.indexCount == 0)
						break;

					CCLOG("tm mesh info:");
					CCLOG(" position[0]: %.2f %.2f", tmRenderMesh.position[0][0], tmRenderMesh.position[0][1]);
					CCLOG(" position[1]: %.2f %.2f", tmRenderMesh.position[1][0], tmRenderMesh.position[1][1]);
					CCLOG(" position[2]: %.2f %.2f", tmRenderMesh.position[2][0], tmRenderMesh.position[2][1]);
					CCLOG(" uv[0]: %.2f %.2f", tmRenderMesh.uv[0][0], tmRenderMesh.uv[0][1]);
					CCLOG(" uv[1]: %.2f %.2f", tmRenderMesh.uv[1][0], tmRenderMesh.uv[1][1]);
					CCLOG(" uv[2]: %.2f %.2f", tmRenderMesh.uv[2][0], tmRenderMesh.uv[2][1]);

					
					SpriteFrame * spriteFrame = static_cast<SpriteFrame *>(renderMesh.resource_data);
					SpriteFrame * tmSpriteFrame = static_cast<SpriteFrame *>(tmRenderMesh.resource_data);
					Texture2D * texture = spriteFrame->getTexture();
					Texture2D * tmTexture = tmSpriteFrame->getTexture();
					
					Vec3 pos[3] = {
						Vec3(tmRenderMesh.position[0][0], tmRenderMesh.position[0][1], 0.f),
						Vec3(tmRenderMesh.position[1][0], tmRenderMesh.position[1][1], 0.f),
						Vec3(tmRenderMesh.position[2][0], tmRenderMesh.position[2][1], 0.f) };

					transform.transformPoint(&pos[0]);
					transform.transformPoint(&pos[1]);
					transform.transformPoint(&pos[2]);
					
					AERenderData tmRenderData;
					tmRenderData.vertices.resize(4);
					for(size_t i=0; i<4; ++i)
					{
						tmRenderData.vertices[i].texCoords.u = tmRenderMesh.uv[i][1];
						tmRenderData.vertices[i].texCoords.v = tmRenderMesh.uv[i][0];
					}
					convertTextCoords(tmRenderData, tmSpriteFrame);

					Vec3 clip[4];
					calc_uv_clip_vectors(
						Vec2(pos[0].x, pos[0].y),
						Vec2(pos[1].x, pos[1].y),
						Vec2(pos[2].x, pos[2].y),
						Vec2(tmRenderData.vertices[0].texCoords.u, tmRenderData.vertices[0].texCoords.v),
						Vec2(tmRenderData.vertices[1].texCoords.u, tmRenderData.vertices[1].texCoords.v),
						Vec2(tmRenderData.vertices[2].texCoords.u, tmRenderData.vertices[2].texCoords.v),
						clip);
					
					//
					// setup vertices
					//

					renderData.vertices.resize(renderMesh.vertexCount);

					const ae_float_t *p = &renderMesh.position[0][0];
					const ae_float_t *uv = renderMesh.uv[0];

                    for( ae_uint32_t i = 0; i != renderMesh.vertexCount; ++i ) 
                    {
						V3F_C4B_T2F &v = renderData.vertices[i];

						v.vertices = Vec3(p[0], p[1], p[2]);
						v.colors = Color4B(Color4F(renderMesh.color.r, renderMesh.color.g, renderMesh.color.b, renderMesh.opacity));
						v.texCoords = Tex2F(uv[0], uv[1]);
                        p += 3;
                        uv += 2;
					}
					convertTextCoords(renderData, spriteFrame);

					TrianglesCommand::Triangles triangles;
					triangles.verts = renderData.vertices.data();
					triangles.vertCount = renderMesh.vertexCount;

					//
					// setup indices
					//

					renderData.indices.assign(renderMesh.indices, renderMesh.indices + renderMesh.indexCount);

					triangles.indices = renderData.indices.data();
					triangles.indexCount = renderMesh.indexCount;

					#if COCOS2D_DEBUG > 0
					for (int i = 0; i < triangles.indexCount; i++) {
						CCLOG(" index %i = %i", i, triangles.indices[i]);
					}
					#endif

					BlendFunc blendFunc;

					switch(renderMesh.blend_mode) {
						case AE_MOVIE_BLEND_ADD:
							blendFunc.src = GL_SRC_ALPHA;
							blendFunc.dst = GL_ONE;
							break;
//						case AE_MOVIE_BLEND_NORMAL:
						default:
							blendFunc.src = GL_SRC_ALPHA;
							blendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;
					}

//					CCLOG(" resource type: %i", (int)renderMesh.resource_type);
//					CCLOG(" resource data: %i", (int)renderMesh.resource_data);

					// don't do this because cocos binds the trianglesCommand.texture as CC_Texture0
//					_trackMatteGPS->setUniformTexture("u_texture", texture);
					_trackMatteGPS->setUniformTexture("u_tmTexture", tmTexture);
//					_trackMatteGPS->setVertexAttribPointer("a_tmTexCoord", 2, GL_FLOAT, GL_FALSE, 0, );

					_trackMatteGPS->setUniformVec3("u_clip0", clip[0]);
					_trackMatteGPS->setUniformVec3("u_clip1", clip[1]);
					_trackMatteGPS->setUniformVec3("u_clip2", clip[2]);
					_trackMatteGPS->setUniformVec3("u_clip3", clip[3]);
					
					Vec2 left;
					Vec2 right;
					if(!tmSpriteFrame->isRotated())
					{
						left = Vec2(tmRenderData.vertices[0].texCoords.u, tmRenderData.vertices[0].texCoords.v);
						right = Vec2(tmRenderData.vertices[2].texCoords.u, tmRenderData.vertices[2].texCoords.v);
					}
					else
					{
						left = Vec2(tmRenderData.vertices[3].texCoords.u, tmRenderData.vertices[3].texCoords.v);
						right = Vec2(tmRenderData.vertices[1].texCoords.u, tmRenderData.vertices[1].texCoords.v);
					}
					_trackMatteGPS->setUniformVec2("left", left);
					_trackMatteGPS->setUniformVec2("right", right);

					setGLProgramState(_trackMatteGPS);

					renderData.trianglesCommand.init(
						_globalZOrder,
						texture,
						_glProgramState,
						blendFunc,
						triangles,
						transform,
						flags);

					renderer->addCommand(&renderData.trianglesCommand);
					renderer->render();

					break;
				}
			}
		}

		CCLOG("");
	}

	CCLOG("AEMovie::Draw(): END");
}

NS_CC_EXT_END;
