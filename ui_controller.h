#ifndef UI_CONTROLLER_H
#define UI_CONTROLLER_H

#include "pipe_manager.h"
#include "compress_manager.h"
#include "file_manager.h"
#include "search_engine.h"
#include "network_manager.h"
#include <iostream>
#include <limits>
#include <iomanip>

using namespace std;

class UIController {
private:
    PipeManager& pipeManager;
    CompressManager& compressManager;
    Logger& logger;
    FileManager& fileManager;
    SearchEngine searchEngine;
    NetworkManager networkManager; // Добавляем менеджер сети

public:
    UIController(PipeManager& pm, CompressManager& cm, Logger& log, FileManager& fm)
        : pipeManager(pm), compressManager(cm), logger(log), fileManager(fm), 
          searchEngine(log), networkManager(pm, cm) {}

    // --- СУЩЕСТВУЮЩИЕ МЕТОДЫ ОСТАЮТСЯ БЕЗ ИЗМЕНЕНИЙ (AddPipe, AddCompress, View..., Edit...) ---
    // (Я их опущу для краткости, они такие же, как в твоем коде, только AddPipe нужно чуть подправить, 
    // чтобы он мог принимать предустановленный диаметр)

    // Перегруженный метод добавления трубы (для автоматического создания при соединении)
    int AddPipeInteractive(int fixedDiameter) {
        Pipe pipe = {};
        pipe.diametr = fixedDiameter;
        
        cout << "\n--- Creating new pipe for connection (Diameter: " << fixedDiameter << " mm) ---\n";
        cout << "Enter KM mark (name): ";
        cin.ignore();
        getline(cin, pipe.km_mark);

        cout << "Enter length (km): ";
        cin >> pipe.length;
        if (cin.fail() || pipe.length <= 0) {
            cout << "Error: Invalid length.\n";
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return -1;
        }

        // Диаметр уже задан
        
        cout << "On repair? (0 - no, 1 - yes): ";
        cin >> pipe.repair;

        pipeManager.Add(pipe);
        // Возвращаем ID только что созданной трубы (она последняя в списке)
        return pipeManager.GetAll().back().id;
    }
    
    // Обычный AddPipe (нужно обновить в твоем коде)
    void AddPipe() {
        Pipe pipe = {};
        cout << "\nPipe parameters:\n";
        cout << "Enter KM mark (name): ";
        cin.ignore();
        getline(cin, pipe.km_mark);
        cout << "Enter length (km): ";
        cin >> pipe.length;
        cout << "Enter diameter (mm): ";
        cin >> pipe.diametr;
        cout << "On repair? (0/1): ";
        cin >> pipe.repair;

        if(cin.fail() || pipe.length <= 0 || pipe.diametr <= 0) {
            cin.clear(); cin.ignore(10000, '\n'); cout << "Invalid input.\n"; return;
        }
        pipeManager.Add(pipe);
        cout << "Pipe added.\n";
    }

    // --- НОВЫЕ МЕТОДЫ ДЛЯ СЕТИ ---

