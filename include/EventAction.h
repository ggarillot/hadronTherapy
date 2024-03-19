#pragma once

#include "RunAction.h"
#include "TrackingAction.h"

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
    EventAction(RunAction* runAction, TrackingAction* trackingAction);

    void BeginOfEventAction(const G4Event* event) override;
    void EndOfEventAction(const G4Event* event) override;

  protected:
    RunAction*      runAction = nullptr;
    TrackingAction* trackingAction = nullptr;

    G4int nEventsElapsed{};
};