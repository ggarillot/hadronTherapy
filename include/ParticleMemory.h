#pragma once

#include <G4ParticleDefinition.hh>
#include <G4ThreeVector.hh>

struct ParticleMemory
{
    G4ThreeVector initialPosition{};
    G4ThreeVector finalPosition{};

    G4double initialEnergy{};
    G4double finalEnergy{};

    const G4ParticleDefinition* particleDefinition{};
};