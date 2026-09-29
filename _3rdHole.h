//3つめのステージ
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


class _3rdHole : public SceneBase
{
private :
	std::vector<MeshBox*> m_courseObjects;  //ここに移動
	Sequencer* m_sequencer;  //シーケンサー
	NumberDisplay* m_number;  //打数の表示
	Gauge* m_gauge;
	MeshBox* m_Goal;
	GolfBall* m_golfball;

	//現在のゲーム状態  
	eGameMode m_currentMode;
	void UpdateStart();
	void UpdateGame();
	void UpdateGoal();
	//下はステージ
	MeshBox* m_fairway;
	MeshBox* m_fairway2;
	MeshBox* m_fairway3;
	MeshBox* m_fairway4;
	MeshBox* m_fairway5;
	MeshBox* m_rough;
	MeshBox* m_rough2;
	MeshBox* m_bunker;
	MeshBox* m_water;  //ウォーターハザード
	MeshBox* m_ob;

	Audio* m_Water;  //水に落ちたときの音
	Audio* m_Gsound;  //ゴールした時になる音
public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;
	GolfBall* GetGolfBall() override
	{
		{ return m_golfball; }  //オーバーライドする
	}
};

