#pragma once

#include "ParticleMemory.h"
#include <G4Types.hh>
#include <G4UserTrackingAction.hh>

#include <map>
#include <set>

class RunAction;
class G4ParticleDefinition;
class RootWriter;

class TrackingAction : public G4UserTrackingAction
{
  public:
    TrackingAction(RootWriter* rootWriter);

    void PreUserTrackingAction(const G4Track* track) override;
    void PostUserTrackingAction(const G4Track* track) override;

    void reset();

    void printParticleMemory() const;

    void setPrintParticleMemoryMap(G4bool doPrint) { printParticleMemoryMap = doPrint; }

    G4bool doPrintParticleMemoryMap() const { return printParticleMemoryMap; }

  protected:
    RootWriter* rootWriter = nullptr;

    std::map<G4int, G4int>           childParentRelationMap{};
    std::map<G4int, std::set<G4int>> parentChildrenRelationMap{};
    std::map<G4int, ParticleMemory>  particleMemoryMap{};

    G4bool printParticleMemoryMap = false;
};