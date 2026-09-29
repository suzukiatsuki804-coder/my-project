#include "MovingBox2.h"

void MovingBox2::Update()
{
	m_timer -= 0.03f;

	m_position.y = 5.0 * sinf(m_timer);
}
