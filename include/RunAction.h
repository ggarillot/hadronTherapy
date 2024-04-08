#pragma once

#include "RootWriter.h"

#include <G4UserRunAction.hh>

#include <chrono>
#include <thread>

class G4Run;

class RunAction : public G4UserRunAction
{
  public:
    RunAction(const G4String& rootFileName);

    void BeginOfRunAction(const G4Run* run) override;
    void EndOfRunAction(const G4Run* run) override;

    RootWriter* getRootWriter() const { return rootWriter.get(); }

  protected:
    std::unique_ptr<RootWriter> rootWriter = nullptr;

    G4String rootFileName{};
    G4bool   omitNeutrons = false;

    std::chrono::steady_clock::time_point beginTime{};
    std::chrono::steady_clock::time_point refTime{};

    std::thread printingThread{};
};