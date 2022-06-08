#ifndef MOLLEROptDetector_h
#define MOLLEROptDetector_h 

#include "cpp_include.h"
#include "Root_include.h"
#include "Geant4_include.hh"

#include "MOLLEROptMaterial.hh"
#include "MOLLEROptTrackingReadout.hh"
#include "MOLLEROptDetectorLightGuide.hh"
#include "MOLLEROptDetectorQuartz.hh"
#include "MOLLEROptDetectorPMT.hh"
#include "MOLLEROptDetectorMessenger.hh"

#include "G4GDMLParser.hh"

struct DesignParameters{

  G4double QuartzSizeX; 
  G4double QuartzSizeY; 
  G4double QuartzSizeZ;
  
  G4double LowerInterfacePlane;
  G4double MiddleBoxHeight;
  G4double UpperInterfacePlane;
  G4double LowerConeFrontFaceAngle;
  G4double LowerConeBackFaceAngle;
  G4double LowerConeSideFaceAngle;
  G4double QuartzInterfaceOpeningZ;
  G4double QuartzInterfaceOpeningX;
  G4double PMTInterfaceOpeningZ;
  G4double PMTInterfaceOpeningX;
  G4double QuartzToPMTOffsetInZ;
  G4double QuartzBevel;
  G4double QuartzRotation;
  G4double DetectorRotation;
};


class MOLLEROptDetectorMessenger;

class MOLLEROptDetector
{
public:
  MOLLEROptDetector(MOLLEROptTrackingReadout*, G4String type, MOLLEROptMaterial* mat);
  ~MOLLEROptDetector();

  G4VPhysicalVolume* ConstructDetector(G4VPhysicalVolume* Mother);
  void ConstructMountingStructure(G4VPhysicalVolume* Mother);
  
  void DestroyComponent();
  void Initialize();
  void SetMaterial(G4String materialName);

  void SetCenterPositionInX(G4double xPos);
  void SetCenterPositionInY(G4double yPos);
  void SetCenterPositionInZ(G4double zPos);
  void SetAzimuthalRotationAngle(G4double a); //w.r.t. to the positive y axis (up) in radians
  void SetPolarRotationAngle(G4double a); //w.r.t. to the positive z axis (downstream) in radians
  

  //The standard coordinate system is: z=beam direction, y=beam up, x = beam left
  //Dimensions below assume a detector that is located at the origin with the 45 degreee
  //exit face at the upper y end and the width of the detector along +-x.
  void SetQuartzSizeX(G4double x); 
  void SetQuartzSizeY(G4double y); 
  void SetQuartzSizeZ(G4double z);
  void SetQuartzRotX(G4double r);

  void SetLightGuideOffsetInX(G4double x);
  void SetLightGuideOffsetInY(G4double y);
  void SetLightGuideOffsetInZ(G4double z);
  
  void SetLowerInterfacePlane(G4double LowerPlane);
  void SetMiddleBoxHeight(G4double MiddlePlane);
  void SetUpperInterfacePlane(G4double UpperPlane);
  void SetLowerConeFrontFaceAngle(G4double angle) ; //in radians
  void SetLowerConeBackFaceAngle(G4double angle);
  void SetLowerConeSideFaceAngle(G4double angle);
  void SetQuartzInterfaceOpeningZ(G4double size);
  void SetQuartzInterfaceOpeningX(G4double size);
  void SetPMTInterfaceOpeningZ(G4double size);
  void SetPMTInterfaceOpeningX(G4double size);
  void SetQuartzToPMTOffsetInZ(G4double val);
  void SetQuartzBevel(G4double bev);

  void SetPMTCathodeRadius(G4double size);
  void SetPMTCathodeThickness(G4double size);
  
  void UpdateThisGeometry();
  void CalculateDimensions();
  void ResetCenterLocation();
  void SetSegRadDamageFlag() {if(Quartz) Quartz->SetSegRadDamageFlag(); Materials->SetSegRadDamage();};

  G4LogicalVolume*   GetLogicalVolume()    {return DetLogical; } 
  G4VPhysicalVolume* GetPhysicalVolume()   {return DetPhysical;}

  void GetQuartzLimits(G4double *vals);
  void GetLightGuideLimits(G4double *vals);
  DesignParameters* GetDesignParameters();

private:

  DesignParameters *DesignParms;

  G4GDMLParser gdmlParser;

  G4String fReadFile;
  G4String fWriteFile;
  G4String fStepFile;
  G4int fWritingChoice;

  G4String DetType; //like: Ring5, or Ring1 ...
  
  MOLLEROptDetectorMessenger*  detMessenger;
  MOLLEROptTrackingReadout *TrackingReadout;
  MOLLEROptMaterial* Materials;
  MOLLEROptDetectorLightGuide* LightGuide; 
  MOLLEROptDetectorQuartz*     Quartz;
  MOLLEROptDetectorQuartz*     Quartz2;
  MOLLEROptDetectorPMT*        PMT;
  //MOLLEROptDetectorStructure*  Structure;

  G4VPhysicalVolume* MotherVolume;

  G4LogicalVolume*   DetLogical; 
  G4VPhysicalVolume* DetPhysical; 
  G4Material*        DetMaterial;
  G4Box*             DetSolid;

  G4double DetFullLengthX;
  G4double DetFullLengthY;
  G4double DetFullLengthZ;

  G4ThreeVector     PositionDet;
  G4RotationMatrix* RotationDet;  

  G4double PositionDetX;
  G4double PositionDetY;
  G4double PositionDetZ;

  G4double AzimuAngle;
  G4double PolarAngle;

  G4double PMTToQuartzOffset;

  //Detector Mounting structure

  //Left side of LG lower cone and quartz tray
  G4GenericTrap* QLGTray_Sld_Left_out;
  G4GenericTrap* QLGTray_Sld_Left_In; 
  G4SubtractionSolid* QLGTray_Sld_Left;

};
#endif
