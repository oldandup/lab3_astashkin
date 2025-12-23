#include <iostream>
#include "logger.h"
#include "pipe_manager.h"
#include "compress_manager.h"
#include "file_manager.h"
#include "ui_controller.h"

using namespace std;

class Application {
private:
    Logger logger;
    int nextPipeId = 1;
    int nextCompressId = 1;
    PipeManager pipeManager;
    CompressManager compressManager;
    FileManager fileManager;
    UIController ui;

public:
    Application() 
        : pipeManager(nextPipeId, logger),
          compressManager(nextCompressId, logger),
          fileManager(logger),
          ui(pipeManager, compressManager, logger, fileManager) {
        logger.Log("APPLICATION STARTED");
    }

    ~Application() {
        logger.Log("APPLICATION CLOSED");
    }

    void Run() {
        int choice;
        while (true) {
            cout << "\n===== Main Menu =====\n";
            cout << "1. Add pipe\n";
            cout << "2. Edit pipe by ID\n";
            cout << "3. Delete pipe\n";
            cout << "4. View all pipes\n";
            cout << "5. Search pipes\n";
            cout << "---------------------\n";
            cout << "6. Add CS\n";
            cout << "7. Edit CS by ID\n";
            cout << "8. Delete CS\n";
            cout << "9. View all CS\n";
            cout << "10. Search CS\n";
            cout << "---------------------\n";
            cout << "11. Connect Network (Pipe + CS)\n";
            cout << "12. View Network Topology\n";
            cout << "13. Topological Sort\n";
            cout << "14. Disconnect Pipe\n";
            cout << "---------------------\n";
            cout << "15. Save all data\n";
            cout << "16. Load all data\n";
            cout << "17. View Logs\n";
            cout << "0. Exit\n";
            cout << "Choose an option: ";
            cin >> choice;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }

            switch (choice) {
            case 1: ui.AddPipe(); break;
            case 2: ui.EditPipeById(); break;
            case 3: ui.DeletePipe(); break;
            case 4: ui.ViewAllPipes(); break;
            case 5: ui.SearchPipes(); break;
            case 6: ui.AddCompress(); break;
            case 7: ui.EditCompressById(); break;
            case 8: ui.DeleteCompress(); break;
            case 9: ui.ViewAllCompress(); break;
            case 10: ui.SearchCompress(); break;
            
            case 11: ui.ConnectNetwork(); break;
            case 12: ui.ViewNetwork(); break;
            case 13: ui.PerformTopologicalSort(); break;
            case 14: ui.DisconnectNetwork(); break;
            
            case 15: ui.SaveData(); break;
            case 16: ui.LoadData(nextPipeId, nextCompressId); break;
            case 17: ui.ViewLogs(); break;
            case 0: return;
            default: cout << "Invalid option.\n";
            }
        }
    }
};

int main() {
    Application app;
    app.Run();
    return 0;
}