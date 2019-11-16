typedef int t1;
t1 g1;
struct s1 {
	struct s2 {
		int a;
	};
  int a;
  int b;
};

typedef struct s1 s1Type;

struct s1 func3 (int a) {
  struct s1 ss1;
  ss1.a = 5;
  ss1.b = 6;
  return ss1;
}
int func1 (int p1, int p2) {
  typedef int tt1;
  tt1 l1;
  tt1 l100;
  l1 = 5;
  g1 = ~(-l1);
  if (g1) {
    l1 = sizeof (l100);
		return l1;
  }
  if (p1 > p2) {
    while (p1 < 100) {
      int temp;
      typedef int t2;
      struct myst {
        int a1;
        int a2;
      };
      typedef struct myst myst1;
      tt1 temp1;
      t1 aa1;
      myst1 mystruct;
      p2 = ++p1;
      p2 *= p1;
      g1 = l1 + p1;
      temp1 = 111;
      printNewLine();
      printInt(temp1);

      if (p1 == p2)
        continue;
      else
        break;
    }
    return p1;
  } else {
     switch(p1) {
       case 1:
          return p2;
       case 2:
          return 3;
          break;
			 case 3:
				 return 5;
				 break;
       default:
          return p1 + p2;
     }
	} 
}
int func2 (int a, int b, char d) {
  return a + b + d;
}
int fibo (char n) {
  if (n < 2)
    return 1;
  return fibo(n-2) + fibo(n-1);
}

int myFunc (int c[3], s1Type ss1[2]) {
  int a;
  int b;
  if (1) {
    int a;
    a = 5;
  }
  a = ss1[1].a;
  b = ss1[1].b;
  return a + b + ss1[0].a + ss1[0].b + c[0] + c[1] * c[2];
}


int main(int argc, char argv[3]) {
  int c;
  int i;
  int j;
	typedef int intType;
  intType d[3];
  int sum1;
  int sum2;
  s1Type ss1[2];
	struct s2 ss2;
	struct bb {
		int a;
	} tempStruct;
	int aaa[3][3];

	ss2.a = 5;
  d[0] = 1;
	d[1] = 2;
	if (2 > 1 && 3 > 2) {
		d[2] = 4;
	} else {
		d[2] = 6;
	}
  ss1[1].a = 100;
	if (0) {
		ss1[1].b = 11;
	} else {
		ss1[1].b = 21;
	}
	ss1[0].a = 1;
	ss1[0].b = 2;
	switch (2) {
	case 2:
		{
			printInt(5);
			printNewLine();
			break;
		}
	case 3:
		{
			printInt(myFunc(d, ss1));  // supposed to be 133
			printNewLine();
			break;
		}
	default:
		{
			printInt(6);
		}
	}
	printInt(myFunc(d, ss1));  // supposed to be 133
	printNewLine();
  return 1;
}

