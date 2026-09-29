//#include "main.h"
#include "Audio.h"

//static変数(静的変数)は初期化しないといけない
IXAudio2* Audio::_XAudio = NULL;
IXAudio2MasteringVoice* Audio::_masteringVoice = NULL;


//1回だけやる初期化処理
void Audio::InitMaster()
{
	//COM初期化(1回しかしない)
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	//XAudio作成
	XAudio2Create(&_XAudio, 0);

	//マスタリングボイス作成
	_XAudio->CreateMasteringVoice(&_masteringVoice);

}

void Audio::UninitMaster()
{
	//お片付け
	_masteringVoice->DestroyVoice();
	_XAudio->Release();
	CoUninitialize();
}

void Audio::Load(const char* FileName)
{
	//サウンドデータ読み込み
	WAVEFORMATEX wfx = { 0 };
	{
		//
		HMMIO hmmio = NULL;
		MMIOINFO mmioinfo = { 0 };
		MMCKINFO riffchunkinfo = { 0 };
		MMCKINFO datachunkinfo = { 0 };
		MMCKINFO mmckinfo = { 0 };
		UINT32 buflen;
		LONG readlen;

		//ファイル読み込み
		hmmio = mmioOpen((LPSTR)FileName, &mmioinfo, MMIO_READ);
		assert(hmmio);
		//WAVEファイルかどうか判断している
		riffchunkinfo.fccType = mmioFOURCC('W','A', 'V', 'E');
		//設定データにアクセス
		mmioDescend(hmmio, &riffchunkinfo, NULL,
			MMIO_FINDRIFF);

		//fmtで音の情報
		//サンプリングレート(OHz)　チャンネル数（ステレオなど）.ビット深度(16bit)など
		mmckinfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
		mmioDescend(hmmio, &mmckinfo, &riffchunkinfo, MMIO_FINDCHUNK);

		//特定のフォーマットだったら仏の読み込み処理
		if (mmckinfo.cksize >= sizeof(WAVEFORMATEX))
		{
			mmioRead(hmmio, (HPSTR)&wfx, sizeof(wfx));
		}
		else
		{
			//特殊な読み込み 古いファイル
			PCMWAVEFORMAT pcmwf = { 0 };
			mmioRead(hmmio, (HPSTR)&pcmwf, sizeof(pcmwf));
			memset(&wfx, 0x00, sizeof(wfx));
			memcpy(&wfx, &pcmwf, sizeof(pcmwf));
			wfx.cbSize = 0;
		}

		//アクセスいったん終了
		mmioAscend(hmmio, &mmckinfo, 0);
		datachunkinfo.ckid = mmioFOURCC('d', 'a','t', 'a');

		//データにアクセス
		mmioDescend(hmmio, &datachunkinfo, &riffchunkinfo, MMIO_FINDCHUNK);

		//読み込みサイズを設定
		buflen = datachunkinfo.cksize;
		_soundData = new unsigned char[buflen];
		//音声ファイルを読み込み
		readlen = mmioRead(hmmio, (HPSTR)_soundData, buflen);
		//音声ファイルのデータサイズ
		_length = readlen;
		//音楽ファイルの再生サンプル数
		_playLength = readlen / wfx.nBlockAlign;

		//お片付け
		mmioClose(hmmio, 0);

	}

	//サウンドソース作成
	_XAudio->CreateSourceVoice(&_sourceVoice, &wfx);
	assert(_sourceVoice);
}

void Audio::Uninit()
{
	//破棄処理
	_sourceVoice->Stop();
	_sourceVoice->DestroyVoice();

	delete[] _soundData;
}


void Audio::Play(bool Loop)
{
	//プレイ中ならいったん止めてバッファクリア(下の行が特に)
	_sourceVoice->Stop();
	_sourceVoice->FlushSourceBuffers();
	//バッファを設定]
	XAUDIO2_BUFFER bufinfo;
	memset(&bufinfo, 0X00, sizeof(bufinfo));
	//音声データの大きさ(Byte)
	bufinfo.AudioBytes = _length;
	//音声の情報
	bufinfo.pAudioData = _soundData;
	//最初から再生(0以外にすると途中から再生される)
	bufinfo.PlayBegin = 0;
	//再生（終了）時間
	bufinfo.PlayLength = _playLength;

	//ループ設定
	if (Loop)
	{
		bufinfo.LoopBegin = 0;
		bufinfo.LoopLength = _playLength;
		bufinfo.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

	//設定反映
	_sourceVoice->SubmitSourceBuffer(&bufinfo, NULL);

	//再生
	_sourceVoice->Start();

}
