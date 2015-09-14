#pragma once

#include "ofMain.h"
#include "ofxSpout2.h"
#include "ofxMidi.h"
#include "ofxFFTBase.h"


class ofApp : public ofBaseApp, public ofxMidiListener{

	public:
		void setup();
		void update();
		void draw();

		void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void drawMidiUI();

		ofFbo fbo;
		ofxSpout2 spout;

		ofPoint cPos;
		float movingRadius;
		float circleRadius;

		ofxMidiIn	midiIn;
		ofxMidiMessage midiMessage;
		void newMidiMessage(ofxMidiMessage& eventArgs);
	
		stringstream text;

		void audioIn(float * input, int bufferSize, int nChannels);
		void drawSamples(vector<float> samples);
		float analyzeSampleRange(vector<float> samples, float minRange, float maxRange);
		void visualizeSampleRange(float minRange, float maxRange);
		ofSoundStream soundStream;
		vector<float> samplesChannelL;
		vector<float> samplesChannelR;
    
		ofxFFTBase fftChannelL;
		ofxFFTBase fftChannelR;
};
