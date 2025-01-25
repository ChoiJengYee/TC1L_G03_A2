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
// Member_1: Function: createTable, insertRow, selectAll, splitString
// Member_2: Function: deleteRow, updateRow, write_to_file, splitString
// Member_3: Function: selectCount, trim, splitString
// Member_4: Function: read_file, get_output_filename
// *********************************************************

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

// Function Prototypes
struct Row; // Structure to represent a single row in a table
struct Table; // Structure to represent the table itself
string trim(const string &str); // Function to trim leading and trailing spaces from a string
void splitString(const string &str, char delimiter, vector<string> &tokens); // Function to split a string into tokens based on a delimiter
void processFile(const string &inputFile, const string &outputFile); // Function to process the input file line by line
void createTable(Table &table, const string &firstLine, ifstream &file); // Function to process the CREATE TABLE command
void insertRow(Table &table, const string &line); // Function to process the INSERT INTO command
void selectAll(const Table &table); // Function to process the SELECT * FROM command
void selectCount(const Table &table); // Function to process the SELECT COUNT(*) command
void updateRow(Table &table, const string &line); // Function to process the UPDATE command
void deleteRow(Table &table, const string &line); // Function to process the DELETE FROM command
void outputToFile(const Table &table, const string &filename); // Function to write the table to an output file


int main()
{
    // Input file name
    string inputFile = "fileInput2.mdb.txt";

    // Output file name to store the results after processing
    string outputFile = "fileOutput2.txt";

    // Process the input file and write the results to the output file
    processFile(inputFile, outputFile);

    return 0;
}

// Structure to represent a single row in the table
struct Row
{
    vector<string> data; // Stores the values for each column in the row
};

// Structure to represent the table
struct Table
{
    string name; // Name of the table
    vector<string> columns; // List of column names for the table
    vector<Row> rows; // Rows in the table
};

// Function to trim leading and trailing spaces from a string
string trim(const string &str)
{
    size_t start = str.find_first_not_of(" \t\n\r"); // Find the first non-space character
    size_t end = str.find_last_not_of(" \t\n\r"); // Find the last non-space character

    // Return the trimmed string or an empty string if no non-space characters exist
    return (start == string::npos) ? "" : str.substr(start, end - start + 1);
}

// Function to split a string into tokens based on a given delimiter
void splitString(const string &str, char delimiter, vector<string> &tokens)
{
    size_t start = 0, end; // Starting position for the search

    // Loop to find and extract tokens separated by the delimiter
    while ((end = str.find(delimiter, start)) != string::npos)
    {
        // Extract and trim the current token, then add it to the tokens vector
        tokens.push_back(trim(str.substr(start, end - start)));
        start = end + 1; // Move the starting position past the delimiter
    }

    // Add the last token after the final delimiter
    tokens.push_back(trim(str.substr(start)));
}

// Function to process the input file containing SQL-like commands
void processFile(const string &inputFile, const string &outputFile)
{
    ifstream file(inputFile); // Open the input file
    Table table; // Create a Table structure to store the data
    string line; // String to store the current line being read

    if (file.is_open()) // Check if the file was opened successfully
    {
        while (getline(file, line)) // Read each line from the file
        {
            line = trim(line); // Remove leading and trailing spaces from the line

            if (line.empty()) continue; // Skip empty lines

            // Check the type of SQL command in the current line
            if (line.find("CREATE TABLE") == 0)
            {
                // Process the CREATE TABLE command
                createTable(table, line, file);
            }
            else if (line.find("INSERT INTO") == 0)
            {
                // Process the INSERT INTO command
                insertRow(table, line);
            }
            else if (line.find("SELECT * FROM") == 0)
            {
                // Process the SELECT * FROM command
                selectAll(table);
            }
            else if (line.find("SELECT COUNT(*) FROM") == 0)
            {
                // Process the SELECT COUNT(*) FROM command
                selectCount(table);
            }
            else if (line.find("DELETE FROM") == 0)
            {
                // Process the DELETE FROM command
                deleteRow(table, line);
            }
            else if (line.find("UPDATE") == 0)
            {
                // Process the UPDATE command
                updateRow(table, line);
            }
        }
        file.close(); // Close the input file

        // Write the processed table to the output file
        outputToFile(table, outputFile);
    }
    else
    {
        // Display an error message if the input file could not be opened
        cout << "File can't be opened" << inputFile << endl;
    }
}

