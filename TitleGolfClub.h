#pragma once
#include "Sprite.h"
class TitleGolfClub : public Sprite
{
private:
    float m_timer = 0.0f;
public:
    void Update() override;
};

