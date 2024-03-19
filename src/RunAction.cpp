#include "RunAction.h"
#include "RootWriter.h"

#include <G4Run.hh>
#include <G4RunManager.hh>

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <mutex>
#include <numeric>
#include <sstream>
#include <string>
#include <thread>

std::vector<G4int> RunAction::processedEventsPerThread = {};
std::atomic<G4int> RunAction::nThreadsCompleted = 0;

RunAction::RunAction(G4String n)
    : baseRootFileName(n)
{
}

void RunAction::BeginOfRunAction(const G4Run*)
{
    const auto runManager = G4RunManager::GetRunManager();
    const auto type = runManager->GetRunManagerType();

    if (type == G4RunManager::masterRM)
    {
        beginTime = std::chrono::steady_clock::now();
        refTime = beginTime;

        processedEventsPerThread = std::vector<G4int>(runManager->GetNumberOfThreads(), 0);

        const auto threadLoop = [&](const G4double interval) -> void
        {
            const auto nEventsToBeProcessed = runManager->GetNumberOfEventsToBeProcessed();
            const auto nThreads = runManager->GetNumberOfThreads();

            G4cout << nEventsToBeProcessed << " events on " << nThreads << " threads" << G4endl;

            using namespace std::chrono_literals;
            auto nEvents = 0;

            while (nThreadsCompleted != nThreads)
            {
                const auto                          now = std::chrono::steady_clock::now();
                const std::chrono::duration<double> deltaTime = now - refTime;

                if (deltaTime.count() > interval)
                {
                    const std::chrono::duration<double> totalTime = now - beginTime;
                    refTime = now;

                    const auto nProcessedEventsGlobal =
                        std::accumulate(processedEventsPerThread.begin(), processedEventsPerThread.end(), 0);

                    const auto nEventsDelta = nProcessedEventsGlobal - nEvents;
                    nEvents = nProcessedEventsGlobal;

                    const auto nEventsPerSec = nEventsDelta / deltaTime.count();
                    const auto timeRemaining = (nEventsToBeProcessed - nProcessedEventsGlobal) / nEventsPerSec;

                    G4cout << nProcessedEventsGlobal << "/" << nEventsToBeProcessed << " events \t total time "
                           << totalTime.count() << " s "
                           << "\t" << nEventsPerSec << " events/s \t time remaining : " << timeRemaining << " s"
                           << G4endl;
                }
                std::this_thread::sleep_for(500ms);
            }
        };

        printingThread = std::thread(threadLoop, 2);
        return;
    }

    rootWriter = new RootWriter;

    G4String rootFileName = baseRootFileName;
    if (type == G4RunManager::workerRM)
        rootFileName += "_T" + std::to_string(G4Threading::G4GetThreadId());

    rootWriter->openRootFile(rootFileName + ".root");
}

void RunAction::EndOfRunAction(const G4Run*)
{
    const auto type = G4RunManager::GetRunManager()->GetRunManagerType();

    if (rootWriter)
    {
        rootWriter->closeRootFile();

        delete rootWriter;
        rootWriter = nullptr;
    }

    if (type == G4RunManager::masterRM)
    {
        const auto                          now = std::chrono::steady_clock::now();
        const std::chrono::duration<double> totalTime = now - beginTime;

        const auto nProcessedEventsGlobal =
            std::accumulate(processedEventsPerThread.begin(), processedEventsPerThread.end(), 0);
        G4cout << nProcessedEventsGlobal << " events processed in " << totalTime.count() << " s" << G4endl;

        printingThread.join();

        std::stringstream haddCmd;
        haddCmd << "hadd -f " << baseRootFileName << ".root " << baseRootFileName << "_T*";
        system(haddCmd.str().c_str());

        std::stringstream cleanCmd;
        cleanCmd << "rm " << baseRootFileName << "_T*";
        system(cleanCmd.str().c_str());
    }
    else if (type == G4RunManager::workerRM)
    {
        G4cout << "end Worker" << G4endl;
        notifyThreadComplete();
    }
    else
        G4cout << "end Sequential" << G4endl;
}

void RunAction::update(G4int nEventsProcessedThread)
{
    const auto type = G4RunManager::GetRunManager()->GetRunManagerType();
    if (type == G4RunManager::masterRM)
        return;

    const auto thread = G4Threading::G4GetThreadId();
    processedEventsPerThread[thread] = nEventsProcessedThread;
}

void RunAction::notifyThreadComplete()
{
    nThreadsCompleted++;
}