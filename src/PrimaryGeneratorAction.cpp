#include "PrimaryGeneratorAction.h"

#include "RunAction.h"

#include <CLHEP/Matrix/SymMatrix.h>
#include <CLHEP/Matrix/Vector.h>
#include <CLHEP/RandomObjects/RandMultiGauss.h>
#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Vector/ThreeVector.h>
#include <G4IonTable.hh>
#include <G4ParticleGun.hh>
#include <G4ParticleTable.hh>
#include <G4RunManager.hh>
#include <G4SystemOfUnits.hh>
#include <G4ios.hh>
#include <Randomize.hh>
#include <stdexcept>

PrimaryGeneratorAction::PrimaryGeneratorAction(RunAction* runAction, G4String n, G4double e)
    : runAction(runAction)
    , particleName(n)
    , beamEnergy(e)
{
    if (particleName != "proton" && particleName != "carbon")
        throw std::logic_error("proton or carbon only");

    if (beamEnergy < 0)
        throw std::logic_error("positive beam energy please");

    particleGun = new G4ParticleGun(1);
    particleGun->SetParticleMomentumDirection({0, 0, 1});
    particleGun->SetParticlePosition({0, 0, -20 * CLHEP::cm});

    G4cout << "Beam energy is " << beamEnergy / CLHEP::MeV << " MeV/u" << G4endl;
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete particleGun;
    delete randMultiGaussX;
    delete randMultiGaussY;
}

void PrimaryGeneratorAction::setBeamProfile(const CLHEP::HepSymMatrix& matrixXPX, const CLHEP::HepSymMatrix& matrixYPY)
{
    delete randMultiGaussX;
    delete randMultiGaussY;

    CLHEP::HepVector means(2);

    const auto randEngine = G4Random::getTheEngine();
    randMultiGaussX = new CLHEP::RandMultiGauss(*randEngine, means, matrixXPX);
    randMultiGaussY = new CLHEP::RandMultiGauss(*randEngine, means, matrixYPY);
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    auto particleDefinition = particleGun->GetParticleDefinition();

    if (particleDefinition->GetPDGEncoding() == 0) // if it is a geantino, the particle def was never set yet
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

        particleGun->SetParticleDefinition(particleDefinition);
    }

    if (randMultiGaussX)
    {
        const auto vecX = randMultiGaussX->fire();
        const auto vecY = randMultiGaussY->fire();

        // G4cout << vecX << "\n" << vecY << G4endl;

        const G4double angleX = std::sin(vecX[1]);
        const G4double angleY = std::sin(vecY[1]);

        const G4double momZ = std::sqrt(angleX * angleX + angleY * angleY + 1);
        const G4double momX = angleX * momZ;
        const G4double momY = angleY * momZ;

        const auto position = CLHEP::Hep3Vector{vecX[0], vecY[0], -20 * CLHEP::cm};
        const auto direction = CLHEP::Hep3Vector{momX, momY, momZ};

        particleGun->SetParticlePosition(position);
        particleGun->SetParticleMomentumDirection(direction);
    }

    // G4cout << "pos : " << particleGun->GetParticlePosition() / CLHEP::m << " m" << G4endl;
    // G4cout << "mom : " << particleGun->GetParticleMomentumDirection() << G4endl;
    G4cout << "energy : " << particleGun->GetParticleEnergy() / CLHEP::MeV << G4endl;

    auto rootWriter = runAction->getRootWriter();

    rootWriter->addBeamProperties(particleGun->GetParticlePosition(), particleGun->GetParticleMomentumDirection());

    particleGun->GeneratePrimaryVertex(anEvent);
};