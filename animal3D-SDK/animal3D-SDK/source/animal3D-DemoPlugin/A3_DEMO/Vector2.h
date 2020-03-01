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
	Vector2& operator*(const Vector2& val);
	Vector2& operator+=(const Vector2& val);
	Vector2& operator-=(const Vector2& val);
	Vector2 operator+(const Vector2& rhs);
	Vector2 operator-(const Vector2& rhs);
	
	Vector2 normalized();

	float x;
	float y;

};