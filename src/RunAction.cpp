#include "RunAction.h"
#include "EventAction.h"
#include "RootWriter.h"
#include "Settings.h"

#include <G4Run.hh>
#include <G4RunManager.hh>

#include <G4ios.hh>
#include <algorithm>
#include <chrono>
#include <memory>
#include <thread>

#include <G4AnalysisManager.hh>

RunAction::RunAction(const Settings& settings)
    : settings(settings)
{
    rootWriter = std::make_unique<RootWriter>(settings);
}

void RunAction::BeginOfRunAction(const G4Run*)
{
    using namespace std::chrono_literals;

    G4String bodyType = "w";
    if (settings.bodyMaterial == "waterGel")
        bodyType = "wg";
    std::stringstream sstr;
    sstr << settings.particleName << "_" << int(std::round(settings.beamMeanEnergy)) << "_" << bodyType << "_"
         << settings.seed;
    if (settings.minimalTreeForTransverseGammas)
        sstr << "_gm";

    auto rootFileName = sstr.str();

    rootWriter->openRootFile(rootFileName + ".root");

    if (IsMaster())
    {
        beginTime = std::chrono::steady_clock::now();

        const auto runManager = G4RunManager::GetRunManager();
        const auto nThreads = runManager->GetNumberOfThreads();
        const auto nEventsToBeProcessed = runManager->GetNumberOfEventsToBeProcessed();

        const auto threadLoop = [=](std::chrono::duration<double> interval) -> void
        {
            auto refTime = beginTime;
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
                const auto timeRemaining = 1s * (nEventsToBeProcessed - nEventsProcessed) / nEventsPerSec;

                G4cout << nEventsProcessed << "/" << nEventsToBeProcessed << " events \t total time "
                       << totalTime.count() << " s "
                       << "\t" << nEventsPerSec << " events/s \t time remaining : " << timeRemaining.count() << " s"
                       << G4endl;

                interval = 1s * std::min(interval.count(), (1.1 * timeRemaining).count());
                std::this_thread::sleep_for(interval);
            }
        };

        printingThread = std::thread(threadLoop, 2s);
        return;
    }
}

void RunAction::EndOfRunAction(const G4Run*)
{
    rootWriter->closeRootFile();

    if (IsMaster())
    {
        printingThread.join();
        const auto                          now = std::chrono::steady_clock::now();
        const std::chrono::duration<double> totalTime = now - beginTime;

        const auto nEventsProcessed = EventAction::getNEventsProcessed();
        G4cout << nEventsProcessed << " events processed in " << totalTime.count()
               << " s : " << nEventsProcessed / totalTime.count() << " events/s" << G4endl;
    }
}