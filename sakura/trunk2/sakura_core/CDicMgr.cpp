/*!	@file
	@brief CDicMgr�N���X

	@author Norio Nakatani
	@date	1998/11/05 �쐬
*/
/*
	Copyright (C) 1998-2001, Norio Nakatani
	Copyright (C) 2002, aroka, Moca
	Copyright (C) 2003, Moca
	Copyright (C) 2006, fon
	Copyright (C) 2007, ryoji

	This source code is designed for sakura editor.
	Please contact the copyright holder to use this code for other purpose.
*/

#include "StdAfx.h"
#include <stdio.h>
#include "CDicMgr.h"
#include "mem/CMemory.h" // 2002/2/10 aroka �w�b�_����
#include "debug/CRunningTimer.h"
#include "io/CTextStream.h"
using namespace std;

CDicMgr::CDicMgr()
{
	return;
}




CDicMgr::~CDicMgr()
{
	return;
}




/*!
	�L�[���[�h�̌���
	�ŏ��Ɍ��������L�[���[�h�̈Ӗ���Ԃ�

	@date 2006.04.10 fon �����q�b�g�s��Ԃ�����pLine��ǉ�
*/
BOOL CDicMgr::Search(
	const wchar_t*		pszKey,				//!< �����L�[���[�h
	const int			nCmpLen,			//!< �����L�[���[�h�̒���
	CNativeW**			ppcmemKey,			//!< ���������L�[���[�h�D�Ăяo�����̐ӔC�ŉ������D
	CNativeW**			ppcmemMean,			//!< ���������L�[���[�h�ɑΉ����鎫�����e�D�Ăяo�����̐ӔC�ŉ������D
	const TCHAR*		pszKeyWordHelpFile,	//!< �L�[���[�h�w���v�t�@�C���̃p�X��
	int*				pLine				//!< ���������L�[���[�h�̃L�[���[�h�w���v�t�@�C�����ł̍s�ԍ�
)
{
#ifdef _DEBUG
	CRunningTimer cRunningTimer( "CDicMgr::Search" );
#endif
	long	i;
	const wchar_t*	pszDelimit = L" /// ";
	wchar_t*	pszWork;
	int		nRes;
	wchar_t*	pszToken;
	const wchar_t*	pszKeySeps = L",\0";


	/* �����t�@�C�� */
	if( 0 >= _tcslen( pszKeyWordHelpFile ) ){
		return FALSE;
	}
	// 2003.06.23 Moca ���΃p�X�͎��s�t�@�C������̃p�X�Ƃ��ĊJ��
	// 2007.05.19 ryoji ���΃p�X�͐ݒ�t�@�C������̃p�X��D��
	CTextInputStream_AbsIni in(pszKeyWordHelpFile);
	if(!in){
		return FALSE;
	}

	wchar_t	szLine[LINEREADBUFSIZE];
	for(int line=1 ; in; line++ ){	// 2006.04.10 fon
		//1�s�ǂݍ���
		{
			wstring tmp = in.ReadLineW(); //NULL != fgetws( szLine, _countof(szLine), pFile );
			wcsncpy_s(szLine,_countof(szLine),tmp.c_str(), _TRUNCATE);
			// auto_strlcpy(szLine,tmp.c_str(), _countof(szLine));
		}

		pszWork = wcsstr( szLine, pszDelimit );
		if( NULL != pszWork && szLine[0] != L';' ){
			*pszWork = L'\0';
			pszWork += wcslen( pszDelimit );

			/* �ŏ��̃g�[�N�����擾���܂��B */
			pszToken = wcstok( szLine, pszKeySeps );
			while( NULL != pszToken ){
				nRes = _wcsnicmp( pszKey, pszToken, nCmpLen );	// 2006.04.10 fon
				if( 0 == nRes ){
					for( i = 0; i < (int)wcslen(pszWork); ++i ){
						if( pszWork[i] == L'\r' ||
							pszWork[i] == L'\n' ){
							pszWork[i] = L'\0';
							break;
						}
					}
					//�L�[���[�h�̃Z�b�g
					*ppcmemKey = new CNativeW;	// 2006.04.10 fon
					(*ppcmemKey)->SetString( pszToken );
					//�Ӗ��̃Z�b�g
					*ppcmemMean = new CNativeW;
					(*ppcmemMean)->SetString( pszWork );

					*pLine = line;	// 2006.04.10 fon
					return TRUE;
				}
				pszToken = wcstok( NULL, pszKeySeps );
			}
		}
	}
	return FALSE;
}





/*
||  ���͕⊮�L�[���[�h�̌���
||
||  �E�w�肳�ꂽ���̍ő吔�𒴂���Ə����𒆒f����
||  �E������������Ԃ�
||
*/
int CDicMgr::HokanSearch(
	const wchar_t*	pszKey,
	BOOL			bHokanLoHiCase,	//!< �p�啶���������𓯈ꎋ����
	vector_ex<std::wstring>&		vKouho,	//!< [out] ��⃊�X�g
	int				nMaxKouho,		//!< Max��␔(0==������)
	const TCHAR*	pszKeyWordFile
)
{
	int		nKeyLen;
	int		nKouhoNum;
	int		nRet;
	if( 0 >= _tcslen( pszKeyWordFile ) ){
		return 0;
	}

	CTextInputStream_AbsIni in(pszKeyWordFile);
	if(!in){
		return 0;
	}
	nKouhoNum = 0;
	nKeyLen = wcslen( pszKey );
	wstring szLine;
	while( in ){
		szLine = in.ReadLineW();
		if( nKeyLen > (int)szLine.length() ){
			continue;
		}

		//�R�����g����
		if( szLine[0] == L';' )continue;

		//��s����
		if( szLine.length() == 0 )continue;

		if( bHokanLoHiCase ){	/* �p�啶���������𓯈ꎋ���� */
			nRet = auto_memicmp( pszKey, szLine.c_str(), nKeyLen );
		}else{
			nRet = auto_memcmp( pszKey, szLine.c_str(), nKeyLen );
		}
		if( 0 == nRet ){
			vKouho.push_back( szLine );
			if( 0 != nMaxKouho && nMaxKouho <= (int)vKouho.size() ){
				break;
			}
		}
	}
	in.Close();
	return (int)vKouho.size();
}


