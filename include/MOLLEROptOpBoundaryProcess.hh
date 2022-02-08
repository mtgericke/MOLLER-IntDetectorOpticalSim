#pragma once

#include "G4OpBoundaryProcess.hh"

#include "G4LogicalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4ParallelWorldProcess.hh"
#include "G4TransportationManager.hh"
#include "G4PhysicalConstants.hh"

class MOLLEROptOpBoundaryProcess : public G4OpBoundaryProcess
{
 public:
  using G4OpBoundaryProcess::G4OpBoundaryProcess;

  G4VParticleChange* PostStepDoIt(const G4Track& aTrack,
                                  const G4Step& aStep) override
  {
    // Before calling G4OpBoundaryProcess::PostStepDoIt, we make sure the property
    // vector will contain the reflectivity for this incident angle

    const G4Step* pStep = &aStep;
    G4VPhysicalVolume* thePrePV  = pStep->GetPreStepPoint()->GetPhysicalVolume();
    G4VPhysicalVolume* thePostPV = pStep->GetPostStepPoint()->GetPhysicalVolume();

    G4LogicalSurface* surface =
      G4LogicalBorderSurface::GetSurface(thePrePV, thePostPV);

    if (surface != nullptr) {

      G4OpticalSurface* optical_surface =
        dynamic_cast<G4OpticalSurface*>(surface->GetSurfaceProperty());

      if (optical_surface != nullptr) {

        G4MaterialPropertiesTable* sMPT =
          optical_surface->GetMaterialPropertiesTable();

        if (sMPT != nullptr) {

          // Incident angle calculation:
          // old momentum
          const G4DynamicParticle* aParticle = aTrack.GetDynamicParticle();
          const G4ThreeVector old_momentum = aParticle->GetMomentumDirection();
          // global normal
          const G4ThreeVector global_point = pStep->GetPostStepPoint()->GetPosition();
          G4bool valid;
          G4int hNavId = G4ParallelWorldProcess::GetHypNavigatorID();
          auto iNav    = G4TransportationManager::GetTransportationManager()
                         ->GetActiveNavigatorsIterator();
          G4ThreeVector global_normal = - (iNav[hNavId])->GetGlobalExitNormal(global_point, &valid);
          // incident angle
          G4double incident_angle = GetIncidentAngle(old_momentum, global_normal);

          // Retrieve material properties
          const std::vector<G4String> names = sMPT->GetMaterialPropertyNames();
          std::vector<std::pair<double, G4String>> angles_keys;
          for (const auto& name: names) {
            if (name.contains("ANGLEREFLECTANCE")) {
              double angle = std::stof(&(name.data()[16]));
              angles_keys.push_back(std::make_pair(angle,name));
            }
          }

          // Find angle index to use
          const auto lower_bound = std::lower_bound(angles_keys.begin(), angles_keys.end(), incident_angle,
            [](const std::pair<double, G4String>& angle_key, double value) {
              return angle_key.first < value;
            });
          const auto property_table = sMPT->GetProperty(lower_bound->second);

          // Substitute for REFLECTIVITY
          sMPT->AddProperty("REFLECTIVITY", property_table);

        }
      }
    }

    return G4OpBoundaryProcess::PostStepDoIt(aTrack, aStep);
  };

 private:

  G4double GetIncidentAngle(const G4ThreeVector& momentum, const G4ThreeVector& normal)
  {
    return pi - std::acos(momentum * normal /
                          (momentum.mag() * normal.mag()));
  }
};
