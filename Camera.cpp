#include "Camera.h"
#include "direct3d.h"
#include "Input.h"
#include "Cube3D.h"
#include "GolfBall.h"  //追加
#include "SceneInGame.h"
#include "main.h"
using namespace DirectX;

namespace Camera
{
	XMFLOAT3 g_position = { 0, 3, -6 }; // カメラの位置
	XMFLOAT3 g_target = { 0, 0, 0 }; // カメラの注視点
	XMFLOAT3 g_up = { 0, 1, 0 }; // カメラの上方向
	XMMATRIX g_view; // ビュー行列
	XMMATRIX g_projection; // プロジェクション行列
	float g_cameraSpeed = CAMERA_SPEED; // カメラの移動速度
	float g_near = 0.1f; // ニアクリップ距離
	float g_far = 100.0f; // ファークリップ距離
	float g_fov = XM_PIDIV4; // 視野角

	// カメラ操作その2
	float g_lockCameraDistance = 5.0f; // ロックモードのカメ
	float g_lockCameraHeight = 2.0f; // ロックモードのカメラの高さ
	float g_lockCameraAngle =-90.0f; // ロックモードのカメラの角度
	bool g_isLockMode = false; // ロックモードかどうか

	GolfBall* g_targetBall = nullptr;  //ロックモードの注視点となるゴルフボールのポインタ

	//5/20追加
	float GetCameraAngle()
	{
		return g_lockCameraAngle;
	}
	// 0518 追加
	void Initialize()
	{
		g_position.x = g_target.x + cosf(XMConvertToRadians(g_lockCameraAngle)) * g_lockCameraDistance;
		g_position.y = g_target.y + g_lockCameraHeight;
		g_position.z = g_target.z + sinf(XMConvertToRadians(g_lockCameraAngle)) * g_lockCameraDistance;
		g_fov = CAMERA_FOV;
	}
	// カメラの通常操作
	void UpdateNormalMode()
	{
		
		//注視点の高さを変える
		if (GetKeyPress(VK_W) && GetKeyPress(VK_SHIFT))
		{
			g_target.y += 0.1f;
		}
		if (GetKeyPress(VK_S) && GetKeyPress(VK_SHIFT))
		{
			g_target.y -= 0.1f;
		}
		//注視点を基準にカメラの高さを設定
		g_position.y = g_target.y + g_lockCameraHeight;
		if (GetKeyPress(VK_A))
		{
			// -360～360度で設定。XMConvertToRadiansでラジアンに変換
			float angle = XMConvertToRadians(g_lockCameraAngle) + XMConvertToRadians(-90.0f);
			// 並行移動
			g_position.x += g_cameraSpeed * cos(angle);
			g_position.z += g_cameraSpeed * sin(angle);
			g_target.x += g_cameraSpeed * cos(angle);
			g_target.z += g_cameraSpeed * sin(angle);
		}
		if (GetKeyPress(VK_D))
		{
			// -360～360度で設定。XMConvertToRadiansでラジアンに変換
			float angle = XMConvertToRadians(g_lockCameraAngle) + XMConvertToRadians(90.0f);
			g_position.x += g_cameraSpeed * cos(angle);
			g_position.z += g_cameraSpeed * sin(angle);
			g_target.x += g_cameraSpeed * cos(angle);
			g_target.z += g_cameraSpeed * sin(angle);
		}
		if (GetKeyPress(VK_W))
		{
			// -360～360度で設定。XMConvertToRadiansでラジアンに変換
			float angle = XMConvertToRadians(g_lockCameraAngle) + XMConvertToRadians(180.0f);
			g_position.x += g_cameraSpeed * cos(angle);
			g_position.z += g_cameraSpeed * sin(angle);
			g_target.x += g_cameraSpeed * cos(angle);
			g_target.z += g_cameraSpeed * sin(angle);
		}
		if (GetKeyPress(VK_S))
		{
			// -360～360度で設定。XMConvertToRadiansでラジアンに変換
			float angle = XMConvertToRadians(g_lockCameraAngle);
			g_position.x += g_cameraSpeed * cos(angle);
			g_position.z += g_cameraSpeed * sin(angle);
			g_target.x += g_cameraSpeed * cos(angle);
			g_target.z += g_cameraSpeed * sin(angle);
		}
		if (GetKeyPress(VK_E))
		{
			g_lockCameraAngle -= CAMERA_ROTATE;
			g_position.x = g_target.x + cosf(XMConvertToRadians(g_lockCameraAngle)) * g_lockCameraDistance;
			g_position.z = g_target.z + sinf(XMConvertToRadians(g_lockCameraAngle)) * g_lockCameraDistance;

		}
		if (GetKeyPress(VK_Q))
		{
			g_lockCameraAngle += CAMERA_ROTATE;
			g_position.x = g_target.x + cosf(XMConvertToRadians(g_lockCameraAngle)) * g_lockCameraDistance;
			g_position.z = g_target.z + sinf(XMConvertToRadians(g_lockCameraAngle)) * g_lockCameraDistance;
		}
		
	}

