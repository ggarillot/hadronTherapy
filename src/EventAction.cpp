#include "EventAction.h"
#include "RunAction.h"
#include "TrackingAction.h"

#include <CLHEP/Units/SystemOfUnits.h>
#include <G4Event.hh>
#include <G4RunManager.hh>
#include <G4Track.hh>
#include <ctime>
#include <map>

EventAction::EventAction(RunAction* ra, TrackingAction* ta)
    : runAction(ra)
    , trackingAction(ta)
{
}

void EventAction::BeginOfEventAction(const G4Event* event)
{
    auto rootWriter = runAction->getRootWriter();
    rootWriter->setEventNumber(event->GetEventID());
}

void EventAction::EndOfEventAction(const G4Event*)
{
    auto rootWriter = runAction->getRootWriter();
    rootWriter->fillTree();

    trackingAction->reset();

    nEventsElapsed++;
    runAction->update(nEventsElapsed);
}