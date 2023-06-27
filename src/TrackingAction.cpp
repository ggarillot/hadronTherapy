#include "TrackingAction.h"
#include "EventAction.h"

#include <CLHEP/Units/SystemOfUnits.h>
#include <G4ProcessType.hh>
#include <G4SystemOfUnits.hh>
#include <G4Track.hh>
#include <G4VProcess.hh>

TrackingAction::TrackingAction(EventAction* ea)
    : eventAction(ea)
{
}

void TrackingAction::PreUserTrackingAction(const G4Track*)
{
    // const auto particleDef = track->GetParticleDefinition();

    // std::cout << track->GetTrackID() << " " << track->GetParticleDefinition()->GetPDGEncoding() << std::endl;
    // const auto time = track->GetGlobalTime();

    // const auto id = particleDef->GetPDGEncoding();
    // const auto z = track->GetPosition().z();

    // if (id == 1000060110)
    //     eventAction->addParticle(6, z);
    // else if (id == 1000070130)
    //     eventAction->addParticle(7, z);
    // else if (id == 1000080150)
    //     eventAction->addParticle(8, z);
}

void TrackingAction::PostUserTrackingAction(const G4Track* track)
{
    const auto particleDef = track->GetParticleDefinition();
    const auto time = track->GetGlobalTime() / CLHEP::second;

    const auto id = particleDef->GetPDGEncoding();
    const auto z = track->GetPosition().z();

    if (id == 1000060110)
        eventAction->addParticle(6, z, time);
    else if (id == 1000070130)
        eventAction->addParticle(7, z, time);
    else if (id == 1000080150)
        eventAction->addParticle(8, z, time);
    // else if (id != -11)
    //     return;

    // auto toto =
    //     G4VProcess::GetProcessTypeName(track->GetStep()->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessType());
    // std::cout << toto << std::endl;

    if (track->GetTrackID() != 1)
        return;

    eventAction->primaryEnd(track->GetPosition());
}
