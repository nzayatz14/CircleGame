/*
 * Circle.h
 *
 *  Created on: Mar 18, 2015
 *      Author: Nick Zayatz
 */

#ifndef CIRCLE_H_
#define CIRCLE_H_
#include "../include/Angel.h"
#include <iostream>
#include<math.h>
#include<vector>


using namespace std;

class Circle{
public:
	Circle();
	Circle(double rad);
	void setVelocity(vec2 vel);
	vec2 getVelocity();
	void printPoints();
	void getPoints(vector<vec2> &pts);
	void moveCircle();
	void putCircle(vec2 transform);
	int numPoints();
	void bounceBall(double mass, vec2 vel, bool in);
	vec2 getCenter();
	double getRadius();
	void setRadius(double rad);
	void setColor(vec4 col);
	vec4 getColor();
	void eatCircle(double otherRadius);
	void setInArea(bool maybe);
	bool getInArea();
	void isInArea();
	void originalRadius();
private:
	void createCircle();
	void generateCircle(vec2 p1, vec2 p2, int level);
	void bounceWall();
	double dist(vec2 p1, vec2 p2);
	vector<vec2> points;
	double radius;
	double origRadius;
	double scaleVelocity;
	vec2 center;
	vec2 velocity;
	vec4 color;
	bool inArea;
};



#endif /* CIRCLE_H_ */
