///////////////////////////////////////
//
//  =====                             =====
//  ===== DMInfo (TVTest�p�v���O�C��) =====
//  ===== �f�W�I�̊y�ȏ���\������  =====
//  =====                             =====
//
// ======== ���p������ ====
// ���̃t�@�C����TVTestVer0.9.0��
//   PacketCounter�v���O�C��
//   TSInfo�v���O�C��
// �̃R�[�h�𗘗p���č쐬���Ă���A�����ɏ�����Public Domain
// �Ƃ��܂��B
//
///////////////////////////////////////

#include <windows.h>
#include <tchar.h>
#define TVTEST_PLUGIN_CLASS_IMPLEMENT
#include "TVTestPlugin.h"
#include "resource.h"

#include "LibDmInfo.h"

//
// �v���O�C���N���X
//
class CDMInfo : public TVTest::CTVTestPlugin
{
private:
	// �����o�[�ϐ�
	HWND m_hwnd;						// �_�C�A���O��Window�n���h��
	HBRUSH m_hbrBack;					// �_�C�A���O�w�i�p�̃u���V
	COLORREF m_crTextColor;				// �_�C�A���O�̕����F
	DWORD m_TextDataPid ;				// �y�ȏ��ES��PID
	DWORD m_LastChNum ;					// ���݂̃`�����l���ԍ�(�ύX���o�p)
	CRITICAL_SECTION m_cs ;				// LibDmInfo�Em_di�̃X���b�h�Ԕr���A�N�Z�X�p
	DI_Info m_di ;						// �y�ȏ��̃X���b�h�Ԏ󂯓n���p
	BOOL m_bEnabled ;					// Plugin��Enable
	BOOL m_bIsDigio ;					// �f�W�I����M��
	BOOL m_bShow ;						// �_�C�A���O��\����
	BOOL m_bCounting ;					// �V�X�e���N���b�N�Ŏ��ԃJ�E���g��
	DWORD m_dStartTick ;				// �ȊJ�n����TickCount(ms)
	DWORD m_dDuration ;					// �Ȃ̑�����(ms)
	DWORD m_dDispSec ;					// �\������Ă��鎞��(�b)
public:
	// �R���X�g���N�^�[
	CDMInfo()
	{
		// �����o�[�ϐ�������
		m_hwnd = NULL;
		m_hbrBack = NULL;
		m_TextDataPid = DWORD(-1);
		m_LastChNum = DWORD(-1) ;
		InitializeCriticalSection(&m_cs) ;
		m_bEnabled = FALSE ;
		m_bIsDigio = FALSE ;
		m_bShow = FALSE ;
		m_bCounting = FALSE ;
	}
private:
	// �����o�[�֐�
	virtual bool GetPluginInfo(TVTest::PluginInfo *pInfo);
	virtual bool Initialize();
	virtual bool Finalize();
	static INT_PTR CALLBACK DlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam,void *pClientData);
	static LRESULT CALLBACK EventCallback(UINT Event,LPARAM lParam1,LPARAM lParam2,void *pClientData);
	static BOOL CALLBACK StreamCallback(BYTE *pData, void *pClientData);
	void SetItemText(int ID,LPCTSTR pszText);
	void UpdateChInfo();
	void UpdateMusicInfo();
	void ShowHide();
	void UpdateTime() ;
};

// �v���O�C���N���X�̃C���X�^���X�𐶐�����
TVTest::CTVTestPlugin *CreatePluginClass()
{
	return new CDMInfo;
}


// �v���O�C���̏���Ԃ�
bool CDMInfo::GetPluginInfo(TVTest::PluginInfo *pInfo)
{
	pInfo->Type           = TVTest::PLUGIN_TYPE_NORMAL;
	pInfo->Flags          = 0;
	pInfo->pszPluginName  = L"�X�^�[�f�W�I�E�y�ȏ��";
	pInfo->pszCopyright   = L"�e�\�[�X�t�@�C�����Q��";
	pInfo->pszDescription = L"�X�^�[�f�W�I�̊y�ȏ���\�����܂��B";
	return true;
}

// �v���O�C���̏���������
bool CDMInfo::Initialize()
{
	// �C�x���g�R�[���o�b�N�֐���o�^
	m_pApp->SetEventCallback(EventCallback,this);
	// �X�g���[���R�[���o�b�N�֐���o�^
	m_pApp->SetStreamCallback(0, StreamCallback, this);
	return true;
}

