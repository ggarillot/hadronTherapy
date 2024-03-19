#pragma once

#include "RunAction.h"
#include <G4UserTrackingAction.hh>

class RunAction;
class G4ParticleDefinition;

class TrackingAction : public G4UserTrackingAction
{
  public:
    TrackingAction(RunAction* runAction);

    void PreUserTrackingAction(const G4Track*) override;
    void PostUserTrackingAction(const G4Track*) override;

    void reset();

  protected:
    RunAction* runAction = nullptr;

    std::map<G4int, const G4ParticleDefinition*> particleDefinitions{};
};