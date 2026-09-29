#pragma once
#include "GolfBall.h"   //一応エラーが出るので追加した

class GolfBall;  //あらかじめ宣言しておく

class SceneBase
{
public:
	// こちらはただの仮想関数。
	// 純粋仮想関数とは違って派生先で絶対実装する必要はない
	// ただし、関数の中身は実装すること。エラーになる。
	virtual void Init();
	virtual void Uninit();
	virtual void Update();
	virtual void Draw();
	//どのシーンでも呼べるようにしてカメラロックで例外参照が起きないようにする
	virtual GolfBall* GetGolfBall() { return nullptr; }
	
};

