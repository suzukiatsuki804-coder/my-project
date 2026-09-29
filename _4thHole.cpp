#include "_4thHole.h"
#include "direct3d.h"
#include "Collision.h" 
#include "main.h"
#include "SceneTitle.h"  
#include "Camera.h"
#include <vector>

void _4thHole::UpdateStart()
{
	m_number->Update();
	m_gauge->SetGaugeValue(m_golfball->GetPower());
	m_gauge->Update();
	m_sequencer->Update(0.01f);  //seqencerになってる
}

void _4thHole::UpdateGame()
{
	m_golfball->Update();
	m_number->Update();
	m_gauge->SetGaugeValue(m_golfball->GetPower());
	m_gauge->Update();
	//---------ここでコースを読み取る
	for (auto obj : m_courseObjects) {
		obj->Update();
	}

	//衝突判定と反射処理
	for (auto obj : m_courseObjects)
	{
		if (Collision::CheckBallAndBox(m_golfball, obj)) {
			switch (obj->type) {
				//フェアウェイ
			case GroundType::Fairway:
				Collision::Refrection(m_golfball, obj);
				break;
				//ラフ
			case GroundType::Rough:
				Collision::Refrection2(m_golfball, obj);
				break;
				//バンカー
			case GroundType::Bunker:
				Collision::Refrection3(m_golfball, obj);
				m_Bsound->Play();
				break;
				//OB
			case GroundType::OB:
				//位置をリセット
				m_golfball->SetPosition(XMFLOAT3(0.0f, 1.0f, 0.0f));
				//速度を0にする
				m_golfball->SetVelocity({ 0,0,0 });

				// OBをショット扱いにして1打増やす
				m_golfball->OnShot();
				break;
			case GroundType::Water:
				//位置をリセット
				m_golfball->SetPosition(XMFLOAT3(0.0f, 1.0f, 0.0f));
				//速度を0にする
				m_golfball->SetVelocity({ 0,0,0 });
				// 1打増やす
				m_golfball->OnShot();
				break;
			case GroundType::Goal:  //ゴールの衝突判定
				m_currentMode = eGameMode::GOAL;
				//シーケンサ情報リセット
				m_sequencer->Reset();
				CommandParam param;
				m_Gsound->Play();

				//1秒後タイトルに遷移する
				param.startTime = 2.0f;
				param.type = FUNCTION;
				param.event = [this]()
					{
						Camera::UnlockCamera();
						//ゴールした時の処理
						ChangeScene<SceneTitle>();

					};
				param.duration = 0.1f;
				m_sequencer->AddCommand(param);
				break;

			}
		}
	}
}

void _4thHole::UpdateGoal()
{
	//ゴールした時の処理
	m_golfball->Update();
	m_sequencer->Update(0.01f);
}

