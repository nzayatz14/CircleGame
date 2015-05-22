/*
 * level.cpp
 *
 *  Created on: Mar 27, 2015
 *      Author: Nick Zayatz
 */

#include "level.h"

#define VELOCITY_SCALE .001

namespace std {

/**
 * Default constructor for the level class.
 *
 * :param: void
 * :returns: void
 * */
level::level() {
	levelNum = 0;
	currentScore = 0;
}


/**
 * Generates a new list of circles to represent a level.
 *
 * :param: circs - the vector of circles
 * :returns: the list of circles of the next level (by reference)
 * */
void level::nextLevel(vector<Circle> &circs){
	levelNum++;
	circles.clear();
	createLevel();
	circs = circles;
}


/**
 * Generates a bunch of new circles for a level.
 *
 * :param: void
 * :returns: void
 * */
void level::createLevel(){

	double randSize[4];
	double positionPoints[8];
	double randVelocitys[8];

	//create 4 circles for each level the user has passed (level 1 = 4 circles, level2 = 8 circles...)
	for(int i=  0;i<levelNum;i++){

		//get the sizes for the 4 circles
		randSize[0] = (rand()%90 + 10)/1000.0 + .11*i;
		randSize[1] = (rand()%90 + 10)/1000.0 + .11*i;
		randSize[2] = (rand()%(int)(randSize[0]*100+randSize[1]*100) + 100)/1000.0 + .11*i;
		randSize[3] = (rand()%(int)(randSize[0]*100+randSize[1]*100 + randSize[2]*100) + 100)/1000.0 + .11*i;

		//get the positions of the circles
		generatePoints(positionPoints, i+1, randSize[3]);

		//get the velocities of the circles
		generateVelocitys(positionPoints, randVelocitys, randSize[3]);

		//create 4 circles based on the values received
		for(int j = 0;j<4;j++){
			Circle test(randSize[j]);
			test.setVelocity(vec2(randVelocitys[2*j], randVelocitys[2*j+1]));
			test.putCircle(vec2(positionPoints[2*j], positionPoints[2*j+1]));
			circles.push_back(test);
		}
	}
}


/**
 * Returns the current level.
 *
 * :param: void
 * :returns: the current level
 * */
int level::getNum(){
	return levelNum;
}


/**
 * Adds a score to the current score.
 *
 * :param: add - the amount to add to the score
 * :returns: void
 * */
void level::addToScore(int add){
	currentScore+=add;
}


/**
 * Returns the score of the user.
 *
 * :param: void
 * :returns: the score of the user
 * */
int level::getScore(){
	return currentScore;
}


/**
 * Resets the game level and score.
 *
 * :param: void
 * :returns: void
 * */
void level::resetGame(){
	levelNum = 0;
	currentScore = 0;
}


/**
 * Generates the center points for 4 circles.
 *
 * :param: pts - the array the points will be stored in (x1,y1,x2,y2...)
 * :param: level - the current level in the loop of circles to generate
 * :param: distance - the distance away the circles need to be set so they don't overlap
 * :returns: the center points of the 4 circles (by reference)
 * */
void level::generatePoints(double pts[8], int level ,double distance){
	//above the view
	pts[0] = ((rand()%100)/50.0 - 1);
	pts[1] = distance+((rand()%100)/100.0 + 1.5)*level;

	//to the right of the view
	pts[2] = distance+((rand()%100)/100.0 + 1.5)*level;
	pts[3] = ((rand()%100)/50.0 - 1);

	//to the left of the view
	pts[4] = level*((rand()%100)/(-100.0) - 1.5);
	pts[5] = ((rand()%100)/50.0 - 1);

	//below the view
	pts[6] = ((rand()%100)/50 - 1);
	pts[7] = level*((rand()%100)/(-100.0) - 1.5);
}


/**
 * Generates the velocities of the circles.
 *
 * :param: pts - the array the points of the circles (x1,y1,x2,y2...)
 * :param: vel - the array the velocities will be stored in (Vx1,Vy1,Vx2,Vy2...)
 * :param: maxSize - the maximum size of any of the circles
 * :returns: the velocities of the 4 circles (by reference)
 * */
void level::generateVelocitys(double pts[8], double vel[8], double maxSize){

	for(int i = 0;i<8;i+=2){
		//get some point in the view to shoot the circle towards, setting the correct direction
		double moveToX = (rand()%100)/(50.0 /(1- maxSize)) - 1 + maxSize;
		double moveToY = (rand()%100)/(50.0 /(1- maxSize)) - 1 + maxSize;

		//scale the velocity to a reasonable size
		double scaleVel = (rand()%1000)/100000.0 + .007;
		double magnitude = sqrt((moveToX - pts[i])*(moveToX - pts[i]) + (moveToY - pts[i+1])*(moveToX - pts[i+1]));

		//set the velocities in the array
		vel[i] = scaleVel*(moveToX - pts[i])/magnitude;
		vel[i+1] = scaleVel*(moveToY - pts[i+1])/magnitude;
	}
}

} /* namespace std */
