#include "ActionInitialization.h"
#include "EventAction.h"
#include "PrimaryGeneratorAction.h"
#include "RunAction.h"
#include "Settings.h"
#include "SteppingAction.h"
#include "TrackingAction.h"

#include <CLHEP/Units/SystemOfUnits.h>
#include <stdexcept>

#include <G4RunManager.hh>
#include <G4String.hh>

ActionInitialization::ActionInitialization(const Settings& settings)
    : settings(settings)
{
    if (settings.particleName != "proton" && settings.particleName != "carbon")
        throw std::logic_error("proton or carbon only");

    if (settings.bodyMaterial != "water" && settings.bodyMaterial != "waterGel")
        throw std::logic_error("water or waterGel only");
}

void ActionInitialization::BuildForMaster() const
{
    auto runAction = new RunAction(settings);
    SetUserAction(runAction);
}

void ActionInitialization::Build() const
{
    auto runAction = new RunAction(settings);

    auto rootWriter = runAction->getRootWriter();

    auto primaryGeneratorAction = new PrimaryGeneratorAction(rootWriter, settings);
    SetUserAction(primaryGeneratorAction);

    // const G4double sigmaX = 0.2 * CLHEP::mm;
    // const G4double sigmaPX = 0.032 * CLHEP::rad;
    // const G4double corrXPX = -0.9411;
    // const G4double covXPX = corrXPX * sigmaX * sigmaPX;

    // const G4double sigmaY = 0.2 * CLHEP::mm;
    // const G4double sigmaPY = 0.032 * CLHEP::rad;
    // const G4double corrYPY = 0.9411;
    // const G4double covYPY = corrYPY * sigmaY * sigmaPY;

    // CLHEP::HepSymMatrix matrixXPX{2};
    // matrixXPX(1, 1) = sigmaX * sigmaX;
    // matrixXPX(2, 2) = sigmaPX * sigmaPX;
    // matrixXPX(1, 2) = matrixXPX(2, 1) = covXPX;

    // CLHEP::HepSymMatrix matrixYPY{2};
    // matrixYPY(1, 1) = sigmaY * sigmaY;
    // matrixYPY(2, 2) = sigmaPY * sigmaPY;
    // matrixYPY(1, 2) = matrixYPY(2, 1) = covYPY;

    // primaryGeneratorAction->setBeamProfile(matrixXPX, matrixYPY);

    auto trackingAction = new TrackingAction(rootWriter);
    auto eventAction = new EventAction(rootWriter, trackingAction);
    auto steppingAction = new SteppingAction(rootWriter, trackingAction, settings);

    SetUserAction(runAction);
    SetUserAction(eventAction);
    SetUserAction(steppingAction);
    SetUserAction(trackingAction);
}
