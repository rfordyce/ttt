#include <iostream>
#include <iomanip>
#include <vector>
#include <getopt.h> // for getopt()
#include <cstdlib> // for exit()
#include <thread> // c++11 threading library
#include <math.h> // for sqrt

bool b_seedsmasher = false;
bool b_cleanrotated = false;
bool b_cleansymmetric = false;

int number_of_threads = 1;

using std::cout;
using std::endl;
using std::thread;

// 0 1 2
// 3 4 5
// 6 7 8
class board
{
public:
	board();
	int layout[9];
};

board::board()
{
	for (int i = 0; i < 9; i++) layout[i] = 0; // set all the entries to 0
}

class game {
public:
	game();
	game(const game &g);
	std::vector <board> boards;
	int winner;
};

game::game()
{
	winner = 0; // no winner,1,2,draw
}

game::game(const game &g)
{
	boards = g.boards;
	winner = g.winner;
}

std::vector <game> games; // games vector

bool all_moves_completed = false;

void makeSeeds() // it is assumed that 1 moves first
{
	const game templateGame;
	const board templateBoard;
	for (int i = 0; i < 9; i++) games.push_back(templateGame);
	for (int i = 0; i < 9; i++) games.at(i).boards.push_back(templateBoard);
	for (int i = 0; i < 9; i++) games.at(i).boards.at(0).layout[i] = 1;
}

int whoseturn(const board& b)
{
	int count = 0; // no moves on the board yet
	for (int i = 0; i < 9; i++)
		if (b.layout[i] > 0) count++; // count the moves on the board
	if (count % 2 == 0) return 1; // 2 moves -> 2%2==0 -> turn 1
	return 2; // not turn 1
}

int testWinner(const board &b)
{
	// rows
	switch(b.layout[0]*b.layout[1]*b.layout[2]) {
		case 1: return 1; break;
		case 8: return 2; break;
		default:;
	}
	switch(b.layout[3]*b.layout[4]*b.layout[5]) {
		case 1: return 1; break;
		case 8: return 2; break;
		default:;
	}
	switch(b.layout[6]*b.layout[7]*b.layout[8]) {
		case 1: return 1; break;
		case 8: return 2; break;
		default:;
	}
	// columns
	switch(b.layout[0]*b.layout[3]*b.layout[6]) {
		case 1: return 1; break;
		case 8: return 2; break;
		default:;
	}
	switch(b.layout[1]*b.layout[4]*b.layout[7]) {
		case 1: return 1; break;
		case 8: return 2; break;
		default:;
	}
	switch(b.layout[2]*b.layout[5]*b.layout[8]) {
		case 1: return 1; break;
		case 8: return 2; break;
		default:;
	}
	// diagonals
	switch(b.layout[0]*b.layout[4]*b.layout[8]) {
		case 1: return 1; break;
		case 8: return 2; break;
		default:;
	}
	switch(b.layout[2]*b.layout[4]*b.layout[6]) {
		case 1: return 1; break;
		case 8: return 2; break;
		default:;
	}
	//no win yet!
	for (int i = 0; i < 9; i++)
		if (b.layout[i] < 1) return 0; //game incomplete -> keep going
	return 3; //game is a draw
}

void addGames(game &g)
{
	bool firstgame = true; // if this the first game board it should edit the pointer
	game gtemp(g); // new game (duplicate of g) using constructor
	board b = g.boards.back();
	int turn = whoseturn(b);
	for (int i = 0; i < 9; i++) { // try each location
		if (b.layout[i] < 1) { // no move in location i
			if (firstgame) { // edit the game pointer
				g.boards.push_back(b); // new game board
				g.boards.back().layout[i] = turn; // put the move into the last layout
				g.winner = testWinner(g.boards.back()); // revise the winner
				firstgame = false; // no longer the first game
				all_moves_completed = false; // there may still be more (*games) to do
			} else { // duplicate game to edit
				games.push_back(gtemp); // duplicate game at end of vector
				games.back().boards.push_back(b);
				games.back().boards.back().layout[i] = turn; // put the move into the last layout
				games.back().winner = testWinner(games.back().boards.back()); // revise the winner
			}
		}
	}
}

void printBoard(const board &b)
{
	cout << b.layout[0] << b.layout[1] << b.layout[2] << endl
	     << b.layout[3] << b.layout[4] << b.layout[5] << endl
	     << b.layout[6] << b.layout[7] << b.layout[8] << endl
	     << endl
	;
}

void printGame(const game &g)
{
	for (int i = 0; i < (int) g.boards.size(); i++)
		printBoard(g.boards.at(i));
}

