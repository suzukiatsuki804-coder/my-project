#include "Collision.h"
#include<cmath>
#include<algorithm>

//玉と玉の衝突判定
bool Collision::CheckBoundingSphere(MeshSphere* s1, MeshSphere* s2)
{
    //各玉の位置とスケールを取得
    XMFLOAT3 pos1 = s1->GetPosition();
    XMFLOAT3 scale1 = s1->GetScale();
    XMFLOAT3 pos2 = s2->GetPosition();
    XMFLOAT3 scale2 = s2->GetScale();

    //玉の半径を計算(スケールの最大値を使用)
    float radius1 = std::max({ scale1.x, scale1.y, scale1.z });
    float radius2 = std::max({ scale2.x, scale2.y, scale2.z });

    //球の中心部の距離を計算
    //はずはベクトルの差分をもとめる
    float dx = pos1.x - pos2.x;
    float dy = pos1.y - pos2.y;
    float dz = pos1.z - pos2.z;
    float distance = std::sqrt(dx * dx + dy * dy + dz * dz); //距離＝ベクトルの長さ=√(x^2 + y^2 + z^2)

    //衝突判定：距離が半径の合計以下であれば衝突している
    return distance <= radius1 + radius2;
}

//AABB同士の衝突判定
bool Collision::CheckAABB(MeshBox* b1, MeshBox* b2)
{
    //各ボックスの位置とスケールを取得
    XMFLOAT3 pos1 = b1->GetPosition();
    XMFLOAT3 scale1 = b1->GetScale();
    XMFLOAT3 pos2 = b2->GetPosition();
    XMFLOAT3 scale2 = b2->GetScale();

    //ボックスの幅、高さ、深さを選択
    float halfWidth1 = scale1.x;
    float halfHeight1 = scale1.y;
    float halfDepth1 = scale1.z;
    float halfWidth2 = scale2.x;
    float halfHeight2 = scale2.y;
    float halfDepth2 = scale2.z;

    //各軸での衝突判定
    //それぞれの軸で、中心間の距離が半分の幅の合計より大きい場合は衝突していない
    if (std::abs(pos1.x - pos2.x) > halfWidth1 + halfWidth2)
        return false;
    if (std::abs(pos1.y = pos2.y) > halfHeight1 + halfHeight2)
        return false;
    if (std::abs(pos1.z - pos2.z) > halfDepth1 + halfDepth2)
        return false;

    //衝突している
    return false;
}

//玉と箱の当たり判定
bool Collision::CheckBallAndBox(MeshSphere* s1, MeshBox* b1)
{
    //球の位置とスケールを取得
    XMFLOAT3 spherePos = s1->GetPosition();
    XMFLOAT3 sphereScale = s1->GetScale();
    //最大値を半径と定義
    float radius = std::max({ sphereScale.x, sphereScale.y, sphereScale.z });

    //ボックスの位置とスケールを取得
    XMFLOAT3 boxPos = b1->GetPosition();
    XMFLOAT3 boxScale = b1->GetScale();
    //ボックスの半分の幅、高さ、深さを計算(本来は2分の１する)
    float halfWidth = boxScale.x;
    float halfHeight = boxScale.y;
    float halfDepth = boxScale.z;
    //玉の中心を基準に
    float closestX = spherePos.x;
    float closestY = spherePos.y;
    float closestZ = spherePos.z;

    //ボックスの中で最も近い点を探す （クランプ）
    if (closestX < boxPos.x - halfWidth)
        closestX = boxPos.x - halfWidth;
    if (closestX > boxPos.x + halfWidth)
        closestX = boxPos.x + halfWidth;
    if (closestY < boxPos.y - halfHeight)
        closestY = boxPos.y - halfHeight;
    if (closestY > boxPos.y + halfHeight)
        closestY = boxPos.y + halfHeight;
    if (closestZ < boxPos.z - halfDepth)
        closestZ = boxPos.z - halfDepth;
    if (closestZ > boxPos.z + halfDepth)
        closestZ = boxPos.z + halfDepth;

    //最も近い点から球の中心までの距離を計算
    float dx = spherePos.x - closestX;
    float dy = spherePos.y - closestY;
    float dz = spherePos.z - closestZ;
    //距離＝ベクトルの長さ＝√(x^2 + y^2 + z^2)
    float distance = std::sqrt(dx * dx + dy * dy + dz * dz);

    //衝突判定　距離が半径以下であれば衝突している
    return distance <= radius;
}

