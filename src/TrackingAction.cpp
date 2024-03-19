#include "TrackingAction.h"
#include "EventAction.h"
#include "RunAction.h"
#include "TrackInformation.h"

#include <CLHEP/Units/SystemOfUnits.h>
#include <G4ParticleDefinition.hh>
#include <G4ProcessType.hh>
#include <G4SystemOfUnits.hh>
#include <G4Track.hh>
#include <G4VProcess.hh>

TrackingAction::TrackingAction(RunAction* ra)
    : runAction(ra)
{
}

void TrackingAction::PreUserTrackingAction(const G4Track* track)
{
    const auto trackID = track->GetTrackID();
    const auto parentID = track->GetParentID();
    const auto particleDefinition = track->GetParticleDefinition();

    particleDefinitions.insert({trackID, particleDefinition});

    const auto initialPosition = track->GetPosition();
    const auto parentParticleDefinition = trackID == 1 ? nullptr : particleDefinitions.at(parentID);

    const auto trackInfo = new TrackInformation(initialPosition, parentParticleDefinition);
    track->SetUserInformation(trackInfo);
}

void TrackingAction::PostUserTrackingAction(const G4Track* track)
{
    const auto particleDef = track->GetParticleDefinition();
    const auto time = track->GetGlobalTime() / CLHEP::second;

    const auto id = particleDef->GetPDGEncoding();
    const auto z = track->GetPosition().z();

    auto rootWriter = runAction->getRootWriter();

    if (id == 1000060110)
        rootWriter->addPositronEmitter(6, z, time);
    else if (id == 1000070130)
        rootWriter->addPositronEmitter(7, z, time);
    else if (id == 1000080150)
        rootWriter->addPositronEmitter(8, z, time);
    // else if (id != -11)
    //     return;

    // auto toto =
    //     G4VProcess::GetProcessTypeName(track->GetStep()->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessType());
    // std::cout << toto << std::endl;

    if (track->GetTrackID() != 1)
        return;

    rootWriter->setPrimaryEnd(track->GetPosition() / CLHEP::mm);
}

void TrackingAction::reset()
{
    particleDefinitions.clear();
}
