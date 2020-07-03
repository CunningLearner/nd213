#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;
enum class State {kEmpty, kObstacle, kClosed, kPath};

vector<State> ParseLine(string line){
    istringstream sline(line);
    int n; char c, ch;
    vector<State> row;
    while(sline >> n >> c >> ch && c == '.' && ch == ','){
        if(!n){
            row.push_back(State::kEmpty);
        }
        else row.push_back(State::kObstacle);
    }
    return row;
}

vector<vector<State>> ReadBoardFile(string path){
    ifstream myfile (path);
    vector<vector<State>> board{};
    if(myfile){
        string line;
        while(getline(myfile,line)){
            vector<State> row = ParseLine(line);
            board.push_back(row);
        }
    }
    return board;
}

string CellString(State cell){
    switch(cell){
        case State::kObstacle: return "⛰️   ";
        case State::kPath: return "🚗   ";
        default: return "0   ";
    }
}

void PrintBoard(const vector<vector<State>>board){
    for(int i = 0; i < board.size(); i++){
        for(int j = 0; j < board[i].size(); j++){
            cout << CellString(board[i][j]);
        }
        cout << endl;
    }
}

bool Compare(vector<int> node_a, vector<int> node_b){
    return ((node_a[3]+node_a[2]) > (node_b[3]+node_b[2]))? true: false;
}

void Cellsort(vector<vector<int>> &list){
    // int last = list.size() - 1;
    sort(list.begin(), list.end(), Compare);
}

int Heuristic(int x1, int y1, int x2, int y2){
    return abs(x2-x1)+abs(y2-y1);
}

void AddToOpen(int x, int y, int g, int h, vector<vector<int>>&openList, vector<vector<State>> &grid){
    vector<int> node = {x, y, g, h};
    openList.push_back(node);
    grid[x][y] = State::kClosed;
}

bool CheckValidCell(int x, int y, vector<vector<State>> &grid) {
    bool on_valid_x = (x >= 0 && x < grid.size());
    bool on_valid_y = (y >= 0 && y < grid[0].size());
    if(on_valid_x && on_valid_y &&grid[x][y] == State::kEmpty){
        return true;
    }
    return false;
}

void ExpandNeighbors(vector<int> current,vector<vector<int>> &open, vector<vector<State>> &grid, int goal[2]){
    int x = current[0], y = current[1], g = current[2];
    if(CheckValidCell(x-1, y, grid)){
        int h = Heuristic(x-1, y, goal[0], goal[1]);
        AddToOpen(x-1, y, g+1, h, open, grid);
    }
    if(CheckValidCell(x+1, y, grid) ){
        int h = Heuristic(x+1, y, goal[0], goal[1]);
        AddToOpen(x+1, y, g+1, h, open, grid);
    }
    if(CheckValidCell(x, y-1, grid) ){
        int h = Heuristic(x, y-1, goal[0], goal[1]);
        AddToOpen(x, y-1, g+1, h, open, grid);
    }
    if(CheckValidCell(x, y+1, grid) ){
        int h = Heuristic(x, y+1, goal[0], goal[1]);
        AddToOpen(x, y+1, g+1, h, open, grid);
    }
}

vector<vector<State>> Search(vector<vector<State>> grid, int start[2], int goal[2]){
    vector<vector<int>> open {};
    int g = 0, h = Heuristic(start[0], start[1], goal[0], goal[1]);
    AddToOpen(start[0], start[1], g, h, open, grid);
    while(open.size()){
        Cellsort(open);
        vector<int> current = open.back();//open[open.size()-1];
        open.pop_back();
        int x = current[0], y = current[1];
        grid[x][y] = State::kPath;
        if(x == goal[0] && y == goal[1])
            return grid;
        ExpandNeighbors(current, open, grid, goal);
    }
    cout << "No path found!" << endl;
    return vector<vector<State>>{};
}

int main()
{
    vector<vector<State>> board = ReadBoardFile("1.board"), solution;
    int start[2] = {0, 0}, goal[2] = {4, 5};
    solution = Search(board, start, goal);
    PrintBoard(solution);
    return 0;
}
