//============================================================================
// Name        : CircleGame.cpp
// Author      : Nick Zayatz
// Version     :
// Class       : CSC-3360
// Description : Main C++ File for the Circle Game
//============================================================================

#include <iostream>
#include <fstream>
#include "Circle.h"
#include "../include/Angel.h"
#include "level.h"

#define NUM_HIGH_SCORES 5

using namespace std;

//variables pertaining to the circles and game play
vector<vec2> points;
vector<Circle> circles;
Circle playerCircle(.1);
level currentLevel;
int totalPoints;
int *pointsPerCircle;

//hold the world coordinates
const GLfloat wld_left = -1.0;
const GLfloat wld_right = 1.0;
const GLfloat wld_bot = -1.0;
const GLfloat wld_top = 1.0;
const GLfloat wld_near = -1.0;
const GLfloat wld_far = 1.0;

GLfloat curr_wld_left = wld_left;
GLfloat curr_wld_right = wld_right;
GLfloat curr_wld_bot = wld_bot;
GLfloat curr_wld_top = wld_top;
GLfloat curr_wld_near = wld_near;
GLfloat curr_wld_far = wld_far;

//variables pertaining to if the game is paused or the high scores
bool paused = false;
bool lost = false;
bool highScoreEntered = false;
int highScores[] = {-12,-12,-12,-12,-12};
string highScoreHolders[] = {"(blank)","(blank)","(blank)","(blank)","(blank)"};

//create mat4's and locations
mat4 p, mv;

GLint projLoc;
GLint mvLoc;
GLint scaleLoc;
GLint positionLoc;
GLint vColor;

//hold the current width and height of the screen
GLint width = 512, height = 512;

void init();
void initFunctions();
void display( void );
void keyboard( unsigned char key, int x, int y );
void updateScreen();
void followMouse(int x, int y);
void myReshape(GLint w, GLint h);
void checkCollides();
double dist(vec2 p1, vec2 p2);
void getWorldFromPixel(int x, int y);
void collideWithUser();
void actionBasedOnCircle(Circle &c);
void checkWin();
void leaveScreen(int state);
void myMenu(int item);
void set_up_menu( void (*myMenu)(int));
void loadHighScores();
void saveNewHighScore();
void printHighScores();
void checkForHighScore();
void printDirections();
void loadShaderProperties();
void printMessages();
void generateMessages(char levelMessage[35], char scoreMessage[35], char lostMessage[35]);

int main(int argc,  char **argv ) {

	//start the first level and load the high scores
	currentLevel.nextLevel(circles);
	loadHighScores();

	//set user circle properties
	playerCircle.setColor(vec4(0.0,1.0,0.0,1.0));
	playerCircle.setInArea(true);

	//print directions
	printDirections();

	//init display window
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize( width, height );
	glutCreateWindow( "Circle Game" );
	glutSetCursor(GLUT_CURSOR_NONE);

	//compatibility issues with the libraries and drivers installed.
#ifndef __APPLE__
	glewExperimental = GL_TRUE;
	glewInit();
#endif

	init();
	initFunctions();

	glutMainLoop();

	return 0;
}


/**
 * Init functions to handle screen actions.
 *
 * :param: void
 * :returns: void
 * */
void initFunctions(){
	glutDisplayFunc( display );
	glutKeyboardFunc( keyboard );
	glutIdleFunc(updateScreen);
	glutPassiveMotionFunc(followMouse);
	glutMotionFunc(followMouse);
	glutReshapeFunc(myReshape);
	glutEntryFunc(leaveScreen);
	set_up_menu(myMenu);
}


/**
 * Init function to set attributes to the screen.
 *
 * :param: void
 * :returns: void
 * */
void init(){

	totalPoints = circles.at(0).numPoints();

	unsigned int vertsBytes = totalPoints * sizeof(vec2);

	vec2 *points2 = new vec2[totalPoints];

	//copy all other points circles over to arrays
	circles.at(0).getPoints(points);

	//copy calculated points in from vector to array to be used in plotting
	for(int i = 0;i<totalPoints;i++){
		points2[i] = points.at(i);
	}

	// Create a vertex array object
	GLuint vao[1];
#ifdef __APPLE__
	glGenVertexArraysAPPLE( 1, vao );
	glBindVertexArrayAPPLE( vao[0] );
#else
	glGenVertexArrays( 1, vao );
	glBindVertexArray( vao[0] );
#endif

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers( 1, &buffer );
	glBindBuffer( GL_ARRAY_BUFFER, buffer );
	glBufferData( GL_ARRAY_BUFFER, vertsBytes, points2, GL_STATIC_DRAW );

	//load the shader proerties
	loadShaderProperties();
}


/**
 * Loads the properties of the shader to be adjusted later.
 *
 * :param: void
 * :returns: void
 * */
