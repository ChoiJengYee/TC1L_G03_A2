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
struct Row;
struct Table;
string trim(const string &str);
void createTable(Table &table, const string &firstLine, ifstream &file);
void insertRow(Table &table, const string &line);
void selectAll(const Table &table);
void selectCount(const Table &table);
void outputToFile(const Table &table, const string &filename);
void processFile(const string &inputFile, const string &outputFile);
void splitString(const string &str, char delimiter, vector<string> &tokens);
void deleteRow(Table &table, const string &line);
void updateRow(Table &table, const string &line);

int main() {
    string inputFile = "fileInput2.mdb";
    string outputFile = "fileOutput2.txt";
    processFile(inputFile, outputFile);
    return 0;
}

struct Row {
    vector<string> data; // Row data as a vector of strings
};

struct Table {
    string name;              // Table name
    vector<string> columns;   // Column names
    vector<Row> rows;         // Rows in the table
};

void processFile(const string &inputFile, const string &outputFile) {
    ifstream file(inputFile);
    Table table;
    string line;

    if (file.is_open()) {
        while (getline(file, line)) { // Read each line
            line = trim(line);        // Remove leading/trailing spaces
            if (line.empty()) continue; // Skip blank lines
            if (line.find("CREATE TABLE") == 0) {
                createTable(table, line, file);
            } else if (line.find("INSERT INTO") == 0) {
                insertRow(table, line);
            } else if (line.find("SELECT * FROM") == 0) {
                selectAll(table);
            } else if (line.find("SELECT COUNT(*) FROM") == 0) {
                selectCount(table);
            } else if (line.find("DELETE FROM") == 0) {
                deleteRow(table, line);
            } else if (line.find("UPDATE") == 0) {  // Detect UPDATE statement
                updateRow(table, line);
            }
        }
        file.close();
        outputToFile(table, outputFile); // Save results to the output file
    } else {
        cout << "> Error: Unable to open file " << inputFile << endl;
    }
}

string trim(const string &str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    return (start == string::npos) ? "" : str.substr(start, end - start + 1);
}

void splitString(const string &str, char delimiter, vector<string> &tokens) {
    size_t start = 0, end;
    while ((end = str.find(delimiter, start)) != string::npos) {
        tokens.push_back(trim(str.substr(start, end - start)));
        start = end + 1;
    }
    tokens.push_back(trim(str.substr(start))); // Add the last token
}

void createTable(Table &table, const string &firstLine, ifstream &file) {
    size_t startPos = firstLine.find('(');
    string line = firstLine;
    string columnStr;

    if (startPos != string::npos) {
        table.name = trim(firstLine.substr(13, startPos - 13)); // Extract table name

        // Extract the part after '(' and append subsequent lines until ')' is found
        columnStr = firstLine.substr(startPos + 1);
        while (columnStr.find(')') == string::npos && getline(file, line)) {
            columnStr += " " + trim(line); // Concatenate lines, handling spaces
        }

        // Extract everything before the closing ')'
        size_t endPos = columnStr.find(')');
        if (endPos != string::npos) {
            columnStr = columnStr.substr(0, endPos);
        } else {
            cout << "> Error: Missing closing parenthesis in CREATE TABLE." << endl;
            return;
        }

        // Split by comma and handle spaces in columns
        vector<string> columnDefs;
        splitString(columnStr, ',', columnDefs);

        // Check for a maximum of 10 columns
        if (columnDefs.size() > 10) {
            cout << "> Error: CREATE TABLE cannot have more than 10 columns." << endl;
            exit(EXIT_FAILURE);
        }

        for (const auto &col : columnDefs) {
            size_t spacePos = col.find(' '); // Find space between column name and type
            if (spacePos != string::npos) {
                table.columns.push_back(trim(col.substr(0, spacePos))); // Extract column name
            } else {
                table.columns.push_back(trim(col)); // If no type is provided, just take the column name
            }
        }
        cout << "> Table created: " << table.name << endl;
    } else {
        cout << "> Error: Invalid CREATE TABLE syntax." << endl;
    }
}

