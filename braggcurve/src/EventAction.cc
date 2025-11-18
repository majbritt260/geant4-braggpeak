//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file EventAction.cc
/// \brief Implementation of the B4c::EventAction class

#include "EventAction.hh"
#include "CalorimeterSD.hh"
#include "CalorHit.hh"

#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>

#include <fstream>

namespace B4c
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CalorHitsCollection*
EventAction::GetHitsCollection(G4int hcID,
                                  const G4Event* event) const
{
  auto hitsCollection
    = static_cast<CalorHitsCollection*>(
        event->GetHCofThisEvent()->GetHC(hcID));

  if ( ! hitsCollection ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hitsCollection ID " << hcID;
    G4Exception("EventAction::GetHitsCollection()",
      "MyCode0003", FatalException, msg);
  }

  return hitsCollection;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void EventAction::BeginOfEventAction(const G4Event* event)
{

  // Clear txt file on first event
  if (event->GetEventID() == 0) {
      std::ofstream outFile("data.txt", std::ios::trunc);
      outFile << "EventID;tEnergy(keV);IonYield\n";  // Write header
      outFile.close();
     }
}

void EventAction::EndOfEventAction(const G4Event* event)
{
  // Get hits collection ID (only once)
  if ( fSensitiveDetectorHCID == -1 ) {
    fSensitiveDetectorHCID = G4SDManager::GetSDMpointer()->GetCollectionID("SensitiveDetectorHitsCollection");
  }

  // Get hits collection for the SensitiveDetector
  auto SensitiveDetectorHC = GetHitsCollection(fSensitiveDetectorHCID, event);

  // Get hit with total values (since we have only one SensitiveDetector, we use index 0)
  auto SensitiveDetectorHit = (*SensitiveDetectorHC)[0];
  // if multipe entries use
  //auto SensitiveDetectorHit = (*SensitiveDetectorHC)[SensitiveDetectorHC->entries()-1];



  // Print per event (modulo n)
  //
  auto eventID = event->GetEventID();
  auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
  if ( ( printModulo > 0 ) && ( eventID % printModulo == 0 ) ) {
    G4cout << "---> End of event: " << eventID << G4endl;

  G4cout << "   SensitiveDetector: total energy: "
         << std::setw(7) << G4BestUnit(SensitiveDetectorHit->GetEdep(), "Energy")
         << "       total track length: "
         << std::setw(7) << G4BestUnit(SensitiveDetectorHit->GetTrackLength(), "Length")
         << G4endl;
}

  // Fill histograms, ntuple
  //

  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // Fill histograms for SensitiveDetector
  analysisManager->FillH1(0, SensitiveDetectorHit->GetEdep());
  analysisManager->FillH1(1, SensitiveDetectorHit->GetTrackLength());

  // Fill ntuple for SensitiveDetector
  analysisManager->FillNtupleDColumn(0, SensitiveDetectorHit->GetEdep());
  analysisManager->FillNtupleDColumn(1, SensitiveDetectorHit->GetTrackLength());
  analysisManager->AddNtupleRow();


  // Fill in txt file for SensitiveDetector
  // Open file in append mode
  std::ofstream outFile("data.txt", std::ios::app);

  if (outFile.is_open()) {
      outFile << eventID << ";"  // Event number
              << SensitiveDetectorHit->GetEdep() / CLHEP::keV << ";"  // Convert energy to eV
              << SensitiveDetectorHit->GetIonYield() << "\n";  // Cluster size
  } else {
      G4cerr << "Error opening file for writing!" << G4endl;
  }

  outFile.close();  // Close the file after writing



}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}

