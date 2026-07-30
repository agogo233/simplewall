// simplewall
// Copyright (c) 2016-2026 Henry++

#pragma once

#include "routine.h"

#define PR_SIZE_BUFFER_MINIMUM PR_SIZE_BUFFER_OVERFLOW
#define GENERAL_ID 0x6E6574

#pragma warning(disable: 4090 4133 4244 4047 4013 4022 4024 4703)

#if !defined(IN6_IS_ADDR_ULA)
#define IN6_IS_ADDR_ULA(a) \
	(((a)->s6_addr[0] == 0xfd) || \
	 ((a)->s6_addr[0] == 0xfc))
#endif

typedef const R_STRINGREF* PCR_STRINGREF;

#include <winsock2.h>
#include <ws2ipdef.h>
#include <ws2tcpip.h>
#include <windns.h>
#include <mstcpip.h>
#include <iphlpapi.h>
#include <tcpestats.h>
#include <aclapi.h>
#include <dbt.h>
#include <fwpmu.h>
#include <mmsystem.h>
#include <netfw.h>
#include <shlguid.h>
#include <shobjidl.h>
#include <softpub.h>
#include <subauth.h>
#include <mscat.h>

#include "app.h"
#include "rapp.h"
#include "main.h"

#include "resource.h"

DECLSPEC_SELECTANY STATIC_DATA config = {0};
DECLSPEC_SELECTANY PROFILE_DATA profile_info = {0};

DECLSPEC_SELECTANY PR_HASHTABLE apps_table = NULL;
DECLSPEC_SELECTANY PR_LIST rules_list = NULL;
DECLSPEC_SELECTANY PR_HASHTABLE rules_config = NULL;
DECLSPEC_SELECTANY PR_HASHTABLE log_table = NULL;

DECLSPEC_SELECTANY PR_HASHTABLE cache_information = NULL;
DECLSPEC_SELECTANY PR_HASHTABLE cache_resolution = NULL;

DECLSPEC_SELECTANY PR_HASHTABLE colors_table = NULL;

DECLSPEC_SELECTANY PR_ARRAY filter_ids = NULL;

DECLSPEC_SELECTANY R_QUEUED_LOCK lock_apps = PR_QUEUED_LOCK_INIT;
DECLSPEC_SELECTANY R_QUEUED_LOCK lock_apply = PR_QUEUED_LOCK_INIT;
DECLSPEC_SELECTANY R_QUEUED_LOCK lock_rules = PR_QUEUED_LOCK_INIT;
DECLSPEC_SELECTANY R_QUEUED_LOCK lock_rules_config = PR_QUEUED_LOCK_INIT;
DECLSPEC_SELECTANY R_QUEUED_LOCK lock_loglist = PR_QUEUED_LOCK_INIT;
DECLSPEC_SELECTANY R_QUEUED_LOCK lock_notify = PR_QUEUED_LOCK_INIT;
DECLSPEC_SELECTANY R_QUEUED_LOCK lock_profile = PR_QUEUED_LOCK_INIT;
DECLSPEC_SELECTANY R_QUEUED_LOCK lock_transaction = PR_QUEUED_LOCK_INIT;

DECLSPEC_SELECTANY R_QUEUED_LOCK lock_cache_information = PR_QUEUED_LOCK_INIT;
DECLSPEC_SELECTANY R_QUEUED_LOCK lock_cache_resolution = PR_QUEUED_LOCK_INIT;

DECLSPEC_SELECTANY R_WORKQUEUE file_queue = {0};
DECLSPEC_SELECTANY R_WORKQUEUE log_queue = {0};
DECLSPEC_SELECTANY R_WORKQUEUE resolver_queue = {0};
DECLSPEC_SELECTANY R_WORKQUEUE resolve_notify_queue = {0};
DECLSPEC_SELECTANY R_WORKQUEUE wfp_queue = {0};

DECLSPEC_SELECTANY R_FREE_LIST context_free_list = {0};
DECLSPEC_SELECTANY R_FREE_LIST listview_free_list = {0};

// timers array
DECLSPEC_SELECTANY const LONG64 timer_array[] =
{
	2LL * 60LL, // 2 min
	5LL * 60LL, // 5 min
	10LL * 60LL, // 10 min
	30LL * 60LL, // 30 min
	1LL * 3600LL, // 1 hour
	2LL * 3600LL, // 2 hour
	4LL * 3600LL, // 4 hour
	6LL * 3600LL, // 6 hour
	12LL * 3600LL, // 12 hour
	24LL * 3600LL // 24 hour
};

// dropped events callback subscription (win7+)
#if !defined(FWP_DIRECTION_IN)
#define FWP_DIRECTION_IN 0x00003900L
#endif // !FWP_DIRECTION_IN

