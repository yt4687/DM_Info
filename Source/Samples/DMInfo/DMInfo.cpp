///////////////////////////////////////
//
//  =====                             =====
//  ===== DMInfo (TVTest用プラグイン) =====
//  ===== デジオの楽曲情報を表示する  =====
//  =====                             =====
//
// ======== 利用条件等 ====
// このファイルはTVTestVer0.9.0の
//   PacketCounterプラグイン
//   TSInfoプラグイン
// のコードを利用して作成しており、これらに準じてPublic Domain
// とします。
//
///////////////////////////////////////

#include <windows.h>
#include <tchar.h>
#define TVTEST_PLUGIN_CLASS_IMPLEMENT
#include "TVTestPlugin.h"
#include "resource.h"

#include "LibDmInfo.h"

//
// プラグインクラス
//
class CDMInfo : public TVTest::CTVTestPlugin
{
private:
	// メンバー変数
	HWND m_hwnd;						// ダイアログのWindowハンドル
	HBRUSH m_hbrBack;					// ダイアログ背景用のブラシ
	COLORREF m_crTextColor;				// ダイアログの文字色
	DWORD m_TextDataPid ;				// 楽曲情報ESのPID
	DWORD m_LastChNum ;					// 現在のチャンネル番号(変更検出用)
	CRITICAL_SECTION m_cs ;				// LibDmInfo・m_diのスレッド間排他アクセス用
	DI_Info m_di ;						// 楽曲情報のスレッド間受け渡し用
	BOOL m_bEnabled ;					// PluginがEnable
	BOOL m_bIsDigio ;					// デジオを受信中
	BOOL m_bShow ;						// ダイアログを表示中
	BOOL m_bCounting ;					// システムクロックで時間カウント中
	DWORD m_dStartTick ;				// 曲開始時のTickCount(ms)
	DWORD m_dDuration ;					// 曲の総時間(ms)
	DWORD m_dDispSec ;					// 表示されている時間(秒)
public:
	// コンストラクター
	CDMInfo()
	{
		// メンバー変数初期化
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
	// メンバー関数
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

// プラグインクラスのインスタンスを生成する
TVTest::CTVTestPlugin *CreatePluginClass()
{
	return new CDMInfo;
}


// プラグインの情報を返す
bool CDMInfo::GetPluginInfo(TVTest::PluginInfo *pInfo)
{
	pInfo->Type           = TVTest::PLUGIN_TYPE_NORMAL;
	pInfo->Flags          = 0;
	pInfo->pszPluginName  = L"スターデジオ・楽曲情報";
	pInfo->pszCopyright   = L"各ソースファイルを参照";
	pInfo->pszDescription = L"スターデジオの楽曲情報を表示します。";
	return true;
}

// プラグインの初期化処理
bool CDMInfo::Initialize()
{
	// イベントコールバック関数を登録
	m_pApp->SetEventCallback(EventCallback,this);
	// ストリームコールバック関数を登録
	m_pApp->SetStreamCallback(0, StreamCallback, this);
	return true;
}

// プラグインの終了処理
bool CDMInfo::Finalize()
{
	// ウィンドウの破棄
	if (m_hwnd!=NULL) ::DestroyWindow(m_hwnd);
	return true;
}

//
// メッセージ・ハンドラ
//
INT_PTR CALLBACK CDMInfo::DlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam,void *pClientData)
{
	CDMInfo *pThis=static_cast<CDMInfo*>(pClientData);
	HDC hdc ;
	switch (uMsg) {
	case WM_INITDIALOG:
		// ダイアログ初期化時、背景・文字色を取得
		pThis->m_hwnd=hDlg;
		pThis->m_hbrBack=::CreateSolidBrush(pThis->m_pApp->GetColor(L"PanelBack"));
		pThis->m_crTextColor=pThis->m_pApp->GetColor(L"PanelText");
		return TRUE;
	case WM_TIMER:
		// チャンネル情報更新
		pThis->UpdateChInfo();
		// 経過時間表示更新
		pThis->UpdateTime();
		return TRUE;
	case WM_CTLCOLORSTATIC:
		// 項目の背景色を設定
		hdc=reinterpret_cast<HDC>(wParam);
		::SetBkMode(hdc,TRANSPARENT);
		::SetTextColor(hdc,pThis->m_crTextColor);
		return reinterpret_cast<INT_PTR>(pThis->m_hbrBack);
	case WM_CTLCOLORDLG:
		// ダイアログの背景色を設定
		return reinterpret_cast<INT_PTR>(pThis->m_hbrBack);
	case WM_COMMAND:
		if (LOWORD(wParam)==IDCANCEL) {
			// 閉じる時はプラグインを無効にする
			pThis->m_pApp->EnablePlugin(false);
			return TRUE;
		}
		else return FALSE ; // IDCANCEL以外は未処理
	case WM_DESTROY:
		// ダイアログの終了時、GDIオブジェクトを開放
		::KillTimer(hDlg,1);
		if (pThis->m_hbrBack!=NULL) {
			::DeleteObject(pThis->m_hbrBack);
			pThis->m_hbrBack=NULL;
		}
		return TRUE;
	case WM_USER+1:
		// 楽曲情報取得の通知で表示更新
		pThis->UpdateMusicInfo() ;
		return TRUE;
	default:
		// 未処理のメッセージ
		return FALSE;
	}
}

// イベントコールバック関数
// 何かイベントが起きると呼ばれる
LRESULT CALLBACK CDMInfo::EventCallback(UINT Event,LPARAM lParam1,LPARAM lParam2,void *pClientData)
{
	CDMInfo *pThis=static_cast<CDMInfo*>(pClientData);
	switch (Event) {
	case TVTest::EVENT_PLUGINENABLE:
		// プラグインの有効状態が変化した
		pThis->m_bEnabled=lParam1!=0;
		if (pThis->m_bEnabled) {
			// ダイアログの作成
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
			// チャンネル情報更新
			pThis->UpdateChInfo();
		}
		// ダイアログの表示
		pThis->ShowHide() ;
		// タイマーの起動・停止
		if (pThis->m_bEnabled) ::SetTimer(pThis->m_hwnd,1,100,NULL);
		else ::KillTimer(pThis->m_hwnd,1);
		return TRUE;
	case TVTest::EVENT_COLORCHANGE:
		// 色の設定が変化した
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

// ストリームコールバック関数
// 188バイトのパケットデータが渡される
BOOL CALLBACK CDMInfo::StreamCallback(BYTE *pData, void *pClientData)
{
	CDMInfo *pThis = static_cast<CDMInfo*>(pClientData);
	// TSヘッダーの構造定義
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
	// 異常のあるパケットは無視
	if ( pH->sync_byte != 0x47 ) return FALSE ;
	if ( pH->transport_error_indicator ) return TRUE ;
	if ( pH->transport_scrambling_control ) return TRUE ;
	// 楽曲情報ESのPID以外は無視
	WORD wPid = ( WORD(pH->PID_h) << 8 ) + pH->PID_l ;
	if ( pThis->m_TextDataPid != wPid ) return TRUE;
	// 楽曲情報を取得する
	//   StreamCallback()は非メインスレッドで呼び出されるため
	//   ここからダイアログの更新はできない
	//   情報をm_dlに保存し、メッセージ"WM_USER+1"でメインスレッドに
	//   通知する。
	EnterCriticalSection(&pThis->m_cs) ; // スレッド間排他アクセス
	if ( LibDI_GetInfo( pData, &pThis->m_di ) ) {
		::PostMessage( pThis->m_hwnd, WM_USER+1, 0, 0 ) ;
	}
	LeaveCriticalSection(&pThis->m_cs) ;
	return TRUE ;
}

// 項目の文字列を設定する
void CDMInfo::SetItemText(int ID,LPCTSTR pszText)
{
	TCHAR szCurText[256] = L"" ;
	// 選択が解除されるのとちらつき防止のために、変化があった時のみ設定する
	::GetDlgItemText(m_hwnd,ID,szCurText,256);
	if (::lstrcmp(szCurText,pszText)!=0) ::SetDlgItemText(m_hwnd,ID,pszText);
}

// チャンネル情報を更新する
void CDMInfo::UpdateChInfo()
{
	// チャンネル情報を取得・表示する
	TVTest::ChannelInfo ChannelInfo;
	if (!m_pApp->GetCurrentChannelInfo(&ChannelInfo)) return ;
	TCHAR szText[256];
	::wsprintf(szText,TEXT("ch.%03d"),ChannelInfo.RemoteControlKeyID);
	SetItemText(IDC_CHNUM,szText);
	SetItemText(IDC_CHNAME,ChannelInfo.szChannelName);
	// チャンネルが変わったら、楽曲情報を一旦リセット
	if ( m_LastChNum != ChannelInfo.RemoteControlKeyID ) {
		m_LastChNum = ChannelInfo.RemoteControlKeyID ;
		EnterCriticalSection(&m_cs) ; // スレッド間排他アクセス
		LibDI_Reset() ;
		LeaveCriticalSection(&m_cs) ;
		m_TextDataPid = DWORD(-1) ;
		SetItemText(IDC_TITLE,   L"");
		SetItemText(IDC_ARTIST,  L"");
		SetItemText(IDC_DURATION,L"");
		SetItemText(IDC_ELAPSED, L"");
		m_bCounting = FALSE ;
	}
	// デジオ以外の視聴中はダイアログを非表示
	m_bIsDigio =
		//( ChannelInfo.NetworkID == 1 ) &&
		( ChannelInfo.TransportStreamID == 21 ) &&
		( ChannelInfo.ServiceID >= 400 ) &&
		( ChannelInfo.ServiceID <= 499 ) ;
	ShowHide() ;
	if ( !m_bIsDigio ) return ;
	// 楽曲情報ESのPIDを取得
	int CurService,NumServices;
	TVTest::ServiceInfo ServiceInfo;
	CurService=m_pApp->GetService(&NumServices);
	if (CurService>=0 && 
		m_pApp->GetServiceInfo(CurService,&ServiceInfo)) {
		m_TextDataPid = ServiceInfo.SubtitlePID ;
	}
}

// 楽曲情報の更新
void CDMInfo::UpdateMusicInfo()
{
	EnterCriticalSection(&m_cs) ; // スレッド間排他アクセス
	DI_Info di = m_di ;
	LeaveCriticalSection(&m_cs) ; // スレッド間排他アクセス
	if ( di.bIntermisson ) { // 番組間等、楽曲情報の無い場合
		SetItemText(IDC_TITLE,   L"-------");
		SetItemText(IDC_ARTIST,  L"-------");
		SetItemText(IDC_DURATION,L"-:--:--");
		SetItemText(IDC_ELAPSED, L"-:--:--");
		return ;
	}
	SetItemText(IDC_TITLE,   di.wcsTitle); // 曲名表示
	SetItemText(IDC_ARTIST,  di.wcsArtist); // アーティスト名表示
	DWORD dwTime = di.dwDuration ; // 曲の総時間を表示
	DWORD dwH = dwTime / 3600 ;
	DWORD dwM = ( dwTime % 3600 ) / 60 ;
	DWORD dwS = dwTime % 60  ;
	WCHAR wsz[256] ;
	::wsprintf( wsz, L"%d:%02d:%02d", dwH, dwM, dwS ) ;
	SetItemText(IDC_DURATION,wsz);
	if ( di.dwElapsed == 0 ) { // 曲の先頭ならカウント開始
		m_bCounting = TRUE ;
		m_dStartTick = GetTickCount() ;
		m_dDuration = di.dwDuration * 1000 ;
		m_dDispSec = DWORD(-1) ;
	}
	else if ( !m_bCounting ) { // カウント中でなければ途中からカウント開始
		m_bCounting = TRUE ;
		m_dStartTick = GetTickCount() - di.dwElapsed * 1000 ;
		m_dDuration = di.dwDuration * 1000 ;
		m_dDispSec = DWORD(-1) ;
	}
	UpdateTime() ;
}

// ダイアログの表示・非表示
void CDMInfo::ShowHide()
{
	if ( m_bEnabled && m_bIsDigio ) { // プラグイン許可＆デジオ視聴中なら表示
		if ( !m_bShow ) {
			m_bShow = TRUE ;
			::ShowWindow(m_hwnd,SW_SHOW);
			::SetFocus(m_pApp->GetAppWindow()); // フォーカスをTVTest本体に返す
		}
	}
	else { // そうでないなら非表示
		if ( m_bShow ) {
			m_bShow = FALSE ;
			::ShowWindow(m_hwnd,SW_HIDE);
		}
	}
}

// 経過時間の表示の更新
void CDMInfo::UpdateTime()
{
	if ( !m_bCounting ) return ; // カウント中？
	DWORD dwElapsed = ( GetTickCount() - m_dStartTick ) / 1000 ; // 経過時間
	if ( m_dDispSec == dwElapsed ) return ; // 表示中の時間と同じ
	m_dDispSec = dwElapsed ; // 新しい時間を表示
	DWORD dwTime = dwElapsed ;
	DWORD dwH = dwTime / 3600 ;
	DWORD dwM = ( dwTime % 3600 ) / 60 ;
	DWORD dwS = dwTime % 60  ;
	WCHAR wsz[256] ;
	::wsprintf( wsz, L"%d:%02d:%02d", dwH, dwM, dwS ) ;
	SetItemText(IDC_ELAPSED,wsz);
}