void loadShaderProperties(){
	// Load shaders and use the resulting shader program
	GLuint program = InitShader( "vshader21.glsl", "fshader21.glsl" );
	glUseProgram( program );

	projLoc = glGetUniformLocation(program,"p");
	mvLoc = glGetUniformLocation(program,"mv");

	vColor = glGetUniformLocation( program, "fColor" );

	scaleLoc = glGetUniformLocation(program, "scale");
	positionLoc = glGetUniformLocation(program, "pos");

	// Initialize the vertex position attribute from the vertex shader
	GLuint loc = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( loc );
	glVertexAttribPointer( loc, 2, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(0) );


	glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background

	p = Ortho(wld_left,wld_right,wld_bot,wld_top,wld_near,wld_far);
}


/**
 * Function to display the screen.
 *
 * :param: void
 * :returns: void
 * */
void display( void )
{
	glClear( GL_COLOR_BUFFER_BIT );     // clear the window

	for(int i = 0;i<(int)circles.size();i++){
		glUniform1f(scaleLoc,circles.at(i).getRadius());
		glUniform2fv(positionLoc,1,circles.at(i).getCenter());
		glUniform4fv(vColor,1,circles.at(i).getColor());
		glDrawArrays( GL_POLYGON, 0, totalPoints );
	}

	glUniform1f(scaleLoc,playerCircle.getRadius());
	glUniform2fv(positionLoc,1,playerCircle.getCenter());
	glUniform4fv(vColor,1, playerCircle.getColor());
	glDrawArrays( GL_POLYGON, 0, totalPoints);

	//reset scale and position matrices
	glUniform1f(scaleLoc,1);
	glUniform2fv(positionLoc,1,vec2(0,0));
	glUniform4fv(vColor,1, vec4(1.0,0,1.0,1.0));

	printMessages();

	glutSwapBuffers();
}


/**
 * Print the messages to the screen
 *
 * :param: void
 * :returns: void
 * */
void printMessages(){
	char levelMessage[35];
	char scoreMessage[35];
	char lostMessage[35];

	//generate the messages
	generateMessages(levelMessage,scoreMessage,lostMessage);

	//display the message for what level the user is on / if the game is paused
	glRasterPos2f(-.9, -0.9);
	int i = 0;
	while(levelMessage[i]){
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, levelMessage[i]);
		i++;
	}

	glRasterPos2f(.7, -0.9);
	i = 0;
	while(scoreMessage[i]){
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, scoreMessage[i]);
		i++;
	}

	//display the message if the user has lost
	if(lost){
		glRasterPos2f(.5, 0.9);
		i = 0;
		while(lostMessage[i]){
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, lostMessage[i]);
			i++;
		}
	}
}


/**
 * Generates the messages to be printed to the screen.
 *
 * :param: levelMessage - the string to display the current level
 * :param: scoreMessage - the string to display the current score
 * :param: lostMessage - the string to display if the user has lost
 * :returns: the 3 strings to be displayed (by reference)
 * */
void generateMessages(char levelMessage[35], char scoreMessage[35], char lostMessage[35]){

	if(!paused){
		sprintf(levelMessage, "Level: %d", currentLevel.getNum());
	}else{
		sprintf(levelMessage, "Paused");
	}

	//set the message for the score of the game
	sprintf(scoreMessage, "%d", currentLevel.getScore());

	//set the lost message
	sprintf(lostMessage, "You lose :(");
}


/**
 * Function to detect keyboard gestures.
 *
 * :param: key - the key that was pressed
 * :param: x - the x-coordinate of the mouse at that time
 * :param: y - the y-coordinate of the mouse at that time
 * :returns: void
 * */
void keyboard( unsigned char key, int x, int y )
{
	//if the escape key or q key is hit, end the program
	//if p is hit, pause the game
	switch ( key ) {
	case 'p':
	case 'P':{
		paused = !paused;
		break;
	}
	case 033:
	case 'q':
	case 'Q':
		if(!highScoreEntered)
			checkForHighScore();
		saveNewHighScore();
		exit( EXIT_SUCCESS );
		break;

	}
}


/**
 * Idle function to update the screen.
 *
 * :param: void
 * :returns: void
 * */
void updateScreen(){

	//if the game is playing, update the positions of the circles
	if(!paused && !lost){
		for(int j = 0;j<(int)circles.size();j++){

			circles.at(j).moveCircle();

			if(circles.at(j).getRadius()<=playerCircle.getRadius()){
				circles.at(j).setColor(vec4(1.0,0,0,1.0));
			}else{
				circles.at(j).setColor(vec4(0,0,1.0,1.0));
			}
		}

		//check to see if there are any collisions
		checkCollides();
		collideWithUser();

		//redraw the scene
		glutPostRedisplay();
	}
}


