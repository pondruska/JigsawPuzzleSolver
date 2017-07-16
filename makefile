all:
	g++ -O2 -o ./bin/puzzle ./src/Puzzle.cpp -l armadillo -l Magick++ -l pthread -l boost_thread
