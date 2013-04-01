/**************************************************************************
 * BASF2 (Belle Analysis Framework 2)                                     *
 * Copyright(C) 2010 - Belle II Collaboration                             *
 *                                                                        *
 * Author: The Belle II Collaboration                                     *
 * Contributors:  Leo Piilonen                                            *
 * Derived from:  G4ErrorPropagatorManager                                *
 *                                                                        *
 * This software is provided "as is" without any warranty.                *
 **************************************************************************/

#include <tracking/modules/ext/ExtManager.h>
#include <tracking/modules/ext/ExtNavigator.h>
#include <G4MagIntegratorStepper.hh>
#include <G4Mag_UsualEqRhs.hh>
#include <G4Mag_EqRhs.hh>
#include <G4MagIntegratorDriver.hh>
#include <G4ClassicalRK4.hh>
#include <G4ExactHelixStepper.hh>
#include <G4HelixExplicitEuler.hh>
#include <G4EventManager.hh>
#include <G4ErrorRunManagerHelper.hh>
#include <G4ErrorPropagator.hh>
#include <G4ErrorMag_UsualEqRhs.hh>
#include <G4VParticleChange.hh>
#include <G4ParticleChangeForMSC.hh>
#include <G4ParticleChange.hh>
#include <G4Track.hh>
#include <G4TransportationManager.hh>
#include <G4GeometryManager.hh>
#include <G4StateManager.hh>
#include <G4ChordFinder.hh>
#include <G4EquationOfMotion.hh>
#include <G4FieldManager.hh>
#include <G4PropagatorInField.hh>
#include <G4VParticleChange.hh>

#include <framework/logging/Logger.h>

using namespace Belle2;

ExtManager* ExtManager::m_manager = NULL;

ExtManager* ExtManager::GetManager()
{
  if (m_manager == NULL) {
    m_manager = new ExtManager;
  }
  return m_manager;
}

ExtManager::ExtManager()
{
  m_propagator = NULL;
  m_equationOfMotion = NULL;
  StartHelper();
  G4ErrorPropagatorData::GetErrorPropagatorData()->SetState(G4ErrorState_PreInit);
  m_navigator = NULL;
  StartNavigator(); //navigator has to be initialized at the beginning !?!?!
}

ExtManager::~ExtManager()
{
  if (m_equationOfMotion) delete m_equationOfMotion;
  if (m_navigator) delete m_navigator;
  if (m_propagator) delete m_propagator;
  if (m_helper) delete m_helper;
  if (m_manager) delete m_manager;
}

void ExtManager::StartHelper()
{
  m_helper = G4ErrorRunManagerHelper::GetRunManagerKernel();
  if (m_helper == NULL) {
    m_helper = new G4ErrorRunManagerHelper();
  }
  B2DEBUG(200, "Module ext: ExtManager::StartHelper() done")

}

void ExtManager::StartNavigator()
{
  // Replace G4Navigator with ExtNavigator, which is an extension of G4Navigator that
  // examines the geant4e "target" surface also when computing distance to boundary
  // (but only if geant4e is actively propagating a track!)
  if (m_navigator == NULL) {
    G4TransportationManager* transportationManager = G4TransportationManager::GetTransportationManager();
    G4Navigator* g4navi = transportationManager->GetNavigatorForTracking();
    G4VPhysicalVolume* world = g4navi->GetWorldVolume();
    G4int verbosity = g4navi->GetVerboseLevel();

    delete g4navi;
    m_navigator = new ExtNavigator;
    if (world != 0) {
      m_navigator->SetWorldVolume(world);
    }
    m_navigator->SetVerboseLevel(verbosity);
    transportationManager->SetNavigatorForTracking((G4Navigator*)m_navigator);
    transportationManager->GetPropagatorInField()->GetIntersectionLocator()->SetNavigatorFor((G4Navigator*)m_navigator);
    G4EventManager::GetEventManager()->GetTrackingManager()->GetSteppingManager()->SetNavigator((G4Navigator*)m_navigator);
  }
  B2INFO("Module ext: ExtManager::StartNavigator(): initial state is " << PrintExtState())
}

void ExtManager::InitGeant4e()
{
  B2INFO("Module ext: ExtManager::InitGeant4e(): At entry, Geant4e state "
         << PrintExtState() << " Geant4 state " << PrintG4State())
  G4ApplicationState currentState = G4StateManager::GetStateManager()->GetCurrentState();
  if (G4ErrorPropagatorData::GetErrorPropagatorData()->GetState() == G4ErrorState_PreInit) {
    if ((currentState == G4State_PreInit) || (currentState == G4State_Idle)) {
      m_helper->InitializeGeometry();
      m_helper->InitializePhysics();
    }
    B2DEBUG(200, "Module ext: ExtManager::InitGeant4e(): Geant4 state before RunInitialization() "
            << G4StateManager::GetStateManager()->GetCurrentState())
    m_helper->RunInitialization();
    B2DEBUG(200, "Module ext: ExtManager::InitGeant4e(): Geant4 state after RunInitialization() "
            << G4StateManager::GetStateManager()->GetCurrentState())
    if (!m_propagator) m_propagator = new G4ErrorPropagator();    // currently the only propagator possible
    InitTrackPropagation();
  } else {
    B2DEBUG(200, "Module ext: ExtManager::InitGeant4e(): Illegal Geant4e state  " << PrintExtState())
  }
  G4ErrorPropagatorData::GetErrorPropagatorData()->SetState(G4ErrorState_Init);
  B2INFO("Module ext: ExtManager::InitGeant4e(): At exit, Geant4e state "
         << PrintExtState() << " Geant4 state " << PrintG4State())
}

