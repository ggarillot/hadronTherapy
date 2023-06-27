#pragma once

#include "RunAction.h"

#include <G4ThreeVector.hh>
#include <G4UserEventAction.hh>
#include <bits/types/clock_t.h>
#include <globals.hh>

#include <chrono>
#include <map>

class G4Event;

class EventAction : public G4UserEventAction
{
  public:
    EventAction(RunAction* runAction);

    void BeginOfEventAction(const G4Event* event) override;
    void EndOfEventAction(const G4Event* event) override;

    void addParticle(G4int Z, G4double z, G4double time);

    void primaryEnd(G4ThreeVector end);

  protected:
    RunAction* runAction = nullptr;

    G4int nEventsElapsed{};

    std::vector<G4int>    idVec{};
    std::vector<G4double> zVec{};
    std::vector<G4double> timeVec{};
};