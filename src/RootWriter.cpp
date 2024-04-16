#include "RootWriter.h"

#include <G4AnalysisManager.hh>
#include <G4RunManager.hh>
#include <G4Step.hh>

#include "TrackInformation.h"

RootWriter::RootWriter()
{
    analysisManager = G4AnalysisManager::Instance();

    const auto runManager = G4RunManager::GetRunManager();
    const auto type = runManager->GetRunManagerType();

    if (type != G4RunManager::sequentialRM)
        analysisManager->SetNtupleMerging(true);

    analysisManager->SetActivation(true);
}

void RootWriter::openRootFile(const G4String& name)
{
    analysisManager->OpenFile(name);
    createHistograms();
}

void RootWriter::closeRootFile()
{
    analysisManager->Write();
    analysisManager->CloseFile();
}

void RootWriter::createHistograms()
{
    id_edepHisto = analysisManager->CreateH2("hs", "Deposited energy;z (mm);x (mm)", 1000, 0, 300, 1000, -150, 150);

    id_tree = analysisManager->CreateNtuple("tree", "tree");
    id_eventID = analysisManager->CreateNtupleIColumn(id_tree, "eventID");
    id_primaryEndX = analysisManager->CreateNtupleDColumn(id_tree, "primaryEndX");
    id_primaryEndY = analysisManager->CreateNtupleDColumn(id_tree, "primaryEndY");
    id_primaryEndZ = analysisManager->CreateNtupleDColumn(id_tree, "primaryEndZ");

    // positron emission
    analysisManager->CreateNtupleIColumn(id_tree, "A", AVec);
    analysisManager->CreateNtupleIColumn(id_tree, "Z", ZVec);
    analysisManager->CreateNtupleDColumn(id_tree, "x", xVec);
    analysisManager->CreateNtupleDColumn(id_tree, "y", yVec);
    analysisManager->CreateNtupleDColumn(id_tree, "z", zVec);
    analysisManager->CreateNtupleDColumn(id_tree, "t", tVec);

    analysisManager->CreateNtupleIColumn(id_tree, "pdgEsc", pdgEscaping);
    analysisManager->CreateNtupleDColumn(id_tree, "xEsc", xEscaping);
    analysisManager->CreateNtupleDColumn(id_tree, "yEsc", yEscaping);
    analysisManager->CreateNtupleDColumn(id_tree, "zEsc", zEscaping);
    analysisManager->CreateNtupleDColumn(id_tree, "thetaEsc", thetaEscaping);
    analysisManager->CreateNtupleDColumn(id_tree, "phiEsc", phiEscaping);
    analysisManager->CreateNtupleDColumn(id_tree, "eEsc", eEscaping);
    analysisManager->CreateNtupleDColumn(id_tree, "timeEsc", timeEscaping);
    analysisManager->CreateNtupleDColumn(id_tree, "initialXEsc", initialXEscaping);
    analysisManager->CreateNtupleDColumn(id_tree, "initialYEsc", initialYEscaping);
    analysisManager->CreateNtupleDColumn(id_tree, "initialZEsc", initialZEscaping);
    analysisManager->CreateNtupleIColumn(id_tree, "nucleiA", nucleiA);
    analysisManager->CreateNtupleIColumn(id_tree, "nucleiZ", nucleiZ);
    analysisManager->CreateNtupleDColumn(id_tree, "nucleiXPos", nucleiXPos);
    analysisManager->CreateNtupleDColumn(id_tree, "nucleiYPos", nucleiYPos);
    analysisManager->CreateNtupleDColumn(id_tree, "nucleiZPos", nucleiZPos);

    analysisManager->FinishNtuple(id_tree);

    id_beamTree = analysisManager->CreateNtuple("beamTree", "beamTree");

    id_beamPosX = analysisManager->CreateNtupleDColumn(id_beamTree, "x");
    id_beamPosY = analysisManager->CreateNtupleDColumn(id_beamTree, "y");
    id_beamPosZ = analysisManager->CreateNtupleDColumn(id_beamTree, "z");
    id_beamMomX = analysisManager->CreateNtupleDColumn(id_beamTree, "momX");
    id_beamMomY = analysisManager->CreateNtupleDColumn(id_beamTree, "momY");
    id_beamEnergy = analysisManager->CreateNtupleDColumn(id_beamTree, "momZ");

    analysisManager->FinishNtuple(id_beamTree);
}

