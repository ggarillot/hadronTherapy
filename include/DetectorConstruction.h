#pragma once

#include <CLHEP/Units/SystemOfUnits.h>
#include <G4VUserDetectorConstruction.hh>

#include "Settings.h"

class G4VPhysicalVolume;

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    enum BodyMaterial
    {
        kWater,
        kWaterGel
    };

  public:
    DetectorConstruction(const Settings& settings);
    ~DetectorConstruction() = default;

    G4VPhysicalVolume* Construct() override;

  protected:
    BodyMaterial bodyMaterialType = kWaterGel;
    G4double     bodyWidth{};
};