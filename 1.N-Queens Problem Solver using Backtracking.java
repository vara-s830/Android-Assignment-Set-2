import java.util.*;


public class NQueens {


    public List<List<String>> solveNQueens(int n) {
        List<List<String>> result = new ArrayList<>();
        char[][] board = new char[n][n];


        // Initialize board with dots
        for (char[] row : board)
            Arrays.fill(row, '.');


        backtrack(0, board, result, n);
        return result;
    }


    private void backtrack(int row, char[][] board, List<List<String>> result, int n) {
        if (row == n) {
            result.add(constructBoard(board));
            return;
        }


        for (int col = 0; col < n; col++) {
            if (isSafe(row, col, board, n)) {
                board[row][col] = 'Q';
                backtrack(row + 1, board, result, n);
                board[row][col] = '.'; // backtrack
            }
        }
    }


    private boolean isSafe(int row, int col, char[][] board, int n) {
        // Check vertical
        for (int i = 0; i < row; i++)
            if (board[i][col] == 'Q') return false;


        // Check left diagonal
        for (int i = row - 1, j = col - 1; i >= 0 && j >= 0; i--, j--)
            if (board[i][j] == 'Q') return false;


        // Check right diagonal
        for (int i = row - 1, j = col + 1; i >= 0 && j < n; i--, j++)
            if (board[i][j] == 'Q') return false;


        return true;
    }


    private List<String> constructBoard(char[][] board) {
        List<String> config = new ArrayList<>();
        for (char[] row : board)
            config.add(new String(row));
        return config;
    }


    public static void main(String[] args) {
        NQueens solver = new NQueens();
        int n = 4;


        List<List<String>> solutions = solver.solveNQueens(n);
        for (List<String> solution : solutions) {
            for (String row : solution) {
                System.out.println(row);
            }
            System.out.println();
        }
    }
}


