#include "MOLLEROptDetector.hh"

MOLLEROptDetector::MOLLEROptDetector(MOLLEROptTrackingReadout *TrRO, G4String type, MOLLEROptMaterial* mat)
{
  DesignParms = new DesignParameters;

  TrackingReadout = TrRO;
  DetType = type;
  Materials = mat;
  DetMaterial = Materials->GetMaterial("Air");  

  Quartz = new  MOLLEROptDetectorQuartz(TrackingReadout,type,Materials);
  LightGuide = new  MOLLEROptDetectorLightGuide(TrackingReadout,type,Materials);
  PMT = new  MOLLEROptDetectorPMT(TrackingReadout,type,Materials,LightGuide);     
  detMessenger = NULL;

  MotherVolume = NULL;
  
  DetLogical  = NULL;
  DetPhysical = NULL;
  DetSolid = NULL;

  RotationDet = NULL;

  fReadFile ="../data/December2021/GDML/Ring5/R5-Module-Al.gdml";
  fWriteFile="wtest.gdml";
  // fStepFile ="mbb";


}

MOLLEROptDetector::~MOLLEROptDetector()
{
  delete Quartz;
  delete LightGuide;
  delete PMT;
  //  delete Structure;
  //delete Messenger;
}

void MOLLEROptDetector::SetQuartzSizeX(G4double x)
{
  if(Quartz)
    Quartz->SetQuartzSizeX(x);

  DesignParms->QuartzSizeX = x;
}

void MOLLEROptDetector::SetQuartzSizeY(G4double y)
{
  if(Quartz)
    Quartz->SetQuartzSizeY(y);

  DesignParms->QuartzSizeY = y;
    
}
				       
void MOLLEROptDetector::SetQuartzSizeZ(G4double z)
{
  if(Quartz)
    Quartz->SetQuartzSizeZ(z);

  DesignParms->QuartzSizeZ = z;
    
}

				       
void MOLLEROptDetector::SetQuartzRotX(G4double rX)
{
  if(Quartz)
    Quartz->SetQuartzRotX(rX);

  //need to counter rotate the entire detector, so that the events
  //still hit the detector at normal incidence.

  //RotationDet->rotateX(-rX);
  DesignParms->QuartzRotation = rX/degree;
}

void MOLLEROptDetector::SetQuartzBevel(G4double bev)
{
  if(Quartz)
    Quartz->SetBevelSize(bev);
  
  DesignParms->QuartzBevel = bev;
}


void MOLLEROptDetector::SetLowerInterfacePlane(G4double LowerPlane)
{
  if(LightGuide)
    LightGuide->SetLowerInterfacePlane(LowerPlane);

  DesignParms->LowerInterfacePlane = LowerPlane;
}

void MOLLEROptDetector::SetMiddleBoxHeight(G4double MiddlePlane)
{
  if(LightGuide)
    LightGuide->SetMiddleBoxHeight(MiddlePlane);

  DesignParms->MiddleBoxHeight = MiddlePlane;
}

void MOLLEROptDetector::SetUpperInterfacePlane(G4double UpperPlane)
{
  if(LightGuide)
    LightGuide->SetUpperInterfacePlane(UpperPlane);

  DesignParms->UpperInterfacePlane = UpperPlane;
}

void MOLLEROptDetector::SetLowerConeFrontFaceAngle(G4double angle)
{
  if(LightGuide)
    LightGuide->SetLowerConeFrontFaceAngle(angle);

  DesignParms->LowerConeFrontFaceAngle = angle;
}

void MOLLEROptDetector::SetLowerConeBackFaceAngle(G4double angle)
{
  if(LightGuide)
    LightGuide->SetLowerConeBackFaceAngle(angle);

  DesignParms->LowerConeBackFaceAngle = angle;
}

void MOLLEROptDetector::SetLowerConeSideFaceAngle(G4double angle)
{  
  if(LightGuide){
    LightGuide->SetLowerConeSideFaceAngle(angle);
  }
  DesignParms->LowerConeSideFaceAngle = angle;
}

void MOLLEROptDetector::SetQuartzInterfaceOpeningZ(G4double size)
{
  if(LightGuide)
    LightGuide->SetQuartzInterfaceOpeningZ(size);
  DesignParms->QuartzInterfaceOpeningZ = size;

}

void MOLLEROptDetector::SetQuartzInterfaceOpeningX(G4double size)
{
  if(LightGuide)
    LightGuide->SetQuartzInterfaceOpeningX(size);
  DesignParms->QuartzInterfaceOpeningX = size;
}

