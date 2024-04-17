#pragma once

#include "RootWriter.h"
#include "Settings.h"

#include <G4VUserPrimaryGeneratorAction.hh>

#include <globals.hh>

#include <CLHEP/RandomObjects/RandMultiGauss.h>

class G4ParticleGun;
class G4ParticleDefinition;
class RootWriter;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction(RootWriter* rootWriter, const Settings& settings);
    ~PrimaryGeneratorAction();

    void setBeamProfile(const CLHEP::HepSymMatrix& matrixXPX, const CLHEP::HepSymMatrix& matrixYPY);

    void GeneratePrimaries(G4Event* anEvent) override;

  protected:
    RootWriter* rootWriter = nullptr;

    G4ParticleGun* particleGun = nullptr;

    CLHEP::RandMultiGauss* randMultiGaussX = nullptr;
    CLHEP::RandMultiGauss* randMultiGaussY = nullptr;

    G4String particleName{};
    G4double beamEnergy{};
};