// Function to process the CREATE TABLE command
void createTable(Table &table, const string &firstLine, ifstream &file)
{
    size_t startPos = firstLine.find('('); // Find the opening parenthesis '(' in the command
    string line = firstLine; // Copy the first line of the command
    string columnStr; // String to store the column definitions

    if (startPos != string::npos) // Check if the '(' was found
    {
        // Extract the table name from the command
        table.name = trim(firstLine.substr(13, startPos - 13));

        // Read the column definitions from subsequent lines if necessary
        columnStr = firstLine.substr(startPos + 1); // Start after '('
        while (columnStr.find(')') == string::npos && getline(file, line))
        {
            // Append and trim lines until the closing parenthesis ')' is found
            columnStr += " " + trim(line);
        }

        // Extract everything before the closing parenthesis ')'
        size_t endPos = columnStr.find(')');
        if (endPos != string::npos)
        {
            columnStr = columnStr.substr(0, endPos);
        }
        else
        {
            // Display an error if the closing parenthesis ')' is missing
            cout << "Error: Missing closing parenthesis in CREATE TABLE." << endl;
            return;
        }

        // Split the column definitions into individual column names
        vector<string> columnDefs;
        splitString(columnStr, ',', columnDefs);

        // Check if too many columns were defined
        if (columnDefs.size() > 10)
        {
            cout << "> Error: More than 10 columns in CREATE TABLE." << endl;
            return;
        }

        // Extract column names from the definitions
        for (const auto &col : columnDefs)
        {
            size_t spacePos = col.find(' '); // Find the space between column name and type
            if (spacePos != string::npos)
            {
                // Add the column name to the table
                table.columns.push_back(trim(col.substr(0, spacePos)));
            }
            else
            {
                // Add the column as is if no type is specified
                table.columns.push_back(trim(col));
            }
        }
        // Display a message indicating the table was created
        cout << "> Table " << table.name << " has been created!" << endl;
    }
    else
    {
        // Display an error if the CREATE TABLE syntax is incorrect
        cout << "Error: Something went wrong with CREATE TABLE syntax." << endl;
    }
}

// Function to process the INSERT INTO command
void insertRow(Table &table, const string &line)
{
    size_t valuesPos = line.find("VALUES("); // Find the position of "VALUES(" in the command

    if (valuesPos != string::npos) // Check if "VALUES(" is present in the command
    {
        size_t startPos = valuesPos + 7; // Starting position for the values (after "VALUES(")
        size_t endPos = line.find(")", startPos); // Find the closing parenthesis for the values

        if (endPos == string::npos) // Check if the closing parenthesis is missing
        {
            cout << "Error: Missing closing parenthesis in INSERT statement." << endl;
            return;
        }

        // Extract the values string from the command
        string valuesStr = line.substr(startPos, endPos - startPos);
        vector<string> tokens; // Vector to store the individual values

        // Split the values string into individual tokens
        splitString(valuesStr, ',', tokens);

        // Check if the number of values matches the number of columns in the table
        if (tokens.size() == table.columns.size())
        {
            Row row; // Create a new row to store the values
            for (auto &token : tokens)
            {
                token = trim(token); // Trim leading and trailing spaces

                // Check if the value is a string (enclosed in single quotes)
                if (!token.empty() && token.front() == '\'' && token.back() == '\'')
                {
                    // Remove the single quotes from the value
                    token = token.substr(1, token.size() - 2);
                }

                row.data.push_back(token); // Add the value to the row
            }

            table.rows.push_back(row); // Add the row to the table
            cout << "Row inserted into table " << table.name << endl;
        }
        else
        {
            // Display an error if the column count doesn't match the number of values
            cout << "Error: Column count doesn't match the number of columns in the table!" << endl;
        }
    }
    else
    {
        // Display an error if the INSERT syntax is incorrect
        cout << "Error: Bad INSERT syntax!" << endl;
    }
}

// Function to process the SELECT * FROM command
void selectAll(const Table &table)
{
    // Display the SELECT * FROM command
    cout << "> SELECT * FROM " << table.name << ";" << endl;

    // Print the column names as a header row
    for (size_t i = 0; i < table.columns.size(); ++i)
    {
        cout << table.columns[i];
        if (i < table.columns.size() - 1) cout << ", "; // Add commas between column names
    }
    cout << endl;

    // Print the data rows
    for (const auto &row : table.rows)
    {
        for (size_t i = 0; i < row.data.size(); ++i)
        {
            cout << row.data[i];
            if (i < row.data.size() - 1) cout << ", "; // Add commas between values
        }
        cout << endl;
    }
}

// Function to process the SELECT COUNT(*) FROM command
void selectCount(const Table &table)
{
    // Display the SELECT COUNT(*) FROM command
    cout << "> SELECT COUNT(*) FROM " << table.name << ";" << endl;

    // Display the number of rows in the table
    cout << "Number of rows: " << table.rows.size() << endl;
}

