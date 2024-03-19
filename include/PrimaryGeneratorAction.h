#pragma once

#include <G4VUserPrimaryGeneratorAction.hh>
#include <globals.hh>

class G4ParticleGun;
class G4ParticleDefinition;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction(G4String particleNane, G4double beamEnergy);
    ~PrimaryGeneratorAction();

    void GeneratePrimaries(G4Event* anEvent) override;

  protected:
    G4ParticleGun* particleGun = nullptr;

    G4ParticleDefinition* particleDefinition = nullptr;

    G4String particleName{};
    G4double beamEnergy{};
};