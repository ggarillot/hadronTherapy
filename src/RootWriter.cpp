#include "RootWriter.h"

#include <CLHEP/Units/SystemOfUnits.h>
#include <G4AnalysisManager.hh>
#include <G4RunManager.hh>
#include <G4Step.hh>
#include <G4ios.hh>

#include "TrackInformation.h"

RootWriter::RootWriter()
{
    analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetVerboseLevel(0);

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
    id_edepHisto = analysisManager->CreateH2("hs", "Deposited energy", 1000, 0, 300, 1000, -150, 150);
    analysisManager->SetH2XAxisTitle(id_edepHisto, "z(mm)");
    analysisManager->SetH2YAxisTitle(id_edepHisto, "x(mm)");

    id_tree = analysisManager->CreateNtuple("tree", "tree");
    id_eventID = analysisManager->CreateNtupleIColumn(id_tree, "eventID");
    id_primaryEndX = analysisManager->CreateNtupleFColumn(id_tree, "primaryEndX");
    id_primaryEndY = analysisManager->CreateNtupleFColumn(id_tree, "primaryEndY");
    id_primaryEndZ = analysisManager->CreateNtupleFColumn(id_tree, "primaryEndZ");

    // positron emission
    analysisManager->CreateNtupleIColumn(id_tree, "A", AVec);
    analysisManager->CreateNtupleIColumn(id_tree, "Z", ZVec);
    analysisManager->CreateNtupleFColumn(id_tree, "x", xVec);
    analysisManager->CreateNtupleFColumn(id_tree, "y", yVec);
    analysisManager->CreateNtupleFColumn(id_tree, "z", zVec);
    analysisManager->CreateNtupleFColumn(id_tree, "t", tVec);

    analysisManager->CreateNtupleIColumn(id_tree, "pdgEsc", pdgEscaping);
    analysisManager->CreateNtupleFColumn(id_tree, "xEsc", xEscaping);
    analysisManager->CreateNtupleFColumn(id_tree, "yEsc", yEscaping);
    analysisManager->CreateNtupleFColumn(id_tree, "zEsc", zEscaping);
    analysisManager->CreateNtupleFColumn(id_tree, "thetaEsc", thetaEscaping);
    analysisManager->CreateNtupleFColumn(id_tree, "phiEsc", phiEscaping);
    analysisManager->CreateNtupleFColumn(id_tree, "eEsc", eEscaping);
    analysisManager->CreateNtupleFColumn(id_tree, "timeEsc", timeEscaping);
    analysisManager->CreateNtupleFColumn(id_tree, "initialXEsc", initialXEscaping);
    analysisManager->CreateNtupleFColumn(id_tree, "initialYEsc", initialYEscaping);
    analysisManager->CreateNtupleFColumn(id_tree, "initialZEsc", initialZEscaping);
    analysisManager->CreateNtupleIColumn(id_tree, "nucleiA", nucleiA);
    analysisManager->CreateNtupleIColumn(id_tree, "nucleiZ", nucleiZ);
    analysisManager->CreateNtupleFColumn(id_tree, "nucleiXPos", nucleiXPos);
    analysisManager->CreateNtupleFColumn(id_tree, "nucleiYPos", nucleiYPos);
    analysisManager->CreateNtupleFColumn(id_tree, "nucleiZPos", nucleiZPos);

    analysisManager->FinishNtuple(id_tree);

    id_beamTree = analysisManager->CreateNtuple("beamTree", "beamTree");

    id_beamPosX = analysisManager->CreateNtupleFColumn(id_beamTree, "x");
    id_beamPosY = analysisManager->CreateNtupleFColumn(id_beamTree, "y");
    id_beamPosZ = analysisManager->CreateNtupleFColumn(id_beamTree, "z");
    id_beamMomX = analysisManager->CreateNtupleFColumn(id_beamTree, "momX");
    id_beamMomY = analysisManager->CreateNtupleFColumn(id_beamTree, "momY");
    id_beamMomZ = analysisManager->CreateNtupleFColumn(id_beamTree, "momZ");
    id_beamEnergy = analysisManager->CreateNtupleFColumn(id_beamTree, "e");

    analysisManager->FinishNtuple(id_beamTree);

    // analysisManager->SetH2Activation(id_edepHisto, false);
    // analysisManager->SetNtupleActivation(id_tree, false);
    // analysisManager->SetNtupleActivation(id_beamTree, false);
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
    analysisManager->FillNtupleFColumn(id_tree, id_primaryEndX, pos.x());
    analysisManager->FillNtupleFColumn(id_tree, id_primaryEndY, pos.y());
    analysisManager->FillNtupleFColumn(id_tree, id_primaryEndZ, pos.z());
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

void RootWriter::addBeamProperties(const CLHEP::Hep3Vector& pos, const CLHEP::Hep3Vector& mom, const G4double energy)
{
    analysisManager->FillNtupleFColumn(id_beamTree, id_beamPosX, pos.x());
    analysisManager->FillNtupleFColumn(id_beamTree, id_beamPosY, pos.y());
    analysisManager->FillNtupleFColumn(id_beamTree, id_beamPosZ, pos.z());
    analysisManager->FillNtupleFColumn(id_beamTree, id_beamMomX, mom.x());
    analysisManager->FillNtupleFColumn(id_beamTree, id_beamMomY, mom.y());
    analysisManager->FillNtupleFColumn(id_beamTree, id_beamMomZ, mom.z());
    analysisManager->FillNtupleFColumn(id_beamTree, id_beamEnergy, energy);
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
