#ifndef MOLLEROptPhysicsList_h
#define MOLLEROptPhysicsList_h 1

#include "G4OpWLS.hh"
#include "G4Cerenkov.hh"
#include "G4Scintillation.hh"

#include "G4OpMieHG.hh"
#include "G4OpRayleigh.hh"
#include "G4OpAbsorption.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4LossTableManager.hh"
#include "G4EmSaturation.hh"

#include "MOLLEROptOpBoundaryProcess.hh"

#include "G4VPhysicsConstructor.hh"

// user includes
#include "MOLLEROptPhysicsListMessenger.hh"
#include "MOLLEROptMaterial.hh"


// user classes
class MOLLEROptPhysicsListMessenger;

class MOLLEROptPhysicsList: public G4VPhysicsConstructor
{
public:

  MOLLEROptPhysicsList(G4bool toggle=true);
  ~MOLLEROptPhysicsList();

  void SetVerbose(G4int) {};

  virtual void ConstructParticle();
  virtual void ConstructProcess();
  
  G4OpWLS* GetWLSProcess() {return theWLSProcess;}
  G4Cerenkov* GetCerenkovProcess() {return theCerenkovProcess;}
  G4Scintillation* GetScintillationProcess() {return theScintProcess;}
  G4OpAbsorption* GetAbsorptionProcess() {return theAbsorptionProcess;}
  G4OpRayleigh* GetRayleighScatteringProcess() {return theRayleighScattering;}
  G4OpMieHG* GetMieHGScatteringProcess() {return theMieHGScatteringProcess;}
  G4OpBoundaryProcess* GetBoundaryProcess() { return theBoundaryProcess;}
  
  void SetNbOfPhotonsCerenkov(G4int);
  
  
private:
  
  G4OpWLS*             theWLSProcess;
  G4Cerenkov*          theCerenkovProcess;
  G4Scintillation*     theScintProcess;
  G4OpAbsorption*      theAbsorptionProcess;
  G4OpRayleigh*        theRayleighScattering;
  G4OpMieHG*           theMieHGScatteringProcess;
  G4OpBoundaryProcess* theBoundaryProcess;
  
  G4bool AbsorptionOn;
  
  
};

#endif

