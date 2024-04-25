#include "DetectorConstruction.h"

#include <CLHEP/Units/SystemOfUnits.h>
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4NistManager.hh>
#include <G4PVPlacement.hh>
#include <G4Region.hh>
#include <G4SystemOfUnits.hh>
#include <G4Tubs.hh>
#include <G4UserLimits.hh>
#include <G4VPhysicalVolume.hh>
#include <stdexcept>

DetectorConstruction::DetectorConstruction(const Settings& settings)
{
    bodyWidth = settings.bodyWidth * CLHEP::cm;
    if (bodyWidth < 0)
        throw;

    if (settings.bodyMaterial == "waterGel")
        bodyMaterialType = kWaterGel;
    else
        bodyMaterialType = kWater;

    G4cout << "Body is " << settings.bodyMaterial << G4endl;
    G4cout << "Body width : " << bodyWidth / CLHEP::cm << " cm" << G4endl;
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    auto nist = G4NistManager::Instance();

    G4Material* bodyMaterial = nullptr;

    if (bodyMaterialType == kWaterGel)
    {
        auto H = nist->FindOrBuildElement(1);
        auto C = nist->FindOrBuildElement(6);
        auto O = nist->FindOrBuildElement(8);

        bodyMaterial = new G4Material("WaterGel", 1.010 * g / cm3, 3);
        bodyMaterial->AddElement(H, 0.11);
        bodyMaterial->AddElement(C, 0.04650);
        bodyMaterial->AddElement(O, 0.8435);
    }
    else if (bodyMaterialType == kWater)
    {
        bodyMaterial = nist->FindOrBuildMaterial("G4_WATER");
    }
    else
    {
        throw std::logic_error("body material unknown");
    }

    auto air = nist->FindOrBuildMaterial("G4_AIR");
    // auto galactic = nist->FindOrBuildMaterial("G4_Galactic");

    //
    // World
    //
    G4double worldSizeXY = 5 * m;
    G4double worldSizeZ = 5 * m;
    G4double bodyLength = 30 * cm;

    auto solidWorld = new G4Box("World", 0.5 * worldSizeXY, 0.5 * worldSizeXY, 0.5 * worldSizeZ);

    auto logicWorld = new G4LogicalVolume(solidWorld, air, "World");

    auto physWorld = new G4PVPlacement(0,          // no rotation
                                       {},         // at (0,0,0)
                                       logicWorld, // its logical volume
                                       "World",    // its name
                                       0,          // its mother  volume
                                       false,      // no boolean operation
                                       0,          // copy number
                                       true);      // overlaps checking

    auto solidBody = new G4Tubs("Body", 0, bodyWidth, 0.5 * bodyLength, 0 * deg, 360 * deg);
    auto logicBody = new G4LogicalVolume(solidBody, bodyMaterial, "Body");

    new G4PVPlacement(nullptr, {0, 0, 0.5 * bodyLength}, logicBody, "Body", logicWorld, false, 0, true);

    auto bodyRegion = new G4Region("Body");
    bodyRegion->AddRootLogicalVolume(logicBody);

    return physWorld;
}