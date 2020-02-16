#pragma once
#include <string>

class GameObject
{
public:
	GameObject(int x = 0, int y = 0, std::string newString = "", bool red = false)
	{
		xPos = x;
		yPos = y;
		myString = newString;
		isRed = red;
	}

	int getX() { return xPos; }
	int getY() { return yPos; }
	std::string getString() { return myString; }
	bool getRed() { return isRed; }

	void setX(int x) { xPos = x; }
	void setY(int y) { yPos = y; }
	void setString(std::string newString) { myString = newString; }
	void setRed(bool newIsRed) { isRed = newIsRed; }

private:
	int xPos = 0;
	int yPos = 0;
	std::string myString = "";
	bool isRed;
};