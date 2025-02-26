//=============================================================================
//
// �T�E���h���� [xa2.cpp]
//
//=============================================================================

#include "xa2.h"

// �p�����[�^�\����
typedef struct
{
	LPCSTR filename;	// �����t�@�C���܂ł̃p�X��ݒ�
	bool bLoop;			// true�Ń��[�v�B�ʏ�BGM��ture�ASE��false�B
} PARAM;

PARAM g_param[SOUND_LABEL_MAX] =
{
	{"Assets/SE/balloon.wav", false},
	{"Assets/SE/decision.wav", false},
	{"Assets/SE/cancel.wav", false},
	{"Assets/SE/select.wav", false},
	{"Assets/SE/count.wav", false},
	{"Assets/SE/dash.wav", false},
	{"Assets/SE/jump.wav", false},
	{"Assets/SE/score.wav", false},
	{"Assets/SE/start.wav", false},
	{"Assets/SE/timeUp.wav", false},
	{"Assets/SE/charge.wav", true},
	{"Assets/SE/chargeOK.wav", false},
	{"Assets/SE/select.wav", false},
	{"Assets/BGM/Title.wav", true},
	{"Assets/BGM/SelectStage.wav", true},
	{"Assets/BGM/Stage.wav", true},
	{"Assets/BGM/Result.wav", true},
	{"Assets/SE/wall.wav", false},
	{"Assets/SE/noBreakItem.wav", false},
	{"Assets/SE/move.wav", false},
};

#ifdef _XBOX //Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif
#ifndef _XBOX //Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif


//-----------------------------------------------------------------
//    �O���[�o���ϐ�
//-----------------------------------------------------------------
IXAudio2               *g_pXAudio2        = NULL;
IXAudio2MasteringVoice *g_pMasteringVoice = NULL;
IXAudio2SourceVoice    *g_pSourceVoice[SOUND_LABEL_MAX];

WAVEFORMATEXTENSIBLE	g_wfx[SOUND_LABEL_MAX];			// WAV�t�H�[�}�b�g
XAUDIO2_BUFFER			g_buffer[SOUND_LABEL_MAX];
BYTE					*g_DataBuffer[SOUND_LABEL_MAX];

//-----------------------------------------------------------------
//    �v���g�^�C�v�錾
//-----------------------------------------------------------------
HRESULT FindChunk(HANDLE, DWORD, DWORD&, DWORD&);
HRESULT ReadChunkData( HANDLE , void* , DWORD , DWORD );

//=============================================================================
// ������
//=============================================================================
HRESULT XA_Initialize()
{
	HRESULT      hr;
	
	HANDLE               hFile;
	DWORD                dwChunkSize;
	DWORD                dwChunkPosition;
	DWORD                filetype;
		
	// COM�̏�����
	CoInitializeEx( NULL , COINIT_MULTITHREADED );
	
	/**** Create XAudio2 ****/
	hr = XAudio2Create( &g_pXAudio2, 0);
	
	if( FAILED( hr ) ){
		CoUninitialize();
		return -1;
	}
	
	/**** Create Mastering Voice ****/
	hr = g_pXAudio2->CreateMasteringVoice(&g_pMasteringVoice);
	
	if( FAILED( hr ) ){
		if( g_pXAudio2 )	g_pXAudio2->Release();
		CoUninitialize();
		return -1;
	}

	for(int i=0; i<SOUND_LABEL_MAX; i++)
	{
		memset( &g_wfx[i] , 0 , sizeof( WAVEFORMATEXTENSIBLE ) );
		memset( &g_buffer[i] , 0 , sizeof( XAUDIO2_BUFFER ) );
	
		hFile = CreateFileA( g_param[i].filename , GENERIC_READ , FILE_SHARE_READ , NULL ,
							OPEN_EXISTING , 0 , NULL );
		if ( hFile == INVALID_HANDLE_VALUE ){
			return HRESULT_FROM_WIN32( GetLastError() );
		}
		if ( SetFilePointer( hFile , 0 , NULL , FILE_BEGIN ) == INVALID_SET_FILE_POINTER ){
			return HRESULT_FROM_WIN32( GetLastError() );
		}

		//check the file type, should be fourccWAVE or 'XWMA'
		FindChunk(hFile,fourccRIFF,dwChunkSize, dwChunkPosition );
		ReadChunkData(hFile,&filetype,sizeof(DWORD),dwChunkPosition);
		if (filetype != fourccWAVE)		return S_FALSE;

		FindChunk(hFile,fourccFMT, dwChunkSize, dwChunkPosition );
		ReadChunkData(hFile, &g_wfx[i], dwChunkSize, dwChunkPosition );

		//fill out the audio data buffer with the contents of the fourccDATA chunk
		FindChunk(hFile,fourccDATA,dwChunkSize, dwChunkPosition );
		g_DataBuffer[i] = new BYTE[dwChunkSize];
		ReadChunkData(hFile, g_DataBuffer[i], dwChunkSize, dwChunkPosition);
		
		CloseHandle(hFile);

		// 	�T�u�~�b�g�{�C�X�ŗ��p����T�u�~�b�g�o�b�t�@�̐ݒ�
		g_buffer[i].AudioBytes = dwChunkSize;
		g_buffer[i].pAudioData = g_DataBuffer[i];
		g_buffer[i].Flags = XAUDIO2_END_OF_STREAM;
		if(g_param[i].bLoop)
			g_buffer[i].LoopCount = XAUDIO2_LOOP_INFINITE;
		else
			g_buffer[i].LoopCount = 0;

		g_pXAudio2->CreateSourceVoice( &g_pSourceVoice[i] , &(g_wfx[i].Format) );
	}

	return hr;
}

