#pragma once
#include "MeshBox.h"
class MovingBox :  public MeshBox
{
private:
	float m_timer = 0.0f;  //タイマー
public :
	//MeshBoxクラスから、この関数だけを上書きするInit～Draw入ってる
	void Update() override;
};

