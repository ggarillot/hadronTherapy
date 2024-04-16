#pragma once

#include <G4ThreeVector.hh>
#include <G4Types.hh>
#include <G4UserEventAction.hh>
#include <globals.hh>

#include <atomic>

class G4Event;
class G4Track;
class RootWriter;
class TrackingAction;

class EventAction : public G4UserEventAction
{
  public:
    EventAction(RootWriter* rootWriter, TrackingAction* trackingAction);

    void BeginOfEventAction(const G4Event* event) override;
    void EndOfEventAction(const G4Event* event) override;

    static G4int getNEventsProcessed() { return nEventsProcessed; }
    static void  setNEventsProcessed() { nEventsProcessed = 0; }

  protected:
    RootWriter*     rootWriter = nullptr;
    TrackingAction* trackingAction = nullptr;

    G4int nEventsElapsed{};

    static std::atomic<G4int> nEventsProcessed;
};