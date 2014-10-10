#pragma once

class BoundingBoxes
{
public:
	struct BoundingBox
	{
		XMFLOAT2 pos;
		float width;
		float height;
		BoundingBox() : pos(0.0f, 0.0f), width(0.0f), height(0.0f) {}
	};
};