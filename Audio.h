#pragma once
//DirectXではサウンドを鳴らすとき必要なライブラリ
//正確には、XBox向けの低レベルオーディオAPI
//遅延が少なく処理負荷を抑えられる
// //3Dオーディオにも対応,クロスプラットホーム(Windows以外)にも利用可能
#include<xaudio2.h>  //DirectSoundというのもある
#include "gameObject.h"
//Audio制御クラス
class Audio
{
private :
	//XAudio2を使うためのインターフェース
	static IXAudio2*		_XAudio;
	//音を発生させる音源 スピーカー
	//UnityでいうAudioSouce
	static IXAudio2MasteringVoice* _masteringVoice;

	IXAudio2SourceVoice*	_sourceVoice{};
	//実際のサウンドデータ
	BYTE*	_soundData{};

    //再生時間
	int		_length{};
	int		_playLength{};
public:
	//おおもとの初期化処理(1回やればOK)
	static void InitMaster();
	//おおもとの終了処理(1回やればOK)
	static void UninitMaster();
	//終了処理(音の破棄)
	void Uninit();
	//音声ファイルの読み込み
	void Load(const char* FileName);
	//音楽ファイルの再生
	//Play(true); Play();
	//引数省略されていたらfalseで入る
	void Play(bool Loop = false);
};

