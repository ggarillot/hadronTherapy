#include "RunAction.h"
#include "EventAction.h"
#include "RootWriter.h"

#include <G4Run.hh>
#include <G4RunManager.hh>

#include <G4ios.hh>
#include <chrono>
#include <cstdlib>
#include <memory>
#include <sstream>
#include <string>
#include <thread>

RunAction::RunAction(const G4String& rootFileName)
    : rootFileName(rootFileName)
{
    rootWriter = std::make_unique<RootWriter>();
}

void RunAction::BeginOfRunAction(const G4Run*)
{
    using namespace std::chrono_literals;
    const auto runManager = G4RunManager::GetRunManager();
    const auto type = runManager->GetRunManagerType();

    if (type == G4RunManager::masterRM)
    {
        beginTime = std::chrono::steady_clock::now();
        refTime = beginTime;

        const auto threadLoop = [&](const std::chrono::seconds interval) -> void
        {
            const auto nEventsToBeProcessed = runManager->GetNumberOfEventsToBeProcessed();
            const auto nThreads = runManager->GetNumberOfThreads();

            G4cout << nEventsToBeProcessed << " events on " << nThreads << " threads" << G4endl;

            G4int nEventsProcessed = 0;
            G4int nEventsLastCheck = nEventsProcessed;

            while (nEventsProcessed < nEventsToBeProcessed)
            {
                const auto                          now = std::chrono::steady_clock::now();
                const std::chrono::duration<double> deltaTime = now - refTime;
                const std::chrono::duration<double> totalTime = now - beginTime;
                refTime = now;

                nEventsProcessed = EventAction::getNEventsProcessed();

                const auto nEventsDelta = nEventsProcessed - nEventsLastCheck;
                nEventsLastCheck = nEventsProcessed;

                const auto nEventsPerSec = nEventsDelta / deltaTime.count();
                const auto timeRemaining = (nEventsToBeProcessed - nEventsProcessed) / nEventsPerSec;

                G4cout << nEventsProcessed << "/" << nEventsToBeProcessed << " events \t total time "
                       << totalTime.count() << " s "
                       << "\t" << nEventsPerSec << " events/s \t time remaining : " << timeRemaining << " s" << G4endl;

                std::this_thread::sleep_for(interval);
            }
        };

        printingThread = std::thread(threadLoop, 2s);
        return;
    }

    if (type == G4RunManager::workerRM)
        rootFileName += "_T" + std::to_string(G4Threading::G4GetThreadId());

    rootWriter->openRootFile(rootFileName + ".root");
}

void RunAction::EndOfRunAction(const G4Run*)
{
    const auto runManager = G4RunManager::GetRunManager();
    const auto type = runManager->GetRunManagerType();

    if (rootWriter)
    {
        rootWriter->closeRootFile();
        // G4cout << "RootFileDeleted" << G4endl;
    }

    if (type == G4RunManager::masterRM)
    {
        const auto                          now = std::chrono::steady_clock::now();
        const std::chrono::duration<double> totalTime = now - beginTime;

        const auto nEventsProcessed = EventAction::getNEventsProcessed();
        G4cout << nEventsProcessed << " events processed in " << totalTime.count() << " s" << G4endl;

        printingThread.join();

        std::stringstream haddCmd;
        haddCmd << "hadd -j " << runManager->GetNumberOfThreads() << " -f " << rootFileName << ".root " << rootFileName
                << "_T*";
        const auto haddReturn = system(haddCmd.str().c_str());

        G4cout << haddReturn << G4endl;

        if (haddReturn == 0)
        {
            std::stringstream cleanCmd;
            cleanCmd << "rm " << rootFileName << "_T*";
            system(cleanCmd.str().c_str());
        }
    }
    else if (type == G4RunManager::workerRM)
        G4cout << "end Worker" << G4endl;
    else
        G4cout << "end Sequential" << G4endl;
}