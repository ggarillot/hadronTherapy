#pragma once

#include "RunAction.h"

#include <G4ThreeVector.hh>
#include <G4UserEventAction.hh>
#include <bits/types/clock_t.h>
#include <globals.hh>

#include <chrono>
#include <map>

class G4Event;
class G4Track;

class EventAction : public G4UserEventAction
{
  public:
    EventAction(RunAction* runAction);

    void BeginOfEventAction(const G4Event* event) override;
    void EndOfEventAction(const G4Event* event) override;

    void addParticle(G4int Z, G4double z, G4double time);
    void addEscapingParticle(
        G4int pdg, G4ThreeVector pos, G4ThreeVector mom, G4double e, G4double time, G4ThreeVector initialPosition);

    void primaryEnd(G4ThreeVector end);

    void addInitialPosition(const G4Track* track);

    G4ThreeVector getInitialPosition(const G4int id);

  protected:
    RunAction* runAction = nullptr;

    G4int nEventsElapsed{};

    std::vector<G4int>    idVec{};
    std::vector<G4double> zVec{};
    std::vector<G4double> timeVec{};

    std::vector<G4int>         pdgEscapingVec{};
    std::vector<G4ThreeVector> posEscapingVec{};
    std::vector<G4ThreeVector> momEscapingVec{};
    std::vector<G4ThreeVector> initPosEscapingVec{};
    std::vector<G4double>      eEscapingVec{};
    std::vector<G4double>      timeEscapingVec{};

    std::map<G4int, G4ThreeVector> initialPositions = {};
};