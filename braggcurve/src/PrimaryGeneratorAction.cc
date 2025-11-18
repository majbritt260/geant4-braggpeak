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
/// \file PrimaryGeneratorAction.cc
/// \brief Implementation of the B4::PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"
#include "G4Event.hh"

// For GPS
#include "G4GeneralParticleSource.hh"
#include "G4SPSPosDistribution.hh"
#include "G4SPSAngDistribution.hh"
#include "G4SPSEneDistribution.hh"

#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

namespace B4
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Constructor
PrimaryGeneratorAction::PrimaryGeneratorAction()
{
  fParticleGun = new G4GeneralParticleSource();

  // Set default particle as  proton
  auto particleDef = G4ParticleTable::GetParticleTable()->FindParticle("proton");
  fParticleGun->SetParticleDefinition(particleDef);

  // Set default mono energy
  auto eneDist = fParticleGun->GetCurrentSource()->GetEneDist();
  eneDist->SetEnergyDisType("Mono");
  eneDist->SetMonoEnergy(100 * MeV);

  // Set default direction
  auto angDist = fParticleGun->GetCurrentSource()->GetAngDist();
  angDist->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));

  // Default position will be updated dynamically in GeneratePrimaries()
  auto posDist = fParticleGun->GetCurrentSource()->GetPosDist();
  posDist->SetPosDisType("Point");

  // This function is called at the beginning of event

  // In order to avoid dependence of PrimaryGeneratorAction
  // on DetectorConstruction class we get world volume
  // from G4LogicalVolumeStore
  //
  G4double worldZHalfLength = 0.;
  auto worldLV = G4LogicalVolumeStore::GetInstance()->GetVolume("World");


  // Check that the world volume has cylindrical  shape
  G4Tubs* worldCylinder = nullptr;
  if (worldLV){
    worldCylinder = dynamic_cast<G4Tubs*>(worldLV->GetSolid());
  }

  if (worldCylinder){
    worldZHalfLength = worldCylinder->GetZHalfLength();
  }
  else{
    G4ExceptionDescription msg;
    msg << "World volume of cylindrical shape not found." << G4endl;
    msg << "Perhaps you have changed geometry." << G4endl;
    msg << "The gun will be placed in the center.";
    G4Exception("PrimaryGeneratorAction::GeneratePrimaries()",
      "MyCode0002", JustWarning, msg);
  }

  // Set gun position
  fParticleGun->GetCurrentSource()->GetPosDist()->SetCentreCoords(G4ThreeVector(0., 0., -worldZHalfLength));

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // This function is called at the beginning of event

  // Generate particle and assign it to the event
  fParticleGun->GeneratePrimaryVertex(anEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
