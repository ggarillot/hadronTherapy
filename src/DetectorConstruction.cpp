#include "DetectorConstruction.h"

#include <CLHEP/Units/SystemOfUnits.h>
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4NistManager.hh>
#include <G4PVPlacement.hh>
#include <G4SystemOfUnits.hh>
#include <G4Tubs.hh>
#include <G4VPhysicalVolume.hh>
#include <stdexcept>

DetectorConstruction::DetectorConstruction(const G4double bWidth, const G4String& bodyMaterialStr)
    : bodyWidth(bWidth)
{
    if (bodyWidth < 0)
        throw;

    if (bodyMaterialStr == "waterGel")
        bodyMaterialType = kWaterGel;
    else
        bodyMaterialType = kWater;

    G4cout << "Body is " << bodyMaterialStr << G4endl;
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
    auto galactic = nist->FindOrBuildMaterial("G4_Galactic");

    //
    // World
    //
    G4double worldSizeXY = 5 * m;
    G4double worldSizeZ = 5 * m;
    G4double bodyLength = 100 * cm;

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
    // auto physBody =
    new G4PVPlacement(nullptr, {0, 0, 0.5 * bodyLength}, logicBody, "Body", logicWorld, false, 0, true);

    // for stepping
    G4double thickness = 3.6 * mm;
    G4double detectorLength = 5 * cm;
    G4double detectorHeight = 5 * cm;

    G4double detectionTargetDistance = 80 * mm;
    G4double detectionDistance = 250 * mm;
    G4double detectionAngle = 30 * deg;

    G4ThreeVector detectorCenter1 = G4ThreeVector(0, 0, detectionTargetDistance - 0.5 * thickness) +
                                    G4ThreeVector(0, 0, detectionDistance).rotate(detectionAngle, {0, 1, 0});

    G4ThreeVector detectorCenter2 = G4ThreeVector(0, 0, detectionTargetDistance + 0.5 * thickness) +
                                    G4ThreeVector(0, 0, detectionDistance).rotate(detectionAngle, {0, 1, 0});

    G4RotationMatrix* detectorRotation = new G4RotationMatrix();
    detectorRotation->rotate(-detectionAngle, {0, 1, 0});

    auto solidDetector1 = new G4Box("Detector1", 0.5 * detectorHeight, 0.5 * detectorLength, 0.05);
    auto logicDetector1 = new G4LogicalVolume(solidDetector1, galactic, "Detector1");

    auto solidDetector2 = new G4Box("Detector2", 0.5 * detectorHeight, 0.5 * detectorLength, 0.05);
    auto logicDetector2 = new G4LogicalVolume(solidDetector2, galactic, "Detector2");

    new G4PVPlacement(detectorRotation, // no rotation
                      detectorCenter1,  // at (0,0,0)
                      logicDetector1,   // its logical volume
                      "Detector1",      // its name
                      logicWorld,       // its mother  volume
                      false,            // no boolean operation
                      0,                // copy number
                      true);            // overlaps checking

    new G4PVPlacement(detectorRotation, // no rotation
                      detectorCenter2,  // at (0,0,0)
                      logicDetector2,   // its logical volume
                      "Detector2",      // its name
                      logicWorld,       // its mother  volume
                      false,            // no boolean operation
                      0,                // copy number
                      true);

    return physWorld;
}