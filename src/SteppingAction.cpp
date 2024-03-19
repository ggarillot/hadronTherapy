#include "SteppingAction.h"
#include "RunAction.h"
#include "TrackInformation.h"

#include <CLHEP/Random/Random.h>
#include <CLHEP/Units/SystemOfUnits.h>

#include <G4RunManager.hh>
#include <G4Step.hh>
#include <G4SystemOfUnits.hh>
#include <G4Track.hh>

SteppingAction::SteppingAction(RunAction* ra)
    : runAction(ra)
{
}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    // if the step is exiting the world we don't care
    if (!step->GetTrack()->GetNextVolume())
        return;

    const auto preStepPoint = step->GetPreStepPoint();
    const auto postStepPoint = step->GetPostStepPoint();

    const auto preLogicalVolume = preStepPoint->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
    const auto postLogicalVolume = postStepPoint->GetTouchableHandle()->GetVolume()->GetLogicalVolume();

    const auto preName = preLogicalVolume->GetName();
    const auto postName = postLogicalVolume->GetName();

    auto rootWriter = runAction->getRootWriter();

    if (preName == "Body")
    {
        HandleBeamInBody(step);
        if (postName == "World")
            rootWriter->addEscapingParticle(step);
    }

    if (!step->GetTrack()->GetDynamicParticle()->GetCharge())
        return;

    if (postName == "Detector1")
    {
        auto userInfo = dynamic_cast<TrackInformation*>(step->GetTrack()->GetUserInformation());
        userInfo->detected1 = postStepPoint->GetPosition();

        if (userInfo->isDetectedBoth())
            rootWriter->addDetectedParticle(step);
    }
    if (postName == "Detector2")
    {
        auto userInfo = dynamic_cast<TrackInformation*>(step->GetTrack()->GetUserInformation());
        userInfo->detected2 = postStepPoint->GetPosition();

        if (userInfo->isDetectedBoth())
            rootWriter->addDetectedParticle(step);
    }
}

void SteppingAction::HandleBeamInBody(const G4Step* step)
{
    auto rootWriter = runAction->getRootWriter();

    const auto dE = step->GetTotalEnergyDeposit() / MeV;
    const auto z2 = step->GetPostStepPoint()->GetPosition().z() / mm;
    const auto z1 = step->GetPreStepPoint()->GetPosition().z() / mm;

    const auto z = CLHEP::HepRandom()() * (z2 - z1) + z1;

    rootWriter->fillHisto(z, dE);
}