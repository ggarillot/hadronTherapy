#pragma once

#include <G4ThreeVector.hh>
#include <G4UserEventAction.hh>
#include <bits/types/clock_t.h>
#include <globals.hh>

class G4Event;
class G4Track;
class RunAction;
class TrackingAction;

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