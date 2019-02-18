#pragma once

class cLine
{
private:
	float x1;
	float y1;
	float x2;
	float y2;
	float normalvector_x;
	float normalvector_y;
	int type;
	//0 : 가로, 1 : 세로
public:
	cLine(float x1, float y1, float x2, float y2);
	~cLine();
public:
	float GetNormal_X() { return normalvector_x; }
	float GetNormal_Y() { return normalvector_y; }
public:
	bool IsInLine(float p_x, float p_y);
};