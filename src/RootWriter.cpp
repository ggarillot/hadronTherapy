#include "RootWriter.h"

#include <CLHEP/Units/SystemOfUnits.h>
#include <G4LogicalVolumeStore.hh>

#include <TFile.h>
#include <TH1D.h>
#include <TTree.h>

void RootWriter::openRootFile(const G4String& name)
{
    if (rootFile)
    {
        G4cout << "Error : a ROOT file is already open" << G4endl;
        return;
    }

    rootFile = TFile::Open(name, "RECREATE");
    createHistograms();
}

void RootWriter::closeRootFile()
{
    histo->Write();
    histoPrimaryEnd->Write();

    tree->Write();

    rootFile->Close();
    delete rootFile;
}

void RootWriter::createHistograms()
{
    rootFile->cd();

    histo = new TH1D("histo", ";;", 10000, 0, 1800);
    histoPrimaryEnd = new TH1D("histoPrimaryEnd", ";;", 10000, 0, 1800);

    tree = new TTree("tree", "tree");

    tree->Branch("eventID", &eventID);
    tree->Branch("id", &idVec);
    tree->Branch("z", &zVec);
    tree->Branch("t", &tVec);

    tree->Branch("pdgEscaping", &pdgEscaping);
    tree->Branch("xEscaping", &xEscaping);
    tree->Branch("yEscaping", &yEscaping);
    tree->Branch("zEscaping", &zEscaping);
    tree->Branch("xMomEscaping", &xMomEscaping);
    tree->Branch("yMomEscaping", &yMomEscaping);
    tree->Branch("zMomEscaping", &zMomEscaping);
    tree->Branch("eEscaping", &eEscaping);
}

void RootWriter::fillHisto(double _z, double dE)
{
    histo->Fill(_z, dE);
}

void RootWriter::fillPrimaryEnd(double _z)
{
    histoPrimaryEnd->Fill(_z);
}

void RootWriter::fillTree(const G4int                       eID,
                          const std::vector<G4int>&         id,
                          const std::vector<G4double>&      z,
                          const std::vector<G4double>&      t,
                          const std::vector<G4int>&         pdgVec,
                          const std::vector<G4ThreeVector>& posVec,
                          const std::vector<G4ThreeVector>& momVec,
                          const std::vector<G4double>&      eVec)
{
    eventID = eID;
    idVec = id;
    zVec = z;
    tVec = t;

    pdgEscaping = pdgVec;

    xEscaping.clear();
    yEscaping.clear();
    zEscaping.clear();
    xMomEscaping.clear();
    yMomEscaping.clear();
    zMomEscaping.clear();
    eEscaping = eVec;

    for (const auto& pos : posVec)
    {
        xEscaping.push_back(pos.x());
        yEscaping.push_back(pos.y());
        zEscaping.push_back(pos.z());
    }

    for (const auto& mom : momVec)
    {
        xMomEscaping.push_back(mom.x());
        yMomEscaping.push_back(mom.y());
        zMomEscaping.push_back(mom.z());
    }

    tree->Fill();
}