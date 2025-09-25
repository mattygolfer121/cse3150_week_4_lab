#include <iostream>
#include <vector>
#include <stack>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <random>
using namespace std;

void write_board_csv(const vector<vector<int>>& board, bool first) {
    ios_base::openmode mode = ios::app;
    if (first) mode = ios::trunc;
    ofstream fout("game_output.csv", mode);
    if (!fout) return;
    for (int r=0;r<4;r++){
        for (int c=0;c<4;c++){
            fout<<board[r][c];
            if (!(r==3 && c==3)) fout<<",";
        }
    }
    fout<<"\n";
}

void print_board(const vector<vector<int>>& board, bool first) {
    // TODO: implement print_board here
	for (const auto& row : board){
		for (auto cell : row){
			std::cout << "[" << cell << "] ";
		}
		std::cout << std::endl;
	}
	write_board_csv(board, first);
}

// TODO: use algorithms to spawn properly
void spawn_tile(vector<vector<int>>& board) {
    vector<pair<int,int>> empty;
    for (int r=0; r<4; r++) {
        for (int c=0; c<4; c++) {
            if (board[r][c] == 0) {
                // collect all empty spots
                empty.push_back({r, c});
            }
        }
    }

    if (empty.empty()) return;  // board is full, nothing to do

    // --- Use C++11 random machinery ---
    static std::random_device rd;   // seed source
    static std::mt19937 gen(rd());  // random engine (Mersenne Twister)

    // pick a random empty position
    std::uniform_int_distribution<> pos_dist(0, empty.size() - 1);
    auto pos = empty[pos_dist(gen)];

    // 90% chance of 2, 10% chance of 4
    std::uniform_int_distribution<> val_dist(1, 10);
    board[pos.first][pos.second] = (val_dist(gen) == 1) ? 4 : 2;
    // TODO: Feed this into chat GPT and have it correct the function for you
    // with proper prompting
}

	
// TODO: Compress a row, remove zeroes, and then pad with zeroes at the end
std::vector<int> compress_row(const std::vector<int>& row) {
	vector<int> newrow;
    	copy_if(row.begin(), row.end(), back_inserter(newrow), [](int x){ return x != 0; });
	while (newrow.size() < 4) {
        	newrow.push_back(0);
    	}	
	return newrow;
}
// TODO: Merge a row (assumes the row is already compressed)
std::vector<int> merge_row(std::vector<int> row) {
	for (int i=0 ; i<3; i++) {
        	if (row[i] != 0 && row[i] == row[i+1]) {
            		row[i] = row[i] * 2;
            		row[i+1] = 0;
        	}
    	}
    	row = compress_row(row);
    	return row;
}
// TODO: use copy_if and iterators
bool move_left(vector<vector<int>>& board){
	vector<int> newrow;
	int yeah_nah = 0;
	for (auto& row : board){
		newrow = merge_row(compress_row(row));
		if (newrow != row){
			row = newrow;
			yeah_nah = 1;
		}
	}
	if (yeah_nah == 1){
		return true;
	} else {
		return false;
	}
}
// TODO: use reverse iterators
bool move_right(vector<vector<int>>& board){
	int yeah_nah = 0;
	for (auto& row : board){
		vector<int> reversed(row.rbegin(), row.rend());
		reversed = merge_row(compress_row(reversed));
		reversed = vector<int>(reversed.rbegin(), reversed.rend());
		if (reversed != row){
			row = reversed;
			yeah_nah = 1;
		}
	}

	if (yeah_nah == 1){
		return true;
	} else {
		return false;
	}
}
// TODO: use column traversal
bool move_up(vector<vector<int>>& board){
	int yeah_nah = 0;
	for (int i = 0; i < 4; i++){
		vector<int> column;
		for (int j = 0; j < 4; j++){
			column.push_back(board[j][i]);
		}	
		vector<int> newcol = merge_row(compress_row(column));
		if (newcol != column){
			for (int j = 0; j < 4; j++){
				board[j][i] = newcol[j];
			}
			yeah_nah = 1;
		}
	}
	if (yeah_nah == 1){
		return true;
	}else{
		return false;
	}
}
// TODO: use column traversal with reverse
bool move_down(vector<vector<int>>& board){
	int yeah_nah = 0;
	for (int i = 0; i < 4; i++){
                vector<int> column;
                for (int j = 3; j >= 0; j--){
                        column.push_back(board[j][i]);
                }    
		
                vector<int> newcol = merge_row(compress_row(column));
                if (newcol != column){
                        for (int j = 3, idx = 0; j >= 0; j--, idx++){
                                board[j][i] = newcol[idx];
                        }   
                        yeah_nah = 1;
                }   
        }   


	if (yeah_nah == 1){
		return true;
	}else{
		return false;
	}
}

int main(){
    srand(time(nullptr));
    vector<vector<int>> board(4, vector<int>(4,0));
    spawn_tile(board);
    spawn_tile(board);

    stack<vector<vector<int>>> history;
    bool first=true;

    while(true){
        print_board(board, first);
        first=false;
        cout<<"Move (w=up, a=left, s=down, d=right), u=undo, q=quit: ";
        char cmd;
        if (!(cin>>cmd)) break;
        if (cmd=='q') break;

        if (cmd=='u') {
            // TODO: get the history and print the board and continue
        	if(!history.empty()){
			board = history.top();
			history.pop();
		}
		continue;
	}

        vector<vector<int>> prev = board;
        bool moved=false;
        if (cmd=='a') moved=move_left(board);
        else if (cmd=='d') moved=move_right(board);
        else if (cmd=='w') moved=move_up(board);
        else if (cmd=='s') moved=move_down(board);

        if (moved) {
            // TODO: Store the previous state here!
		history.push(prev);
		spawn_tile(board);
        }
    }
    return 0;
}
