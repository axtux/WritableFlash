// WritableFlash - mount a writable dev_flash
// by @jjolano

#include <io/pad.h>
#include <io/msg.h>

#include <sysutil/events.h>

#include <lv2/process.h>

#include <psl1ght/lv2.h>
#include <psl1ght/lv2/filesystem.h>

#include "rsxutil.h"
#include "filesystem_mount.h"

const char* MOUNT_POINT = "/dev_rwflash"; // the directory to mount writable dev_flash

int currentBuffer = 0;
msgButton dlg_action;

void handledialog(msgButton button, void *userdata)
{
	dlg_action = button;
}

void handleevent(u64 status, u64 param, void * userdata)
{
	if(status == EVENT_REQUEST_EXITAPP)
	{
		sysProcessExit(0);
	}
}

void showmessage(msgType type, const char* message)
{
	msgDialogOpen(type, message, handledialog, 0, NULL);
	
	dlg_action = 0;
	while(!dlg_action)
	{
		sysCheckCallback();
		
		flip(currentBuffer);
		waitFlip();
		currentBuffer = !currentBuffer;
	}
	
	msgDialogClose();
}

int main(int argc, const char* argv[])
{
	msgType mdialogok	= MSGDIALOG_NORMAL | MSGDIALOG_BUTTON_TYPE_OK;
	msgType mdialogyesno	= MSGDIALOG_NORMAL | MSGDIALOG_BUTTON_TYPE_YESNO;
	
	sysRegisterCallback(EVENT_SLOT0, handleevent, NULL);
	
	init_screen();
	ioPadInit(7);
	
	waitFlip();

	Lv2FsStat entry;
	int is_mounted = lv2FsStat(MOUNT_POINT, &entry);
	
	showmessage(mdialogyesno, (is_mounted == 0) ? "Do you want to unmount dev_rwflash ?" : "Do you want to mount dev_rwflash ?");
	
	if(dlg_action == MSGDIALOG_BUTTON_YES)
	{
		if(is_mounted == 0)
			showmessage(mdialogok, (lv2FsUnmount(MOUNT_POINT) == 0) ? "Successfully unmounted dev_rwflash." : "An error occured while unmounting dev_rwflash.");
		else
			showmessage(mdialogok, (lv2FsMount(DEV_FLASH1, FS_FAT32, MOUNT_POINT, 0) == 0) ? "Successfully mounted dev_rwflash." : "An error occured while mounting dev_rwflash.");
	}
	
	return 0;
}

