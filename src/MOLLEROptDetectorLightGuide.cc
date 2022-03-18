#include "MOLLEROptDetectorLightGuide.hh"

/*
  The light guides are locally defined with the y-axis as the direction between
  the quartz and the PMT. There are three imaginary interface planes:  
  

     
             _____  PMT window _____   Upper Interface Plane
                  /           \
    upper cone   /             \ 
           ____ /               \___   Lower Interface Plane
                \               /
    lower cone   \    .        /
                  \   . .     /
              _____\  .   .  /______  y = 0     
                      .   .
                      .   .
                     quartz 
  
    --> z-direction (beam)


  The y = 0 plane defines the interface between the quartz and the light guide 
  This plane is always fixed at y = 0 and everything else is defined with respect to this
  plane and the positive y axis, in this implementation.

  The second plane (LowerPlane) is between the lower guide cone that forms the primary reflector
  and the upper cone that defines the rest of the light guide. This plane is defined by 
  
      (y-d) = 0     a distance d = LowerPlane above the y = 0 plane
  
  This plane can be repositioned along the y-axis to redfine the relative size 
  between the lower and upper cone.

  The third plane (UpperPlane) defines the length of the light guide and forms the interface
  between the guide and the PMT window.

*/

MOLLEROptDetectorLightGuide::MOLLEROptDetectorLightGuide(MOLLEROptTrackingReadout *TrRO, G4String name, MOLLEROptMaterial* mat)
{
  thisName = name+"_LG";

  LowerInterfacePlane     = 5.6*cm;
  UpperInterfacePlane     = 25.0*cm;
  LowerConeFrontFaceAngle = 28*TMath::Pi()/180*rad;                            
  LowerConeBackFaceAngle  =  22*TMath::Pi()/180*rad;
  LowerConeSideFaceAngle  =  22*TMath::Pi()/180*rad;
  QuartzInterfaceOpeningZ = 1.8*cm;
  QuartzInterfaceOpeningX = 9.2*cm;
  PMTInterfaceOpeningZ    = 5.6*cm;
  PMTInterfaceOpeningX    = 5.6*cm;
  QuartzToPMTOffsetInZ    = -0.3*cm;  //PMT moved upstream by 3 mm w.r.t. the quartz
  TopPlateThickness       = 0.1*cm;

  CenterPositionX = 0.0*cm;
  CenterPositionY = 0.0*cm; 
  CenterPositionZ = 0.0*cm;

  Materials = mat;
  GuideMaterial = Materials->GetMaterial("Aluminum");
  GuideCoreMaterial = Materials->GetMaterial("Air");

  TrackingReadout = TrRO;

  RotationLG = NULL;

  LowerConeVertices.resize(8);
  UpperConeVertices.resize(8);

  LowerConeVertices_out.resize(8);
  UpperConeVertices_out.resize(8);

}

MOLLEROptDetectorLightGuide::~MOLLEROptDetectorLightGuide()
{

}

void MOLLEROptDetectorLightGuide::ClearVolumes()
{
  if(GuideOptSurface) delete GuideOptSurface;
  if(GuideLogicalSkinSurface) delete GuideLogicalSkinSurface;
  if(GuidePhysical) delete GuidePhysical;
  if(GuideLogical) delete GuideLogical;
  
}

void MOLLEROptDetectorLightGuide::Construct(G4VPhysicalVolume* MotherVolume)
{
  Mother = MotherVolume;

  
  GuidePhysical   = new G4PVPlacement(RotationLG,  
				      PositionLG, 
				      thisName+"_Physical", 
				      GuideLogical,
				      Mother, 
				      false, 
				      3);
  
  GuideCorePhysical   = new G4PVPlacement(RotationLG,  
					  PositionLG, 
					  thisName+"_Core_Physical", 
					  GuideCoreLogical,
					  Mother, 
					  false, 
					  3);

  PositionTP  = G4ThreeVector(CenterPositionX,
  			      CenterPositionY + UpperInterfacePlane + TopPlateThickness,
  			      CenterPositionZ + QuartzToPMTOffsetInZ);
  
  GuideTopPhysical = new G4PVPlacement(RotationLG,  
  				       PositionTP, 
  				       thisName+"_GuideTop_Physical", 
  				       GuideTopLogical,
  				       Mother, 
  				       false, 
  				       6);

  
  GuidePhysical->CheckOverlaps();
  
}
 
