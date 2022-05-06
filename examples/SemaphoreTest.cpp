#include <stdio.h>
#include <unistd.h>
#include "Semaphore.h"

using namespace LightIPC;

bool test0()
{
	::printf("\ntest0 semaphore check\n");
	Result res = Semaphore::Exist("/sem1");
	if (res) {
		::printf("found\n");
	} else {
		::printf("not found: %s\n",res.ErrorMessage().c_str());
	}
	return res;
}

void test1(Semaphore &sem1, Semaphore &sem2)
{
	::printf("\ntest1 \n");
	::printf("sem 1 locked...");

	sem1.Wait();
	printf("done\n");

	::sleep(3);

	::printf("sem 2 locked...");

	sem2.Wait();
	::printf("done\n");

	::sleep(3);

	::printf("sem 1 unlocked...");

	sem1.Post();
	::printf("done\n");

	::sleep(3);

	::printf("sem 2 unlocked...");

	sem2.Post();
	::printf("done\n");
}

void test2(Semaphore &sem1, Semaphore &sem2)
{
	::printf("\ntest2 \n");
	::printf("sem 1 locked...");

	sem1.Wait();
	::printf("done\n");

	::sleep(3);

	::printf("sem 2 locked...");

	sem2.Wait();
	::printf("done\n");

	::sleep(3);

	::printf("sem 2 unlocked...");

	sem2.Post();
	::printf("done\n");

	::sleep(3);

	::printf("sem 1 unlocked...");

	sem1.Post();
	::printf("done\n");
}

int main(int argc, char *argv[]) {
	bool isOwner = (argc > 1);
	if (!isOwner && !test0()) {
		return 0;
	}

	Semaphore sem1("/sem1", isOwner);
	Semaphore sem2("/sem2", isOwner);
	
	test1(sem1, sem2);
	test2(sem1, sem2);

	return 0;
}
