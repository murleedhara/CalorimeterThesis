/**
 * @file /DB_EMCal/src/ActionInitialization.cc
 * @author Balázs Demeter <balazsdemeter92@gmail.com>
 * @date 2017/09/15 <creation>
 * 
 * @section DESCRIPTION
 * 
 * The Geant4 simulation of EMcal's action initialization source code. 
 * Latest updates of project can be found in README file.
 **/

#include "ActionInitialization.hh"


/** @brief Constructor of Action initialization
 *
 *  @param e0 		Kinetic energy of particle
 *  @param Particle Type of particle
 * 
 **/

ActionInitialization::ActionInitialization(G4double e0, G4String Particle)
 : G4VUserActionInitialization()
{
	fParticle=Particle;
	fEnergy=e0;
}

/// @brief Destructor of Action initialization

ActionInitialization::~ActionInitialization()
{}

/// @brief Setting run action 

void ActionInitialization::BuildForMaster() const
{
  
  SetUserAction(new RunAction);
}

/// @brief Build void 

void ActionInitialization::Build() const
{
  
  SetUserAction(new PrimaryGeneratorAction(fEnergy,fParticle));
  SetUserAction(new RunAction);
  
  EventAction* eventAction = new EventAction;
  SetUserAction(eventAction);
  
  SetUserAction(new SteppingAction(eventAction));
}  

/// End of file
