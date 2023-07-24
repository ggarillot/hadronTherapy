#pragma once

#include <G4ThreeVector.hh>
#include <globals.hh>
#include <vector>

class TTree;
class TH1D;
class TFile;

class RootWriter
{
  public:
    void openRootFile(const G4String& name = "test.root");
    void closeRootFile();

    void fillHisto(double z, double dEdX);
    void fillPrimaryEnd(double _z);

    void fillTree(const G4int                       eventID,
                  const std::vector<G4int>&         id,
                  const std::vector<G4double>&      z,
                  const std::vector<G4double>&      t,
                  const std::vector<G4int>&         pdgVec,
                  const std::vector<G4ThreeVector>& posVec,
                  const std::vector<G4ThreeVector>& momVec,
                  const std::vector<G4double>&      eVec);

  protected:
    void createHistograms();

  protected:
    TFile* rootFile = nullptr;

    TTree* tree = nullptr;

    G4int               eventID{};
    std::vector<int>    idVec{};
    std::vector<double> zVec{};
    std::vector<double> tVec{};

    // TODO
    std::vector<int>    pdgEscaping{};
    std::vector<double> xEscaping{};
    std::vector<double> yEscaping{};
    std::vector<double> zEscaping{};
    std::vector<double> xMomEscaping{};
    std::vector<double> yMomEscaping{};
    std::vector<double> zMomEscaping{};
    std::vector<double> eEscaping{};

    TH1D* histo = nullptr;
    TH1D* histoPrimaryEnd = nullptr;
};