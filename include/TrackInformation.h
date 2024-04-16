#pragma once

#include <G4ParticleDefinition.hh>
#include <G4ThreeVector.hh>
#include <G4VUserTrackInformation.hh>

class TrackInformation : public G4VUserTrackInformation
{
  public:
    TrackInformation(const G4ThreeVector initPos, const G4ParticleDefinition* pd, const G4double initEnergy)
        : initialPosition(initPos)
        , parentParticleDefinition(pd)
        , initialEnergy(initEnergy)
    {
    }

    bool                        doComeFromBody = false;
    const G4ThreeVector         initialPosition{};
    const G4ParticleDefinition* parentParticleDefinition{};
    const G4double              initialEnergy{};
};