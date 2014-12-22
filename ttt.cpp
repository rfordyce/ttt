#include <iostream>
#include <iomanip>
#include <vector>
#include <thread>
#include <getopt.h> //for getopt()
#include <cstdlib> //for exit()

bool b_cleanrotated = false;
bool b_cleansymmetric = false;
bool b_cleanboth = false;

struct board {
	// 0 1 2
	// 3 4 5
	// 6 7 8
public:
	int layout[9];
board() {
	for (int i = 0; i < 9; i++) layout[i] = 0; //set all the entries to 0
}
};/**/

class game { //class or struct?
public:
	std::vector <board> boards;
	int winner;
	game() {
		winner = 0; // no winner,1,2,draw
	}
	game(const game &g) {
		boards = g.boards;
		winner = g.winner;
	}
};/**/

const game templateGame;
const board templateBoard;

std::vector <game> gamesvector; //games vector
std::vector <game>* games = &gamesvector; //a pointer to gamesvector

bool all_moves_completed = false;
int games_deleted_rotation = 0; //how many games have been deleted
int games_deleted_symmetry = 0;

int threadcount = 1;
int badmutex = 0;

void makeSeeds() //it is assumed that 1 moves first
{
	for (int i = 0; i < 9; i++) (*games).push_back(templateGame);
	for (int i = 0; i < 9; i++) (*games).at(i).boards.push_back(templateBoard);
	for (int i = 0; i < 9; i++) (*games).at(i).boards.at(0).layout[i] = 1;
}/**/

int whoseturn(board b)
{
	int count = 0; //no moves on the board yet
	for (int i = 0; i < 9; i++)
		if (b.layout[i] > 0) count++; //count the moves on the board
	if (count % 2 == 0) return 1; //2 moves -> 2%2==0 -> turn 1
	return 2; //not turn 1
}/**/

int testWinner(board b)
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
}/**/

void addGames(game &g)
{
	bool firstgame = true; //if this the first game board it should edit the pointer
	game gtemp(g); //new game (duplicate of g) using constructor
	board b = g.boards.back(); //could be a pointer
	int turn = whoseturn(b);
	for (int i = 0; i < 9; i++) { //try each location
		if (b.layout[i] < 1) { //no move in location i
			if (firstgame) { //edit the game pointer
				g.boards.push_back(b); //new game board
				g.boards.back().layout[i] = turn; //put the move into the last layout
				g.winner = testWinner(g.boards.back()); //revise the winner
				firstgame = false; //no longer the first game
				all_moves_completed = false; //there may still be more (*games) to do
			} else { //duplicate game to edit
				(*games).push_back(gtemp); //duplicate game at end of vector
				(*games).back().boards.push_back(b);
				(*games).back().boards.back().layout[i] = turn; //put the move into the last layout
				(*games).back().winner = testWinner((*games).back().boards.back()); //revise the winner
			}
		}
	}
}/**/

void printBoard(board b)
{
	std::cout << b.layout[0] << b.layout[1] << b.layout[2] << std::endl
	          << b.layout[3] << b.layout[4] << b.layout[5] << std::endl
	          << b.layout[6] << b.layout[7] << b.layout[8] << std::endl
	;
}/**/

void printGame(game g)
{
	for (int i = 0; i < (int) g.boards.size(); i++)
		printBoard(g.boards.at(i));
}/**/

board rotate(board b, int n)
{
	//if (n < 1) return b; //this safeguard is already performed by for loop
	board b1,b2;
	b1 = b;
	for (int i = 0 ; i < n ; i++) { //rotate n times
		b2.layout[4] = b1.layout[4]; //center value [4] is equal
		b2.layout[0] = b1.layout[6]; // 0 1 2
		b2.layout[1] = b1.layout[3]; // 3 4 5
		b2.layout[2] = b1.layout[0]; // 6 7 8
		b2.layout[5] = b1.layout[1];
		b2.layout[8] = b1.layout[2];
		b2.layout[7] = b1.layout[5];
		b2.layout[6] = b1.layout[8];
		b2.layout[3] = b1.layout[7];
		b1 = b2; //for next rotation
	}
	return b1; //b1 = b2
}/**/

bool testRotationBoard(board b, board btest, int rotations)
{
	board btemp = rotate(btest,rotations);
	for (int i = 0 ; i < 9 ;i++) //check every layout entry
		if (b.layout[i] != btemp.layout[i]) return false; //if an entry is not equal, return false
	return true; //all the entries matched, therefore the rotation is the same
}/**/

bool testRotationGame(game g, game gtest)
{
	if (g.boards.size() != gtest.boards.size()) return false; //cannot be the same if not the same size
	if (g.winner != gtest.winner) return false; //cannot be the same if different winner
	int counter_layout_entries = 0;
	//board btest;
	for (int rotation = 1; rotation <= 3 ; rotation++) { //rotations 1,2,3
		for (int boardindex = 0; boardindex < (int) g.boards.size() ; boardindex++) { //all boards to be rotated by rotation
			if (testRotationBoard(g.boards.at(boardindex), gtest.boards.at(boardindex), rotation))
				counter_layout_entries++; //better to kick this out if not true
		}
		if ((int) g.boards.size() == counter_layout_entries) return true; //this rotation is the same
		counter_layout_entries = 0; //reset counter for next rotation
	}
	return false;
}/**/

