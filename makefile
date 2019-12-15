main.out:
	g++ -std=c++11 main.cpp -lpthread -o main.out
	
clean:
	rm -f *.out
	rm -f *-*
