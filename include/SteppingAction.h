#pragma once

#include <G4Types.hh>
#include <G4UserSteppingAction.hh>

#include "Settings.h"

class G4Step;
class RootWriter;
class TrackingAction;

class SteppingAction : public G4UserSteppingAction
{
  public:
    SteppingAction(RootWriter* rootWriter, const Settings& settings);

    void UserSteppingAction(const G4Step* step) override;

  protected:
    void HandleBeamInBody(const G4Step* step);

  protected:
    RootWriter* rootWriter = nullptr;
    G4bool      omitNeutrons = false;
};