#include "ActionInitialization.h"
#include "CLI11.hpp"
#include "DetectorConstruction.h"
#include "PhysicsList.h"

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

    auto settings = Settings{};

    // G4String particleName{};
    // G4String beamEnergyStr{};
    // G4int    seed{};
    // // G4int    nEvents{};
    // G4String bodyMaterial{};
    // G4double bodyWidth{};
    // // G4int    nThreads{};

    app.add_option("-N", settings.particleName, "name of the beam particle : proton or carbon")->required();
    app.add_option("-e", settings.beamMeanEnergy, "beam energy in MeV")->required();
    app.add_option("-s", settings.seed, "seed")->required();
    // app.add_option("-n", nEvents, "number of events")->required();
    app.add_option("-m", settings.bodyMaterial, "body material : water of waterGel")->default_val("waterGel");
    app.add_option("-b", settings.bodyWidth, "body width in cm")->default_val(10);
    // app.add_option("-t", nThreads, "number of threads")->default_val(1);

    CLI11_PARSE(app, argc, argv);

    G4UIExecutive* ui = new G4UIExecutive(argc, argv);

    G4Random::setTheSeed(settings.seed + 2);

    auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::SerialOnly);
    // runManager->SetNumberOfThreads(nThreads);

    runManager->SetUserInitialization(new DetectorConstruction(settings));

    // G4VModularPhysicsList* phys = new QGSP_BIC_HP;
    G4VModularPhysicsList* phys = new PhysicsList;
    // phys->RegisterPhysics(new G4RadioactiveDecayPhysics);
    runManager->SetUserInitialization(phys);

    // User action initialization
    runManager->SetUserInitialization(new ActionInitialization(settings));

    // Initialize visualization
    //
    G4VisManager* visManager = new G4VisExecutive;

    // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
    // G4VisManager* visManager = new G4VisExecutive("Quiet");
    visManager->Initialize();

    // Get the pointer to the User Interface manager
    G4UImanager* UImanager = G4UImanager::GetUIpointer();

    // Process macro or start UI session
    //

    if (!ui)
    {
        // batch mode
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        UImanager->ApplyCommand(command + fileName);
    }
    else
    {
        // interactive mode
        UImanager->ApplyCommand("/control/execute init_vis.mac");
        ui->SessionStart();
        delete ui;
    }

    // Job termination
    // Free the store: user actions, physics_list and detector_description are
    // owned and deleted by the run manager, so they should not be deleted
    // in the main() program !

    // runManager->Initialize();
    // runManager->BeamOn(nEvents);

    delete visManager;
    delete runManager;
}
