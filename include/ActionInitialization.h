#pragma once

#include <G4Types.hh>
#include <G4VUserActionInitialization.hh>
#include <globals.hh>

#include "Settings.h"

class ActionInitialization : public G4VUserActionInitialization
{
  public:
    ActionInitialization(const Settings& settings);
    ~ActionInitialization() = default;

    void BuildForMaster() const override;
    void Build() const override;

  protected:
    G4String baseRootFileName{};

    G4String particleName{};
    G4double beamEnergy{};
    G4int    seed{};
    Settings settings{};
};