void MOLLEROptDetectorLightGuide::CreateOpticalSurface(G4LogicalVolume *logV)
{
  OptParam* Optpar = Materials->GetOpticalParametersTable();
  
  GuideOptSurface = new G4OpticalSurface ( thisName+"_OpSurface",glisur, polished, dielectric_metal );
  //GuideOptSurface->SetPolish ( 0.99 ); //only works for "ground" surface
  
  G4MaterialPropertiesTable *GuideMatPropTable = new G4MaterialPropertiesTable();
  GuideMatPropTable->AddProperty ( "ANGLEREFLECTANCE00", Optpar->EPhoton, Optpar->LGRefl90, Optpar->GetNPar(), true);
  GuideMatPropTable->AddProperty ( "ANGLEREFLECTANCE30", Optpar->EPhoton, Optpar->LGRefl60, Optpar->GetNPar(), true);
  GuideMatPropTable->AddProperty ( "ANGLEREFLECTANCE45", Optpar->EPhoton, Optpar->LGRefl45, Optpar->GetNPar(), true);
  GuideMatPropTable->AddProperty ( "ANGLEREFLECTANCE60", Optpar->EPhoton, Optpar->LGRefl30, Optpar->GetNPar(), true);
  // GuideMatPropTable->AddProperty ( "REFLECTIVITY", Optpar->EPhoton, Optpar->LGRefl30, Optpar->GetNPar());
  // GuideMatPropTable->AddProperty ( "ANGLEREFLECTIVITY30", Optpar->EPhoton, Optpar->LGRefl30, Optpar->GetNPar());
  
  GuideOptSurface->SetMaterialPropertiesTable ( GuideMatPropTable ); 
  GuideLogicalSkinSurface = new G4LogicalSkinSurface(thisName+"_SkinSurface",logV,GuideOptSurface);
  
}

void MOLLEROptDetectorLightGuide::SetMaterial(G4String materialName)
{
  
}

void MOLLEROptDetectorLightGuide::SetCenterPositionInX(G4double xPos)
{
    CenterPositionX =xPos;	 

    GuidePhysical->SetTranslation(G4ThreeVector(CenterPositionX,
						CenterPositionY+LowerInterfacePlane/2, 
						CenterPositionZ));
    GuideCorePhysical->SetTranslation(G4ThreeVector(CenterPositionX,
						    CenterPositionY+LowerInterfacePlane/2, 
						    CenterPositionZ));
    GuideTopPhysical->SetTranslation(G4ThreeVector(CenterPositionX,
    						   CenterPositionY+UpperInterfacePlane+TopPlateThickness, 
    						   CenterPositionZ+QuartzToPMTOffsetInZ));
}

void MOLLEROptDetectorLightGuide::SetCenterPositionInY(G4double yPos)
{
    CenterPositionY = yPos;

    GuidePhysical->SetTranslation(G4ThreeVector(CenterPositionX,
						CenterPositionY+LowerInterfacePlane/2, 
						CenterPositionZ));
    GuideCorePhysical->SetTranslation(G4ThreeVector(CenterPositionX,
						    CenterPositionY+LowerInterfacePlane/2, 
						    CenterPositionZ));
    GuideTopPhysical->SetTranslation(G4ThreeVector(CenterPositionX,
    						   CenterPositionY+UpperInterfacePlane+TopPlateThickness, 
    						   CenterPositionZ+QuartzToPMTOffsetInZ));
}

void MOLLEROptDetectorLightGuide::SetCenterPositionInZ(G4double zPos)
{
    CenterPositionZ = zPos;

    GuidePhysical->SetTranslation(G4ThreeVector(CenterPositionX,
						CenterPositionY+LowerInterfacePlane/2, 
						CenterPositionZ));
    GuideCorePhysical->SetTranslation(G4ThreeVector(CenterPositionX,
						    CenterPositionY+LowerInterfacePlane/2, 
						    CenterPositionZ));
    GuideTopPhysical->SetTranslation(G4ThreeVector(CenterPositionX,
    						   CenterPositionY+UpperInterfacePlane+TopPlateThickness, 
    						   CenterPositionZ+QuartzToPMTOffsetInZ));
}



