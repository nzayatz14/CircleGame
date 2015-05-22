/*
 * level.h
 *
 *  Created on: Mar 27, 2015
 *      Author: Nick Zayatz
 */

#ifndef LEVEL_H_
#define LEVEL_H_
#include "../include/Angel.h"
#include "Circle.h"
#include <iostream>
#include<math.h>
#include<vector>
#include<cstdlib>

namespace std {

class level {
public:
	level();
	void nextLevel(vector<Circle> &circs);
	int getNum();
	void addToScore(int add);
	int getScore();
	void resetGame();
private:
	int levelNum;
	int currentScore;
	vector<Circle> circles;
	void createLevel();
	void generatePoints(double pts[8], int level ,double distance);
	void generateVelocitys(double pts[8], double vel[8], double maxSize);

};

} /* namespace std */

#endif /* LEVEL_H_ */
