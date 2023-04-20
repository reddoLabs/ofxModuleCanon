#include "ofxModuleCanon.h"

namespace ofxModule {
	//------------------------------------------------------------------
	ofxModuleCanon::ofxModuleCanon(string moduleName,string settingsPath) :ModuleRunnable("Canon", moduleName, settingsPath) {

		//setup cam
		devices = ofxCanon::listDevices();
		isMirrorX = settings["mirrorX"] == nullptr ? false : settings["mirrorX"];
		isMirrorY = settings["mirrorY"] == nullptr ? false : settings["mirrorY"];
		streamingMethod = settings["streamingMethod"] == nullptr || settings["streamingMethod"].get<string>() == "" ? "texture" : settings["streamingMethod"].get<string>();
		//width = settings["width"] == nullptr ? 0 : settings["width"];
		//height = settings["height"] == nullptr ? 0 : settings["height"];

		//dest = ofRectangle(0, 0, width, height);

		for (auto device : devices) {
			ofLogNotice("Canon", "connect device");
			device->open();
			this->device = device;
			this->device->setLiveViewEnabled(true, false);
			break;
		}

		isPhotoNew = false;
		takePhoto = false;
		isLiveView = false;

		texPtrLiveView = shared_ptr<ofTexture>(new ofTexture());
		texPtrPhoto = shared_ptr<ofTexture>(new ofTexture());

		pixPtrLiveView = shared_ptr<ofPixels>(&liveViewPixels);
		pixPtrPhoto = shared_ptr<ofPixels>(&photoPixels);

		lastActivation = ofGetElapsedTimeMillis();

	//	device->setLiveViewEnabled(true, false);
	//	ofSleepMillis(2000);

	}

	//------------------------------------------------------------------
	void ofxModuleCanon::update() {

		if (!isIdle && device) {

			device->update();

			if (takePhoto)
			{
				
				if (this->device->takePhoto(photoPixels)) {
					if (isMirrorX || isMirrorY) {
						photoPixels.mirror(isMirrorY, isMirrorX);
					}
					if (settings["rotate"] == 90) {
						photoPixels.rotate90(1);
					} else if (settings["rotate"] == 180) {
						photoPixels.rotate90(2);
					} else if (settings["rotate"] == 270) {
						photoPixels.rotate90(3);
					}
					/*if (width != 0 && height != 0) {
						ofRectangle t = dest;
						ofRectangle src = ofRectangle(0, 0, liveViewPixels.getWidth(), liveViewPixels.getHeight());
						t.scaleTo(src, OF_SCALEMODE_FIT);
						photoPixels.crop(t.x, t.y, t.width, t.height);
						photoPixels.resize(width, height);
					}*/
					if (streamingMethod == "texture" || streamingMethod == "both") {
						this->texPtrPhoto->loadData(photoPixels);				
					}

				}
				takePhoto = false;
				isPhotoNew = true;
			}

			//update live view
			if (!isPhotoNew && device->getLiveViewEnabled())
			{
				if (device->getLiveView(liveViewPixels)) {
					if (isMirrorX || isMirrorY) {
						liveViewPixels.mirror(isMirrorY, isMirrorX);
					}
					if (settings["rotate"] == 90) {
						liveViewPixels.rotate90(1);
					} else if (settings["rotate"] == 180) {
						liveViewPixels.rotate90(2);
					} else if (settings["rotate"] == 270) {
						liveViewPixels.rotate90(3);
					}
					//if (width != 0 && height != 0) {
					//	ofRectangle t = dest;
					//	ofRectangle src = ofRectangle(0, 0, liveViewPixels.getWidth(), liveViewPixels.getHeight());
					//	t.scaleTo(src, OF_SCALEMODE_FIT);
					//	liveViewPixels.crop(t.x, t.y, t.width, t.height);
					//	liveViewPixels.resize(width, height);
					//}
					this->texPtrLiveView->loadData(liveViewPixels);
					//use a black preview to prevent a not current image
					if (!blackLiveView.isAllocated()) {
						ofFbo fbo;
						fbo.allocate(liveViewPixels.getWidth(), liveViewPixels.getHeight());
						fbo.begin();
						ofClear(32,203,82,0);
						fbo.end();
						fbo.readToPixels(blackLiveView);
					}
					lastActivation = ofGetElapsedTimeMillis();
				}
				else if (ofGetElapsedTimeMillis() - lastActivation < 500 && ofGetElapsedTimeMillis() - lastActivation > 100 && blackLiveView.isAllocated()) {
					liveViewPixels = blackLiveView;
					texPtrLiveView->loadData(blackLiveView);
				}
				else if (ofGetElapsedTimeMillis() - lastActivation > 3000) {
					
					//liveViewPixels = blackLiveView;
					//texPtrLiveView->loadData(blackLiveView);
					device->close();
					devices = ofxCanon::listDevices();
					for (auto device : devices) {
						ofLogNotice("Canon", "connect device");
						if (device->open()) {
							this->device = device;
							this->device->setLiveViewEnabled(true, false);
						}
						break;
					}
					lastActivation = ofGetElapsedTimeMillis();
				}
			}


			if (isPhotoNew) {
				if (streamingMethod == "texture" || streamingMethod == "both") {
					notifyEvent(texPtrPhoto, "photo");
				}
				if (streamingMethod == "pixels" || streamingMethod == "both") {
					notifyEvent(pixPtrPhoto, "photo");
				}
				//logNotice("draw", "high resolution picture sent");
				isPhotoNew = false;
			}
			else if(texPtrLiveView->isAllocated()){
				if (streamingMethod == "texture" || streamingMethod == "both") {
					notifyEvent(texPtrLiveView);
				}
				if (streamingMethod == "pixels" || streamingMethod == "both") {
					notifyEvent(pixPtrLiveView);
				}
				
			}
		}
	}

	//------------------------------------------------------------------
	void ofxModuleCanon::stopModule() {
		device->close();
	}

	// received module events will be processed here
	void ofxModuleCanon::proceedModuleEvent(ModuleEvent & e)
	{
		if (e.address == "capture") {
			takePhoto = true;
		}
		else if (e.address == "setIdle") {
			if (e.message["value"].get<bool>() == true) {
				device->setLiveViewEnabled(false, false);
				
				/*while (device->getLiveViewEnabled())
				{
					device->setLiveViewEnabled(false, false);
					sleep(150);
				}*/
				ofLogNotice("proceedModuleEvent", "closing cam");
			}
			else //if (e.message["value"].get<bool>() == false)
			{
				device->setLiveViewEnabled(true, false);
				/*while (!device->getLiveViewEnabled())
				{
					device->setLiveViewEnabled(true, false);
					sleep(150);
				}*/
				lastActivation = ofGetElapsedTimeMillis();
				ofLogNotice("proceedModuleEvent", "opening cam");
			}
		}
	}
}
