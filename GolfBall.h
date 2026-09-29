#pragma once
#include "MeshSphere.h"
#include <functional>  //6/3追加
#include "audio.h"
class GolfBall :  public MeshSphere
{
private :
	//XMFLOAT3 m_velocity;  //速さを保持する変数
	float m_timer = 0.0f; //移動時間
	XMFLOAT3 m_accel;  //加速度を保持する変数
	const XMFLOAT3 GRAVITY = { 0.0f, -9.8f, 0.0f };  //重力加速度 今回はconstで不変に
	float m_power;  //ボールを飛ばすパワー
	const float MAXPOWER = 2.0f;   //パワーの限界設定 2.0f  ここの値で飛距離が変わる
	//追加
	Audio* m_shotse;  //ボールを飛ばしたときの音
	Audio* m_goroshotse;  //Bボタンでボールを飛ばした時の音
	
public :
	bool IsStopped(); //ボールが止まった判定用の関数
	//ゲッター
	XMFLOAT3 GetVelocity() { return m_velocity; }
	//セッター
	void SetVelocity(XMFLOAT3 vel) { m_velocity = vel; }
	//更新処理をoverride
	void Update() override;

	//ボールが止まった後のイベント関数
	//std::functionは、関数やラムダ式など
	//呼び出し可能なオブジェクトを保持できる便利な形
	//std::function<void()>は、void Hoge()みたいなもので登録できる。
	std::function<void()> OnShot;

	//パワー取得、計算式は適当なので適当な数値に調整する
	float GetPower() const { return m_power * 10.0f / MAXPOWER; }

	//ショットの判定
	int Shottype;

	// / MAXPOWER
};

