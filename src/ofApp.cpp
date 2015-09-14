#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofSetFrameRate(60);
	//fbo.allocate(ofGetWidth()*fboOS, ofGetHeight()*fboOS);
	fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA, 4);

	circleRadius = 0;

	//midiIn.listPorts();
	midiIn.openPort(0);

	// add testApp as a listener
	midiIn.addListener(this);
	
	// print received messages to the console
	//midiIn.setVerbose(true);

	//soundStream.listDevices();
	soundStream.setDeviceID(3);

	int channelsOut = 0;        // number of requested output channels (i.e. 2 for stereo).
    int channelsIn = 2;         // number of requested input channels.
    int sampleRate = 48000;     // requested sample rate (44100 is typical).
    int bufferSize = 512;       // requested buffer size (256 is typical).
    int numOfBuffers = 4;       // number of buffers to queue, less buffers will be more responsive, but less stable.

    soundStream.setup(this, channelsOut, channelsIn, sampleRate, bufferSize, numOfBuffers);
    
    samplesChannelL.assign(bufferSize, 0.0);
    samplesChannelR.assign(bufferSize, 0.0);
}

//--------------------------------------------------------------
void ofApp::update(){
	   
	fftChannelL.update();
    fftChannelR.update();
	
	///////////////////////
	fbo.begin();

	ofClear(0,0,0,255);
	ofSetCircleResolution(60);
	ofSetColor(255,0,0);

	float time = ofGetElapsedTimef();
	float easeFact = 0.1;
	float midiVal = ofMap(midiMessage.value,0,127,0,250);

	circleRadius = circleRadius + (midiVal-circleRadius)*easeFact;
	cPos.x = cos(time * M_TWO_PI * 2.0)*circleRadius + ofGetWidth()/2;
	cPos.y = sin(time * M_TWO_PI * 2.0)*circleRadius + ofGetHeight()/2;
	
	ofCircle(cPos, 50);

	fbo.end();
	///////////////////////


	//Spout
	//spout.sendTexture(kitten.getTextureReference(), "kitten");
	//spout.sendTexture(snow.getTextureReference(), "snow");
	spout.sendTexture(fbo.getTextureReference(), "spout_midi_test01");
}

//--------------------------------------------------------------
void ofApp::draw(){
	fbo.draw(0,0,ofGetWidth(),ofGetHeight());
	drawMidiUI();

	ofRectangle rectTop(0, 0, ofGetWidth(), ofGetHeight() * 0.5);
    ofRectangle rectBot(0, ofGetHeight() * 0.5, ofGetWidth(), ofGetHeight() * 0.5);
    
    ofPushStyle();
    
    ofSetColor(ofColor::paleGreen);
    ofRect(rectTop);
    
    ofSetColor(ofColor::paleVioletRed);
    ofRect(rectBot);
    
    ofNoFill();
    ofSetLineWidth(4);
    ofSetColor(ofColor::white);
    
    ofRect(rectTop);
    ofRect(rectBot);
    
    ofSetColor(ofColor::black);
    ofDrawBitmapString("LEFT CHANNEL", 10, 20);
    ofDrawBitmapString("RIGHT CHANNEL", 10, ofGetHeight() * 0.5 + 20);
    
    ofPopStyle();
    
    //----------------------------------------------------------
    ofPushMatrix();
    ofTranslate(0, ofGetHeight() * 0.5);
    drawSamples(fftChannelL.getFftNormData());
    ofPopMatrix();
    
    ofPushMatrix();
    ofTranslate(0, ofGetHeight() * 1.0);
    drawSamples(fftChannelR.getFftNormData());
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::newMidiMessage(ofxMidiMessage& msg) {

	// make a copy of the latest message
	midiMessage = msg;
}

//--------------------------------------------------------------
void ofApp::exit() {
	spout.exit();
}

//--------------------------------------------------------------
void ofApp::drawSamples(vector<float> samples) {
    
    int sampleWidth = ofGetWidth() / samples.size();
    int sampleHeight = ofGetHeight() / 2;
    int numOfSamples = samples.size();
    
    for(int i=0; i<numOfSamples; i++) {
        int x = ofMap(i, 0, numOfSamples-1, 0, ofGetWidth()-sampleWidth);
        int y = 0;
        int w = sampleWidth;
        int h = -samples[i] * sampleHeight;
        
        ofRect(x, y, w, h);
    }
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