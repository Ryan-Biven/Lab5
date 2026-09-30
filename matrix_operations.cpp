#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

// a matrix is just a vector of rows, and each row is a vector of ints
// this nickname saves us from typing the long name every time
using Matrix = std::vector<std::vector<int>>;

// counts how many characters a number takes up when printed
// a minus sign counts as a character too
int numWidth(int v) {
    // use long long so flipping the sign of a huge negative number cant overflow
    long long x = v;
    // every number has at least 1 digit
    int w = 1;
    // negative numbers need 1 extra spot for the minus sign
    if (x < 0) {
        w = 2;
        // make it positive so we can count digits the same way
        x = -x;
    }
    // every time we can divide by 10 there is one more digit
    while (x >= 10) {
        x /= 10;
        ++w;
    }
    return w;
}

// prints a matrix with right-aligned columns of uniform width
void printMatrix(const Matrix& m) {
    // first pass: find the widest number in the whole matrix
    // every column will be as wide as that one so things line up
    int width = 1;
    for (const auto& row : m)
        for (int v : row)
            if (numWidth(v) > width) width = numWidth(v);

    // second pass: actually print, one row per line
    for (const auto& row : m) {
        for (int v : row) {
            // print spaces first so the number ends up pushed to the right
            // the +2 leaves a gap of 2 spaces between columns
            for (int p = numWidth(v); p < width + 2; ++p) std::cout << ' ';
            std::cout << v;
        }
        // end of the row so go to the next line
        std::cout << '\n';
    }
}

//1

// reads n, then two nxn matrices from the file
// returns false if anything goes wrong, true if it all worked
bool loadMatrices(const char* filename, Matrix& a, Matrix& b) {
    // try to open the file
    std::ifstream in(filename);
    if (!in) {
        // couldnt open it, tell the user which file and give up
        std::cerr << "Error: cannot open file '" << filename << "'.\n";
        return false;
    }

    // the first number in the file is the size
    // it has to be a real number and bigger than 0
    int n;
    if (!(in >> n) || n <= 0) {
        std::cerr << "Error: first line must be a positive integer N.\n";
        return false;
    }

    // small helper function that fills one matrix from the file
    // [&] means it can use the variables around it (like in and n)
    auto readOne = [&](Matrix& m) {
        // make the matrix n rows by n columns, all zeros to start
        m.assign(n, std::vector<int>(n));
        // read the numbers in order, row by row
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                // if a read fails the file ran out of numbers (or has junk)
                if (!(in >> m[i][j])) return false;
        return true;
    };

    // read the first matrix, then the second
    // if either one fails we say the file is incomplete
    if (!readOne(a) || !readOne(b)) {
        std::cerr << "Error: file does not contain two complete " << n << "x" << n
                  << " matrices.\n";
        return false;
    }
    return true;
}

//2

// adds two matrices and gives back a new one
// each spot is the sum of the same spot in a and b
Matrix addMatrices(const Matrix& a, const Matrix& b) {
    size_t n = a.size();
    // start with an empty n by n result
    Matrix result(n, std::vector<int>(n));
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            result[i][j] = a[i][j] + b[i][j];
    return result;
}

//3

// multiplies two matrices and gives back a new one
// spot (i, j) in the result = row i of a times column j of b
Matrix multiplyMatrices(const Matrix& a, const Matrix& b) {
    size_t n = a.size();
    // result starts at all zeros because we add into it below
    Matrix result(n, std::vector<int>(n, 0));
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            // k walks along row i of a and down column j of b
            // multiply the pairs and keep adding them up
            for (size_t k = 0; k < n; ++k)
                result[i][j] += a[i][k] * b[k][j];
    return result;
}

//4

// adds up both diagonals and prints the two sums
void diagonalSums(const Matrix& m) {
    size_t n = m.size();
    // long long so big matrices dont overflow the sum
    long long mainSum = 0, secondarySum = 0;
    for (size_t i = 0; i < n; ++i) {
        // main diagonal goes top left to bottom right: (0,0) (1,1) (2,2)
        mainSum += m[i][i];
        // secondary diagonal goes top right to bottom left
        // the column counts down from the last one: n-1, then n-2, then n-3 and so on
        secondarySum += m[i][n - 1 - i];
    }
    std::cout << "Main diagonal sum:      " << mainSum << '\n';
    std::cout << "Secondary diagonal sum: " << secondarySum << '\n';
}

//5

// swaps two rows and prints the result
void swapRows(Matrix& m, int r1, int r2) {
    int n = static_cast<int>(m.size());
    // both row numbers must be between 0 and n-1
    // if not, say so and leave the matrix alone
    if (r1 < 0 || r1 >= n || r2 < 0 || r2 >= n) {
        std::cout << "Invalid row index. Valid range is 0 to " << n - 1 << ".\n";
        return;
    }
    // a row is a whole vector, so we can swap the two vectors in one go
    m[r1].swap(m[r2]);
    printMatrix(m);
}

//6

// swaps two columns and prints the result
void swapColumns(Matrix& m, int c1, int c2) {
    int n = static_cast<int>(m.size());
    // same check as the rows, both columns must exist
    if (c1 < 0 || c1 >= n || c2 < 0 || c2 >= n) {
        std::cout << "Invalid column index. Valid range is 0 to " << n - 1 << ".\n";
        return;
    }
    // a column is spread over every row, so go through each row
    // and swap the two values in that row using a temp variable
    for (auto& row : m) {
        int temp = row[c1];
        row[c1] = row[c2];
        row[c2] = temp;
    }
    printMatrix(m);
}