void _4thHole::Init()
{
	//BunkerのSE
	m_Bsound = new Audio();
	m_Bsound->Load("Sound\\bunkerse.wav");

	//ゴールl時のBGM   
	m_Gsound = new Audio();
	m_Gsound->Load("Sound\\goal.wav");

	//数字
	m_number = new NumberDisplay();
	m_number->Init(
		Direct3D_GetDevice(),
		Direct3D_GetDeviceContext());
	m_number->LoadTexture("Texture\\number.png");
	m_number->SetSize(150.0f, 50.0f);  //元（200.0, 50.0）
	m_number->SetPosition(XMFLOAT3(100.0f, 50.0f, 0.0f));
	m_number->Setnumber(0);

	//パワーゲージ
	m_gauge = new Gauge();
	m_gauge->Init(
		Direct3D_GetDevice(),
		Direct3D_GetDeviceContext());
	m_gauge->SetGaugePosition(XMFLOAT3(800.0f, 750.0f, 0.0f));
	m_gauge->SetGaugeSize(500.0f, 100.0f);
	m_gauge->SetGaugeValue(0.0f);
	//ボール
	m_golfball = new GolfBall();
	m_golfball->Init(
		Direct3D_GetDevice(),
		Direct3D_GetDeviceContext());
	m_golfball->SetPosition(XMFLOAT3(0.0f, 1.0f, 0.0f));
	m_golfball->SetScale(XMFLOAT3(0.2f, 0.2f, 0.2f));

	//イベントの手動登録　ラムダ式を登録 *privateのm_numberを取得
//ラムダ式は[キャプチャ](引数){処理}という形で書く
//今回は引数なし、thisをキャプチャしている *この書き方覚える！
	m_golfball->OnShot = [this]()
		{
			//ボールが打たれた時の処理
			int currentNumber = m_number->Getnumber();
			//現在のショット数を取得して１増やし、表示を更新する
			m_number->Setnumber(currentNumber + 1);
		};

	//---------------------------------------------------------
	auto addBox = [&](XMFLOAT3 pos, XMFLOAT3 scale, XMFLOAT4 color, GroundType type) {
		MeshBox* box = new MeshBox();
		box->Init(Direct3D_GetDevice(), Direct3D_GetDeviceContext());
		box->SetPosition(pos);
		box->SetScale(scale);
		box->SetColor(color);
		box->type = type;
		m_courseObjects.push_back(box);
		return box;
		};
	//                   位置　　　　　大きさ　　　　色
	m_fairway = addBox({ 0,0,0 }, { 10,0.2f,15 }, { 0.2f,0.6f,0.2f,1 }, GroundType::Fairway);
	m_rough = addBox({ 0,0,-20 }, { 10,0.2,10 }, { 0.2f,0.4f,0.2f,1 }, GroundType::Rough);
	m_fairway2 = addBox({ -20,0,40 }, { 15,0.2f,15 }, { 0.2f,0.6f,0.2,1 }, GroundType::Fairway);
	m_bunker = addBox({ -37,0,40 }, { 5,0.2f,12 }, { 0.7f,0.7f,0.3f,1 }, GroundType::Bunker);
	m_fairway3 = addBox({ 0,0,115 }, { 9,0.2f,30 }, { 0.2f,0.6f,0.2f,1 }, GroundType::Fairway);
	m_rough2 = addBox({ 0,0,145 }, { 10,4.0f,10 }, { 0.2f,0.4f,0.2f,1 }, GroundType::Rough);
	m_fairway4 = addBox({ -15,0,135 }, { 8,0.2f,10 }, { 0.2f,0.6f,0.2f,1 }, GroundType::Fairway);
	m_Goal = addBox({ -18,0,135 }, { 0.4f,4.0f,0.4f }, { 0.8f,0.2f,0.2f,0.65f }, GroundType::Goal);

	m_ob = addBox({ 0,-50,0 }, { 300, 0.4f,300 }, { 0.6f,0.2f,0.8f,1 }, GroundType::OB);





	//----------------------------シーケンサー
	m_currentMode = eGameMode::START;
	m_sequencer = new Sequencer();
	m_sequencer->Init();
	//コマンド、指定秒数になったら登録したtypeに合わせて実行
	CommandParam param;
	//コマンド開始時間
	param.startTime = 0.0f;
	//コマンドの種別
	param.type = CAM_MOVE;
	//移動量
	param.value = { 0.0f, 0.01f, -0.01f };
	//コマンド実行時間　今回は2秒実行する
	param.duration = 2.0f;
	//シーケンサに登録
	m_sequencer->AddCommand(param);
	//構造体の中身をリセット
	ZeroMemory(&param, sizeof(param));
	param.startTime = 2.0f;
	param.type = FUNCTION;
	param.event = [this]()
		{
			m_currentMode = eGameMode::WAIT_SHOT;
		};
	//0だとバグ
	param.duration = 0.01f;
	m_sequencer->AddCommand(param);
}

void _4thHole::Uninit()
{
	m_number->Uninit();
	delete m_number;
	m_number = nullptr;
	m_gauge->Uninit();
	delete m_gauge;
	m_gauge = nullptr;
	m_golfball->Uninit();
	delete m_golfball;
	m_golfball = nullptr;
	//シーケンサ
	m_sequencer->UnInit();
	delete m_sequencer;

	for (auto obj : m_courseObjects)
	{
		obj->Uninit();
		delete obj;
		obj = nullptr;
	}
}

void _4thHole::Update()
{
	//シーケンサ これは残す
	switch (m_currentMode)
	{
	case eGameMode::START:
		UpdateStart();
		break;
	case eGameMode::WAIT_SHOT:
		UpdateGame();
		break;
	case eGameMode::GOAL:
		UpdateGoal();
		break;
	default:
		break;
	}
}

void _4thHole::Draw()
{
	XMMATRIX view, proj;
	Camera::GetCameraMatrix(view, proj);

	m_number->Draw();
	m_gauge->Draw();
	m_golfball->Draw();
	//ここで地形をドロー
	for (auto obj : m_courseObjects)
	{
		obj->Draw();
	}
}
