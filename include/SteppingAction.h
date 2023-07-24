#pragma once
#include "EventAction.h"

#include <G4UserSteppingAction.hh>

class G4Step;

class SteppingAction : public G4UserSteppingAction
{
  public:
    SteppingAction(EventAction* eventAction);

    void UserSteppingAction(const G4Step* step) override;

  protected:
    void HandleBeamInBody(const G4Step* step);

  protected:
    EventAction* eventAction = nullptr;
};