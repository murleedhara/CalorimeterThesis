/**
 * @file /EMCal_MT/EMCal_MT.cc
 * @author Balázs Demeter <balazsdemeter92@gmail.com>
 * @date 2017/09/15 <creation>
 * 
 * @section DESCRIPTION
 * 
 * The Geant4 simulation of EMcal's main function. 
 * Latest updates of project can be found in README file.
 **/
 
#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"
#include "PhysicsList.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif
#include "QGSP_BIC_HP.hh"
#include "G4UImanager.hh"
#include "QBBC.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "FTFP_BERT.hh"
#include "G4StepLimiterPhysics.hh"
#include "Randomize.hh"
#include "time.h"
#include "QGSP_BIC.hh"

/**
 * @brief Start of main function
 * 
 * @param	NoE 		Command line argument for Number of events 
 * @param	Energy 		Command line argument for Kinetic energy of particle
 * @param	PhysList 	Command line argument for Hadronic physics list
 * @param	Particle	Command line argument for type of Particle
 * @param	fiber		Command line argument for number of fibers in ECal
 * @param   CutEx       Command line argument for cut of deexcitation
 * @param   RootFile    Command line argument for name of ROOT file for results
 * 
 **/

int main(int argc,char** argv)
{
  
  /// default values of parameters from command line
  unsigned int NoE=0;
  unsigned int CutEx=0;
  unsigned int nThreads = 1;
  unsigned int fiber = 2;
  G4double Energy=0.0;
  G4String PhysList="QGSP_BERT";
  G4String Particle="gamma";
  
  if (argc==8)
  {  
    NoE=atoi(argv[1]);
    Energy=atof(argv[2]);
    PhysList=argv[3];
    Particle=argv[4];
      fiber=atoi(argv[5]);
    CutEx=atoi(argv[6]);
    nThreads=atoi(argv[7]);
  }

  G4Random::setTheEngine(new CLHEP::RanecuEngine);

  G4long seed = time(NULL);
  CLHEP::HepRandom::setTheSeed(seed);

#ifdef G4MULTITHREADED
  G4MTRunManager* runManager = new G4MTRunManager;

  runManager->SetNumberOfThreads(nThreads);

#else
  G4RunManager* runManager = new G4RunManager;
#endif
  
  runManager->SetUserInitialization(new DetectorConstruction(fiber));
  runManager->SetUserInitialization(new PhysicsList(PhysList,CutEx));
  runManager->SetUserInitialization(new ActionInitialization(Energy, Particle, fiber));

  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  if (argc==8)
  {   
    /// batch
   runManager->Initialize();
   runManager->BeamOn(NoE);
   
  }
  else
  { 
    /// interactive
    G4UIExecutive* ui = 0;
    
    if ( argc == 1 )
    {
      ui = new G4UIExecutive(argc, argv);
    }
    
    UImanager->ApplyCommand("/control/execute gui.mac");
    ui->SessionStart();
    delete ui;
  }
  
  
  delete visManager; 
  delete runManager;
}

/// End of file