void ExtManager::InitTrackPropagation()
{
  m_propagator->SetStepN(0);
  G4ErrorPropagatorData::GetErrorPropagatorData()->SetState(G4ErrorState_Propagating);
}

G4bool ExtManager::InitFieldForBackwards()
{
  B2FATAL("Module ext: ExtManager::InitFieldForBackwards() should never be called")
  return false;
}

G4int ExtManager::Propagate(G4ErrorTrajState* currentTS, const G4ErrorTarget* target, G4ErrorMode mode)
{
  G4ErrorPropagatorData::GetErrorPropagatorData()->SetMode(mode);
  if (!m_propagator) m_propagator = new G4ErrorPropagator();    // currently the only propagator possible
  SetSteppingManagerVerboseLevel();
  InitTrackPropagation();
  G4int ierr = m_propagator->Propagate(currentTS, target, mode);
  EventTermination();
  return ierr;
}

G4int ExtManager::PropagateOneStep(G4ErrorTrajState* currentTS, G4ErrorMode mode)
{
  G4ErrorPropagatorData::GetErrorPropagatorData()->SetMode(mode);
  if (!m_propagator) m_propagator = new G4ErrorPropagator();    // currently the only propagator possible
  SetSteppingManagerVerboseLevel();
  return m_propagator->PropagateOneStep(currentTS);
}

void ExtManager::SetUserInitialization(G4VUserDetectorConstruction* userInit)
{
  m_helper->SetUserInitialization(userInit);
}

void ExtManager::SetUserInitialization(G4VPhysicalVolume* userInit)
{
  m_helper->SetUserInitialization(userInit);
}

void ExtManager::SetUserInitialization(G4VUserPhysicsList* userInit)
{
  m_helper->SetUserInitialization(userInit);
}

void ExtManager::SetUserAction(G4UserTrackingAction* userAction)
{
  G4EventManager::GetEventManager()->SetUserAction(userAction);
}

void ExtManager::SetUserAction(G4UserSteppingAction* userAction)
{
  G4EventManager::GetEventManager()->SetUserAction(userAction);
}

void ExtManager::SetSteppingManagerVerboseLevel()
{
  G4TrackingManager* trkmgr = G4EventManager::GetEventManager()->GetTrackingManager();
  trkmgr->GetSteppingManager()->SetVerboseLevel(trkmgr->GetVerboseLevel());
}

void ExtManager::EventTermination()
{
  G4ErrorPropagatorData::GetErrorPropagatorData()->SetState(G4ErrorState_Init);
}

void ExtManager::RunTermination()
{
  G4ErrorPropagatorData::GetErrorPropagatorData()->SetState(G4ErrorState_PreInit);
  m_helper->RunTermination();
}

G4String ExtManager::PrintExtState()
{
  return PrintExtState(G4ErrorPropagatorData::GetErrorPropagatorData()->GetState());
}

G4String ExtManager::PrintExtState(G4ErrorState state)
{
  G4String nam = "";
  switch (state) {
    case G4ErrorState_PreInit:
      nam = "G4ErrorState_PreInit";
      break;
    case G4ErrorState_Init:
      nam = "G4ErrorState_Init";
      break;
    case G4ErrorState_Propagating:
      nam = "G4ErrorState_Propagating";
      break;
    case G4ErrorState_TargetCloserThanBoundary:
      nam = "G4ErrorState_TargetCloserThanBoundary";
      break;
    case G4ErrorState_StoppedAtTarget:
      nam = "G4ErrorState_StoppedAtTarget";
      break;
  }
  return nam;
}

G4String ExtManager::PrintG4State()
{
  return PrintG4State(G4StateManager::GetStateManager()->GetCurrentState());
}

G4String ExtManager::PrintG4State(G4ApplicationState state)
{
  G4String nam = "";
  switch (state) {
    case G4State_PreInit:
      nam = "G4State_PreInit";
      break;
    case G4State_Init:
      nam = "G4State_Init";
      break;
    case G4State_Idle:
      nam = "G4State_Idle";
      break;
    case G4State_GeomClosed:
      nam = "G4State_GeomClosed";
      break;
    case G4State_EventProc:
      nam = "G4State_EventProc";
      break;
    case G4State_Quit:
      nam = "G4State_Quit";
      break;
    case G4State_Abort:
      nam = "G4State_Abort";
      break;
  }
  return nam;
}
