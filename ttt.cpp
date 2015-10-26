#include <iostream>
#include <iomanip>
#include <vector>
#include <getopt.h> //for getopt()
#include <cstdlib> //for exit()

bool b_seedsmasher = false;
bool b_cleanrotated = false;
bool b_cleansymmetric = false;
bool b_cleanboth = false;

using std::cout;
using std::endl;

// 0 1 2
// 3 4 5
// 6 7 8
class board
{
public:
	board();
	int layout[9];
};/**/

board::board()
{
	for (int i = 0; i < 9; i++) layout[i] = 0; //set all the entries to 0
}

class game {
public:
	game();
	game(const game &g);
	std::vector <board> boards;
	int winner;
};/**/

game::game()
{
	winner = 0; // no winner,1,2,draw
}

game::game(const game &g)
{
	boards = g.boards;
	winner = g.winner;
}

const game templateGame;
const board templateBoard;

std::vector <game> gamesvector; //games vector
std::vector <game>* games = &gamesvector; //a pointer to gamesvector

bool all_moves_completed = false;
int games_deleted_rotation = 0; //how many games have been deleted
int games_deleted_symmetry = 0;

void makeSeeds() //it is assumed that 1 moves first
{
	for (int i = 0; i < 9; i++) (*games).push_back(templateGame);
	for (int i = 0; i < 9; i++) (*games).at(i).boards.push_back(templateBoard);
	for (int i = 0; i < 9; i++) (*games).at(i).boards.at(0).layout[i] = 1;
}/**/

int whoseturn(const board& b)
{
	int count = 0; //no moves on the board yet
	for (int i = 0; i < 9; i++)
		if (b.layout[i] > 0) count++; //count the moves on the board
	if (count % 2 == 0) return 1; //2 moves -> 2%2==0 -> turn 1
	return 2; //not turn 1
}/**/

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

void printBoard(const board &b)
{
	cout << b.layout[0] << b.layout[1] << b.layout[2] << endl
	     << b.layout[3] << b.layout[4] << b.layout[5] << endl
	     << b.layout[6] << b.layout[7] << b.layout[8] << endl
	;
}/**/

void printGame(const game &g)
{
	for (int i = 0; i < (int) g.boards.size(); i++)
		printBoard(g.boards.at(i));
}/**/

void rotate2(board &b)
{
	board b2 = b; // ugly
	//b.layout[4] = b.layout[4]; //center value [4] is equal
	b.layout[0] = b2.layout[6]; // 0 1 2
	b.layout[1] = b2.layout[3]; // 3 4 5
	b.layout[2] = b2.layout[0]; // 6 7 8
	b.layout[5] = b2.layout[1];
	b.layout[8] = b2.layout[2];
	b.layout[7] = b2.layout[5];
	b.layout[6] = b2.layout[8];
	b.layout[3] = b2.layout[7];
}/**/

bool testRotationBoard(const board& b, const board& btest, const int& rotations)
{
	//if (b.layout[4] != btest.layout[4]) return false; // test center
	switch(rotations) {
	case 1:
		if (b.layout[0] != btest.layout[6]) return false; // 0 1 2
		if (b.layout[1] != btest.layout[3]) return false; // 3 4 5
		if (b.layout[2] != btest.layout[0]) return false; // 6 7 8
		if (b.layout[3] != btest.layout[7]) return false;
		if (b.layout[5] != btest.layout[1]) return false; // 6 3 0
		if (b.layout[6] != btest.layout[8]) return false; // 7 4 1
		if (b.layout[7] != btest.layout[5]) return false; // 8 5 2
		if (b.layout[8] != btest.layout[6]) return false;
		return true;
		break;
	case 2:
		if (b.layout[0] != btest.layout[8]) return false; // 0 1 2
		if (b.layout[1] != btest.layout[7]) return false; // 3 4 5
		if (b.layout[2] != btest.layout[6]) return false; // 6 7 8
		if (b.layout[3] != btest.layout[5]) return false;
		if (b.layout[4] != btest.layout[4]) return false; // center
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
		if (b.layout[4] != btest.layout[4]) return false; // center
		if (b.layout[5] != btest.layout[7]) return false; // 2 5 8
		if (b.layout[6] != btest.layout[0]) return false; // 1 4 7
		if (b.layout[7] != btest.layout[3]) return false; // 0 3 6
		if (b.layout[8] != btest.layout[6]) return false;
		return true;
		break;
	default:
		// flag error - this this should always be 1,2,3
		return true;
	}
}