void MOLLEROptDetector::SetPMTInterfaceOpeningZ(G4double size)
{
  if(LightGuide)
    LightGuide->SetPMTInterfaceOpeningZ(size);
  if(PMT)
    PMT->SetLGInterfaceOpeningZ(size);

  DesignParms->PMTInterfaceOpeningZ = size;

}

void MOLLEROptDetector::SetPMTInterfaceOpeningX(G4double size)
{
  if(LightGuide)
    LightGuide->SetPMTInterfaceOpeningX(size);
  if(PMT)
    PMT->SetLGInterfaceOpeningX(size);

  DesignParms->PMTInterfaceOpeningX = size;
}


void MOLLEROptDetector::SetQuartzToPMTOffsetInZ(G4double val)
{
  PMTToQuartzOffset = val;
  if(LightGuide)
    LightGuide->SetQuartzToPMTOffsetInZ(val);
  if(PMT)
    PMT->SetCenterPositionInZ(val);

  DesignParms->QuartzToPMTOffsetInZ = val;
}



void MOLLEROptDetector::SetAzimuthalRotationAngle(G4double val)
{
  AzimuAngle = val; 
}

void MOLLEROptDetector::SetPolarRotationAngle(G4double val)
{
  PolarAngle = val;

  DesignParms->DetectorRotation = val/degree;

}

void MOLLEROptDetector::SetPMTCathodeThickness(G4double val)
{
  if(PMT)
    PMT->SetCathodeThickness(val);
}

void MOLLEROptDetector::SetPMTCathodeRadius(G4double val)
{
  if(PMT)
    PMT->SetCathodeRadius(val);
  if(LightGuide)
    LightGuide->SetPMTOpeningRadius(val);
}





void MOLLEROptDetector::UpdateThisGeometry()
{
  G4LogicalVolume *mLog;

  G4GeometryManager::GetInstance()->OpenGeometry();
  
  delete DetPhysical;
  RotationDet->rotateX(PolarAngle);  
  PMT->UpdateGeometry();
  Quartz->UpdateGeometry();
  LightGuide->UpdateGeometry();
  CalculateDimensions();
  DetSolid = new G4Box(DetType+"_Solid",
		       0.5* DetFullLengthX, 
		       0.5* DetFullLengthY,
		       0.5* DetFullLengthZ);
  DetLogical = new G4LogicalVolume( DetSolid, DetMaterial, DetType+"_Logical");
  DetLogical->SetSolid(DetSolid);
  ResetCenterLocation();
  ConstructDetector(MotherVolume);
  //ConstructMountingStructure(MotherVolume);
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
   
}

void MOLLEROptDetector::CalculateDimensions()
{
  if(LightGuide->GetLightGuideWidth() > 2*PMT->GetRadius())
    DetFullLengthX = LightGuide->GetLightGuideWidth() + 1.0*cm;
  else
    DetFullLengthX = 2*PMT->GetRadius() +1.0*cm;
  
  if(LightGuide->GetLightGuideDepth() > 2*PMT->GetRadius())
    DetFullLengthZ = LightGuide->GetLightGuideDepth() + 2*PMTToQuartzOffset + 4.0*cm;
  else
    DetFullLengthZ = 2*PMT->GetRadius() + 2*PMTToQuartzOffset + 4.0*cm;
  
  DetFullLengthY = Quartz->GetQuartzSizeY()+LightGuide->GetLightGuideLength()+PMT->GetPMTLength()+1.0*cm+LightGuide->GetCurrentMiddleBoxHeight();
}

void MOLLEROptDetector::ResetCenterLocation()
{
  PositionDetX = 0.0*cm;
  PositionDetY = 0.0*cm;//DetFullLengthY/2 - Quartz->GetQuartzSizeY()/2;
  PositionDetZ = 0.0*cm;

  PositionDet.setX(PositionDetX);
  PositionDet.setY(PositionDetY);
  PositionDet.setZ(PositionDetZ);  
}

