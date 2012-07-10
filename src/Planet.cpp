
/*
 * Planet.cpp
 *
 *  Created on: Apr 28, 2012
 *      Author: faebser
 */

#include "Planet.h"

bool compareByLength( Resource a, Resource b)
{
	return a.getAmount() > b.getAmount();
}
// TODO fix constructors and part them
Planet::Planet() { //used for a really new planet
	this->velocity = ofRandom(-0.003, 0.003); //TODO fix velocity
	this->angle = ofRandom(0,TWO_PI);
	baseConstructor();
	// ::update and ::generateTexture will be called after creation of Planet
}
Planet::Planet(Config* config) { //used for a really new planet
	this->config = config;
	this->radius = config->getRandomPlanetRadius();
	this->velocity = ofRandom(-0.003, 0.003); //TODO fix velocity
	this->angle = ofRandom(0,TWO_PI);
	baseConstructor();
	// ::update and ::generateTexture will be called after creation of Planet
}
Planet::Planet(string planetName, Config* config) { // used for already existing planets
	this->config = config;
	this->velocity = ofRandom(-0.003, 0.003); //TODO fmove into config
	this->angle = ofRandom(0,TWO_PI);
	baseConstructor();
}
void Planet::addModificator(Modificator* newMod) {
	modificators.push_back(newMod);
}

void Planet::clearModificator() {
	/*vector<Modificator*>::iterator it = modificators.begin(), end = modificators.end();
	for(; it < end; ++it) {
		delete it;
	}*/
	modificators.clear();
}
void Planet::baseConstructor() {
	this->type = config->getRandomPlanetType();
	vector<string> resources = config->getResourceTypes();
	int maxI = resources.size();
	for(int i = 0; i < maxI;i++) {
		string resourceType = resources[i];
		this->resources.push_back(Resource(resourceType));
		if(i < 3) {
			ofFbo newFbo;
			newFbo.allocate(800, 600, GL_RGBA, 4);
			fbos.push_back(newFbo);
		}
	}
	generateTexture();
}
void Planet::getResource(Resource* incomingResource) {
	vector<Resource>::iterator it;
	for(it = this->resources.begin(); it < this->resources.end(); ++it) {
		if(incomingResource->getType() == (*it).getType()) {
			(*it).addAmount(incomingResource->getAmount());
			delete incomingResource;
			break;
		}
	}
}
void Planet::sendResource(Resource* outgoingResource, string* planetName) {
	//this->parent->relayResource(outgoingResource, planetName);
}
void Planet::update() {
	std::sort(this->resources.begin(), this->resources.end(), compareByLength); // TODO sort by amount
	angle += velocity;
	this->pos.set( sin(this->angle) * this->getResizedRadius() , cos(this->angle) * this->getResizedRadius());
	//this->generateTexture();
}
void Planet::generateTexture() {
	//fire, water, gas, rock
	int xMax = 800;
	int yMax = 600;
	float xNoise = 0, yNoise = 0;
	// gas planet = yInc = 0.1 xInc = 0.0001
	float yIncrement = 0.0005, xIncrement = 0.008;
	fbos[0].begin();
		for(int y = 0; y < yMax; y++) {
			yNoise = 0;
			xNoise += xIncrement;
			for(int x = 0 ; x < xMax; x++) {
				yNoise += yIncrement;
				float brig = ofNoise(xNoise, yNoise)*240;
				if(brig > 100 && brig < 150) {
					ofSetColor(142, 109, 86, 240);
				}
				else {
					ofSetColor(0, 0, 0, 0);
				}
				ofRect(x, y, 150, 150);
			}
		}
	fbos[0].end();
	fbos[1].begin();
			for(int y = 0; y < yMax; y++) {
				yNoise = 0;
				xNoise += xIncrement;
				for(int x = 0 ; x < xMax; x++) {
					yNoise += yIncrement;
					float brig = ofNoise(xNoise, yNoise)*240;
					if(brig < 100) {
						ofSetColor(99, 69, 39, 240);
						ofRect(x, y, 1, 1);
					}

				}
			}
	fbos[1].end();
	fbos[1].draw(0,0);
	fbos[2].begin();
				for(int y = 0; y < yMax; y++) {
					yNoise = 0;
					xNoise += xIncrement;
					for(int x = 0 ; x < xMax; x++) {
						yNoise += yIncrement;
						float brig = ofNoise(xNoise, yNoise)*240;
						if(brig > 150) {
							ofSetColor(51, 41, 32, 255);
							ofRect(x, y, 1, 1);
						}

					}
				}
	fbos[2].end();
}
void Planet::clicked(int player) {

}
void Planet::draw() {
	vector<ofFbo>::iterator it = fbos.begin(), end = fbos.end();
	int dist = 2;

	ofTranslate(pos);

	ofEnableAlphaBlending();
	glEnable(GL_DEPTH_TEST); //enable depth comparisons and update the depth buffer
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	ofScale(fbos[0].getWidth(), fbos[0].getHeight());
	glMatrixMode(GL_MODELVIEW);

	for (;it < end; ++it, dist += dist) {
		(*it).getTextureReference().bind();
			ofSphere(getSize() + dist);
		(*it).getTextureReference().unbind();
	}

	glMatrixMode(GL_TEXTURE);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_DEPTH_TEST);
	ofDisableAlphaBlending();
	ofSetColor(255);
	ofDrawBitmapString("name: " + this->planetName, 20, 20);
}
void Planet::newRound() {
	//TODO prototyp: erase and rewrite
	vector<Resource>::iterator it;
	for(it = this->resources.begin(); it < this->resources.end(); ++it) {
		(*it).addAmount(config->getRandomStartAmount());
	}
}
// getter
string* Planet::getPlanetName() {
	return &this->planetName;
}
string* Planet::getPlayerName() {
	return &this->playerName;
}
float* Planet::getRadius() {
	return &this->radius;
}
float Planet::getSize() {
	float size = 0;
	vector<Resource>::iterator it;
	for(it = this->resources.begin(); it < this->resources.end(); ++it) {
		size += (*it).getAmount();
		break;
	}
	return size;
}
float Planet::getAngle() {
	return this->angle;
}
float Planet::getVelocity() {
	return this->velocity;
}
ofVec2f Planet::getPos() { //TODO rewrite this it to make it beautiful
	return this->pos + config->getMiddle();
}
ofColor* Planet::getColor() {
	this->generateTexture();
	return &this->testColor;
}
float Planet::getResizedRadius() {
	float realMax = ofGetWindowWidth() * 0.5, realMin = 30;
	return ofMap(radius, config->getNumber("minRadius"), config->getNumber("maxRadius"), realMin, realMax);
}
float Planet::getResourceValueAsPercent(string resName) {
	float total = 0, amount = 0;
	vector<Resource>::iterator it, end;
	it = resources.begin();
	end = resources.end();
	for(;it < end; ++it) {
		total += (*it).getAmount();
		if((*it).getType() == resName) {
			amount = (*it).getAmount();
		}
	}
	return total/amount;
}
// setter
void Planet::setPlanetName(string name) {
	this->planetName = name;
}
void Planet::setPlayerName(string name) {
	this->playerName = name;
}
void Planet::setRadius(float radius) {
	this->radius = radius;
}
void Planet::setAngle(float angle) {
	this->angle = angle;
}
void Planet::setVelocity(float v) {
	this->velocity = v;
}
void Planet::setPos(float x, float y) {
	this->pos.set(x,y);
}
Planet::~Planet() {
}

vector<Resource> Planet::getResources() const {
	return resources;
}

