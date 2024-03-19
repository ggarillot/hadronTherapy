#pragma once

#include "RootWriter.h"

#include <G4UserRunAction.hh>

#include <atomic>
#include <chrono>
#include <cstddef>
#include <thread>

class G4Run;

class RunAction : public G4UserRunAction
{
  public:
    RunAction(G4String rootFileName);

    void BeginOfRunAction(const G4Run* run) override;
    void EndOfRunAction(const G4Run* run) override;

    inline RootWriter* getRootWriter() const { return rootWriter; }

    void update(G4int nEventsProcessedThread);
    void notifyThreadComplete();

  protected:
    RootWriter* rootWriter = nullptr;

    G4String baseRootFileName{};

    std::chrono::steady_clock::time_point beginTime{};
    std::chrono::steady_clock::time_point refTime{};

    static std::vector<G4int> processedEventsPerThread;

    std::thread printingThread{};

    static std::atomic<G4int> nThreadsCompleted;
};