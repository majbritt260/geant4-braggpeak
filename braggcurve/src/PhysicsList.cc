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
// This example is provided by the Geant4-DNA collaboration
// Any report or published results obtained using the Geant4-DNA software
// shall cite the following Geant4-DNA collaboration publications:
// Phys. Med. 31 (2015) 861-874
// Med. Phys. 37 (2010) 4692-4708
// The Geant4-DNA web site is available at http://geant4-dna.org
//
/// \file PhysicsList.cc
/// \brief Implementation of the PhysicsList class

#include "PhysicsList.hh"
#include "PhysicsListMessenger.hh"

#include "G4EmDNAPhysics.hh"
#include "G4EmDNAPhysics_option1.hh"
#include "G4EmDNAPhysics_option2.hh"
#include "G4EmDNAPhysics_option3.hh"
#include "G4EmDNAPhysics_option4.hh"
#include "G4EmDNAPhysics_option5.hh"
#include "G4EmDNAPhysics_option6.hh"
#include "G4EmDNAPhysics_option7.hh"
#include "G4EmDNAPhysics_option8.hh"

#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"
#include "G4EmStandardPhysics_option4.hh"

#include "G4UserSpecialCuts.hh"
#include "G4StepLimiter.hh"

// hadronics

#include "G4hMultipleScattering.hh"
#include "G4WentzelVIModel.hh"
#include "G4CoulombScattering.hh"
#include "G4hIonisation.hh"
#include "G4BraggModel.hh"
#include "G4BetheBlochModel.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4HadronInelasticProcess.hh"
#include "G4BinaryCascade.hh"
#include "G4HadronPhysicsINCLXX.hh"
#include "G4BGGNucleonInelasticXS.hh"  // Cross-section for protons

// particles

#include "G4Proton.hh"
#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BosonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4ShortLivedConstructor.hh"
#include "G4DNAGenericIonsManager.hh"