//玉とAABBの衝突後の反射処理
void Collision::Refrection(MeshSphere* sphere, MeshBox* box)
{
    //玉の位置とスケール取得
    XMFLOAT3 spherePos = sphere->GetPosition();
    XMFLOAT3 sphereScale = sphere->GetScale();
    //半径
    float radius = std::max({ sphereScale.x, sphereScale.y,sphereScale.z });

    //ボックスの位置とスケール取得
    XMFLOAT3 boxPos = box->GetPosition();
    XMFLOAT3 boxScale = box->GetScale();

    float halfWidth = boxScale.x;
    float halfHeight = boxScale.y;
    float halfDepth = boxScale.z;
    //玉の位置を基準として,ボックスの中で最も近い点を探す（クランプ）上のコードの省略
    float closestX = std::max(boxPos.x - halfWidth, std::min(spherePos.x, boxPos.x + halfWidth));
    float closestY = std::max(boxPos.y - halfHeight, std::min(spherePos.y, boxPos.y + halfHeight));
    float closestZ = std::max(boxPos.z - halfDepth, std::min(spherePos.z, boxPos.z + halfDepth));

    //最も近い点から球の中心までの距離を計算
    float dx = spherePos.x - closestX;
    float dy = spherePos.y - closestY;
    float dz = spherePos.z - closestZ;

    //距離＝ベクトルの長さ＝√(x^2 + y^2 + z^2)
    float distance = std::sqrt(dx * dx + dy * dy + dz * dz);
    if (distance < 0.00001f) return;  //0.00001f
    //法線ベクトル計算（距離を正規化）
    XMFLOAT3 normal = { dx / distance, dy / distance, dz / distance };

    //---位置修正(今の処理)　　　　衝突したときのめり込みを治す処理
    float pushDistance = radius - distance + 0.01f;  //元は0.01

    spherePos.x += normal.x * pushDistance;
    spherePos.y += normal.y * pushDistance;
    spherePos.z += normal.z * pushDistance;
    sphere->SetPosition(spherePos);
    //---ここから反射処理
    //velocityの長さチェック
    XMFLOAT3 velocity = sphere->GetVelocity();
    float len = velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z;
    if (len < 0.001f)
    {
        sphere->SetVelocity(XMFLOAT3());
        return;
    }
    //入射ベクトル・法線ベクトルの内積を計算
    //内積＝ベクトルA・ベクトルB＝|A||B|cosθ
    //normal葉単位ベクトルなので|B| = 1,入社ベクトルの長さも考慮する必要がないため単純に成分ごとに掛け算して足し合わせる
    float dot = velocity.x * normal.x + velocity.y * normal.y + velocity.z * normal.z;
    //反射ベクトル = 入射ベクトル - 2 * (入射ベクトル.法線) * 法線
    velocity.x = velocity.x - 2.0f * dot * normal.x;
    velocity.y = velocity.y - 2.0f * dot * normal.y;
    velocity.z = velocity.z - 2.0f * dot * normal.z;

    //減衰（任意）
    float restitution = 0.75f;  //1.0で完全反射 元0.8f
    velocity.x *= restitution;
    velocity.y *= restitution;
    velocity.z *= restitution;

    sphere->SetVelocity(velocity);
}

