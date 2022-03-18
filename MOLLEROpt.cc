#include "G4Version.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4DecayPhysics.hh"

#include "QGSP_BERT_HP.hh"
#include "FTFP_BERT.hh"
#include "G4PhysListFactory.hh"
#include "G4StepLimiterPhysics.hh"
#include "G4OpticalPhysics.hh"
#include "G4EmStandardPhysics_option4.hh"

#include "MOLLEROptAnalysis.hh"
#include "MOLLEROptPhysicsList.hh"
#include "MOLLEROptPrimaryGeneratorAction.hh"
#include "MOLLEROptConstruction.hh"
#include "MOLLEROptEventAction.hh"
#include "MOLLEROptRunAction.hh"
#include "MOLLEROptSteppingAction.hh"
#include "MOLLEROptTrackingAction.hh"
#include "MOLLEROptTrackingReadout.hh"
#include "MOLLEROptMaterial.hh"
#include "MOLLEROptTrackingReadout.hh"

#if G4VERSION_NUMBER >= 1070
#include "G4OpticalParameters.hh"
#endif

#include "G4RunManager.hh"

#include "G4VUserPhysicsList.hh"

#include <G4UImanager.hh>
#include <G4UIExecutive.hh>
#include <G4UIterminal.hh>
//#include <UVA_VisAction.hh>
#include "G4VisExecutive.hh"


#ifdef G4UI_USE_QT
#include "G4UIQt.hh"
#endif

#ifdef G4UI_USE_XM
#include "G4UIXm.hh"
#endif

#ifdef G4UI_USE_TCSH
#include "G4UItcsh.hh"
#endif

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif


int main(int argc,char** argv) {


  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());
  
  // Run manager
  G4RunManager * runManager = new G4RunManager();

  // UserInitialization classes (mandatory)
  MOLLEROptTrackingReadout*       thisMOLLEROptTrackingReadout  = new MOLLEROptTrackingReadout();
  MOLLEROptMaterial *Materials = new MOLLEROptMaterial(thisMOLLEROptTrackingReadout);
  Materials->DefineMaterials();
  MOLLEROptConstruction*          thisMOLLEROptExperiment       = new MOLLEROptConstruction(thisMOLLEROptTrackingReadout,Materials);
  MOLLEROptAnalysis*              thisMOLLEROptAnalysis         = new MOLLEROptAnalysis(thisMOLLEROptExperiment);
  
  runManager->SetUserInitialization(thisMOLLEROptExperiment);

  G4int verbose = 3;

  G4VModularPhysicsList* physicsList = new FTFP_BERT;
  physicsList->ReplacePhysics(new G4EmStandardPhysics_option4());
  G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();

  #if G4VERSION_NUMBER >= 1070
  G4OpticalParameters* params = G4OpticalParameters::Instance();
  params->SetWLSTimeProfile("delta");
  
  params->SetCerenkovMaxPhotonsPerStep(100);
  params->SetCerenkovMaxBetaChange(10.0);
  
  params->SetCerenkovTrackSecondariesFirst(true);
  params->SetScintTrackSecondariesFirst(true);
  #else
  opticalPhysics->SetWLSTimeProfile("delta");
  
  opticalPhysics->SetScintillationYieldFactor(1.0);
  opticalPhysics->SetScintillationExcitationRatio(0.0);
  
  opticalPhysics->SetMaxNumPhotonsPerStep(100);
  opticalPhysics->SetMaxBetaChangePerStep(10.0);
  
  opticalPhysics->SetTrackSecondariesFirst(kCerenkov, true);
  opticalPhysics->SetTrackSecondariesFirst(kScintillation, true);
  #endif
  
  physicsList->RegisterPhysics(opticalPhysics);
  runManager->SetUserInitialization(physicsList);

  
  // G4PhysListFactory factory;
  // G4VModularPhysicsList* physlist = factory.GetReferencePhysList("FTFP_BERT");//FTFP_BERT
  // physlist->RegisterPhysics(new G4StepLimiterPhysics());
  // physlist->SetVerboseLevel(verbose);
  // runManager->SetUserInitialization(physlist);
  // physlist->RegisterPhysics( new MOLLEROptPhysicsList() );
  // UserAction classes
  
  runManager->SetUserAction( new MOLLEROptPrimaryGeneratorAction(thisMOLLEROptExperiment) );
  runManager->SetUserAction( new MOLLEROptRunAction(thisMOLLEROptAnalysis, thisMOLLEROptTrackingReadout ) );  
  runManager->SetUserAction( new MOLLEROptEventAction(thisMOLLEROptAnalysis, thisMOLLEROptTrackingReadout) );
  runManager->SetUserAction( new MOLLEROptSteppingAction(thisMOLLEROptTrackingReadout) );
  runManager->SetUserAction( new MOLLEROptTrackingAction(thisMOLLEROptTrackingReadout) );

  //Initialize G4 kernel
  runManager->Initialize();

  G4UIExecutive* ui = 0;
  if ( argc == 1 ) {
    ui = new G4UIExecutive(argc, argv);
  }
  
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();
  
  G4UImanager * UIManager = G4UImanager::GetUIpointer();  
  
  if ( ! ui ) {
    // batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UIManager->ApplyCommand(command+fileName);
  }
  else {
    // interactive mode
    UIManager->ApplyCommand("/control/execute macros/gui.mac");
    ui->SessionStart();
    delete ui;
  }


  delete visManager;
  delete runManager;
  
  return 0;
}

