#ifndef _ofxModuleCanon
#define _ofxModuleCanon

#include "ofMain.h"
#include "ModuleRunnable.h"
#include "ofxCanon.h"

namespace ofxModule {

	/// \brief control for a Canon SLR 
	/// \ outputes the current Picture as ofTexture via ModuleEvent 
	/// \ outputes can be controlled using ModuleEvents
	/// \ 
	/// \ Messages
	/// \ capture : true  -> camera takes photo
	/// \ setIdle : true/false -> enables disables Live-View
class ofxModuleCanon : public ModuleRunnable{
	
  public:
	
	ofxModuleCanon(string moduleName = "Canon", string settingsPath = "settings.json");
    void update();
    void stopModule();
    
protected:
    void proceedModuleEvent(ModuleEvent& e);
    
private:
	shared_ptr<ofxCanon::Device> device;
	ofPixels photoPixels;
	ofPixels liveViewPixels;

	shared_ptr<ofTexture> texPtrLiveView;
	shared_ptr <ofTexture> texPtrPhoto;

	shared_ptr<ofPixels> pixPtrLiveView;
	shared_ptr<ofPixels> pixPtrPhoto;

	bool isPhotoNew;
	bool takePhoto;
	bool isLiveView;

	//int width;
	//int height;
	//ofRectangle dest;

	uint64_t lastActivation = 0;
	ofPixels blackLiveView;

	string streamingMethod;
	bool isMirrorX;
	bool isMirrorY;
	
	ofFbo mirrorFbo;
	ofFbo rotateFbo;

	
};
}
#endif