    void ConnectNetwork() {
        cout << "\n===== Connect Network Elements =====\n";
        
        // 1. Ввод ID станций
        int srcId, destId;
        cout << "Enter Source CS ID: "; cin >> srcId;
        cout << "Enter Destination CS ID: "; cin >> destId;

        if (cin.fail()) {
            cin.clear(); cin.ignore(10000, '\n'); cout << "Invalid input.\n"; return;
        }

        // Проверка существования станций
        if (!compressManager.FindById(srcId) || !compressManager.FindById(destId)) {
            cout << "Error: One or both CS not found.\n";
            return;
        }
        if (srcId == destId) {
            cout << "Error: Cannot connect CS to itself.\n";
            return;
        }

        // 2. Ввод диаметра
        int diameter;
        cout << "Enter Pipe Diameter (500, 700, 1000, 1400): ";
        cin >> diameter;

        if (diameter != 500 && diameter != 700 && diameter != 1000 && diameter != 1400) {
            cout << "Error: Invalid diameter. Allowed: 500, 700, 1000, 1400.\n";
            return;
        }

        // 3. Поиск свободной трубы
        int pipeId = pipeManager.FindFreePipeID(diameter);

        if (pipeId != -1) {
            cout << "Found free pipe ID: " << pipeId << ". Connecting...\n";
            pipeManager.LinkPipe(pipeId, srcId, destId);
            logger.Log("NETWORK CONNECTED: CS" + to_string(srcId) + " -> CS" + to_string(destId) + " via Pipe" + to_string(pipeId));
        } else {
            cout << "No free pipe with diameter " << diameter << " found.\n";
            cout << "Proceeding to create a new pipe...\n";
            
            pipeId = AddPipeInteractive(diameter);
            
            if (pipeId != -1) {
                pipeManager.LinkPipe(pipeId, srcId, destId);
                logger.Log("NETWORK CONNECTED (NEW PIPE): CS" + to_string(srcId) + " -> CS" + to_string(destId) + " via Pipe" + to_string(pipeId));
            } else {
                cout << "Failed to create pipe. Connection cancelled.\n";
            }
        }
    }

    void DisconnectNetwork() {
        cout << "\n===== Disconnect Network =====\n";
        networkManager.DisplayNetwork();
        cout << "Enter Pipe ID to disconnect: ";
        int id;
        cin >> id;
        
        Pipe* p = pipeManager.FindById(id);
        if (p && p->source_cs_id != 0) {
            pipeManager.UnlinkPipe(id);
            cout << "Pipe disconnected successfully.\n";
            logger.Log("NETWORK DISCONNECTED: Pipe" + to_string(id));
        } else {
            cout << "Error: Pipe not found or not connected.\n";
        }
    }

    void PerformTopologicalSort() {
        cout << "\n===== Topological Sort =====\n";
        vector<int> result = networkManager.TopologicalSort();
        
        if (!result.empty()) {
            cout << "Topological Order of CS: ";
            for (size_t i = 0; i < result.size(); i++) {
                cout << result[i];
                if (i < result.size() - 1) cout << " -> ";
            }
            cout << "\n";
            logger.Log("TOPOLOGICAL SORT EXECUTED");
        }
    }

    void ViewNetwork() {
        networkManager.DisplayNetwork();
    }
    
    // --- ОСТАЛЬНЫЕ МЕТОДЫ (AddCompress, ViewAllPipes, EditPipeById и т.д.) ---
    // Они должны быть здесь, как в твоем оригинальном файле.
    // Убедись, что AddPipe заменен/дополнен выше.
    
    // Вспомогательные заглушки для полноты (чтобы код компилировался, если ты копируешь)
    void AddCompress() { 
        // Код из старого файла 
        Compress c;
        cout << "Enter name: "; cin.ignore(); getline(cin, c.name);
        cout << "Workshops: "; cin >> c.workshop_count;
        cout << "Working: "; cin >> c.workshop_working;
        cout << "Class: "; cin.ignore(); getline(cin, c.classification);
        cout << "Status (0/1): "; cin >> c.working;
        compressManager.Add(c);
    }
    void ViewAllPipes() {
        auto pipes = pipeManager.GetAll();
        for(auto& p : pipes) cout << "ID:" << p.id << " D:" << p.diametr << (p.source_cs_id ? " [Connected]" : "") << endl;
    }
    void ViewAllCompress() {
        auto cs = compressManager.GetAll();
        for(auto& c : cs) cout << "ID:" << c.id << " " << c.name << endl;
    }
    void EditPipeById() { /* старый код */ }
    void EditCompressById() { /* старый код */ }
    void DeletePipe() { /* старый код */ }
    void DeleteCompress() { /* старый код */ }
    void SearchPipes() { /* старый код */ }
    void SearchCompress() { /* старый код */ }
    void SaveData() { fileManager.SaveAllData(pipeManager, compressManager); }
    void LoadData(int& pid, int& cid) { fileManager.LoadAllData(pipeManager, compressManager, pid, cid); }
    void ViewLogs() { logger.ViewLogs(); }
};

#endif