//7

// puts a new value into one spot and prints the result
void updateElement(Matrix& m, int r, int c, int value) {
    int n = static_cast<int>(m.size());
    // row and column both have to be inside the matrix
    if (r < 0 || r >= n || c < 0 || c >= n) {
        std::cout << "Invalid index. Valid range is 0 to " << n - 1 << ".\n";
        return;
    }
    m[r][c] = value;
    printMatrix(m);
}

// shows a prompt and keeps asking until the user types one whole number on the line
// returns false only if the input ends (nothing left to read)
bool readInt(const char* prompt, int& value) {
    // place to hold the line the user types (up to 255 characters)
    char line[256];
    while (true) {
        std::cout << prompt;
        // read a whole line, this is what fails if input ends or the line is too long
        if (!std::cin.getline(line, sizeof(line))) {
            // input is finished, nothing more to read so stop
            if (std::cin.eof()) return false;
            // otherwise the line was too long
            // clear the error, throw away the rest of the line, then ask again
            std::cin.clear();
            std::cin.ignore(100000, '\n');
            std::cout << "Enter a single whole number\n";
            continue;
        }
        // try to turn the start of the line into a number
        // end will point at the first character that wasnt part of the number
        char* end;
        long v = std::strtol(line, &end, 10);
        // ok means we found at least one digit and it fits in an int
        bool ok = (end != line) && (v == static_cast<int>(v));
        // skip any spaces after the number
        while (*end == ' ' || *end == '\t' || *end == '\r') ++end;
        // if the line is used up after the number then it was just one clean number
        if (ok && *end == '\0') {
            value = static_cast<int>(v);
            return true;
        }
        // something like abc or 2 3 ends up here, so ask again
        std::cout << "Enter a single whole number\n";
    }
}

// asks which matrix to work on and gives back a pointer to it
// returns nullptr if the input ends
Matrix* chooseMatrix(Matrix& a, Matrix& b) {
    int which;
    while (true) {
        if (!readInt("Choose matrix (1 = first, 2 = second): ", which)) return nullptr;
        // hand back the address of the matrix they picked
        // so changes we make later hit the real one and not a copy
        if (which == 1) return &a;
        if (which == 2) return &b;
        std::cout << "Enter 1 or 2\n";
    }
}

// just prints the list of choices
void printMenu() {
    std::cout << "\n===== Matrix Operations =====\n"
              << "1. Show matrices\n"
              << "2. Add matrices\n"
              << "3. Multiply matrices\n"
              << "4. Diagonal sums\n"
              << "5. Swap rows\n"
              << "6. Swap columns\n"
              << "7. Update an element\n"
              << "0. Exit\n";
}

int main() {
    // ask for the file name (up to 255 characters)
    char filename[256];
    std::cout << "Enter input file name: ";
    if (!std::cin.getline(filename, sizeof(filename))) return 1;

    // load both matrices, quit with an error code if that fails
    Matrix a, b;
    if (!loadMatrices(filename, a, b)) return 1;

    // show what we loaded
    std::cout << "\nMatrix A:\n";
    printMatrix(a);
    std::cout << "\nMatrix B:\n";
    printMatrix(b);

    // main menu loop, keeps going until the user picks 0
    // starts at -1 just so the loop runs the first time
    int choice = -1;
    while (choice != 0) {
        printMenu();
        // if the input ends, get out of the loop
        if (!readInt("Choice: ", choice)) break;

        switch (choice) {
            case 0:
                std::cout << "Goodbye\n";
                break;
            case 1:
                // show both matrices again (they may have changed)
                std::cout << "\nMatrix A:\n";
                printMatrix(a);
                std::cout << "\nMatrix B:\n";
                printMatrix(b);
                break;
            case 2:
                std::cout << "\nA + B:\n";
                printMatrix(addMatrices(a, b));
                break;
            case 3:
                std::cout << "\nA x B:\n";
                printMatrix(multiplyMatrices(a, b));
                break;
            case 4:
                // do the diagonals for both matrices, one after the other
                std::cout << "\nMatrix A:\n";
                diagonalSums(a);
                std::cout << "\nMatrix B:\n";
                diagonalSums(b);
                break;
            // 5 and 6 share the same code since they only differ in rows vs columns
            case 5:
            case 6: {
                // figure out which matrix to change
                Matrix* m = chooseMatrix(a, b);
                if (!m) return 0;
                // true for choice 5 (rows), false for choice 6 (columns)
                bool rows = (choice == 5);
                int i1, i2;
                // ask for the two indexes, wording depends on rows or columns
                if (!readInt(rows ? "Enter first row index: " : "Enter first column index: ", i1)) return 0;
                if (!readInt(rows ? "Enter second row index: " : "Enter second column index: ", i2)) return 0;
                // *m means the matrix that the pointer points at
                if (rows) swapRows(*m, i1, i2);
                else swapColumns(*m, i1, i2);
                break;
            }
            case 7: {
                // pick the matrix, then ask for the row, column and new value
                Matrix* m = chooseMatrix(a, b);
                if (!m) return 0;
                int r, c, v;
                if (!readInt("Enter row index: ", r)) return 0;
                if (!readInt("Enter column index: ", c)) return 0;
                if (!readInt("Enter new value: ", v)) return 0;
                updateElement(*m, r, c, v);
                break;
            }
            default:
                // anything that isnt 0 to 7
                std::cout << "Unknown option.\n";
        }
    }
    return 0;
}