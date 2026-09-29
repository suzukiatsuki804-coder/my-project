#pragma once
#include "GameObject.h"
#include <math.h>
#include <algorithm>
#include <functional>
enum eActionType
{
    NONE,
    MOVE,
    ROTATE,
    SCALE,
    CAM_MOVE,        // カメラ移動
    CAM_ROTATE,    // 回転
    FUNCTION,       // 登録されたイベントを実行
};

struct CommandParam
{
    float startTime;              // 開始時間
    eActionType type;
    CGameObject* obj;
    XMFLOAT3 value;          // 移動量 / 回転量 / スケール量
    float duration;          // かける時間
    std::function<void()> event;    // イベント
};

class Command
{
private:
    CommandParam m_param;
    float m_elapsedTime = 0.0f;
    bool m_isStarted = false;

public:
    Command(const CommandParam& param);

    bool IsFinished() const;

    void Update(float deltaTime, float currentTime);

private:
    void Apply(float t);

    void ApplyMove(float t);
    void ApplyRotate(float t);
    void ApplyScale(float t);
    void ApplyCamMove(float t);
    void ApplyCamRotate(float t);
    void ApplyEvent(float t);
};