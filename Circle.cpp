//============================================================================
// Name        : Circle.cpp
// Author      : Nick Zayatz
// Version     :
// Copyright   : Your copyright notice
// Description : Circle Class
//============================================================================

#include "Circle.h"

//constants for pi and the standard radius of the circle
#define PI 3.141593
#define STANDARD_RADIUS 0.1

using namespace std;


/**
 * Default constructor for the circle class.
 *
 * :param: void
 * :returns: void
 * */
Circle::Circle(){
	radius = STANDARD_RADIUS;
	origRadius = STANDARD_RADIUS;
	color = vec4( 0.0, 0.0, 1.0, 1.0 );
	scaleVelocity = 1;
	createCircle();
	inArea = false;
}


/**
 * Constructor for the circle class with a given radius.
 *
 * :param: radius - the radius of the circle being constructed
 * :returns: void
 * */
Circle::Circle(double rad) : radius(rad){

	//if someone creates a circle of radius 0 or less, set its radius to .1
	if(radius >0){
		radius = rad;
		origRadius = rad;
	}else{
		radius = STANDARD_RADIUS;
		origRadius = STANDARD_RADIUS;
	}

	//set the color of the circle based on its size relative to the standard circle size
	if(radius >= STANDARD_RADIUS){
		color = vec4( 0.0, 0.0, 1.0, 1.0 );
	}else{
		color = vec4( 1.0, 0.0, 0.0, 1.0 );
	}

	scaleVelocity = 1;
	inArea = false;

	createCircle();
}


/**
 * Generates the points of the circle.
 *
 * :param: void
 * :returns: void
 * */
void Circle::createCircle () {

	int levels = 4;

	vec2 startPoints[4];

	//create initial points of square
	startPoints[0].x = 1;
	startPoints[0].y = 0;
	startPoints[1].x = 0;
	startPoints[1].y = 1;
	startPoints[2].x = -1;
	startPoints[2].y = 0;
	startPoints[3].x = 0;
	startPoints[3].y = -1;

	//push initial points into the vector then call calculation function for each of the starting 4 line segments
	points.push_back(startPoints[0]);
	generateCircle(startPoints[0], startPoints[1],levels);
	points.push_back(startPoints[1]);
	generateCircle(startPoints[1], startPoints[2],levels);
	points.push_back(startPoints[2]);
	generateCircle(startPoints[2], startPoints[3],levels);
	points.push_back(startPoints[3]);
	generateCircle(startPoints[3], startPoints[0],levels);
}



/**
 * Creates a circle out of a beginning square with the precision based on the variable (levels)
 * the higher levels is, the more circular the object will be. Adds these points to the points vector in counter-clockwise fashion.
 *
 *	:param: p1 - the starting point of the line that is being split
 *	:param: p2 - the ending point of the line that is being split
 *	:param: level - the current level of recursion
 *	:returns: void
 */
void Circle::generateCircle(vec2 p1, vec2 p2, int level){

	//if there are no more levels, return
	if(level == 0){
		return;
	}else{

		//if there are more levethe number of points the circle hasls, find the midpoint
		vec2 mid;

		mid.x = (p1.x+p2.x)/2;
		mid.y = (p1.y+p2.y)/2;

		//get the distance from the origin, then normalize it accordingly
		vec2 origin;
		origin.x = 0;
		origin.y = 0;

		double lengthOfAdd = dist(origin, mid);

		mid.x = mid.x/lengthOfAdd;
		mid.y = mid.y/lengthOfAdd;

		//recursively get the next 2 points from the 2 new segments
		generateCircle(p1, mid, level-1);
		points.push_back(mid);
		generateCircle(mid, p2, level-1);
	}
}


/**
 * Takes in 2 points and calculates the distance between them.
 *
 * :param: p1 - the first point
 * :param: p2 - the second point
 * :returns: the distance between the 2 points
 */
double Circle::dist(vec2 p1, vec2 p2){
	return sqrt((p2.y-p1.y)*(p2.y-p1.y) + (p2.x-p1.x)*(p2.x-p1.x));
}


/**
 * Prints the points of the circle.
 *
 * :param: void
 * :returns: void
 * */
void Circle::printPoints(){
	for(int i = 0;i<(int)points.size();i++){
		cout<<"("<<points.at(i).x<<","<<points.at(i).y<<")"<<endl;
	}
}


/**
 * puts the points of the circle into a vector passed by reference
 *
 * :param: pts - the vector for the points to be loaded into
 * :returns: (by reference) the points of the circle
 * */
void Circle::getPoints(vector<vec2> &pts){
	pts.clear();
	pts = points;

}


/**
 * Moves the circle by 1 unit of velocity.
 *
 * :param: void
 * :returns: void
 * */
void Circle::moveCircle(){
	center = center + velocity;
	bounceWall();
	isInArea();
}


/**
 * Sets the velocity of the circle and adjusts the scale of the velocity to fit the new velocity.
 *
 * :param: vel - the new velocity
 * :returns: void
 * */
