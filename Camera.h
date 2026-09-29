#pragma once
#include <DirectXMath.h>
using namespace DirectX;
#define CAMERA_SPEED 0.1f	// 移動スピード
#define CAMERA_ROTATE 1.0f	// 回転スピード
#define CAMERA_FOV 60.0f		// 画角
#define CAMERA_HEIGHT_MOVESPEED	0.1f	//高さ調整スピード
namespace Camera
{
	// 0518 追加
	void Initialize();
	// カメラの通常操作
	void UpdateNormalMode();
	//ロックオン
	void UpdateLockMode();
	//カメラの角度を取得する 5/20add
	float GetCameraAngle();
	// 更新処理
	void Update();
	// 3Dカメラ行列の設定（ビュー行列とプロジェクション行列の計算）
	void Set3DCamera();
	// カメラ関係の行列の取得
	void GetCameraMatrix(XMMATRIX& view, XMMATRIX& projection);
	XMFLOAT3 GetCameraPosition();

	//平行移動位置と注視点を同じ量だけ移動する
	void Translate(const XMFLOAT3& delta);
	//注視点を変えずにカメラをかいてんさせる(度単位)
	void RotateAroundTarget(float deltaAngleDeg);
	//ステージ遷移時にカメラのロック解除
	void UnlockCamera();
}