#if !defined(FWP_DIRECTION_OUT)
#define FWP_DIRECTION_OUT 0x00003901L
#endif // !FWP_DIRECTION_OUT

#if !defined(FWP_DIRECTION_FORWARD)
#define FWP_DIRECTION_FORWARD 0x00003902L
#endif // !FWP_DIRECTION_FORWARD

#if !defined(FWP_DIRECTION_FORWARD2)
#define FWP_DIRECTION_FORWARD2 0x00003903L
#endif // !FWP_DIRECTION_FORWARD2

#define WM_NOTIFICATION (WM_APP + 21)

// Missing routine library functions (compatibility shims)
FORCEINLINE VOID _r_edit_setreadonly (
	_In_ HWND hwnd,
	_In_opt_ INT ctrl_id,
	_In_ BOOLEAN is_readonly
)
{
	SendMessage (GetDlgItem (hwnd, ctrl_id), EM_SETREADONLY, (WPARAM)is_readonly, 0);
}

FORCEINLINE VOID _r_edit_setmargin (
	_In_ HWND hwnd,
	_In_opt_ INT ctrl_id,
	_In_ INT left_margin,
	_In_ INT right_margin
)
{
	SendMessage (GetDlgItem (hwnd, ctrl_id), EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM (left_margin, right_margin));
}

FORCEINLINE VOID _r_wnd_sendcommand (
	_In_ HWND hwnd,
	_In_ INT ctrl_id,
	_In_ WPARAM wparam
)
{
	SendMessage (GetDlgItem (hwnd, ctrl_id), WM_COMMAND, wparam, 0);
}

FORCEINLINE VOID _r_menu_addseparator (
	_In_ HMENU hmenu
)
{
	MENUITEMINFO mii = {0};
	mii.cbSize = sizeof (mii);
	mii.fMask = MIIM_FTYPE;
	mii.fType = MFT_SEPARATOR;
	InsertMenuItem (hmenu, GetMenuItemCount (hmenu), TRUE, &mii);
}

FORCEINLINE VOID _r_button_setcheck (
	_In_ HWND hwnd,
	_In_opt_ INT ctrl_id,
	_In_ BOOLEAN is_checked
)
{
	Button_SetCheck (GetDlgItem (hwnd, ctrl_id), is_checked ? BST_CHECKED : BST_UNCHECKED);
}

FORCEINLINE BOOLEAN _r_button_ischecked (
	_In_ HWND hwnd,
	_In_opt_ INT ctrl_id
)
{
	return (Button_GetCheck (GetDlgItem (hwnd, ctrl_id)) == BST_CHECKED);
}

FORCEINLINE BOOLEAN _r_fs_isexists (
	_In_ PR_STRINGREF path
)
{
	return _r_fs_exists (path);
}

FORCEINLINE BOOLEAN _r_obj_isbyteempty (
	_In_ PR_BYTE string
)
{
	return _r_obj_isbyteempty2 (string);
}

FORCEINLINE BOOLEAN _r_toolbar_getidealsize (
	_In_ HWND hwnd,
	_In_opt_ INT ctrl_id,
	_In_ BOOLEAN is_vertical,
	_Out_ PSIZE out_size
)
{
	SendMessage (GetDlgItem (hwnd, ctrl_id), TB_GETIDEALSIZE, (WPARAM)is_vertical, (LPARAM)out_size);
	return TRUE;
}

FORCEINLINE BOOLEAN _r_rebar_getinfo (
	_In_ HWND hwnd,
	_In_opt_ INT ctrl_id,
	_In_ UINT_PTR band_id,
	_Inout_ PVOID rbi
)
{
	return (BOOLEAN)SendMessage (GetDlgItem (hwnd, ctrl_id), RB_GETBANDINFO, (WPARAM)band_id, (LPARAM)rbi);
}

FORCEINLINE VOID _r_rebar_setinfo (
	_In_ HWND hwnd,
	_In_opt_ INT ctrl_id,
	_In_ UINT_PTR band_id,
	_In_ PVOID rbi
)
{
	SendMessage (GetDlgItem (hwnd, ctrl_id), RB_SETBANDINFO, (WPARAM)band_id, (LPARAM)rbi);
}

#include "controls.h"
#include "db.h"
#include "editor.h"
#include "helper.h"
#include "icons.h"
#include "listview.h"
#include "log.h"
#include "messages.h"
#include "network.h"
#include "notifications.h"
#include "packages.h"
#include "profile.h"
#include "search.h"
#include "security.h"
#include "timer.h"

FORCEINLINE VOID _r_listview_scroll (
	_In_ HWND hwnd,
	_In_opt_ INT ctrl_id,
	_In_ INT scroll_pos
)
{
	SendMessage (GetDlgItem (hwnd, ctrl_id), LVM_GETTOPINDEX, 0, 0);
	SendMessage (GetDlgItem (hwnd, ctrl_id), LVM_ENSUREVISIBLE, scroll_pos, FALSE);
}

