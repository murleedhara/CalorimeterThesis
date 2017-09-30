/**
 * @file /DB_EMCal/src/DetectorConstruction.cc
 * @author Balázs Demeter <balazsdemeter92@gmail.com>
 * @date 2017/09/15 <creation>
 * 
 * @section DESCRIPTION
 * 
 * The Geant4 simulation of EMcal's Detector construction source code to define materials and geometry. 
 * Latest updates of project can be found in README file.
 **/

#include "DetectorConstruction.hh"
#include "G4SubtractionSolid.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpticalSurface.hh"


/// @brief Constructor of Detector construction

DetectorConstruction::DetectorConstruction()
: G4VUserDetectorConstruction()
{}

/// @brief Destructor of Detector construction

DetectorConstruction::~DetectorConstruction()
{ }

/**
 * @brief Construct function to built objects and frame of reference
 * 
 * @return	Placement of world
 * 
 **/

G4VPhysicalVolume* DetectorConstruction::Construct()
{ 
  G4double a, z, density_pmma, density_ps, pos=13; /// Useable constants and variables (radius, density and etc.)
  G4int nelements;
  
  G4double tank_sizeXY = 1.47*mm, tank_sizeZ = 12.6*cm; /// Size of Tank
  
  G4NistManager* nist = G4NistManager::Instance(); /// Get nist material manager
   
  G4bool checkOverlaps = true; /// Option to switch on/off checking of volumes overlaps
  
  G4Element* C = new G4Element("Carbon", "C", z=6 , a=12.01*g/mole); /// definition of elements for materials
  G4Element* H = new G4Element("Hydrogen", "H", z=1 , a=1.01*g/mole);
  G4Element* O = new G4Element("Oxygen"  , "O", z=8 , a=16.00*g/mole);

/**
  * ... Frame of reference ...
  * 
  * @param world_sizeXY		Size of world in meter (x,y axis)
  * @param world_sizeZ		Size of world in meter (z axis)
  * @param world_mat		Saterial of world 
  * @param solidWorld		Geometry of world
  * @param logicWorld		Logical volume of world
  * @param physWorld		Placement of world
  * 
  **/
  
  G4double world_sizeXY = 1.2*m, world_sizeZ  = 1.2*m;
  G4Box* solidWorld = new G4Box("World", world_sizeXY, world_sizeXY, world_sizeZ);
  
  G4Material* world_mat = nist->FindOrBuildMaterial("G4_Galactic"); 
      
  G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, world_mat, "World");                                  
  G4VPhysicalVolume* physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World", 0, false, 0, checkOverlaps);

/**
  * 
  * ... Fiber ...
  * 
  * @param r 				 Radius of fiber in milimeter
  * @param posFiber 		 Position of fiber in world
  * @param fiberInterior 	 Geometry of fiber's interior
  * @param fiberCover 		 Geometry of fiber's cover
  * @param fiberFull		 Geometry of complete fiber (for subtraction)
  * @param pmma				 PMMA (Material of fiber's cover)
  * @param ps				 Polystyrene (Material of fiber's interior)
  * @param fiberInteriorLog	 Logical volume of fiber's interior
  * @param fiberCoverLog	 Logical volume of fiber's cover
  * @param fiberColour		 Color of fiber
  * @param fiberVisAtt		 Visual attributes of tank
  * @param interiorPhysVol   Physical volume of fiber's interior
  * 
  **/
  
  G4double r = (0.47/2)*mm;
  G4ThreeVector posFiber = G4ThreeVector(0, 0*cm, pos*cm);
  G4Tubs* fiberInterior  = new G4Tubs("fInterior", 0*cm, (r-(r*0.02)), tank_sizeZ, 0*deg, 360*deg);
  G4Tubs* fiberCover 	 = new G4Tubs("fCover", (r-(r*0.02)), r, tank_sizeZ, 0*deg, 360*deg);
  G4Tubs* fiberFull 	 = new G4Tubs("Fiber", 0*cm, r, (tank_sizeZ+0.1*cm), 0*deg, 360*deg);

  G4Material* pmma = new G4Material("PMMA", density_pmma = 1.190*g/cm3, nelements=3);
  pmma->AddElement(C, 33.34*perCent);
  pmma->AddElement(H, 53.33*perCent);
  pmma->AddElement(O, 13.33*perCent);
  
  G4Material* polyStyrene = new G4Material("Polystyrene", density_ps = 1.05*g/cm3, nelements=2);
  polyStyrene->AddElement(C, 50.00*perCent);
  polyStyrene->AddElement(H, 50.00*perCent);
  
  G4LogicalVolume* fiberInteriorLog = new G4LogicalVolume(fiberInterior, polyStyrene, "fiberInterior");
  G4LogicalVolume* fiberCoverLog = new G4LogicalVolume(fiberCover, pmma, "fiberCover");
  
  G4Colour fiberColour( 1.0, 0.0, 0.0, 1.0 );
  G4VisAttributes* fiberVisAtt = new G4VisAttributes( fiberColour );
  fiberInteriorLog->SetVisAttributes(fiberVisAtt);
  fiberCoverLog->SetVisAttributes(fiberVisAtt);
  
  new G4PVPlacement(0, posFiber, fiberInteriorLog, "fiberInterior", logicWorld, false, 0, checkOverlaps);
  new G4PVPlacement(0, posFiber, fiberCoverLog, "fiberCover", logicWorld, false, 0, checkOverlaps);
                     