// �v���O�C���̏I������
bool CDMInfo::Finalize()
{
	// �E�B���h�E�̔j��
	if (m_hwnd!=NULL) ::DestroyWindow(m_hwnd);
	return true;
}

//
// ���b�Z�[�W�E�n���h��
//
INT_PTR CALLBACK CDMInfo::DlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam,void *pClientData)
{
	CDMInfo *pThis=static_cast<CDMInfo*>(pClientData);
	HDC hdc ;
	switch (uMsg) {
	case WM_INITDIALOG:
		// �_�C�A���O���������A�w�i�E�����F���擾
		pThis->m_hwnd=hDlg;
		pThis->m_hbrBack=::CreateSolidBrush(pThis->m_pApp->GetColor(L"PanelBack"));
		pThis->m_crTextColor=pThis->m_pApp->GetColor(L"PanelText");
		return TRUE;
	case WM_TIMER:
		// �`�����l�����X�V
		pThis->UpdateChInfo();
		// �o�ߎ��ԕ\���X�V
		pThis->UpdateTime();
		return TRUE;
	case WM_CTLCOLORSTATIC:
		// ���ڂ̔w�i�F��ݒ�
		hdc=reinterpret_cast<HDC>(wParam);
		::SetBkMode(hdc,TRANSPARENT);
		::SetTextColor(hdc,pThis->m_crTextColor);
		return reinterpret_cast<INT_PTR>(pThis->m_hbrBack);
	case WM_CTLCOLORDLG:
		// �_�C�A���O�̔w�i�F��ݒ�
		return reinterpret_cast<INT_PTR>(pThis->m_hbrBack);
	case WM_COMMAND:
		if (LOWORD(wParam)==IDCANCEL) {
			// ���鎞�̓v���O�C���𖳌��ɂ���
			pThis->m_pApp->EnablePlugin(false);
			return TRUE;
		}
		else return FALSE ; // IDCANCEL�ȊO�͖�����
	case WM_DESTROY:
		// �_�C�A���O�̏I�����AGDI�I�u�W�F�N�g���J��
		::KillTimer(hDlg,1);
		if (pThis->m_hbrBack!=NULL) {
			::DeleteObject(pThis->m_hbrBack);
			pThis->m_hbrBack=NULL;
		}
		return TRUE;
	case WM_USER+1:
		// �y�ȏ��擾�̒ʒm�ŕ\���X�V
		pThis->UpdateMusicInfo() ;
		return TRUE;
	default:
		// �������̃��b�Z�[�W
		return FALSE;
	}
}

// �C�x���g�R�[���o�b�N�֐�
// �����C�x���g���N����ƌĂ΂��
LRESULT CALLBACK CDMInfo::EventCallback(UINT Event,LPARAM lParam1,LPARAM lParam2,void *pClientData)
{
	CDMInfo *pThis=static_cast<CDMInfo*>(pClientData);
	switch (Event) {
	case TVTest::EVENT_PLUGINENABLE:
		// �v���O�C���̗L����Ԃ��ω�����
		pThis->m_bEnabled=lParam1!=0;
		if (pThis->m_bEnabled) {
			// �_�C�A���O�̍쐬
			if (pThis->m_hwnd==NULL) {
				TVTest::ShowDialogInfo Info;
				Info.Flags = TVTest::SHOW_DIALOG_FLAG_MODELESS;
				Info.hinst = g_hinstDLL;
				Info.pszTemplate = MAKEINTRESOURCE(IDD_MAIN);
				Info.pMessageFunc = DlgProc;
				Info.pClientData = pThis;
				Info.hwndOwner = pThis->m_pApp->GetAppWindow();
				if ((HWND)pThis->m_pApp->ShowDialog(&Info) == NULL)
					return FALSE;
			}
			// �`�����l�����X�V
			pThis->UpdateChInfo();
		}
		// �_�C�A���O�̕\��
		pThis->ShowHide() ;
		// �^�C�}�[�̋N���E��~
		if (pThis->m_bEnabled) ::SetTimer(pThis->m_hwnd,1,100,NULL);
		else ::KillTimer(pThis->m_hwnd,1);
		return TRUE;
	case TVTest::EVENT_COLORCHANGE:
		// �F�̐ݒ肪�ω�����
		if (pThis->m_hwnd!=NULL) {
			HBRUSH hbrBack=::CreateSolidBrush(pThis->m_pApp->GetColor(L"PanelBack"));
			if (hbrBack!=NULL) {
				if (pThis->m_hbrBack!=NULL)
					::DeleteObject(pThis->m_hbrBack);
				pThis->m_hbrBack=hbrBack;
			}
			pThis->m_crTextColor=pThis->m_pApp->GetColor(L"PanelText");
			::RedrawWindow(pThis->m_hwnd,NULL,NULL,RDW_INVALIDATE | RDW_UPDATENOW);
		}
		return TRUE;
	}
	return FALSE;
}

