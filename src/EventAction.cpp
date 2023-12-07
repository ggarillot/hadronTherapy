#include "EventAction.h"
#include "RunAction.h"

#include <CLHEP/Units/SystemOfUnits.h>
#include <G4Event.hh>
#include <G4Track.hh>
#include <ctime>
#include <map>

EventAction::EventAction(RunAction* ra)
    : runAction(ra)
{
}

void EventAction::BeginOfEventAction(const G4Event*) {}

void EventAction::EndOfEventAction(const G4Event* event)
{
    auto rootWriter = runAction->getRootWriter();
    rootWriter->fillTree(event->GetEventID(), idVec, zVec, timeVec, pdgEscapingVec, posEscapingVec, momEscapingVec,
                         eEscapingVec, timeEscapingVec, initPosEscapingVec);

    idVec.clear();
    zVec.clear();
    timeVec.clear();

    pdgEscapingVec.clear();
    posEscapingVec.clear();
    momEscapingVec.clear();
    eEscapingVec.clear();
    timeEscapingVec.clear();

    nEventsElapsed++;
    runAction->update(nEventsElapsed);

    initialPositions.clear();
    initPosEscapingVec.clear();
}

void EventAction::addParticle(G4int Z, G4double z, G4double t)
{
    idVec.push_back(Z);
    zVec.push_back(z);
    timeVec.push_back(t);
}

void EventAction::addEscapingParticle(
    G4int pdg, G4ThreeVector pos, G4ThreeVector mom, G4double e, G4double time, G4ThreeVector initialPosition)
{
    pdgEscapingVec.push_back(pdg);
    posEscapingVec.push_back(pos);
    momEscapingVec.push_back(mom);
    eEscapingVec.push_back(e);
    timeEscapingVec.push_back(time);
    initPosEscapingVec.push_back(initialPosition);
}

void EventAction::primaryEnd(G4ThreeVector end)
{
    auto rootWriter = runAction->getRootWriter();

    rootWriter->fillPrimaryEnd(end.z());
}

void EventAction::addInitialPosition(const G4Track* track)
{
    const auto position = track->GetPosition() / CLHEP::mm;
    const auto trackID = track->GetTrackID();

    initialPositions.insert({trackID, position});
}

G4ThreeVector EventAction::getInitialPosition(const G4int id)
{
    return initialPositions.at(id);
}