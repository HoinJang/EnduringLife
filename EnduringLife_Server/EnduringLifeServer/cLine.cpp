#include "stdafx.h"
#include "cMyNamespace.h"
#include "cLine.h"

cLine::cLine(float x1, float y1, float x2, float y2) : x1(x1), y1(y1), x2(x2), y2(y2)
{
	if (x1 == x2)
	{
		if (y1 > y2)
		{
			normalvector_x = 1.0f;
			normalvector_y = 0.0f;
		}
		else
		{
			normalvector_x = -1.0f;
			normalvector_y = 0.0f;
		}
		type = 1;
	}
	else if (y1 == y2)
	{
		if (x1 > x2)
		{
			normalvector_x = 0.0f;
			normalvector_y = -1.0f;
		}
		else
		{
			normalvector_x = 0.0f;
			normalvector_y = 1.0f;
		}
		type = 0;
	}
}
cLine::~cLine()
{
}

bool cLine::IsInLine(float p_x, float p_y)
{
	//cout << x1 << "," << y1 << "-" << x2 << "," << y2 << endl;
	//cout << p_y << "," << p_x << endl;
	if (type == 0) // 가로
	{
		if (x1 > x2) // 0,-1
		{
			if (p_y > y1 && p_y <= y1+5 && p_x < x1 && p_x > x2)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else // 0, 1
		{
			if (p_y <= y1 && p_y > y1-5 && p_x < x2 && p_x > x1)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	else //세로
	{
		if (y1 > y2) // 1.0
		{
			if (p_x < x1 && p_x >= x1-5 && p_y < y1 && p_y > y2)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else // -1,0
		{
			if (p_x > x1 && p_x <= x1+5 && p_y < y2 && p_y >y1)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
}