void MOLLEROptDetectorLightGuide::Initialize()
{
  DefineGeometry();

  PositionLG  = G4ThreeVector(CenterPositionX,
			      CenterPositionY,
			      CenterPositionZ);
  
  RotationLG = new G4RotationMatrix;
  RotationLG->rotateX(TMath::Pi()*rad/2.0);

  GuideLogical = new G4LogicalVolume(GuideSolid,GuideMaterial,thisName+"_Logical");
  GuideCoreLogical = new G4LogicalVolume(InnerSolid,GuideCoreMaterial,thisName+"_Core_Logical");
  
  CreateOpticalSurface(GuideLogical);

  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  LightGuideSD = new MOLLEROptLightGuideSD("/LightGuide",TrackingReadout);
  SDman->AddNewDetector(LightGuideSD);  

  GuideCoreLogical->SetSensitiveDetector(LightGuideSD);
  
  G4Colour  magenta   ( 255/255.,   0/255., 255/255.);   
  G4VisAttributes* LGVisAtt = new G4VisAttributes(magenta);
  LGVisAtt->SetVisibility(true);
  LGVisAtt->SetForceWireframe(true);
  GuideLogical->SetVisAttributes(LGVisAtt);

  G4Colour green    (   0/255., 255/255.,   0/255.);
  G4VisAttributes* LGCoreVisAtt = new G4VisAttributes(green);
  LGCoreVisAtt->SetVisibility(false);
  LGCoreVisAtt->SetForceWireframe(true);
  GuideCoreLogical->SetVisAttributes(LGCoreVisAtt);

  GuideTopLogical = new G4LogicalVolume(GuideTopSolid,GuideMaterial,thisName+"_GuideTop_Logical");
  G4Colour white    ( 255/255., 255/255., 255/255.);
  G4VisAttributes* LGTopVisAtt = new G4VisAttributes(white);
  LGTopVisAtt->SetVisibility(false);
  // LGCoreVisAtt->SetForceWireframe(true);
  GuideTopLogical->SetVisAttributes(LGTopVisAtt);


}

