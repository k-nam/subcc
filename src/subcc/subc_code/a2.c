struct A
{
	int a;
	int b;
};

int myFunc(int a)
{
	return a;
}

int main()
{

	struct A aa[3][2];
	aa[0][0].a = 11;
	aa[0][1].b = 22;
	aa[1][0].a = 33;
	aa[1][1].b = 44;
	aa[2][0].a = 55;
	aa[2][1].b = 66;
	printInt(aa[0][0].a + aa[1][1].b + aa[2][1].b);
	return 0;
}