void RootWriter::setEventNumber(const G4int eventNumber)
{
    analysisManager->FillNtupleIColumn(id_tree, id_eventID, eventNumber);
}

void RootWriter::addEdep(const CLHEP::Hep3Vector& pos, const double dE)
{
    analysisManager->FillH2(id_edepHisto, pos.z(), pos.x(), dE);
}

void RootWriter::setPrimaryEnd(const G4ThreeVector pos)
{
    analysisManager->FillNtupleDColumn(id_tree, id_primaryEndX, pos.x());
    analysisManager->FillNtupleDColumn(id_tree, id_primaryEndY, pos.y());
    analysisManager->FillNtupleDColumn(id_tree, id_primaryEndZ, pos.z());
}

void RootWriter::addPositronEmitter(const G4ParticleDefinition* particleDefinition,
                                    const G4ThreeVector&        position,
                                    const G4double              time)
{
    if (!particleDefinition)
        return;

    const auto A = particleDefinition->GetBaryonNumber();
    const auto Z = particleDefinition->GetAtomicNumber();

    AVec.push_back(A);
    ZVec.push_back(Z);
    xVec.push_back(position.x());
    yVec.push_back(position.y());
    zVec.push_back(position.z());
    tVec.push_back(time);
}

void RootWriter::addEscapingParticle(const G4Step* step)
{
    const auto postStepPoint = step->GetPostStepPoint();

    const auto pdg = step->GetTrack()->GetDefinition()->GetPDGEncoding();
    const auto energy = step->GetPostStepPoint()->GetTotalEnergy() / CLHEP::MeV;
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

    const auto trackInfo = static_cast<const TrackInformation*>(step->GetTrack()->GetUserInformation());

    const auto initialPosition = trackInfo->initialPosition / CLHEP::mm;

    initialXEscaping.push_back(initialPosition.x());
    initialYEscaping.push_back(initialPosition.y());
    initialZEscaping.push_back(initialPosition.z());
}

void RootWriter::addBeamProperties(const CLHEP::Hep3Vector& pos, const CLHEP::Hep3Vector& mom, G4double energy)
{
    analysisManager->FillNtupleDColumn(id_beamTree, id_beamPosX, pos.x());
    analysisManager->FillNtupleDColumn(id_beamTree, id_beamPosY, pos.y());
    analysisManager->FillNtupleDColumn(id_beamTree, id_beamPosZ, pos.z());
    analysisManager->FillNtupleDColumn(id_beamTree, id_beamMomX, mom.x());
    analysisManager->FillNtupleDColumn(id_beamTree, id_beamMomY, mom.y());
    analysisManager->FillNtupleDColumn(id_beamTree, id_beamMomZ, mom.z());
    analysisManager->FillNtupleDColumn(id_beamTree, id_beamEnergy, energy);
}

void RootWriter::addStepLength(const G4double stepLength)
{
    // stepLengthHisto->Fill(stepLength);
}

void RootWriter::addNuclei(const G4ParticleDefinition* particleDefinition, const G4ThreeVector& position)
{
    nucleiA.push_back(particleDefinition->GetBaryonNumber());
    nucleiZ.push_back(particleDefinition->GetAtomicNumber());
    nucleiXPos.push_back(position.x());
    nucleiYPos.push_back(position.y());
    nucleiZPos.push_back(position.z());
}

void RootWriter::fillTree()
{
    analysisManager->AddNtupleRow(id_tree);
    analysisManager->AddNtupleRow(id_beamTree);

    AVec.clear();
    ZVec.clear();
    xVec.clear();
    yVec.clear();
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

    nucleiA.clear();
    nucleiZ.clear();
    nucleiXPos.clear();
    nucleiYPos.clear();
    nucleiZPos.clear();
}
