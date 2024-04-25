#include "RootWriter.h"

#include <CLHEP/Units/SystemOfUnits.h>
#include <G4AnalysisManager.hh>
#include <G4RunManager.hh>
#include <G4Step.hh>
#include <G4ios.hh>

#include "Settings.h"
#include "TrackInformation.h"

RootWriter::RootWriter(const Settings& settings)
    : settings(settings)
{
    analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetVerboseLevel(0);

    const auto runManager = G4RunManager::GetRunManager();
    const auto type = runManager->GetRunManagerType();

    if (type != G4RunManager::sequentialRM)
        analysisManager->SetNtupleMerging(true);
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

    if (!settings.minimalTreeForTransverseGammas)
    {
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

        // general nuclei position
        analysisManager->CreateNtupleIColumn(id_tree, "nucleiA", nucleiA);
        analysisManager->CreateNtupleIColumn(id_tree, "nucleiZ", nucleiZ);
        analysisManager->CreateNtupleFColumn(id_tree, "nucleiXPos", nucleiXPos);
        analysisManager->CreateNtupleFColumn(id_tree, "nucleiYPos", nucleiYPos);
        analysisManager->CreateNtupleFColumn(id_tree, "nucleiZPos", nucleiZPos);

        analysisManager->CreateNtupleIColumn(id_tree, "pdgEsc", pdgEscaping);
    }

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

    if (settings.beamTree)
    {
        id_beamPosX = analysisManager->CreateNtupleFColumn(id_tree, "beamX");
        id_beamPosY = analysisManager->CreateNtupleFColumn(id_tree, "beamY");
        id_beamPosZ = analysisManager->CreateNtupleFColumn(id_tree, "beamZ");
        id_beamMomX = analysisManager->CreateNtupleFColumn(id_tree, "beamPX");
        id_beamMomY = analysisManager->CreateNtupleFColumn(id_tree, "beamPY");
        id_beamMomZ = analysisManager->CreateNtupleFColumn(id_tree, "beamPZ");
        id_beamEnergy = analysisManager->CreateNtupleFColumn(id_tree, "beamE");
    }

    analysisManager->FinishNtuple(id_tree);
}

void RootWriter::setEventNumber(const G4int eventNumber)
{
    if (settings.minimalTreeForTransverseGammas)
        return;
    analysisManager->FillNtupleIColumn(id_tree, id_eventID, eventNumber);
}

void RootWriter::addEdep(const CLHEP::Hep3Vector& pos, const double dE)
{
    analysisManager->FillH2(id_edepHisto, pos.z() / CLHEP::mm, pos.x() / CLHEP::mm, dE / CLHEP::MeV);
}

void RootWriter::setPrimaryEnd(const G4ThreeVector pos)
{
    if (settings.minimalTreeForTransverseGammas)
        return;
    analysisManager->FillNtupleFColumn(id_tree, id_primaryEndX, pos.x() / CLHEP::mm);
    analysisManager->FillNtupleFColumn(id_tree, id_primaryEndY, pos.y() / CLHEP::mm);
    analysisManager->FillNtupleFColumn(id_tree, id_primaryEndZ, pos.z() / CLHEP::mm);
}

void RootWriter::addPositronEmitter(const G4ParticleDefinition* particleDefinition,
                                    const G4ThreeVector&        position,
                                    const G4double              time)
{
    if (settings.minimalTreeForTransverseGammas)
        return;
    if (!particleDefinition)
        return;

    const auto A = particleDefinition->GetBaryonNumber();
    const auto Z = particleDefinition->GetAtomicNumber();

    AVec.push_back(A);
    ZVec.push_back(Z);
    xVec.push_back(position.x() / CLHEP::mm);
    yVec.push_back(position.y() / CLHEP::mm);
    zVec.push_back(position.z() / CLHEP::mm);
    tVec.push_back(time / CLHEP::s);
}

void RootWriter::addEscapingParticle(const G4Step* step)
{
    const auto postStepPoint = step->GetPostStepPoint();

    const auto pdg = step->GetTrack()->GetDefinition()->GetPDGEncoding();

    if (settings.minimalTreeForTransverseGammas && pdg != 22)
        return;

    const auto energy = step->GetPostStepPoint()->GetTotalEnergy() / CLHEP::MeV;
    const auto pos = postStepPoint->GetPosition() / CLHEP::mm;
    const auto mom = postStepPoint->GetMomentumDirection();
    const auto time = postStepPoint->GetGlobalTime() / CLHEP::second;

    const auto theta = mom.theta();
    if (settings.minimalTreeForTransverseGammas && (theta < 85 * CLHEP::deg || theta > 95 * CLHEP::deg))
        return;

    pdgEscaping.push_back(pdg);
    eEscaping.push_back(energy);
    timeEscaping.push_back(time);

    xEscaping.push_back(pos.x());
    yEscaping.push_back(pos.y());
    zEscaping.push_back(pos.z());

    thetaEscaping.push_back(theta);
    phiEscaping.push_back(mom.phi());

    const auto trackInfo = static_cast<const TrackInformation*>(step->GetTrack()->GetUserInformation());

    const auto initialPosition = trackInfo->initialPosition / CLHEP::mm;

    initialXEscaping.push_back(initialPosition.x());
    initialYEscaping.push_back(initialPosition.y());
    initialZEscaping.push_back(initialPosition.z());
}

void RootWriter::addBeamProperties(const CLHEP::Hep3Vector& pos, const CLHEP::Hep3Vector& mom, const G4double energy)
{
    if (!settings.beamTree)
        return;

    analysisManager->FillNtupleFColumn(id_tree, id_beamPosX, pos.x() / CLHEP::mm);
    analysisManager->FillNtupleFColumn(id_tree, id_beamPosY, pos.y() / CLHEP::mm);
    analysisManager->FillNtupleFColumn(id_tree, id_beamPosZ, pos.z() / CLHEP::mm);
    analysisManager->FillNtupleFColumn(id_tree, id_beamMomX, mom.x());
    analysisManager->FillNtupleFColumn(id_tree, id_beamMomY, mom.y());
    analysisManager->FillNtupleFColumn(id_tree, id_beamMomZ, mom.z());
    analysisManager->FillNtupleFColumn(id_tree, id_beamEnergy, energy / CLHEP::MeV);
}

void RootWriter::addStepLength(const G4double stepLength)
{
    // stepLengthHisto->Fill(stepLength);
}

void RootWriter::addNuclei(const G4ParticleDefinition* particleDefinition, const G4ThreeVector& position)
{
    if (settings.minimalTreeForTransverseGammas)
        return;

    nucleiA.push_back(particleDefinition->GetBaryonNumber());
    nucleiZ.push_back(particleDefinition->GetAtomicNumber());
    nucleiXPos.push_back(position.x() / CLHEP::mm);
    nucleiYPos.push_back(position.y() / CLHEP::mm);
    nucleiZPos.push_back(position.z() / CLHEP::mm);
}

void RootWriter::fillTree()
{
    analysisManager->AddNtupleRow(id_tree);

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
