#include "ofApp.h"
#include <math.h>

//--------------------------------------------------------------
void ofApp::setup(){

	//  ---------------------------------------------------
	//  ____ ___  ____ _  _ ___    ____ ____ ___ _  _ ___  
	//  [__  |__] |  | |  |  |     [__  |___  |  |  | |__] 
	//  ___] |    |__| |__|  |     ___] |___  |  |__| |    
	//                   
	//  ---------------------------------------------------
	
	ofSetFrameRate(60);
	fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA, 4); // Preparing Spout-Canvas


	//  ---------------------------------------------------
	//  _  _ _ ___  _    ____ ____ ___ _  _ ___  
	//  |\/| | |  \ |    [__  |___  |  |  | |__] 
	//  |  | | |__/ |    ___] |___  |  |__| |    
	//                              
	//  ---------------------------------------------------
	//midiIn.listPorts(); //  List and get correct Midi-Port
	midiIn.openPort(0);

	// add testApp as a listener
	midiIn.addListener(this);


	//  ---------------------------------------------------
	//  ____ ____ _  _ _  _ ___  ____ ___ ____ ____ ____ _  _    ____ ____ ___ _  _ ___  
	//  [__  |  | |  | |\ | |  \ [__   |  |__/ |___ |__| |\/|    [__  |___  |  |  | |__] 
	//  ___] |__| |__| | \| |__/ ___]  |  |  \ |___ |  | |  |    ___] |___  |  |__| |    
	//                                                                                   
	//  Fetch the external sound-input by setting up the soundStream-Listener
	//  ---------------------------------------------------

	//soundStream.listDevices(); //  List and get correct Sound-Device
	soundStream.setDeviceID(3);

	int channelsOut = 0;        // number of requested output channels (i.e. 2 for stereo).
    int channelsIn = 2;         // number of requested input channels.
    int sampleRate = 48000;     // requested sample rate (44100 is typical).
    int bufferSize = 512;       // requested buffer size (256 is typical).
    int numOfBuffers = 4;       // number of buffers to queue, less buffers will be more responsive, but less stable.

    soundStream.setup(this, channelsOut, channelsIn, sampleRate, bufferSize, numOfBuffers);
    samplesChannelL.assign(bufferSize, 0.0);
    samplesChannelR.assign(bufferSize, 0.0);

	//  ---------------------------------------------------

	circleRadius = 0; 	//  Initialize Smooth Rotation-Radius for the circle
	movingRadius = 0; 	//  Initialize Smooth Rotation-Radius for the circle

}

//--------------------------------------------------------------
void ofApp::update(){

	//  ---------------------------------------------------
	//  ____ ____ _  _ _  _ ___  ____ ___ ____ ____ ____ _  _    _  _ ___  ___  ____ ___ ____ 
	//  [__  |  | |  | |\ | |  \ [__   |  |__/ |___ |__| |\/|    |  | |__] |  \ |__|  |  |___ 
	//  ___] |__| |__| | \| |__/ ___]  |  |  \ |___ |  | |  |    |__| |    |__/ |  |  |  |___ 
	//                                                     
	//  ---------------------------------------------------
	
	fftChannelL.update();
    fftChannelR.update();
	
	//  ---------------------------------------------------
	//  ___  ____ ____ _ _ _    ____ ____ _  _ _  _ ____ ____ 
	//  |  \ |__/ |__| | | |    |    |__| |\ | |  | |__| [__  
	//  |__/ |  \ |  | |_|_|    |___ |  | | \|  \/  |  | ___] 
	//                                                                                                                                                                                                                              
	//  ---------------------------------------------------
	
	fbo.begin(); // Starting to draw onto spout-canvas

	ofClear(0,0,0,255);
	ofSetCircleResolution(60);
	ofSetColor(255,0,0);

	float time = ofGetElapsedTimef();
	float easeFact = 0.1;
	float midiVal = ofMap(midiMessage.value,0,127,0,250);

	movingRadius = movingRadius + (midiVal-movingRadius)*easeFact;
	cPos.x = cos(time * M_TWO_PI * 1.0)*movingRadius + ofGetWidth()/2;
	cPos.y = sin(time * M_TWO_PI * 1.0)*movingRadius + ofGetHeight()/2;
	
	float soundVal = analyzeSampleRange(fftChannelR.getFftNormData(),0,0.15);

	circleRadius = (circleRadius < soundVal) ? soundVal : circleRadius-0.03;
	ofCircle(cPos, circleRadius*100*ofMap(midiMessage.value,0,127,1,10));

	fbo.end();

	//  ---------------------------------------------------
	//  ____ ___  ____ _  _ ___    ____ ____ _  _ ___  ____ ____ 
	//  [__  |__] |  | |  |  |     [__  |___ |\ | |  \ |___ |__/ 
	//  ___] |    |__| |__|  |     ___] |___ | \| |__/ |___ |  \ 
	//                                                        
	//  ---------------------------------------------------

	spout.sendTexture(fbo.getTextureReference(), "spout_midi_test01");
}

