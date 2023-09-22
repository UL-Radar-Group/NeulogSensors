#include <QApplication>
#include <windows.h>
#include <psapi.h>

#include "data_plotter.h"
#include "neulog_data_fetcher.h"


bool isApplicationRunning(const QString& appName) {
    DWORD processIds[1024];
    DWORD bytesReturned;

    // Get a list of all running process identifiers
    if (EnumProcesses(processIds, sizeof(processIds), &bytesReturned)) {
        DWORD numProcesses = bytesReturned / sizeof(DWORD);

        for (DWORD i = 0; i < numProcesses; ++i) {
            DWORD processId = processIds[i];
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);

            if (hProcess != NULL) {
                TCHAR processName[MAX_PATH];
                if (GetModuleBaseName(hProcess, NULL, processName, sizeof(processName) / sizeof(TCHAR))) {
                    QString processNameStr = QString::fromWCharArray(processName);
                    if (processNameStr.endsWith(appName, Qt::CaseInsensitive)) {
                        // The target application is running
                        CloseHandle(hProcess);
                        return true;
                    }
                }
                CloseHandle(hProcess);
            }
        }
    }
    return false;
}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Check if the target application is already running
    if (!isApplicationRunning("neulog_api.exe")) {
        // The application is already running; you can handle this case as needed
        // For example, display a message and exit
        QMessageBox::information(nullptr, "NeuLog API not running", "Please start neulog_api.exe before launching this tool.");
        return 0;
    }

    QMainWindow mainWindow;
    QWidget* centralWidget = new QWidget(&mainWindow);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    NeulogDataFetcher dataFetcher(&mainWindow);

    QCustomPlot customPlot;
    layout->addWidget(&customPlot);
    DataPlotter dataPlotter(&customPlot);

    // Connect the dataReceived signal from DataFetcher to the updatePlot slot in NeulogDataFetcher
    QObject::connect(&dataFetcher, &NeulogDataFetcher::dataReceived, &dataPlotter, &DataPlotter::updatePlot);

    mainWindow.setMinimumSize(800, 600);  // Set the minimum size to 800x600 pixels
    mainWindow.setCentralWidget(centralWidget);
    mainWindow.show();

    return app.exec();
}