bool testSeeds(const board& b)
{
	if (b.layout[0] == 0)
		if (b.layout[1] == 0)
			if (b.layout[4] == 0)
				return true;
	return false;
}

bool testRotationBoard(const board& b, const board& btest, const int& rotations)
{
	switch(rotations) {
	case 1:
		if (b.layout[0] != btest.layout[6]) return false; // 0 1 2
		if (b.layout[1] != btest.layout[3]) return false; // 3 4 5
		if (b.layout[2] != btest.layout[0]) return false; // 6 7 8
		if (b.layout[3] != btest.layout[7]) return false;
		if (b.layout[4] != btest.layout[4]) return false;
		if (b.layout[5] != btest.layout[1]) return false; // 6 3 0
		if (b.layout[6] != btest.layout[8]) return false; // 7 4 1
		if (b.layout[7] != btest.layout[5]) return false; // 8 5 2
		if (b.layout[8] != btest.layout[2]) return false;
		return true;
		break;
	case 2:
		if (b.layout[0] != btest.layout[8]) return false; // 0 1 2
		if (b.layout[1] != btest.layout[7]) return false; // 3 4 5
		if (b.layout[2] != btest.layout[6]) return false; // 6 7 8
		if (b.layout[3] != btest.layout[5]) return false;
		if (b.layout[4] != btest.layout[4]) return false;
		if (b.layout[5] != btest.layout[3]) return false; // 8 7 6 
		if (b.layout[6] != btest.layout[2]) return false; // 5 4 3
		if (b.layout[7] != btest.layout[1]) return false; // 2 1 0
		if (b.layout[8] != btest.layout[0]) return false;
		return true;
		break;
	case 3:
		if (b.layout[0] != btest.layout[2]) return false; // 0 1 2
		if (b.layout[1] != btest.layout[5]) return false; // 3 4 5
		if (b.layout[2] != btest.layout[8]) return false; // 6 7 8
		if (b.layout[3] != btest.layout[1]) return false;
		if (b.layout[4] != btest.layout[4]) return false;
		if (b.layout[5] != btest.layout[7]) return false; // 2 5 8
		if (b.layout[6] != btest.layout[0]) return false; // 1 4 7
		if (b.layout[7] != btest.layout[3]) return false; // 0 3 6
		if (b.layout[8] != btest.layout[6]) return false;
		return true;
		break;
	default:
		return true; // flag error? - this this should always be 1,2,3
	}
}

bool testRotationGame(const game &g, const game &gtest)
{
	if (g.boards.size() != gtest.boards.size()) return false; //cannot be the same if not the same size
	if (g.winner != gtest.winner) return false; //cannot be the same if different winner
	int counter_layout_entries = 0;

	for (int rotation = 1; rotation <= 3 ; rotation++) { //rotations 1,2,3
		for (int boardindex = 0; boardindex < (int) g.boards.size() ; boardindex++) { //all boards to be rotated by rotation
			if (testRotationBoard(g.boards.at(boardindex), gtest.boards.at(boardindex), rotation))
				counter_layout_entries++;
			else
				boardindex = 99; // exit for()
		}
		if ((int) g.boards.size() == counter_layout_entries) return true; //this rotation is the same
		counter_layout_entries = 0; // reset counter for next rotation
	}
	return false;
}