/**
  * ... Tank ...
  * 
  * @param tank_sizeX		Size of tank in meter (x axis)
  * @param tank_sizeY		Size of tank in meter (y axis)
  * @param tank_sizeZ		Size of tank in meter (z axis)
  * @param posTank			Position of tank in world
  * @param tank_mat			Material of tank 
  * @param solidTank		Geometry of tank (full)
  * @param subTank			Geometry of tank (after subtraction)
  * @param logicTank		Logical volume of tank
  * @param tankColour		Color of tank
  * @param tankVisAtt		Visual attributes of tank
  * 
  **/
  
  G4ThreeVector posTank = G4ThreeVector(0, 0*cm, pos*cm);
  
  G4Box* solidTank = new G4Box("Tank", tank_sizeXY, tank_sizeXY, tank_sizeZ);
  G4SubtractionSolid* subTank = new G4SubtractionSolid("Tank", solidTank, fiberFull, 0, G4ThreeVector(0,0,0));
  
  G4Material* tank_mat = nist->FindOrBuildMaterial("G4_W");
                
  G4LogicalVolume* logicTank = new G4LogicalVolume(subTank, tank_mat, "Tank");               
                    
  G4Colour tankColour( 0.0, 1.0, 0.0, 0.4 );
  G4VisAttributes* tankVisAtt = new G4VisAttributes( tankColour );
  logicTank->SetVisAttributes(tankVisAtt);
  
  new G4PVPlacement(0, posTank, logicTank, "Tank", logicWorld, false, 0, checkOverlaps);
  
/**
  * ... Detector ...
  * 
  * @param detec_sizeZ		Size of detector in meter (z axis)
  * @param posDetec			Position of detector in world
  * @param detec_mat		Material of detector 
  * @param solidDetec		Geometry of detector (full)
  * @param subDetec			Geometry of detector (after subtraction)
  * @param logicDetec		Logical volume of detector
  * @param detecColour		Color of detector
  * @param detecVisAtt		Visual attributes of detector
  * 
  **/
  
  G4double detec_sizeZ = 1*mm;
  G4Box* solidDetec = new G4Box("Detector", tank_sizeXY, tank_sizeXY, detec_sizeZ);
  G4ThreeVector posDetec =G4ThreeVector(0, 0*cm, ((pos*cm)+(tank_sizeZ)+(detec_sizeZ))); ///zpos+(fTank_z)+(mirror_z)
  
  G4Material* detec_mat = nist->FindOrBuildMaterial("G4_Pyrex_Glass");
  
  G4LogicalVolume* logicDetec = new G4LogicalVolume(solidDetec, detec_mat, "Detector");
  
  G4Colour detColour( 1.0, 1.0, 1.0, 1.0 );
  G4VisAttributes* detVisAtt = new G4VisAttributes(detColour);
  logicDetec->SetVisAttributes(detVisAtt);
  
  new G4PVPlacement(0,posDetec, logicDetec, "Detector", logicWorld, false, 0, checkOverlaps);
  
