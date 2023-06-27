#include "SteppingAction.h"

#include <CLHEP/Random/Random.h>
#include <CLHEP/Units/SystemOfUnits.h>
#include <RunAction.h>

#include <G4RunManager.hh>
#include <G4Step.hh>
#include <G4SystemOfUnits.hh>

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    const auto logicalVolume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
    const auto name = logicalVolume->GetName();

    if (name == "Body" /*&& step->GetTrack()->GetTrackID() == 1*/)
        HandleBeamInBody(step);

    // if (name != "Barrel" && /*name != "FrontCap" &&*/ name != "EndCap")
    //     return;

    // const auto pdg = step->GetTrack()->GetDefinition()->GetPDGEncoding();
    // const auto energy = step->GetPreStepPoint()->GetTotalEnergy();
    // const auto pos = step->GetPreStepPoint()->GetPosition();
    // const auto mom = step->GetPreStepPoint()->GetMomentumDirection();

    // G4cout << name << " - " << pdg << " : " << energy / keV << " keV" << G4endl;
}

void SteppingAction::HandleBeamInBody(const G4Step* step)
{
    auto runAction = dynamic_cast<const RunAction*>(G4RunManager::GetRunManager()->GetUserRunAction());
    auto rootWriter = runAction->getRootWriter();

    const auto dE = step->GetTotalEnergyDeposit() / MeV;
    const auto z2 = step->GetPostStepPoint()->GetPosition().z() / mm;
    const auto z1 = step->GetPreStepPoint()->GetPosition().z() / mm;

    const auto z = CLHEP::HepRandom()() * (z2 - z1) + z1;

    rootWriter->fillHisto(z, dE);
}