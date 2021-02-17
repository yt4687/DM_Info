///////////////////////////////////////
//
//  =====                             =====
//  ===== DMInfo (TVTest�p�v���O�C��) =====
//  ===== �f�W�I�̊y�ȏ���\������  =====
//  =====                             =====
//
// ======== ���p������ ====
// ���̃t�@�C����DMInfo�p�ɐV�K�ɍ쐬�������̂ł��B
// ���̃R�[�h�����p�\�t�g��V�F�A�E�F�A�ɍė��p���邱�Ƃ͕s�Ƃ��܂��B
// �t���[�\�t�g�ւ͂��̃R�[�h�����̂܂܁A���邢�͉��ς��čė��p���č\���܂��񂪁A
// �\���܂��񂪁A���̏ꍇ�A���p���������̃\�[�X�R�[�h������Ɠ��l�̏����Ŕz�z
// �p�b�P�[�W�ɓY�t���邱�Ƃ𗘗p�����Ƃ��܂��B
//
///////////////////////////////////////

//
//  �X�^�[�f�W�I�̊y�ȏ��ES����ȉ����擾����
//   �� �Ȗ�
//   �� �A�[�e�B�X�g��
//   �� �Ȃ̑�����
//   �� �Ȃ̐擪����̌o�ߎ���
//

#include <windows.h>
#include "LibDmInfo.h"
#include "ARIB8CharDecode.h"

//
// �f�o�b�O�p
//
#ifdef _DEBUG
#include <stdio.h>
#endif
static void TRACE( const char* pszForm,... )
{
	#ifdef _DEBUG
	char szBuf[10000] ;
	va_list vlMarker ;
	va_start( vlMarker, pszForm ) ;
	vsprintf_s( szBuf, pszForm, vlMarker ) ;
	va_end( vlMarker ) ;
	OutputDebugStringA(szBuf) ;
	#endif
}

//
// �X�^�[�f�W�I�̊y�ȏ����擾���郉�C�u����
// (���`�����l���Ή����l�����N���X��)
//
class CSongInfo
{
public:
	CSongInfo() ;
	~CSongInfo() ;
	void Reset() ;
	BOOL PutPacket( int nCh, const BYTE* pyPacket ) ;
public:
	DWORD m_dTime0 ;						// �Ȃ̑�����
	DWORD m_dTime1 ;						// �Ȑ擪����̌o�ߎ���
	WCHAR m_wcsTitleJ[DI_NAME_LEN+1];		// �y�Ȗ�(���{��)
	WCHAR m_wcsArtistJ[DI_NAME_LEN+1];		// �A�[�e�B�X�g��(���{��)
	WCHAR m_wcsTitleE[DI_NAME_LEN+1];		// �y�Ȗ�(�p���\�L)
	WCHAR m_wcsArtistE[DI_NAME_LEN+1];		// �A�[�e�B�X�g��(�p���\�L)
private:
	int m_nTstat ;
	BYTE m_ayTbuf[DI_NAME_LEN*2] ;
	DWORD m_dTbytes ;
	DWORD m_dPacketDataPos ;
	DWORD m_dPosNext ;
	DWORD m_dPosTime0 ;
	DWORD m_dPosTime1 ;
	DWORD m_dPosTitleJ ;
	DWORD m_dPosArtistJ ;
	DWORD m_dPosTitleE ;
	DWORD m_dPosArtistE ;
} ;

/******************************************************************
***																***
***		Name:	constructor/destructor  						***
***																***
*************************<< Function >>****************************
***																***
***																***
***																***
******************************************************************/

CSongInfo::CSongInfo()
{
								/*************************************/
								/**	initialize data 				**/
								/*************************************/
	Reset() ;
}

CSongInfo::~CSongInfo()
{
}

/******************************************************************
***																***
***		Name:	reset data  									***
***																***
*************************<< Function >>****************************
***																***
***																***
***																***
******************************************************************/

void CSongInfo::Reset()
{
	m_dTime0 = 0 ;
	m_dTime1 = 0 ;
	wcscpy_s( m_wcsTitleJ, L"" ) ;
	wcscpy_s( m_wcsArtistJ, L"" ) ;
	wcscpy_s( m_wcsTitleE, L"" ) ;
	wcscpy_s( m_wcsArtistE, L"" ) ;
	m_nTstat = 0 ;
}

/******************************************************************
***																***
***		Name:	put & analyze packet							***
***																***
*************************<< Function >>****************************
***																***
***																***
***																***
******************************************************************/

