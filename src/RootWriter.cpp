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
    tree->Branch("thetaEscaping", &thetaEscaping);
    tree->Branch("phiEscaping", &phiEscaping);
    tree->Branch("eEscaping", &eEscaping);
    tree->Branch("timeEscaping", &timeEscaping);

    tree->Branch("initialX", &initialX);
    tree->Branch("initialY", &initialY);
    tree->Branch("initialZ", &initialZ);
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
                          const std::vector<G4double>&      eVec,
                          const std::vector<G4double>&      timeVec,
                          const std::vector<G4ThreeVector>  initPosVec)
{
    eventID = eID;
    idVec = id;
    zVec = z;
    tVec = t;

    pdgEscaping = pdgVec;

    xEscaping.clear();
    yEscaping.clear();
    zEscaping.clear();
    thetaEscaping.clear();
    phiEscaping.clear();

    initialX.clear();
    initialY.clear();
    initialZ.clear();

    eEscaping = eVec;
    timeEscaping = timeVec;

    for (const auto& pos : posVec)
    {
        xEscaping.push_back(pos.x());
        yEscaping.push_back(pos.y());
        zEscaping.push_back(pos.z());
    }

    for (const auto& mom : momVec)
    {
        thetaEscaping.push_back(mom.theta());
        phiEscaping.push_back(mom.phi());
    }

    for (const auto& initPos : initPosVec)
    {
        initialX.push_back(initPos.x());
        initialY.push_back(initPos.y());
        initialZ.push_back(initPos.z());
    }

    tree->Fill();
}