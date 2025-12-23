#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "pipe_manager.h"
#include "compress_manager.h"
#include "logger.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

class FileManager {
private:
    Logger& logger;
    string backupFile;

public:
    FileManager(Logger& log, const string& filename = "data_backup.txt") 
        : logger(log), backupFile(filename) {}

    void SaveAllData(const PipeManager& pipeManager, const CompressManager& compressManager, const string& customFilename = "") {
        string filename = customFilename.empty() ? backupFile : customFilename;
        
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "Error: Could not open file.\n";
            return;
        }

        file << "===== DATA BACKUP =====\n\n";
        SavePipes(file, pipeManager.GetAll());
        SaveCompress(file, compressManager.GetAll());

        file.close();
        cout << "All data saved to " << filename << "\n";
        logger.Log("SAVED DATA to " + filename);
    }

    void LoadAllData(PipeManager& pipeManager, CompressManager& compressManager, 
                     int& nextPipeId, int& nextCompressId, const string& customFilename = "") {
        string filename = customFilename.empty() ? backupFile : customFilename;
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Error: File not found.\n";
            return;
        }

        pipeManager.Clear();
        compressManager.Clear();

        string line;
        int maxPipeId = 0;
        int maxStationId = 0;

        Pipe currentPipe = {};
        Compress currentStation = {};
        bool inPipe = false;
        bool inStation = false;

        // Простой парсер
        while (getline(file, line)) {
            if (line.find("===== PIPES DATA") != string::npos) {
                // Если были накопленные данные - сохраняем
                continue;
            }
            if (line.find("===== COMPRESSOR") != string::npos) {
                if (inPipe && currentPipe.id > 0) pipeManager.Add(currentPipe);
                inPipe = false; 
                continue;
            }

            if (line.find("~~~") != string::npos) {
                if (inPipe && currentPipe.id > 0) {
                    pipeManager.Add(currentPipe);
                    maxPipeId = max(maxPipeId, currentPipe.id);
                    currentPipe = {};
                    inPipe = false;
                }
                if (inStation && currentStation.id > 0) {
                    compressManager.Add(currentStation);
                    maxStationId = max(maxStationId, currentStation.id);
                    currentStation = {};
                    inStation = false;
                }
                continue;
            }

            if (line.empty()) continue;

            // Определяем, что парсим
            if (line.find("ID: ") == 0) {
                // Пытаемся понять, это ID трубы или КС, по контексту предыдущих строк или простой логике
                // Здесь упростим: если мы еще не встретили уникальные поля трубы/КС, считаем по заголовкам секций (но тут простой вариант)
                // Лучше полагаться на флаги секций, но для надежности добавим проверку полей.
            }
            
            // Парсинг Труб
            if (line.find("Length (km):") != string::npos || 
                line.find("Diameter (mm):") != string::npos ||
                line.find("From CS:") != string::npos) {
                inPipe = true; inStation = false;
            }
            // Парсинг КС
            if (line.find("Workshops:") != string::npos || line.find("Working:") != string::npos) {
                inStation = true; inPipe = false;
            }

            if (inPipe || (!inStation && line.find("ID: ") == 0 && line.find("Name:") == string::npos)) {
                 ParsePipeLine(line, currentPipe, inPipe);
            } else {
                 ParseCompressLine(line, currentStation, inStation);
            }
        }
        
        // Добавляем последние
        if (inPipe && currentPipe.id > 0) {
            pipeManager.Add(currentPipe);
            maxPipeId = max(maxPipeId, currentPipe.id);
        }
        if (inStation && currentStation.id > 0) {
            compressManager.Add(currentStation);
            maxStationId = max(maxStationId, currentStation.id);
        }

        nextPipeId = maxPipeId + 1;
        nextCompressId = maxStationId + 1;
        file.close();
        logger.Log("LOADED DATA from " + filename);
        cout << "Data loaded.\n";
    }

private:
    void SavePipes(ofstream& file, const vector<Pipe>& pipes) {
        file << "===== PIPES DATA =====\n";
        for (const auto& pipe : pipes) {
            file << "ID: " << pipe.id << "\n";
            file << "KM Mark: " << pipe.km_mark << "\n";
            file << "Length (km): " << fixed << setprecision(2) << pipe.length << "\n";
            file << "Diameter (mm): " << pipe.diametr << "\n";
            file << "On repair: " << (pipe.repair ? "Yes" : "No") << "\n";
            // Сохраняем топологию
            file << "From CS: " << pipe.source_cs_id << "\n";
            file << "To CS: " << pipe.dest_cs_id << "\n";
            file << "~~~\n";
        }
    }

    void SaveCompress(ofstream& file, const vector<Compress>& stations) {
        file << "\n===== COMPRESSOR STATIONS DATA =====\n";
        for (const auto& station : stations) {
            file << "ID: " << station.id << "\n";
            file << "Name: " << station.name << "\n";
            file << "Workshops: " << station.workshop_count << "\n";
            file << "Working: " << station.workshop_working << "\n";
            file << "Classification: " << station.classification << "\n";
            file << "Active: " << (station.working ? "Yes" : "No") << "\n";
            file << "~~~\n";
        }
    }

    void ParsePipeLine(const string& line, Pipe& pipe, bool& inPipe) {
        if (line.find("ID: ") == 0) { pipe.id = stoi(line.substr(4)); inPipe = true; }
        else if (line.find("KM Mark: ") == 0) pipe.km_mark = line.substr(9);
        else if (line.find("Length (km): ") == 0) pipe.length = stod(line.substr(13));
        else if (line.find("Diameter (mm): ") == 0) pipe.diametr = stoi(line.substr(15));
        else if (line.find("On repair: ") == 0) pipe.repair = (line.substr(11) == "Yes");
        else if (line.find("From CS: ") == 0) pipe.source_cs_id = stoi(line.substr(9));
        else if (line.find("To CS: ") == 0) pipe.dest_cs_id = stoi(line.substr(7));
    }

    void ParseCompressLine(const string& line, Compress& station, bool& inStation) {
        if (line.find("ID: ") == 0) { station.id = stoi(line.substr(4)); inStation = true; }
        else if (line.find("Name: ") == 0) station.name = line.substr(6);
        else if (line.find("Workshops: ") == 0) station.workshop_count = stoi(line.substr(11));
        else if (line.find("Working: ") == 0) station.workshop_working = stoi(line.substr(9));
        else if (line.find("Classification: ") == 0) station.classification = line.substr(16);
        else if (line.find("Active: ") == 0) station.working = (line.substr(8) == "Yes");
    }
};

#endif