// *********************************************************
// Program: CCP_Asg.cpp
// Course: CCP6114 Programming Fundamentals
// Lecture Class: TC1L
// Tutorial Class: TT1L
// Trimester: 2430
// Member_1: 242UC244LW | CHOI JENG YEE | CHOI.JENG.YEE@student.mmu.edu.my | 011-21113923
// Member_2: 242UC244KV | KOH HUI WEN | KOH.HUI.WEN@student.mmu.edu.my | 012-9817286
// Member_3: 242UC244M9 | ISABELLE SOO ZI LING | ISABELLE.SOO@student.mmu.edu.my | 012-5086150
// Member_4: 242UC244PB | LEM JOE ERN | LEM.JOE.ERN@student.mmu.edu.my | 016-2237965
// *********************************************************
// Task Distribution
// Member_1: Function: create_table, insert_row, select_all, split_string
// Member_2: Function: delete_row, update_row, read_to_file, split_string
// Member_3: Function: select_count, trim, split_string
// Member_4: Function: read_to_file, output_to_file
// *********************************************************

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

// Function Prototypes
struct Row;
struct Table;
string trim(const string &str);  // Function to remove leading/trailing spaces from a string
void split_string(const string &str, char delimiter, vector<string> &tokens); // Function to split a string by a delimiter
void read_to_file(const string &inputFile, const string &outputFile); // Function to read input and write to output file
void create_table(Table &table, const string &firstLine, ifstream &file); // Function to create a table
void insert_row(Table &table, const string &line); // Function to insert a new row into a table
void select_all(const Table &table); // Function to display all rows in a table
void select_count(const Table &table); // Function to count the number of rows in a table
void update_row(Table &table, const string &line); // Function to update a row based on a condition
void delete_row(Table &table, const string &line); // Function to delete a row based on a condition
void output_to_file(const Table &table, const string &filename); // Function to write table data to a file


int main()
{
    string inputFile = "fileInput2.mdb.txt"; // Input file containing SQL commands
    string outputFile = "fileOutput2.txt"; // Output file to store results
    read_to_file(inputFile, outputFile); // Read from input and write the processed data to output file
    return 0;
}

struct Row
{
    vector<string> data; // Stores data for each row as a vector of strings
};

struct Table
{
    string name;              // Name of the table
    vector<string> columns;   // List of column names
    vector<Row> rows;         // List of rows in the table
};

// Function to read the input file and process SQL commands
void read_to_file(const string &inputFile, const string &outputFile)
{
    ifstream file(inputFile); // Open the input file for reading
    Table table;              // Create an empty table object to store table data
    string line;              // Variable to store each line read from the file

    if (file.is_open()) {     // Check if the file is opened successfully
        while (getline(file, line)) { // Read each line from the file
            line = trim(line);        // Remove leading and trailing spaces from the line
            if (line.empty()) continue; // Skip empty lines

            if (line.find("CREATE TABLE") == 0) {
                create_table(table, line, file); // Call function to create a table
            } else if (line.find("INSERT INTO") == 0) {
                insert_row(table, line); // Call function to insert a row into the table
            } else if (line.find("SELECT * FROM") == 0) {
                select_all(table); // Call function to select and display all rows from the table
            } else if (line.find("SELECT COUNT(*) FROM") == 0) {
                select_count(table); // Call function to count the number of rows in the table
            } else if (line.find("DELETE FROM") == 0) {
                delete_row(table, line); // Call function to delete a row based on the condition
            } else if (line.find("UPDATE") == 0) {
                update_row(table, line); // Call function to update a row based on the condition
            }
        }
        file.close(); // Close the file after processing all lines
        output_to_file(table, outputFile); // Write the processed table data to the output file
    } else {
        cout << "> Error: Unable to open file " << inputFile << endl; // Display error if the file can't be opened
    }
}

// Function to trim leading and trailing spaces from a string
string trim(const string &str)
{
    size_t start = str.find_first_not_of(" \t\n\r"); // Find the first non-whitespace character
    size_t end = str.find_last_not_of(" \t\n\r"); // Find the last non-whitespace character
    return (start == string::npos) ? "" : str.substr(start, end - start + 1); // Return trimmed string
}

// Function to split a string by a delimiter (e.g., comma) and store tokens in a vector
void split_string(const string &str, char delimiter, vector<string> &tokens)
{
    size_t start = 0, end;
    while ((end = str.find(delimiter, start)) != string::npos) { // Find delimiter in the string
        tokens.push_back(trim(str.substr(start, end - start))); // Add substring before delimiter to tokens
        start = end + 1; // Move start position after the delimiter
    }
    tokens.push_back(trim(str.substr(start))); // Add the last substring (after the last delimiter)
}

