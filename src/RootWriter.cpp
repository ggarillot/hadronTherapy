#include "RootWriter.h"
#include "TrackInformation.h"

#include <CLHEP/Units/SystemOfUnits.h>
#include <G4LogicalVolumeStore.hh>
#include <G4Step.hh>
#include <G4Track.hh>

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

    tree->Write();

    detectorTree->Write();

    rootFile->Close();
    delete rootFile;
}

void RootWriter::createHistograms()
{
    rootFile->cd();

    histo = new TH1D("histo", ";;", 10000, 0, 1800);

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

    detectorTree = new TTree("detector", "detector");

    detectorTree->Branch("PDG", &detected_PDG);
    detectorTree->Branch("y1", &detected_Y1);
    detectorTree->Branch("z1", &detected_Z1);
    detectorTree->Branch("y2", &detected_Y2);
    detectorTree->Branch("z2", &detected_Z2);
    detectorTree->Branch("initX", &detected_initX);
    detectorTree->Branch("initY", &detected_initY);
    detectorTree->Branch("initZ", &detected_initZ);
    detectorTree->Branch("time", &detected_time);
    detectorTree->Branch("energy", &detected_energy);
}

void RootWriter::setEventNumber(const G4int eventNumber)
{
    eventID = eventNumber;
}

void RootWriter::fillHisto(double _z, double dE)
{
    histo->Fill(_z, dE);
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

void RootWriter::addDetectedParticle(const G4Step* step)
{
    const auto trackInfo = dynamic_cast<const TrackInformation*>(step->GetTrack()->GetUserInformation());
    const auto postStepPoint = step->GetPostStepPoint();

    const auto detectedPosition1 = trackInfo->detected1.value() / CLHEP::mm;
    const auto detectedPosition2 = trackInfo->detected2.value() / CLHEP::mm;

    detected_PDG = step->GetTrack()->GetParticleDefinition()->GetPDGEncoding();
    detected_Y1 = detectedPosition1.y();
    detected_Z1 = detectedPosition1.z();
    detected_Y2 = detectedPosition2.y();
    detected_Z2 = detectedPosition2.z();

    const auto initialPosition = trackInfo->initialPosition / CLHEP::mm;

    detected_initX = initialPosition.x();
    detected_initY = initialPosition.y();
    detected_initZ = initialPosition.z();

    detected_time = postStepPoint->GetGlobalTime() / CLHEP::second;
    detected_energy = postStepPoint->GetTotalEnergy();

    detectorTree->Fill();
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
}