//=============================================================================
// �J������
//=============================================================================
void XA_Release(void)
{
	for(int i=0; i<SOUND_LABEL_MAX; i++)
	{
		if(g_pSourceVoice[i])
		{
			g_pSourceVoice[i]->Stop( 0 );
			g_pSourceVoice[i]->FlushSourceBuffers();
			g_pSourceVoice[i]->DestroyVoice();			// �I�[�f�B�I�O���t����\�[�X�{�C�X���폜
			delete[]  g_DataBuffer[i];
		}
	}

	g_pMasteringVoice->DestroyVoice();
	
	if ( g_pXAudio2 ) g_pXAudio2->Release();
	
	// �b�n�l�̔j��
	CoUninitialize();
}

//=============================================================================
// �Đ�
//=============================================================================
void XA_Play(SOUND_LABEL label, float vol)
{

	if (!isXAPlay) return;

	IXAudio2SourceVoice*& pSV = g_pSourceVoice[(int)label];

	if (pSV != nullptr)
	{
		pSV->DestroyVoice();
		pSV = nullptr;
	}

	// �\�[�X�{�C�X�쐬
	g_pXAudio2->CreateSourceVoice( &pSV, &(g_wfx[(int)label].Format) );
	pSV->SubmitSourceBuffer( &(g_buffer[(int)label]) );	// �{�C�X�L���[�ɐV�����I�[�f�B�I�o�b�t�@�[��ǉ�
	//���ʐݒ�
	pSV->SetVolume(vol);
	// �Đ�
	pSV->Start( 0 );

}

//=============================================================================
// ��~
//=============================================================================
void XA_Stop(SOUND_LABEL label)
{
	if (g_pSourceVoice[(int)label] == nullptr) return;

	XAUDIO2_VOICE_STATE xa2state;
	g_pSourceVoice[(int)label]->GetState(&xa2state);

	// �����̒�~����
	if (xa2state.BuffersQueued > 0)
	{
		// �������Đ����̏ꍇ�A��~�������s��
		g_pSourceVoice[(int)label]->Stop(0);                // �Đ����~
		g_pSourceVoice[(int)label]->FlushSourceBuffers();    // �L���[���N���A
	}
}

//=============================================================================
// �ꎞ��~
//=============================================================================
void XA_Resume(SOUND_LABEL label)
{
	IXAudio2SourceVoice*& pSV = g_pSourceVoice[(int)label];
	pSV->Start();
}

void XA_SetSpeed(SOUND_LABEL label, float speed)
{
	IXAudio2SourceVoice*& pSV = g_pSourceVoice[(int)label];

	if (pSV != nullptr)
	{
		// ���g�����ύX���čĐ����x��ύX
		pSV->SetFrequencyRatio(speed);
	}
}
void XA_SetVolume(SOUND_LABEL label, float vol)
{
	IXAudio2SourceVoice*& pSV = g_pSourceVoice[(int)label];

	if (pSV != nullptr)
	{
		pSV->SetVolume(vol);
	}
}
//=============================================================================
// ���[�e�B���e�B�֐��Q
//=============================================================================
HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition)
{
	HRESULT hr = S_OK;
	if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, 0, NULL, FILE_BEGIN ) )
		return HRESULT_FROM_WIN32( GetLastError() );
	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD bytesRead = 0;
	DWORD dwOffset = 0;
	while (hr == S_OK)
	{
		DWORD dwRead;
		if( 0 == ReadFile( hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL ) )
			hr = HRESULT_FROM_WIN32( GetLastError() );
		if( 0 == ReadFile( hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL ) )
			hr = HRESULT_FROM_WIN32( GetLastError() );
		switch (dwChunkType)
		{
		case fourccRIFF:
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if( 0 == ReadFile( hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL ) )
				hr = HRESULT_FROM_WIN32( GetLastError() );
			break;
		default:
			if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, dwChunkDataSize, NULL, FILE_CURRENT ) )
				return HRESULT_FROM_WIN32( GetLastError() );
		}
		dwOffset += sizeof(DWORD) * 2;
		if (dwChunkType == fourcc)
		{
			dwChunkSize = dwChunkDataSize;
			dwChunkDataPosition = dwOffset;
			return S_OK;
		}
		dwOffset += dwChunkDataSize;
		if (bytesRead >= dwRIFFDataSize) return S_FALSE;
	}
	return S_OK;
}

bool XA_IsPlaying(SOUND_LABEL label)
{
	if (g_pSourceVoice[(int)label] == nullptr)
		return false;

	XAUDIO2_VOICE_STATE xa2state;
	g_pSourceVoice[(int)label]->GetState(&xa2state);

	// BuffersQueued �� 0 �ł��A�Đ����̃o�b�t�@�����邩���m�F����
	return xa2state.BuffersQueued > 0 || xa2state.pCurrentBufferContext != nullptr;
}

HRESULT ReadChunkData(HANDLE hFile, void * buffer, DWORD buffersize, DWORD bufferoffset)
{
	HRESULT hr = S_OK;
	if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, bufferoffset, NULL, FILE_BEGIN ) )
		return HRESULT_FROM_WIN32( GetLastError() );
	DWORD dwRead;
	if( 0 == ReadFile( hFile, buffer, buffersize, &dwRead, NULL ) )
		hr = HRESULT_FROM_WIN32( GetLastError() );
	return hr;
}