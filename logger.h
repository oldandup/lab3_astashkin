#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <iostream>
#include <ctime>

using namespace std;

class Logger {
private:
    string logFile;

public:
    Logger(const string& filename = "operations_log.txt") : logFile(filename) {}

    string GetCurrentDateTime() const {
        time_t now = time(0);
        struct tm* timeinfo = localtime(&now);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
        return string(buffer);
    }

    void Log(const string& action) {
        ofstream file(logFile, ios::app);
        if (file.is_open()) {
            file << "[" << GetCurrentDateTime() << "] " << action << "\n";
            file.close();
        }
    }

    void ViewLogs() const {
        ifstream file(logFile);
        if (!file.is_open()) {
            cout << "\nNo log file found yet.\n";
            return;
        }

        cout << "\n===== Operation Logs =====\n";
        string line;
        int lineCount = 0;
        while (getline(file, line)) {
            cout << line << "\n";
            lineCount++;
        }
        file.close();
        
        if (lineCount == 0) {
            cout << "No operations logged yet.\n";
        }
    }
};

#endif
