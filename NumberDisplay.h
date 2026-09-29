#pragma once
#include "Sprite.h"
//数字表示クラス(ゲームシーンに何打目かを出す)
//Spriteクラスを継承する
class NumberDisplay : public Sprite
{
private:
	//画像を横5分割　縦5分割する　割り算するためfloat
	//切り抜き用の計算に用いる
	const float m_digit_column = 5.0f;
	const float m_digit_row = 5.0f;  
	//表示する数字
	int m_number = 0;
public:
	//数字のゲッターとセッター
	int Getnumber() { return m_number; }
	void Setnumber(int number);
	//更新処理をオーバーライド
	void Update() override;
	void Draw() override;
};

