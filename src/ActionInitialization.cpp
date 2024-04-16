#include "ActionInitialization.h"
#include "EventAction.h"
#include "PrimaryGeneratorAction.h"
#include "RunAction.h"
#include "Settings.h"
#include "SteppingAction.h"
#include "TrackingAction.h"

#include <CLHEP/Units/SystemOfUnits.h>
#include <stdexcept>
#include <string>

#include <G4RunManager.hh>

ActionInitialization::ActionInitialization(const Settings& settings)
    : settings(settings)
{
    particleName = settings.particleName;
    seed = settings.seed;
    if (particleName != "proton" && particleName != "carbon")
        throw std::logic_error("proton or carbon only");

    std::stringstream sstr;
    sstr << particleName << "_" << int(std::round(settings.beamMeanEnergy)) << "_" << seed;
    baseRootFileName = sstr.str();

    // beamEnergy = stod(b) * CLHEP::MeV;
    beamEnergy = settings.beamMeanEnergy;
}

void ActionInitialization::BuildForMaster() const
{
    auto runAction = new RunAction(baseRootFileName);
    SetUserAction(runAction);
}

void ActionInitialization::Build() const
{
    auto runAction = new RunAction(baseRootFileName);

    auto rootWriter = runAction->getRootWriter();

    auto primaryGeneratorAction = new PrimaryGeneratorAction(rootWriter, particleName, beamEnergy);
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
