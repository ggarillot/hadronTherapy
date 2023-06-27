#pragma once

#include "EventAction.h"
#include <G4UserTrackingAction.hh>

class TrackingAction : public G4UserTrackingAction
{
  public:
    TrackingAction(EventAction* eventAction);

    void PreUserTrackingAction(const G4Track*) override;
    void PostUserTrackingAction(const G4Track*) override;

  protected:
    EventAction* eventAction = nullptr;
};