/// @brief Energy of photons in fiber table
	
  G4double photonEnergy[] = { 2.0*eV, 2.158*eV, 2.316*eV, 2.474*eV,
							   2.632*eV, 2.789*eV, 2.947*eV, 3.105*eV,
							   3.263*eV, 3.421*eV, 3.579*eV, 3.737*eV,
							   3.895*eV, 4.053*eV, 4.211*eV, 4.368*eV,
							   4.526*eV, 4.684*eV, 4.842*eV, 5.0*eV
							 };

  const G4int nEntries = sizeof(photonEnergy)/sizeof(G4double); /// Number of entries
    
/**
 * Fiber's ring optical table
 * 
 * @param rIndexFiberRing	Refractive index table of ring
 * @param refFiberRing		Reflectivity table of ring 	
 * @param absFiberRing		Absorption table of ring
 * 
 **/
    
  G4double rIndexFiberRing[] = { 1.49, 1.4925, 1.495, 1.4975, 1.5, 1.5025, 1.505,  
							  1.5075, 1.51, 1.5125, 1.515, 1.5175, 1.52, 1.5225,
							  1.525, 1.5275, 1.53, 1.5325, 1.535, 1.5375};
                          
  G4double refFiberRing[] = { 0.04, 0.047, 0.054, 0.061, 0.068, 0.075, 0.082,  
                               0.089, 0.096, 0.103, 0.11, 0.117, 0.124, 0.131,
                               0.138, 0.145, 0.152, 0.159, 0.166, 0.173};
        
  G4double absFiberRing[] = {3.448*m,  4.082*m,  6.329*m,  9.174*m, 12.346*m, 13.889*m, 
							 15.152*m, 17.241*m, 18.868*m, 20.000*m, 26.316*m, 35.714*m,
							 45.455*m, 47.619*m, 52.632*m, 52.632*m, 55.556*m, 52.632*m,
							 52.632*m, 47.619*m, 45.455*m, 41.667*m, 37.037*m, 33.333*m,
							 30.000*m, 28.500*m, 27.000*m, 24.500*m, 22.000*m, 19.500*m,
							 17.500*m, 14.500*m };

/**
 * Fiber's interior optical table
 * 
 * @param rIndexFiberInterior	Refractive index table of interior
 * @param refFiberInterior		Reflectivity table of interior
 * @param absFiberInterior		Absorption table of interior
 * 
 **/

  G4double rIndexFiberInterior[] = { 1.59, 1.5925, 1.595, 1.5975, 1.6, 1.6025, 1.605, 
                          1.6075, 1.61, 1.6125, 1.615, 1.6175, 1.62, 1.6225,
                          1.625, 1.6275, 1.63, 1.6325, 1.635, 1.6375};
                          
  G4double refFiberInterior[] = { 0.02, 0.027, 0.034, 0.041, 0.048, 0.055, 0.062, 
                           0.069, 0.076, 0.083, 0.091, 0.097, 0.104, 0.111,
                           0.118, 0.125, 0.132, 0.139, 0.146, 0.153};
        
  G4double absFiberInterior[] = {3.448*m,  4.082*m,  6.329*m,  9.174*m, 12.346*m, 13.889*m, 
						 15.152*m, 17.241*m, 18.868*m, 20.000*m, 26.316*m, 35.714*m,
						 45.455*m, 47.619*m, 52.632*m, 52.632*m, 55.556*m, 52.632*m,
						 52.632*m, 47.619*m, 45.455*m, 41.667*m, 37.037*m, 33.333*m,
						 30.000*m, 28.500*m, 27.000*m, 24.500*m, 22.000*m, 19.500*m,
						 17.500*m, 14.500*m };

/**
 * Detector's optical table
 * 
 * @param rIndexDetector 	Refractive index table of detector
 * @param refDetector 		Reflectivity table of detector
 * 
 **/
 
  G4double rIndexDetector[] = { 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
                          1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
                          1.00, 1.00, 1.00, 1.00, 1.00, 1.00};
  
  G4double refDetector[] = { 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,
                           0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,
                           0.00, 0.00, 0.00, 0.00, 0.00, 0.00};