FORCEINLINE NTSTATUS _r_sys_terminatethread (
	_In_ HANDLE hthread,
	_In_ NTSTATUS exit_code
)
{
	return NtTerminateThread (hthread, exit_code);
}

FORCEINLINE NTSTATUS _r_fs_createhardlink (
	_In_ PR_STRINGREF link_path,
	_In_ PR_STRINGREF target_path
)
{
	return STATUS_NOT_IMPLEMENTED;
}

FORCEINLINE BOOLEAN _r_button_isradiochecked (
	_In_ HWND hwnd,
	_In_opt_ INT ctrl_id,
	_In_opt_ INT last_ctrl_id
)
{
	UNREFERENCED_PARAMETER (last_ctrl_id);
	return (Button_GetCheck (GetDlgItem (hwnd, ctrl_id)) == BST_CHECKED);
}

FORCEINLINE HWND _r_tooltip_create (
	_In_ HWND hwnd
)
{
	return CreateWindow (TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		hwnd, NULL, NULL, NULL);
}

FORCEINLINE VOID _r_tooltip_settext (
	_In_ HWND hwnd_tooltip,
	_In_ HWND hwnd_parent,
	_In_ INT ctrl_id,
	_In_ LPWSTR text
)
{
	TOOLINFO ti = {sizeof (ti), TTF_IDISHWND | TTF_SUBCLASS, hwnd_parent, (UINT_PTR) GetDlgItem (hwnd_parent, ctrl_id), 0, NULL, text, NULL, NULL};
	SendMessage (hwnd_tooltip, TTM_ADDTOOL, 0, (LPARAM) &ti);
}

FORCEINLINE VOID _r_button_checkradio (
	_In_ HWND hwnd,
	_In_ INT ctrl_id_first,
	_In_ INT ctrl_id_last,
	_In_ INT ctrl_id_check
)
{
	UNREFERENCED_PARAMETER (ctrl_id_last);
	CheckDlgButton (hwnd, ctrl_id_first, BST_UNCHECKED);
	CheckDlgButton (hwnd, ctrl_id_check, BST_CHECKED);
}

FORCEINLINE VOID _r_updown_setacceleration (
	_In_ HWND hwnd,
	_In_ INT ctrl_id,
	_In_ INT step
)
{
	UDACCEL accel = {0, (UINT) step};
	SendMessage (GetDlgItem (hwnd, ctrl_id), UDM_SETACCEL, 1, (LPARAM) &accel);
}

FORCEINLINE BOOLEAN _r_config_invertboolean (
	_In_ LPCWSTR key,
	_In_ BOOLEAN default_value,
	_In_opt_ LPCWSTR section
)
{
	BOOLEAN val = _r_config_getboolean_ex (key, default_value, section);
	_r_config_setboolean_ex (key, !val, section);
	return !val;
}

FORCEINLINE VOID _r_button_seticon (
	_In_ HWND hwnd,
	_In_ INT ctrl_id,
	_In_ HICON hicon
)
{
	SendMessage (GetDlgItem (hwnd, ctrl_id), BM_SETIMAGE, IMAGE_ICON, (LPARAM) hicon);
}

#ifndef BCM_SETTEXTMARGINS
#define BCM_SETTEXTMARGINS (BCM_FIRST + 0x0016)
#endif

FORCEINLINE VOID _r_button_setmargins (
	_In_ HWND hwnd,
	_In_ INT ctrl_id,
	_In_ INT margins
)
{
	SendMessage (GetDlgItem (hwnd, ctrl_id), BCM_SETTEXTMARGINS, 0, MAKELPARAM (margins, margins));
}

FORCEINLINE VOID _r_sys_settimer (
	_In_ HWND hwnd,
	_In_ UINT_PTR event_id,
	_In_ UINT elapse,
	_In_opt_ TIMERPROC timerproc
)
{
	SetTimer (hwnd, event_id, elapse, timerproc);
}

FORCEINLINE INT _r_path_getdrivenumber (
	_In_ PR_STRINGREF path
)
{
	if (path->length >= 2 && path->buffer[1] == L':')
	{
		WCHAR c = path->buffer[0];
		if ((c >= L'A' && c <= L'Z') || (c >= L'a' && c <= L'z'))
			return (c | 0x20) - L'a';
	}
	return INT_ERROR;
}

FORCEINLINE BOOLEAN _r_path_isnetwork (
	_In_ PR_STRINGREF path
)
{
	return (path->length >= 2 && path->buffer[0] == L'\\' && path->buffer[1] == L'\\');
}

#include "uwp.h"
#include "wfp.h"