//玉とラフの衝突後の反射処理
void Collision::Refrection2(MeshSphere* sphere, MeshBox* box)
{
    //玉の位置とスケール取得
    XMFLOAT3 spherePos = sphere->GetPosition();
    XMFLOAT3 sphereScale = sphere->GetScale();
    //半径
    float radius = std::max({ sphereScale.x, sphereScale.y,sphereScale.z });

    //ボックスの位置とスケール取得
    XMFLOAT3 boxPos = box->GetPosition();
    XMFLOAT3 boxScale = box->GetScale();

    float halfWidth = boxScale.x;
    float halfHeight = boxScale.y;
    float halfDepth = boxScale.z;
    //玉の位置を基準として,ボックスの中で最も近い点を探す（クランプ）上のコードの省略
    float closestX = std::max(boxPos.x - halfWidth, std::min(spherePos.x, boxPos.x + halfWidth));
    float closestY = std::max(boxPos.y - halfHeight, std::min(spherePos.y, boxPos.y + halfHeight));
    float closestZ = std::max(boxPos.z - halfDepth, std::min(spherePos.z, boxPos.z + halfDepth));

    //最も近い点から球の中心までの距離を計算
    float dx = spherePos.x - closestX;
    float dy = spherePos.y - closestY;
    float dz = spherePos.z - closestZ;

    //距離＝ベクトルの長さ＝√(x^2 + y^2 + z^2)
    float distance = std::sqrt(dx * dx + dy * dy + dz * dz);
    if (distance < 0.00001f) return;  //0.00001f
    //法線ベクトル計算（距離を正規化）
    XMFLOAT3 normal = { dx / distance, dy / distance, dz / distance };

    //---位置修正(今の処理)　　　　衝突したときのめり込みを治す処理
    float pushDistance = radius - distance + 0.01f;  //元は0.01

    spherePos.x += normal.x * pushDistance;
    spherePos.y += normal.y * pushDistance;
    spherePos.z += normal.z * pushDistance;
    sphere->SetPosition(spherePos);
    //---ここから反射処理
    //velocityの長さチェック
    XMFLOAT3 velocity = sphere->GetVelocity();
    float len = velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z;
    if (len < 0.001f)
    {
        sphere->SetVelocity(XMFLOAT3());
        return;
    }
    //入射ベクトル・法線ベクトルの内積を計算
    //内積＝ベクトルA・ベクトルB＝|A||B|cosθ
    //normal葉単位ベクトルなので|B| = 1,入社ベクトルの長さも考慮する必要がないため単純に成分ごとに掛け算して足し合わせる
    float dot = velocity.x * normal.x + velocity.y * normal.y + velocity.z * normal.z;
    //反射ベクトル = 入射ベクトル - 2 * (入射ベクトル.法線) * 法線
    velocity.x = velocity.x - 2.0f * dot * normal.x;
    velocity.y = velocity.y - 2.0f * dot * normal.y;
    velocity.z = velocity.z - 2.0f * dot * normal.z;

    //減衰（任意）
    float restitution = 0.5f;  //1.0で完全反射 ラフはここをいじる
    velocity.x *= restitution;
    velocity.y *= restitution;
    velocity.z *= restitution;

    sphere->SetVelocity(velocity);
}

