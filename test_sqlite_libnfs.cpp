#include <iostream>
#include "sqlite_libnfs.h"

static int process_rows(void* meh, int argc, char **argv, char** azColName)
{
	for (int i = 0; i < argc; ++i)
	{
		std::cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL")
				<< std::endl;
	}
	return 0;
}

int main(void)
{
	sqlite3* cxn = nullptr;
	
	init_sqlite3_nfs();

	if (sqlite3_vfs_register(&sqlite3_nfs, 1) != 0)
	{
		std::cerr << "Couldn't register Sqlite VFS.\n";
		return -1;
	}

	if (sqlite3_open("nfs://192.168.0.157/opt/weather_data/weather_data.db", &cxn) != SQLITE_OK)
	{
		std::cerr << "Couldn't open database.\n";
	}

	if (sqlite3_exec(cxn, "SELECT * FROM weather_data LIMIT 100;", process_rows,
			0, nullptr) != SQLITE_OK)
	{
		std::cerr << "Query failed. " << sqlite3_errmsg(cxn) << "\n";
	}
	return 0;
}
