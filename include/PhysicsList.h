#pragma once

#include "G4VModularPhysicsList.hh"

class G4VPhysicsConstructor;
class HadrontherapyStepMax;
class HadrontherapyPhysicsListMessenger;

class PhysicsList : public G4VModularPhysicsList
{
  public:
    PhysicsList();
    virtual ~PhysicsList();

    void ConstructParticle() override;
    void ConstructProcess() override;

  private:
    std::vector<G4VPhysicsConstructor*> physVec;
};
