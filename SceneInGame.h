#pragma once
#include "SceneBase.h"
#include"MeshBox.h"
#include "MeshSphere.h"
#include "MovingBox.h"
#include "MovingBox2.h"
#include "GolfBall.h"  //ボール用追加
#include "NumberDisplay.h"  //数字追加
#include "Gauge.h"  //ゲージ追加
#include "audio.h"  //音響追加
#include "ParticleSystem.h"
#include "Sequencer.h"

//モードを分けておく
enum class eGameMode
{
	NONE = 0,
	START,  //スタート時
	WAIT_SHOT,  //ショット待機時
	GOAL,  //ゴール時

};

class SceneInGame : public SceneBase
{
private:

	ParticleSystem* m_particle;  //パーティクル
	ParticleSystem* m_particle2;  //ゴール時のパーティクル
	
	Sequencer* m_sequencer;  //シーケンサー

	MeshBox* m_fairway;  //フェアウェイ、　ボールがよく転がる
	MeshBox* m_fairway2;
	MeshBox* m_fairway3;
	MeshBox* m_fairway4;  //中央ラフ横のフェアウェイ

	//OB　とりあえず降れたらタイトルに戻る処理
	MeshBox* m_ob;

	NumberDisplay* m_number;  //ショット数表示

	Gauge* m_gauge;

	//ゴール
	MeshBox* m_Goal;
	MeshSphere* m_Yamaobj;  //ゴール付近の山
	MeshSphere* m_Yamaobj2; //二つで連峰


	//BGM
	Audio* m_bgm;
	Audio* m_goal;  //ゴール時のse
	Audio* m_bunkersound;  //バンカーに入った時のサウンド

	//動く
	MovingBox* m_moveStage;
	MovingBox* m_moverough;  //動く＋ラフ
	MovingBox2* m_moveStage2;



	MeshBox* m_rough;  //ラフ ボールが転がりにくい
	MeshBox* m_rough2;
	MeshBox* m_rough3;

	MeshBox* m_bunker;  //バンカー　とてもボールが転がらない



	MeshBox* m_green;  //グリーン とても転がる、パター限定

//------------------------------------------------------------------------------------
	//ゴルフボール
	GolfBall* m_golfball;

	//現在のゲーム状態  シーケンサーは今までと処理が違う子でここが必要
	eGameMode m_currentMode;
	void UpdateStart();
	void UpdateGame();
	void UpdateGoal();

public:  //ゲームに必要な4処理
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;
	GolfBall* GetGolfBall() override
	{
		{ return m_golfball; }  //オーバーライドする
	}
};

