#pragma once

#include <G4UserSteppingAction.hh>

class G4Step;

class SteppingAction : public G4UserSteppingAction
{
  public:
    void UserSteppingAction(const G4Step* step) override;

  protected:
    void HandleBeamInBody(const G4Step* step);
};