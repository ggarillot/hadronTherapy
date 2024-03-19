#pragma once

#include <G4ParticleDefinition.hh>
#include <G4ThreeVector.hh>
#include <G4VUserTrackInformation.hh>

#include <optional>

class TrackInformation : public G4VUserTrackInformation
{
  public:
    TrackInformation(const G4ThreeVector initPos, const G4ParticleDefinition* pd)
        : initialPosition(initPos)
        , parentParticleDefinition(pd)
    {
    }

    const G4ThreeVector         initialPosition{};
    const G4ParticleDefinition* parentParticleDefinition{};

    bool isDetectedBoth() { return detected1.has_value() && detected2.has_value(); }

    std::optional<G4ThreeVector> detected1{};
    std::optional<G4ThreeVector> detected2{};
};