board symmetrize(board b, int n) //complete
{
	board b2;
	switch (n) {
		case 1: //147
			b2.layout[0] = b.layout[2]; // 0 1 2
			b2.layout[1] = b.layout[1]; // 3 4 5
			b2.layout[2] = b.layout[0]; // 6 7 8
			b2.layout[3] = b.layout[5];
			b2.layout[4] = b.layout[4]; // 2 1 0
			b2.layout[5] = b.layout[3]; // 5 4 3
			b2.layout[6] = b.layout[8]; // 8 7 6
			b2.layout[7] = b.layout[7];
			b2.layout[8] = b.layout[6];
			break;
		case 2: //048
			b2.layout[0] = b.layout[0]; // 0 1 2
			b2.layout[1] = b.layout[3]; // 3 4 5
			b2.layout[2] = b.layout[6]; // 6 7 8
			b2.layout[3] = b.layout[1];
			b2.layout[4] = b.layout[4]; // 0 3 6
			b2.layout[5] = b.layout[7]; // 1 4 7
			b2.layout[6] = b.layout[2]; // 2 5 8
			b2.layout[7] = b.layout[5];
			b2.layout[8] = b.layout[8];
			break;
		case 3: //246
			b2.layout[0] = b.layout[8]; // 0 1 2
			b2.layout[1] = b.layout[5]; // 3 4 5
			b2.layout[2] = b.layout[2]; // 6 7 8
			b2.layout[3] = b.layout[7];
			b2.layout[4] = b.layout[4]; // 8 5 2
			b2.layout[5] = b.layout[1]; // 7 4 1
			b2.layout[6] = b.layout[6]; // 6 3 0
			b2.layout[7] = b.layout[3];
			b2.layout[8] = b.layout[0];
			break;
		case 4: //345
			b2.layout[0] = b.layout[6]; // 0 1 2
			b2.layout[1] = b.layout[7]; // 3 4 5
			b2.layout[2] = b.layout[8]; // 6 7 8
			b2.layout[3] = b.layout[3];
			b2.layout[4] = b.layout[4]; // 6 7 8
			b2.layout[5] = b.layout[5]; // 3 4 5
			b2.layout[6] = b.layout[0]; // 0 1 2
			b2.layout[7] = b.layout[1];
			b2.layout[8] = b.layout[2];
			break;
		default:
			return b; //no available symmetry
	}
	return b2;
}/**/

bool testSymmetric(board b, board btest, int symmetryindex)
{
	board b2;
	b2 = symmetrize(btest,symmetryindex);
	for (int i = 0; i < 9; i++)
		if (b.layout[i] != b2.layout[i]) return false; //one value does not match
	return true;
}/**/

bool testSymmetryGame(game g, game gtest)
{
	if (g.boards.size() != gtest.boards.size()) return false; //cannot be the same if not the same size
	if (g.winner != gtest.winner) return false; //cannot be the same if different winner
	int counter_symmetric_entries = 0;
	for (int symmetryindex = 1; symmetryindex <= 4; symmetryindex++) { //test symmetry 1,2,3,4
		for (int boardindex = 0; boardindex < (int) g.boards.size(); boardindex++) {
			if (testSymmetric(g.boards.at(boardindex),gtest.boards.at(boardindex),symmetryindex))
				counter_symmetric_entries++;
		}
		if ((int) g.boards.size() == counter_symmetric_entries) return true;
		counter_symmetric_entries = 0;
	}
	return false;
}/**/

bool testSymmetryRotationGame(game g, game gtest)
{
	if (g.boards.size() != gtest.boards.size()) return false; //cannot be the same if not the same size
	if (g.winner != gtest.winner) return false; //cannot be the same if different winner
	int counter_symmetric_entries = 0;

	game gmodified;
	
	for (int symmetryindex = 1; symmetryindex <= 4; symmetryindex++) { //test symmetry 1,2,3,4
		gmodified = gtest;

		//modify gmodified to be a rotation
		for (int rotation = 1; rotation <= 3 ; rotation++) { //rotations 1,2,3
			for (int boardindex = 0; boardindex < (int) g.boards.size() ; boardindex++) { //rotate all the boards
				gmodified.boards.at(boardindex) = rotate(gtest.boards.at(boardindex),rotation);
			}
			if (testSymmetryGame(g,gmodified)) return true; //just use testSymmetryGame to check it
		} // next rotation
	}
	return false;
}

void eraseWinner10() //delete all the games that are symmetric or rotations of previous games (winner == 10)
{
	int count = 0;
	for (int i = (*games).size() - 1 ; i >= 0 ; i--) //check from [(size -1)] to [0]
		if ((*games).at(i).winner == 10) {
			(*games).erase((*games).begin() + i);
			count++;
		}
	std::cout << "Removed " << count << " games." << std::endl;
}/**/

