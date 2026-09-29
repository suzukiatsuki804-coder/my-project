#include "GolfBall.h"
#include "Input.h"
#include "Camera.h"

bool GolfBall::IsStopped()
{
	return m_velocity.x == 0.0f 
		&& m_velocity.y == 0.0f 
		&& m_velocity.z == 0.0f;
}


void GolfBall::Update()
{   
	//1F前の位置
	XMFLOAT3 old_pos = m_position;
	//地面にいる && スペースキーを押したらパワーをためる
	if(GetKeyPress(VK_SPACE) )  //&&IsStopped だったがいったん消去　
	{
		Shottype = 1;  //ここでショットの判定
		//限界までは加算可能
		if (m_power < MAXPOWER)
		{
			m_power += 0.001f;  //0.01f
		}
		
	}
	else if (GetKeyPress(VK_B))  
	{
		Shottype = 2;  //ここでショットの判定
		//限界までは加算可能
		if (m_power < MAXPOWER * 2)
		{
			m_power += 0.001f;  
		}

	}
	else
	{
		//離したときためた力に応じて速度設定
		if (m_power > 0.0f && Shottype == 1)
		{
			
			//ボールが打たれた時のイベントを呼ぶ
			if (OnShot != nullptr)
			{
				OnShot();
			}
			float angle = Camera::GetCameraAngle();
			m_velocity.x = -(m_power)* cos(XMConvertToRadians(angle));
			m_velocity.z = -(m_power)* sin(XMConvertToRadians(angle));
			m_velocity.y = m_power;

			//SE
			m_shotse = new Audio();
			m_shotse->Load("Sound\\Gshot.wav");
			m_shotse->Play();
			
			//ためたパワーはリセット
			m_power = 0.0f;
			//ショットタイプもリセット
			Shottype = 0;
		}
		else if (m_power > 0.0f && Shottype == 2)  //ゴロショット
		{
			//ボールが打たれた時のイベントを呼ぶ
			if (OnShot != nullptr)
			{
				OnShot();
			}
			float angle = Camera::GetCameraAngle();
			m_velocity.x = -(m_power)*cos(XMConvertToRadians(angle));
			m_velocity.z = -(m_power)*sin(XMConvertToRadians(angle));
			m_velocity.y = 0.03f;  //値は適当

			//SE
			m_goroshotse = new Audio();
			m_goroshotse->Load("Sound\\Goroshotse.wav");
			m_goroshotse->Play();

			//ためたパワーはリセット
			m_power = 0.0f;
			//ショットタイプもリセット
			Shottype = 0;
		}
		
	}

	//速度に重力加速度を足していく
	m_velocity.y += GRAVITY.y / 10000.0f;  //10000.0f
	
	//毎フレームXYZを計算
	m_position.x += m_velocity.x;
	m_position.y += m_velocity.y ;
	m_position.z += m_velocity.z;

	
	
}