bool testRotationGame(const game &g, const game &gtest)
{
	if (g.boards.size() != gtest.boards.size()) return false; //cannot be the same if not the same size
	if (g.winner != gtest.winner) return false; //cannot be the same if different winner
	int counter_layout_entries = 0;
	//board btest;
	for (int rotation = 1; rotation <= 3 ; rotation++) { //rotations 1,2,3
		for (int boardindex = 0; boardindex < (int) g.boards.size() ; boardindex++) { //all boards to be rotated by rotation
			if (testRotationBoard(g.boards.at(boardindex), gtest.boards.at(boardindex), rotation))
				counter_layout_entries++;
			else
				boardindex = 99; // exit for()
		}
		if ((int) g.boards.size() == counter_layout_entries) return true; //this rotation is the same
		counter_layout_entries = 0; //reset counter for next rotation
	}
	return false;
}/**/

bool testSymmetric(const board& b, const board& btest, const int &symmetryindex)
{
	switch (symmetryindex) {
	case 1: //147
		if(b.layout[0] != btest.layout[2]) return false; // 0 1 2
		if(b.layout[1] != btest.layout[1]) return false; // 3 4 5
		if(b.layout[2] != btest.layout[0]) return false; // 6 7 8
		if(b.layout[3] != btest.layout[5]) return false;
		if(b.layout[4] != btest.layout[4]) return false;
		if(b.layout[5] != btest.layout[3]) return false; // 2 1 0
		if(b.layout[6] != btest.layout[8]) return false; // 5 4 3
		if(b.layout[7] != btest.layout[7]) return false; // 8 7 6
		if(b.layout[8] != btest.layout[6]) return false;
		return true;
		break;
	case 2: //048
		if(b.layout[0] != btest.layout[0]) return false; // 0 1 2
		if(b.layout[1] != btest.layout[3]) return false; // 3 4 5
		if(b.layout[2] != btest.layout[6]) return false; // 6 7 8
		if(b.layout[3] != btest.layout[1]) return false;
		if(b.layout[4] != btest.layout[4]) return false;
		if(b.layout[5] != btest.layout[7]) return false; // 0 3 6
		if(b.layout[6] != btest.layout[2]) return false; // 1 4 7
		if(b.layout[7] != btest.layout[5]) return false; // 2 5 8
		if(b.layout[8] != btest.layout[8]) return false;
		return true;
		break;
	case 3: //246
		if(b.layout[0] != btest.layout[8]) return false; // 0 1 2
		if(b.layout[1] != btest.layout[5]) return false; // 3 4 5
		if(b.layout[2] != btest.layout[2]) return false; // 6 7 8
		if(b.layout[3] != btest.layout[7]) return false;
		if(b.layout[4] != btest.layout[4]) return false;
		if(b.layout[5] != btest.layout[1]) return false; // 8 5 2
		if(b.layout[6] != btest.layout[6]) return false; // 7 4 1
		if(b.layout[7] != btest.layout[3]) return false; // 6 3 0
		if(b.layout[8] != btest.layout[0]) return false;
		return true;
		break;
	case 4: //345
		if(b.layout[0] != btest.layout[6]) return false; // 0 1 2
		if(b.layout[1] != btest.layout[7]) return false; // 3 4 5
		if(b.layout[2] != btest.layout[8]) return false; // 6 7 8
		if(b.layout[3] != btest.layout[3]) return false;
		if(b.layout[4] != btest.layout[4]) return false;
		if(b.layout[5] != btest.layout[5]) return false; // 6 7 8
		if(b.layout[6] != btest.layout[0]) return false; // 3 4 5
		if(b.layout[7] != btest.layout[1]) return false; // 0 1 2
		if(b.layout[8] != btest.layout[2]) return false;
		return true;
		break;
	default:
		// flag error - this this should always be 1,2,3,4
		return true;
	}
}

bool testSymmetryGame(const game& g, const game& gtest)
{
	if (g.boards.size() != gtest.boards.size()) return false; //cannot be the same if not the same size
	if (g.winner != gtest.winner) return false; //cannot be the same if different winner
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
}/**/

bool testSymmetryRotationGame(const game& g, const game& gtest)
{
	if (g.boards.size() != gtest.boards.size()) return false; //cannot be the same if not the same size
	if (g.winner != gtest.winner) return false; //cannot be the same if different winner
	int counter_symmetric_entries = 0;

	//game gmodified;
	game gmodified = gtest;
	
	for (int symmetryindex = 1; symmetryindex <= 4; symmetryindex++) { //test symmetry 1,2,3,4
		//gmodified = gtest;

		for (int rotation = 1; rotation <= 3 ; rotation++) { //rotations 1,2,3
			for (int boardindex = 0; boardindex < (int) g.boards.size() ; boardindex++) { //rotate all the boards
				//gmodified.boards.at(boardindex) = rotate(gtest.boards.at(boardindex),rotation);
				rotate2(gmodified.boards.at(boardindex));
			}
			if (testSymmetryGame(g,gmodified)) return true; //just use testSymmetryGame to check it
		} // next rotation
	}
	return false;
}