void MOLLEROptDetectorLightGuide::DefineGeometry()
{

  //do the innter surface first **************************************************************************************************
 
  G4double LowerIP_x = LowerInterfacePlane*TMath::Tan(LowerConeSideFaceAngle)+QuartzInterfaceOpeningX/2;
  G4double LowerIP_py = LowerInterfacePlane*TMath::Tan(LowerConeFrontFaceAngle)+QuartzInterfaceOpeningZ/2;
  G4double LowerIP_ny = -LowerInterfacePlane*TMath::Tan(LowerConeBackFaceAngle)-QuartzInterfaceOpeningZ/2;

  LowerConeVertices[0] = G4TwoVector(QuartzInterfaceOpeningX/2,-QuartzInterfaceOpeningZ/2);
  LowerConeVertices[1] = G4TwoVector(-QuartzInterfaceOpeningX/2,-QuartzInterfaceOpeningZ/2);
  LowerConeVertices[2] = G4TwoVector(-QuartzInterfaceOpeningX/2,QuartzInterfaceOpeningZ/2);
  LowerConeVertices[3] = G4TwoVector(QuartzInterfaceOpeningX/2,QuartzInterfaceOpeningZ/2);

  LowerConeVertices[4] = G4TwoVector(LowerIP_x,LowerIP_ny);
  LowerConeVertices[5] = G4TwoVector(-LowerIP_x,LowerIP_ny);
  LowerConeVertices[6] = G4TwoVector(-LowerIP_x,LowerIP_py);
  LowerConeVertices[7] = G4TwoVector(LowerIP_x,LowerIP_py);

  UpperConeVertices[0] = G4TwoVector(LowerIP_x,LowerIP_ny);
  UpperConeVertices[1] = G4TwoVector(-LowerIP_x,LowerIP_ny);
  UpperConeVertices[2] = G4TwoVector(-LowerIP_x,LowerIP_py);
  UpperConeVertices[3] = G4TwoVector(LowerIP_x,LowerIP_py);

  UpperConeVertices[4] = G4TwoVector(PMTInterfaceOpeningX/2,-(PMTInterfaceOpeningZ/2+QuartzToPMTOffsetInZ));
  UpperConeVertices[5] = G4TwoVector(-PMTInterfaceOpeningX/2,-(PMTInterfaceOpeningZ/2+QuartzToPMTOffsetInZ));
  UpperConeVertices[6] = G4TwoVector(-PMTInterfaceOpeningX/2,PMTInterfaceOpeningZ/2-QuartzToPMTOffsetInZ);
  UpperConeVertices[7] = G4TwoVector(PMTInterfaceOpeningX/2,PMTInterfaceOpeningZ/2-QuartzToPMTOffsetInZ);  


  
  //now do the outer surface ******************************************************************************************************

  LowerIP_x = LowerInterfacePlane*TMath::Tan(LowerConeSideFaceAngle)+ (QuartzInterfaceOpeningX+2.0*mm)/2;
  LowerIP_py = LowerInterfacePlane*TMath::Tan(LowerConeFrontFaceAngle)+ (QuartzInterfaceOpeningZ+2.0*mm)/2;
  LowerIP_ny = -LowerInterfacePlane*TMath::Tan(LowerConeBackFaceAngle)-(QuartzInterfaceOpeningZ+2.0*mm)/2;
  
  LowerConeVertices_out[0] = G4TwoVector((QuartzInterfaceOpeningX+2.0*mm)/2,-(QuartzInterfaceOpeningZ+2.0*mm)/2);
  LowerConeVertices_out[1] = G4TwoVector(-(QuartzInterfaceOpeningX+2.0*mm)/2,-(QuartzInterfaceOpeningZ+2.0*mm)/2);
  LowerConeVertices_out[2] = G4TwoVector(-(QuartzInterfaceOpeningX+2.0*mm)/2,(QuartzInterfaceOpeningZ+2.0*mm)/2);
  LowerConeVertices_out[3] = G4TwoVector((QuartzInterfaceOpeningX+2.0*mm)/2,(QuartzInterfaceOpeningZ+2.0*mm)/2);

  LowerConeVertices_out[4] = G4TwoVector(LowerIP_x,LowerIP_ny);
  LowerConeVertices_out[5] = G4TwoVector(-LowerIP_x,LowerIP_ny);
  LowerConeVertices_out[6] = G4TwoVector(-LowerIP_x,LowerIP_py);
  LowerConeVertices_out[7] = G4TwoVector(LowerIP_x,LowerIP_py);
  
  UpperConeVertices_out[0] = G4TwoVector(LowerIP_x,LowerIP_ny);
  UpperConeVertices_out[1] = G4TwoVector(-LowerIP_x,LowerIP_ny);
  UpperConeVertices_out[2] = G4TwoVector(-LowerIP_x,LowerIP_py);
  UpperConeVertices_out[3] = G4TwoVector(LowerIP_x,LowerIP_py);

  UpperConeVertices_out[4] = G4TwoVector((PMTInterfaceOpeningX+2.0*mm)/2,-((PMTInterfaceOpeningZ+2.0*mm)/2+QuartzToPMTOffsetInZ));
  UpperConeVertices_out[5] = G4TwoVector(-(PMTInterfaceOpeningX+2.0*mm)/2,-((PMTInterfaceOpeningZ+2.0*mm)/2+QuartzToPMTOffsetInZ));
  UpperConeVertices_out[6] = G4TwoVector(-(PMTInterfaceOpeningX+2.0*mm)/2,(PMTInterfaceOpeningZ+2.0*mm)/2-QuartzToPMTOffsetInZ);
  UpperConeVertices_out[7] = G4TwoVector((PMTInterfaceOpeningX+2.0*mm)/2,(PMTInterfaceOpeningZ+2.0*mm)/2-QuartzToPMTOffsetInZ);

  //******************************************************************************************************************************

  GuideTotalLength = UpperInterfacePlane;
  GuideTotalWidth = 2*LowerIP_x;
  GuideTotalDepth = LowerIP_py-LowerIP_ny;
  
  LowerCone = new G4GenericTrap(thisName+"_LowerConeSolid",LowerInterfacePlane/2,LowerConeVertices);
  UpperCone = new G4GenericTrap(thisName+"_UpperConeSolid",(UpperInterfacePlane-LowerInterfacePlane)/2,UpperConeVertices);
  
  LowerCone_out = new G4GenericTrap(thisName+"_LowerConeSolid_out",LowerInterfacePlane/2,LowerConeVertices_out);
  UpperCone_out = new G4GenericTrap(thisName+"_UpperConeSolid_out",(UpperInterfacePlane-LowerInterfacePlane)/2,UpperConeVertices_out);
  
  
  G4RotationMatrix *rot = new G4RotationMatrix;
  G4ThreeVector  trans = G4ThreeVector(0,0,UpperInterfacePlane/2);

  
  //********************* Define the shell that is the guide **********************************************************
  
  InnerSolid = new G4UnionSolid(thisName+"_InnerSolid",LowerCone,UpperCone,rot,trans);
  OuterSolid = new G4UnionSolid(thisName+"_OuterSolid",LowerCone_out,UpperCone_out,rot,trans);

  G4SubtractionSolid *tempSolid = new G4SubtractionSolid(thisName+"_Temp_Solid",OuterSolid,InnerSolid);
  
  // G4Box *LowerConeSideCutout = new G4Box(thisName+"_LowerConeSideCutout_Solid_BX",(QuartzInterfaceOpeningX+10.0*mm)/2.0,QuartzInterfaceOpeningZ/2.0,
  // 					 QuartzInterfaceOpeningZ/2.0);
  trans = G4ThreeVector(0,0,-(LowerInterfacePlane/2.0-(QuartzInterfaceOpeningZ-4.0*mm)/2.0)-2.0*mm);
  G4Box *LowerConeSideCutout = new G4Box(thisName+"_LowerConeSideCutout_Solid_BX",(QuartzInterfaceOpeningX+10.0*mm)/2.0,(QuartzInterfaceOpeningZ-4.0*mm)/2.0,(QuartzInterfaceOpeningZ-4.0*mm)/2.0);
  
  // GuideSolid = new G4SubtractionSolid(thisName+"_Solid",OuterSolid,InnerSolid);
  GuideSolid = new G4SubtractionSolid(thisName+"_Solid",tempSolid,LowerConeSideCutout,rot,trans);

  //*******************************************************************************************************************

  GuideTopCutoutSolid = new G4Tubs(thisName+"_GuideTop_Solid_CO",0,PMTOpeningRadius,0.15*cm,0,2.0*TMath::Pi());
  GuideTopBoxSolid = new G4Box(thisName+"_GuideTop_Solid_BX",(PMTInterfaceOpeningX+2.0*mm)/2.0,(PMTInterfaceOpeningZ+2.0*mm)/2.0,0.1*cm);
  GuideTopSolid = new G4SubtractionSolid(thisName+"_GuideTop_Solid",GuideTopBoxSolid,GuideTopCutoutSolid);

  trans = G4ThreeVector(0,0,-(LowerInterfacePlane/2.0-1.0*cm));
  GuideCoreBottomCutoutSolid = new G4Box(thisName+"_GuideBottom_Solid_Cut",(QuartzInterfaceOpeningX-4.0*mm)/2.0,(QuartzInterfaceOpeningZ-4.0*mm)/2.0,1.0*cm);
  GuideCoreSolid = new G4SubtractionSolid(thisName+"_GuideCore_Solid",InnerSolid,GuideCoreBottomCutoutSolid,rot,trans);

  
  // GuideSolid = new G4UnionSolid(thisName+"_InnerSolid",LowerCone,UpperCone,rot,trans);
  ExportGeometrySTL();
}

