#pragma once
#include <nfsc/libnfs.h>
#include "sqlite3.h"

int sqlite3_nfs_close(sqlite3_file* f);
int sqlite3_nfs_read(sqlite3_file* f, void* buf, int iAmt, sqlite3_int64 iOfst);
int sqlite3_nfs_write(sqlite3_file* f, const void* buf, int iAmt,
		sqlite3_int64 iOfst);
int sqlite3_nfs_truncate(sqlite3_file* f, sqlite3_int64 size);
int sqlite3_nfs_sync(sqlite3_file* f, int flags);
int sqlite3_nfs_filesize(sqlite3_file* f, sqlite3_int64 *pSize);
int sqlite3_nfs_lock(sqlite3_file* f, int flags);
int sqlite3_nfs_unlock(sqlite3_file*, int flags);
int sqlite3_nfs_checkreservedlock(sqlite3_file* f, int* pResOut);
int sqlite3_nfs_filecontrol(sqlite3_file* f, int op, void* pArg);
int sqlite3_nfs_sectorsize(sqlite3_file* f);
int sqlite3_nfs_devicecharacteristics(sqlite3_file* f);
int sqlite3_nfs_shmmap(sqlite3_file* f, int iPg, int pgsz,
		int, void volatile**);
int sqlite3_nfs_shmlock(sqlite3_file* f, int offset, int n, int flags);
void sqlite3_nfs_shmbarrier(sqlite3_file* f);
int sqlite3_nfs_shmunmap(sqlite3_file* f, int deleteFlag);
int sqlite3_nfs_fetch(sqlite3_file* f, sqlite3_int64 iOfst, int iAmt,
		void** pp);
int sqlite3_nfs_unfetch(sqlite3_file* f, sqlite3_int64 iOfst, void* p);

const struct sqlite3_io_methods sqlite3_nfs_io_methods {
	3,
	sqlite3_nfs_close,
	sqlite3_nfs_read,
	sqlite3_nfs_write,
	sqlite3_nfs_truncate,
	sqlite3_nfs_sync,
	sqlite3_nfs_filesize,
	sqlite3_nfs_lock,
	sqlite3_nfs_unlock,
	sqlite3_nfs_checkreservedlock,
	sqlite3_nfs_filecontrol,
	sqlite3_nfs_sectorsize,
	sqlite3_nfs_devicecharacteristics,
	sqlite3_nfs_shmmap,
	sqlite3_nfs_shmlock,
	sqlite3_nfs_shmbarrier,
	sqlite3_nfs_shmunmap,
	sqlite3_nfs_fetch,
	sqlite3_nfs_unfetch
};

struct sqlite3_nfs_file {
	sqlite3_file super_file;
	nfs_context* nfs_ctx;
	struct nfsfh* nfs_fh;
	nfs_url* url;
};

int sqlite3_nfs_open(sqlite3_vfs* v, const char* zName, sqlite3_file* f,
		int flags, int *pOutFlags);
int sqlite3_nfs_delete(sqlite3_vfs* v, const char* zName, int syncDir);
int sqlite3_nfs_access(sqlite3_vfs* v, const char* zName, int flags,
		int *pResOut);
int sqlite3_nfs_fullpathname(sqlite3_vfs* v, const char* zName, int nOut,
		char* zOut);
void* sqlite3_nfs_dlopen(sqlite3_vfs* v, const char* zFilename);
void sqlite3_nfs_dlerror(sqlite3_vfs* v, int nByte, char* zErrMsg);
void (*sqlite3_nfs_dlsym(sqlite3_vfs* v, void*, const char* zSymbol))(void);
void sqlite3_nfs_dlclose(sqlite3_vfs* v, void*);
int sqlite3_nfs_randomness(sqlite3_vfs* v, int nByte, char* zOut);
int sqlite3_nfs_sleep(sqlite3_vfs* v, int microseconds);
int sqlite3_nfs_currenttime(sqlite3_vfs* v, double*);
int sqlite3_nfs_getlasterror(sqlite3_vfs* v, int, char*);
int sqlite3_nfs_currentimeint64(sqlite3_vfs* v, sqlite3_int64*);
int sqlite3_nfs_setsystemcall(sqlite3_vfs* v, const char* zName,
		sqlite3_syscall_ptr);
sqlite3_syscall_ptr sqlite3_nfs_getsystemcall(sqlite3_vfs* v,
		const char* zName);
const char* sqlite3_nfs_nextsystemcall(sqlite3_vfs* v, const char *zName);

extern struct sqlite3_vfs sqlite3_nfs;

void init_sqlite3_nfs(void);
