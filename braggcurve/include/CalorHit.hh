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
/// \file CalorHit.hh
/// \brief Definition of the B4c::CalorHit class

// Prevent multiple inclusions of this headerfile: define B4cCalorHit_h if it is not already defined
#ifndef B4cCalorHit_h
#define B4cCalorHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4Threading.hh"

namespace B4c
{

/// Calorimeter hit class
///
/// It defines data members to store the the energy deposit and track lengths
/// of charged particles in a selected volume:
/// - fEdep, fTrackLength

// Inheret Calorhit from G4VHit
class CalorHit : public G4VHit
{
  public:
    CalorHit(); 				// Constructor
    CalorHit(const CalorHit&) = default;	// Default copy constructor
    ~CalorHit() override;			// Destructor overiding G4VHit's destructor

    // Overloading operators --> change them in only specific contexts?
    CalorHit& operator=(const CalorHit&) = default;
    G4bool operator==(const CalorHit&) const;

    // Custom memory management --> overide 'new' and 'delete'
    inline void* operator new(size_t);
    inline void  operator delete(void*);

    // Overwrite Base class methods
    void Draw()  override{}		// Empty function, could be implemented later
    void Print() override;		// Print method, implemented in .cc file

    // Data handling methods
    void Add(G4double de, G4double dl, G4int dnIon);	// Declare Add method for  deposited energy, track length and number of ionizations
    G4double GetEdep() const;				// Declare method to return stored energy deposit
    G4double GetTrackLength() const;			// Declare method to return stored track length
    G4int GetIonYield() const;				// Declare method to return stored ionization yield

  private:
    G4double fEdep = 0.;        ///< Energy deposit in the sensitive volume
    G4double fTrackLength = 0.; ///< Track length in the  sensitive volume
    G4int fIonYield = 0.;	///< Ionization yield in the sensitive volume
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Define a type alias for collection of CalorHit objects
using CalorHitsCollection = G4THitsCollection<CalorHit>;

// Declare a thread-local allocator for efficient hit memory management
extern G4ThreadLocal G4Allocator<CalorHit>* CalorHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Custom ´new´ operator using Geant4 allocator
inline void* CalorHit::operator new(size_t)
{
  if (!CalorHitAllocator) {
    CalorHitAllocator = new G4Allocator<CalorHit>;
  }
  void *hit;
  hit = (void *) CalorHitAllocator->MallocSingle();
  return hit;
}

// Custom ´delete´ operator using Geant4 allocator
inline void CalorHit::operator delete(void *hit)
{
  if (!CalorHitAllocator) {
    CalorHitAllocator = new G4Allocator<CalorHit>;
  }
  CalorHitAllocator->FreeSingle((CalorHit*) hit);
}

// Adds deposited energy and track length to the existing values
inline void CalorHit::Add(G4double de, G4double dl, G4int dnIon) {
  fEdep += de;
  fTrackLength += dl;
  fIonYield += dnIon;
}

// Returns stored energy deposit in hit
inline G4double CalorHit::GetEdep() const {
  return fEdep;
}

// Returns stored track length of the particle in the hit
inline G4double CalorHit::GetTrackLength() const {
  return fTrackLength;

}
// Returns stored ionization yield in hit
inline G4int CalorHit::GetIonYield() const {
  return fIonYield;
}

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// End the #ifndef check
#endif
