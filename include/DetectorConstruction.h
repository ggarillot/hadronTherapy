#pragma once

#include <G4VUserDetectorConstruction.hh>

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
    DetectorConstruction(const G4String& bodyMaterialStr = "waterGel");
    ~DetectorConstruction() = default;

    G4VPhysicalVolume* Construct() override;

  protected:
    BodyMaterial bodyMaterialType = kWaterGel;
};