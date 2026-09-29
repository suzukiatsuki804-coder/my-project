#include "TitlePushStart.h"
#include "Input.h"
#include "main.h"
#include "SceneInGame.h"
#include "Audio.h"

void TitlePushStart::Update()
{
	//点滅処理の実装
	m_timer += 0.05f;  //適当にタイマーを進める
	//sin波を0から1の範囲に変換して適当にα値に設定
	m_alpha = (sinf(m_timer) + 1.0f) / 2.0f;
	if (GetKeyTrigger(VK_SPACE))
	{
		//スペースキーが押されたら、シーンをゲームシーンに切り替える
		ChangeScene<SceneInGame>();
		//クリック音
		m_clickse = new Audio();
		m_clickse->Load("Sound\\Click.se.wav");
		m_clickse->Play();
	}
}