void insertRow(Table &table, const string &line) {
    size_t valuesPos = line.find("VALUES(");
    if (valuesPos != string::npos) {
        size_t startPos = valuesPos + 7;
        size_t endPos = line.find(")", startPos);
        if (endPos == string::npos) {
            cout << "> Error: Missing closing parenthesis in INSERT statement." << endl;
            return;
        }

        string valuesStr = line.substr(startPos, endPos - startPos);
        vector<string> tokens;
        splitString(valuesStr, ',', tokens);

        if (tokens.size() == table.columns.size()) {
            Row row;
            for (auto &token : tokens) {
                token = trim(token);
                if (!token.empty() && token.front() == '\'' && token.back() == '\'') {
                    token = token.substr(1, token.size() - 2);
                }
                row.data.push_back(token);
            }
            table.rows.push_back(row);
            cout << "> Row inserted into " << table.name << endl;
        } else {
            cout << "> Error: Column count mismatch in INSERT statement." << endl;
        }
    } else {
        cout << "> Error: Invalid INSERT syntax." << endl;
    }
}

void selectAll(const Table &table) {
    cout << "> SELECT * FROM " << table.name << ";" << endl;

    for (size_t i = 0; i < table.columns.size(); ++i) {
        cout << table.columns[i];
        if (i < table.columns.size() - 1) cout << ",";
    }
    cout << endl;

    for (const auto &row : table.rows) {
        for (size_t i = 0; i < row.data.size(); ++i) {
            cout << row.data[i];
            if (i < row.data.size() - 1) cout << ",";
        }
        cout << endl;
    }
}

void selectCount(const Table &table) {
    cout << "> SELECT COUNT(*) FROM " << table.name << ";" << endl;
    cout << "Count: " << table.rows.size() << endl;
}

void deleteRow(Table &table, const string &line) {
    // Find WHERE part
    size_t wherePos = line.find("WHERE");
    if (wherePos == string::npos) {
        cout << "> Error: WHERE clause missing." << endl;
        return;
    }

    string wherePart = trim(line.substr(wherePos + 5));  // Extract "WHERE" part

    // Find the equal sign for WHERE part
    size_t whereEqualPos = wherePart.find("=");
    if (whereEqualPos == string::npos) {
        cout << "> Error: Invalid WHERE syntax." << endl;
        return;
    }

    // Extract column and value
    string whereColumn = trim(wherePart.substr(0, whereEqualPos));
    string whereValue = trim(wherePart.substr(whereEqualPos + 1));

    // Remove quotes if whereValue is a string
    if (whereValue.front() == '\'' && whereValue.back() == '\'') {
        whereValue = whereValue.substr(1, whereValue.size() - 2);
    }

    // Convert whereValue to integer for customer_id comparison
    int whereIntValue = -1;
    if (whereColumn == "customer_id") {
        try {
            whereIntValue = stoi(whereValue);  // Convert string to int
        } catch (const invalid_argument &e) {
            cout << "> Error: Invalid customer_id value." << endl;
            return;
        }
    }

    // Iterate over rows to find and delete the correct row
    for (auto it = table.rows.begin(); it != table.rows.end(); ++it) {
        if (whereColumn == "customer_id" && stoi(it->data[0]) == whereIntValue) {  // Compare customer_id (row.data[0] as string)
            table.rows.erase(it);
            cout << "> Row deleted." << endl;
            return;
        }
    }

    cout << "> Error: Row not found." << endl;
}

