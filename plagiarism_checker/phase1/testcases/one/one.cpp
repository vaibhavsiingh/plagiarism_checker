class Solution {
public:
    bool findEmpty(vector<vector<char>>& board, int &x, int &y){
        // for finding empty cell in the board
        for(int i = 0; i<9; i++){
            for(int j = 0; j<9; j++){
                if(board[i][j] == '.'){
                    x = i;
                    y = j;
                    return true;
                }
            }
        }
        return false;
    }


    bool checkRow(vector<vector<char>>& board, int x , int elem){
        char ch = '0' + elem;
        // checking the row
        for(int i = 0; i<9; i++){
            if(board[x][i] == ch){
                return false;
            }
        }
        return true;
    }

    bool checkColumn(vector<vector<char>>& board, int y , int elem){
        char ch = '0' + elem;
        // checking the column
        for(int i = 0; i<9; i++){
            if(board[i][y] == ch){
                return false;
            }
        }
        return true;
    }

    bool checkBox(vector<vector<char>>& board, int x, int y , int elem){
        int box_x = x/3;
        int box_y = y/3;
        char ch = '0' + elem;
        // checking the box
        for(int i = 0; i<3; i++){
            for(int j =0; j<3; j++){
                if(board[3*box_x + i][3*box_y + j] == ch){
                    return false;
                }
            }
        }
        return true;
    }




    bool checkValid(vector<vector<char>>& board, int x, int y, int elem){
        if(checkRow(board, x, elem) && checkColumn(board, y, elem) && 
            checkBox(board, x,y, elem)){
                return true;
        }
        return false;
    }


    bool helper(vector<vector<char>>& board){
        int x = -1;
        int y = -1;
        bool empty = findEmpty(board, x,y);
        if(!empty){
            // the sudoku has been solved
            return true;
        }

        for(int i = 1; i<=9; i++){
            if(checkValid(board, x , y , i)){
                // after checking we place the i
                board[x][y] = ('0'+i);
                if(helper(board)){
                    // we were able to solve the entire sudoku
                    return true;
                }
                // could not solve the entire sudoku so backtrack
                board[x][y] = '.';
            }
            
        }
        return false;
    }
    void solveSudoku(vector<vector<char>>& board) {
        helper(board);
    }
};