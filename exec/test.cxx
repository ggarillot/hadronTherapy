#include "ActionInitialization.h"
#include "CLI11.hpp"
#include "DetectorConstruction.h"
#include "PhysicsList.h"

#include <CLHEP/Units/SystemOfUnits.h>
#include <TROOT.h>

#include <G4RunManagerFactory.hh>
#include <G4SteppingVerbose.hh>
#include <G4UImanager.hh>

#include <G4DecayPhysics.hh>
#include <G4PhysListFactory.hh>
#include <G4RadioactiveDecayPhysics.hh>
#include <G4UIExecutive.hh>
#include <G4VisExecutive.hh>
#include <QGSP_BIC_HP.hh>

int main(int argc, char** argv)
{
    CLI::App app;

    G4String particleName{};
    G4String beamEnergyStr{};
    G4int    seed{};
    G4int    nEvents{};
    G4String bodyMaterial{};
    G4double bodyWidth{};
    G4int    nThreads{};

    app.add_option("-N", particleName, "name of the beam particle : proton or carbon")->required();
    app.add_option("-e", beamEnergyStr, "beam energy")->required();
    app.add_option("-s", seed, "seed")->required();
    app.add_option("-n", nEvents, "number of events")->required();
    app.add_option("-m", bodyMaterial, "body material : water of waterGel")->default_val("waterGel");
    app.add_option("-b", bodyWidth, "body width in cm")->default_val(20);
    app.add_option("-t", nThreads, "number of threads")->default_val(1);

    CLI11_PARSE(app, argc, argv);

    ROOT::EnableThreadSafety();

    G4Random::setTheSeed(seed + 2);

    auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::MT);
    runManager->SetNumberOfThreads(nThreads);

    runManager->SetUserInitialization(new DetectorConstruction(bodyWidth * CLHEP::cm, bodyMaterial));

    // G4VModularPhysicsList* phys = new QGSP_BIC_HP;
    G4VModularPhysicsList* phys = new PhysicsList;
    // phys->RegisterPhysics(new G4RadioactiveDecayPhysics);
    runManager->SetUserInitialization(phys);

    // User action initialization
    runManager->SetUserInitialization(new ActionInitialization(particleName, beamEnergyStr, seed));

    runManager->Initialize();
    runManager->BeamOn(nEvents);

    delete runManager;
}