void MOLLEROptDetectorLightGuide::ExportGeometrySTL()
{
  
  G4double LowerIP_x = LowerInterfacePlane*TMath::Tan(LowerConeSideFaceAngle)+ (QuartzInterfaceOpeningX+2.0*mm)/2;
  G4double LowerIP_py = LowerInterfacePlane*TMath::Tan(LowerConeFrontFaceAngle)+ (QuartzInterfaceOpeningZ+2.0*mm)/2;
  G4double LowerIP_ny = -LowerInterfacePlane*TMath::Tan(LowerConeBackFaceAngle)-(QuartzInterfaceOpeningZ+2.0*mm)/2;
  
  LowerConeBottom[0].set((QuartzInterfaceOpeningX+2.0*mm)/2,0,-(QuartzInterfaceOpeningZ+2.0*mm)/2);
  LowerConeBottom[1].set((QuartzInterfaceOpeningX+2.0*mm)/2,0,(QuartzInterfaceOpeningZ+2.0*mm)/2);
  LowerConeBottom[2].set(-(QuartzInterfaceOpeningX+2.0*mm)/2,0,(QuartzInterfaceOpeningZ+2.0*mm)/2);
  LowerConeBottom[3].set(-(QuartzInterfaceOpeningX+2.0*mm)/2,0,-(QuartzInterfaceOpeningZ+2.0*mm)/2);
  LowerConeBottom[4].set((QuartzInterfaceOpeningX+2.0*mm)/2,0,-(QuartzInterfaceOpeningZ+2.0*mm)/2);

  LowerConeFront[0].set((QuartzInterfaceOpeningX+2.0*mm)/2,0,-(QuartzInterfaceOpeningZ+2.0*mm)/2);
  LowerConeFront[1].set(-(QuartzInterfaceOpeningX+2.0*mm)/2,0,-(QuartzInterfaceOpeningZ+2.0*mm)/2);
  LowerConeFront[2].set(-LowerIP_x,LowerInterfacePlane,LowerIP_ny);
  LowerConeFront[3].set(LowerIP_x,LowerInterfacePlane,LowerIP_ny); 
  LowerConeFront[4].set((QuartzInterfaceOpeningX+2.0*mm)/2,0,-(QuartzInterfaceOpeningZ+2.0*mm)/2);
  
  LowerConeBack[0].set((QuartzInterfaceOpeningX+2.0*mm)/2,0,(QuartzInterfaceOpeningZ+2.0*mm)/2);
  LowerConeBack[1].set(LowerIP_x,LowerInterfacePlane,LowerIP_py); 
  LowerConeBack[2].set(-LowerIP_x,LowerInterfacePlane,LowerIP_py);
  LowerConeBack[3].set(-(QuartzInterfaceOpeningX+2.0*mm)/2,0,(QuartzInterfaceOpeningZ+2.0*mm)/2);    
  LowerConeBack[4].set((QuartzInterfaceOpeningX+2.0*mm)/2,0,(QuartzInterfaceOpeningZ+2.0*mm)/2);
  
  LowerConeSide1[0].set((QuartzInterfaceOpeningX+2.0*mm)/2,0,(QuartzInterfaceOpeningZ+2.0*mm)/2);
  LowerConeSide1[1].set((QuartzInterfaceOpeningX+2.0*mm)/2,0,-(QuartzInterfaceOpeningZ+2.0*mm)/2);
  LowerConeSide1[2].set(LowerIP_x,LowerInterfacePlane,LowerIP_ny);
  LowerConeSide1[3].set(LowerIP_x,LowerInterfacePlane,LowerIP_py);
  LowerConeSide1[4].set((QuartzInterfaceOpeningX+2.0*mm)/2,0,(QuartzInterfaceOpeningZ+2.0*mm)/2);
  
  LowerConeSide2[0].set(-(QuartzInterfaceOpeningX+2.0*mm)/2,0,-(QuartzInterfaceOpeningZ+2.0*mm)/2);
  LowerConeSide2[1].set(-(QuartzInterfaceOpeningX+2.0*mm)/2,0,(QuartzInterfaceOpeningZ+2.0*mm)/2);
  LowerConeSide2[2].set(-LowerIP_x,LowerInterfacePlane,LowerIP_py);
  LowerConeSide2[3].set(-LowerIP_x,LowerInterfacePlane,LowerIP_ny);
  LowerConeSide2[4].set(-(QuartzInterfaceOpeningX+2.0*mm)/2,0,-(QuartzInterfaceOpeningZ+2.0*mm)/2);

  
  UpperConeFront[0].set(LowerIP_x,LowerInterfacePlane,LowerIP_ny);
  UpperConeFront[1].set(-LowerIP_x,LowerInterfacePlane,LowerIP_ny);
  UpperConeFront[2].set(-(PMTInterfaceOpeningX+2.0*mm)/2,UpperInterfacePlane,-((PMTInterfaceOpeningZ+2.0*mm)/2+QuartzToPMTOffsetInZ));
  UpperConeFront[3].set((PMTInterfaceOpeningX+2.0*mm)/2,UpperInterfacePlane,-((PMTInterfaceOpeningZ+2.0*mm)/2+QuartzToPMTOffsetInZ));
  UpperConeFront[4].set(LowerIP_x,LowerInterfacePlane,LowerIP_ny);

  UpperConeBack[0].set(-LowerIP_x,LowerInterfacePlane,LowerIP_py);
  UpperConeBack[1].set(LowerIP_x,LowerInterfacePlane,LowerIP_py);
  UpperConeBack[2].set((PMTInterfaceOpeningX+2.0*mm)/2,UpperInterfacePlane,((PMTInterfaceOpeningZ+2.0*mm)/2+QuartzToPMTOffsetInZ));
  UpperConeBack[3].set(-(PMTInterfaceOpeningX+2.0*mm)/2,UpperInterfacePlane,((PMTInterfaceOpeningZ+2.0*mm)/2+QuartzToPMTOffsetInZ));
  UpperConeBack[4].set(-LowerIP_x,LowerInterfacePlane,LowerIP_py);

  UpperConeSide1[0].set(LowerIP_x,LowerInterfacePlane,LowerIP_ny);
  UpperConeSide1[1].set((PMTInterfaceOpeningX+2.0*mm)/2,UpperInterfacePlane,-((PMTInterfaceOpeningZ+2.0*mm)/2+QuartzToPMTOffsetInZ));
  UpperConeSide1[2].set((PMTInterfaceOpeningX+2.0*mm)/2,UpperInterfacePlane,((PMTInterfaceOpeningZ+2.0*mm)/2+QuartzToPMTOffsetInZ));
  UpperConeSide1[3].set(LowerIP_x,LowerInterfacePlane,LowerIP_py);
  UpperConeSide1[4].set(LowerIP_x,LowerInterfacePlane,LowerIP_ny);

  UpperConeSide2[0].set(-LowerIP_x,LowerInterfacePlane,LowerIP_py);
  UpperConeSide2[1].set(-(PMTInterfaceOpeningX+2.0*mm)/2,UpperInterfacePlane,((PMTInterfaceOpeningZ+2.0*mm)/2+QuartzToPMTOffsetInZ));
  UpperConeSide2[2].set(-(PMTInterfaceOpeningX+2.0*mm)/2,UpperInterfacePlane,-((PMTInterfaceOpeningZ+2.0*mm)/2+QuartzToPMTOffsetInZ));
  UpperConeSide2[3].set(-LowerIP_x,LowerInterfacePlane,LowerIP_ny);
  UpperConeSide2[4].set(-LowerIP_x,LowerInterfacePlane,LowerIP_py);

  UpperConeTop[0].set((PMTInterfaceOpeningX+2.0*mm)/2,UpperInterfacePlane,-((PMTInterfaceOpeningZ+2.0*mm)/2+QuartzToPMTOffsetInZ));
  UpperConeTop[1].set(-(PMTInterfaceOpeningX+2.0*mm)/2,UpperInterfacePlane,-((PMTInterfaceOpeningZ+2.0*mm)/2+QuartzToPMTOffsetInZ));
  UpperConeTop[2].set(-(PMTInterfaceOpeningX+2.0*mm)/2,UpperInterfacePlane,((PMTInterfaceOpeningZ+2.0*mm)/2+QuartzToPMTOffsetInZ));
  UpperConeTop[3].set((PMTInterfaceOpeningX+2.0*mm)/2,UpperInterfacePlane,((PMTInterfaceOpeningZ+2.0*mm)/2+QuartzToPMTOffsetInZ));
  UpperConeTop[4].set((PMTInterfaceOpeningX+2.0*mm)/2,UpperInterfacePlane,-((PMTInterfaceOpeningZ+2.0*mm)/2+QuartzToPMTOffsetInZ));

  STLFile.open("LightGuideGeom.stl");
  STLFile << std::scientific;
  STLFile << "solid " << thisName+"_OuterSolid" << '\n'  << '\n';

  WriteSTLFacet(LowerConeBottom);
  WriteSTLFacet(&LowerConeBottom[2]);
  WriteSTLFacet(LowerConeFront);
  WriteSTLFacet(&LowerConeFront[2]);
  WriteSTLFacet(LowerConeSide1);
  WriteSTLFacet(&LowerConeSide1[2]);
  WriteSTLFacet(LowerConeBack);
  WriteSTLFacet(&LowerConeBack[2]);
  WriteSTLFacet(LowerConeSide2);
  WriteSTLFacet(&LowerConeSide2[2]);
  WriteSTLFacet(UpperConeFront);
  WriteSTLFacet(&UpperConeFront[2]);
  WriteSTLFacet(UpperConeBack);
  WriteSTLFacet(&UpperConeBack[2]);
  WriteSTLFacet(UpperConeSide1);
  WriteSTLFacet(&UpperConeSide1[2]);
  WriteSTLFacet(UpperConeSide2);
  WriteSTLFacet(&UpperConeSide2[2]);
  WriteSTLFacet(UpperConeTop);
  WriteSTLFacet(&UpperConeTop[2]);

  STLFile << "endsolid " << thisName+"_OuterSolid" << '\n';
  STLFile.close();
}

