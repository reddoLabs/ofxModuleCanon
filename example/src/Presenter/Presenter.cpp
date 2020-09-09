#include "Presenter.h"
#include "GPF.h"

namespace ofxModule {
    
	Presenter::Presenter(string moduleName):ModuleDrawable("Presenter",moduleName){
		
		// register mouse events for interaction
		ofRegisterMouseEvents(this);


		ofBackground(0);
    }
  
    
    
    //------------------------------------------------------------------
    void Presenter::update() {
		// here could run something ;)
    }

	void Presenter::draw()
	{
		// draw camera image from texture
		if (camTexture != nullptr) camTexture->draw(0, 0, camTexture->getWidth(), camTexture->getHeight());
		if (highResTexture != nullptr) highResTexture->draw(0, camTexture->getHeight(), camTexture->getWidth(), camTexture->getHeight());

		if (camImage.isAllocated()) camImage.draw(camImage.getWidth(), 0, camImage.getWidth(), camImage.getHeight());
		if (highResImage.isAllocated()) highResImage.draw(camImage.getWidth(), camImage.getHeight(), camImage.getWidth(), camImage.getHeight());

		ofDrawBitmapStringHighlight("take photo by clicking mouse button", 50, 50);
	}

	void Presenter::mousePressed(ofMouseEventArgs & mouse)
	{
		if (mouse.button == 0) {
			ofJson j;
			j["value"] = false;
			notifyEvent("setIdle", j);
		} else {
			// stop recording with sending an idle message to the camera
			notifyEvent("capture");
		}

		
	}

	void Presenter::mouseReleased(ofMouseEventArgs & mouse)
	{
	}
    
    
    //------------------------------------------------------------------
    void Presenter::proceedModuleEvent(ModuleEvent& e) {
		
		// update the cam image 
		if (e.type == ModuleEvent::MODULE_EVENT_TEXTURE) {
			if (e.address == "photo") {
				highResTexture = e.texture;
			}
			else  camTexture = e.texture;
		}else if (e.type == ModuleEvent::MODULE_EVENT_PIXELS) {
			if (e.address == "photo") {
				highResImage.setFromPixels(*e.pixels);
			} else  camImage.setFromPixels(*e.pixels);
		}
    }
    
}