//  configurate EM models for particles/processes/regions
#include "G4EmConfigurator.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList::PhysicsList() : G4VModularPhysicsList(),
  fEmPhysicsList(0), fMessenger(0)
{
  fMessenger = new PhysicsListMessenger(this);

  SetVerboseLevel(1);

  // EM physics
  fEmPhysicsList = new G4EmDNAPhysics_option4();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList::~PhysicsList()
{
  delete fMessenger;
  delete fEmPhysicsList;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::ConstructParticle()
{
    G4BosonConstructor  pBosonConstructor;
    pBosonConstructor.ConstructParticle();

    G4LeptonConstructor pLeptonConstructor;
    pLeptonConstructor.ConstructParticle();

    G4MesonConstructor pMesonConstructor;
    pMesonConstructor.ConstructParticle();

    G4BaryonConstructor pBaryonConstructor;
    pBaryonConstructor.ConstructParticle();

    G4IonConstructor pIonConstructor;
    pIonConstructor.ConstructParticle();

    G4ShortLivedConstructor pShortLivedConstructor;
    pShortLivedConstructor.ConstructParticle();

    G4DNAGenericIonsManager* genericIonsManager;
    genericIonsManager=G4DNAGenericIonsManager::Instance();
    genericIonsManager->GetIon("alpha++");
    genericIonsManager->GetIon("alpha+");
    genericIonsManager->GetIon("helium");
    genericIonsManager->GetIon("hydrogen");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::ConstructProcess()
{
  // transportation
  //
  AddTransportation();

  // electromagnetic physics list
  //
  fEmPhysicsList->ConstructProcess();


  // user-defined hadronic processes

  if (fEmName.find("hadCustom") != std::string::npos) {

    // Lower energy thresholds only for this physics list -- set to 100 keV prior to implementation --> didnt seem to change anything in regards to ionization yield
    //auto emParams = G4EmParameters::Instance();
    //emParams->SetStepFunction(0.05, 0.001 * CLHEP::mm);  // ~1 µm max step
    //emParams->SetLowestElectronEnergy(100 *  CLHEP::eV);
    //emParams->SetLowestMuHadEnergy(100 * CLHEP::eV);
    //emParams->SetMinEnergy(10 * CLHEP::eV);

    G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();	// physics helper helps add physics processes to particles

    auto particleIterator = GetParticleIterator();				// get list of all defined particles in simulation
    particleIterator->reset();

    while ((*particleIterator)()) {						// loop through all particles
      G4ParticleDefinition* particle = particleIterator->value();			// get current particle
      G4String particleName = particle->GetParticleName();			// get name as string

      if (particleName == "proton") {						// add hadronic processes only for protons

        // Multiple Scattering -- small angles
        G4hMultipleScattering* msc = new G4hMultipleScattering();
        msc->SetEmModel(new G4WentzelVIModel());				// 0 eV - 100 TeV
        ph->RegisterProcess(msc, particle);

        // Coulomb Scattering -- large angles
        G4CoulombScattering* pcou = new G4CoulombScattering();			// 0 eV - 100 TeV
        ph->RegisterProcess(pcou, particle);

        // Ionization
        G4hIonisation* hion = new G4hIonisation();				// dE/dx and range tables from 100 eV
        hion->SetEmModel(new G4BraggModel());					// for low-energy protons 0 eV - 2 MeV
        hion->SetEmModel(new G4BetheBlochModel());				// for high-energy protons 2 MeV - 100 TeV
        ph->RegisterProcess(hion, particle);

	// High-Precision Elastic Scattering
	G4HadronElasticPhysics* hadElastic = new G4HadronElasticPhysics();	// 0 eV - 100 TeV
	hadElastic->ConstructProcess();  // Ensure elastic physics is applied	// 0 eV - 100 TeV

	// Binary Cascade for Inelastic Interactions
	G4HadronInelasticProcess* inelasticProcess = new G4HadronInelasticProcess("protonInelastic", G4Proton::Definition());
	G4BinaryCascade* bicModel = new G4BinaryCascade();
	inelasticProcess->RegisterMe(bicModel);
	inelasticProcess->AddDataSet(new G4BGGNucleonInelasticXS(G4Proton::Proton())); // assign proper cross-section for inelastic hadron interactions
	ph->RegisterProcess(inelasticProcess, particle);
      }
    }
  }

  // tracking cut
  //
  AddTrackingCut();

  // maximum step size
  //
  AddMaxStepSize();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::AddPhysicsList(const G4String& name)
{
  if (verboseLevel>-1) {
    G4cout << "PhysicsList::AddPhysicsList: <" << name << ">" << G4endl;
  }

  if (name == fEmName) return;

  if (name == "dna") {
    fEmName = name;
    delete fEmPhysicsList;
    fEmPhysicsList = new G4EmDNAPhysics();

  } else if (name == "dna_opt1") {
    fEmName = name;
    delete fEmPhysicsList;
    fEmPhysicsList = new G4EmDNAPhysics_option1();

  } else if (name == "dna_opt2") {
    fEmName = name;
    delete fEmPhysicsList;
    fEmPhysicsList = new G4EmDNAPhysics_option2();

  } else if (name == "dna_opt3") {
    fEmName = name;
    delete fEmPhysicsList;
    fEmPhysicsList = new G4EmDNAPhysics_option3();

  } else if (name == "dna_opt4") {
    fEmName = name;
    delete fEmPhysicsList;
    fEmPhysicsList = new G4EmDNAPhysics_option4();

  } else if (name == "dna_opt5") {
    fEmName = name;
    delete fEmPhysicsList;
    fEmPhysicsList = new G4EmDNAPhysics_option5();

  } else if (name == "dna_opt6") {
    fEmName = name;
    delete fEmPhysicsList;
    fEmPhysicsList = new G4EmDNAPhysics_option6();

  } else if (name == "dna_opt7") {
    fEmName = name;
    delete fEmPhysicsList;
    fEmPhysicsList = new G4EmDNAPhysics_option7();

  } else if (name == "dna_opt8") {
    fEmName = name;
    delete fEmPhysicsList;
    fEmPhysicsList = new G4EmDNAPhysics_option8();

  } else if (name == "liv") {
    fEmName = name;
    delete fEmPhysicsList;
    fEmPhysicsList = new G4EmLivermorePhysics();

  } else if (name == "pene") {
    fEmName = name;
    delete fEmPhysicsList;
    fEmPhysicsList = new G4EmPenelopePhysics();

  } else if (name == "emstandard_opt4") {
    fEmName = name;
    delete fEmPhysicsList;
    fEmPhysicsList = new G4EmStandardPhysics_option4();

  } else if (name == "emStd4_hadCustom") {
    fEmName = name;
    delete fEmPhysicsList;
    fEmPhysicsList = new G4EmStandardPhysics_option4();

  } else if (name == "penelope_hadCustom") {
    fEmName = name;
    delete fEmPhysicsList;
    fEmPhysicsList = new G4EmPenelopePhysics();

  } else {

    G4cout << "PhysicsList::AddPhysicsList: <" << name << ">"
           << " is not defined"
           << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::AddTrackingCut()
{

  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();

  auto particleIterator=GetParticleIterator();
  particleIterator->reset();
  while ((*particleIterator)())
  {
    G4ParticleDefinition* particle = particleIterator->value();
    G4String particleName = particle->GetParticleName();

    if (particleName == "e-")
    {
      ph->RegisterProcess(new G4UserSpecialCuts(), particle);
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::AddMaxStepSize()
{

  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();

  auto particleIterator=GetParticleIterator();
  particleIterator->reset();
  while ((*particleIterator)())
  {
    G4ParticleDefinition* particle = particleIterator->value();
    G4String particleName = particle->GetParticleName();

    if (particleName == "e-")
    {
      ph->RegisterProcess(new G4StepLimiter(), particle);
    }
  }
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