/**
 * Function to detect passive mouse gestures.
 *
 * :param: x - the x-coordinate of the mouse at that time
 * :param: y - the y-coordinate of the mouse at that time
 * :returns: void
 * */
void followMouse(int x, int y){
	getWorldFromPixel(x,y);
}


/**
 * Gets the world coordinates from pixel coordinates.
 * Adjusts the user circle to put its center where the mouse is.
 *
 * :param: x - the pixel x-coordinate
 * :param: y - the pixel y-coordinate
 * :returns: void
 * */
void getWorldFromPixel(int x, int y){

	//get the number of pixels per 1 unit of measure in world coordinates
	double pixelsPerX = width/(curr_wld_right - curr_wld_left);
	double pixelsPerY = height/(curr_wld_top - curr_wld_bot);

	double newX = x/pixelsPerX - 1;
	double newY = 1 - y/pixelsPerY;

	vec2 newCenter(newX,newY);
	playerCircle.putCircle(newCenter);
}


/**
 * Check to see if the user collides with another circle.
 *
 * :param: void
 * :returns: void
 * */
void collideWithUser(){
	vec2 circle1;

	//check each circle
	for(int i = 0;i<(int)circles.size();i++){
		circle1 = circles.at(i).getCenter();

		double distance = dist(circle1,playerCircle.getCenter());

		//if there is a collision, check what to do
		if(distance <= (circles.at(i).getRadius()+playerCircle.getRadius())){
			actionBasedOnCircle(circles.at(i));
		}
	}
}


/**
 * Determines which action to take if the user collides with another circle.
 *
 * :param: c - the circle the user collided with
 * :returns: void
 * */
void actionBasedOnCircle(Circle &c){

	//if the circle is bigger, you lose, if not, eat it
	if(c.getRadius() > playerCircle.getRadius()){
		lost = true;
		checkForHighScore();
	}else if (c.getRadius() <= playerCircle.getRadius() && c.getRadius() > 0) {
		playerCircle.eatCircle(c.getRadius());
		currentLevel.addToScore((int)(c.getRadius()*1000));
		c.setRadius(0);
		checkWin();
	}
}


/**
 * Resizes the maze while maintaining its proportion.
 *
 * :param: w - the width of the screen
 * :param: h - the height of the screen
 * :returns: void
 * */
void myReshape(GLint w, GLint h){
	glViewport(0,0,w,h);

	width = w;
	height=  h;

	//proportionally resize the matrix
	if(w<=h){
		curr_wld_bot = wld_bot*(GLfloat)h/w;
		curr_wld_top = wld_top*(GLfloat)h/w;
	}else{
		curr_wld_left = wld_left*(GLfloat)w/h;
		curr_wld_right = wld_right*(GLfloat)w/h;
	}

	//keep the maze in the center of the screen
	p = Ortho(curr_wld_left,curr_wld_right,curr_wld_bot,curr_wld_top,curr_wld_near,curr_wld_far);

	cout<<p<<endl;
	cout<<mv<<endl;

	glUniformMatrix4fv(projLoc,1,GL_TRUE,p);
	glUniformMatrix4fv(mvLoc,1,GL_TRUE,mv);
}


/**
 * Checks to see if the user has won the level.
 *
 * :param: void
 * :returns: void
 * */
void checkWin(){
	bool won = true;

	//if all of the circles radii are 0, you won
	for(int i = 0;i<(int)circles.size();i++){
		if(circles.at(i).getRadius() != 0)
			won = false;
	}

	if (won){
		circles.clear();
		currentLevel.nextLevel(circles);
		playerCircle.originalRadius();
	}
}


/**
 * Checks to see if each circle has collided with another.
 *
 * :param: void
 * :returns: void
 * */
void checkCollides(){
	vec2 circle1;
	vec2 circle2;
	vec2 vel1;
	vec2 vel2;

	//check each pair of circles
	for(int i = 0;i<(int)circles.size();i++){
		for(int j = i;j<(int)circles.size();j++){
			if(i != j){

				circle1 = circles.at(i).getCenter() + circles.at(i).getVelocity();
				circle2 = circles.at(j).getCenter() + circles.at(j).getVelocity();
				double distance = dist(circle1,circle2);

				//if there is a collision
				if(distance <= (circles.at(i).getRadius()+circles.at(j).getRadius())){

					//change velocities
					vel1 = circles.at(i).getVelocity();
					vel2 = circles.at(j).getVelocity();

					if(circles.at(i).getInArea()){
						circles.at(i).bounceBall(circles.at(j).getRadius(), vel2, circles.at(j).getInArea());
					}

					if(circles.at(j).getInArea()){
						circles.at(j).bounceBall(circles.at(i).getRadius(), vel1, circles.at(i).getInArea());
					}
				}
			}
		}
	}
}


/**
 * Calculates the distance between 2 points.
 *
 * :param: p1 - the first point
 * :param: p2 - the second point
 * :returns: the distance between the points
 * */
