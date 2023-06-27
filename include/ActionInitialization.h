#pragma once

#include <G4VUserActionInitialization.hh>
#include <globals.hh>

class ActionInitialization : public G4VUserActionInitialization
{
  public:
    ActionInitialization(G4String particleName, G4String beamEnergy, G4int seed);
    ~ActionInitialization() = default;

    void BuildForMaster() const override;
    void Build() const override;

  protected:
    G4String baseRootFileName{};

    G4String particleName{};
    G4double beamEnergy{};
    G4int    seed{};
};
