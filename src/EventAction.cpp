#include "EventAction.h"
#include "RunAction.h"

#include <G4Event.hh>
#include <ctime>

EventAction::EventAction(RunAction* ra)
    : runAction(ra)
{
}

void EventAction::BeginOfEventAction(const G4Event*) {}

void EventAction::EndOfEventAction(const G4Event* event)
{
    auto rootWriter = runAction->getRootWriter();
    rootWriter->fillTree(event->GetEventID(), idVec, zVec, timeVec);

    idVec.clear();
    zVec.clear();
    timeVec.clear();

    nEventsElapsed++;
    runAction->update(nEventsElapsed);
}

void EventAction::addParticle(G4int Z, G4double z, G4double t)
{
    idVec.push_back(Z);
    zVec.push_back(z);
    timeVec.push_back(t);
}

void EventAction::primaryEnd(G4ThreeVector end)
{
    auto rootWriter = runAction->getRootWriter();

    rootWriter->fillPrimaryEnd(end.z());
}