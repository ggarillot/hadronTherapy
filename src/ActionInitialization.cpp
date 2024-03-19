#include "ActionInitialization.h"
#include "DetectorConstruction.h"
#include "EventAction.h"
#include "PrimaryGeneratorAction.h"
#include "RunAction.h"
#include "SteppingAction.h"
#include "TrackingAction.h"
#include <CLHEP/Units/SystemOfUnits.h>
#include <stdexcept>
#include <string>

#include <G4RunManager.hh>

ActionInitialization::ActionInitialization(G4String p, G4String b, G4int s)
    : particleName(p)
    , seed(s)
{
    if (particleName != "proton" && particleName != "carbon")
        throw std::logic_error("proton or carbon only");

    baseRootFileName = particleName + "_" + b + "_" + std::to_string(seed);

    beamEnergy = stod(b) * CLHEP::MeV;
}

void ActionInitialization::BuildForMaster() const
{
    auto runAction = new RunAction(baseRootFileName);
    SetUserAction(runAction);
}

void ActionInitialization::Build() const
{
    SetUserAction(new PrimaryGeneratorAction(particleName, beamEnergy));

    auto runAction = new RunAction(baseRootFileName);
    auto trackingAction = new TrackingAction(runAction);
    auto eventAction = new EventAction(runAction, trackingAction);
    auto steppingAction = new SteppingAction(runAction);

    SetUserAction(runAction);
    SetUserAction(eventAction);
    SetUserAction(steppingAction);
    SetUserAction(trackingAction);
}
