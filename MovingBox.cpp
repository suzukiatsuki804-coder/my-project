#include "MovingBox.h"

void MovingBox::Update()
{
	m_timer += 0.01f;
	//X•ûŒü‚É‰•œˆÚ“®‚·‚é
	m_position.x = 5.0f * sin(m_timer); 
}
