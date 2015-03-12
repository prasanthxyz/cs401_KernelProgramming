#include <fstream>
#include <iostream>
#include <sys/stat.h>

using namespace std;

int main()
{
	char fname[255];
	cin >> fname;
	struct stat var;
	int ret = stat(fname, &var);
	cout << "Inode number: "<< var.st_ino << endl;
	return 0;
}
