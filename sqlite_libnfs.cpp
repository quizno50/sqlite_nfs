#include <iostream>
#include <string.h>
#include <fcntl.h>
#include "sqlite_libnfs.h"

struct sqlite3_vfs sqlite3_nfs;

#define SQLITE3_NFS_FILEOBJ(inPtr) struct sqlite3_nfs_file* nf = \
		(sqlite3_nfs_file*)(inPtr)

void init_sqlite3_nfs(void)
{
	sqlite3_nfs = {
		1, 
		sizeof(struct sqlite3_nfs_file),
		255,
		nullptr,
		"libnfs",
		nullptr,
		sqlite3_nfs_open,
		sqlite3_nfs_delete,
		sqlite3_nfs_access,
		sqlite3_nfs_fullpathname,
		sqlite3_nfs_dlopen,
		sqlite3_nfs_dlerror,
		sqlite3_nfs_dlsym,
		sqlite3_nfs_dlclose,
		sqlite3_nfs_randomness,
		sqlite3_nfs_sleep,
		nullptr, // sqlite3_nfs_currenttime,
		sqlite3_nfs_getlasterror,
		nullptr, // sqlite3_nfs_currentimeint64,
		nullptr, // sqlite3_nfs_setsystemcall,
		nullptr, // sqlite3_nfs_getsystemcall,
		nullptr, // sqlite3_nfs_nextsystemcall
	};
}

int sqlite3_nfs_close(sqlite3_file* f)
{
	SQLITE3_NFS_FILEOBJ(f);
	int ret = SQLITE_OK;

	if (nfs_close(nf->nfs_ctx, nf->nfs_fh) != 0)
	{
		ret = SQLITE_ERROR;
	}

	nfs_destroy_context(nf->nfs_ctx);

	delete nf;
	return ret;
}

int sqlite3_nfs_read(sqlite3_file* f, void* buf, int iAmt, sqlite3_int64 iOfst)
{
	SQLITE3_NFS_FILEOBJ(f);
	int ret = SQLITE_OK;
	int bytes = nfs_pread(nf->nfs_ctx, nf->nfs_fh, iOfst, (uint64_t)iAmt, buf);

	if (bytes > 0 && bytes < iAmt)
	{
		ret = SQLITE_IOERR_SHORT_READ;
		// Zero fill the buffer per sqlite docs.
		::memset((char*)buf + bytes, 0, iAmt - bytes);
	}
	else if (bytes < 0)
	{
		ret = SQLITE_ERROR;
	}
	return ret;
}

int sqlite3_nfs_write(sqlite3_file* f, const void* buf, int iAmt,
		sqlite3_int64 iOfst)
{
	// Not implemented, we're doing read only at this time.
	return SQLITE_ERROR;
}

int sqlite3_nfs_truncate(sqlite3_file* f, sqlite3_int64 size)
{
	// Not implemented, we're doing read only at this time.
	return SQLITE_ERROR;
}

int sqlite3_nfs_sync(sqlite3_file* f, int flags)
{
	// Not implemented, we're doing read only at this time.
	return SQLITE_ERROR;
}

int sqlite3_nfs_filesize(sqlite3_file* f, sqlite3_int64 *pSize)
{
	SQLITE3_NFS_FILEOBJ(f);
	struct nfs_stat_64 s;
	if (nfs_fstat64(nf->nfs_ctx, nf->nfs_fh, &s) != 0)
	{
		return SQLITE_ERROR;
	}
	if (s.nfs_size > INT64_MAX)
	{
		return SQLITE_ERROR;
	}
	*pSize = (sqlite3_int64)s.nfs_size;
	return SQLITE_OK;
}

int sqlite3_nfs_lock(sqlite3_file* f, int flags)
{
	return SQLITE_ERROR;
}

int sqlite3_nfs_unlock(sqlite3_file*, int flags)
{
	return SQLITE_ERROR;
}

int sqlite3_nfs_checkreservedlock(sqlite3_file* f, int* pResOut)
{
	return SQLITE_ERROR;
}

int sqlite3_nfs_filecontrol(sqlite3_file* f, int op, void* pArg)
{
	return SQLITE_ERROR;
}

int sqlite3_nfs_sectorsize(sqlite3_file* f)
{
	SQLITE3_NFS_FILEOBJ(f);
	return nfs_get_readmax(nf->nfs_ctx);
}