BOOL CSongInfo::PutPacket( int nCh, const BYTE* pyPacket )
{
								/*************************************/
								/**	check payload_unit_start_indicat**/
								/*************************************/
	if ( pyPacket[0x01] & 0x40 ) {
		if ( m_nTstat != 0 ) {
			TRACE( "Ch%03u : payload_unit_start_indicator=1 in Tstat=%d.\n",
				nCh, m_nTstat ) ;
		}
		m_nTstat = 1 ;
		m_dTbytes = 0 ;
	}
	else if ( m_nTstat == 0 ) return FALSE ;
								/*************************************/
								/**	calc. payload start pos.		**/
								/*************************************/
	DWORD dPos ;
	switch ( pyPacket[3] & 0x30 ) {
	case 0x00 :									// invalid afc
	case 0x20 : return FALSE ;					// no payload
	case 0x10 : dPos = 4 ; break ;				// payload only
	case 0x30 : dPos = 4 + pyPacket[4] + 1 ;	// with adaptation field
		if ( dPos >= 188 ) {
			TRACE( "Ch%03u : Too large adaptation field length.\n", nCh ) ;
			m_nTstat = 0 ;
			return FALSE ;
		}
	}
								/*************************************/
								/**	copy data   					**/
								/*************************************/
	memcpy( m_ayTbuf + m_dTbytes, pyPacket + dPos, 188 - dPos ) ;
	m_dTbytes += 188 - dPos ;
	if ( m_dTbytes > sizeof(m_ayTbuf) - 188 ) {
		TRACE( "Ch%03u : Payload is too large.\n", nCh ) ;
		m_nTstat = 0 ;
		return FALSE ;
	}
								/*************************************/
								/**	check state 					**/
								/*************************************/
	while ( TRUE ) {
								/*************************************/
								/**	check PES header				**/
								/*************************************/
		if ( m_nTstat == 1 ) {
			if ( m_dTbytes < 9 ) return FALSE ;
			if ( m_ayTbuf[0] != 0x00 || m_ayTbuf[1] != 0x00 ||
				m_ayTbuf[2] != 0x01 ) {
				TRACE( "Ch%03u : Invalid packet start code.\n", nCh ) ;
				m_nTstat = 0 ;
				return FALSE ;
			}
			if ( m_ayTbuf[3] != 0xbd ) {
				TRACE( "Ch%03u : Invalid stream id.\n", nCh ) ;
				m_nTstat = 0 ;
				return FALSE ;
			}
			m_dPacketDataPos = 9 + m_ayTbuf[8] ;
			m_nTstat = 2 ;
			continue ;
		}
								/*************************************/
								/**	check sub stream number         **/
								/*************************************/
		if ( m_nTstat == 2 ) {
			if ( m_dTbytes < m_dPacketDataPos + 1 ) return FALSE ;
			if ( m_ayTbuf[m_dPacketDataPos] != 0x81 ) {
				TRACE( "Ch%03u : Invalid substream-ID.\n", nCh ) ;
				m_nTstat = 0 ;
				return FALSE ;
			}
			m_dPosNext = m_dPacketDataPos + 1 ;
			m_dPosTime0 = m_dPosTime1 = 0 ;
			m_dPosTitleJ = m_dPosArtistJ = m_dPosTitleE = m_dPosArtistE = 0 ;
			m_nTstat = 3 ;
			continue ;
		}
								/*************************************/
								/**	check if complete   			**/
								/*************************************/
		if ( m_dPosTime0 && m_dPosTime1 &&
			m_dPosTitleJ && m_dPosArtistJ &&
			m_dPosTitleE && m_dPosArtistE &&
			m_dTbytes >= m_dPosNext ) {
			m_nTstat = 0 ;
			break ;
		}
								/*************************************/
								/**	check tag   					**/
								/*************************************/
		if ( m_dTbytes < m_dPosNext + 2 ) return FALSE ;
		switch ( m_ayTbuf[m_dPosNext] ) {
		case 0xa0 : m_dPosTime0  = m_dPosNext ; break ;
		case 0xa1 : m_dPosTime1  = m_dPosNext ; break ;
		case 0xa2 : m_dPosTitleJ  = m_dPosNext ; break ;
		case 0xa3 : m_dPosArtistJ = m_dPosNext ; break ;
		case 0xa4 : m_dPosTitleE  = m_dPosNext ; break ;
		case 0xa5 : m_dPosArtistE = m_dPosNext ; break ;
		default :
			TRACE( "Ch%03u : Invalid TAG code.\n", nCh ) ;
			m_nTstat = 0 ;
			return FALSE ;
		}
		m_dPosNext += m_ayTbuf[m_dPosNext+1] + 2 ;
		continue ;
	}
								/*************************************/
								/**	BCD -> binary   				**/
								/*************************************/
	for ( int i = 0 ; i < 2 ; i++ ) {
		const BYTE* p = i ? &m_ayTbuf[m_dPosTime1+1] : &m_ayTbuf[m_dPosTime0+1] ;
		DWORD d = 0 ;
		for ( BYTE j = 1 ; j <= p[0] ; j++ ) {
			DWORD d10 = p[j] >> 4 ;
			DWORD d1 = p[j] & 0xf ;
			if ( d10 > 9 || d1 > 9 ) {
				TRACE( "Ch%03u : Invalid Time-Code.\n", nCh ) ;
			}
			d = d * 100 + d10 * 10 + d1 ;
		}
		if ( i ) m_dTime1 = d ;
		else m_dTime0 = d ;
	}
								/*************************************/
								/**	ARIB8 -> UNICODE				**/
								/*************************************/
	for ( int i = 0 ; i < 4 ; i++ ) {
		DWORD dPos ;
		WCHAR* pws ;
		switch( i ) {
		case 0 : dPos = m_dPosTitleJ  ; pws = m_wcsTitleJ  ; break ;
		case 1 : dPos = m_dPosArtistJ ; pws = m_wcsArtistJ ; break ;
		case 2 : dPos = m_dPosTitleE  ; pws = m_wcsTitleE  ; break ;
		case 3 : dPos = m_dPosArtistE ; pws = m_wcsArtistE ; break ;
		}
		WCHAR wszTmp[DI_NAME_LEN] ;
		memset( wszTmp, 0, sizeof(wszTmp) ) ;
		ARIB8toUNICODE( &m_ayTbuf[dPos+2], m_ayTbuf[dPos+1],
			wszTmp, DI_NAME_LEN, NULL ) ;
		for ( int i = 0 ; WCHAR& C = wszTmp[i]; i++ ) {
			if ( C < 0x20 ) {
				C = L'�^' ;
				continue ;
			}
			// �\�Ȃ�S�p�𔼊p�ɕϊ�
			static const WCHAR CT1[] =
				L"�@�I�h���������f�i�j���{�C�|�D�^"
				L"�O�P�Q�R�S�T�U�V�W�X�F�G�������H"
				L"�`�a�b�c�d�e�f�g�h�i�j�k�l�m�n�o�p�q�r�s�t�u�v�w�x�y"
				L"����������������������������������������������������"
				L"���m���n�O�Q�o�b�p"
				//L"�u�v"
				L"";
			static const WCHAR CT2[] =
				L" !\"#$%&'()*+,-./"
				L"0123456789:;<=>?"
				L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				L"abcdefghijklmnopqrstuvwxyz"
				L"@[\\]^_{|}"
				//L"��"
				L"";
			const WCHAR* p = wcschr( CT1, C ) ;
			if ( !p ) continue ;
			C = CT2[p-CT1] ;
		}
		wcscpy_s( pws, DI_NAME_LEN, wszTmp ) ;
	}
	return TRUE ;
}

