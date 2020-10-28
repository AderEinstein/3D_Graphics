#pragma once

class RectangleF
{
public:
	RectangleF(float centerX, float centerY, float radius);	// create a binding box from a sphere/circle
	bool OverlappWith(const RectangleF& other);
	bool MouseIn(float mouseX, float mouseY);
	~RectangleF() = default;

public:
	float x;
	float y;
	float width;
	float height;
};