void MOLLEROptDetectorLightGuide::WriteSTLFacet(G4ThreeVector *vertices)
{
  G4ThreeVector tmp3 = vertices[1].cross(vertices[2]) - vertices[0].cross(vertices[2]);
  G4ThreeVector tr_norm = tmp3 + vertices[0].cross(vertices[1]);
  G4ThreeVector tr_unorm = tr_norm/tr_norm.mag();

  STLFile << "facet normal " << tr_unorm.x() << " " << tr_unorm.y() << " " << tr_unorm.z() << '\n';
  STLFile << '\t' << "outer loop" << '\n';
  STLFile << '\t' << '\t' <<"vertex " << vertices[0].x() << " " << vertices[0].y() << " " << vertices[0].z() << '\n';
  STLFile << '\t' << '\t' <<"vertex " << vertices[1].x() << " " << vertices[1].y() << " " << vertices[1].z() << '\n';
  STLFile << '\t' << '\t' <<"vertex " << vertices[2].x() << " " << vertices[2].y() << " " << vertices[2].z() << '\n';
  STLFile << '\t' << "endloop" << '\n';
  STLFile << "endfacet" << '\n' << '\n';
  
}

void MOLLEROptDetectorLightGuide::UpdateGeometry()
{
  // G4PhysicalVolumeStore::GetInstance()->DeRegister(GuidePhysical);
  // G4LogicalVolumeStore::GetInstance()->DeRegister(GuideLogical);
  
  G4SolidStore::GetInstance()->DeRegister(GuideSolid);
  G4SolidStore::GetInstance()->DeRegister(InnerSolid);
  G4SolidStore::GetInstance()->DeRegister(GuideTopSolid);
  G4SolidStore::GetInstance()->DeRegister(GuideCoreSolid);
  G4SolidStore::GetInstance()->DeRegister(GuideTopBoxSolid);
  G4SolidStore::GetInstance()->DeRegister(GuideTopCutoutSolid);
    
  // G4LogicalBorderSurface::CleanSurfaceTable();
  delete LowerCone;
  delete UpperCone;
  delete InnerSolid;
  
  delete LowerCone_out;
  delete UpperCone_out;
  delete OuterSolid;

  delete GuideSolid;
  delete GuideTopSolid;
  delete GuideTopBoxSolid;
  delete GuideTopCutoutSolid;
  delete GuideCoreSolid;

  DefineGeometry();
  
  GuideLogical->SetSolid(GuideSolid);
  GuideCoreLogical->SetSolid(GuideCoreSolid);
  GuideTopLogical->SetSolid(GuideTopSolid);

}


