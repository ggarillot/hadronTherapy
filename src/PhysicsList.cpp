#include "PhysicsList.h"

#include <G4DecayPhysics.hh>
#include <G4EmExtraPhysics.hh>
#include <G4EmStandardPhysics_option4.hh>
#include <G4HadronElasticPhysicsHP.hh>
#include <G4HadronPhysicsQGSP_BIC_HP.hh>
#include <G4IonBinaryCascadePhysics.hh>
#include <G4IonConstructor.hh>
#include <G4LossTableManager.hh>
#include <G4NeutronTrackingCut.hh>
#include <G4RadioactiveDecayPhysics.hh>
#include <G4StoppingPhysics.hh>
#include <G4SystemOfUnits.hh>

PhysicsList::PhysicsList()
    : G4VModularPhysicsList()
{
    G4LossTableManager::Instance();
    defaultCutValue = 1. * mm;

    SetVerboseLevel(0);

    physVec.push_back(new G4DecayPhysics(verboseLevel));
    physVec.push_back(new G4EmStandardPhysics_option4(verboseLevel));

    physVec.push_back(new G4RadioactiveDecayPhysics(verboseLevel));
    physVec.push_back(new G4IonBinaryCascadePhysics(verboseLevel));
    physVec.push_back(new G4EmExtraPhysics(verboseLevel));
    physVec.push_back(new G4HadronElasticPhysicsHP(verboseLevel));
    physVec.push_back(new G4StoppingPhysics(verboseLevel));
    physVec.push_back(new G4HadronPhysicsQGSP_BIC_HP(verboseLevel));
    physVec.push_back(new G4NeutronTrackingCut(verboseLevel));
}

PhysicsList::~PhysicsList()
{
    for (const auto& phys : physVec)
        delete phys;
}

void PhysicsList::ConstructParticle()
{
    for (const auto& phys : physVec)
        phys->ConstructParticle();
}

void PhysicsList::ConstructProcess()
{
    AddTransportation();

    for (const auto& phys : physVec)
        phys->ConstructProcess();

    return;
}
