#pragma once

#include <G4ThreeVector.hh>
#include <globals.hh>
#include <vector>

class TTree;
class TH1D;
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

    void fillHisto(double z, double dEdX);
    void setPrimaryEnd(const G4ThreeVector pos);

    void addPositronEmitter(const G4int Z, const G4double z, const G4double time);
    void addEscapingParticle(const G4Step* step);

    void addDetectedParticle(const G4Step* step);

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

    TTree* detectorTree = nullptr;

    int    detected_PDG{};
    double detected_Y1{};
    double detected_Z1{};
    double detected_Y2{};
    double detected_Z2{};
    double detected_initX{};
    double detected_initY{};
    double detected_initZ{};
    double detected_time{};
    double detected_energy{};

    TH1D* histo = nullptr;
};