bool testSymmetric(const board& b, const board& btest, const int &symmetryindex)
{
	switch (symmetryindex) {
	case 1: // about 147
		if (b.layout[0] != btest.layout[2]) return false; // 0 1 2
		if (b.layout[1] != btest.layout[1]) return false; // 3 4 5
		if (b.layout[2] != btest.layout[0]) return false; // 6 7 8
		if (b.layout[3] != btest.layout[5]) return false;
		if (b.layout[4] != btest.layout[4]) return false;
		if (b.layout[5] != btest.layout[3]) return false; // 2 1 0
		if (b.layout[6] != btest.layout[8]) return false; // 5 4 3
		if (b.layout[7] != btest.layout[7]) return false; // 8 7 6
		if (b.layout[8] != btest.layout[6]) return false;
		return true;
		break;
	case 2: // about 048
		if (b.layout[0] != btest.layout[0]) return false; // 0 1 2
		if (b.layout[1] != btest.layout[3]) return false; // 3 4 5
		if (b.layout[2] != btest.layout[6]) return false; // 6 7 8
		if (b.layout[3] != btest.layout[1]) return false;
		if (b.layout[4] != btest.layout[4]) return false;
		if (b.layout[5] != btest.layout[7]) return false; // 0 3 6
		if (b.layout[6] != btest.layout[2]) return false; // 1 4 7
		if (b.layout[7] != btest.layout[5]) return false; // 2 5 8
		if (b.layout[8] != btest.layout[8]) return false;
		return true;
		break;
	case 3: // about 246
		if (b.layout[0] != btest.layout[8]) return false; // 0 1 2
		if (b.layout[1] != btest.layout[5]) return false; // 3 4 5
		if (b.layout[2] != btest.layout[2]) return false; // 6 7 8
		if (b.layout[3] != btest.layout[7]) return false;
		if (b.layout[4] != btest.layout[4]) return false;
		if (b.layout[5] != btest.layout[1]) return false; // 8 5 2
		if (b.layout[6] != btest.layout[6]) return false; // 7 4 1
		if (b.layout[7] != btest.layout[3]) return false; // 6 3 0
		if (b.layout[8] != btest.layout[0]) return false;
		return true;
		break;
	case 4: // about 345
		if (b.layout[0] != btest.layout[6]) return false; // 0 1 2
		if (b.layout[1] != btest.layout[7]) return false; // 3 4 5
		if (b.layout[2] != btest.layout[8]) return false; // 6 7 8
		if (b.layout[3] != btest.layout[3]) return false;
		if (b.layout[4] != btest.layout[4]) return false;
		if (b.layout[5] != btest.layout[5]) return false; // 6 7 8
		if (b.layout[6] != btest.layout[0]) return false; // 3 4 5
		if (b.layout[7] != btest.layout[1]) return false; // 0 1 2
		if (b.layout[8] != btest.layout[2]) return false;
		return true;
		break;
	default:
		return true; // flag error - this this should always be 1,2,3,4
	}
}

bool testSymmetryGame(const game& g, const game& gtest)
{
	if (g.boards.size() != gtest.boards.size()) return false; // cannot be the same if not the same size
	if (g.winner != gtest.winner) return false; // cannot be the same if different winner
	int counter_symmetric_entries = 0;
	for (int symmetryindex = 1; symmetryindex <= 4; symmetryindex++) { //test symmetry 1,2,3,4
		for (int boardindex = 0; boardindex < (int) g.boards.size(); boardindex++) {
			if (testSymmetric(g.boards.at(boardindex),gtest.boards.at(boardindex),symmetryindex))
				counter_symmetric_entries++;
			else
				boardindex = 99; // exit for()
		}
		if ((int) g.boards.size() == counter_symmetric_entries) return true;
		counter_symmetric_entries = 0;
	}
	return false;
}

#include <algorithm> // for std::remove_if
bool toDeleteGame(const game& o) // http://stackoverflow.com/a/7958447/4541045
{
	return (o.winner == 10);
}

void eraseWinner10()
{
	int initial_games = games.size();
	cout << "games marked for deletion, now erasing them" << endl;
	games.erase(
		std::remove_if(games.begin(), games.end(), toDeleteGame),
		games.end()
	);
	cout << games.size() << " games remaining (" << initial_games - games.size() << " erased)" << endl
		<< endl;
}

void testRotationThread(const int start, const int end)
{
	for (int gameindex = end; gameindex >= start ; gameindex--) // from (size -1) to zero
		for (int i = 0; i < gameindex; i++)
			if (testRotationGame(games.at(gameindex),games.at(i))) // test rotation
				games.at(gameindex).winner = 10;
}

void testSymmetryThread(const int start, const int end)
{
	for (int gameindex = end; gameindex >= start ; gameindex--) // from (size -1) to zero
		for (int i = 0; i < gameindex; i++)
			if (testSymmetryGame(games.at(gameindex),games.at(i))) // test rotation
				games.at(gameindex).winner = 10;
}

/* Returns an array containing the upper indicies of N such that the sum of the
 * series from 1 to N is split equally. The sum of the series from 1 to N is
 * known as a triangular number, and may calculated by the following equation
 * sum = (N * (N+1) / 2)
 * N is given by the positive portion of the inverse function
 * N = 0.5 * (sqrt(sum * 8 + 1) - 1)
 *
 * Dividing the sum by the split forms the first index, and successive indicies
 * are formed by multiplying again by the number of the index so each
 * successive sum includes the previous portion.
 * index = 0.5 * (sqrt(sum * 8 / split * position + 1) - 1)
 *
 * Finally
 * index = 0.5 * (sqrt((N * (N+1) / 2) * 8 / split * position) - 1)
 * which simplifies to
 * index = 0.5 * (sqrt( (N * N + N) * 4 / split * position) - 1)
 * note that position is found as i+1 due to being an array index
 */