// Function to create a table by parsing the "CREATE TABLE" SQL command
void create_table(Table &table, const string &firstLine, ifstream &file)
{
    size_t startPos = firstLine.find('('); // Find the position of opening parenthesis for columns
    string line = firstLine;
    string columnStr;

    if (startPos != string::npos) { // If opening parenthesis is found
        table.name = trim(firstLine.substr(13, startPos - 13)); // Extract table name (assuming it's after "CREATE TABLE ")

        columnStr = firstLine.substr(startPos + 1); // Get column definition part after '('
        while (columnStr.find(')') == string::npos && getline(file, line)) {
            columnStr += " " + trim(line); // Concatenate lines until the closing parenthesis is found
        }

        size_t endPos = columnStr.find(')');
        if (endPos != string::npos) {
            columnStr = columnStr.substr(0, endPos); // Extract column definitions before ')'
        } else {
            cout << "> Error: Missing closing parenthesis in CREATE TABLE." << endl;
            return;
        }

        vector<string> columnDefs;
        split_string(columnStr, ',', columnDefs); // Split column definitions by commas

        if (columnDefs.size() > 10) { // Check if the number of columns exceeds the limit of 10
            cout << "> Error: CREATE TABLE cannot have more than 10 columns." << endl;
            exit(EXIT_FAILURE);
        }

        for (const auto &col : columnDefs) {
            size_t spacePos = col.find(' '); // Find space between column name and type
            if (spacePos != string::npos) {
                table.columns.push_back(trim(col.substr(0, spacePos))); // Add column name to columns list
            } else {
                table.columns.push_back(trim(col)); // If no type, just add the column name
            }
        }
        cout << "> Table created: " << table.name << endl; // Confirm table creation
    } else {
        cout << "> Error: Invalid CREATE TABLE syntax." << endl; // Error if '(' is not found
    }
}

// Function to insert a row into the table by parsing the "INSERT INTO" SQL command
void insert_row(Table &table, const string &line)
{
    size_t valuesPos = line.find("VALUES("); // Find the "VALUES" part in the SQL command
    if (valuesPos != string::npos) { // If "VALUES" is found
        size_t startPos = valuesPos + 7; // Start position after "VALUES("
        size_t endPos = line.find(")", startPos); // Find the closing parenthesis of values
        if (endPos == string::npos) {
            cout << "> Error: Missing closing parenthesis in INSERT statement." << endl;
            return;
        }

        string valuesStr = line.substr(startPos, endPos - startPos); // Extract values part
        vector<string> tokens;
        split_string(valuesStr, ',', tokens); // Split the values by commas

        if (tokens.size() == table.columns.size()) { // Check if the number of values matches the number of columns
            Row row;
            for (auto &token : tokens) {
                token = trim(token); // Trim each value
                if (!token.empty() && token.front() == '\'' && token.back() == '\'') {
                    token = token.substr(1, token.size() - 2); // Remove quotes around string values
                }
                row.data.push_back(token); // Add the value to the row
            }
            table.rows.push_back(row); // Add the row to the table
            cout << "> Row inserted into " << table.name << endl;
        } else {
            cout << "> Error: Column count mismatch in INSERT statement." << endl;
        }
    } else {
        cout << "> Error: Invalid INSERT syntax." << endl;
    }
}

// Function to select and display all rows from the table
void select_all(const Table &table)
{
    cout << "> SELECT * FROM " << table.name << ";" << endl; // Display the SELECT statement
    for (size_t i = 0; i < table.columns.size(); ++i) { // Display column names
        cout << table.columns[i];
        if (i < table.columns.size() - 1) cout << ",";
    }
    cout << endl;

    for (const auto &row : table.rows) { // Display each row's data
        for (size_t i = 0; i < row.data.size(); ++i) {
            cout << row.data[i];
            if (i < row.data.size() - 1) cout << ",";
        }
        cout << endl;
    }
}

// Function to count and display the number of rows in the table
void select_count(const Table &table)
{
    cout << "> SELECT COUNT(*) FROM " << table.name << ";" << endl; // Display the COUNT statement
    cout << "Count: " << table.rows.size() << endl; // Display the count of rows
}

