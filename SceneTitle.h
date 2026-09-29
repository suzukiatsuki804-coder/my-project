#pragma once
#include "SceneBase.h"
#include "Sprite.h"
#include "TitlePushStart.h"
#include "TitleGolfClub.h"
class SceneTitle : public SceneBase
{
private:
	//タイトル画面
	Sprite* m_titleSprite;
	//点滅する[PushStart]
	TitlePushStart* m_pushStart;
	//タイトルの題字
	Sprite* m_golfDai;
	//ゴルフクラブ表示
	TitleGolfClub* m_golfclub;
public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

};

