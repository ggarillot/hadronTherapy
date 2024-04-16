#include "EventAction.h"
#include "RootWriter.h"
#include "TrackingAction.h"

#include <CLHEP/Units/SystemOfUnits.h>
#include <G4Event.hh>
#include <G4RunManager.hh>
#include <G4Track.hh>

std::atomic<G4int> EventAction::nEventsProcessed = 0;

EventAction::EventAction(RootWriter* rootWriter, TrackingAction* ta)
    : rootWriter(rootWriter)
    , trackingAction(ta)
{
}

void EventAction::BeginOfEventAction(const G4Event* event)
{
    rootWriter->setEventNumber(event->GetEventID());
}

void EventAction::EndOfEventAction(const G4Event*)
{
    rootWriter->fillTree();

    // if (trackingAction->doPrintParticleMemoryMap())
    //     trackingAction->printParticleMemory();

    trackingAction->reset();

    nEventsProcessed++;
}