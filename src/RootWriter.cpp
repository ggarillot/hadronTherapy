#include "RootWriter.h"
#include "TrackInformation.h"

#include <CLHEP/Units/SystemOfUnits.h>
#include <G4LogicalVolumeStore.hh>
#include <G4Step.hh>
#include <G4Track.hh>

#include <G4ios.hh>
#include <TAxis.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <THnSparse.h>
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
    edepHisto->Write();
    // edepHistoZoom->Write();
    // stepLengthHisto->Write();

    tree->Write();

    // beamTree->Write();

    rootFile->Purge();

    rootFile->Close();
    delete rootFile;
}

void RootWriter::createHistograms()
{
    rootFile->cd();

    // std::array<int, 3>    bins = {100, 100, 2000};
    // std::array<double, 3> xmin = {-150, -150, 0};
    // std::array<double, 3> xmax = {150, 150, 300};

    // edepHisto = new THnSparseF("hs", "", 3, bins.data(), xmin.data(), xmax.data());
    // edepHisto->GetAxis(0)->SetTitle("x");
    // edepHisto->GetAxis(1)->SetTitle("y");
    // edepHisto->GetAxis(2)->SetTitle("z");

    // bins = {300, 300, 1200};
    // xmin = {-20, -20, 0};
    // xmax = {20, 20, 180};

    // edepHistoZoom = new THnSparseF("hsZoom", "", 3, bins.data(), xmin.data(), xmax.data());
    // edepHistoZoom->GetAxis(0)->SetTitle("x");
    // edepHistoZoom->GetAxis(1)->SetTitle("y");
    // edepHistoZoom->GetAxis(2)->SetTitle("z");

    edepHisto = new TH2F("hs", "Deposited energy;z (mm);x (mm)", 3000, 0, 300, 3000, -150, 150);

    // stepLengthHisto = new TH1F("step", ";step length (mm);", 10000, 0, 30);

    tree = new TTree("tree", "tree");

    tree->Branch("eventID", &eventID);
    tree->Branch("primaryEndX", &primaryEndX);
    tree->Branch("primaryEndY", &primaryEndY);
    tree->Branch("primaryEndZ", &primaryEndZ);

    tree->Branch("id", &idVec);
    tree->Branch("z", &zVec);
    tree->Branch("t", &tVec);

    tree->Branch("pdgEsc", &pdgEscaping);
    tree->Branch("xEsc", &xEscaping);
    tree->Branch("yEsc", &yEscaping);
    tree->Branch("zEsc", &zEscaping);
    tree->Branch("thetaEsc", &thetaEscaping);
    tree->Branch("phiEsc", &phiEscaping);
    tree->Branch("eEsc", &eEscaping);
    tree->Branch("timeEsc", &timeEscaping);

    tree->Branch("initialXEsc", &initialXEscaping);
    tree->Branch("initialYEsc", &initialYEscaping);
    tree->Branch("initialZEsc", &initialZEscaping);
    tree->Branch("parentEsc", &parentEscaping);

    beamTree = new TTree("beamTree", "beamTree");

    beamTree->Branch("x", &beamPosX);
    beamTree->Branch("y", &beamPosY);
    beamTree->Branch("z", &beamPosZ);
    beamTree->Branch("momX", &beamMomX);
    beamTree->Branch("momY", &beamMomY);
    beamTree->Branch("momZ", &beamMomZ);
}

void RootWriter::setEventNumber(const G4int eventNumber)
{
    eventID = eventNumber;
}

void RootWriter::addEdep(const CLHEP::Hep3Vector& pos, const double dE)
{
    // edepHisto->Fill(pos.x(), pos.y(), pos.z(), dE);
    edepHisto->Fill(pos.z(), pos.x(), dE);
    // edepHistoZoom->Fill(pos.x(), pos.y(), pos.z(), dE);
}

void RootWriter::setPrimaryEnd(const G4ThreeVector pos)
{
    primaryEndX = pos.x();
    primaryEndY = pos.y();
    primaryEndZ = pos.z();
}

void RootWriter::addPositronEmitter(const G4int Z, const G4double z, const G4double time)
{
    idVec.push_back(Z);
    zVec.push_back(z);
    tVec.push_back(time);
}

void RootWriter::addEscapingParticle(const G4Step* step)
{
    const auto postStepPoint = step->GetPostStepPoint();

    const auto pdg = step->GetTrack()->GetDefinition()->GetPDGEncoding();
    const auto energy = step->GetPreStepPoint()->GetTotalEnergy() / CLHEP::MeV;
    const auto pos = postStepPoint->GetPosition() / CLHEP::mm;
    const auto mom = postStepPoint->GetMomentumDirection();
    const auto time = postStepPoint->GetGlobalTime() / CLHEP::second;

    pdgEscaping.push_back(pdg);
    eEscaping.push_back(energy);
    timeEscaping.push_back(time);

    xEscaping.push_back(pos.x());
    yEscaping.push_back(pos.y());
    zEscaping.push_back(pos.z());

    thetaEscaping.push_back(mom.theta());
    phiEscaping.push_back(mom.phi());

    const auto trackInfo = dynamic_cast<const TrackInformation*>(step->GetTrack()->GetUserInformation());

    const auto initialPosition = trackInfo->initialPosition / CLHEP::mm;

    initialXEscaping.push_back(initialPosition.x());
    initialYEscaping.push_back(initialPosition.y());
    initialZEscaping.push_back(initialPosition.z());

    const auto parentParticleDefinition = trackInfo->parentParticleDefinition;
    if (parentParticleDefinition)
        parentEscaping.push_back(parentParticleDefinition->GetPDGEncoding());
    else
        parentEscaping.push_back(0);
}

void RootWriter::addBeamProperties(const CLHEP::Hep3Vector& pos, const CLHEP::Hep3Vector& mom)
{
    beamPosX = pos.x();
    beamPosY = pos.y();
    beamPosZ = pos.z();
    beamMomX = mom.x();
    beamMomY = mom.y();
    beamMomZ = mom.z();
}

void RootWriter::addStepLength(const G4double stepLength)
{
    // stepLengthHisto->Fill(stepLength);
}

void RootWriter::fillTree()
{
    tree->Fill();

    idVec.clear();
    zVec.clear();
    tVec.clear();
    pdgEscaping.clear();
    xEscaping.clear();
    yEscaping.clear();
    zEscaping.clear();
    thetaEscaping.clear();
    phiEscaping.clear();
    eEscaping.clear();
    timeEscaping.clear();
    initialXEscaping.clear();
    initialYEscaping.clear();
    initialZEscaping.clear();
    parentEscaping.clear();

    beamTree->Fill();
}