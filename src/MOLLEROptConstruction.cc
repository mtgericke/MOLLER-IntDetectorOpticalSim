#include "MOLLEROptConstruction.hh"

MOLLEROptConstruction::MOLLEROptConstruction(MOLLEROptTrackingReadout *trReadout, MOLLEROptMaterial* mat) 
  : G4VUserDetectorConstruction()
{
  World_Solid    = NULL;  
  World_Logical  = NULL;  
  World_Physical = NULL;
  
  MOLLERMessenger = NULL;
  Materials       = mat;

  Detector        = NULL;
  LightGuide      = NULL;

  fWorldLengthInX = 0.0; 
  fWorldLengthInY = 0.0;
  fWorldLengthInZ = 0.0;

  TrackingReadout = trReadout;

  MOLLERMessenger = new MOLLEROptMessenger(this);
}

MOLLEROptConstruction::~MOLLEROptConstruction()
{
  if (Detector)           delete Detector;
  //if (Material)         delete Material;
  if (MOLLERMessenger)    delete MOLLERMessenger;             
}

G4VPhysicalVolume* MOLLEROptConstruction::Construct()
{
  Detector   = new MOLLEROptDetector(TrackingReadout,"Ring5",Materials);
  //DetMessenger = new MOLLEROptDetectorMessenger(Detector);
  
  fWorldLengthInX =  15.0*m;
  fWorldLengthInY =  15.0*m;
  fWorldLengthInZ =  30.0*m;
  
  World_Material   = Materials->GetMaterial("Vacuum");
  
  World_Solid = new G4Box("World_Solid",
			  0.5* fWorldLengthInX , 
			  0.5* fWorldLengthInY ,
			  0.5* fWorldLengthInZ );
  
  World_Logical = new G4LogicalVolume( World_Solid, 
				       World_Material, 
				       "World_Logical", 
				       0, 0, 0);
  World_Physical = new G4PVPlacement(0, // no rotation
				     G4ThreeVector(), // at (0,0,0)
				     World_Logical,  
				     "World_Physical", 
				     0,false,0);             
  
  // Detector->ConstructComponent(World_Physical);
  // Detector->SetCenterPositionInZ(0.0*cm);
  // LightGuide->ConstructComponent(World_Physical);
  // LightGuide->SetCenterPositionInY(7.5*cm);

  Detector->SetQuartzSizeX(8.4*cm);
  Detector->SetQuartzSizeY(15*cm);  //without the 45 degree cut region
  Detector->SetQuartzSizeZ(1.5*cm);
  Detector->SetLowerInterfacePlane(5.6*cm);
  Detector->SetUpperInterfacePlane(25*cm);
  Detector->SetLowerConeFrontFaceAngle(28*degree); //degrees
  Detector->SetLowerConeBackFaceAngle(22*degree);
  Detector->SetLowerConeSideFaceAngle(0*degree);
  Detector->SetQuartzInterfaceOpeningZ(1.8*cm);
  Detector->SetQuartzInterfaceOpeningX(9.2*cm);
  Detector->SetPMTInterfaceOpeningZ(5.6*cm);
  Detector->SetPMTInterfaceOpeningX(5.6*cm);  
  Detector->SetQuartzToPMTOffsetInZ(-0.3*cm);
  Detector->SetPMTCathodeRadius(3.81*cm);
  Detector->SetPMTCathodeThickness(0.1*cm);
  Detector->SetAzimuthalRotationAngle(0);
  Detector->SetPolarRotationAngle(0);
  Detector->Initialize();
  Detector->ConstructDetector(World_Physical);
  Detector->SetCenterPositionInY(12*cm);

  World_Logical->SetVisAttributes (G4VisAttributes::GetInvisible());
  
  DumpGeometricalTree(World_Physical);
  
  return World_Physical;
}

void MOLLEROptConstruction::GetQuartzLimits(G4double *vals)
{
  Detector->GetQuartzLimits(vals);
}

void MOLLEROptConstruction::GetLightGuideLimits(G4double *vals)
{
  Detector->GetLightGuideLimits(vals);
}


void MOLLEROptConstruction::DumpGeometricalTree(G4VPhysicalVolume* aVolume,G4int depth)
{
  for(int isp=0;isp<depth;isp++)
    { G4cout << "  "; }
  G4cout << aVolume->GetName() << "[" << aVolume->GetCopyNo() << "] "
         << aVolume->GetLogicalVolume()->GetName() << " "
         << aVolume->GetLogicalVolume()->GetNoDaughters() << " "
	 << aVolume->GetLogicalVolume()->GetMaterial()->GetName();
  if(aVolume->GetLogicalVolume()->GetSensitiveDetector())
  {
    G4cout << " " << aVolume->GetLogicalVolume()->GetSensitiveDetector()
                            ->GetFullPathName();
  }
  G4cout << G4endl;
  for(int i=0;i<aVolume->GetLogicalVolume()->GetNoDaughters();i++){
    
    
    DumpGeometricalTree(aVolume->GetLogicalVolume()->GetDaughter(i),depth+1);


  }
}


void MOLLEROptConstruction::UpdateGeometry()
{
  G4GeometryManager::GetInstance()->OpenGeometry();

  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();
  G4LogicalBorderSurface::CleanSurfaceTable();


  G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

