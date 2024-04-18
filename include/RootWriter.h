#pragma once

#include <G4AnalysisManager.hh>

class G4ParticleDefinition;
class G4Step;

class RootWriter
{
  public:
    RootWriter();
    virtual ~RootWriter() = default;

    void openRootFile(const G4String& name = "test.root");
    void closeRootFile();

    void setEventNumber(const G4int eventNumber);

    void addEdep(const CLHEP::Hep3Vector& pos, const double dE);
    void setPrimaryEnd(const G4ThreeVector pos);

    void addPositronEmitter(const G4ParticleDefinition* particleDefinition,
                            const G4ThreeVector&        position,
                            const G4double              time);

    void addEscapingParticle(const G4Step* step);

    void addBeamProperties(const CLHEP::Hep3Vector& pos, const CLHEP::Hep3Vector& mom, const G4double energy);

    void addStepLength(const G4double stepLength);

    void addNuclei(const G4ParticleDefinition* particleDefinition, const G4ThreeVector& position);

    void fillTree();

  protected:
    void createHistograms();

  protected:
    G4AnalysisManager* analysisManager = nullptr;

    G4int id_edepHisto{};
    G4int id_tree{};

    G4int id_eventID{};
    G4int id_primaryEndX{};
    G4int id_primaryEndY{};
    G4int id_primaryEndZ{};

    std::vector<int>   AVec{};
    std::vector<int>   ZVec{};
    std::vector<float> xVec{};
    std::vector<float> yVec{};
    std::vector<float> zVec{};
    std::vector<float> tVec{};

    std::vector<int>   pdgEscaping{};
    std::vector<float> xEscaping{};
    std::vector<float> yEscaping{};
    std::vector<float> zEscaping{};
    std::vector<float> thetaEscaping{};
    std::vector<float> phiEscaping{};
    std::vector<float> eEscaping{};
    std::vector<float> timeEscaping{};
    std::vector<float> initialXEscaping{};
    std::vector<float> initialYEscaping{};
    std::vector<float> initialZEscaping{};
    std::vector<int>   nucleiA{};
    std::vector<int>   nucleiZ{};
    std::vector<float> nucleiXPos{};
    std::vector<float> nucleiYPos{};
    std::vector<float> nucleiZPos{};

    G4int id_beamTree{};

    G4int id_beamPosX{};
    G4int id_beamPosY{};
    G4int id_beamPosZ{};
    G4int id_beamMomX{};
    G4int id_beamMomY{};
    G4int id_beamMomZ{};
    G4int id_beamEnergy{};
};