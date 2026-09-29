#include "NumberDisplay.h"

void NumberDisplay::Setnumber(int number)
{
	m_number = number;
}

void NumberDisplay::Update()
{
	int x = m_number % static_cast<int>
		(m_digit_column);  //列のインデックス
	int y = m_number / static_cast<int>
		(m_digit_column);  //行のインデックス
	//数字一つの幅。高さ
	float digitWidth = 1.0f / m_digit_column; //一桁の幅
	float digitHeight = 1.0f / m_digit_row;  //一桁の高さ

	//UVの最小値を設定
	SetMinTex(XMFLOAT2(x * digitWidth, y * digitHeight));
	//UVの最大値を設定
	SetMaxTex(XMFLOAT2(digitWidth + x * digitWidth, 
		               digitHeight + y * digitHeight));
}

void NumberDisplay::Draw()
{
	//Spriteで登録したDraw処理を呼び出せる
	Sprite::Draw();
}
