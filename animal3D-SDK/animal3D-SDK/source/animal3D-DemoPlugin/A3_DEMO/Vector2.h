#pragma once

class Vector2
{
public:
	
	Vector2(float xVal = 0, float yVal = 0)
	{
		x = xVal;
		y = yVal;
	}
	~Vector2()
	{

	}

	Vector2 operator*(float const &num);
	Vector2 operator+=(Vector2 const& val);

	float x;
	float y;

};