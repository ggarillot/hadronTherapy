#pragma once

#include <CLHEP/Vector/ThreeVector.h>
#include <G4ThreeVector.hh>

#include <globals.hh>
#include <vector>

class TTree;
class TH2;
class TH1;
// class THnSparse;
class TFile;

class G4Step;
class G4Track;
class TrackInformation;

class RootWriter
{
  public:
    void openRootFile(const G4String& name = "test.root");
    void closeRootFile();

    void setEventNumber(const G4int eventNumber);

    void addEdep(const CLHEP::Hep3Vector& pos, const double dE);
    void setPrimaryEnd(const G4ThreeVector pos);

    void addPositronEmitter(const G4int Z, const G4double z, const G4double time);
    void addEscapingParticle(const G4Step* step);

    void addBeamProperties(const CLHEP::Hep3Vector& pos, const CLHEP::Hep3Vector& mom);

    void addStepLength(const G4double stepLength);

    void fillTree();

  protected:
    void createHistograms();

  protected:
    TFile* rootFile = nullptr;

    TTree* tree = nullptr;

    G4int    eventID{};
    G4double primaryEndX{};
    G4double primaryEndY{};
    G4double primaryEndZ{};

    std::vector<int>    idVec{};
    std::vector<double> zVec{};
    std::vector<double> tVec{};

    // TODO
    std::vector<int>    pdgEscaping{};
    std::vector<double> xEscaping{};
    std::vector<double> yEscaping{};
    std::vector<double> zEscaping{};
    std::vector<double> thetaEscaping{};
    std::vector<double> phiEscaping{};
    std::vector<double> eEscaping{};
    std::vector<double> timeEscaping{};

    std::vector<double> initialXEscaping{};
    std::vector<double> initialYEscaping{};
    std::vector<double> initialZEscaping{};

    std::vector<int> parentEscaping{};

    TTree* beamTree = nullptr;

    double beamPosX{};
    double beamPosY{};
    double beamPosZ{};

    double beamMomX{};
    double beamMomY{};
    double beamMomZ{};

    // THnSparse* edepHisto = nullptr;
    // THnSparse* edepHistoZoom = nullptr;
    TH2* edepHisto = nullptr;

    // TH1* stepLengthHisto = nullptr;
};