/*
    =====================================================
    UTILS.H
    Low-level helpers used across the project:
    - portable file/folder helpers (work on Windows + Linux)
    - safe console input readers
    - date/time formatting
    - string splitting/trimming
    None of these belong to a single class - they are plain
    utility functions used by Customer, Account and Bank.
    =====================================================
*/
#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#ifdef _WIN32
#include <direct.h>
#endif

using namespace std;

// ===========================================================
//               PORTABLE FILE / FOLDER HELPERS
// ===========================================================

inline bool pathExists(const string &path)
{
    ifstream fin(path);
    return fin.good();
}

inline bool isDirectory(const string &path)
{
    struct stat st;
    return stat(path.c_str(), &st) == 0 && (st.st_mode & S_IFDIR);
}

inline bool createDirectory(const string &path)
{
#ifdef _WIN32
    int result = _mkdir(path.c_str());
#else
    int result = mkdir(path.c_str(), 0755);
#endif
    return result == 0 || errno == EEXIST;
}

inline vector<string> listFiles(const string &directory)
{
    vector<string> files;
    DIR *dir = opendir(directory.c_str());
    if (!dir) return files;

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        string name = entry->d_name;
        if (name == "." || name == "..") continue;

        string path = directory + name;
        struct stat st;
        if (stat(path.c_str(), &st) == 0 && (st.st_mode & S_IFREG))
            files.push_back(path);
    }

    closedir(dir);
    return files;
}

// ===========================================================
//                      GENERAL UTILITIES
// ===========================================================

inline string trim(const string &s)
{
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

inline string readNonEmptyLine(const string &prompt)
{
    string line;
    while (true)
    {
        cout << prompt;
        getline(cin, line);
        line = trim(line);
        if (!line.empty()) return line;
        cout << "  [Error] This field cannot be empty. Please try again.\n";
    }
}

inline double readDoubleInput(const string &prompt)
{
    string line;
    while (true)
    {
        cout << prompt;
        if (!getline(cin, line)) { cin.clear(); continue; }

        line = trim(line);
        if (line.empty())
        {
            cout << "  [Error] Invalid number. Please enter numeric value.\n";
            continue;
        }

        stringstream ss(line);
        double value;
        if (ss >> value && ss.eof()) return value;

        cout << "  [Error] Invalid number. Please enter numeric value.\n";
    }
}

inline int readIntInput(const string &prompt)
{
    string line;
    while (true)
    {
        cout << prompt;
        if (!getline(cin, line)) { cin.clear(); continue; }

        line = trim(line);
        if (line.empty())
        {
            cout << "  [Error] Invalid input. Please enter a whole number.\n";
            continue;
        }

        stringstream ss(line);
        int value;
        if (ss >> value && ss.eof()) return value;

        cout << "  [Error] Invalid input. Please enter a whole number.\n";
    }
}

inline string currentDate()
{
    time_t now = time(nullptr);
    tm *ltm = localtime(&now);
    ostringstream oss;
    oss << setfill('0') << setw(2) << ltm->tm_mday << "-"
        << setfill('0') << setw(2) << (1 + ltm->tm_mon) << "-"
        << (1900 + ltm->tm_year);
    return oss.str();
}

inline string currentTime()
{
    time_t now = time(nullptr);
    tm *ltm = localtime(&now);
    ostringstream oss;
    oss << setfill('0') << setw(2) << ltm->tm_hour << ":"
        << setfill('0') << setw(2) << ltm->tm_min << ":"
        << setfill('0') << setw(2) << ltm->tm_sec;
    return oss.str();
}

inline vector<string> splitLine(const string &line, char delimiter)
{
    vector<string> tokens;
    stringstream ss(line);
    string token;
    while (getline(ss, token, delimiter))
        tokens.push_back(token);
    return tokens;
}

// ===========================================================
//                 GLOBAL STORAGE CONFIGURATION
// ===========================================================

const string DATA_DIR     = "data/";
const string USERS_DIR    = "users/";
const string TRANS_DIR    = "transactions/";
const string ACCOUNT_FILE = "data/account_number.txt";

const int    STARTING_ACCOUNT_NUMBER = 100001;
const double MINIMUM_DEPOSIT         = 500.0;
const double MINIMUM_BALANCE         = 500.0;

#endif // UTILS_H