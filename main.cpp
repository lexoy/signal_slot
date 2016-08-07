#include <iostream>
#include "signal.h"

using namespace std;

struct UserCase : public Slot
{
	auto func1(float, double)->float;
	auto func2(float, double)->float;
};
struct UserCase_ : public Slot
{
	auto func(float, double)->float;
};

auto UserCase::func1(float op1, double op2)->float
{
	cout << "UserCase::func1 called ~" << endl;
	return .1;
}

auto UserCase::func2(float op1, double op2)->float
{
	cout << "UserCase::func2 called ~" << endl;
	return .1;
}

auto UserCase_::func(float op1, double op2)->float
{
	cout << "UserCase_::func called ~" << endl;
	return .1;
}

float globalfunc1(float op1, double op2)
{
	cout << "globalfunc1 called ~" << endl;
	return .1;
}

float globalfunc2(float op1, double op2)
{
	cout << "globalfunc2 called ~" << endl;
	return .1;
}

struct Sender
{
	Signal<float, float, double> si;
};

int main()
{
	UserCase* uc = new UserCase;
	UserCase_* uc_ = new UserCase_;
	{
		Sender se;
		se.si.connect(uc, &UserCase::func1);
		se.si.connect(uc, &UserCase::func2);
		se.si.connect(uc_, &UserCase_::func);
		se.si.connect(globalfunc1);
		se.si.connect(globalfunc2);

		se.si.disconnect(globalfunc1);
		delete uc_;

		se.si(50, 100);
	}

	return 0;
}