// Function to process the UPDATE command
void updateRow(Table &table, const string &line)
{
    size_t setPos = line.find("SET"); // Find the position of the SET clause
    size_t wherePos = line.find("WHERE"); // Find the position of the WHERE clause

    if (setPos == string::npos || wherePos == string::npos) // Check if the SET or WHERE clause is missing
    {
        cout << "Error: Invalid UPDATE statement." << endl;
        return;
    }

    // Extract the SET and WHERE parts of the command
    string setPart = trim(line.substr(setPos + 3, wherePos - setPos - 3));
    string wherePart = trim(line.substr(wherePos + 5));

    size_t whereEqualPos = wherePart.find("="); // Find the '=' operator in the WHERE clause

    if (whereEqualPos == string::npos) // Check if the WHERE clause syntax is invalid
    {
        cout << "Error: Incorrect syntax in WHERE clause." << endl;
        return;
    }

    // Extract the column name and value from the WHERE clause
    string whereColumn = trim(wherePart.substr(0, whereEqualPos));
    string whereValue = trim(wherePart.substr(whereEqualPos + 1));

    // Remove single quotes from the value if it is a string
    if (whereValue.front() == '\'' && whereValue.back() == '\'')
    {
        whereValue = whereValue.substr(1, whereValue.size() - 2);
    }

    // Iterate through the rows in the table to find and update the matching row
    for (auto &row : table.rows)
    {
        if (row.data[0] == whereValue) // Check if the condition matches
        {
            size_t setEqualPos = setPart.find("="); // Find the '=' operator in the SET clause
            string columnName = trim(setPart.substr(0, setEqualPos)); // Extract the column name to update
            string columnValue = trim(setPart.substr(setEqualPos + 1)); // Extract the new value

            // Update the value in the matching row
            for (size_t i = 0; i < table.columns.size(); ++i)
            {
                if (table.columns[i] == columnName)
                {
                    row.data[i] = columnValue;
                    cout << "Row updated!" << endl;
                    return;
                }
            }
        }
    }

    // Display an error if no matching row was found
    cout << "Error: Row can't be found." << endl;
}

// Function to process the DELETE FROM command
void deleteRow(Table &table, const string &line)
{
    size_t wherePos = line.find("WHERE"); // Find the position of the WHERE clause

    if (wherePos == string::npos) // Check if the WHERE clause is missing
    {
        cout << "Error: WHERE clause is missing." << endl;
        return;
    }

    // Extract the condition part of the WHERE clause
    string wherePart = trim(line.substr(wherePos + 5));
    size_t whereEqualPos = wherePart.find("="); // Find the '=' operator in the WHERE clause

    if (whereEqualPos == string::npos) // Check if the WHERE clause syntax is invalid
    {
        cout << "Error: Incorrect syntax in WHERE clause." << endl;
        return;
    }

    // Extract the column name and value from the WHERE clause
    string whereColumn = trim(wherePart.substr(0, whereEqualPos));
    string whereValue = trim(wherePart.substr(whereEqualPos + 1));

    // Remove single quotes from the value if it is a string
    if (whereValue.front() == '\'' && whereValue.back() == '\'')
    {
        whereValue = whereValue.substr(1, whereValue.size() - 2);
    }

    // Iterate through the rows in the table to find and delete the matching row
    for (auto it = table.rows.begin(); it != table.rows.end(); ++it)
    {
        if (whereColumn == table.columns[0] && it->data[0] == whereValue) // Check if the condition matches
        {
            table.rows.erase(it); // Delete the matching row
            cout << "Row deleted!" << endl;
            return;
        }
    }

    // Display an error if no matching row was found
    cout << "Error: Row can't be found." << endl;
}

// Function to write the table content to an output file
void outputToFile(const Table &table, const string &filename)
{
    ofstream outFile(filename); // Open the output file for writing

    if (outFile.is_open()) // Check if the output file was opened successfully
    {
        // Write the table name to the output file
        outFile << table.name << endl;

        // Write the column names to the output file
        for (const auto &col : table.columns)
        {
            outFile << col << " ";
        }
        outFile << endl;

        // Write the data rows to the output file
        for (const auto &row : table.rows)
        {
            for (size_t i = 0; i < row.data.size(); ++i)
            {
                outFile << row.data[i] << " ";
            }
            outFile << endl;
        }

        outFile.close(); // Close the output file
        cout << "Output written to " << filename << endl; // Display a success message
    }
    else
    {
        // Display an error message if the output file could not be opened
        cout << "Error: Couldn't write to file." << endl;
    }
}

