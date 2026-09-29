#pragma once
#include "MeshBox.h"
class MovingBox2 :  public MeshBox
{
private :
float m_timer = 0.0f;
public :
	void Update();
};