/// @brief Fiber's ring material properties table

  G4MaterialPropertiesTable* mptPMMA = new G4MaterialPropertiesTable();
    
  mptPMMA->AddProperty("RINDEX", photonEnergy, rIndexFiberRing, nEntries);
  mptPMMA->AddProperty("REFLECTIVITY", photonEnergy, refFiberRing, nEntries);
  mptPMMA->AddProperty("ABSLENGTH",    photonEnergy, absFiberRing,     nEntries);

  G4cout << "PMMA G4MaterialPropertiesTable" << G4endl;
        
  mptPMMA->DumpTable();
  pmma->SetMaterialPropertiesTable(mptPMMA);
    
/// @brief Fiber's interior material properties table

  G4MaterialPropertiesTable* mptPolyStyrene = new G4MaterialPropertiesTable();
  mptPolyStyrene->AddProperty("RINDEX", photonEnergy, rIndexFiberInterior, nEntries);
  mptPolyStyrene->AddProperty("REFLECTIVITY", photonEnergy, refFiberInterior, nEntries);
  mptPolyStyrene->AddProperty("ABSLENGTH",    photonEnergy, absFiberInterior, nEntries);

  G4cout << "polyStyrene G4MaterialPropertiesTable" << G4endl;
  mptPolyStyrene->DumpTable();
  polyStyrene->SetMaterialPropertiesTable(mptPolyStyrene);
  
/// @brief Detector's material properties table
  
  G4MaterialPropertiesTable* mptDetector = new G4MaterialPropertiesTable();
  mptDetector->AddProperty("RINDEX", photonEnergy, rIndexDetector, nEntries);
  mptDetector->AddProperty("REFLECTIVITY", photonEnergy, refDetector, nEntries);

  G4cout << "Detector G4MaterialPropertiesTable" << G4endl;
  mptDetector->DumpTable();

  detec_mat->SetMaterialPropertiesTable(mptDetector);
  
/// @brief Fiber's surface optical definition
  
  G4OpticalSurface*  fiberInteriorSurface = new G4OpticalSurface("fiberInteriorOpticalSurface");
  fiberInteriorSurface->SetType(dielectric_dielectric);
  fiberInteriorSurface->SetFinish(polished);
  fiberInteriorSurface->SetModel(unified);

  G4OpticalSurface*  fiberCoverSurface = new G4OpticalSurface("fiberCoverSurfaceOpticalSurface");
  fiberCoverSurface->SetType(dielectric_dielectric);
  fiberCoverSurface->SetFinish(polished);
  fiberCoverSurface->SetModel(unified);
    
  G4LogicalSkinSurface* fiberInteriorLogSurface = new G4LogicalSkinSurface("fiberInteriorLogSurface", fiberInteriorLog, fiberInteriorSurface);
  G4LogicalSkinSurface* fiberCoverLogSurface = new G4LogicalSkinSurface("fiberCoverLogSurface", fiberCoverLog, fiberCoverSurface);
    
  G4OpticalSurface* fiberInteriorSurfaceDynamic = dynamic_cast <G4OpticalSurface*>(fiberInteriorLogSurface->GetSurface(fiberInteriorLog)->GetSurfaceProperty());
    
  if (fiberInteriorSurfaceDynamic)
  { 
	  fiberInteriorSurfaceDynamic->DumpInfo();
  }
  
  G4OpticalSurface* fiberCoverSurfaceDynamic = dynamic_cast <G4OpticalSurface*>(fiberCoverLogSurface->GetSurface(fiberCoverLog)->GetSurfaceProperty());

  if (fiberCoverSurfaceDynamic)
  {
	  fiberCoverSurfaceDynamic->DumpInfo();
  }
  
/// @brief Detector's surface optical definition

  G4OpticalSurface*  DetectorSurface = new G4OpticalSurface("DetectorSurface");
  
  DetectorSurface->SetType(dielectric_metal);
  DetectorSurface->SetFinish(polished);
  DetectorSurface->SetModel(unified);
  
  G4LogicalSkinSurface* detectorLogSurface = new G4LogicalSkinSurface("DetectorSurface", logicDetec, DetectorSurface);
  

  return physWorld;
  
}

/// End of file