void splitSummation(const double N, const int split, int ret[])
{
	for (int i = 0; i < split; i++)
		ret[i] = 0.5 * (sqrt((N * N + N)/split * (i+1) * 4 + 1) - 1);
}

void cleanGames() // go backwards through the games and erase rotations and symmetries
{
	cout << "Cleaning games!" << endl;
	thread* threads;
	if (number_of_threads > 1)
		threads = new thread[number_of_threads - 1]; // create threads	

	int thread_terminations[number_of_threads];
	
	if (b_seedsmasher) {
		cout << "cleaning unecessary seeds.." << std::endl;
		for (int gameindex = games.size() - 1 ; gameindex >= 0 ; gameindex--) {
			if (testSeeds(games.at(gameindex).boards.at(0)))
				games.at(gameindex).winner = 10;
		}
		eraseWinner10();
	}
	
	if (b_cleanrotated) {
		cout << "cleaning rotated games.." << endl;
		splitSummation(games.size() - 1,number_of_threads,thread_terminations); // less one from size because it's an array
		for (int i = 0; i < number_of_threads - 1; i++)
			threads[i] = thread(testRotationThread, thread_terminations[i], thread_terminations[i+1]);
		testRotationThread(0, thread_terminations[0]); // main (this) thread
		for (int i = 0; i < number_of_threads - 1; i++)
			threads[i].join();
		eraseWinner10();
	}
	
	if (b_cleansymmetric) {
		cout << "cleaning symmetric games.." << endl;
		splitSummation(games.size() - 1,number_of_threads,thread_terminations); // less one from size because it's an array
		for (int i = 0; i < number_of_threads - 1; i++)
			threads[i] = thread(testSymmetryThread, thread_terminations[i], thread_terminations[i+1]);
		testSymmetryThread(0, thread_terminations[0]); // main (this) thread
		for (int i = 0; i < number_of_threads - 1; i++)
			threads[i].join();
		eraseWinner10();
	}
	// threads* deleted when function exits
}

void printReport()
{
	int qtygames = games.size();
	int qty1won = 0;
	int qty2won = 0;
	int qtydraw = 0;
	for (int i = 0; i < (int) games.size(); i++) {
		switch(games.at(i).winner) {
			case 1: qty1won++;break;
			case 2: qty2won++;break;
			case 3: qtydraw++;break;
			case 10:
				cout << "a duplicate (winner = 10) wasn't deleted" << endl;
			default:
				cout << "found bad value!" << endl;
		}
	}
	cout << endl
	     << "Report" << endl
	     << "Total games  : " << qtygames << endl
	     << "1 Wins games : " << qty1won << endl
	     << "2 Wins games : " << qty2won << endl
	     << "Draw games   : " << qtydraw << endl
	     << endl
	;
}

int main(int argc, char* argv[])
{
	cout << "Program beginning.." << endl;
	int c;
	while ((c = getopt(argc, argv, "rsbft:h")) != -1) {
		switch (c) {
			case 'r':
				b_cleanrotated = true;
				break;
			case 's':
				b_cleansymmetric = true;
				break;
			case 'b': // clean everything!
				b_cleanrotated = true;
				b_cleansymmetric = true;
				break;
			case 'f':
				b_seedsmasher = true;
				break;
			case 't':
				number_of_threads = atoi(optarg);
				if (number_of_threads < 1) number_of_threads = 1;
				cout << "Performing reduction with " << number_of_threads << " threads." << endl;
				break;
			case 'h':
				cout << "tic tac toe halp - please explain program" << endl
				     << "-r\tclean rotated games" << endl
				     << "-s\tclean symmetric games" << endl
				     << "-f\tremove games in rotated seed positions (not starting in first, second, or center)" << endl
				     << "-t\tthreads to use (specify as -t 4 for 4 threads)" << endl
				;
				exit(0); break;
			default:;
		}
	}
	makeSeeds(); // create first 9 games and add the first move
	cout << "Creating games!" << endl;
	while (not(all_moves_completed)) { // if there are still moves to make
		all_moves_completed = true; // so it can be deterined if any more moves exist
		for (int i = 0; i < (int) games.size(); i++) {
			if (games.at(i).winner < 1) { // nobody has won yet
				addGames(games.at(i)); // new games in array incoming!
			}
		}
	}
	cout << "All games created!" << endl;
	printReport();
	if (b_cleanrotated or b_cleansymmetric or b_seedsmasher) {
		cleanGames();
		printReport(); // show final results
	}
	return 0;
}
