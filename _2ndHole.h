//2つめのステージ

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
#include "SceneInGame.h"



class _2ndHole : public SceneBase
{
private :
	std::vector<MeshBox*> m_courseObjects;
	Sequencer* m_sequencer;  //シーケンサー
	NumberDisplay* m_number;  //打数の表示
	Gauge* m_gauge;
	MeshBox* m_Goal;
	//上は必須要素
	MeshBox* m_fairway;
	MeshBox* m_rough;
	MeshBox* m_fairway2;
	MeshBox* m_rough2;
	MeshBox* m_fairway3;
	MeshBox* m_ob;

	//BGM
	Audio* m_bgm;
	Audio* m_goal;  //ゴール時のse
	


//------------------------------------------------------------------------------------
	GolfBall* m_golfball;

	//現在のゲーム状態  シーケンサーは今までと処理が違う子でここが必要
	eGameMode m_currentMode;
	void UpdateStart();
	void UpdateGame();
	void UpdateGoal();

public:  //ゲームに必要な4処理 ここもSceneInGameより流用
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;
	GolfBall* GetGolfBall() override
	{
		{ return m_golfball; }  //オーバーライドする
	}
};