void updateRow(Table &table, const string &line) {
    size_t setPos = line.find("SET");
    size_t wherePos = line.find("WHERE");

    if (setPos == string::npos || wherePos == string::npos) {
        cout << "> Error: Invalid UPDATE syntax." << endl;
        return;
    }

    string setPart = trim(line.substr(setPos + 3, wherePos - setPos - 3));  // Extract "SET" part
    string wherePart = trim(line.substr(wherePos + 5));  // Extract "WHERE" part

    // Parse the SET part (field=value)
    size_t equalsPos = setPart.find("=");
    if (equalsPos == string::npos) {
        cout << "> Error: Invalid SET syntax." << endl;
        return;
    }

    string columnToUpdate = trim(setPart.substr(0, equalsPos));
    string newValue = trim(setPart.substr(equalsPos + 1));
    if (newValue.front() == '\'' && newValue.back() == '\'') {
        newValue = newValue.substr(1, newValue.size() - 2);  // Remove quotes from string values
    }

    // Parse the WHERE part (column=value)
    size_t whereEqualsPos = wherePart.find("=");
    if (whereEqualsPos == string::npos) {
        cout << "> Error: Invalid WHERE clause." << endl;
        return;
    }

    string whereColumn = trim(wherePart.substr(0, whereEqualsPos));
    string whereValue = trim(wherePart.substr(whereEqualsPos + 1));

    // Convert whereValue to integer for customer_id comparison if applicable
    int whereIntValue = -1;
    if (whereColumn == "customer_id") {
        try {
            whereIntValue = stoi(whereValue);  // Convert string to int
        } catch (const invalid_argument &e) {
            cout << "> Error: Invalid customer_id value." << endl;
            return;
        }
    }

    // Update the correct row based on customer_id
    for (auto &row : table.rows) {
        if (whereColumn == "customer_id" && stoi(row.data[0]) == whereIntValue) {  // Compare customer_id
            // Find the column to update
            for (size_t i = 0; i < table.columns.size(); ++i) {
                if (table.columns[i] == columnToUpdate) {
                    row.data[i] = newValue;  // Update the column value
                    cout << "> Row updated." << endl;
                    return;
                }
            }
            cout << "> Error: Column " << columnToUpdate << " not found." << endl;
            return;
        }
    }

    cout << "> Error: Row with customer_id " << whereIntValue << " not found." << endl;
}

void outputToFile(const Table &table, const string &filename) {
    ofstream file(filename);
    if (file.is_open()) {
        file << ">CREATE fileOutput2.txt;" << endl;
        file << ">DATABASES;" << endl;
        file << "C:\\Windows\\Programming_fundamental\\fileInput2.mdb" << endl;

        file << ">CREATE TABLE " << table.name << "(" << endl;
        for (size_t i = 0; i < table.columns.size(); ++i) {
            file << "customer_" << table.columns[i] << " INT";
            if (i < table.columns.size() - 1) file << "," << endl;
            else file << endl;
        }
        file << ");" << endl;

        for (const auto &row : table.rows) {
            file << ">INSERT INTO" << endl;
            file << "customer(";
            for (size_t i = 0; i < table.columns.size(); ++i) {
                file << table.columns[i];
                if (i < table.columns.size() - 1) file << ",";
            }
            file << ") VALUES(";
            for (size_t i = 0; i < row.data.size(); ++i) {
                file << "'" << row.data[i] << "'";
                if (i < row.data.size() - 1) file << ",";
            }
            file << ");" << endl;
        }

        file << ">SELECT * FROM customer;" << endl;
        for (size_t i = 0; i < table.columns.size(); ++i) {
            file << table.columns[i];
            if (i < table.columns.size() - 1) file << ",";
        }
        file << endl;
        for (const auto &row : table.rows) {
            for (size_t i = 0; i < row.data.size(); ++i) {
                file << row.data[i];
                if (i < row.data.size() - 1) file << ",";
            }
            file << endl;
        }

        file << ">SELECT COUNT(*) FROM customer;" << endl;
        file << "Count: " << table.rows.size() << endl;
        file.close(); // Properly close the file
    } else {
        cout << "> Error: Unable to open output file " << filename << endl;
    }
}
