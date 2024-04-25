#include "ActionInitialization.h"
#include "CLI11.hpp"
#include "DetectorConstruction.h"
#include "PhysicsList.h"
#include "Settings.h"

#include <CLHEP/Units/SystemOfUnits.h>

#include <G4RunManagerFactory.hh>
#include <G4SteppingVerbose.hh>
#include <G4UImanager.hh>

#include <G4DecayPhysics.hh>
#include <G4PhysListFactory.hh>
#include <G4RadioactiveDecayPhysics.hh>
#include <G4StepLimiterPhysics.hh>
#include <G4UIExecutive.hh>
#include <G4VisExecutive.hh>
#include <QGSP_BIC_HP.hh>

int main(int argc, char** argv)
{
    CLI::App app;

    auto settings = Settings{};

    app.add_option("-N", settings.particleName, "name of the beam particle : proton or carbon")->required();
    app.add_option("-e", settings.beamMeanEnergy, "beam energy in MeV")->required();
    app.add_option("-s", settings.seed, "seed")->required();
    app.add_option("-n", settings.nEvents, "number of events")->required();
    app.add_option("-m", settings.bodyMaterial, "body material : water of waterGel")->default_val("waterGel");
    app.add_option("-b", settings.bodyWidth, "body width in cm")->default_val(15);
    app.add_option("-t", settings.nThreads, "number of threads")->default_val(1);
    app.add_flag("--omitNeutrons", settings.omitNeutrons, "do note write neutrons in file");
    app.add_flag("--beamTree", settings.beamTree, "write beam tree");
    app.add_flag("--minimalTree", settings.minimalTreeForTransverseGammas,
                 "produce a minimal tree with only info about transverse gammas");

    CLI11_PARSE(app, argc, argv);

    if (settings.minimalTreeForTransverseGammas)
    {
        settings.beamTree = false;
        settings.omitNeutrons = true;
    }

    G4Random::setTheSeed(settings.seed + 2);

    auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::MT);
    // auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::SerialOnly);
    runManager->SetNumberOfThreads(settings.nThreads);

    runManager->SetUserInitialization(new DetectorConstruction(settings));

    // G4VModularPhysicsList* phys = new QGSP_BIC_HP;
    G4VModularPhysicsList* phys = new PhysicsList;
    // phys->RegisterPhysics(new G4RadioactiveDecayPhysics);
    runManager->SetUserInitialization(phys);

    // User action initialization
    runManager->SetUserInitialization(new ActionInitialization(settings));

    runManager->Initialize();
    runManager->BeamOn(settings.nEvents);

    delete runManager;
}
