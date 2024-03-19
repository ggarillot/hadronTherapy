#pragma once

#include <G4UserSteppingAction.hh>

class G4Step;
class RunAction;
class TrackingAction;

class SteppingAction : public G4UserSteppingAction
{
  public:
    SteppingAction(RunAction* runAction);

    void UserSteppingAction(const G4Step* step) override;

  protected:
    void HandleBeamInBody(const G4Step* step);

  protected:
    RunAction* runAction = nullptr;
};