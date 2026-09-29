#pragma once
#include "SceneBase.h"
//シーンの取得用のテンプレート関数
//Rigidbody rb = GetComponent<Rigidbody>();
//GetScene<シーンのクラス名>()でシーンのポインタを取得できる
template<typename T>
inline T* GetScene()
{
	//main.cppで定義されているシーンを取得して返す
	extern SceneBase* g_scene;
	return static_cast<T*>(g_scene);
}
//シーン切り替え用テンプレート関数
//ChangeScene<SceneTitle>()という感じで切り替え可能
//ChangeScene<SceneTitle>();ChangeScene<SceneInGame>
template<typename T>
inline T* ChangeScene()
{
	//main.cppで定義されているシーンを取得して返す
	extern SceneBase* g_scene;
	delete g_scene;  //古いシーンを削除
	g_scene = new T();  //新しいシーンを生成
	g_scene->Init();  //新しいシーンの初期化
	return static_cast<T*>(g_scene);
}