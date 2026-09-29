#pragma once
#include <DirectXMath.h>
#include "Command.h"
using namespace DirectX;
#include <vector>

class Sequencer
{
private:
    std::vector<Command> m_commands;
    float m_time = 0.0f;

public:
    void Init()
    {
        m_time = 0.0f;
    }

    void UnInit()
    {
        m_commands.clear();
    }

    void Update(float deltaTime)
    {
        m_time += deltaTime;

        for (auto& cmd : m_commands)
        {
            cmd.Update(deltaTime, m_time);
        }
    }
    void Reset()
    {
        m_time = 0.0f;
        m_commands.clear();
    }
    void AddCommand(const CommandParam& param)
    {
        m_commands.emplace_back(param);
    }
};
