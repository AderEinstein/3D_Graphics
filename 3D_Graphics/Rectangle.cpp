#include "Rectangle.h"

RectangleF::RectangleF(float centerX, float centerY, float radius)
{
	x = centerX - radius;
	y = centerY - radius;
	width = centerX + radius;
	height = centerY + radius;
}

bool RectangleF::OverlappWith(const RectangleF& other)
{
	return !(x > other.x + other.width || other.x > x + width ||	// Return false if the left edge of either rectangle is farther to the right than the right edge of the other
		y > other.y + other.height || other.y > y + height);		// or if the bottom edge of either rectangle is higher than the top edge of the other
}

bool RectangleF::MouseIn(float mouseX, float mouseY)
{
	return mouseX > x && mouseX < x + width 
		&& mouseY > y && mouseY < y + height;
}

