/*---------------------------------------
 *   header for compatibility
 *   between Windows win32 and Palmos source
 *
 *   BUSSY-SOCRATE REGAN
 *
 *   -----------------------------------*/

MemPtr MemHandleNew(int size){
	return callmem(size);
	}
Handle MemHandleLock(Handle hdl){
	return hdl;
	}
void MemHandleFree(Handle hdl){
	FreeMem(hdl);
	return;
	}
int FrmGetFormPtr(int frm){
		return frm;
		}
void EnableControl(int Frm, int btnstart, boolean true){
	return;
	}
void SrmClose( int gPortId ){
	return;
	}


