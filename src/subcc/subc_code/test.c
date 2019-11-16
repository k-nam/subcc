int fibo(int n) {
	if (n == 1 || n == 2) {
		return 1;
	} else {
		return fibo(n - 2) + fibo(n - 1);
	}
}

struct my {
	int array[5];
};
typedef struct my DataType;

DataType insertionSort(DataType input) {
	DataType copy;
	int right;
	int p;
	int store;

	copy = input;	
	right = 1;	
	while (right < 5) {
		p = right;
		store = copy.array[p];
		while (p > 0) {
			if (copy.array[p - 1] <= store) { // insert here
				break;
			} else {
				copy.array[p] = copy.array[p - 1];
				--p; // move to left
			}
		}
		copy.array[p] = store;
		++right;
	}

	return copy;
}

void sortTest() {
	int result[5];
	int index;
	char message1[80];
	char message2[80];
	char space[80];
	DataType data;
	
	message1 = "Before sorting";
	message2 = "After sorting";
	space = "  ";
	data.array[0] = 20;
	data.array[1] = 50;
	data.array[2] = 10;
	data.array[3] = 30;
	data.array[4] = 40;	
	
	printStr(message1);
	printNewLine();
	index = 0;
	while (index < 5) {
		printInt(data.array[index]);
		printStr(space);
		++index;
	}
	printNewLine();
	
	printStr(message2);
	printNewLine();
	data = insertionSort(data);
	index = 0;
	while (index < 5) {
		printInt(data.array[index]);
		printStr(space);
		++index;
	}
}

void loopTest(int size) {
	char star[80];
	int i;
	int j;
	i = 0;
	star = "<>";
	while (i < size) {
		j = 0;
		while (j < i) {
			printStr(star);
			++j;
		}
		printNewLine();
		++i;
	}
}

int main() {
	char helloMessage[80];
	char fiboMessage[80];
	char sortingMessage[80];
	char loopMessage[80];
	char endMessage[80];
	
	
	helloMessage = "1. Hello~ I was compiled by subcc.";
	printStr(helloMessage);
	printNewLine();
	printNewLine();
	
	fiboMessage = "2. Calculated value of fibo(20) >> ";	
	printStr(fiboMessage);	
	printInt(fibo(10));
	printNewLine();
	printNewLine();
	
	sortingMessage = "3. Now testing insertion sort.";
	printStr(sortingMessage);
	printNewLine();
	sortTest();
	printNewLine();
	printNewLine();
	
	loopMessage = "4. Now testing nested loop.";
	printStr(loopMessage);
	loopTest(20);	
	printNewLine();
	printNewLine();

	endMessage = "5. Finished all test.";
	printStr(endMessage);
	printNewLine();
	
	return 0;
}

















