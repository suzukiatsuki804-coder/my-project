#include "Command.h"
#include "Camera.h"
#include <algorithm>

Command::Command(const CommandParam& param)
    : m_param(param)
{
}

bool Command::IsFinished() const
{
    return m_elapsedTime >= m_param.duration;
}

void Command::Update(float deltaTime, float currentTime)
{
    // 開始時間まで待つ
    if (currentTime < m_param.startTime) return;

    if (!m_isStarted)
    {
        m_isStarted = true;
    }

    if (IsFinished())
    {
        return;
    }

    m_elapsedTime += deltaTime;

    float t = (m_param.duration > 0.0f) ? (m_elapsedTime / m_param.duration) : 1.0f;
    t = std::min(t, 1.0f);

    Apply(t);
}

void Command::Apply(float t)
{
    switch (m_param.type)
    {
    case MOVE:
        ApplyMove(t);
        break;

    case ROTATE:
        ApplyRotate(t);
        break;

    case SCALE:
        ApplyScale(t);
        break;
    case CAM_MOVE:
        ApplyCamMove(t);
        break;
    case CAM_ROTATE:
        ApplyCamRotate(t);
        break;
    case FUNCTION:
        ApplyEvent(t);
        break;
    default:
        break;
    }
}

void Command::ApplyMove(float t)
{
    // 線形補間
    XMFLOAT3 pos = m_param.obj->GetPosition();

    pos.x += m_param.value.x * t;
    pos.y += m_param.value.y * t;
    pos.z += m_param.value.z * t;

    m_param.obj->SetPosition(pos);
}

void Command::ApplyRotate(float t)
{
    XMFLOAT3 rot = m_param.obj->GetRotation();

    rot.x += m_param.value.x * t;
    rot.y += m_param.value.y * t;
    rot.z += m_param.value.z * t;

    m_param.obj->SetRotation(rot);
}

void Command::ApplyScale(float t)
{
    XMFLOAT3 scale = m_param.obj->GetScale();

    scale.x += m_param.value.x * t;
    scale.y += m_param.value.y * t;
    scale.z += m_param.value.z * t;

    m_param.obj->SetScale(scale);
}

void Command::ApplyCamMove(float t)
{
    // 並行移動：カメラ位置と注視点を同じ量だけ移動する
    XMFLOAT3 delta;
    delta.x = m_param.value.x * t;
    delta.y = m_param.value.y * t;
    delta.z = m_param.value.z * t;

    Camera::Translate(delta);
}

void Command::ApplyCamRotate(float t)
{
    // 注視点を変えずに回転（Y軸回転を想定。m_param.value.y に度数を入れてください）
    float angleDelta = m_param.value.y * t;
    Camera::RotateAroundTarget(angleDelta);
}

void Command::ApplyEvent(float t)
{
    if (m_param.event != nullptr)
    {
        m_param.event();
        m_param.event = nullptr;
    }
}