/******************************************************************
***																***
***		Name:	���J�֐�										***
***																***
*************************<< Function >>****************************
***																***
***																***
***																***
******************************************************************/

static CSongInfo SongInfo ;

				//
				// ��Ԃ̏�����
				//   �A�v���N�����A�y�сA�`�����l���ύX���Ŋy�ȏ��ES
				//   �̘A������������ꍇ�ɁA���֐����ĂԕK�v����B
				//
void LibDI_Reset( void )
{
	SongInfo.Reset() ;
}

				//
				// �y�ȏ��ES��������擾
				//   �Ȗ����������A��񂪕����p�P�b�g�Ɍׂ�ꍇ�A���
				//   �m�肷��ŏI�p�P�b�g�ȊO�ł�FALSE��Ԃ��B
				//   ���̎��ADI_Info�ɕԂ��f�[�^�͎g�p�s�B
				//
BOOL LibDI_GetInfo(
	const BYTE* pyPacket,				// �y�ȏ��ES�̃p�P�b�g(188�o�C�g)
	DI_Info* pInfo )					// �y�ȏ���Ԃ��\����
{
	if ( !SongInfo.PutPacket( 0, pyPacket ) ) return FALSE ;
	wcscpy_s( pInfo->wcsTitle, SongInfo.m_wcsTitleJ ) ;
	wcscpy_s( pInfo->wcsArtist, SongInfo.m_wcsArtistJ ) ;
	pInfo->dwDuration = SongInfo.m_dTime0 ;
	pInfo->dwElapsed = SongInfo.m_dTime1 ;
	if ( SongInfo.m_dTime0 == 0 ||
		!wcscmp( SongInfo.m_wcsTitleJ , L"" ) ) pInfo->bIntermisson = TRUE ;
	else pInfo->bIntermisson = FALSE ;
	return TRUE ;
}