void MOLLEROptDetector::Initialize()
{
  //let these objects setup their default solids and logical volumes
  PMTToQuartzOffset = 0;
  AzimuAngle = 0; 
  PolarAngle = 0;
  Quartz->Initialize();
  LightGuide->Initialize();
  PMT->Initialize();
  
  // G4double quartzY = Quartz->GetQuartzSizeY();
  // G4double quartzZ = Quartz->GetQuartzSizeZ();  
  // G4double lguideY = LightGuide->GetCurrentUpperInterfacePlane();

  // Quartz->SetCenterPositionInY(-0.5*DetFullLengthY+quartzY/2.0 + 5*mm);
  // LightGuide->SetCenterPositionInY(-0.5*DetFullLengthY+quartzY + 5*mm);// + 0.5*LightGuide->GetCurrentQuartzInterfaceOpeningY()*TMath::Sin(Qrot));
  // PMT->SetCenterPositionInY(-0.5*DetFullLengthY+quartzY+lguideY+PMT->GetPMTLength()/2.0 + 5.0*mm);// + 0.5*LightGuide->GetCurrentQuartzInterfaceOpeningY()*TMath::Sin(Qrot));

  CalculateDimensions();
  ResetCenterLocation();

  RotationDet = new G4RotationMatrix;

  DetSolid = new G4Box(DetType+"_Solid",
		       0.5* DetFullLengthX, 
		       0.5* DetFullLengthY,
		       0.5* DetFullLengthZ);
  
  DetLogical = new G4LogicalVolume( DetSolid, DetMaterial, DetType+"_Logical");

}

G4VPhysicalVolume* MOLLEROptDetector::ConstructDetector(G4VPhysicalVolume* Mother)
{
  if(!Mother) return 0;
  
  if(!MotherVolume){
    MotherVolume = Mother;
  }

  DetPhysical = new G4PVPlacement(RotationDet,
				  PositionDet,   
				  DetType+"_Physical",
				  DetLogical,
				  MotherVolume,false,1);    
  
  G4double quartzY = Quartz->GetQuartzSizeY();
  G4double quartzZ = Quartz->GetQuartzSizeZ();  
  G4double lguideY = LightGuide->GetCurrentUpperInterfacePlane();

  G4double Qrot = Quartz->GetQuartzRotationX();
     
  Quartz->Construct(DetPhysical);
  Quartz->SetCenterPositionInZ(0.5*quartzY*(TMath::Sin(Qrot)));
  Quartz->SetCenterPositionInY(-0.5*DetFullLengthY + 0.5*quartzY + 0.5*quartzY*(1.0-TMath::Cos(Qrot)) + 0.5*quartzZ*fabs(TMath::Sin(Qrot)) + 5*mm);
  LightGuide->Construct(DetPhysical);
  LightGuide->SetCenterPositionInY(-0.5*DetFullLengthY+quartzY + 5*mm);// + 0.5*LightGuide->GetCurrentQuartzInterfaceOpeningY()*TMath::Sin(Qrot));
  PMT->Construct(DetPhysical);
  // We have to let the PMT extend into the light guide lsig
  PMT->SetCenterPositionInY(-0.5*DetFullLengthY+quartzY+lguideY+PMT->GetPMTLength()/2.0 + 5.0*mm+LightGuide->GetCurrentMiddleBoxHeight());// + 0.5*LightGuide->GetCurrentQuartzInterfaceOpeningY()*TMath::Sin(Qrot));

  G4Colour  grey      ( 127/255., 127/255., 127/255.);
  G4VisAttributes *att = new G4VisAttributes(grey);
  att->SetVisibility(true);
  att->SetForceWireframe(true);
  DetLogical->SetVisAttributes(att);

  if(!detMessenger)
    detMessenger = new MOLLEROptDetectorMessenger(this);  


  return DetPhysical;
} 