int sqlite3_nfs_devicecharacteristics(sqlite3_file* f)
{
	return SQLITE_IOCAP_IMMUTABLE;
}

int sqlite3_nfs_shmmap(sqlite3_file* f, int iPg, int pgsz,
		int, void volatile**)
{
	return SQLITE_ERROR;
}

int sqlite3_nfs_shmlock(sqlite3_file* f, int offset, int n, int flags)
{
	return SQLITE_ERROR;
}
void sqlite3_nfs_shmbarrier(sqlite3_file* f)
{
}
int sqlite3_nfs_shmunmap(sqlite3_file* f, int deleteFlag)
{
	return SQLITE_ERROR;
}
int sqlite3_nfs_fetch(sqlite3_file* f, sqlite3_int64 iOfst, int iAmt,
		void** pp)
{
	return SQLITE_ERROR;
}
int sqlite3_nfs_unfetch(sqlite3_file* f, sqlite3_int64 iOfst, void* p)
{
	return SQLITE_ERROR;
}

int sqlite3_nfs_open(sqlite3_vfs* v, const char* zName, sqlite3_file* f,
		int flags, int *pOutFlags)
{
	struct sqlite3_nfs_file* nf = reinterpret_cast<sqlite3_nfs_file*>(f);
	nf->nfs_ctx = nfs_init_context();
	nf->super_file.pMethods = &sqlite3_nfs_io_methods;
	nf->url = nfs_parse_url_full(nf->nfs_ctx, zName);
	if (nf->url == nullptr)
	{
		return SQLITE_ERROR;
	}
	if (nfs_mount(nf->nfs_ctx, nf->url->server, nf->url->path) != 0)
	{
		std::cerr << "unable to mount: " << nfs_get_error(nf->nfs_ctx)
				<< "\n";
		nfs_destroy_url(nf->url);
		return SQLITE_ERROR;
	}

	*pOutFlags |= SQLITE_OPEN_READONLY;
	if (nfs_open(nf->nfs_ctx, nf->url->file, O_RDONLY | O_SYNC,
			&(nf->nfs_fh)) != 0)
	{
		std::cerr << "unable to open: " << nfs_get_error(nf->nfs_ctx)
				<< "\n";
		nfs_destroy_url(nf->url);
		return SQLITE_ERROR;
	}
	return SQLITE_OK;
}
int sqlite3_nfs_delete(sqlite3_vfs* v, const char* zName, int syncDir)
{
	return SQLITE_ERROR;
}
int sqlite3_nfs_access(sqlite3_vfs* v, const char* zName, int flags,
		int *pResOut)
{
	return SQLITE_ERROR;
}

int sqlite3_nfs_fullpathname(sqlite3_vfs* v, const char* zName, int nOut,
		char* zOut)
{
	strncpy(zOut, zName, nOut);
	return SQLITE_OK;
}

void* sqlite3_nfs_dlopen(sqlite3_vfs* v, const char* zFilename)
{
	return nullptr;
}
void sqlite3_nfs_dlerror(sqlite3_vfs* v, int nByte, char* zErrMsg)
{
}

void (*sqlite3_nfs_dlsym(sqlite3_vfs* v, void*, const char* zSymbol))(void)
{
	return nullptr;
}

void sqlite3_nfs_dlclose(sqlite3_vfs* v, void*)
{
}

int sqlite3_nfs_randomness(sqlite3_vfs* v, int nByte, char* zOut)
{
	return SQLITE_ERROR;
}

int sqlite3_nfs_sleep(sqlite3_vfs* v, int microseconds)
{
	return SQLITE_ERROR;
}

int sqlite3_nfs_currenttime(sqlite3_vfs* v, double*)
{
	return SQLITE_ERROR;
}

int sqlite3_nfs_getlasterror(sqlite3_vfs* v, int l, char* msg)
{
	const char* omsg = "Not Implemented.";
	msg = strncpy(msg, omsg, l);
	return SQLITE_OK;
}

int sqlite3_nfs_currentimeint64(sqlite3_vfs* v, sqlite3_int64*);
int sqlite3_nfs_setsystemcall(sqlite3_vfs* v, const char* zName,
		sqlite3_syscall_ptr);
sqlite3_syscall_ptr sqlite3_nfs_getsystemcall(sqlite3_vfs* v,
		const char* zName);
const char* sqlite3_nfs_nextsystemcall(sqlite3_vfs* v, const char *zName);
