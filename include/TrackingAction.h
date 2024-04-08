#pragma once

#include "RootWriter.h"
#include <G4Types.hh>
#include <G4UserTrackingAction.hh>

#include <map>

class RunAction;
class G4ParticleDefinition;

class TrackingAction : public G4UserTrackingAction
{
  public:
    TrackingAction(RootWriter* rootWriter);

    void PreUserTrackingAction(const G4Track* track) override;
    void PostUserTrackingAction(const G4Track* track) override;

    void reset();

  protected:
    RootWriter* rootWriter = nullptr;

    std::map<G4int, const G4ParticleDefinition*> particleDefinitions{};
};