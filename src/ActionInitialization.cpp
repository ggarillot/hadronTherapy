#include "ActionInitialization.h"
#include "DetectorConstruction.h"
#include "EventAction.h"
#include "PrimaryGeneratorAction.h"
#include "RunAction.h"
#include "SteppingAction.h"
#include "TrackingAction.h"
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

    beamEnergy = stod(b);
}

void ActionInitialization::BuildForMaster() const
{
    RunAction* runAction = new RunAction(baseRootFileName);
    SetUserAction(runAction);
}

void ActionInitialization::Build() const
{

    SetUserAction(new PrimaryGeneratorAction(particleName, beamEnergy));

    RunAction* runAction = new RunAction(baseRootFileName);
    SetUserAction(runAction);

    EventAction* eventAction = new EventAction(runAction);
    SetUserAction(eventAction);

    SetUserAction(new SteppingAction);

    SetUserAction(new TrackingAction(eventAction));
}
