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
	static Vector2 lerp(Vector2 a = Vector2(0, 0), Vector2 b = Vector2(0, 0), float t = 1);

	float x;
	float y;
	
};