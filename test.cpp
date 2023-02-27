#include <iostream>

class test {
	private:
		int t;
	public:
		test (int t);

};

test::test (int t)
{
	this->t = t;
}

test fun()
{
	test y (5);

	std::cout << &y << std::endl;
	return (y);
}

int main()
{
	test y = fun();

	std::cout << &y;
}
