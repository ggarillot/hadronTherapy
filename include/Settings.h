#pragma once

#include <CLHEP/Units/SystemOfUnits.h>
#include <G4String.hh>
#include <G4Types.hh>

struct Settings
{
    G4int seed = 0;

    G4int nThreads = 1;
    G4int nEvents = 0;

    G4String particleName = "proton";
    G4double beamMeanEnergy = 0;
    G4double sigmaEnergy = 0;

    G4String bodyMaterial = "waterGel";
    G4double bodyWidth = 15 * CLHEP::cm;

    G4bool omitNeutrons = false;
};