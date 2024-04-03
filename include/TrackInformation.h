#pragma once

#include <G4ParticleDefinition.hh>
#include <G4ThreeVector.hh>
#include <G4VUserTrackInformation.hh>

class TrackInformation : public G4VUserTrackInformation
{
  public:
    TrackInformation(const G4ThreeVector initPos, const G4ParticleDefinition* pd)
        : initialPosition(initPos)
        , parentParticleDefinition(pd)
    {
    }

    bool                        doComeFromBody = false;
    const G4ThreeVector         initialPosition{};
    const G4ParticleDefinition* parentParticleDefinition{};
};