#include <algorithm>
bool toDeleteGame(const game& o) // http://stackoverflow.com/a/7958447/4541045
{
	return (o.winner == 10);
}

void eraseWinner10()
{
	(*games).erase(
		std::remove_if((*games).begin(), (*games).end(), toDeleteGame),
		(*games).end()
	);
	std::cout << (*games).size() << " games remaining" << std::endl;
}

void cleanGames() //go cackwards through the games and delete rotations and symmetries
{
	cout << "cleaning games" << endl;
	
	if (b_seedsmasher) {
		std::cout << "cleaning unecessary seeds.." << std::endl;
		for (int gameindex = (*games).size() - 1 ; gameindex >= 0 ; gameindex--) {
			if ((*games).at(gameindex).boards.at(0).layout[0] == 0)
				if ((*games).at(gameindex).boards.at(0).layout[1] == 0)
					if ((*games).at(gameindex).boards.at(0).layout[4] == 0)
						(*games).at(gameindex).winner = 10;
		}
		eraseWinner10();
	}
	
	if (b_cleanrotated) {
		cout << "cleaning rotated games.." << endl;
		#pragma omp parallel for
		for (int gameindex = (*games).size() - 1 ; gameindex >= 0 ; gameindex--) { //from (size -1) to zero
			//cout << "processing game " << gameindex << endl;
			for (int i = 0; i < gameindex; i++)
				if (testRotationGame((*games).at(gameindex),(*games).at(i))) //test rotation
					(*games).at(gameindex).winner = 10;
		}
		eraseWinner10();
	}
	
	if (b_cleansymmetric) {
		cout << "cleaning symmetric games.." << endl;
		#pragma omp parallel for
		for (int gameindex = (*games).size() - 1 ; gameindex >= 0 ; gameindex--) { //from (size -1) to zero
			for (int i = 0; i < gameindex; i++)
				if (testSymmetryGame((*games).at(gameindex),(*games).at(i))) //test symmetry
					(*games).at(gameindex).winner = 10;
		}
		eraseWinner10();
	}
	
	if (b_cleanboth) {
		cout << "cleaning rotated, symmetric games.." << endl;
		#pragma omp parallel for
		for (int gameindex = (*games).size() - 1 ; gameindex >= 0 ; gameindex--) { //from (size -1) to zero
			for (int i = 0; i < gameindex; i++)
				if (testSymmetryRotationGame((*games).at(gameindex),(*games).at(i))) //test symmetry
					(*games).at(gameindex).winner = 10;
		}
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
				cout << "a duplicate (winner = 10) wasn't deleted" << endl;
			default:
				cout << "found bad value!" << endl;
		}
	}
	//int qtydraw = qty(*games) - (qty1won + qty2won);
	cout << endl
	     << "Report" << endl
	     << "Total games  : " << qtygames << endl
	     << "1 Wins games : " << qty1won << endl
	     << "2 Wins games : " << qty2won << endl
	     << "Draw games   : " << qtydraw << endl
	     << endl
	;
}/**/

int main(int argc, char* argv[])
{
	int c;
	while ((c = getopt(argc, argv, "rsbfh")) != -1) {
		switch (c) {
			case 'r':
				b_cleanrotated = true;
				break;
			case 's':
				b_cleansymmetric = true;
				break;
			case 'b': //clean everything!
				b_cleanrotated = true;
				b_cleansymmetric = true;
				b_cleanboth = true;
				break;
			case 'f':
				b_seedsmasher = true;
				break;
			case 'h':
				cout << "tic tac toe halp - please explain program" << endl
				     << "-r\tclean rotated games" << endl
				     << "-s\tclean symmetric games" << endl
				     << "-b\tclean rotated, symmetric games" << endl
				     << "-f\tremove games in rotated seed positions (not starting in first, second, or center)" << std::endl
				;
				exit(0); break;
			default:;
		}
	}
	cout << "Program beginning.." << endl;
	makeSeeds(); //create first 9 games and add the first move
	while (not(all_moves_completed)) { //if there are still moves to make
		all_moves_completed = true; //so it can be deterined if any more moves exist
		for (int i = 0; i < (int) (*games).size(); i++) {
			if ((*games).at(i).winner < 1) { //nobody has won yet
				addGames((*games).at(i)); //new games in array incoming!
			}
		}
	}
	cout << "All games created!" << endl;
	printReport();
	if (b_cleanrotated or b_cleansymmetric or b_cleanboth or b_seedsmasher) {
		cleanGames();
		printReport(); //show final results
	}
	return 0;
}/**/