void cleanGamesRotated(int gameindex)
{
	for (int i = 0; i < gameindex; i++)
		if (testRotationGame((*games).at(gameindex),(*games).at(i))) //test rotation
			(*games).at(gameindex).winner = 10;
	badmutex--; //thread complete
}

void cleanGamesSymmetric(int gameindex)
{
	for (int i = 0; i < gameindex; i++)
		if (testSymmetryGame((*games).at(gameindex),(*games).at(i))) //test symmetry
			(*games).at(gameindex).winner = 10;
	badmutex--;
}

void cleanGamesBoth(int gameindex)
{
	for (int i = 0; i < gameindex; i++)
		if (testSymmetryRotationGame((*games).at(gameindex),(*games).at(i))) //test symmetry
			(*games).at(gameindex).winner = 10;
	badmutex--;
}

void cleanGames() //go backwards through the games and delete rotations and symmetries
{
	std::cout << "Cleaning games";
	if (threadcount > 1) std::cout << " using " << threadcount << " threads.";
	std::cout << std::endl;

	std::vector<std::thread> threads;

	if (b_cleanrotated) {
		std::cout << "Cleaning rotated games.." << std::endl;
		for (int gameindex = (*games).size() - 1 ; gameindex >= 0 ; gameindex--) { //from (size -1) to zero
			while (badmutex >= threadcount) {} //wait until badmutex has an opening
			badmutex++; //thread count
			threads.push_back(std::thread(cleanGamesRotated, gameindex));
		}
		while (badmutex > 0) {} //wait until all threads complete
		eraseWinner10();
	}
	if (b_cleansymmetric) {
		std::cout << "Cleaning symmetric games.." << std::endl;
		for (int gameindex = (*games).size() - 1 ; gameindex >= 0 ; gameindex--) {
			while (badmutex >= threadcount) {}
			badmutex++;
			threads.push_back(std::thread(cleanGamesSymmetric, gameindex));
		}
		while (badmutex > 0) {}
		eraseWinner10();
	}
	if (b_cleanboth) {
		std::cout << "Cleaning rotated, symmetric games.." << std::endl;
		for (int gameindex = (*games).size() - 1 ; gameindex >= 0 ; gameindex--) {
			while (badmutex >= threadcount) {}
			badmutex++;
			threads.push_back(std::thread(cleanGamesBoth, gameindex));
		}
		while (badmutex > 0) {}
		eraseWinner10();
	}
}/**/

void printReport() //before results have been cleaned
{
	int qtygames = (*games).size();
	int qty1won = 0;
	int qty2won = 0;
	int qtydraw = 0; //optimize later?
	for (int i = 0; i < (int) (*games).size(); i++) {
		switch((*games).at(i).winner) {
			case 1: qty1won++;break;
			case 2: qty2won++;break;
			case 3: qtydraw++;break;
			case 10:
				std::cout << "a duplicate (winner = 10) wasn't deleted" << std::endl;
			default:
				std::cout << "found bad value!" << std::endl;
		}
	}
	//int qtydraw = qty(*games) - (qty1won + qty2won);
	std::cout << std::endl
	          << "Report" << std::endl
	          << "Total games  : " << qtygames << std::endl
	          << "1 Wins games : " << qty1won << std::endl
	          << "2 Wins games : " << qty2won << std::endl
	          << "Draw games   : " << qtydraw << std::endl
	          << std::endl
	;
}/**/

int main(int argc, char* argv[])
{
	int c;
	while ((c = getopt(argc, argv, "rsbt:h")) != -1) {
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
				b_cleanboth = true;
				break;
			case 't': // threads! note that -t=4 causes an error; use -t4
				threadcount = atoi(optarg);
				if (threadcount < 1) {
					std::cout << "invalid thread argument (" << optarg << ") settings threads = 1" << std::endl;
					threadcount = 1;
				}
				break;
			case 'h':
				std::cout << "tic tac toe halp - please explain program" << std::endl
				          << "-r\tclean rotated games" << std::endl
				          << "-s\tclean symmetric games" << std::endl
				          << "-b\tclean rotated, symmetric games" << std::endl
				          << "-t:\tspecify number of threads" << std::endl
				          << std::endl
				          << "usage: ./m_ttt -rsb -t5" << std::endl
				          << "run full program, reduce all games with each method, reduce with 5 threads" << std::endl
				;
				exit(0); break;
			default:;
		}
	}
	std::cout << "Program beginning.." << std::endl;
	makeSeeds(); //create first 9 games and add the first move
	while (not(all_moves_completed)) { //if there are still moves to make
		all_moves_completed = true; //so it can be deterined if any more moves exist
		for (int i = 0; i < (int) (*games).size(); i++) {
			if ((*games).at(i).winner < 1) { //nobody has won yet
				addGames((*games).at(i)); //new games in array incoming!
			}
		}
	}
	std::cout << "All games created!" << std::endl;
	printReport();
	if (b_cleanrotated or b_cleansymmetric or b_cleanboth) {
		cleanGames();
		printReport(); //show final results
	}
	return 0;
}/**/