//玉とバンカーの衝突後の反射処理
void Collision::Refrection3(MeshSphere* sphere, MeshBox* box)
{
    //玉の位置とスケール取得
    XMFLOAT3 spherePos = sphere->GetPosition();
    XMFLOAT3 sphereScale = sphere->GetScale();
    //半径
    float radius = std::max({ sphereScale.x, sphereScale.y,sphereScale.z });

    //ボックスの位置とスケール取得
    XMFLOAT3 boxPos = box->GetPosition();
    XMFLOAT3 boxScale = box->GetScale();

    float halfWidth = boxScale.x;
    float halfHeight = boxScale.y;
    float halfDepth = boxScale.z;
    //玉の位置を基準として,ボックスの中で最も近い点を探す（クランプ）上のコードの省略
    float closestX = std::max(boxPos.x - halfWidth, std::min(spherePos.x, boxPos.x + halfWidth));
    float closestY = std::max(boxPos.y - halfHeight, std::min(spherePos.y, boxPos.y + halfHeight));
    float closestZ = std::max(boxPos.z - halfDepth, std::min(spherePos.z, boxPos.z + halfDepth));

    //最も近い点から球の中心までの距離を計算
    float dx = spherePos.x - closestX;
    float dy = spherePos.y - closestY;
    float dz = spherePos.z - closestZ;

    //距離＝ベクトルの長さ＝√(x^2 + y^2 + z^2)
    float distance = std::sqrt(dx * dx + dy * dy + dz * dz);
    if (distance < 0.00001f) return;  //0.00001f
    //法線ベクトル計算（距離を正規化）
    XMFLOAT3 normal = { dx / distance, dy / distance, dz / distance };

    //---位置修正(今の処理)　　　　衝突したときのめり込みを治す処理
    float pushDistance = radius - distance + 0.01f;  //元は0.01

    spherePos.x += normal.x * pushDistance;
    spherePos.y += normal.y * pushDistance;
    spherePos.z += normal.z * pushDistance;
    sphere->SetPosition(spherePos);
    //---ここから反射処理
    //velocityの長さチェック
    XMFLOAT3 velocity = sphere->GetVelocity();
    float len = velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z;
    if (len < 0.001f)
    {
        sphere->SetVelocity(XMFLOAT3());
        return;
    }
    //入射ベクトル・法線ベクトルの内積を計算
    //内積＝ベクトルA・ベクトルB＝|A||B|cosθ
    //normal葉単位ベクトルなので|B| = 1,入社ベクトルの長さも考慮する必要がないため単純に成分ごとに掛け算して足し合わせる
    float dot = velocity.x * normal.x + velocity.y * normal.y + velocity.z * normal.z;
    //反射ベクトル = 入射ベクトル - 2 * (入射ベクトル.法線) * 法線
    velocity.x = velocity.x - 2.0f * dot * normal.x;
    velocity.y = velocity.y - 2.0f * dot * normal.y;
    velocity.z = velocity.z - 2.0f * dot * normal.z;

    //減衰（任意）
    float restitution = 0.2f;  //1.0で完全反射 バンカーはかなり小さく
    velocity.x *= restitution;
    velocity.y *= restitution;
    velocity.z *= restitution;

    sphere->SetVelocity(velocity);
}
//玉同士の衝突判定(ゴルフボール,球体の障害物)を想定
void Collision::RefrectionSphere(MeshSphere* sphere, MeshSphere* sphere2)
{

    // =========================
        // 位置・半径取得
        // =========================
    XMFLOAT3 posA = sphere->GetPosition();
    XMFLOAT3 posB = sphere2->GetPosition();

    XMFLOAT3 scaleA = sphere->GetScale();
    XMFLOAT3 scaleB = sphere2->GetScale();

    float radiusA = std::max({ scaleA.x, scaleA.y, scaleA.z });
    float radiusB = std::max({ scaleB.x, scaleB.y, scaleB.z });

    float sumRadius = radiusA + radiusB;

    // =========================
    // 距離計算
    // =========================
    float dx = posA.x - posB.x;
    float dy = posA.y - posB.y;
    float dz = posA.z - posB.z;

    float distance = std::sqrt(dx * dx + dy * dy + dz * dz);

    // 衝突してなければ終了
    if (distance >= sumRadius || distance < 0.00001f) return;

    // =========================
    // 法線（B → A）
    // =========================
    XMFLOAT3 normal = {
        dx / distance,
        dy / distance,
        dz / distance
    };

    // =========================
    // めり込み修正（両方動かす）
    // =========================
    float penetration = sumRadius - distance;

    float correction = penetration * 0.5f + 0.001f;

    posA.x += normal.x * correction;
    posA.y += normal.y * correction;
    posA.z += normal.z * correction;

    posB.x -= normal.x * correction;
    posB.y -= normal.y * correction;
    posB.z -= normal.z * correction;

    sphere->SetPosition(posA);
    sphere2->SetPosition(posB);

    // =========================
    // 速度取得
    // =========================
    XMFLOAT3 vA = sphere->GetVelocity();
    XMFLOAT3 vB = sphere2->GetVelocity();

    // 相対速度
    XMFLOAT3 relativeVel = {
        vA.x - vB.x,
        vA.y - vB.y,
        vA.z - vB.z
    };

    // 法線方向速度
    float dot = relativeVel.x * normal.x +
        relativeVel.y * normal.y +
        relativeVel.z * normal.z;

    // 離れているなら何もしない
    if (dot > 0.0f) return;

    // =========================
    // 反射（インパルス）
    // =========================
    float restitution = 0.75f; // 1.0で完全反射

    // 等質量想定
    float impulse = -(1.0f + restitution) * dot * 0.5f;

    XMFLOAT3 impulseVec = {
        normal.x * impulse,
        normal.y * impulse,
        normal.z * impulse
    };

    vA.x += impulseVec.x;
    vA.y += impulseVec.y;
    vA.z += impulseVec.z;

    vB.x -= impulseVec.x;
    vB.y -= impulseVec.y;
    vB.z -= impulseVec.z;

    // =========================
    // 速度設定
    // =========================
    sphere->SetVelocity(vA);
    //sphere2->SetVelocity(vB);山は動かさないのでコメントアウト

}

//線分とAABBの衝突判定
//
bool Collision::CheckSegmentAndAABB(XMFLOAT3 strat, XMFLOAT3 end, MeshBox* box, float& outT)
{
    return false;
}
