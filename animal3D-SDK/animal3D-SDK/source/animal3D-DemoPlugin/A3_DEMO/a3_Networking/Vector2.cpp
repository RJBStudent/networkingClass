#include "A3_DEMO/Vector2.h"
#include <math.h>
//Operator overoads

Vector2 Vector2::operator*(float const& num)
{
	Vector2 output;
	output.x = x * num;
	output.y = y * num;

	return output;
}

Vector2& Vector2::operator*(const Vector2& val)
{
	this->x *= val.x;
	this->y *= val.y;

	return *this;
}

Vector2& Vector2::operator+=( const Vector2& val)
{
	this->x = this->x + val.x;
	this->y = this->y + val.y;

	return *this;
}

Vector2 Vector2::operator+(const Vector2& val)
{
	this->x = this->x + val.x;
	this->y = this->y + val.y;

	return *this;
}

Vector2& Vector2::operator-=(const Vector2& val)
{
	this->x = this->x - val.x;
	this->y = this->y - val.y;

	return *this;
}
Vector2 Vector2::operator-(const Vector2& val)
{
	this->x = this->x - val.x;
	this->y = this->y - val.y;

	return *this;
}

Vector2 Vector2::normalized()
{
	float dstSquared = this->x * this->x + this->y * this->y;
	float magnitude = (float)sqrt(dstSquared);

	return Vector2(this->x/magnitude, this->y/magnitude);
}