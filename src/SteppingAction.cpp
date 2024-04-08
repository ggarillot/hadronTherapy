#include "SteppingAction.h"
#include "RootWriter.h"
#include "Settings.h"
#include "TrackInformation.h"

#include <CLHEP/Random/Random.h>
#include <CLHEP/Units/SystemOfUnits.h>

#include <G4RunManager.hh>
#include <G4Step.hh>
#include <G4String.hh>
#include <G4SystemOfUnits.hh>
#include <G4Track.hh>
#include <G4Types.hh>

SteppingAction::SteppingAction(RootWriter* rootWriter, const Settings& settings)
    : rootWriter(rootWriter)
    , omitNeutrons(settings.omitNeutrons)
{
}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    // if the step is exiting the world we don't care
    if (!step->GetTrack()->GetNextVolume())
        return;

    const auto trackInfo = dynamic_cast<TrackInformation*>(step->GetTrack()->GetUserInformation());
    const auto particleDefinition = step->GetTrack()->GetParticleDefinition();

    const auto preStepPoint = step->GetPreStepPoint();
    const auto postStepPoint = step->GetPostStepPoint();

    const auto preLogicalVolume = preStepPoint->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
    const auto postLogicalVolume = postStepPoint->GetTouchableHandle()->GetVolume()->GetLogicalVolume();

    G4String   regionName = "";
    const auto region = preLogicalVolume->GetRegion();
    if (region)
        regionName = region->GetName();

    const auto postName = postLogicalVolume->GetName();

    if (regionName == "Body")
    {
        HandleBeamInBody(step);
        if (postName == "World" && trackInfo->doComeFromBody)
        {
            bool write = true;
            if (omitNeutrons && particleDefinition->GetPDGEncoding() == 2112)
                write = false;

            if (write)
                rootWriter->addEscapingParticle(step);
        }
    }
}

void SteppingAction::HandleBeamInBody(const G4Step* step)
{
    const auto dE = step->GetTotalEnergyDeposit() / MeV;

    const auto endPos = step->GetPostStepPoint()->GetPosition() / mm;
    const auto beginPos = step->GetPreStepPoint()->GetPosition() / mm;

    const auto pos = CLHEP::HepRandom()() * (endPos - beginPos) + beginPos;

    rootWriter->addEdep(pos, dE);

    // if (step->GetTrack()->GetTrackID() == 1)
    //     rootWriter->addStepLength(step->GetStepLength());
}