//--------------------------------------------------------------
void ofApp::draw(){

	fbo.draw(0,0,ofGetWidth(),ofGetHeight());	// Also draw the spout-canvas in Application
	
	drawMidiUI();								// Show Midi-Inputs
    drawSamples(fftChannelR.getFftNormData());	// Show Audio-Spectrum
	visualizeSampleRange(0,0.15);
}

//--------------------------------------------------------------
void ofApp::exit() {
	spout.exit();
	soundStream.close();
}

//--------------------------------------------------------------
void ofApp::drawSamples(vector<float> samples) {

	ofPushMatrix();
    ofTranslate(0, ofGetHeight() * 1.0);
    
    int sampleWidth = ofGetWidth() / samples.size();
    int sampleHeight = ofGetHeight() / 2;
    int numOfSamples = samples.size();

    for(int i=0; i<numOfSamples; i++) {
		float sIndex = exp(log(numOfSamples)/numOfSamples*i);

        int x = ofMap(i, 0, numOfSamples-1, 0, ofGetWidth()-sampleWidth);
        int y = 0;
        int w = sampleWidth;
        int h = -samples[floor(sIndex)] * sampleHeight;
        
        ofRect(x, y, w, h);
    }

	ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::visualizeSampleRange(float minRange, float maxRange) {

	ofPushMatrix();
    ofTranslate(0, ofGetHeight());
    
	ofSetColor(0,0,255,20);
	ofRect(minRange*ofGetWidth(),0,ofGetWidth()*(maxRange-minRange), -ofGetHeight() / 2);

	ofPopMatrix();
}

float ofApp::analyzeSampleRange(vector<float> samples, float minRange, float maxRange) {
	int numOfSamples = samples.size();
	int minSample = floor(minRange*numOfSamples);
	int maxSample = floor(maxRange*numOfSamples);
	
	float volume = 0.0;
	for(int i=minSample; i < maxSample; i++) {
		float sIndex = exp(log(numOfSamples)/numOfSamples*i);
		volume += samples[floor(sIndex)];
	}
	volume /= (maxSample-minSample);
	return volume;
}

//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels) {
    
    for(int i = 0; i<bufferSize; i++) {
        samplesChannelL[i] = input[i * 2 + 0];
        samplesChannelR[i] = input[i * 2 + 1];
    }
    
    float * dataL = &samplesChannelL[0];
    float * dataR = &samplesChannelL[0];
    
    fftChannelL.audioIn(dataL);
    fftChannelR.audioIn(dataR);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void ofApp::drawMidiUI(){
		// draw the last recieved message contents to the screen
	text << "Received: " << ofxMidiMessage::getStatusString(midiMessage.status);
	ofDrawBitmapString(text.str(), 20, 20);
	text.str(""); // clear
	
	text << "channel: " << midiMessage.channel;
	ofDrawBitmapString(text.str(), 20, 34);
	text.str(""); // clear
	
	text << "pitch: " << midiMessage.pitch;
	ofDrawBitmapString(text.str(), 20, 48);
	text.str(""); // clear
	ofRect(20, 58, ofMap(midiMessage.pitch, 0, 127, 0, ofGetWidth()-40), 20);
	
	text << "velocity: " << midiMessage.velocity;
	ofDrawBitmapString(text.str(), 20, 96);
	text.str(""); // clear
	ofRect(20, 105, ofMap(midiMessage.velocity, 0, 127, 0, ofGetWidth()-40), 20);
	
	text << "control: " << midiMessage.control;
	ofDrawBitmapString(text.str(), 20, 144);
	text.str(""); // clear
	ofRect(20, 154, ofMap(midiMessage.control, 0, 127, 0, ofGetWidth()-40), 20);
	
	text << "value: " << midiMessage.value;
	ofDrawBitmapString(text.str(), 20, 192);
	text.str(""); // clear

	if(midiMessage.status == MIDI_PITCH_BEND) {
		ofRect(20, 202, ofMap(midiMessage.value, 0, MIDI_MAX_BEND, 0, ofGetWidth()-40), 20);
	}
	else {
		ofRect(20, 202, ofMap(midiMessage.value, 0, 127, 0, ofGetWidth()-40), 20);
	}
}

//--------------------------------------------------------------
void ofApp::newMidiMessage(ofxMidiMessage& msg) {

	// make a copy of the latest message
	midiMessage = msg;
}