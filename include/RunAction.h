#pragma once

#include "RootWriter.h"

#include <G4UserRunAction.hh>

#include "Settings.h"
#include <chrono>
#include <thread>

class G4Run;

class RunAction : public G4UserRunAction
{
  public:
    RunAction(const Settings& settings);

    void BeginOfRunAction(const G4Run* run) override;
    void EndOfRunAction(const G4Run* run) override;

    RootWriter* getRootWriter() const { return rootWriter.get(); }

  protected:
    std::unique_ptr<RootWriter> rootWriter = nullptr;

    Settings settings{};

    std::chrono::steady_clock::time_point beginTime{};

    std::thread printingThread{};
};