CFLAGS=-Wall -g
CXXFLAGS=-Wall -g -std=c++17
LDFLAGS=-lnfs -pthread -ldl

test_sqlite_libnfs: sqlite_libnfs.o test_sqlite_libnfs.o sqlite3.o
	$(CXX) -o $@ $(LDFLAGS) $^

sqlite3.c:
	curl -O https://sqlite.org/2021/sqlite-amalgamation-3360000.zip
	unzip -j sqlite-amalgamation-3360000.zip sqlite-amalgamation-3360000/sqlite3.c sqlite-amalgamation-3360000/sqlite3.h

.PHONY: clean

clean:
	rm -f *.o test_sqlite_libnfs sqlite-amalgamation-*.zip sqlite3.c sqlite3.h