void MOLLEROptDetectorLightGuide::GetLightGuideLimits(G4double *vals)
{
  G4ThreeVector trans =  GuidePhysical->GetTranslation();

  //lower cone
  vals[0] = -PMTInterfaceOpeningX/2+trans.x();
  vals[1] = PMTInterfaceOpeningX/2+trans.x();
  vals[2] = -LowerInterfacePlane/2+trans.y()+1.6*cm; //The + 1.6*cm avoids the quartz 45 degree cut region
  vals[3] = (-LowerInterfacePlane/2 + LowerInterfacePlane) +trans.y();
  // vals[2] = GuideSolid->GetExtent().GetYmin();
  // vals[3] = GuideSolid->GetExtent().GetYmin() + LowerInterfacePlane;

  //upper cone
  vals[4] = -PMTInterfaceOpeningX/2+trans.x();
  vals[5] = PMTInterfaceOpeningX/2+trans.x();
  vals[6] = (-LowerInterfacePlane/2 + LowerInterfacePlane) + trans.y();
  vals[7] = UpperInterfacePlane - (LowerInterfacePlane)/2 + trans.y();

  // vals[6] = GuideSolid->GetExtent().GetYmin() + LowerInterfacePlane;
  // vals[7] = GuideSolid->GetExtent().GetYmax();


}