// Function to update a row based on the "UPDATE" SQL command
void update_row(Table &table, const string &line)
{
    size_t setPos = line.find("SET"); // Find the "SET" clause in the UPDATE command
    size_t wherePos = line.find("WHERE"); // Find the "WHERE" clause in the UPDATE command

    if (setPos == string::npos || wherePos == string::npos) {
        cout << "> Error: Invalid UPDATE syntax." << endl;
        return;
    }

    string setPart = trim(line.substr(setPos + 3, wherePos - setPos - 3));  // Extract "SET" part
    string wherePart = trim(line.substr(wherePos + 5));  // Extract "WHERE" part

    size_t equalsPos = setPart.find("="); // Find the "=" in the "SET" part
    if (equalsPos == string::npos) {
        cout << "> Error: Invalid SET syntax." << endl;
        return;
    }

    string columnToUpdate = trim(setPart.substr(0, equalsPos)); // Get the column name to update
    string newValue = trim(setPart.substr(equalsPos + 1)); // Get the new value to update

    if (newValue.front() == '\'' && newValue.back() == '\'') {
        newValue = newValue.substr(1, newValue.size() - 2); // Remove quotes from string values
    }

    size_t whereEqualsPos = wherePart.find("="); // Find the "=" in the "WHERE" part
    if (whereEqualsPos == string::npos) {
        cout << "> Error: Invalid WHERE clause." << endl;
        return;
    }

    string whereColumn = trim(wherePart.substr(0, whereEqualsPos)); // Get column name from WHERE clause
    string whereValue = trim(wherePart.substr(whereEqualsPos + 1)); // Get value from WHERE clause

    int whereIntValue = -1;
    if (whereColumn == "customer_id") {
        try {
            whereIntValue = stoi(whereValue);  // Convert whereValue to integer
        } catch (const invalid_argument &e) {
            cout << "> Error: Invalid customer_id value." << endl;
            return;
        }
    }

    for (auto &row : table.rows) {
        if (whereColumn == "customer_id" && stoi(row.data[0]) == whereIntValue) {  // Check customer_id
            for (size_t i = 0; i < table.columns.size(); ++i) { // Find the column to update
                if (table.columns[i] == columnToUpdate) {
                    row.data[i] = newValue; // Update the column value
                    cout << "> Row updated." << endl;
                    return;
                }
            }
            cout << "> Error: Column " << columnToUpdate << " not found." << endl;
            return;
        }
    }

    cout << "> Error: Row with customer_id " << whereIntValue << " not found." << endl; // Error if row is not found
}

// Function to delete a row based on the "DELETE FROM" SQL command
void delete_row(Table &table, const string &line)
{
    size_t wherePos = line.find("WHERE"); // Find the WHERE clause in the DELETE command
    if (wherePos == string::npos) {
        cout << "> Error: WHERE clause missing." << endl;
        return;
    }

    string wherePart = trim(line.substr(wherePos + 5));  // Extract the WHERE part
    size_t whereEqualPos = wherePart.find("="); // Find the equal sign in the WHERE clause
    if (whereEqualPos == string::npos) {
        cout << "> Error: Invalid WHERE syntax." << endl;
        return;
    }

    string whereColumn = trim(wherePart.substr(0, whereEqualPos)); // Get the column name from WHERE
    string whereValue = trim(wherePart.substr(whereEqualPos + 1)); // Get the value from WHERE

    if (whereValue.front() == '\'' && whereValue.back() == '\'') {
        whereValue = whereValue.substr(1, whereValue.size() - 2); // Remove quotes from string values
    }

    int whereIntValue = -1;
    if (whereColumn == "customer_id") {
        try {
            whereIntValue = stoi(whereValue);  // Convert the value to integer
        } catch (const invalid_argument &e) {
            cout << "> Error: Invalid customer_id value." << endl;
            return;
        }
    }

    for (auto it = table.rows.begin(); it != table.rows.end(); ++it) {
        if (whereColumn == "customer_id" && stoi(it->data[0]) == whereIntValue) {  // Check customer_id
            table.rows.erase(it);  // Delete the row
            cout << "> Row deleted." << endl;
            return;
        }
    }

    cout << "> Error: Row not found." << endl; // Display error if no matching row is found
}

// Function to output the table data to the file
void output_to_file(const Table &table, const string &filename)
{
    ofstream file(filename); // Open output file for writing
    if (file.is_open()) { // Check if the file is opened successfully
        file << ">CREATE fileOutput2.txt;" << endl;
        file << ">DATABASES;" << endl;
        file << "C:\\Users\\Gloria\\Documents\\FCI\\fileInput2.mdb.txt" << endl;

        file << ">CREATE TABLE " << table.name << "(" << endl; // Write CREATE TABLE statement
        for (size_t i = 0; i < table.columns.size(); ++i) { // Write column names
            file << table.columns[i];
            if (i < table.columns.size() - 1) file << ",";
            file << endl;
        }
        file << ");" << endl;
        file << ">INSERT INTO " << table.name << " VALUES" << endl;

        for (const auto &row : table.rows) { // Write the data rows
            file << "(";
            for (size_t i = 0; i < row.data.size(); ++i) {
                file << "'" << row.data[i] << "'";
                if (i < row.data.size() - 1) file << ",";
            }
            file << ")" << endl;
        }

        cout << "> Output written to " << filename << endl; // Display success message
        file.close(); // Close the output file
    } else {
        cout << "> Error: Unable to write to file " << filename << endl; // Display error if file can't be opened
    }
}