void MOLLEROptDetector::ConstructMountingStructure(G4VPhysicalVolume* Mother)
{
  gdmlParser.Read(fReadFile);

  G4LogicalVolume *logV = gdmlParser.GetVolume("R5-Module-Aluminum");

  if(logV){

    G4RotationMatrix  *Rot1 = new G4RotationMatrix;
   Rot1->rotateZ(180*degree);
  G4double quartzY = Quartz->GetQuartzSizeY();
  
  // cout << "Quartz center position X: " << Quartz->GetCurrentCenterPositionInX() << endl;
  // cout << "Quartz center position Y: " << Quartz->GetCurrentCenterPositionInY() << "Set = " << -0.5*DetFullLengthY+quartzY/2.0 + 5*mm << endl;
  // cout << "Quartz center position Z: " << Quartz->GetCurrentCenterPositionInZ() << endl;

  G4ThreeVector Qpos = Quartz->GetCurrentCenterPosition();
  // cout << "Quartz center position X: " << Qpos.x() << endl;
  // cout << "Quartz center position Y: " << Qpos.y() << endl;
  // cout << "Quartz center position Z: " << Qpos.z() << endl;
  // cout << "Polar Angle = " << PolarAngle << endl;
  G4double dz = 2*fabs(Qpos.y())*TMath::Sin(PolarAngle/2)*TMath::Cos(PolarAngle/2);
  G4double dy = 2*fabs(Qpos.y())*TMath::Sin(PolarAngle/2)*TMath::Sin(PolarAngle/2);
  // cout << "dz: " << dz << endl;
  // cout << "dy: " << dy << endl;
  
  G4ThreeVector Trans1 = G4ThreeVector(Qpos.x(),Qpos.y()+dy,Qpos.z()+dz);
  // cout << "Quartz center position X: " << Trans1.x() << endl;
  // cout << "Quartz center position Y: " << Trans1.y() << endl;
  // cout << "Quartz center position Z: " << Trans1.z() << endl;
  
  G4ThreeVector LGpos = LightGuide->GetCurrentCenterPosition();
  // cout << "Guide center position X: " << LGpos.x() << endl;
  // cout << "Guide center position Y: " << LGpos.y() << endl;
  // cout << "Guide center position Z: " << LGpos.z() << endl;
    
  G4VPhysicalVolume* MPhys = new G4PVPlacement(Rot1,
					       Trans1,   
					       DetType+"_logV",
					       logV ,
					       Mother,false,1); 
  
    G4Colour green    (   0/255., 255/255.,   0/255.);
    G4VisAttributes *att2 = new G4VisAttributes(green);
    att2->SetVisibility(true);
    //att2->SetForceWireframe(true);
    logV->SetVisAttributes(att2);

    G4ThreeVector MPos = MPhys->GetTranslation();
    // cout << "Mount center position X: " << MPos.x() << endl;
    // cout << "Mount center position Y: " << MPos.y() << endl;
    // cout << "Mount center position Z: " << MPos.z() << endl;
    
  }  
}



void MOLLEROptDetector::SetMaterial(G4String materialName)
{
  DetMaterial = Materials->GetMaterial(materialName);
}

void MOLLEROptDetector::SetCenterPositionInX(G4double xPos)
{
    PositionDetX = xPos;	 

    DetPhysical->SetTranslation(G4ThreeVector(PositionDetX,
					      PositionDetY, 
					      PositionDetZ));
}

void MOLLEROptDetector::SetCenterPositionInY(G4double yPos)
{
    PositionDetY = yPos;

    DetPhysical->SetTranslation(G4ThreeVector(PositionDetX,
					      PositionDetY, 
					      PositionDetZ));
}

void MOLLEROptDetector::SetCenterPositionInZ(G4double zPos)
{
    PositionDetZ = zPos;

    DetPhysical->SetTranslation(G4ThreeVector(PositionDetX,
					      PositionDetY, 
					      PositionDetZ));
}

void MOLLEROptDetector::SetLightGuideOffsetInX(G4double x)
{
  G4double cX;
  
  if(LightGuide &&  PMT){

    cX = LightGuide->GetCurrentCenterPositionInX() + x*cm;
    
    LightGuide->SetCenterPositionInX(cX);

    cX = PMT->GetCurrentCenterPositionInX() + x*cm;

    PMT->SetCenterPositionInX(cX);
  }
}

void MOLLEROptDetector::SetLightGuideOffsetInY(G4double y)
{
  G4double cY;
  
  if(LightGuide &&  PMT){

    cY = LightGuide->GetCurrentCenterPositionInY() + y*cm;
    
    LightGuide->SetCenterPositionInY(cY);

    cY = PMT->GetCurrentCenterPositionInY() + y*cm;

    PMT->SetCenterPositionInY(cY);
  }
}

void MOLLEROptDetector::SetLightGuideOffsetInZ(G4double z)
{
  G4double cZ;
  
  if(LightGuide &&  PMT){

    cZ = LightGuide->GetCurrentCenterPositionInZ() + z*cm;
    
    LightGuide->SetCenterPositionInZ(cZ);

    cZ = PMT->GetCurrentCenterPositionInZ() + z*cm;

    PMT->SetCenterPositionInZ(cZ);
  }
}


void MOLLEROptDetector::GetQuartzLimits(G4double *vals)
{
  Quartz->GetQuartzLimits(vals);
}

void MOLLEROptDetector::GetLightGuideLimits(G4double *vals)
{
  LightGuide->GetLightGuideLimits(vals);
}


DesignParameters*  MOLLEROptDetector::GetDesignParameters()
{
  return DesignParms;
}
