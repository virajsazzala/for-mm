#define mms_api_h
#ifdef mms_api_h

#include <string>

int mazeWidth();
int mazeHeight();

bool wallFront();
bool wallRight();
bool wallLeft();

void moveForward(int distance = 1);
void turnRight();
void turnLeft();

void setWall(int x, int y, char direction);
void clearWall(int x, int y, char direction);

void setColor(int x, int y, char color);
void clearColor(int x, int y);
void clearAllColor();

void setText(int x, int y, const std::string& text);
void clearText(int x, int y);
void clearAllText();

bool wasReset();
void ackReset();

#endif