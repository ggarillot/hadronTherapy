#include "SteppingAction.h"
#include "EventAction.h"

#include <CLHEP/Random/Random.h>
#include <CLHEP/Units/SystemOfUnits.h>
#include <RunAction.h>

#include <G4RunManager.hh>
#include <G4Step.hh>
#include <G4SystemOfUnits.hh>

SteppingAction::SteppingAction(EventAction* ea)
    : eventAction(ea)
{
}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    // if the step is exiting the world we don't cares
    if (!step->GetTrack()->GetNextVolume())
        return;

    const auto trackID = step->GetTrack()->GetTrackID();

    const auto preStepPoint = step->GetPreStepPoint();
    const auto postStepPoint = step->GetPostStepPoint();

    const auto preLogicalVolume = preStepPoint->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
    const auto postLogicalVolume = postStepPoint->GetTouchableHandle()->GetVolume()->GetLogicalVolume();

    const auto preName = preLogicalVolume->GetName();
    const auto postName = postLogicalVolume->GetName();

    if (preName == "Body" /*&& step->GetTrack()->GetTrackID() == 1*/)
    {
        HandleBeamInBody(step);
        return;
    }

    if (preName != "World")
        return;

    if (postName == "Barrel" || postName == "EndCap")
    {
        const auto pdg = step->GetTrack()->GetDefinition()->GetPDGEncoding();
        const auto energy = step->GetPreStepPoint()->GetTotalEnergy() / CLHEP::MeV;
        const auto pos = postStepPoint->GetPosition() / CLHEP::mm;
        const auto mom = postStepPoint->GetMomentumDirection();
        const auto time = postStepPoint->GetGlobalTime() / CLHEP::second;

        const auto initialPosition = eventAction->getInitialPosition(trackID);

        eventAction->addEscapingParticle(pdg, pos, mom, energy, time, initialPosition);
    }

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