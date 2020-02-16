#pragma once
#include "animal3D/animal3D.h"
#include"a3_DemoState.h"
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

	void Render(a3_DemoState* const demoState)
	{
		a3f32 x = (a3f32)xPos;
		a3f32 y = (a3f32)yPos;
		a3f32 height = (a3f32)demoState->windowHeight;
		a3f32 width = (a3f32)demoState->windowWidth;
		if (isRed)
		{
			a3textDraw(demoState->text, (x/ width),  (y / height), -1, 1, 0, 0, 1, "%s", myString);

		}
		else
		{
			a3textDraw(demoState->text, (x/ width), (y / height), -1, 0, 0, 1, 1, "%s", myString);
		}
	}
	//void Update(a3_DemoState* const demoState);


private:
	int xPos = 0;
	int yPos = 0;
	std::string myString = "";
	bool isRed;
};