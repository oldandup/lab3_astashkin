#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include "pipe_manager.h"
#include "compress_manager.h"
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <iostream>

using namespace std;

class NetworkManager {
private:
    PipeManager& pipeManager;
    CompressManager& compressManager;

public:
    NetworkManager(PipeManager& pm, CompressManager& cm) 
        : pipeManager(pm), compressManager(cm) {}

    // Отображение сети (Графа)
    void DisplayNetwork() {
        cout << "\n===== Gas Transport Network =====\n";
        bool hasConnections = false;
        
        // Словарь для быстрого доступа к названиям станций
        auto& stations = compressManager.GetAll();
        map<int, string> csNames;
        for(const auto& cs : stations) csNames[cs.id] = cs.name;

        for (const auto& pipe : pipeManager.GetAll()) {
            if (pipe.source_cs_id != 0 && pipe.dest_cs_id != 0) {
                cout << "CS " << pipe.source_cs_id << " (" << csNames[pipe.source_cs_id] << ")"
                     << " --[Pipe " << pipe.id << ", d" << pipe.diametr << "]--> "
                     << "CS " << pipe.dest_cs_id << " (" << csNames[pipe.dest_cs_id] << ")\n";
                hasConnections = true;
            }
        }

        if (!hasConnections) {
            cout << "No active connections in the network.\n";
        }
    }

    // Топологическая сортировка (DFS)
    // Возвращает отсортированный список ID станций или пустой список, если есть цикл
    vector<int> TopologicalSort() {
        map<int, vector<int>> adj; // Граф: ID -> список ID соседей
        set<int> nodes;            // Все уникальные узлы, участвующие в сети

        // Строим граф
        for (const auto& pipe : pipeManager.GetAll()) {
            if (pipe.source_cs_id != 0 && pipe.dest_cs_id != 0) {
                adj[pipe.source_cs_id].push_back(pipe.dest_cs_id);
                nodes.insert(pipe.source_cs_id);
                nodes.insert(pipe.dest_cs_id);
            }
        }

        if (nodes.empty()) {
            cout << "Network is empty.\n";
            return {};
        }

        // 0 - White (не посещен), 1 - Gray (в процессе), 2 - Black (посещен)
        map<int, int> visited; 
        vector<int> result;
        bool hasCycle = false;

        // Лямбда-функция для DFS (чтобы не создавать отдельный метод)
        // Примечание: в C++ рекурсивные лямбды требуют auto&& или std::function
        // Используем std::function для простоты
        function<void(int)> dfs = [&](int u) {
            visited[u] = 1; // Gray
            for (int v : adj[u]) {
                if (visited[v] == 1) {
                    hasCycle = true;
                    return;
                }
                if (visited[v] == 0) {
                    dfs(v);
                    if (hasCycle) return;
                }
            }
            visited[u] = 2; // Black
            result.push_back(u);
        };

        for (int node : nodes) {
            if (visited[node] == 0) {
                dfs(node);
                if (hasCycle) break;
            }
        }

        if (hasCycle) {
            cout << "\nERROR: Cycle detected in the network! Topological sort impossible.\n";
            return {};
        }

        reverse(result.begin(), result.end());
        return result;
    }

    void DisconnectPipe(int pipeId) {
        pipeManager.UnlinkPipe(pipeId);
    }
};

#endif