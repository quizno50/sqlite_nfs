CFLAGS=-Wall -g
CXXFLAGS=-Wall -g -std=c++17
LDFLAGS=-lnfs -pthread -ldl

test_sqlite_libnfs: sqlite_libnfs.o test_sqlite_libnfs.o sqlite3.o
	$(CXX) -o $@ $(LDFLAGS) $^

.PHONY: clean

clean:
	rm -f *.o test_sqlite_libnfs
