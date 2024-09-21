prog: Vector.o
	g++ -std=c++11 -Wall -Wextra -Werror -o prog Vector.o

Vector.o: Vector.cpp Vectors.hpp
	g++ -std=c++11 -Wall -Wextra -Werror -c Vector.cpp -o Vector.o

clean:
	rm prog Vector.o