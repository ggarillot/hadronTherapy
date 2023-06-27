#pragma once

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

    void fillTree(const G4int                  eventID,
                  const std::vector<G4int>&    id,
                  const std::vector<G4double>& z,
                  const std::vector<G4double>& t);

  protected:
    void createHistograms();

  protected:
    TFile* rootFile = nullptr;

    TTree* tree = nullptr;

    G4int               eventID{};
    std::vector<int>    idVec{};
    std::vector<double> zVec{};
    std::vector<double> tVec{};

    TH1D* histo = nullptr;
    TH1D* histoPrimaryEnd = nullptr;
};