#include <stdio.h>
#include "Result.h"

using namespace LightIPC;

void test1()
{
	Result res = Result::CreateSuccess();
	if (res) {
		printf("result is success [%s]\n", res.ErrorMessage().c_str());
	} else {
		printf("result is error [%s]\n", res.ErrorMessage().c_str());
	}

	if (res.IsSuccess()) {
		printf("result is success [%s]\n", res.ErrorMessage().c_str());
	} else {
		printf("result is error [%s]\n", res.ErrorMessage().c_str());
	}
}

void test2()
{
	Result res = Result::CreateError("File Not Found:%s count:%d", "/file/path", 3);
	if (res) {
		printf("result is success [%s]\n", res.ErrorMessage().c_str());
	} else {
		printf("result is error [%s]\n", res.ErrorMessage().c_str());
	}
    
	if (res.IsError()) {
		printf("result is success [%s]\n", res.ErrorMessage().c_str());
	} else {
		printf("result is error [%s]\n", res.ErrorMessage().c_str());
	}
}

int main(int argc, char *argv[]) {
	test1();
	test2();

	return 0;
}