	// 更新処理
	void Update()
	{
		
		if(GetKeyTrigger(VK_L))
		{
			g_isLockMode = !g_isLockMode;
			SceneBase* scene = GetScene<SceneBase>();
			g_targetBall = scene->GetGolfBall();
		}
		if (g_isLockMode)
		{
			UpdateLockMode();
		}
		else
		{
			UpdateNormalMode();
		}
	}
	// 3Dカメラ行列の設定（ビュー行列とプロジェクション行列の計算）
	void Set3DCamera()
	{
		// アスペクト比を出すための解像度を取得
		const float SCREEN_WIDTH = (float)Direct3D_GetBackBufferWidth();
		const float SCREEN_HEIGHT = (float)Direct3D_GetBackBufferHeight();

		// ビュー行列とプロジェクション行列を作る
		// アスペクト比を計算
		float aspect = SCREEN_WIDTH / SCREEN_HEIGHT;
		// ビュー行列を作る（位置、注視点、上方向）
		g_view = XMMatrixLookAtLH(XMLoadFloat3(&g_position), XMLoadFloat3(&g_target), XMLoadFloat3(&g_up));
		// プロジェクション行列を作る（視野角、アスペクト比、ニア、ファー）
		g_projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(g_fov), aspect, g_near, g_far);

	}	
	// カメラ関係の行列の取得
	void GetCameraMatrix(XMMATRIX& view, XMMATRIX& projection)
	{
		view = g_view;
		projection = g_projection;
	}
	XMFLOAT3 GetCameraPosition()
	{
		return g_position;
	}

	//平行移動、位置と注視点を同じだけ移動する 6/15追加
	void Translate(const XMFLOAT3& delta)
	{
		g_position.x += delta.x;
		g_position.y += delta.y;
		g_position.z += delta.z;

		g_target.x += delta.x;
		g_target.y += delta.y;
		g_target.z += delta.z;
	}

	//注視点を変えながらカメラを回転させる(度単位)
	void RotateAroundTarget(float deltaAngleDeg)
	{
		g_lockCameraAngle += deltaAngleDeg;
		//距離を保って位置を再計算
		g_position.x = g_target.x + cosf(XMConvertToRadians(g_lockCameraAngle)) * g_lockCameraDistance;
		g_position.z = g_target.z + sinf(XMConvertToRadians(g_lockCameraAngle)) * g_lockCameraDistance;
		g_position.y = g_target.y + g_lockCameraHeight;
	}

	void UpdateLockMode()
	{
		if (g_targetBall != nullptr)
		{
			g_target = g_targetBall->GetPosition();
		}
		if (GetKeyPress(VK_A))
		{
			g_lockCameraAngle -= CAMERA_ROTATE;
		}
		if (GetKeyPress(VK_D))
		{
			g_lockCameraAngle += CAMERA_ROTATE;
		}
		if (GetKeyPress(VK_E))
		{
			g_lockCameraHeight += CAMERA_HEIGHT_MOVESPEED;
			g_position.x = g_target.x + cosf(XMConvertToRadians(g_lockCameraAngle)) * g_lockCameraDistance;
			g_position.z = g_target.z + sinf(XMConvertToRadians(g_lockCameraAngle)) * g_lockCameraDistance;



		}
		if (GetKeyPress(VK_Q))
		{
			g_lockCameraHeight -= CAMERA_HEIGHT_MOVESPEED;
			g_position.x = g_target.x + cosf(XMConvertToRadians(g_lockCameraAngle)) * g_lockCameraDistance;
			g_position.z = g_target.z + sinf(XMConvertToRadians(g_lockCameraAngle)) * g_lockCameraDistance;
		}
		// 注視点から一定距離離れた位置にカメラを配置する
		g_position.x = g_target.x + cosf(XMConvertToRadians(g_lockCameraAngle)) * g_lockCameraDistance;
		g_position.y = g_target.y + g_lockCameraHeight;
		g_position.z = g_target.z + sinf(XMConvertToRadians(g_lockCameraAngle)) * g_lockCameraDistance;
	}

	void UnlockCamera()
	{
		g_isLockMode = false;
		g_targetBall = nullptr;
	}
}
