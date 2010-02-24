#pragma once

enum MenuIDEnum
{
	MENU_ID_START = wxID_HIGHEST + 1,

	IDM_ABOUT = MENU_ID_START,
	IDM_PREFERENCE,
	IDM_VIEW_AS_HTML,
	IDM_VIEW_AS_TEXT,
	IDM_NEXT_FILE,
	IDM_PREV_FILE,
	IDM_BOOKMARKS,
	IDM_ADD_BOOKMARK,
	IDM_CLEAR_BOOKMARKS, 
	IDM_HIDE,
	IDM_FULL_SCREEN,
	IDM_VIEW_DISPLAY_ORIGINAL,
	IDM_VIEW_DISPLAY_SIMPLIFY,
	IDM_VIEW_DISPLAY_TRADITIONAL,
	IDM_GOTO,

	IDM_ENCODE_UNKNOWN,
	IDM_ENCODE_ID_START,
	IDM_ENCODE_GB = IDM_ENCODE_ID_START,
	IDM_ENCODE_CHARSET,
	IDM_ENCODE_BIG5,
	IDM_ENCODE_UTF7,
	IDM_ENCODE_UTF8,
	IDM_ENCODE_WINDOWS_1252,
	IDM_ENCODE_UNICODE,
	IDM_ENCODE_UNICODE_BE,
	IDM_ENCODE_UNICODE_32,
	IDM_ENCODE_UNICODE_32_BE,
	IDM_ENCODE_ID_END = IDM_ENCODE_UNICODE_32_BE,

	IDM_RECENT_FILE,
	IDM_RECENT_FILE_END = IDM_RECENT_FILE + 20,

	IDM_LANG_ID_START,
	IDM_LANG_CHINESE = IDM_LANG_ID_START,
	IDM_LANG_JAPANESE,
	IDM_LANG_KOREAN,
	IDM_LANG_ID_END = IDM_LANG_KOREAN
};
