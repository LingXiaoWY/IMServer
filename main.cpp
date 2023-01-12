#include"CMySql.h"
#include<iostream>
#include<string>
#include<Windows.h>
#include"CKernel.h"
using namespace std;

int main()
{
	CKernel kernel;
	if (kernel.startServer()) {
		while (true) {
			Sleep(1000);
			cout << "server is running" << endl;
		}
	}
	else {
		cout << "¿ªÆô·þÎñ¶ËÊ§°Ü" << endl;
	}
	system("pause");
	return 0;
}