#ifndef __READBOOK_KEYS_H__
#define __READBOOK_KEYS_H__

#define READBOOK_KEY_CODE(alt,ctrl,shift,key) ((((alt & 0x01)<<18) & 0x00040000) | (((ctrl & 0x01)<<17) & 0x00020000) | (((shift & 0x01) << 16) & 0x00010000) | ((key) & 0xFFFF))

enum ReadBookKeysEnum
{
	ReadBook_Key_Ctrl = READBOOK_KEY_CODE(0,1,0,0),
	ReadBOOK_Key_CTRL_D = READBOOK_KEY_CODE(0,1,0,'D'),
	ReadBOOK_Key_CTRL_G = READBOOK_KEY_CODE(0,1,0,'G'),
};

#endif