// �X�g���[���R�[���o�b�N�֐�
// 188�o�C�g�̃p�P�b�g�f�[�^���n�����
BOOL CALLBACK CDMInfo::StreamCallback(BYTE *pData, void *pClientData)
{
	CDMInfo *pThis = static_cast<CDMInfo*>(pClientData);
	// TS�w�b�_�[�̍\����`
	#pragma pack(1)
	typedef struct _PacketHeader {
		BYTE sync_byte ;
		BYTE PID_h : 5 ;
		BYTE transport_priority : 1 ;
		BYTE payload_unit_start_indicator : 1 ;
		BYTE transport_error_indicator : 1 ;
		BYTE PID_l ;
		BYTE continuity_counter : 4 ;
		BYTE adaptation_field_control : 2 ;
		BYTE transport_scrambling_control : 2 ;
		BYTE adaptation_field_length ;
		BYTE flags : 5 ;
		BYTE elementary_stream_priority_indicator : 1 ;
		BYTE random_access_indicator : 1 ;
		BYTE discontinuity_indicator : 1 ;
	} PacketHeader ;
	#pragma pack()
	PacketHeader* pH = (PacketHeader*)pData;
	// �ُ�̂���p�P�b�g�͖���
	if ( pH->sync_byte != 0x47 ) return FALSE ;
	if ( pH->transport_error_indicator ) return TRUE ;
	if ( pH->transport_scrambling_control ) return TRUE ;
	// �y�ȏ��ES��PID�ȊO�͖���
	WORD wPid = ( WORD(pH->PID_h) << 8 ) + pH->PID_l ;
	if ( pThis->m_TextDataPid != wPid ) return TRUE;
	// �y�ȏ����擾����
	//   StreamCallback()�͔񃁃C���X���b�h�ŌĂяo����邽��
	//   ��������_�C�A���O�̍X�V�͂ł��Ȃ�
	//   ����m_dl�ɕۑ����A���b�Z�[�W"WM_USER+1"�Ń��C���X���b�h��
	//   �ʒm����B
	EnterCriticalSection(&pThis->m_cs) ; // �X���b�h�Ԕr���A�N�Z�X
	if ( LibDI_GetInfo( pData, &pThis->m_di ) ) {
		::PostMessage( pThis->m_hwnd, WM_USER+1, 0, 0 ) ;
	}
	LeaveCriticalSection(&pThis->m_cs) ;
	return TRUE ;
}

// ���ڂ̕������ݒ肷��
void CDMInfo::SetItemText(int ID,LPCTSTR pszText)
{
	TCHAR szCurText[256] = L"" ;
	// �I�������������̂Ƃ�����h�~�̂��߂ɁA�ω������������̂ݐݒ肷��
	::GetDlgItemText(m_hwnd,ID,szCurText,256);
	if (::lstrcmp(szCurText,pszText)!=0) ::SetDlgItemText(m_hwnd,ID,pszText);
}

// �`�����l�������X�V����
void CDMInfo::UpdateChInfo()
{
	// �`�����l�������擾�E�\������
	TVTest::ChannelInfo ChannelInfo;
	if (!m_pApp->GetCurrentChannelInfo(&ChannelInfo)) return ;
	TCHAR szText[256];
	::wsprintf(szText,TEXT("ch.%03d"),ChannelInfo.RemoteControlKeyID);
	SetItemText(IDC_CHNUM,szText);
	SetItemText(IDC_CHNAME,ChannelInfo.szChannelName);
	// �`�����l�����ς������A�y�ȏ�����U���Z�b�g
	if ( m_LastChNum != ChannelInfo.RemoteControlKeyID ) {
		m_LastChNum = ChannelInfo.RemoteControlKeyID ;
		EnterCriticalSection(&m_cs) ; // �X���b�h�Ԕr���A�N�Z�X
		LibDI_Reset() ;
		LeaveCriticalSection(&m_cs) ;
		m_TextDataPid = DWORD(-1) ;
		SetItemText(IDC_TITLE,   L"");
		SetItemText(IDC_ARTIST,  L"");
		SetItemText(IDC_DURATION,L"");
		SetItemText(IDC_ELAPSED, L"");
		m_bCounting = FALSE ;
	}
	// �f�W�I�ȊO�̎������̓_�C�A���O���\��
	m_bIsDigio =
		//( ChannelInfo.NetworkID == 1 ) &&
		( ChannelInfo.TransportStreamID == 21 ) &&
		( ChannelInfo.ServiceID >= 400 ) &&
		( ChannelInfo.ServiceID <= 499 ) ;
	ShowHide() ;
	if ( !m_bIsDigio ) return ;
	// �y�ȏ��ES��PID���擾
	int CurService,NumServices;
	TVTest::ServiceInfo ServiceInfo;
	CurService=m_pApp->GetService(&NumServices);
	if (CurService>=0 && 
		m_pApp->GetServiceInfo(CurService,&ServiceInfo)) {
		m_TextDataPid = ServiceInfo.SubtitlePID ;
	}
}

