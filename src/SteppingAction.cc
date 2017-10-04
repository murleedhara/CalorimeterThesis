/**
 * @file /DB_EMCal/src/SteppingAction.cc
 * @author Balázs Demeter <balazsdemeter92@gmail.com>
 * @date 2017/09/15 <creation>
 * 
 * @section DESCRIPTION
 * 
 * The Geant4 simulation of EMcal's Stepping action source code for steps of simulation.
 * Latest updates of project can be found in README file.
 **/

#include "SteppingAction.hh"


/// Constructor of Stepping action

SteppingAction::SteppingAction(EventAction* eventAction)
: G4UserSteppingAction(),
  fEventAction(eventAction)
{}

/// Destructor of Stepping action

SteppingAction::~SteppingAction()
{}

/// User Stepping action for collecting datas from steps

void SteppingAction::UserSteppingAction(const G4Step* fStep)
{
  
  G4double edepStep = fStep->GetTotalEnergyDeposit();
  G4Track * fTrack = fStep->GetTrack();
  G4int trackID=fTrack->GetTrackID();
  
  if(fTrack->GetTrackStatus()!=fAlive) { return; } /// check if it is alive
  
  G4ParticleDefinition *particle=fTrack->GetDefinition();

  G4VPhysicalVolume* prevolume  =
    fStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
  G4VPhysicalVolume* postvolume  =
    fStep->GetPostStepPoint()->GetTouchableHandle()->GetVolume();

  G4double preX = fStep->GetPreStepPoint()->GetPosition().x();
  G4double preY = fStep->GetPreStepPoint()->GetPosition().y();
  G4double preZ  = fStep->GetPreStepPoint()->GetPosition().z();
  G4double prekinE  = fStep->GetPreStepPoint()->GetKineticEnergy();

  G4double postX = fStep->GetPostStepPoint()->GetPosition().x();
  G4double postY = fStep->GetPostStepPoint()->GetPosition().y();
  G4double postZ = fStep->GetPostStepPoint()->GetPosition().z();
  G4double postkinE  = fStep->GetPostStepPoint()->GetKineticEnergy();
  
  G4int copynum = fStep->GetPostStepPoint()->GetTouchableHandle()->GetCopyNumber();
  G4int copynumpre = fStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber();

  G4ThreeVector momentumdirection = fTrack->GetMomentumDirection();
  G4double theta = momentumdirection.theta();

  G4String particleName = particle->GetParticleName();
  G4String preName=prevolume->GetName();
  G4String postName=postvolume->GetName();
  G4String procN;
  if((fTrack->GetCreatorProcess()!=0)&&(postName!="World"))
  {
  procN=fTrack->GetCreatorProcess()->GetProcessName();
  
  fEventAction->fHistoManager->FillHisto(1,postX,postY,postZ,edepStep);
  
  
  if(((postName=="fiberInterior")||(postName=="fiberCover"))&&((preName=="fiberInterior")||(preName=="fiberCover")))
  {
	  fEventAction->fHistoManager->FillHisto(2,postX,postY,postZ,postkinE);
  }
  if(preName=="Detector")
  {
	  fTrack->SetTrackStatus(fStopAndKill);  
  }
   
  }
}

/// End of file