double dist(vec2 p1, vec2 p2){
	return sqrt((p2.y-p1.y)*(p2.y-p1.y) + (p2.x-p1.x)*(p2.x-p1.x));
}


/**
 * If the user leaves the screen, pause the game.
 *
 * :param: state - an integer representing if the mouse is on the screen
 * :returns: void
 * */
void leaveScreen(int state){
	if(state == 1){
		paused = false;
	}else{
		paused = true;
	}
}


/**
 * Handles clicks on the menu.
 *
 * :param: item - the item clicked on the menu
 * :returns: void
 * */
void myMenu(int item){
	//1 = reset the game
	//2 = show the high scores
	//3 = quit the game
	switch (item){
	case 1:{
		circles.clear();
		playerCircle.originalRadius();
		currentLevel.resetGame();
		currentLevel.nextLevel(circles);
		lost = false;
		highScoreEntered = false;
		break;
	}
	case 2:{
		printHighScores();
		break;
	}
	case 3:{
		if(!highScoreEntered)
			checkForHighScore();
		saveNewHighScore();
		exit( EXIT_SUCCESS );
		break;
	}
	default:
		break;
	}
}


/**
 * Creates the values for the menu that will drop down on a right click.
 *
 * :param: myMenu - the function that will handle this menu
 * :returns: void
 * */
void set_up_menu(void (*myMenu)(int)){
	glutCreateMenu(myMenu);
	glutAddMenuEntry("New Game",1);
	glutAddMenuEntry("View High Scores",2);
	glutAddMenuEntry("Quit",3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}


/**
 * Load in the high scores from the text file.
 *
 * :param: void
 * :returns: void
 * */
void loadHighScores(){
	ifstream in;
	int score;
	string name;

	//read in the high scores from the file
	in.open("highScores.txt");
	int i = 0;
	while(!in.eof() && i<NUM_HIGH_SCORES){
		in>>score;
		in>>name;

		highScores[i] = score;
		highScoreHolders[i] = name;
		i++;
	}

	in.close();
}


/**
 * Save the new high scores to the text file.
 *
 * :param: void
 * :returns: void
 * */
void saveNewHighScore(){
	fstream outFile;

	//clear the current high scores
	outFile.open("highScores.txt", std::ofstream::out | std::ofstream::trunc);
	outFile.close();

	//load in the new high scores
	outFile.open("highScores.txt");
	outFile.clear();
	for(int i = 0;i<NUM_HIGH_SCORES;i++){
		outFile<<highScores[i]<<" "<<highScoreHolders[i]<<endl;
	}

	outFile.close();
}


/**
 * Print the high scores
 *
 * :param: void
 * :returns: void
 * */
void printHighScores(){
	cout<<"HIGH SCORES"<<endl;
	cout<<"-----------"<<endl;
	for(int i = 0;i<NUM_HIGH_SCORES;i++){
		cout<<highScores[i]<<" "<<highScoreHolders[i]<<endl;
	}
	cout<<endl;
}


/**
 * Check to see if the users finished game is a high score
 *
 * :param: void
 * :returns: void
 * */
void checkForHighScore(){
	int score = currentLevel.getScore();
	int place = NUM_HIGH_SCORES+1;

	//see if the user is among the high scores
	for(int i = NUM_HIGH_SCORES-1;i>=0 && score>highScores[i];i--){
		place--;
	}

	//if the user got a high score, ask for their name, and adjust the list accordingly
	if(place < NUM_HIGH_SCORES+1){
		highScoreEntered = true;
		cout<<"YOU GOT A HIGH SCORE!"<<endl<<"Type you name here to be on the leaderboard: ";
		cout<<place<<endl;
		string name;
		cin>>name;

		for(int i = NUM_HIGH_SCORES-2;i>=place-1;i--){
			highScores[i+1] = highScores[i];
			highScoreHolders[i+1] = highScoreHolders[i];
		}

		highScores[place-1] = score;
		highScoreHolders[place-1] = name;

		printHighScores();
	}
}


/**
 * Print the directions
 *
 * :param: void
 * :returns: void
 * */
void printDirections(){
	cout<<"WELCOME TO THE CIRCLE GAME"<<endl;
	cout<<"To play, manuever the your circle (the green circle) around circles that are bigger than you (blue circles)."<<endl;
	cout<<"Get your circle to touch red circles in order to eat them, this will grow your size so you will be able to eat bigger circles."<<endl;
	cout<<"Once you touch a blue circle, you lose the game."<<endl<<endl;

	cout<<"CONTROLS:"<<endl;
	cout<<"Press 'P' or move mouse off the screen. - Pause the game"<<endl;
	cout<<"Press 'Q' - Quit the game"<<endl;
	cout<<"Right Click for more options."<<endl;
}