// �y�ȏ��̍X�V
void CDMInfo::UpdateMusicInfo()
{
	EnterCriticalSection(&m_cs) ; // �X���b�h�Ԕr���A�N�Z�X
	DI_Info di = m_di ;
	LeaveCriticalSection(&m_cs) ; // �X���b�h�Ԕr���A�N�Z�X
	if ( di.bIntermisson ) { // �ԑg�ԓ��A�y�ȏ��̖����ꍇ
		SetItemText(IDC_TITLE,   L"-------");
		SetItemText(IDC_ARTIST,  L"-------");
		SetItemText(IDC_DURATION,L"-:--:--");
		SetItemText(IDC_ELAPSED, L"-:--:--");
		return ;
	}
	SetItemText(IDC_TITLE,   di.wcsTitle); // �Ȗ��\��
	SetItemText(IDC_ARTIST,  di.wcsArtist); // �A�[�e�B�X�g���\��
	DWORD dwTime = di.dwDuration ; // �Ȃ̑����Ԃ�\��
	DWORD dwH = dwTime / 3600 ;
	DWORD dwM = ( dwTime % 3600 ) / 60 ;
	DWORD dwS = dwTime % 60  ;
	WCHAR wsz[256] ;
	::wsprintf( wsz, L"%d:%02d:%02d", dwH, dwM, dwS ) ;
	SetItemText(IDC_DURATION,wsz);
	if ( di.dwElapsed == 0 ) { // �Ȃ̐擪�Ȃ�J�E���g�J�n
		m_bCounting = TRUE ;
		m_dStartTick = GetTickCount() ;
		m_dDuration = di.dwDuration * 1000 ;
		m_dDispSec = DWORD(-1) ;
	}
	else if ( !m_bCounting ) { // �J�E���g���łȂ���Γr������J�E���g�J�n
		m_bCounting = TRUE ;
		m_dStartTick = GetTickCount() - di.dwElapsed * 1000 ;
		m_dDuration = di.dwDuration * 1000 ;
		m_dDispSec = DWORD(-1) ;
	}
	UpdateTime() ;
}

// �_�C�A���O�̕\���E��\��
void CDMInfo::ShowHide()
{
	if ( m_bEnabled && m_bIsDigio ) { // �v���O�C�������f�W�I�������Ȃ�\��
		if ( !m_bShow ) {
			m_bShow = TRUE ;
			::ShowWindow(m_hwnd,SW_SHOW);
			::SetFocus(m_pApp->GetAppWindow()); // �t�H�[�J�X��TVTest�{�̂ɕԂ�
		}
	}
	else { // �����łȂ��Ȃ��\��
		if ( m_bShow ) {
			m_bShow = FALSE ;
			::ShowWindow(m_hwnd,SW_HIDE);
		}
	}
}

// �o�ߎ��Ԃ̕\���̍X�V
void CDMInfo::UpdateTime()
{
	if ( !m_bCounting ) return ; // �J�E���g���H
	DWORD dwElapsed = ( GetTickCount() - m_dStartTick ) / 1000 ; // �o�ߎ���
	if ( m_dDispSec == dwElapsed ) return ; // �\�����̎��ԂƓ���
	m_dDispSec = dwElapsed ; // �V�������Ԃ�\��
	DWORD dwTime = dwElapsed ;
	DWORD dwH = dwTime / 3600 ;
	DWORD dwM = ( dwTime % 3600 ) / 60 ;
	DWORD dwS = dwTime % 60  ;
	WCHAR wsz[256] ;
	::wsprintf( wsz, L"%d:%02d:%02d", dwH, dwM, dwS ) ;
	SetItemText(IDC_ELAPSED,wsz);
}