void Circle::setVelocity(vec2 vel){
	//set the new velocity
	velocity = vel;

	//scale the velocity accordingly
	scaleVelocity = sqrt(vel.x*vel.x + vel.y*vel.y);
}


/**
 * Gets the velocity of the circle.
 *
 * :param: void
 * :returns: The velocity of the circle
 * */
vec2 Circle::getVelocity(){
	return velocity;
}


/**
 * Places the center of the circle at the point inputted.
 *
 * :param: transform - where you want to place the circle
 * :returns: void
 * */
void Circle::putCircle(vec2 transform){
	center = transform;
}


/**
 * Adjusts the velocity if the ball was to bounce off of a wall.
 *
 * :param: void
 * :returns: void
 * */
void Circle::bounceWall(){

	//only bounce off the walls if the circle is in thye area
	if(inArea){
		//right wall
		if((center.x + velocity.x + radius) > 1){
			center.x = 1-radius;
			velocity.x = -1*velocity.x;
		}

		//left wall
		if((center.x + velocity.x - radius) < -1){
			center.x = -1+radius;
			velocity.x = -1*velocity.x;
		}

		//top wall
		if((center.y + velocity.y + radius) > 1){
			center.y = 1-radius;
			velocity.y = -1*velocity.y;
		}

		//bottom wall
		if((center.y + velocity.y - radius) < -1){
			center.y = -1+radius;
			velocity.y = -1*velocity.y;
		}
	}

}


/**
 * Returns the number of points the circle has.
 *
 * :param: void
 * :returns: the number of points the circle has
 * */
int Circle::numPoints(){
	return points.size();
}


/**
 * Bounces this ball off of another ball.
 *
 * :param: mass - the mass of the other ball
 * :param: vel - the velocity of the other ball
 * :param: in - boolean to see if the other ball is fully in the area
 * :returns: void
 * */
void Circle::bounceBall(double mass, vec2 vel, bool in){
	//calculate the new direction of the velocity of this ball
	velocity.x = (velocity.x*(radius-mass) + 2*mass*vel.x)/(radius+mass);
	velocity.y = (velocity.y*(radius-mass) + 2*mass*vel.y)/(radius+mass);

	//if the other ball is in the area, scale the velocity based on the newly calculated velocity
	//if not, maintain the same speed of the ball to conserve momentum, just using the new direction
	if(in){
		scaleVelocity = sqrt(velocity.x*velocity.x + velocity.y*velocity.y);
	}else{
		velocity = (velocity*scaleVelocity) / sqrt(velocity.x*velocity.x + velocity.y*velocity.y);
	}
}


/**
 * Returns the center of the circle.
 *
 * :param: void
 * :returns: the center of the circle
 * */
vec2 Circle::getCenter(){
	return center;
}


/**
 * Sets the circle to a new radius.
 *
 * :param: rad - the new radius
 * :returns: void
 * */
void Circle::setRadius(double rad){
	radius = rad;
}


/**
 * Returns the radius of the circle.
 *
 * :param: void
 * :returns: the radius of the circle
 * */
double Circle::getRadius(){
	return radius;
}


/**
 * Sets the color of the circle.
 *
 * :param: col - the new color
 * :returns: void
 * */
void Circle::setColor(vec4 col){
	color = col;
}


/**
 * Returns the color of the circle.
 *
 * :param: void
 * :returns: the color of the circle
 * */
vec4 Circle::getColor(){
	return color;
}


/**
 * Recalculates the area of the circle based of if the circle eats another circle
 *
 * :param: otherRadius - the radius of the circle that has just been consumed
 * :returns: void
 * */
void Circle::eatCircle(double otherRadius){

	//calcuate the area of the other circle, add it to the radius of this circle, then calculate the new radius
	double otherArea = PI*otherRadius*otherRadius;

	double area = PI*radius*radius;

	double newArea = area+otherArea;

	radius = sqrt(newArea/PI);
}


/**
 * Manually sets if the circle is in the area.
 *
 * :param: maybe - the boolean to represent if the circle is in the area
 * :returns: void
 * */
void Circle::setInArea(bool maybe){
	inArea = maybe;
}


/**
 * Returns if the circle is in the area.
 *
 * :param: void
 * :returns: boolean to represent if the circle is in the area
 * */
bool Circle::getInArea(){
	return inArea;
}


/**
 * Function to determine if the circle is in the area.
 *
 * :param: void
 * :returns: void
 * */
void Circle::isInArea(){
	//if the circle is inside the 4 walls, it is in the area
	if((center.x + velocity.x + radius) < 1 && (center.x + velocity.x - radius) > -1 && (center.y + velocity.y + radius) < 1 && (center.y + velocity.y - radius) > -1){
		inArea = true;
	}
}


/**
 * Sets the current radius to the starting radius of the circle.
 *
 * :param: void
 * :returns: void
 * */
void Circle::originalRadius(){
	radius = origRadius;
}