void MOLLEROptDetectorLightGuide::GetLightGuideLowerConeSideVertices(std::vector<G4TwoVector> *Vertices)
{

  // LowerConeVertices_out[0] = G4TwoVector((QuartzInterfaceOpeningX+2.0*mm)/2,-(QuartzInterfaceOpeningZ+2.0*mm)/2);
  // LowerConeVertices_out[1] = G4TwoVector(-(QuartzInterfaceOpeningX+2.0*mm)/2,-(QuartzInterfaceOpeningZ+2.0*mm)/2);
  // LowerConeVertices_out[2] = G4TwoVector(-(QuartzInterfaceOpeningX+2.0*mm)/2,(QuartzInterfaceOpeningZ+2.0*mm)/2);
  // LowerConeVertices_out[3] = G4TwoVector((QuartzInterfaceOpeningX+2.0*mm)/2,(QuartzInterfaceOpeningZ+2.0*mm)/2);

  (*Vertices)[0] = G4TwoVector(LowerConeVertices_out[2].y(), -LowerInterfacePlane/2);
  (*Vertices)[1] = G4TwoVector(LowerConeVertices_out[0].y(), -LowerInterfacePlane/2);
  (*Vertices)[2] = G4TwoVector(LowerConeVertices_out[5].y(),  LowerInterfacePlane/2);
  (*Vertices)[3] = G4TwoVector(LowerConeVertices_out[6].y(),  LowerInterfacePlane/2);

  // (*Vertices)[1] = LowerConeVertices_out[1];

  // LowerConeVertices_out[4] = G4TwoVector(LowerIP_x,LowerIP_ny);
  // LowerConeVertices_out[5] = G4TwoVector(-LowerIP_x,LowerIP_ny);
  // LowerConeVertices_out[6] = G4TwoVector(-LowerIP_x,LowerIP_py);
  // LowerConeVertices_out[7] = G4TwoVector(LowerIP_x,LowerIP_py);
  
  // UpperConeVertices_out[0] = G4TwoVector(LowerIP_x,LowerIP_ny);
  // UpperConeVertices_out[1] = G4TwoVector(-LowerIP_x,LowerIP_ny);
  // UpperConeVertices_out[2] = G4TwoVector(-LowerIP_x,LowerIP_py);
  // UpperConeVertices_out[3] = G4TwoVector(LowerIP_x,LowerIP_py);

  // UpperConeVertices_out[4] = G4TwoVector((PMTInterfaceOpeningX+2.0*mm)/2,-((PMTInterfaceOpeningZ+2.0*mm)/2+QuartzToPMTOffsetInZ));
  // UpperConeVertices_out[5] = G4TwoVector(-(PMTInterfaceOpeningX+2.0*mm)/2,-((PMTInterfaceOpeningZ+2.0*mm)/2+QuartzToPMTOffsetInZ));
  // UpperConeVertices_out[6] = G4TwoVector(-(PMTInterfaceOpeningX+2.0*mm)/2,(PMTInterfaceOpeningZ+2.0*mm)/2-QuartzToPMTOffsetInZ);
  // UpperConeVertices_out[7] = G4TwoVector((PMTInterfaceOpeningX+2.0*mm)/2,(PMTInterfaceOpeningZ+2.0*mm)/2-QuartzToPMTOffsetInZ);


}
