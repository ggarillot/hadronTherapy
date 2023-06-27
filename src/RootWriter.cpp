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
}

void RootWriter::fillHisto(double _z, double dE)
{
    histo->Fill(_z, dE);
}

void RootWriter::fillPrimaryEnd(double _z)
{
    histoPrimaryEnd->Fill(_z);
}

void RootWriter::fillTree(const G4int                  eID,
                          const std::vector<G4int>&    id,
                          const std::vector<G4double>& z,
                          const std::vector<G4double>& t)
{
    eventID = eID;
    idVec = id;
    zVec = z;
    tVec = t;
    tree->Fill();
}