#include "PrimaryGeneratorAction.h"

#include <CLHEP/Units/SystemOfUnits.h>
#include <G4IonTable.hh>
#include <G4ParticleGun.hh>
#include <G4ParticleTable.hh>
#include <G4SystemOfUnits.hh>
#include <stdexcept>

PrimaryGeneratorAction::PrimaryGeneratorAction(G4String n, G4double e)
    : particleName(n)
    , beamEnergy(e)
{
    if (particleName != "proton" && particleName != "carbon")
        throw std::logic_error("proton or carbon only");

    if (beamEnergy < 0)
        throw std::logic_error("positive beam energy please");

    particleGun = new G4ParticleGun(1);
    particleGun->SetParticleMomentumDirection({0, 0, 1});
    particleGun->SetParticlePosition({0, 0, -1.2 * m});

    G4cout << "Beam energy is " << beamEnergy / CLHEP::MeV << " MeV/u" << G4endl;
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete particleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    if (particleDefinition == nullptr)
    {
        if (particleName == "proton")
        {
            particleDefinition = G4ParticleTable::GetParticleTable()->FindParticle("proton");
            particleGun->SetParticleEnergy(beamEnergy);
        }
        else
        {
            particleDefinition = G4IonTable::GetIonTable()->GetIon(1000060120);
            particleGun->SetParticleEnergy(beamEnergy * 12);
        }
    }
    particleGun->SetParticleDefinition(particleDefinition);

    particleGun->GeneratePrimaryVertex(anEvent);
}
